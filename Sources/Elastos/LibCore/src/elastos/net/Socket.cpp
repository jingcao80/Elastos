
#include "Socket.h"
#include "CInet4Address.h"
#include "InetAddress.h"
#include "CInetSocketAddress.h"
#include "CPlainSocketImpl.h"
#include "CInteger32.h"
#include "CBoolean.h"
#include "CIoBridge.h"
#include "AutoLock.h"

using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Net::CInet4Address;
using Elastos::Net::CPlainSocketImpl;
using Elastos::Net::CInetSocketAddress;
using Libcore::IO::CIoBridge;
using Elastos::IO::EIID_ICloseable;

namespace Elastos {
namespace Net {

AutoPtr<ISocketImplFactory> Socket::sFactory;
Object Socket::sLock;

CAR_INTERFACE_IMPL_2(Socket, Object, ISocket, ICloseable)

Socket::Socket()
    : mIsCreated(FALSE)
    , mIsBound(FALSE)
    , mIsConnected(FALSE)
    , mIsClosed(FALSE)
    , mIsInputShutdown(FALSE)
    , mIsOutputShutdown(FALSE)
{
    mLocalAddress = CInet4Address::ANY;
}

ECode Socket::constructor()
{
    if (sFactory != NULL) {
        sFactory->CreateSocketImpl((ISocketImpl**)&mImpl);
    }
    else {
        CPlainSocketImpl::New((ISocketImpl**)&mImpl);
    }
    mProxy = NULL;

    return NOERROR;
}

ECode Socket::constructor(
    /* [in] */ IProxy* proxy)
{
    ProxyType type;
    if (proxy == NULL || (proxy->GetType(&type), type == ProxyType_HTTP)) {
//        throw new IllegalArgumentException("Proxy is null or invalid type");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mProxy = proxy;

    if (sFactory != NULL) {
        sFactory->CreateSocketImpl((ISocketImpl**)&mImpl);
    }
    else {
        CPlainSocketImpl::New(proxy, (ISocketImpl**)&mImpl);
    }

    return NOERROR;
}

ECode Socket::constructor(
    /* [in] */ const String& dstName,
    /* [in] */ Int32 dstPort)
{
    return constructor(dstName, dstPort, NULL, 0);
}

ECode Socket::constructor(
    /* [in] */ const String& dstName,
    /* [in] */ Int32 dstPort,
    /* [in] */ IInetAddress* localAddress,
    /* [in] */ Int32 localPort)
{
    FAIL_RETURN(constructor());

    return TryAllAddresses(dstName, dstPort, localAddress, localPort, TRUE);
}

ECode Socket::constructor(
    /* [in] */ const String& hostName,
    /* [in] */ Int32 port,
    /* [in] */ Boolean streaming)
{
    FAIL_RETURN(constructor());
    return TryAllAddresses(hostName, port, NULL, 0, streaming);
}

ECode Socket::constructor(
    /* [in] */ IInetAddress* dstAddress,
    /* [in] */ Int32 dstPort)
{
    FAIL_RETURN(constructor());
    FAIL_RETURN(CheckDestination(dstAddress, dstPort));
    return StartupSocket(dstAddress, dstPort, NULL, 0, TRUE);
}

ECode Socket::constructor(
    /* [in] */ IInetAddress* dstAddress,
    /* [in] */ Int32 dstPort,
    /* [in] */ IInetAddress* localAddress,
    /* [in] */ Int32 localPort)
{
    FAIL_RETURN(constructor());
    FAIL_RETURN(CheckDestination(dstAddress, dstPort));
    return StartupSocket(dstAddress, dstPort, localAddress, localPort, TRUE);
}

ECode Socket::constructor(
    /* [in] */ IInetAddress* addr,
    /* [in] */ Int32 port,
    /* [in] */ Boolean streaming)
{
    FAIL_RETURN(constructor());
    FAIL_RETURN(CheckDestination(addr, port));
    return StartupSocket(addr, port, NULL, 0, streaming);
}

ECode Socket::TryAllAddresses(
    /* [in] */ const String& dstName,
    /* [in] */ Int32 dstPort,
    /* [in] */ IInetAddress* localAddress,
    /* [in] */ Int32 localPort,
    /* [in] */ Boolean streaming)
{
    AutoPtr<ArrayOf<IInetAddress*> > dstAddresses;

    FAIL_RETURN(InetAddress::GetAllByName(dstName, (ArrayOf<IInetAddress*>**)&dstAddresses));

    // Loop through all the destination addresses except the last, trying to
    // connect to each one and ignoring errors. There must be at least one
    // address, or getAllByName would have thrown UnknownHostException.

    AutoPtr<IInetAddress> dstAddress;
    for (Int32 i = 0; i < dstAddresses->GetLength(); i++) {
        dstAddress = (*dstAddresses)[i];
        // try {
        ECode ec = CheckDestination(dstAddress, dstPort);
        if (SUCCEEDED(ec)) {
            ec = StartupSocket(dstAddress, dstPort, localAddress, localPort, streaming);
            if (SUCCEEDED(ec)) {
                return NOERROR;
            }
        }
        // catch (IOException e2) {
        // }
    }

    // Now try to connect to the last address in the array, handing back to
    // the caller any exceptions that are thrown.
    dstAddress = (*dstAddresses)[dstAddresses->GetLength() - 1];
    FAIL_RETURN(CheckDestination(dstAddress, dstPort));
    return StartupSocket(dstAddress, dstPort, localAddress, localPort, streaming);
}

ECode Socket::CheckDestination(
    /* [in] */ IInetAddress* destAddr,
    /* [in] */ Int32 dstPort)
{
    if (dstPort < 0 || dstPort > 65535) {
        // throw new IllegalArgumentException("Port out of range: " + dstPort);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

ECode Socket::Close()
{
    AutoLock lock(this);

    mIsClosed = TRUE;
    mIsConnected = FALSE;
    // RI compatibility: the RI returns the any address (but the original local port) after close.
    mLocalAddress = CInet4Address::ANY;
    return mImpl->Close();
}

ECode Socket::OnClose()
{
    mIsClosed = TRUE;
    mIsConnected = FALSE;
    // RI compatibility: the RI returns the any address (but the original local port) after
    // close.
    mLocalAddress = CInet4Address::ANY;
    return ((SocketImpl *)mImpl.Get())->OnClose();
}

ECode Socket::GetInetAddress(
    /* [out] */ IInetAddress** address)
{
    VALIDATE_NOT_NULL(address);

    Boolean isConnected;
    if (IsConnected(&isConnected), !isConnected) {
        *address = NULL;
        return NOERROR;
    }
    return mImpl->GetInetAddress(address);
}

ECode Socket::GetInputStream(
    /* [out] */ IInputStream** is)
{
    VALIDATE_NOT_NULL(is);

    FAIL_RETURN(CheckOpenAndCreate(FALSE));
    Boolean isInputShutdown;
    if (IsInputShutdown(&isInputShutdown), isInputShutdown) {
        // throw new SocketException("Socket input is shutdown");
        return E_SOCKET_EXCEPTION;
    }
    return mImpl->GetInputStream(is);
}

ECode Socket::GetKeepAlive(
    /* [out] */ Boolean* keepAlive)
{
    VALIDATE_NOT_NULL(keepAlive);

    FAIL_RETURN(CheckOpenAndCreate(TRUE));
    AutoPtr<IBoolean> optVal;
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    FAIL_RETURN(option->GetOption(ISocketOptions::_SO_KEEPALIVE, (IInterface**)&optVal));
    return optVal->GetValue(keepAlive);
}

ECode Socket::GetLocalAddress(
    /* [out] */ IInetAddress** address)
{
    VALIDATE_NOT_NULL(address);

    *address = mLocalAddress;
    REFCOUNT_ADD(*address);
    return NOERROR;
}

ECode Socket::GetLocalPort(
    /* [out] */ Int32* localPort)
{
    VALIDATE_NOT_NULL(localPort);

    Boolean isBound;
    if (IsBound(&isBound), !isBound) {
        *localPort = -1;
        return NOERROR;
    }
    return mImpl->GetLocalPort(localPort);
}

ECode Socket::GetOutputStream(
    /* [out] */ IOutputStream** os)
{
    VALIDATE_NOT_NULL(os);

    FAIL_RETURN(CheckOpenAndCreate(FALSE));
    Boolean isOutputShutdown;
    if (IsOutputShutdown(&isOutputShutdown), isOutputShutdown) {
        // throw new SocketException("Socket output is shutdown");
        return E_SOCKET_EXCEPTION;
    }
    return mImpl->GetOutputStream(os);
}

ECode Socket::GetPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port);

    Boolean isConnected;
    if (IsConnected(&isConnected), !isConnected) {
        *port = 0;
        return NOERROR;
    }
    return mImpl->GetPort(port);
}

ECode Socket::GetSoLinger(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    FAIL_RETURN(CheckOpenAndCreate(TRUE));
    // The RI explicitly guarantees this idiocy in the SocketOptions.setOption documentation.
    AutoPtr<IInterface> optVal;
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    FAIL_RETURN(option->GetOption(ISocketOptions::_SO_LINGER, (IInterface**)&optVal));
    if (IInteger32::Probe(optVal.Get()) != NULL) {
        return IInteger32::Probe(optVal.Get())->GetValue(value);
    }
    else {
        *value = -1;
        return NOERROR;
    }
}

ECode Socket::GetReceiveBufferSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);

    AutoLock lock(this);

    FAIL_RETURN(CheckOpenAndCreate(TRUE));
    AutoPtr<IInteger32> optVal;
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    FAIL_RETURN(option->GetOption(ISocketOptions::_SO_RCVBUF, (IInterface**)&optVal));
    return optVal->GetValue(size);
}

ECode Socket::GetSendBufferSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);

    AutoLock lock(this);

    FAIL_RETURN(CheckOpenAndCreate(TRUE));
    AutoPtr<IInteger32> optVal;
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    FAIL_RETURN(option->GetOption(ISocketOptions::_SO_SNDBUF, (IInterface**)&optVal));
    return optVal->GetValue(size);
}

ECode Socket::GetSoTimeout(
    /* [out] */ Int32* timeout)
{
    VALIDATE_NOT_NULL(timeout);

    AutoLock lock(this);

    FAIL_RETURN(CheckOpenAndCreate(TRUE));
    AutoPtr<IInteger32> optVal;
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    FAIL_RETURN(option->GetOption(ISocketOptions::_SO_TIMEOUT, (IInterface**)&optVal));
    return optVal->GetValue(timeout);
}

ECode Socket::GetTcpNoDelay(
    /* [out] */ Boolean* noDelay)
{
    VALIDATE_NOT_NULL(noDelay);

    FAIL_RETURN(CheckOpenAndCreate(TRUE));
    AutoPtr<IInterface> ouface;
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    FAIL_RETURN(option->GetOption(ISocketOptions::_TCP_NODELAY, (IInterface**)&ouface));
    AutoPtr<IBoolean> optVal = IBoolean::Probe(ouface);
    if (optVal != NULL) {
        return optVal->GetValue(noDelay);
    }
    return E_SOCKET_EXCEPTION;
}

ECode Socket::SetKeepAlive(
    /* [in] */ Boolean keepAlive)
{
    if (mImpl != NULL) {
        FAIL_RETURN(CheckOpenAndCreate(TRUE));
        AutoPtr<IBoolean> optVal;
        CBoolean::New(keepAlive, (IBoolean**)&optVal);
        ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
        return option->SetOption(ISocketOptions::_SO_KEEPALIVE, optVal.Get());
    }
    return NOERROR;
}

ECode Socket::SetSocketImplFactory(
    /* [in] */ ISocketImplFactory* fac)
{
    AutoLock lock(&sLock);

    if (sFactory != NULL) {
        // throw new SocketException("Factory already set");
        return E_SOCKET_EXCEPTION;
    }
    sFactory = fac;

    return NOERROR;
}

ECode Socket::SetSendBufferSize(
    /* [in] */ Int32 size)
{
    AutoLock lock(this);

    FAIL_RETURN(CheckOpenAndCreate(TRUE));
    if (size < 1) {
        // throw new IllegalArgumentException("size < 1");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInteger32> optVal;
    CInteger32::New(size, (IInteger32**)&optVal);
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    return option->SetOption(ISocketOptions::_SO_SNDBUF, optVal.Get());
}

ECode Socket::SetReceiveBufferSize(
    /* [in] */ Int32 size)
{
    AutoLock lock(this);

    FAIL_RETURN(CheckOpenAndCreate(TRUE));
    if (size < 1) {
        // throw new IllegalArgumentException("size < 1");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInteger32> optVal;
    CInteger32::New(size, (IInteger32**)&optVal);
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    return option->SetOption(ISocketOptions::_SO_RCVBUF, optVal.Get());
}

ECode Socket::SetSoLinger(
    /* [in] */ Boolean on,
    /* [in] */ Int32 timeout)
{
    FAIL_RETURN(CheckOpenAndCreate(TRUE));
    if (on && timeout < 0) {
        // throw new IllegalArgumentException("timeout < 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (on) {
        AutoPtr<IInteger32> optVal;
        CInteger32::New(timeout, (IInteger32**)&optVal);
        ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
        return option->SetOption(ISocketOptions::_SO_LINGER, optVal.Get());
    }
    else {
        AutoPtr<IBoolean> optVal;
        CBoolean::New(FALSE, (IBoolean**)&optVal);
        ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
        return option->SetOption(ISocketOptions::_SO_LINGER, optVal.Get());
    }
}

ECode Socket::SetSoTimeout(
    /* [in] */ Int32 timeout)
{
    AutoLock lock(this);

    FAIL_RETURN(CheckOpenAndCreate(TRUE));
    if (timeout < 0) {
        // throw new IllegalArgumentException("timeout < 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInteger32> optVal;
    CInteger32::New(timeout, (IInteger32**)&optVal);
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    return option->SetOption(ISocketOptions::_SO_TIMEOUT, optVal.Get());
}

ECode Socket::SetTcpNoDelay(
    /* [in] */ Boolean on)
{
    FAIL_RETURN(CheckOpenAndCreate(TRUE));
    AutoPtr<IBoolean> optVal;
    CBoolean::New(on, (IBoolean**)&optVal);
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    return option->SetOption(ISocketOptions::_TCP_NODELAY, optVal.Get());
}

ECode Socket::StartupSocket(
    /* [in] */ IInetAddress* dstAddress,
    /* [in] */ Int32 dstPort,
    /* [in] */ IInetAddress* localAddress,
    /* [in] */ Int32 localPort,
    /* [in] */ Boolean streaming)
{
    if (localPort < 0 || localPort > 65535) {
        // throw new IllegalArgumentException("Local port out of range: " + localPort);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    IInetAddress* addr = localAddress == NULL ? CInet4Address::ANY.Get() : localAddress;

    AutoLock lock(this);
    mImpl->Create(streaming);

    mIsCreated = TRUE;
    // try {
    ECode ec = NOERROR;
    if (!streaming || !UsingSocks()) {
        ec = mImpl->Bind(addr, localPort);
        if (FAILED(ec)) {
            mImpl->Close();
            return ec;
        }
    }

    mIsBound = TRUE;
    CacheLocalAddress();
    ec = mImpl->Connect(dstAddress, dstPort);
    if (FAILED(ec)) {
        mImpl->Close();
        return ec;
    }

    mIsConnected = TRUE;
    CacheLocalAddress();
    // } catch (IOException e) {
    //    impl.close();
    //    throw e;
    // }
    return ec;
}

Boolean Socket::UsingSocks()
{
    Int32 type;
    return mProxy != NULL && (mProxy->GetType(&type), type == ProxyType_SOCKS);
}

ECode Socket::ShutdownInput()
{
    Boolean isInputShutdown;
    if (IsInputShutdown(&isInputShutdown), isInputShutdown) {
        // throw new SocketException("Socket input is shutdown");
        return E_SOCKET_EXCEPTION;
    }
    FAIL_RETURN(CheckOpenAndCreate(FALSE));
    FAIL_RETURN(mImpl->ShutdownInput());
    mIsInputShutdown = TRUE;
    return NOERROR;
}

ECode Socket::ShutdownOutput()
{
    Boolean isOutputShutdown;
    if (IsOutputShutdown(&isOutputShutdown), isOutputShutdown) {
        // throw new SocketException("Socket output is shutdown");
        return E_SOCKET_EXCEPTION;
    }
    FAIL_RETURN(CheckOpenAndCreate(FALSE));
    FAIL_RETURN(mImpl->ShutdownOutput());
    mIsOutputShutdown = TRUE;
    return NOERROR;
}

ECode Socket::CheckOpenAndCreate(
    /* [in] */ Boolean create)
{
    Boolean isClosed;
    if (IsClosed(&isClosed), isClosed) {
        // throw new SocketException("Socket is closed");
        return E_SOCKET_EXCEPTION;
    }
    if (!create) {
        Boolean isConnected;
        if (IsConnected(&isConnected), !isConnected) {
            // throw new SocketException("Socket is not connected");
            return E_SOCKET_EXCEPTION;
            // a connected socket must be created
        }

        /*
         * return directly to fix a possible bug, if !create, should return
         * here
         */
        return NOERROR;
    }
    if (mIsCreated) {
        return NOERROR;
    }

    AutoLock lock(this);

    if (mIsCreated) {
        return NOERROR;
    }
    // try {
    ECode ec = mImpl->Create(TRUE);
    if (FAILED(ec)) {
        return E_SOCKET_EXCEPTION;
    }
    // } catch (SocketException e) {
    //    throw e;
    // } catch (IOException e) {
    //    throw new SocketException(e.toString());
    // }
    mIsCreated = TRUE;
    return NOERROR;
}

ECode Socket::GetLocalSocketAddress(
    /* [out] */ ISocketAddress** address)
{
    VALIDATE_NOT_NULL(address);

    Boolean isBound;
    if (IsBound(&isBound), !isBound) {
        *address = NULL;
        return NOERROR;
    }
    AutoPtr<IInetAddress> localAddress;
    Int32 localPort;
    GetLocalAddress((IInetAddress**)&localAddress);
    GetLocalPort(&localPort);
    return CInetSocketAddress::New(localAddress, localPort, address);
}

ECode Socket::GetRemoteSocketAddress(
    /* [out] */ ISocketAddress** address)
{
    VALIDATE_NOT_NULL(address);

    Boolean isConnected;
    if (IsConnected(&isConnected), !isConnected) {
        *address = NULL;
        return NOERROR;
    }
    AutoPtr<IInetAddress> netAddress;
    Int32 port;
    GetInetAddress((IInetAddress**)&netAddress);
    GetPort(&port);
    return CInetSocketAddress::New(netAddress, port, address);
}

ECode Socket::IsBound(
    /* [out] */ Boolean* isBound)
{
    VALIDATE_NOT_NULL(isBound);

    *isBound = mIsBound;
    return NOERROR;
}

ECode Socket::IsConnected(
    /* [out] */ Boolean* isConnected)
{
    VALIDATE_NOT_NULL(isConnected);

    *isConnected = mIsConnected;
    return NOERROR;
}

ECode Socket::IsClosed(
    /* [out] */ Boolean* isClosed)
{
    VALIDATE_NOT_NULL(isClosed);

    *isClosed = mIsClosed;
    return NOERROR;
}

ECode Socket::Bind(
    /* [in] */ ISocketAddress* localAddr)
{
    FAIL_RETURN(CheckOpenAndCreate(TRUE));
    Boolean isBound;
    if (IsBound(&isBound), isBound) {
        // throw new BindException("Socket is already bound");
        return E_BIND_EXCEPTION;
    }

    Int32 port = 0;
    AutoPtr<IInetAddress> addr = CInet4Address::ANY;
    if (localAddr != NULL) {
        if (IInetSocketAddress::Probe(localAddr) == NULL) {
            // throw new IllegalArgumentException("Local address not an InetSocketAddress: " +
            //        localAddr.getClass());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        IInetSocketAddress* inetAddr = IInetSocketAddress::Probe(localAddr);
        addr = NULL;
        inetAddr->GetAddress((IInetAddress**)&addr);
        if (addr == NULL) {
            // throw new SocketException("Host is unresolved: " + inetAddr.getHostName());
            return E_SOCKET_EXCEPTION;
        }
        inetAddr->GetPort(&port);
    }

    AutoLock lock(this);

    // try {
    ECode ec = mImpl->Bind(addr, port);
    FAIL_GOTO(ec ,_EXIT_)

    mIsBound = TRUE;
    CacheLocalAddress();
    return NOERROR;
    // } catch (IOException e) {
    //    impl.close();
    //    throw e;
    // }
_EXIT_:

    mImpl->Close();
    return ec;
}

ECode Socket::OnBind(
    /* [in] */ IInetAddress* localAddress,
    /* [in] */ Int32 localPort)
{
    mIsBound = true;
    mLocalAddress = localAddress;
    return ((SocketImpl*)mImpl.Get())->OnBind(localAddress, localPort);
}

ECode Socket::Connect(
    /* [in] */ ISocketAddress* remoteAddr)
{
    return Connect(remoteAddr, 0);
}

ECode Socket::Connect(
    /* [in] */ ISocketAddress* remoteAddr,
    /* [in] */ Int32 timeout)
{
    FAIL_RETURN(CheckOpenAndCreate(TRUE));
    if (timeout < 0) {
        // throw new IllegalArgumentException("timeout < 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean isConnected;
    if (IsConnected(&isConnected), isConnected) {
        // throw new SocketException("Already connected");
        return E_SOCKET_EXCEPTION;
    }
    if (remoteAddr == NULL) {
        // throw new IllegalArgumentException("remoteAddr == null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (IInetSocketAddress::Probe(remoteAddr) == NULL) {
        // throw new IllegalArgumentException("Remote address not an InetSocketAddress: " +
        //             remoteAddr.getClass());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;;
    }
    IInetSocketAddress* inetAddr = IInetSocketAddress::Probe(remoteAddr);
    AutoPtr<IInetAddress> addr;
    inetAddr->GetAddress((IInetAddress**)&addr);
    if (addr == NULL) {
        // throw new SocketException("Host is unresolved: " + inetAddr.getHostName());
        return E_SOCKET_EXCEPTION;
    }
    Int32 port;
    inetAddr->GetPort(&port);

    FAIL_RETURN(CheckDestination(addr, port));

    AutoLock lock(mConnectLock);
    // try {
    ECode ec = NOERROR;
    Boolean isBound;
    if (IsBound(&isBound), !isBound) {
        // socket already created at this point by earlier call or
        // checkOpenAndCreate this caused us to lose socket
        // options on create
        // impl.create(true);
        if (!UsingSocks()) {
            ec = mImpl->Bind(CInet4Address::ANY, 0);
            if (FAILED(ec)) {
                mImpl->Close();
                return ec;
            }
        }
        mIsBound = TRUE;
    }
    ec = mImpl->Connect(remoteAddr, timeout);
    if (FAILED(ec)) {
        mImpl->Close();
        return ec;
    }
    mIsConnected = TRUE;
    CacheLocalAddress();
    // } catch (IOException e) {
    //    impl.close();
    //    throw e;
    // }
    return NOERROR;
}

ECode Socket::OnConnect(
    /* [in] */ ISocketAddress* remoteAddress,
    /* [in] */ Int32 remotePort)
{
    mIsConnected = TRUE;
    return ((SocketImpl*)mImpl.Get())->OnConnect(IInetAddress::Probe(remoteAddress), remotePort);
}

ECode Socket::IsInputShutdown(
    /* [out] */ Boolean* isInputShutdown)
{
    VALIDATE_NOT_NULL(isInputShutdown);

    *isInputShutdown = mIsInputShutdown;
    return NOERROR;
}

ECode Socket::IsOutputShutdown(
    /* [out] */ Boolean* isOutputShutdown)
{
    VALIDATE_NOT_NULL(isOutputShutdown);

    *isOutputShutdown = mIsOutputShutdown;
    return NOERROR;
}

ECode Socket::SetReuseAddress(
    /* [in] */ Boolean reuse)
{
    FAIL_RETURN(CheckOpenAndCreate(TRUE));
    AutoPtr<IBoolean> optVal;
    CBoolean::New(reuse, (IBoolean**)&optVal);
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    return option->SetOption(ISocketOptions::_SO_REUSEADDR, optVal.Get());
}

ECode Socket::GetReuseAddress(
    /* [out] */ Boolean* reuse)
{
    VALIDATE_NOT_NULL(reuse);

    FAIL_RETURN(CheckOpenAndCreate(TRUE));
    AutoPtr<IBoolean> optVal;
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    FAIL_RETURN(option->GetOption(ISocketOptions::_SO_REUSEADDR, (IInterface**)&optVal));
    return optVal->GetValue(reuse);
}

ECode Socket::SetOOBInline(
    /* [in] */ Boolean oobinline)
{
    FAIL_RETURN(CheckOpenAndCreate(TRUE));
    AutoPtr<IBoolean> optVal;
    CBoolean::New(oobinline, (IBoolean**)&optVal);
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    return option->SetOption(ISocketOptions::_SO_OOBINLINE, optVal.Get());
}

ECode Socket::GetOOBInline(
    /* [out] */ Boolean* oobinline)
{
    VALIDATE_NOT_NULL(oobinline);

    FAIL_RETURN(CheckOpenAndCreate(TRUE));
    AutoPtr<IBoolean> optVal;
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    FAIL_RETURN(option->GetOption(ISocketOptions::_SO_OOBINLINE, (IInterface**)&optVal));
    return optVal->GetValue(oobinline);
}

ECode Socket::SetTrafficClass(
    /* [in] */ Int32 value)
{
    FAIL_RETURN(CheckOpenAndCreate(TRUE));
    if (value < 0 || value > 255) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInteger32> optVal;
    CInteger32::New(value, (IInteger32**)&optVal);
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    return option->SetOption(ISocketOptions::_IP_TOS, optVal.Get());
}

ECode Socket::GetTrafficClass(
    /* [out] */ Int32* value)
{
    FAIL_RETURN(CheckOpenAndCreate(TRUE));
    AutoPtr<IInteger32> optVal;
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    FAIL_RETURN(option->GetOption(ISocketOptions::_IP_TOS, (IInterface**)&optVal));
    return optVal->GetValue(value);
}

ECode Socket::SendUrgentData(
    /* [in] */ Int32 value)
{
    return mImpl->SendUrgentData(value);
}

void Socket::Accepted()
{
    mIsCreated = mIsBound = mIsConnected = TRUE;
    CacheLocalAddress();
}

void Socket::CacheLocalAddress()
{
    AutoPtr<IFileDescriptor> fd;
    mImpl->GetFileDescriptor((IFileDescriptor**)&fd);
    CIoBridge::_GetSocketLocalAddress(fd, (IInetAddress**)&mLocalAddress);
}

ECode Socket::GetChannel(
    /* [out] */ ISocketChannel** channel)
{
    VALIDATE_NOT_NULL(channel)
    *channel = NULL;
    return NOERROR;
}

ECode Socket::GetFileDescriptor(
    /* [out] */ IFileDescriptor** fd)
{
    return mImpl->GetFileDescriptor(fd);
}

ECode Socket::SetPerformancePreferences(
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
