#include "FileChannelImpl.h"
#include "Math.h"
#include "MemoryBlock.h"
#include "NioUtils.h"
#include "CLibcore.h"
#include "OsConstants.h"
#include "CStructFlock.h"
#include "DirectByteBuffer.h"
#include "Arrays.h"
#include "CTreeSet.h"

using Libcore::IO::IOs;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;
using Elastos::IO::NioUtils;
using Elastos::Core::EIID_IComparator;
using Elastos::Droid::System::OsConstants;
using Elastos::Droid::System::IStructStat;
using Elastos::Droid::System::IStructFlock;
using Elastos::Droid::System::CStructFlock;
using Elastos::IO::MemoryBlock;
using Elastos::IO::Channels::EIID_IFileLock;
using Elastos::IO::Channels::IChannel;
using Elastos::IO::Channels::EIID_IFileChannel;
using Elastos::IO::Channels::FileChannelMapMode_READ_ONLY;
using Elastos::Utility::Arrays;
using Elastos::Utility::ITreeSet;
using Elastos::Utility::CTreeSet;

namespace Elastos {
namespace IO {

//==========================================================
//       FileChannelImpl::FileLockImpl
//==========================================================
FileChannelImpl::FileLockImpl::FileLockImpl(
    /* [in] */ IFileChannel* channel,
    /* [in] */ Int64 position,
    /* [in] */ Int64 size,
    /* [in] */ Boolean shared)
    : FileLock(channel, position, size, shared)
    , mIsReleased(FALSE)
{
}

ECode FileChannelImpl::FileLockImpl::IsValid(
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = FALSE;

    AutoPtr<IFileChannel> channel;
    ECode ec = Channel((IFileChannel**)&channel);
    FAIL_RETURN(ec)

    Boolean isOpen;
    ec = IChannel::Probe(channel)->IsOpen(&isOpen);
    FAIL_RETURN(ec)

    *ret = mIsReleased && isOpen;
    return NOERROR;
}

ECode FileChannelImpl::FileLockImpl::ReleaseLock()
{
    AutoPtr<IFileChannel> channel;
    ECode ec = Channel((IFileChannel**)&channel);
    FAIL_RETURN(ec)

    Boolean isOpen;
    ec = IChannel::Probe(channel)->IsOpen(&isOpen);
    FAIL_RETURN(ec)

    if (!isOpen) {
        return E_CLOSED_CHANNEL_EXCEPTION;
    }

    if (!mIsReleased) {
        IFileChannelImpl* impl = IFileChannelImpl::Probe(channel);
        impl->ReleaseLock(this);
        mIsReleased = TRUE;
    }
    return NOERROR;
}

//==========================================================
//       FileChannelImpl::FileLockComparator
//==========================================================

FileChannelImpl::FileLockComparator::FileLockComparator()
{
}

FileChannelImpl::FileLockComparator::~FileLockComparator()
{
}

CAR_INTERFACE_IMPL(FileChannelImpl::FileLockComparator, Object, IComparator)

ECode FileChannelImpl::FileLockComparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int64 position1 = 0;
    IFileLock::Probe(lhs)->GetPosition(&position1);
    Int64 position2 = 0;
    IFileLock::Probe(rhs)->GetPosition(&position2);
    *result = position1 > position2 ? 1 : (position1 < position2 ? -1 : 0);
    return NOERROR;
}

//==========================================================
//       FileChannelImpl
//==========================================================

AutoPtr<IComparator> FileChannelImpl::LOCK_COMPARATOR = (IComparator*) new FileChannelImpl::FileLockComparator();

CAR_INTERFACE_IMPL(FileChannelImpl, Object, IFileChannelImpl)

FileChannelImpl::FileChannelImpl(
    /* [in] */ ICloseable* stream,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 mode)
    : mStream(stream)
    , mFd(fd)
    , mMode(mode)
{
    assert(NULL != stream);
    assert(NULL != fd);
    AutoPtr<ITreeSet> res;
    CTreeSet::New(LOCK_COMPARATOR, (ITreeSet**)&res);
    mLocks = ISortedSet::Probe(res);
}

ECode FileChannelImpl::IsOpen(
    /* [out] */ Boolean* rst)
{
    return FileChannel::IsOpen(rst);
}

ECode FileChannelImpl::Close()
{
    return FileChannel::Close();
}

ECode FileChannelImpl::Lock(
    /* [out] */ IFileLock** lock)
{
    return FileChannel::Lock(lock);
}

ECode FileChannelImpl::TryLock(
    /* [out] */ IFileLock** lock)
{
    return FileChannel::TryLock(lock);
}

ECode FileChannelImpl::Read(
    /* [in] */ ArrayOf<IByteBuffer*>* buffers,
    /* [out] */ Int64* number)
{
    return FileChannel::Read(buffers , number);
}

ECode FileChannelImpl::Write(
    /* [in] */ ArrayOf<IByteBuffer*>* buffers,
    /* [out] */ Int64* number)
{
    return FileChannel::Write(buffers , number);
}

ECode FileChannelImpl::CheckOpen()
{
    Boolean bOpenFlag;
    FileChannel::IsOpen(&bOpenFlag);
    if (!bOpenFlag)
    {
        return E_CLOSED_CHANNEL_EXCEPTION;
    }
    return NOERROR;
}

ECode FileChannelImpl::CheckReadable()
{
    Int32 accMode = OsConstants::_O_ACCMODE;
    Int32 wrOnly = OsConstants::_O_WRONLY;

    if ((mMode & accMode) == wrOnly)
    {
        return E_NON_READABLE_CHANNEL_EXCEPTION;
    }
    return NOERROR;
}

ECode FileChannelImpl::CheckWritable()
{
    Int32 accMode = OsConstants::_O_ACCMODE;
    Int32 rdOnly = OsConstants::_O_RDONLY;
    if ((mMode & accMode) == rdOnly) {
        return E_NON_WRITABLE_CHANNEL_EXCEPTION;
    }
    return NOERROR;
}

ECode FileChannelImpl::ImplCloseChannel()
{
    return mStream->Close();
}

ECode FileChannelImpl::BasicLock(
    /* [in] */ Int64 position,
    /* [in] */ Int64 size,
    /* [in] */ Boolean shared,
    /* [in] */ Boolean wait,
    /* [out] */ IFileLock** outlock)
{
    VALIDATE_NOT_NULL(outlock)

    Int32 ACCMOD = OsConstants::_O_ACCMODE;
    Int32 RDONLY = OsConstants::_O_RDONLY;
    Int32 WRONLY = OsConstants::_O_WRONLY;
    Int32 accessMode = mMode & ACCMOD;
    if (RDONLY == accessMode)
    {
        if (!shared)
        {
            return E_NON_WRITABLE_CHANNEL_EXCEPTION;
        }

    }
    else if (WRONLY == accessMode)
    {
        if (shared)
        {
            return E_NON_WRITABLE_CHANNEL_EXCEPTION;
        }
    }

    if(position < 0 || size < 0)
    {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IFileLock> pendingLock = new FileLockImpl(this, position, size, shared);
    AddLock(pendingLock);

    AutoPtr<IStructFlock> flock;
    CStructFlock::New((IStructFlock**)&flock);
    flock->SetType((Int16) (shared ? OsConstants::_F_RDLCK : OsConstants::_F_WRLCK));
    flock->SetWhence((Int16) SEEK_SET);
    flock->SetStart(position);
    flock->SetLen(TranslateLockLength(size));

    Boolean success = FALSE;
    Int32 outvalue = 0;
    // try {
    FAIL_RETURN(CLibcore::sOs->FcntlFlock(mFd, wait ? OsConstants::_F_SETLKW64 : OsConstants::_F_SETLK64, flock, &outvalue));
    success = outvalue != -1;
    // } catch (ErrnoException errnoException) {
    //     throw errnoException.rethrowAsIOException();
    // } finally {
    if (!success) {
        RemoveLock(pendingLock);
    }
    // }
    *outlock = success ? pendingLock : NULL;
    REFCOUNT_ADD(*outlock)
    return NOERROR;
}

Int64 FileChannelImpl::TranslateLockLength(
    /* [in] */ Int64 byteCount)
{
    // FileChannel uses Long.MAX_VALUE to mean "lock the whole file" where POSIX uses 0.
    return (byteCount == Elastos::Core::Math::INT64_MAX_VALUE) ? 0 : byteCount;
}

ECode FileChannelImpl::Lock(
    /* [in] */ Int64 pos,
    /* [in] */ Int64 size,
    /* [in] */ Boolean shared,
    /* [out] */ IFileLock** lock)
{
    VALIDATE_NOT_NULL(lock)

    FAIL_RETURN(CheckOpen());
    AutoPtr<IFileLock> resultLock = NULL;
    {
        Boolean completed = FALSE;
        // try {
            Begin();
            BasicLock(pos, size, shared, TRUE, (IFileLock**)&resultLock);
            completed = TRUE;
        // } finally {
            // try {
                End(completed);
            // } catch (ClosedByInterruptException e) {
            //     throw new FileLockInterruptionException();
            // }
        // }
    }
    *lock = resultLock;
    REFCOUNT_ADD(*lock)
    return NOERROR;
}

ECode FileChannelImpl::TryLock(
    /* [in] */ Int64 position,
    /* [in] */ Int64 size,
    /* [in] */ Boolean shared,
    /* [out] */ IFileLock** lock)
{
    VALIDATE_NOT_NULL(lock)

    FAIL_RETURN(CheckOpen());
    return BasicLock(position, size, shared, FALSE, lock);
}

ECode FileChannelImpl::ReleaseLock(
    /* [in] */ IFileLock* lock)
{
    FAIL_RETURN(CheckOpen());

    AutoPtr<IStructFlock> flock;
    CStructFlock::New((IStructFlock**)&flock);
    flock->SetType((Int16) OsConstants::_F_UNLCK);
    flock->SetWhence((Int16) OsConstants::_SEEK_SET);
    Int64 pos;
    lock->GetPosition(&pos);
    flock->SetStart(pos);
    Int64 size;
    lock->GetSize(&size);
    flock->SetLen(TranslateLockLength(size));
    // try {
    Int32 ouvalue = 0;
    FAIL_RETURN(CLibcore::sOs->FcntlFlock(mFd, OsConstants::_F_SETLKW64, flock, &ouvalue));
    // } catch (ErrnoException errnoException) {
    //     throw errnoException.rethrowAsIOException();
    // }

    RemoveLock(lock);
    return NOERROR;
}

ECode FileChannelImpl::Force(
    /* [in] */ Boolean metadata)
{
    ECode ecRet = CheckOpen();
    if(NOERROR != ecRet)
       return ecRet;

    Int32 ACCMODE = OsConstants::_O_ACCMODE;
    Int32 RDONLY = OsConstants::_O_RDONLY;

    if( (mMode & ACCMODE) != RDONLY)
    {
        if (metadata)
        {
            ecRet = CLibcore::sOs->Fsync(mFd);
            if (NOERROR != ecRet)
            {
                return ecRet;
            }
        }
        else
        {
            ecRet = CLibcore::sOs->Fdatasync(mFd);
            if (NOERROR != ecRet)
            {
                return ecRet;
            }
        }
    }

    return NOERROR;
}

ECode FileChannelImpl::Map(
    /* [in] */ FileChannelMapMode mode,
    /* [in] */ Int64 position,
    /* [in] */ Int64 size,
    /* [out] */ IMappedByteBuffer** buffer)
{
    ECode ecRet;
    ecRet = CheckOpen();

    assert(mode >= Elastos::IO::Channels::FileChannelMapMode_READ_ONLY &&
          mode <= Elastos::IO::Channels::FileChannelMapMode_PRIVATE);

    if(position < 0 || size < 0 || size > Elastos::Core::Math::INT32_MAX_VALUE)
    {
       return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 ACCESSMODE = OsConstants::_O_ACCMODE;
    Int32 RDONLY = OsConstants::_O_RDONLY;
    Int32 WRONLY = OsConstants::_O_WRONLY;
    Int32 accessMode = (mode & ACCESSMODE);
    if(accessMode == RDONLY)
    {
       if(mode != Elastos::IO::Channels::FileChannelMapMode_READ_ONLY)
       {
           return E_NON_WRITABLE_CHANNEL_EXCEPTION;
       }
    } else if(accessMode == WRONLY) {
       return E_NON_READABLE_CHANNEL_EXCEPTION;
    }

    Int64 nSize;
    ecRet = GetSize(&nSize);

    if(position + size > nSize)
    {
        ecRet = CLibcore::sOs->Ftruncate(mFd, position + size);
        if(NOERROR != ecRet)
           return ecRet;
    }

    Int32 SC_PAGE_SIZE = OsConstants::__SC_PAGE_SIZE;
    Int64 nPageSize = 0;
    assert(NOERROR == ecRet);

    ecRet = CLibcore::sOs->Sysconf(SC_PAGE_SIZE, &nPageSize);
    assert(NOERROR == ecRet);

    Int64 alignment = position - position % nPageSize;
    Int32 offset = (Int32)(position - alignment);

    AutoPtr<MemoryBlock> block;
    MemoryBlock::Mmap(mFd.Get(), alignment, size + offset, mode, (MemoryBlock**)&block);

    AutoPtr<DirectByteBuffer> db = new DirectByteBuffer();
    FAIL_RETURN(db->constructor(block, (Int32) size, offset, (mode == FileChannelMapMode_READ_ONLY), mode))
    AutoPtr<IMappedByteBuffer> mbb = IMappedByteBuffer::Probe(db);
    *buffer = mbb;
    REFCOUNT_ADD(*buffer);

    return NOERROR;
}

ECode FileChannelImpl::Position(
    /* [out] */ Int64* position)
{
    FAIL_RETURN(CheckOpen());

    Int32 seek_cur = OsConstants::_SEEK_CUR;
    return CLibcore::sOs->Lseek(mFd, 0L, seek_cur, position);
}

ECode FileChannelImpl::Position(
    /* [in] */ Int64 newPosition)
{
    FAIL_RETURN(CheckOpen());
    if (newPosition < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 seek_cur = OsConstants::_SEEK_CUR;
    Int64 result = 0;
    return CLibcore::sOs->Lseek(mFd, newPosition, seek_cur, &result);
}

ECode FileChannelImpl::Read(
   /* [in] */ IByteBuffer* buffer,
   /* [out] */ Int32* number)
{
    return ReadImpl(buffer, -1, number);
}

ECode FileChannelImpl::Read(
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int64 position,
    /* [out] */ Int32* number)
{
    if (position < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return ReadImpl(buffer, position, number);
}

ECode FileChannelImpl::ReadImpl(
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int64 position,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)

    FAIL_RETURN(((Buffer*)IBuffer::Probe(buffer))->CheckWritable());
    FAIL_RETURN(CheckOpen());
    FAIL_RETURN(CheckReadable());
    Boolean isflag = FALSE;
    if (IBuffer::Probe(buffer)->HasRemaining(&isflag), !isflag) {
        *ret = 0;
        return NOERROR;
    }
    Int32 bytesRead = 0;
    Boolean completed = FALSE;
    // try {
        Begin();
        // try {
            if (position == -1) {
                FAIL_RETURN(CLibcore::sOs->Read(mFd, buffer, &bytesRead));
            }
            else {
                FAIL_RETURN(CLibcore::sOs->Pread(mFd, buffer, position, &bytesRead));
            }
            if (bytesRead == 0) {
                bytesRead = -1;
            }
        // } catch (ErrnoException errnoException) {
        //     if (errnoException.errno == EAGAIN) {
        //         // We don't throw if we try to read from an empty non-blocking pipe.
        //         bytesRead = 0;
        //     } else {
        //         throw errnoException.rethrowAsIOException();
        //     }
        // }
        completed = TRUE;
    // } finally {
        End(completed && bytesRead >= 0);
    // }
    if (bytesRead > 0) {
        Int32 pos = 0;
        IBuffer::Probe(buffer)->GetPosition(&pos);
        IBuffer::Probe(buffer)->SetPosition(pos + bytesRead);
    }
    *ret = bytesRead;
    return NOERROR;

}

ECode FileChannelImpl::TransferIoVec(
    /* [in] */ IoVec* ioVec,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)

    if (ioVec->Init(ret), *ret == 0) {
        *ret = 0;
        return NOERROR;
    }
    Int32 bytesTransferred = 0;
    Boolean completed = FALSE;
    // try {
    Begin();
    ioVec->DoTransfer(mFd, &bytesTransferred);
    completed = TRUE;
    // } finally {
    End(completed);
    // }
    ioVec->DidTransfer(bytesTransferred);
    *ret = bytesTransferred;
    return NOERROR;
}

ECode FileChannelImpl::Read(
    /* [in] */ ArrayOf<IByteBuffer*>* buffers,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number)

    if ((offset | length) < 0 || offset > buffers->GetLength()
            || buffers->GetLength() - offset < length)
    {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    ECode ecRet = CheckOpen();
    if(NOERROR != ecRet)
        return ecRet;

    ecRet = CheckReadable();
    if(NOERROR != ecRet)
        return ecRet;

    AutoPtr<IoVec> res = new IoVec(buffers, offset, length, IoVec::Direction_READV);
    Int32 outvalue = 0;
    TransferIoVec(res, &outvalue);
    *number = outvalue;
    return NOERROR;
}

ECode FileChannelImpl::GetSize(
    /* [out] */ Int64* size)
{
    VALIDATE_NOT_NULL(size)

    ECode ecRet = CheckOpen();
    if(NOERROR != ecRet)
       return ecRet;

    AutoPtr<IStructStat> stat;
    CLibcore::sOs->Fstat(mFd, (IStructStat**)&stat);
    return stat->GetSize(size);
}

ECode FileChannelImpl::TransferFrom(
    /* [in] */ IReadableByteChannel* src,
    /* [in] */ Int64 position,
    /* [in] */ Int64 count,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number)

    ECode ecRet;
    ecRet = CheckOpen();
    if(NOERROR != ecRet)
        return ecRet;

    Boolean isOpen;
    IChannel::Probe(src)->IsOpen(&isOpen);
    if(!isOpen)
        return E_CLOSED_CHANNEL_EXCEPTION;

    ecRet = CheckWritable();
    if(NOERROR != ecRet)
        return ecRet;

    if(position < 0 || count < 0 || count > Elastos::Core::Math::INT32_MAX_VALUE)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;

    Int64 nSize;
    GetSize(&nSize);
    if(position > nSize)
    {
        *number = 0;
        return NOERROR;
    }

    if(IFileChannel::Probe(src) != NULL)
    {
        IFileChannel *fileSrc = (IFileChannel*)src;
        Int64 size;
        fileSrc->GetSize(&size);
        Int64 filePosition;
        fileSrc->Position(&filePosition);
        count = Elastos::Core::Math::Min(count, size-filePosition);
        AutoPtr<IMappedByteBuffer> buffer;
        fileSrc->Map(Elastos::IO::Channels::FileChannelMapMode_READ_ONLY, filePosition, count, (IMappedByteBuffer**)&buffer);
        ecRet = fileSrc->Position(filePosition + count);
        if(NOERROR != ecRet)
            return ecRet;

        ecRet = Write(IByteBuffer::Probe(buffer), position, (Int32*)number);
        NioUtils::FreeDirectBuffer((IByteBuffer*)buffer->Probe(EIID_IByteBuffer));

        return ecRet;
    }

    AutoPtr<IByteBuffer> buffer;
    ByteBuffer::Allocate((Int32)count, (IByteBuffer**)&buffer);
    Int32 nRead;
    src->Read(buffer, &nRead);
    IBuffer::Probe(buffer)->Flip();
    return Write(buffer, position, (Int32*)number);
}

ECode FileChannelImpl::TransferTo(
    /* [in] */ Int64 position,
    /* [in] */ Int64 count,
    /* [in] */ IWritableByteChannel* target,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number)

    ECode ecRet = CheckOpen();
    if(NOERROR != ecRet)
        return ecRet;

    Boolean isOpen;
    IChannel::Probe(target)->IsOpen(&isOpen);
    if(!isOpen)
        return E_CLOSED_CHANNEL_EXCEPTION;

    ecRet = CheckReadable();
    if(NOERROR != ecRet)
        return ecRet;

    if(IFileChannelImpl::Probe(target) != NULL)
    {
        ecRet = ((FileChannelImpl*)target)->CheckWritable();
        if(NOERROR != ecRet)
            return ecRet;
    }

    if(position < 0 || count < 0)
    {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int64 nSize;
    GetSize(&nSize);
    if(count == 0 || position >= nSize)
    {
        *number = 0;
        return NOERROR;
    }

    count = Elastos::Core::Math::Min(count, nSize-position);

    // Try sendfile(2) first...
    Boolean completed = FALSE;
    if (IFileDescriptorChannel::Probe(target)) {
        AutoPtr<IFileDescriptor> outFd;
        IFileDescriptorChannel::Probe(target)->GetFD((IFileDescriptor**)&outFd);
        // try {
        Begin();
            // try {
        ECode ec = (CLibcore::sOs->Sendfile(outFd, mFd, &position, count, number));
        if (ec == NOERROR) {
            completed = TRUE;
            return NOERROR;
        }
            // } catch (ErrnoException errnoException) {
            //     // If the OS doesn't support what we asked for, we want to fall through and
            //     // try a different approach. If it does support it, but it failed, we're done.
            //     if (errnoException.errno != ENOSYS && errnoException.errno != EINVAL) {
            //         throw errnoException.rethrowAsIOException();
            //     }
            // }
        // } finally {
        End(completed);
        // }
    }
    // ...fall back to write(2).
    AutoPtr<IMappedByteBuffer> buffer;
    // try {
    FAIL_RETURN(Map(Elastos::IO::Channels::FileChannelMapMode_READ_ONLY, position, count, (IMappedByteBuffer**)&buffer));
    Int32 ouvalue = 0;
    FAIL_RETURN(target->Write(IByteBuffer::Probe(buffer), &ouvalue));
    *number = ouvalue;
    return NOERROR;
    // } finally {
    NioUtils::FreeDirectBuffer((IByteBuffer*)buffer->Probe(EIID_IByteBuffer));
    // }
    return NOERROR;
}

ECode FileChannelImpl::Truncate(
    /* [in] */ Int64 size,
    /* [out]*/ IFileChannel** channel)
{
    VALIDATE_NOT_NULL(channel)

    ECode ecRet = CheckOpen();
    if(NOERROR != ecRet)
       return ecRet;

    if(size < 0)
       return E_ILLEGAL_ARGUMENT_EXCEPTION;

    ecRet = CheckWritable();
    if(NOERROR != ecRet)
       return ecRet;

    Int64 nSize;
    ecRet = GetSize(&nSize);
    assert(NOERROR == ecRet);

    if (size < nSize) {
        ecRet = CLibcore::sOs->Ftruncate(mFd, nSize);
    }

    *channel = (IFileChannel*) this->Probe(EIID_IFileChannel);
    REFCOUNT_ADD(*channel)

    return ecRet;
}

ECode FileChannelImpl::Write(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ Int32* number)
{
    return WriteImpl(buffer, -1, number);
}

ECode FileChannelImpl::Write(
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int64 position,
    /* [out] */ Int32* number)
{
    if(position < 0)
    return E_ILLEGAL_ARGUMENT_EXCEPTION;

    return WriteImpl(buffer, position, number);
}

ECode FileChannelImpl::WriteImpl(
    /* [in] */ IByteBuffer* buffer,
    /* [in] */ Int64 position,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)

    ECode ecRet = CheckOpen();
    if(NOERROR != ecRet)
        return ecRet;

    ecRet = CheckWritable();
    if(NOERROR != ecRet)
        return ecRet;

    if(NULL == buffer)
        return E_NULL_POINTER_EXCEPTION;

    Boolean hasRemaining;
    IBuffer::Probe(buffer)->HasRemaining(&hasRemaining);
    if(!hasRemaining)
    {
        *ret = 0;
        return NOERROR;
    }

    Int32 bytesWritten = 0;
    Boolean completed = FALSE;

    Begin();
    {
        if (-1 == position) {
            ecRet = CLibcore::sOs->Write(mFd, buffer, &bytesWritten);
        }
        else {
            ecRet = CLibcore::sOs->Pwrite(mFd, buffer, position, &bytesWritten);
        }

        completed = TRUE;
    }
    End(completed);

    if(NOERROR != ecRet)
        return ecRet;

    if(bytesWritten > 0)
    {
        Int32 pos;
        IBuffer::Probe(buffer)->GetPosition(&pos);
        IBuffer::Probe(buffer)->SetPosition(pos + bytesWritten);
    }

    *ret = bytesWritten;
    return NOERROR;
}

ECode FileChannelImpl::Write(
    /* [in] */ ArrayOf<IByteBuffer*>* buffers,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number)

    FAIL_RETURN(Arrays::CheckOffsetAndCount(buffers->GetLength(), offset, length));
    FAIL_RETURN(CheckOpen());
    FAIL_RETURN(CheckWritable());
    AutoPtr<IoVec> res = new IoVec(buffers, offset, length, IoVec::Direction_WRITEV);
    Int32 ouvalue;
    TransferIoVec(res, &ouvalue);
    *number = ouvalue;
    return NOERROR;
}

ECode FileChannelImpl::CalculateTotalRemaining(
    /* [in] */ ArrayOf<IByteBuffer*> * buffers,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ Boolean copyingIn,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = 0;

    Int32 count = 0, i, remaining;
    Int32 LEN = offset + length;
    for (i = offset; i < LEN; ++i) {
        IBuffer::Probe((*buffers)[i])->GetRemaining(&remaining);
        count += remaining;

        if (copyingIn) {
            FAIL_RETURN(((Buffer*)IBuffer::Probe((*buffers)[i]))->CheckWritable())
        }
    }

    *ret = count;
    return NOERROR;
}

ECode FileChannelImpl::GetFD(
    /* [out] */ IFileDescriptor** descriptor)
{
    VALIDATE_NOT_NULL(descriptor)

    (*descriptor) = mFd;
    REFCOUNT_ADD(*descriptor)
    return NOERROR;
}

ECode FileChannelImpl::AddLock(
    /* [in] */ IFileLock* lock)
{
    Int64 pos,size;
    lock->GetPosition(&pos);
    lock->GetSize(&size);
    Int64 lockEnd = pos + size;
    AutoPtr< ArrayOf<IInterface*> > outarr;
    mLocks->ToArray((ArrayOf<IInterface*>**)&outarr);
    Boolean isflag = FALSE;
    for (Int32 i = 0; i < outarr->GetLength(); i++) {
        AutoPtr<IFileLock> existingLock = IFileLock::Probe((*outarr)[i]);
        existingLock->GetPosition(&pos);
        if (pos > lockEnd) {
            // This, and all remaining locks, start beyond our end (so
            // cannot overlap).
            break;
        }
        if (existingLock->Overlaps(pos, size, &isflag), isflag) {
            // throw new OverlappingFileLockException();
            return E_OVERLAPPING_FILE_LOCK_EXCEPTION;
        }
    }
    return mLocks->Add(lock, &isflag);
}

void FileChannelImpl::RemoveLock(
    /* [in] */ IFileLock* lock)
{
    Boolean isflag = FALSE;
    mLocks->Remove(lock, &isflag);
}

} // namespace IO
} // namespace Elastos
