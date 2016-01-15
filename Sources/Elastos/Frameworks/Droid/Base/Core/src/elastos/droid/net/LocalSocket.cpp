
#include <Elastos.CoreLibrary.Net.h>
#include "elastos/droid/net/LocalSocket.h"
#include "elastos/droid/net/LocalSocketImpl.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Net::ISocketOptions;
using Elastos::IO::EIID_ICloseable;
using Elastos::IO::IFileDescriptor;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL_2(LocalSocket, Object, ILocalSocket, ICloseable)

const Int32 LocalSocket::SOCKET_UNKNOWN = 0;

ECode LocalSocket::constructor()
{
    return constructor(SOCKET_STREAM);
}

ECode LocalSocket::constructor(
    /* [in] */ Int32 sockType)
{
    AutoPtr<LocalSocketImpl> localSocketImpl = new LocalSocketImpl();
    localSocketImpl->constructor();
    constructor(localSocketImpl, sockType);
    mIsBound = FALSE;
    mIsConnected = FALSE;
    return NOERROR;
}

ECode LocalSocket::constructor(
    /* [in] */ IFileDescriptor* fd)
{
    AutoPtr<LocalSocketImpl> localSocketImpl = new LocalSocketImpl();
    localSocketImpl->constructor(fd);
    constructor(localSocketImpl, SOCKET_UNKNOWN);
    mIsBound = TRUE;
    mIsConnected = TRUE;
    return NOERROR;
}

ECode LocalSocket::constructor(
    /* [in] */ ILocalSocketImpl* impl,
    /* [in] */ Int32 sockType)
{
    mImpl = impl;
    mSockType = sockType;
    mIsConnected = FALSE;
    mIsBound = FALSE;
    return NOERROR;
}

ECode LocalSocket::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    Object::ToString(result);
    *result += " impl:";
    String s;
    IObject::Probe(mImpl)->ToString(&s);
    *result += s;
    return NOERROR;
}

ECode LocalSocket::ImplCreateIfNeeded()
{
    if (!mImplCreated) {
        synchronized(this) {
            if (!mImplCreated) {
    //            try {
                mImpl->Create(mSockType);
    //            } finally {
                mImplCreated = TRUE;
    //            }
            }
        }
    }
    return NOERROR;
}

ECode LocalSocket::Connect(
    /* [in] */ ILocalSocketAddress* endpoint)
{
    synchronized(this) {
        if (mIsConnected) {
            Logger::E("LocalSocket", "already connected");
            return E_IO_EXCEPTION;
        }

        ImplCreateIfNeeded();
        ((LocalSocketImpl*)mImpl.Get())->Connect(endpoint, 0);
        mIsConnected = TRUE;
        mIsBound = TRUE;
    }
    return NOERROR;
}

ECode LocalSocket::Bind(
    /* [in] */ ILocalSocketAddress* bindpoint)
{
    ImplCreateIfNeeded();

    synchronized(this) {
        if (mIsBound) {
            Logger::E("LocalSocket", "already bound");
            return E_IO_EXCEPTION;
        }

        mLocalAddress = bindpoint;
        mImpl->Bind(mLocalAddress);
        mIsBound = TRUE;
    }
    return NOERROR;
}

ECode LocalSocket::GetLocalSocketAddress(
    /* [out] */ ILocalSocketAddress** address)
{
    VALIDATE_NOT_NULL(address);

    *address = mLocalAddress;
    REFCOUNT_ADD(*address)
    return NOERROR;
}

ECode LocalSocket::GetInputStream(
    /* [out] */ IInputStream** is)
{
    VALIDATE_NOT_NULL(is);

    ImplCreateIfNeeded();
    return ((LocalSocketImpl*)mImpl.Get())->GetInputStream(is);
}

ECode LocalSocket::GetOutputStream(
    /* [out] */ IOutputStream** os)
{
    VALIDATE_NOT_NULL(os);

    ImplCreateIfNeeded();
    return ((LocalSocketImpl*)mImpl.Get())->GetOutputStream(os);
}

ECode LocalSocket::Close()
{
    ImplCreateIfNeeded();
    return mImpl->Close();
}

ECode LocalSocket::ShutdownInput()
{
    ImplCreateIfNeeded();
    return ((LocalSocketImpl*)mImpl.Get())->ShutdownInput();
}

ECode LocalSocket::ShutdownOutput()
{
    ImplCreateIfNeeded();
    return ((LocalSocketImpl*)mImpl.Get())->ShutdownOutput();
}

ECode LocalSocket::SetReceiveBufferSize(
    /* [in] */ Int32 size)
{
    AutoPtr<IInteger32> sizeObj;
    CInteger32::New(size, (IInteger32**)&sizeObj);
    return mImpl->SetOption(ISocketOptions::_SO_RCVBUF, sizeObj);
}

ECode LocalSocket::GetReceiveBufferSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);

    AutoPtr<IInteger32> sizeObj;
    FAIL_RETURN(mImpl->GetOption(ISocketOptions::_SO_RCVBUF, (IInterface**)&sizeObj));
    return sizeObj->GetValue(size);
}

ECode LocalSocket::SetSoTimeout(
    /* [in] */ Int32 n)
{
    AutoPtr<IInteger32> nObj;
    CInteger32::New(n, (IInteger32**)&nObj);
    return mImpl->SetOption(ISocketOptions::_SO_TIMEOUT, nObj);
}

ECode LocalSocket::GetSoTimeout(
    /* [out] */ Int32* n)
{
    VALIDATE_NOT_NULL(n);

    AutoPtr<IInteger32> nObj;
    FAIL_RETURN(mImpl->GetOption(ISocketOptions::_SO_TIMEOUT, (IInterface**)&nObj));
    return nObj->GetValue(n);
}

ECode LocalSocket::SetSendBufferSize(
    /* [in] */ Int32 size)
{
    AutoPtr<IInteger32> sizeObj;
    CInteger32::New(size, (IInteger32**)&sizeObj);
    return mImpl->SetOption(ISocketOptions::_SO_SNDBUF, sizeObj);
}

ECode LocalSocket::GetSendBufferSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);

    AutoPtr<IInteger32> sizeObj;
    FAIL_RETURN(mImpl->GetOption(ISocketOptions::_SO_SNDBUF, (IInterface**)&sizeObj));
    return sizeObj->GetValue(size);
}

ECode LocalSocket::GetRemoteSocketAddress(
    /* [out] */ ILocalSocketAddress** result)
{
    VALIDATE_NOT_NULL(result);

    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode LocalSocket::IsConnected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    synchronized(this) {
        *result = mIsConnected;
    }
    return NOERROR;
}

ECode LocalSocket::IsClosed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode LocalSocket::IsBound(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    synchronized(this) {
        *result = mIsBound;
    }
    return NOERROR;
}

ECode LocalSocket::IsOutputShutdown(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode LocalSocket::IsInputShutdown(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode LocalSocket::Connect(
    /* [in] */ ILocalSocketAddress* endpoint,
    /* [in] */ Int32 timeout)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode LocalSocket::SetFileDescriptorsForSend(
    /* [in] */ ArrayOf<IFileDescriptor*>* fds)
{
    mImpl->SetFileDescriptorsForSend(fds);
    return NOERROR;
}

ECode LocalSocket::GetAncillaryFileDescriptors(
    /* [out, callee] */ ArrayOf<IFileDescriptor*>** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ArrayOf<IFileDescriptor*> > fds;
    mImpl->GetAncillaryFileDescriptors((ArrayOf<IFileDescriptor*>**)&fds);
    *result = fds;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode LocalSocket::GetPeerCredentials(
    /* [out] */ ICredentials** result)
{
    VALIDATE_NOT_NULL(result);

    return mImpl->GetPeerCredentials(result);
}

ECode LocalSocket::GetFileDescriptor(
    /* [out] */ IFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd);

    AutoPtr<IFileDescriptor> _fd;
    ((LocalSocketImpl*)mImpl.Get())->GetFileDescriptor((IFileDescriptor**)&_fd);
    *fd = _fd;
    REFCOUNT_ADD(*fd);
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
