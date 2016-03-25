
#include "FileInputStream.h"
#include "CFile.h"
#include "IoUtils.h"
#include "OsConstants.h"
#include "CLibcore.h"
#include "CIoBridge.h"
#include "NioUtils.h"
#include "CFileDescriptor.h"
#include "AutoLock.h"
#include "CStreams.h"
#include "CCloseGuard.h"

using Elastos::Core::CCloseGuard;
using Elastos::Droid::System::OsConstants;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;
using Libcore::IO::IOs;
using Libcore::IO::IIoBridge;
using Libcore::IO::CIoBridge;
using Libcore::IO::IStreams;
using Libcore::IO::CStreams;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(FileInputStream, InputStream, IFileInputStream)

FileInputStream::FileInputStream()
    : mShouldClose(FALSE)
{
    CCloseGuard::New((ICloseGuard**)&mGuard);
}

FileInputStream::~FileInputStream()
{
    // try {
    if (mGuard != NULL) {
        mGuard->WarnIfOpen();
    }
    Close();
}

ECode FileInputStream::constructor(
    /* [in] */ IFile* file)
{
    if (file == NULL) {
//        throw new NullPointerException("file == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    CFileDescriptor::New((IFileDescriptor**)&mFd);
    String path;
    file->GetPath(&path);
    AutoPtr<IIoBridge> ioBridge;
    CIoBridge::AcquireSingleton((IIoBridge**)&ioBridge);
    AutoPtr<IFileDescriptor> fd;
    FAIL_RETURN(ioBridge->Open(path, OsConstants::_O_RDONLY, (IFileDescriptor**)&fd));
    Int32 ifd;
    fd->GetDescriptor(&ifd);
    mFd->SetDescriptor(ifd);
    mShouldClose = TRUE;
    mGuard->Open(String("close"));
    return NOERROR;
}

ECode FileInputStream::constructor(
    /* [in] */ IFileDescriptor* fd)
{
    if (fd == NULL) {
//        throw new NullPointerException("fd == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mFd = fd;
    mShouldClose = FALSE;
    return NOERROR;
}

ECode FileInputStream::constructor(
    /* [in] */ const String& fileName)
{
    AutoPtr<IFile> file;
    CFile::New(fileName, (IFile**)&file);
    return constructor(file);
}

ECode FileInputStream::Available(
    /* [out] */ Int32* avail)
{
    VALIDATE_NOT_NULL(avail)

    AutoPtr<IIoBridge> ioBridge;
    CIoBridge::AcquireSingleton((IIoBridge**)&ioBridge);
    ECode ec = ioBridge->Available(mFd, avail);
    if (FAILED(ec)) {
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode FileInputStream::Close()
{
    mGuard->Close();
    synchronized (this) {
        if (mChannel != NULL) {
            ICloseable::Probe(mChannel)->Close();
        }
        if (mShouldClose) {
            AutoPtr<IIoBridge> ioBridge;
            CIoBridge::AcquireSingleton((IIoBridge**)&ioBridge);
            ioBridge->CloseAndSignalBlockedThreads(mFd);
        }
        else {
            // An owned fd has been invalidated by IoUtils.close, but
            // we need to explicitly stop using an unowned fd (http://b/4361076).
            mFd = NULL;
            CFileDescriptor::New((IFileDescriptor**)&mFd);
        }
    }
    return NOERROR;
}

ECode FileInputStream::GetChannel(
    /* [out] */ IFileChannel** channel)
{
    VALIDATE_NOT_NULL(channel)

    AutoLock lock(this);

    if (mChannel == NULL) {
        mChannel = NioUtils::NewFileChannel(this, mFd, OsConstants::_O_RDONLY);
    }
    *channel = mChannel;
    REFCOUNT_ADD(*channel)
    return NOERROR;
}

ECode FileInputStream::GetFD(
    /* [out] */ IFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd)

    *fd = (IFileDescriptor*)mFd.Get();
    REFCOUNT_ADD(*fd);
    return NOERROR;
}

ECode FileInputStream::Read(
    /* [out] */ Int32* value)
{
    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    return streams->ReadSingleByte(this, value);
}

ECode FileInputStream::Read(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = -1;
    VALIDATE_NOT_NULL(buffer);

    AutoPtr<IIoBridge> ioBridge;
    CIoBridge::AcquireSingleton((IIoBridge**)&ioBridge);
    return ioBridge->Read(mFd, buffer, byteOffset, byteCount, number);
}

ECode FileInputStream::Skip(
    /* [in] */ Int64 byteCount,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;

    if (byteCount < 0) {
//      throw new IOException("byteCount < 0: " + byteCount);
        return E_IO_EXCEPTION;
    }
    // try {
        // Try lseek(2). That returns the new offset, but we'll throw an
        // exception if it couldn't perform exactly the seek we asked for.
        AutoPtr<ILibcore> libcore;
        CLibcore::AcquireSingleton((ILibcore**)&libcore);
        AutoPtr<IOs> os;
        libcore->GetOs((IOs**)&os);
        os->Lseek(mFd, byteCount, OsConstants::_SEEK_CUR, number);
        *number = byteCount;
        return NOERROR;
    // } catch (ErrnoException errnoException) {
    //     if (errnoException.errno == ESPIPE) {
    //         // You can't seek on a pipe, so fall back to the superclass' implementation.
    //         return super.skip(byteCount);
    //     }
    //     throw errnoException.rethrowAsIOException();
    // }
}

} // namespace IO
} // namespace Elastos
