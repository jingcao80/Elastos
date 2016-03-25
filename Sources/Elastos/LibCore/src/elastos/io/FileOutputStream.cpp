
#include "FileOutputStream.h"
#include "CFile.h"
#include "IoUtils.h"
#include "NioUtils.h"
#include "OsConstants.h"
#include "CLibcore.h"
#include "CIoBridge.h"
#include "CFileDescriptor.h"
#include "AutoLock.h"

using Elastos::Droid::System::OsConstants;
using Elastos::IO::NioUtils;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;
using Libcore::IO::IOs;
using Libcore::IO::IIoBridge;
using Libcore::IO::CIoBridge;
using Libcore::IO::IoUtils;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(FileOutputStream::_Closable, Object, ICloseable)

ECode FileOutputStream::_Closable::Close()
{
    return mHost->Close();
}


CAR_INTERFACE_IMPL(FileOutputStream, OutputStream, IFileOutputStream)

FileOutputStream::FileOutputStream()
{}

FileOutputStream::~FileOutputStream()
{
    // try {
    CloseInner();
    // } finally {
    //     try {
    //         super.finalize();
    //     } catch (Throwable t) {
    //         // for consistency with the RI, we must override Object.finalize() to
    //         // remove the 'throws Throwable' clause.
    //         throw new AssertionError(t);
    //     }
    // }
}

ECode FileOutputStream::constructor(
    /* [in] */ IFile* file)
{
    return constructor(file, FALSE);
}

ECode FileOutputStream::constructor(
    /* [in] */ IFile* file,
    /* [in] */ Boolean append)
{
    if (file == NULL) {
        // throw new NullPointerException("file == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    mMode = OsConstants::_O_WRONLY
        | OsConstants::_O_CREAT
        | (append ? OsConstants::_O_APPEND : OsConstants::_O_TRUNC);
    AutoPtr<CIoBridge> ioBridge;
    CIoBridge::AcquireSingletonByFriend((CIoBridge**)&ioBridge);
    String path;
    file->GetPath(&path);
    FAIL_RETURN(ioBridge->Open(path, mMode, (IFileDescriptor**)&mFd))
    mShouldClose = TRUE;
    return NOERROR;
}

ECode FileOutputStream::constructor(
    /* [in] */ IFileDescriptor* fd)
{
    if (fd == NULL) {
//        throw new NullPointerException("fd == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mFd = fd;
    mShouldClose = FALSE;
    mMode = OsConstants::_O_WRONLY;
    mChannel = NioUtils::NewFileChannel((ICloseable*)new _Closable(this), fd, mMode);
    return NOERROR;
}

ECode FileOutputStream::constructor(
    /* [in] */ const String& fileName)
{
    return constructor(fileName, FALSE);
}

ECode FileOutputStream::constructor(
    /* [in] */ const String& fileName,
    /* [in] */ Boolean append)
{
    AutoPtr<CFile> file;
    CFile::NewByFriend(fileName, (CFile**)&file);
    return constructor((IFile*)file.Get(), append);
}

ECode FileOutputStream::Close()
{
    return CloseInner();
}

ECode FileOutputStream::GetChannel(
    /* [out] */ IFileChannel** channel)
{
    VALIDATE_NOT_NULL(channel)

    AutoLock lock(this);

    if (mChannel == NULL) {
        mChannel = NioUtils::NewFileChannel(this, mFd, mMode);
    }
    *channel = mChannel;
    REFCOUNT_ADD(*channel);
    return NOERROR;
}

ECode FileOutputStream::GetFD(
    /* [out] */ IFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd)

    *fd = mFd;
    REFCOUNT_ADD(*fd);
    return NOERROR;
}

ECode FileOutputStream::Write(
    /* [in] */ Int32 oneByte)
{
    AutoPtr< ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(1);
    (*bytes)[0] = (Byte)oneByte;
    return Write(bytes, 0, 1);
}

ECode FileOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount)
{
    AutoPtr<IIoBridge> ioBridge;
    CIoBridge::AcquireSingleton((IIoBridge**)&ioBridge);
    return ioBridge->Write(mFd, buffer, byteOffset, byteCount);
}

ECode FileOutputStream::CloseInner()
{
    AutoLock lock(this);

    if (mShouldClose) {
        AutoPtr<IIoBridge> ioBridge;
        CIoBridge::AcquireSingleton((IIoBridge**)&ioBridge);
        return ioBridge->CloseAndSignalBlockedThreads(mFd);
    }
    else {
        // An owned fd has been invalidated by IoUtils.close, but
        // we need to explicitly stop using an unowned fd (http://b/4361076).
        AutoPtr<CFileDescriptor> cfd;
        CFileDescriptor::NewByFriend((CFileDescriptor**)&cfd);
        mFd = (IFileDescriptor*)cfd.Get();
    }
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
