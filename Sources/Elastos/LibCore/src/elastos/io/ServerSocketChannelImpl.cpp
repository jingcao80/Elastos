
#include "ServerSocketChannelImpl.h"
#include "CInetSocketAddress.h"
#include "AutoLock.h"
#include "SocketImpl.h"
#include "SocketChannelImpl.h"
#include "IoUtils.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Libcore::IO::IoUtils;
using Elastos::Net::CInetSocketAddress;
using Elastos::Net::ISocketImpl;
using Elastos::Net::SocketImpl;

namespace Elastos {
namespace IO {

//==========================================================
//       ServerSocketChannelImpl::ServerSocketAdapter
//==========================================================
ServerSocketChannelImpl::ServerSocketAdapter::ServerSocketAdapter(
    /* [in] */ ServerSocketChannelImpl* aChannelImpl)
{
    mChannelImpl = aChannelImpl;
}

CAR_INTERFACE_IMPL(ServerSocketChannelImpl, Object, IFileDescriptorChannel)

ECode ServerSocketChannelImpl::ServerSocketAdapter::Accept(
    /* [out] */ ISocket** socket)
{
    VALIDATE_NOT_NULL(socket)
    *socket = NULL;

    Boolean isflag = FALSE;
    if (IsBound(&isflag), !isflag) {
        // throw new IllegalBlockingModeException();
        return E_ILLEGAL_BLOCKING_MODE_EXCEPTION;
    }
    AutoPtr<ISocketChannel> sc;
    mChannelImpl->Accept((ISocketChannel**)&sc);
    if (sc == NULL) {
        // throw new IllegalBlockingModeException();
        return E_ILLEGAL_BLOCKING_MODE_EXCEPTION;
    }
    return sc->GetSocket(socket);
}

ECode ServerSocketChannelImpl::ServerSocketAdapter::ImplAccept(
    /* [in] */ SocketChannelImpl* clientSocketChannel,
    /* [out] */ ISocket** aSocket)
{
    VALIDATE_NOT_NULL(aSocket)
    *aSocket = NULL;

    AutoPtr<ISocket> clientSocket;
    clientSocketChannel->GetSocket((ISocket**)&clientSocket);
    Boolean connectOK = FALSE;
    // try {
    {    AutoLock syncLock(this);
        ServerSocket::ImplAccept(clientSocket);

        // Sync the client socket's associated channel state with the Socket and OS.
        AutoPtr<IInetSocketAddress> remoteAddress;
        AutoPtr<IInetAddress> addres;
        clientSocket->GetInetAddress((IInetAddress**)&addres);
        Int32 portnum = 0;
        clientSocket->GetPort(&portnum);
        FAIL_RETURN(CInetSocketAddress::New(addres, portnum, (IInetSocketAddress**)&remoteAddress));
        clientSocketChannel->OnAccept(remoteAddress, FALSE /* updateSocketState */);
    }
    connectOK = TRUE;
    // } finally {
    if (!connectOK) {
        clientSocket->Close();
    }
    // }
    *aSocket = clientSocket;
    REFCOUNT_ADD(*aSocket)
    return NOERROR;
}

ECode ServerSocketChannelImpl::ServerSocketAdapter::GetChannel(
    /* [out] */ IServerSocketChannel** channel)
{
    VALIDATE_NOT_NULL(channel)

    *channel = IServerSocketChannel::Probe(mChannelImpl);
    REFCOUNT_ADD(*channel)
    return NOERROR;
}

ECode ServerSocketChannelImpl::ServerSocketAdapter::Close()
{
    {    AutoLock syncLock(mChannelImpl);
        ServerSocket::Close();
        Boolean isflag = FALSE;
        if (mChannelImpl->IsOpen(&isflag), isflag) {
            mChannelImpl->Close();
        }
    }
    return NOERROR;
}

AutoPtr<IFileDescriptor> ServerSocketChannelImpl::ServerSocketAdapter::GetFD()
{
    AutoPtr<ISocketImpl> si;
    ServerSocket::GetImpl((ISocketImpl**)&si);
    AutoPtr<IFileDescriptor> fd;
    ((SocketImpl*)si.Get())->GetFD((IFileDescriptor**)&fd);
    return fd;
}

//==========================================================
//       ServerSocketChannelImpl
//==========================================================

ServerSocketChannelImpl::ServerSocketChannelImpl(
    /* [in] */ ISelectorProvider* sp)
{
    ServerSocketChannel::constructor(sp);
    mSocket = new ServerSocketAdapter(this);
}

ECode ServerSocketChannelImpl::GetSocket(
    /* [out] */ IServerSocket** outsock)
{
    VALIDATE_NOT_NULL(outsock)

    *outsock = mSocket;
    REFCOUNT_ADD(*outsock)
    return NOERROR;
}

ECode ServerSocketChannelImpl::Accept(
    /* [out] */ ISocketChannel** channel)
{
    VALIDATE_NOT_NULL(channel)
    *channel = NULL;

    Boolean isflag = FALSE;
    if (IsOpen(&isflag), !isflag) {
        // throw new ClosedChannelException();
        return E_CLOSED_CHANNEL_EXCEPTION;
    }
    if (mSocket->IsBound(&isflag), isflag) {
        // throw new NotYetBoundException();
        return E_NOT_YET_BOUND_EXCEPTION;
    }

    // Create an empty socket channel. This will be populated by ServerSocketAdapter.implAccept.
    AutoPtr<ISelectorProvider> spro;
    GetProvider((ISelectorProvider**)&spro);
    AutoPtr<SocketChannelImpl> result = new SocketChannelImpl(spro, FALSE);
    // try {
        Begin();
        {    AutoLock syncLock(mAcceptLock);
            // try {
            AutoPtr<ISocket> outsoc;
            FAIL_RETURN(mSocket->ImplAccept(result, (ISocket**)&outsoc));
            // } catch (SocketTimeoutException e) {
            //     if (shouldThrowSocketTimeoutExceptionFromAccept(e)) {
            //         throw e;
            //     }
            //     // Otherwise, this is a non-blocking socket and there's nothing ready, so we'll
            //     // fall through and return null.
            // }
        }
    // } finally {
        result->IsConnected(&isflag);
        End(isflag);
    // }

    if (isflag) {
        *channel =  ISocketChannel::Probe(result);
        REFCOUNT_ADD(*channel)
    }
    return NOERROR;
}

ECode ServerSocketChannelImpl::GetFD(
    /* [out] */ IFileDescriptor** outfd)
{
    VALIDATE_NOT_NULL(outfd)
    AutoPtr<IFileDescriptor> temp = mSocket->GetFD();
    *outfd = temp;
    REFCOUNT_ADD(*outfd)
    return NOERROR;
}

Boolean ServerSocketChannelImpl::ShouldThrowSocketTimeoutExceptionFromAccept(
    /* [in] */ ECode e)
{
    Boolean isflag = FALSE;
    if (IsBlocking(&isflag), isflag) {
        return TRUE;
    }
    assert(0 && "TODO");
    // Throwable cause = e.getCause();
    // if (cause instanceof ErrnoException) {
    //     if (((ErrnoException) cause).errno == EAGAIN) {
    //         return false;
    //     }
    // }
    return TRUE;
}

ECode ServerSocketChannelImpl::ImplConfigureBlocking(
    /* [in] */ Boolean blocking)
{
    return IoUtils::SetBlocking(mSocket->GetFD(), blocking);
}

ECode ServerSocketChannelImpl::ImplCloseSelectableChannel()
{
    Boolean isflag = FALSE;
    if (mSocket->IsClosed(&isflag), isflag) {
        mSocket->Close();
    }
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
