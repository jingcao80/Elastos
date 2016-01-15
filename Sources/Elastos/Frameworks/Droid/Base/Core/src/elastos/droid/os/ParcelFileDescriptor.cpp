
#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.CoreLibrary.Net.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/ParcelFileDescriptor.h"
#include "elastos/droid/os/CParcelFileDescriptor.h"
#include "elastos/droid/os/CParcel.h"

#include <elastos/core/Math.h>
#include <elastos/droid/system/OsConstants.h>
#include <elastos/droid/system/Os.h>
#include <elastos/utility/logging/Logger.h>

#include <sys/stat.h>
#include <unistd.h>

using Libcore::IO::IIoUtils;
using Libcore::IO::CIoUtils;
using Libcore::IO::IIoBridge;
using Libcore::IO::CIoBridge;
using Libcore::IO::IMemory;
using Libcore::IO::CMemory;
using Elastos::IO::ByteOrder_BIG_ENDIAN;
using Elastos::IO::EIID_ICloseable;
using Elastos::Core::CCloseGuard;
using Elastos::IO::CFileDescriptor;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::System::OsConstants;

namespace Elastos {
namespace Droid {
namespace Os {

const String ParcelFileDescriptor::TAG("ParcelFileDescriptor");
const Int32 ParcelFileDescriptor::MAX_STATUS = 1024;

//==================================================================
// ParcelFileDescriptor::Status
//==================================================================

/** Special value indicating remote side died. */
const Int32 ParcelFileDescriptor::Status::DEAD = -2;
/** Special value indicating no status should be written. */
const Int32 ParcelFileDescriptor::Status::SILENCE = -1;

/** Remote reported that everything went better than expected. */
const Int32 ParcelFileDescriptor::Status::OK = 0;
/** Remote reported error; length and message follow. */
const Int32 ParcelFileDescriptor::Status::ERROR = 1;
/** Remote reported {@link #detachFd()} and went rogue. */
const Int32 ParcelFileDescriptor::Status::DETACHED = 2;
/** Remote reported their object was finalized. */
const Int32 ParcelFileDescriptor::Status::LEAKED = 3;

ECode ParcelFileDescriptor::Status::AsIOException()
{
    switch (mStatus) {
        case DEAD: {
            //return new IOException("Remote side is dead");
            Logger::E("ParcelFileDescriptor", "Remote side is dead");
            return E_IO_EXCEPTION;
        }
        case OK: {
            return NOERROR;
        }
        case ERROR: {
            Logger::E("ParcelFileDescriptor", "Remote error: %s", mMsg.string());
            // return new IOException("Remote error: " + msg);
            return E_IO_EXCEPTION;
        }
        case DETACHED: {
            // return new FileDescriptorDetachedException();
            return E_IO_EXCEPTION;
        }
        case LEAKED: {
            Logger::E("ParcelFileDescriptor", "Remote side was leaked");
            // return new IOException("Remote side was leaked");
            return E_IO_EXCEPTION;
        }
        default: {
            // return new IOException("Unknown status: " + status);
            return E_IO_EXCEPTION;
        }
    }
    return NOERROR;
}

//==================================================================
// ParcelFileDescriptor::AutoCloseInputStream
//==================================================================
CAR_INTERFACE_IMPL(ParcelFileDescriptor::AutoCloseInputStream, FileInputStream, IParcelFileDescriptorAutoCloseInputStream)

ParcelFileDescriptor::AutoCloseInputStream::AutoCloseInputStream()
{
}

ECode ParcelFileDescriptor::AutoCloseInputStream::constructor(
    /* [in] */ IParcelFileDescriptor* pfd)
{
    AutoPtr<IFileDescriptor> fd;
    pfd->GetFileDescriptor((IFileDescriptor**)&pfd);
    FAIL_RETURN(FileInputStream::constructor(fd))

    mPfd = pfd;
    return NOERROR;
}

ECode ParcelFileDescriptor::AutoCloseInputStream::Close()
{
    //try {
    mPfd->Close();
    //} finally {
    return FileInputStream::Close();
    //}
}

//==================================================================
// ParcelFileDescriptor::AutoCloseOutputStream
//==================================================================
CAR_INTERFACE_IMPL(ParcelFileDescriptor::AutoCloseOutputStream, FileOutputStream, IParcelFileDescriptorAutoCloseOutputStream)

ParcelFileDescriptor::AutoCloseOutputStream::AutoCloseOutputStream()
{}

ECode ParcelFileDescriptor::AutoCloseOutputStream::constructor(
    /* [in] */ IParcelFileDescriptor* pfd)
{
    AutoPtr<IFileDescriptor> fd;
    pfd->GetFileDescriptor((IFileDescriptor**)&pfd);
    FAIL_RETURN(FileOutputStream::constructor(fd))

    mPfd = pfd;
    return NOERROR;
}

ECode ParcelFileDescriptor::AutoCloseOutputStream::Close()
{
    //try {
    mPfd->Close();
    //} finally {
    return FileOutputStream::Close();
    //}
}

//==================================================================
// ParcelFileDescriptor::MyHander
//==================================================================
ParcelFileDescriptor::MyHander::MyHander(
    /* [in] */ ILooper* looper,
    /* [in] */ IParcelFileDescriptorOnCloseListener* listener)
    : Handler(looper)
    , mListener(listener)
{
}

ECode ParcelFileDescriptor::MyHander::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    AutoPtr<Status> s = (Status*)IObject::Probe(obj);
    mListener->OnClose(s != NULL ? s->AsIOException() : NOERROR);
    return NOERROR;
}

//==================================================================
// ParcelFileDescriptor::ListenerBridge
//==================================================================
ParcelFileDescriptor::ListenerBridge::ListenerBridge(
    /* [in] */ IFileDescriptor* comm,
    /* [in] */ ILooper* looper,
    /* [in] */ IParcelFileDescriptorOnCloseListener* listener)
{
    Thread::constructor();
    mCommFd = comm;
    mHandler = new MyHander(looper, listener);
}

ECode ParcelFileDescriptor::ListenerBridge::Run()
{
    // try {
    AutoPtr<ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(MAX_STATUS);
    AutoPtr<Status> status = ParcelFileDescriptor::ReadCommStatus(mCommFd, buf);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(0, TO_IINTERFACE(status), (IMessage**)&msg);
    msg->SendToTarget();
    // } finally {

    AutoPtr<IIoUtils> iu;
    CIoUtils::AcquireSingleton((IIoUtils**)&iu);
    iu->CloseQuietly(mCommFd);

    mCommFd = NULL;
    // }
    return NOERROR;
}

//==================================================================
// ParcelFileDescriptor
//==================================================================
CAR_INTERFACE_IMPL_3(ParcelFileDescriptor, Object, IParcelFileDescriptor, IParcelable, ICloseable)

ParcelFileDescriptor::ParcelFileDescriptor()
    : mClosed(FALSE)
{
    CCloseGuard::New((ICloseGuard**)&mGuard);
}

ParcelFileDescriptor::~ParcelFileDescriptor()
{
    Finalize();
}

ECode ParcelFileDescriptor::constructor()
{
    return NOERROR;
}

ECode ParcelFileDescriptor::constructor(
    /* [in] */ IParcelFileDescriptor* wrapped)
{
    // We keep a strong reference to the wrapped PFD, and rely on its
    // finalizer to trigger CloseGuard. All calls are delegated to wrapper.
    mWrapped = wrapped;
    mFd = NULL;
    mCommFd = NULL;
    mClosed = TRUE;
    return NOERROR;
}

ECode ParcelFileDescriptor::constructor(
    /* [in] */ IFileDescriptor* fd)
{
    return constructor(fd, NULL);
}

ECode ParcelFileDescriptor::constructor(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IFileDescriptor* commChannel)
{
    if (fd == NULL) {
        // throw new NullPointerException("FileDescriptor must not be NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    mWrapped = NULL;
    mFd = fd;
    mCommFd = commChannel;
    mGuard->Open(String("close"));
    return NOERROR;
}

ECode ParcelFileDescriptor::Open(
    /* [in] */ IFile* file,
    /* [in] */ Int32 mode,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    VALIDATE_NOT_NULL(descriptor)
    *descriptor = NULL;

    AutoPtr<IFileDescriptor> fd;
    FAIL_RETURN(OpenInternal(file, mode, (IFileDescriptor**)&fd))
    if (fd == NULL) return NOERROR;

    AutoPtr<CParcelFileDescriptor> cpfd;
    CParcelFileDescriptor::NewByFriend((CParcelFileDescriptor**)&cpfd);
    cpfd->constructor(fd);
    *descriptor = (IParcelFileDescriptor*)cpfd.Get();
    REFCOUNT_ADD(*descriptor)
    return NOERROR;
}

ECode ParcelFileDescriptor::Open(
    /* [in] */ IFile* file,
    /* [in] */ Int32 mode,
    /* [in] */ IHandler* handler,
    /* [in] */ IParcelFileDescriptorOnCloseListener* listener,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    VALIDATE_NOT_NULL(descriptor)
    *descriptor = NULL;

    if (handler == NULL) {
        Logger::E(TAG, "Handler must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (listener == NULL) {
        Logger::E(TAG, "Listener must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IFileDescriptor> fd;
    FAIL_RETURN(OpenInternal(file, mode, (IFileDescriptor**)&fd))
    if (fd == NULL) return NOERROR;

    AutoPtr<ArrayOf<IFileDescriptor*> > comm;
    FAIL_RETURN(CreateCommSocketPair((ArrayOf<IFileDescriptor*>**)&comm))

    AutoPtr<CParcelFileDescriptor> cpfd;
    CParcelFileDescriptor::NewByFriend((CParcelFileDescriptor**)&cpfd);
    cpfd->constructor(fd, (*comm)[0]);

    // Kick off thread to watch for status updates
    AutoPtr<IIoUtils> iu;
    CIoUtils::AcquireSingleton((IIoUtils**)&iu);
    iu->SetBlocking((*comm)[1], TRUE);

    AutoPtr<ILooper> looper;
    handler->GetLooper((ILooper**)&looper);
    AutoPtr<ListenerBridge> bridge = new ListenerBridge(
        (*comm)[1], looper, listener);
    bridge->Start();

    *descriptor = (IParcelFileDescriptor*)cpfd.Get();
    REFCOUNT_ADD(*descriptor)
    return NOERROR;
}

ECode ParcelFileDescriptor::OpenInternal(
    /* [in] */ IFile* file,
    /* [in] */ Int32 mode,
    /* [out] */ IFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd)
    *fd = NULL;

    if ((mode & MODE_READ_WRITE) == 0) {
        Logger::E(TAG, "Must specify MODE_READ_ONLY, MODE_WRITE_ONLY, or MODE_READ_WRITE");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String path;
    file->GetPath(&path);
    return CParcel::OpenFileDescriptor(path, mode, fd);
}

ECode ParcelFileDescriptor::Dup(
    /* [in] */ IFileDescriptor* orig,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    VALIDATE_NOT_NULL(descriptor)
    *descriptor = NULL;

    // try {
    AutoPtr<IFileDescriptor> fd;
    ECode ec = Elastos::Droid::System::Os::Dup(orig, (IFileDescriptor**)&fd);
    if (FAILED(ec)) {
        return E_IO_EXCEPTION;
    }

    AutoPtr<CParcelFileDescriptor> cpfd;
    CParcelFileDescriptor::NewByFriend((CParcelFileDescriptor**)&cpfd);
    cpfd->constructor(fd);
    *descriptor = (IParcelFileDescriptor*)cpfd.Get();
    REFCOUNT_ADD(*descriptor)
    return NOERROR;
}

ECode ParcelFileDescriptor::Dup(
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    VALIDATE_NOT_NULL(descriptor)
    *descriptor = NULL;

    if (mWrapped != NULL) {
        return mWrapped->Dup(descriptor);
    }

    AutoPtr<IFileDescriptor> fd;
    GetFileDescriptor((IFileDescriptor**)&fd);
    return Dup(fd, descriptor);
}

ECode ParcelFileDescriptor::FromFd(
    /* [in] */ Int32 fd,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    VALIDATE_NOT_NULL(descriptor)
    *descriptor = NULL;

    AutoPtr<IFileDescriptor> original;
    CFileDescriptor::New((IFileDescriptor**)&original);
    original->SetDescriptor(fd);

    AutoPtr<IFileDescriptor> dup;
    ECode ec = Elastos::Droid::System::Os::Dup(original, (IFileDescriptor**)&dup);
    if (FAILED(ec)) {
        return E_IO_EXCEPTION;
    }

    AutoPtr<CParcelFileDescriptor> cpfd;
    CParcelFileDescriptor::NewByFriend((CParcelFileDescriptor**)&cpfd);
    cpfd->constructor(dup);
    *descriptor = (IParcelFileDescriptor*)cpfd.Get();
    REFCOUNT_ADD(*descriptor)
    return NOERROR;
}

ECode ParcelFileDescriptor::AdoptFd(
    /* [in] */ Int32 fd,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    VALIDATE_NOT_NULL(descriptor)
    *descriptor = NULL;

    AutoPtr<IFileDescriptor> fdesc;
    CFileDescriptor::New((IFileDescriptor**)&fdesc);
    fdesc->SetDescriptor(fd);

    AutoPtr<CParcelFileDescriptor> cpfd;
    CParcelFileDescriptor::NewByFriend((CParcelFileDescriptor**)&cpfd);
    cpfd->constructor(fdesc);
    *descriptor = (IParcelFileDescriptor*)cpfd.Get();
    REFCOUNT_ADD(*descriptor)
    return NOERROR;
}

ECode ParcelFileDescriptor::FromSocket(
    /* [in] */ ISocket* socket,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    VALIDATE_NOT_NULL(descriptor)
    *descriptor = NULL;

    AutoPtr<IFileDescriptor> fd;
    socket->GetFileDescriptor((IFileDescriptor**)&fd);

    if (fd != NULL) {
        AutoPtr<CParcelFileDescriptor> cpfd;
        CParcelFileDescriptor::NewByFriend((CParcelFileDescriptor**)&cpfd);
        cpfd->constructor(fd);
        *descriptor = (IParcelFileDescriptor*)cpfd.Get();
        REFCOUNT_ADD(*descriptor)
    }
    return NOERROR;
}

ECode ParcelFileDescriptor::FromDatagramSocket(
    /* [in] */ IDatagramSocket* socket,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    VALIDATE_NOT_NULL(descriptor)
    *descriptor = NULL;

    AutoPtr<IFileDescriptor> fd;
    socket->GetFileDescriptor((IFileDescriptor**)&fd);

    if (fd != NULL) {
        AutoPtr<CParcelFileDescriptor> cpfd;
        CParcelFileDescriptor::NewByFriend((CParcelFileDescriptor**)&cpfd);
        cpfd->constructor(fd);
        *descriptor = (IParcelFileDescriptor*)cpfd.Get();
        REFCOUNT_ADD(*descriptor)
    }
    return NOERROR;
}

ECode ParcelFileDescriptor::CreatePipe(
    /* [out, callee] */ ArrayOf<IParcelFileDescriptor*>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr< ArrayOf<IFileDescriptor*> > fds;
    ECode ec = Elastos::Droid::System::Os::Pipe((ArrayOf<IFileDescriptor*>**)&fds);
    if (FAILED(ec)) {
        return E_IO_EXCEPTION;
    }

    AutoPtr< ArrayOf<IParcelFileDescriptor*> > pfds = ArrayOf<IParcelFileDescriptor*>::Alloc(2);
    AutoPtr<CParcelFileDescriptor> pfd1, pfd2;
    CParcelFileDescriptor::NewByFriend((CParcelFileDescriptor**)&pfd1);
    CParcelFileDescriptor::NewByFriend((CParcelFileDescriptor**)&pfd2);
    pfd1->constructor((*fds)[0]);
    pfd2->constructor((*fds)[1]);
    pfds->Set(0, (IParcelFileDescriptor*)pfd1.Get());
    pfds->Set(1, (IParcelFileDescriptor*)pfd2.Get());

    *result = pfds;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelFileDescriptor::CreateReliablePipe(
    /* [out, callee] */ ArrayOf<IParcelFileDescriptor*>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr< ArrayOf<IFileDescriptor*> > comm;
    AutoPtr< ArrayOf<IFileDescriptor*> > fds;

    ECode ec = Elastos::Droid::System::Os::Pipe((ArrayOf<IFileDescriptor*>**)&fds);
    if (FAILED(ec)) return E_IO_EXCEPTION;

    ec = CreateCommSocketPair((ArrayOf<IFileDescriptor*>**)&comm);
    if (FAILED(ec)) return E_IO_EXCEPTION;

    AutoPtr< ArrayOf<IParcelFileDescriptor*> > pfds = ArrayOf<IParcelFileDescriptor*>::Alloc(2);
    AutoPtr<CParcelFileDescriptor> pfd1, pfd2;
    CParcelFileDescriptor::NewByFriend((CParcelFileDescriptor**)&pfd1);
    CParcelFileDescriptor::NewByFriend((CParcelFileDescriptor**)&pfd2);
    pfd1->constructor((*fds)[0], (*comm)[0]);
    pfd2->constructor((*fds)[1], (*comm)[1]);
    pfds->Set(0, (IParcelFileDescriptor*)pfd1.Get());
    pfds->Set(1, (IParcelFileDescriptor*)pfd2.Get());

    *result = pfds;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelFileDescriptor::CreateSocketPair(
    /* [out, callee] */ ArrayOf<IParcelFileDescriptor*>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IFileDescriptor> fd0, fd1;
    CFileDescriptor::New((IFileDescriptor**)&fd0);
    CFileDescriptor::New((IFileDescriptor**)&fd1);

    ECode ec = Elastos::Droid::System::Os::Socketpair(
        OsConstants::_AF_UNIX, OsConstants::_SOCK_STREAM, 0, fd0, fd1);
    if (FAILED(ec)) return E_IO_EXCEPTION;

    AutoPtr< ArrayOf<IParcelFileDescriptor*> > pfds = ArrayOf<IParcelFileDescriptor*>::Alloc(2);
    AutoPtr<CParcelFileDescriptor> pfd1, pfd2;
    CParcelFileDescriptor::NewByFriend((CParcelFileDescriptor**)&pfd1);
    CParcelFileDescriptor::NewByFriend((CParcelFileDescriptor**)&pfd2);
    pfd1->constructor(fd0);
    pfd2->constructor(fd1);
    pfds->Set(0, (IParcelFileDescriptor*)pfd1.Get());
    pfds->Set(1, (IParcelFileDescriptor*)pfd2.Get());

    *result = pfds;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelFileDescriptor::CreateReliableSocketPair(
    /* [out, callee] */ ArrayOf<IParcelFileDescriptor*>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IFileDescriptor> fd0, fd1;
    CFileDescriptor::New((IFileDescriptor**)&fd0);
    CFileDescriptor::New((IFileDescriptor**)&fd1);

    ECode ec = Elastos::Droid::System::Os::Socketpair(
        OsConstants::_AF_UNIX, OsConstants::_SOCK_STREAM, 0, fd0, fd1);
    if (FAILED(ec)) return E_IO_EXCEPTION;

    AutoPtr< ArrayOf<IFileDescriptor*> > comm;
    ec = CreateCommSocketPair((ArrayOf<IFileDescriptor*>**)&comm);
    if (FAILED(ec)) return E_IO_EXCEPTION;

    AutoPtr< ArrayOf<IParcelFileDescriptor*> > pfds = ArrayOf<IParcelFileDescriptor*>::Alloc(2);
    AutoPtr<CParcelFileDescriptor> pfd1, pfd2;
    CParcelFileDescriptor::NewByFriend((CParcelFileDescriptor**)&pfd1);
    CParcelFileDescriptor::NewByFriend((CParcelFileDescriptor**)&pfd2);
    pfd1->constructor(fd0, (*comm)[0]);
    pfd2->constructor(fd1, (*comm)[1]);
    pfds->Set(0, (IParcelFileDescriptor*)pfd1.Get());
    pfds->Set(1, (IParcelFileDescriptor*)pfd2.Get());

    *result = pfds;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelFileDescriptor::CreateCommSocketPair(
    /* [out, callee] */ ArrayOf<IFileDescriptor*>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IFileDescriptor> comm1, comm2;
    CFileDescriptor::New((IFileDescriptor**)&comm1);
    CFileDescriptor::New((IFileDescriptor**)&comm2);

    ECode ec = Elastos::Droid::System::Os::Socketpair(
        OsConstants::_AF_UNIX, OsConstants::_SOCK_STREAM, 0, comm1, comm2);
    if (FAILED(ec)) return E_IO_EXCEPTION;

    AutoPtr<IIoUtils> iu;
    CIoUtils::AcquireSingleton((IIoUtils**)&iu);
    iu->SetBlocking(comm1, FALSE);
    iu->SetBlocking(comm2, FALSE);

    *result = ArrayOf<IFileDescriptor*>::Alloc(2);
    REFCOUNT_ADD(*result)

    (*result)->Set(0, comm1);
    (*result)->Set(1, comm2);
    return NOERROR;
}

ECode ParcelFileDescriptor::FromData(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ const String& name,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    return NOERROR;
}

ECode ParcelFileDescriptor::ParseMode(
    /* [in] */ const String& mode,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 modeBits;
    if (mode.Equals("r")) {
        modeBits = IParcelFileDescriptor::MODE_READ_ONLY;
    } else if (mode.Equals("w") || mode.Equals("wt")) {
        modeBits = IParcelFileDescriptor::MODE_WRITE_ONLY
                | IParcelFileDescriptor::MODE_CREATE
                | IParcelFileDescriptor::MODE_TRUNCATE;
    } else if (mode.Equals("wa")) {
        modeBits = IParcelFileDescriptor::MODE_WRITE_ONLY
                | IParcelFileDescriptor::MODE_CREATE
                | IParcelFileDescriptor::MODE_APPEND;
    } else if (mode.Equals("rw")) {
        modeBits = IParcelFileDescriptor::MODE_READ_WRITE
                | IParcelFileDescriptor::MODE_CREATE;
    } else if (mode.Equals("rwt")) {
        modeBits = IParcelFileDescriptor::MODE_READ_WRITE
                | IParcelFileDescriptor::MODE_CREATE
                | IParcelFileDescriptor::MODE_TRUNCATE;
    } else {
        // throw new IllegalArgumentException("Bad mode '" + mode + "'");
        Logger::E(TAG, "Bad mode %s", mode.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *result = modeBits;
    return NOERROR;
}

ECode ParcelFileDescriptor::GetFileDescriptor(
    /* [out] */ IFileDescriptor** des)
{
    if (mWrapped != NULL) {
        return mWrapped->GetFileDescriptor(des);
    }
    else {
        *des = mFd;
        REFCOUNT_ADD(*des)
    }

    return NOERROR;
}

ECode ParcelFileDescriptor::GetStatSize(
    /* [out] */ Int64* len)
{
    VALIDATE_NOT_NULL(len)
    if (mWrapped != NULL) {
        return mWrapped->GetStatSize(len);
    }

    // try {
    AutoPtr<IStructStat> st;
    Elastos::Droid::System::Os::Fstat(mFd, (IStructStat**)&st);
    Int32 mode;
    st->GetMode(&mode);
    if (OsConstants::S_IsREG(mode) || OsConstants::S_IsLNK(mode)) {
        return st->GetSize(len);
    }
    else {
        *len = -1;
        return NOERROR;
    }
    // } catch (ErrnoException e) {
    //     Logger::W(TAG, "fstat() failed: " + e);
    //     return -1;
    // }
}

ECode ParcelFileDescriptor::SeekTo(
    /* [in] */ Int64 pos,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    if (mWrapped != NULL) {
        return mWrapped->SeekTo(pos, result);
    }

    // try {
    return Elastos::Droid::System::Os::Lseek(mFd, pos, SEEK_SET, result);
    // } catch (ErrnoException e) {
    //     throw e.rethrowAsIOException();
    // }
}

ECode ParcelFileDescriptor::GetFd(
    /* [out] */ Int32* fd)
{
    VALIDATE_NOT_NULL(fd)
    *fd = -1;

    if (mWrapped != NULL) {
        return mWrapped->GetFd(fd);
    }

    if (mClosed) {
        // throw new IllegalStateException("Already closed");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return mFd->GetDescriptor(fd);
}

ECode ParcelFileDescriptor::DetachFd(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = -1;

    if (mWrapped != NULL) {
        return mWrapped->DetachFd(result);
    }

    if (mClosed) {
        //throw new IllegalStateException("Already closed");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 fd;
    GetFd(&fd);
    CParcel::ClearFileDescriptor(mFd);
    WriteCommStatusAndClose(Status::DETACHED, String(NULL));
    *result = fd;

    return NOERROR;
}

ECode ParcelFileDescriptor::Close()
{
    if (mWrapped != NULL) {
        // try {
            mWrapped->Close();
        // } finally {
            ReleaseResources();
        // }
    }
    else {
        CloseWithStatus(Status::OK, String(NULL));
    }
    return NOERROR;
}

ECode ParcelFileDescriptor::CloseWithError(
    /* [in] */ const String& msg)
{
    if (mWrapped != NULL) {
        // try {
            mWrapped->CloseWithError(msg);
        // } finally {
            ReleaseResources();
        // }
    }
    else {
        if (msg == NULL) {
            // throw new IllegalArgumentException("Message must not be NULL");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        CloseWithStatus(Status::ERROR, msg);
    }
    return NOERROR;
}

ECode ParcelFileDescriptor::CloseWithStatus(
    /* [in] */ Int32 status,
    /* [in] */ const String& msg)
{
    if (mClosed) return NOERROR;
    mClosed = TRUE;
    mGuard->Close();
    // Status MUST be sent before closing actual descriptor
    WriteCommStatusAndClose(status, msg);
    AutoPtr<IIoUtils> iu;
    CIoUtils::AcquireSingleton((IIoUtils**)&iu);
    iu->CloseQuietly(mFd);
    ReleaseResources();
    return NOERROR;
}

ECode ParcelFileDescriptor::ReleaseResources()
{
    return NOERROR;
}

ECode ParcelFileDescriptor::GetOrCreateStatusBuffer(
    /* [out, callee] */ ArrayOf<Byte>** buffer)
{
    VALIDATE_NOT_NULL(buffer)
    if (mStatusBuf == NULL) {
        mStatusBuf = ArrayOf<Byte>::Alloc(MAX_STATUS);
    }

    *buffer = mStatusBuf;
    REFCOUNT_ADD(*buffer)
    return NOERROR;
}

ECode ParcelFileDescriptor::WriteCommStatusAndClose(
    /* [in] */ Int32 status,
    /* [in] */ const String& msg)
{
    if (mCommFd == NULL) {
        // Not reliable, or someone already sent status
        if (!msg.IsNull()) {
            Logger::W(TAG, "Unable to inform peer: %s", msg.string());
        }
        return NOERROR;
    }

    if (status == Status::DETACHED) {
        Logger::W(TAG, "Peer expected signal when closed; unable to deliver after detach");
    }

    // try {
    if (status == Status::SILENCE) return NOERROR;

    // Since we're about to close, read off any remote Status:: It's
    // okay to remember missing here.
    AutoPtr<ArrayOf<Byte> > buf;
    GetOrCreateStatusBuffer((ArrayOf<Byte>**)&buf);
    mStatus = ReadCommStatus(mCommFd, buf);

    // Skip writing status when other end has already gone away.
    if (mStatus != NULL) return NOERROR;

    // try {
        Int32 writePtr = 0;

        AutoPtr<IMemory> memory;
        CMemory::AcquireSingleton((IMemory**)&memory);
        memory->PokeInt32(buf, writePtr, status, ByteOrder_BIG_ENDIAN);
        writePtr += 4;

        if (msg != NULL) {
            AutoPtr<ArrayOf<Byte> > rawMsg = msg.GetBytes();
            Int32 len = Elastos::Core::Math::Min(rawMsg->GetLength(), buf->GetLength() - writePtr);
            buf->Copy(writePtr, rawMsg, 0, len);
            writePtr += len;
        }

        Int32 num;
        ECode ec = Elastos::Droid::System::Os::Write(mCommFd, buf, 0, writePtr, &num);
    //} catch (ErrnoException e) {
        // Reporting status is best-effort
        if (FAILED(ec)) Logger::W(TAG, "Failed to report status: %08x", ec);
    //} catch (InterruptedIOException e) {
        // Reporting status is best-effort
        // Logger::W(TAG, "Failed to report status: " + e);
    //}

    // } finally {
    AutoPtr<IIoUtils> iu;
    CIoUtils::AcquireSingleton((IIoUtils**)&iu);
    iu->CloseQuietly(mCommFd);
    mCommFd = NULL;
    // }
    return NOERROR;
}

AutoPtr<ParcelFileDescriptor::Status> ParcelFileDescriptor::ReadCommStatus(
    /* [in] */ IFileDescriptor* comm,
    /* [in] */ ArrayOf<Byte>* buf)
{
    // try {
    AutoPtr<Status> result;
    AutoPtr<IMemory> memory;
    CMemory::AcquireSingleton((IMemory**)&memory);
    Int32 n, status;
    ECode ec = Elastos::Droid::System::Os::Read(comm, buf, 0, buf->GetLength(), &n);
    FAIL_GOTO(ec, _EXIT_)

    if (n == 0) {
        // EOF means they're dead
        result = new Status(Status::DEAD);
        return result;
    }
    else {
        ec = memory->PeekInt32(buf, 0, ByteOrder_BIG_ENDIAN, &status);
        FAIL_GOTO(ec, _EXIT_)
        if (status == Status::ERROR) {
            String msg((const char*)buf->GetPayload() + 4, n - 4);
            result = new Status(status, msg);
            return result;
        }
        result = new Status(status);
        return result;
    }

_EXIT_:
    if (ec == OsConstants::_EAGAIN) {
        return NULL;
    }
    else {
        Logger::D(TAG, "Failed to read status; assuming dead: %08x", ec);
        result = new Status(Status::DEAD);
        return result;
    }
}

ECode ParcelFileDescriptor::CanDetectErrors(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mWrapped != NULL) {
        return mWrapped->CanDetectErrors(result);
    }

    *result = mCommFd != NULL;
    return NOERROR;
}

ECode ParcelFileDescriptor::CheckError()
{
    if (mWrapped != NULL) {
        mWrapped->CheckError();
    }
    else {
        if (mStatus == NULL) {
            if (mCommFd == NULL) {
                Logger::W(TAG, "Peer didn't provide a comm channel; unable to check for errors");
                return NOERROR;
            }

            // Try reading status; it might be NULL if nothing written yet.
            // Either way, we keep comm open to write our status later.
            AutoPtr<ArrayOf<Byte> > buf;
            GetOrCreateStatusBuffer((ArrayOf<Byte>**)&buf);
            mStatus = ReadCommStatus(mCommFd, buf);
        }

        if (mStatus == NULL || mStatus->mStatus == Status::OK) {
            // No status yet, or everything is peachy!
            return NOERROR;
        }
        else {
            return mStatus->AsIOException();
        }
    }
    return NOERROR;
}

ECode ParcelFileDescriptor::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    if (mWrapped != NULL) {
        *str = Object::ToString(mWrapped);
        return NOERROR;
    }

    String value("{ParcelFileDescriptor: ");
    value += Object::ToString(mFd);
    value += "}";
    *str = value;
    return NOERROR;
}

ECode ParcelFileDescriptor::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    // WARNING: This must stay in sync with Parcel::writeParcelFileDescriptor()
    // in frameworks/native/libs/binder/Parcel.cpp
    Int32 value;
    in->ReadInt32(&value);
    if (value == 1) {
        // mWrapped
        mWrapped = NULL;
        CParcelFileDescriptor::New((IParcelFileDescriptor**)&mWrapped);
        IParcelable::Probe(mWrapped)->ReadFromParcel(in);
        return NOERROR;
    }
    else {
        Int32 fd;
        FAIL_RETURN(in->ReadFileDescriptor(&fd))
        fd = dup(fd);
        if (fd < 0) return E_IO_EXCEPTION;
        if (mFd == NULL) {
            CFileDescriptor::New((IFileDescriptor**)&mFd);
        }
        mFd->SetDescriptor(fd);

        in->ReadInt32(&value);
        if (value != 0) {
            FAIL_RETURN(in->ReadFileDescriptor(&fd))
            fd = dup(fd);
            if (fd < 0) return E_IO_EXCEPTION;
            if (mCommFd == NULL) {
                CFileDescriptor::New((IFileDescriptor**)&mCommFd);
            }
            mCommFd->SetDescriptor(fd);
        }
    }

    return NOERROR;
}

ECode ParcelFileDescriptor::WriteToParcel(
    /* [in] */ IParcel* out)
{
    // WARNING: This must stay in sync with Parcel::readParcelFileDescriptor()
    // in frameworks/native/libs/binder/Parcel.cpp
    if (mWrapped != NULL) {
        out->WriteInt32(1);
        // try {
            IParcelable::Probe(mWrapped)->WriteToParcel(out);
        // } finally {
            ReleaseResources();
        // }
    }
    else {
        out->WriteInt32(0);

        Int32 fd = -1;
        mFd->GetDescriptor(&fd);
        out->WriteFileDescriptor(fd);
        if (mCommFd != NULL) {
            mCommFd->GetDescriptor(&fd);
            out->WriteInt32(1);
            out->WriteFileDescriptor(fd);
        }
        else {
            out->WriteInt32(0);
        }
        if (/*(flags & PARCELABLE_WRITE_RETURN_VALUE) != 0 &&*/ !mClosed) {
            // Not a real close, so emit no status
            CloseWithStatus(Status::SILENCE, String(NULL));
        }
    }
    return NOERROR;
}

ECode ParcelFileDescriptor::Finalize()
{
    if (mWrapped != NULL) {
        ReleaseResources();
    }
    if (mGuard != NULL) {
        mGuard->WarnIfOpen();
    }
    // try {
        if (!mClosed) {
            CloseWithStatus(Status::LEAKED, String(NULL));
        }
    // } finally {
    //     super.finalize();
    // }
    return NOERROR;
}


} // namespace Os
} // namespace Droid
} // namespace Elastos
