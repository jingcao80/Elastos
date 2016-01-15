
#include "ServerSocket.h"
#include "CInet4Address.h"
#include "CInetSocketAddress.h"
#include "CPlainServerSocketImpl.h"
#include "CPlainSocketImpl.h"
#include "CInteger32.h"
#include "CBoolean.h"
#include "CSocket.h"
#include "AutoLock.h"
#include "CIoBridge.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::IO::EIID_ICloseable;
using Elastos::Net::ISocketImpl;
using Elastos::Net::CSocket;
using Elastos::Net::CInet4Address;
using Elastos::Net::CPlainSocketImpl;
using Elastos::Net::CPlainServerSocketImpl;
using Libcore::IO::CIoBridge;

namespace Elastos {
namespace Net {

AutoPtr<ISocketImplFactory> ServerSocket::sFactory;
Object ServerSocket::sLock;

const Int32 ServerSocket::DEFAULT_BACKLOG = 50;

CAR_INTERFACE_IMPL_2(ServerSocket, Object, IServerSocket, ICloseable)

ServerSocket::ServerSocket()
    : mIsBound(FALSE)
    , mIsClosed(FALSE)
{}

ECode ServerSocket::constructor()
{
    if (sFactory != NULL) {
        return sFactory->CreateSocketImpl((ISocketImpl**)&mImpl);
    }
    else {
        AutoPtr<CPlainServerSocketImpl> pss;
        FAIL_RETURN(CPlainServerSocketImpl::NewByFriend((CPlainServerSocketImpl**)&pss));
        mImpl = (ISocketImpl*) pss->Probe(EIID_ISocketImpl);
        return NOERROR;
    }
}

ECode ServerSocket::constructor(
    /* [in] */ Int32 aPort)
{
    return constructor(aPort, DEFAULT_BACKLOG, CInet4Address::ANY);
}

ECode ServerSocket::constructor(
    /* [in] */ Int32 aPort,
    /* [in] */ Int32 backlog)
{
    return constructor(aPort, backlog, CInet4Address::ANY);
}

ECode ServerSocket::constructor(
    /* [in] */ Int32 aPort,
    /* [in] */ Int32 backlog,
    /* [in] */ IInetAddress* localAddr)
{
    FAIL_RETURN(CheckListen(aPort));
    ECode ec = NOERROR;
    if (sFactory != NULL) {
        ec = sFactory->CreateSocketImpl((ISocketImpl**)&mImpl);
        if (FAILED(ec)) {
            return ec;
        }
    }
    else {
        AutoPtr<CPlainServerSocketImpl> pss;
        ec = CPlainServerSocketImpl::NewByFriend((CPlainServerSocketImpl**)&pss);
        if (FAILED(ec)) {
            return ec;
        }
        mImpl = (ISocketImpl*)pss->Probe(EIID_ISocketImpl);
    }

    AutoPtr<IInetAddress> addr = localAddr == NULL ? CInet4Address::ANY.Get() : localAddr;

    synchronized (this) {
        mImpl->Create(TRUE);
    //    try {
        ec = mImpl->Bind(addr, aPort);
        FAIL_GOTO(ec, _EXIT_)

        ec = ReadBackBindState();
        FAIL_GOTO(ec, _EXIT_)

        mIsBound = TRUE;
        ec = mImpl->Listen(backlog > 0 ? backlog : DEFAULT_BACKLOG);
        FAIL_GOTO(ec, _EXIT_)
    }

_EXIT_:

    Close();

//    } catch (IOException e) {
//        close();
//        throw e;
//    }

    return ec;
}

ECode ServerSocket::ReadBackBindState()
{
    AutoPtr<IFileDescriptor> fd;
    FAIL_RETURN(mImpl->GetFileDescriptor((IFileDescriptor**)&fd))
    mLocalAddress = NULL;
    FAIL_RETURN(CIoBridge::_GetSocketLocalAddress(fd, (IInetAddress**)&mLocalAddress))
    mIsBound = true;
    return NOERROR;
}

ECode ServerSocket::GetImpl(
    /* [out] */ ISocketImpl** impl)
{
    VALIDATE_NOT_NULL(impl);
    *impl = mImpl;
    REFCOUNT_ADD(*impl);
    return NOERROR;
}

ECode ServerSocket::Accept(
    /* [out] */ ISocket** socket)
{
    VALIDATE_NOT_NULL(socket);
    *socket = NULL;

    FAIL_RETURN(CheckOpen());

    Boolean isBound;
    if (IsBound(&isBound), !isBound) {
        // throw new SocketException("Socket is not bound");
        return E_SOCKET_EXCEPTION;
    }

    AutoPtr<ISocket> socketObj;
    FAIL_RETURN(CSocket::New((ISocket**)&socketObj));
    // try {
    ECode ec = ImplAccept(socketObj);
    if (FAILED(ec)) {
        socketObj->Close();
        *socket = NULL;
        return ec;
    }
    // } catch (SecurityException e) {
    //    aSocket.close();
    //    throw e;
    // } catch (IOException e) {
    //    aSocket.close();
    //    throw e;
    // }
    *socket = socketObj;
    REFCOUNT_ADD(*socket);
    return NOERROR;
}

ECode ServerSocket::CheckListen(
    /* [in] */ Int32 aPort)
{
    if (aPort < 0 || aPort > 65535) {
//        throw new IllegalArgumentException("Port out of range: " + aPort);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode ServerSocket::Close()
{
    mIsClosed = TRUE;
    return mImpl->Close();
}

ECode ServerSocket::GetInetAddress(
    /* [out] */ IInetAddress** address)
{
    VALIDATE_NOT_NULL(address);

    Boolean isBound;
    *address = NULL;

    if (IsBound(&isBound), !isBound) {
        return NOERROR;
    }

    *address = mLocalAddress;
    REFCOUNT_ADD(*address)
    return NOERROR;
}

ECode ServerSocket::GetLocalPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port);

    Boolean isBound;
    if (IsBound(&isBound), !isBound) {
        *port = -1;
        return NOERROR;
    }
    return mImpl->GetLocalPort(port);
}

ECode ServerSocket::GetSoTimeout(
    /* [out] */ Int32* timeout)
{
    VALIDATE_NOT_NULL(timeout);
    *timeout = 0;

    AutoLock lock(this);

    FAIL_RETURN(CheckOpen());

    AutoPtr<IInteger32> optVal;
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    FAIL_RETURN(option->GetOption(ISocketOptions::_SO_TIMEOUT, (IInterface**)&optVal));
    return optVal->GetValue(timeout);
}

ECode ServerSocket::ImplAccept(
    /* [in] */ ISocket* aSocket)
{
    AutoLock lock(this);
    mImpl->Accept(((CSocket*)aSocket)->mImpl);
    ((CSocket*)aSocket)->Accepted();

    return NOERROR;
}

ECode ServerSocket::SetSocketFactory(
    /* [in] */ ISocketImplFactory* aFactory)
{
    AutoLock lock(&sLock);

    if (sFactory != NULL) {
//        throw new SocketException("Factory already set");
        return E_SOCKET_EXCEPTION;
    }
    sFactory = aFactory;
    return NOERROR;
}

ECode ServerSocket::SetSoTimeout(
    /* [in] */ Int32 timeout)
{
    AutoLock lock(this);

    FAIL_RETURN(CheckOpen())
    if (timeout < 0) {
//        throw new IllegalArgumentException("timeout < 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInteger32> optVal;
    CInteger32::New(timeout, (IInteger32**)&optVal);
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    return option->SetOption(ISocketOptions::_SO_TIMEOUT, optVal);
}

ECode ServerSocket::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info)
    StringBuilder result(64);
    result.Append("ServerSocket[");
    Boolean bval;
    if (IsBound(&bval), !bval) {
        result.Append("unbound]");
        *info = result.ToString();
        return NOERROR;
    }

    result.Append("addr=");
    AutoPtr<IInetAddress> ia;
    GetInetAddress((IInetAddress**)&ia);
    String hostname, address;
    ia->GetHostName(&hostname);
    ia->GetHostAddress(&address);
    result.Append(hostname);
    result.Append("/");
    result.Append(address);
    result.Append(",port=0,localport=");
    Int32 port;
    GetLocalPort(&port);
    result.Append(port);
    result.Append("]");
    *info = result.ToString();
    return NOERROR;
}

ECode ServerSocket::Bind(
    /* [in] */ ISocketAddress* localAddr)
{
    return Bind(localAddr, DEFAULT_BACKLOG);
}

ECode ServerSocket::Bind(
    /* [in] */ ISocketAddress* localAddr,
    /* [in] */ Int32 backlog)
{
    FAIL_RETURN(CheckOpen());
    Boolean isBound;
    if (IsBound(&isBound), isBound) {
//        throw new BindException("Socket is already bound");
        return E_BIND_EXCEPTION;
    }

    Int32 port = 0;
    AutoPtr<IInetAddress> addr = CInet4Address::ANY;
    if (localAddr != NULL) {
        if (IInetSocketAddress::Probe(localAddr) == NULL) {
//            throw new IllegalArgumentException("Local address not an InetSocketAddress: " +
//                    localAddr.getClass());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        IInetSocketAddress* inetAddr = IInetSocketAddress::Probe(localAddr);
        addr = NULL;
        inetAddr->GetAddress((IInetAddress**)&addr);
        if (addr == NULL) {
//            throw new SocketException("Host is unresolved: " + inetAddr.getHostName());
            return E_SOCKET_EXCEPTION;
        }
        inetAddr->GetPort(&port);
    }

    AutoLock lock(this);
//    try {
    ECode ec = mImpl->Bind(addr, port);
    FAIL_GOTO(ec, _EXIT_)

    mIsBound = TRUE;
    ec = ReadBackBindState();
    FAIL_GOTO(ec, _EXIT_)

    ec = mImpl->Listen(backlog > 0 ? backlog : DEFAULT_BACKLOG);
    FAIL_GOTO(ec, _EXIT_)
    // } catch (IOException e) {
    //     close();
    //     throw e;
    // }
_EXIT_:
    Close();
    return ec;
}

ECode ServerSocket::GetLocalSocketAddress(
    /* [out] */ ISocketAddress** address)
{
    VALIDATE_NOT_NULL(address);

    Boolean isBound;
    if (IsBound(&isBound), !isBound) {
        *address = NULL;
        return NOERROR;
    }

    AutoPtr<IInetAddress> addr;
    FAIL_RETURN(GetInetAddress((IInetAddress**)&addr));
    Int32 port;
    FAIL_RETURN(GetLocalPort(&port));
    return CInetSocketAddress::New(addr, port, (IInetSocketAddress**)address);
}

ECode ServerSocket::IsBound(
    /* [out] */ Boolean* isBound)
{
    VALIDATE_NOT_NULL(isBound);
    *isBound = mIsBound;
    return NOERROR;
}

ECode ServerSocket::IsClosed(
    /* [out] */ Boolean* isClosed)
{
    VALIDATE_NOT_NULL(isClosed);
    *isClosed = mIsClosed;
    return NOERROR;
}

ECode ServerSocket::CheckOpen()
{
    Boolean isClosed;
    if (IsClosed(&isClosed), isClosed) {
//        throw new SocketException("Socket is closed");
        return E_SOCKET_EXCEPTION;
    }

    return NOERROR;
}

ECode ServerSocket::SetReuseAddress(
    /* [in] */ Boolean reuse)
{
    FAIL_RETURN(CheckOpen())
    AutoPtr<IBoolean> optVal;
    CBoolean::New(reuse, (IBoolean**)&optVal);
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    return option->SetOption(ISocketOptions::_SO_REUSEADDR, optVal);
}

ECode ServerSocket::GetReuseAddress(
    /* [out] */ Boolean* reuse)
{
    VALIDATE_NOT_NULL(reuse);

    FAIL_RETURN(CheckOpen())
    AutoPtr<IBoolean> optVal;
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    FAIL_RETURN(option->GetOption(ISocketOptions::_SO_REUSEADDR, (IInterface**)&optVal));
    return optVal->GetValue(reuse);
}

ECode ServerSocket::SetReceiveBufferSize(
    /* [in] */ Int32 size)
{
    FAIL_RETURN(CheckOpen())
    if (size < 1) {
//        throw new IllegalArgumentException("size < 1");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInteger32> optVal;
    CInteger32::New(size, (IInteger32**)&optVal);
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    return option->SetOption(ISocketOptions::_SO_RCVBUF, optVal);
}

ECode ServerSocket::GetReceiveBufferSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);

    FAIL_RETURN(CheckOpen())
    AutoPtr<IInteger32> optVal;
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    FAIL_RETURN(option->GetOption(ISocketOptions::_SO_RCVBUF, (IInterface**)&optVal));
    return optVal->GetValue(size);
}

ECode ServerSocket::GetChannel(
    /* [out] */ IServerSocketChannel** channel)
{
    VALIDATE_NOT_NULL(channel);

    *channel = NULL;
    return NOERROR;
}

ECode ServerSocket::SetPerformancePreferences(
    /* [in] */ Int32 connectionTime,
    /* [in] */ Int32 latency,
    /* [in] */ Int32 bandwidth)
{
    // Our socket implementation only provide one protocol: TCP/IP, so
    // we do nothing for this method
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
