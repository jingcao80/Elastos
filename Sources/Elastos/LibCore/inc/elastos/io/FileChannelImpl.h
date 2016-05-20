#ifndef __ELASTOS_IO_FILECHANNELIMPL_H__
#define __ELASTOS_IO_FILECHANNELIMPL_H__

#include "FileLock.h"
#include "FileChannel.h"
#include "IoVec.h"

using Elastos::Core::IComparator;
using Elastos::IO::Channels::FileChannel;
using Elastos::IO::Channels::IFileChannel;
using Elastos::IO::Channels::IFileLock;
using Elastos::IO::Channels::FileLock;
using Elastos::IO::Channels::IReadableByteChannel;
using Elastos::IO::Channels::IWritableByteChannel;
using Elastos::IO::Channels::FileChannelMapMode;
using Elastos::Utility::ISortedSet;

namespace Elastos {
namespace IO {

class FileChannelImpl
    : public FileChannel
    , public IFileChannelImpl
{
private:
    class FileLockImpl
        : public FileLock
    {
    public:
        FileLockImpl(
            /* [in] */ IFileChannel* channel,
            /* [in] */ Int64 position,
            /* [in] */ Int64 size,
            /* [in] */ Boolean shared);

        CARAPI IsValid(
            /* [out] */ Boolean* ret);

        CARAPI ReleaseLock();

    private:
        Boolean mIsReleased;
    };

    class FileLockComparator
        : public Object
        , public IComparator
    {
    public:
        FileLockComparator();

        ~FileLockComparator();

        CAR_INTERFACE_DECL()

        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out] */ Int32* result);
    };

public:
    CAR_INTERFACE_DECL()

    FileChannelImpl(
        /* [in] */ ICloseable* stream,
        /* [in] */ IFileDescriptor* desc,
        /* [in] */ Int32 mode);

    CARAPI IsOpen(
        /* [out] */ Boolean* rst);

    CARAPI Close();

    CARAPI Lock(
        /* [out] */ IFileLock** lock);

    CARAPI Read(
        /* [in] */ ArrayOf<IByteBuffer*>* buffers,
        /* [out] */ Int64* number);

    CARAPI TryLock(
        /* [out] */ IFileLock** lock);

    virtual CARAPI Lock(
        /* [in] */ Int64 position,
        /* [in] */ Int64 size,
        /* [in] */ Boolean shared,
        /* [out] */ IFileLock** lock);

    CARAPI TryLock(
        /* [in] */ Int64 position,
        /* [in] */ Int64 size,
        /* [in] */ Boolean shared,
        /* [out] */ IFileLock** lock);

    CARAPI Force(
        /* [in] */ Boolean metadata);

    CARAPI Map(
        /* [in] */ FileChannelMapMode mode,
        /* [in] */ Int64 position,
        /* [in] */ Int64 size,
        /* [out] */ IMappedByteBuffer** buffer);

    CARAPI Position(
        /* [out] */ Int64* position);

    CARAPI Position(
        /* [in] */ Int64 offset);

    CARAPI Read(
        /* [in] */ IByteBuffer* buffer,
        /* [out] */ Int32* number);

    CARAPI Read(
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Int64 position,
        /* [out] */ Int32* number);

    CARAPI Read(
        /* [in] */ ArrayOf<IByteBuffer*>* buffers,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int64* number);

    CARAPI TransferFrom(
        /* [in] */ IReadableByteChannel* src,
        /* [in] */ Int64 position,
        /* [in] */ Int64 count,
        /* [out] */ Int64* number);

    CARAPI TransferTo(
        /* [in] */ Int64 position,
        /* [in] */ Int64 count,
        /* [in] */ IWritableByteChannel* target,
        /* [out] */ Int64* number);

    CARAPI Truncate(
        /* [in] */ Int64 size,
        /* [out]*/ IFileChannel** channel);

    CARAPI GetSize(
        /* [out] */ Int64* size);

    CARAPI Write(
        /* [in] */ IByteBuffer* buffer,
        /* [out] */ Int32* number);

    CARAPI Write(
        /* [in] */ ArrayOf<IByteBuffer*>* buffers,
        /* [out] */ Int64* number);

    CARAPI Write(
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Int64 position,
        /* [out] */ Int32* number);

    CARAPI Write(
        /* [in] */ ArrayOf<IByteBuffer*>* buffers,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int64* number);

    CARAPI ReleaseLock(
        /* [in] */ IFileLock* lock);

    CARAPI GetFD(
        /* [out] */ IFileDescriptor** descriptor);

    static CARAPI CalculateTotalRemaining(
        /* [in] */ ArrayOf<IByteBuffer*> * buffers,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [in] */ Boolean copyingIn,
        /* [out] */ Int32* ret);

protected:
    CARAPI ImplCloseChannel();

private:
    CARAPI CheckOpen();

    CARAPI CheckReadable();

    CARAPI CheckWritable();

    CARAPI BasicLock(
        /* [in] */ Int64 position,
        /* [in] */ Int64 size,
        /* [in] */ Boolean shared,
        /* [in] */ Boolean wait,
        /* [out] */ IFileLock** outlock);

    CARAPI_(Int64) TranslateLockLength(
        /* [in] */ Int64 byteCount);

    CARAPI ReadImpl(
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Int64 position,
        /* [out] */ Int32* ret);

    CARAPI TransferIoVec(
        /* [in] */ IoVec* ioVec,
        /* [out] */ Int32* ret);

    CARAPI WriteImpl(
        /* [in] */ IByteBuffer* buffer,
        /* [in] */ Int64 position,
        /* [out] */ Int32* ret);

    CARAPI AddLock(
        /* [in] */ IFileLock* lock);

    void RemoveLock(
        /* [in] */ IFileLock* lock);

private:
    AutoPtr<IComparator> sCompator;
    AutoPtr<ICloseable> mStream;
    AutoPtr<IFileDescriptor> mFd;
    Int32 mMode;
    AutoPtr<ISortedSet> mLocks;
    static AutoPtr<IComparator> LOCK_COMPARATOR;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_BUFFER_H__
