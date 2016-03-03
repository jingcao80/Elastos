
#include "DatagramSocket.h"
#include "CInet4Address.h"
#include "CDatagramPacket.h"
#include "CInetSocketAddress.h"
#include "CInteger32.h"
#include "CBoolean.h"
#include "CPlainDatagramSocketImpl.h"
#include "AutoLock.h"
#include "CLibcore.h"
#include "CIoBridge.h"
#include "OsConstants.h"
#include <cutils/log.h>

using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::IO::ICloseable;
using Elastos::IO::EIID_ICloseable;
using Elastos::Net::EIID_ISocketOptions;
using Libcore::IO::IOs;
using Libcore::IO::CLibcore;
using Libcore::IO::CIoBridge;
using Droid::System::OsConstants;

namespace Elastos {
namespace Net {

const String DatagramSocket::TAG("DatagramSocket");

AutoPtr<IDatagramSocketImplFactory> DatagramSocket::mFactory;
Object DatagramSocket::sLock;

CAR_INTERFACE_IMPL_2(DatagramSocket, Object, IDatagramSocket, ICloseable)

DatagramSocket::DatagramSocket()
    : mPort(-1)
    , mIsBound(FALSE)
    , mIsConnected(FALSE)
    , mPendingConnectException(NOERROR)
    , mIsClosed(FALSE)
{}

ECode DatagramSocket::constructor()
{
    return constructor(0);
}

ECode DatagramSocket::constructor(
    /* [in] */ Int32 aPort)
{
    FAIL_RETURN(CheckPort(aPort));
    return CreateSocket(aPort, CInet4Address::ANY);
}

ECode DatagramSocket::constructor(
    /* [in] */ Int32 aport,
    /* [in] */ IInetAddress* addr)
{
    FAIL_RETURN(CheckPort(aport));
    return CreateSocket(aport, addr == NULL ? CInet4Address::ANY.Get() : addr);
}

ECode DatagramSocket::constructor(
    /* [in] */ ISocketAddress* localAddr)
{
    if (localAddr != NULL) {
        IInetSocketAddress* netSocketAddr = IInetSocketAddress::Probe(localAddr);
        if (netSocketAddr == NULL) {
            ALOGD("Error in DatagramSocket: Local address not an InetSocketAddress!");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        Int32 port;
        netSocketAddr->GetPort(&port);
        FAIL_RETURN(CheckPort(port));
    }
    if (mFactory != NULL) {
        mFactory->CreateDatagramSocketImpl((IDatagramSocketImpl**)&mImpl);
    }
    else {
       CPlainDatagramSocketImpl::New((IDatagramSocketImpl**)&mImpl);
    }
    mImpl->Create();
    if (localAddr != NULL) {
        // try {
        ECode ec = Bind(localAddr);
        if (FAILED(ec)) {
            Close();
            ALOGD("Error in DatagramSocket: failed to bind socket!");
            return ec;
        }
        // } catch (SocketException e) {
        //    close();
        //    throw e;
        // }
    }
    // SocketOptions.SO_BROADCAST is set by default for DatagramSocket
    return SetBroadcast(TRUE);
}

ECode DatagramSocket::constructor(
    /* [in] */ IDatagramSocketImpl* socketImpl)
{
    VALIDATE_NOT_NULL(socketImpl)

    mImpl = socketImpl;
    return NOERROR;
}

ECode DatagramSocket::CheckPort(
    /* [in] */ Int32 aPort)
{
    if (aPort < 0 || aPort > 65535) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode DatagramSocket::Close()
{
    mIsClosed = TRUE;
    return mImpl->Close();
}

ECode DatagramSocket::OnClose()
{
    mIsClosed = TRUE;
    return mImpl->Close();
}

ECode DatagramSocket::Disconnect()
{
    Boolean isClosed = FALSE;
    Boolean isConnected = FALSE;
    if ((IsClosed(&isClosed), isClosed) || (IsConnected(&isConnected), !isConnected)) {
        return NOERROR;
    }
    mImpl->Disconnect();
    mAddress = NULL;
    mPort = -1;
    mIsConnected = FALSE;
    return NOERROR;
}

ECode DatagramSocket::OnDisconnect()
{
    mAddress = NULL;
    mPort = -1;
    mIsConnected = false;
    mImpl->Disconnect();
    return NOERROR;
}

ECode DatagramSocket::CreateSocket(
    /* [in] */ Int32 aPort,
    /* [in] */ IInetAddress* addr)
{
    synchronized(this) {
        mImpl = NULL;
        if (mFactory != NULL) {
            mFactory->CreateDatagramSocketImpl((IDatagramSocketImpl**)&mImpl);
        }
        else {
            FAIL_RETURN(CPlainDatagramSocketImpl::New((IDatagramSocketImpl**)&mImpl));
        }
        mImpl->Create();

        // try {
        ECode ec = mImpl->Bind(aPort, addr);
        if (FAILED(ec)) {
            Close();
            ALOGD("Error in DatagramSocket: failed to create socket!");
            return ec;
        }
        mIsBound = TRUE;
        // } catch (SocketException e) {
        //    close();
        //    throw e;
        // }
    }

    return NOERROR;
}

ECode DatagramSocket::GetInetAddress(
    /* [out] */ IInetAddress** address)
{
    VALIDATE_NOT_NULL(address);

    *address = mAddress;
    REFCOUNT_ADD(*address);
    return NOERROR;
}

ECode DatagramSocket::GetLocalAddress(
    /* [out] */ IInetAddress** address)
{
    VALIDATE_NOT_NULL(address);

    AutoPtr<IFileDescriptor> fd;
    mImpl->GetFileDescriptor((IFileDescriptor**)&fd);
    return CIoBridge::_GetSocketLocalAddress(fd, address);
}

ECode DatagramSocket::GetLocalPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port);

    Boolean isClosed = FALSE;
    if (IsClosed(&isClosed), isClosed) {
        *port = -1;
        return NOERROR;
    }
    Boolean isBound = FALSE;
    if (IsBound(&isBound), !isBound) {
        *port = 0;
        return NOERROR;
    }
    return mImpl->GetLocalPort(port);
}

ECode DatagramSocket::GetPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port);

    *port = mPort;
    return NOERROR;
}

Boolean DatagramSocket::IsMulticastSocket()
{
    return FALSE;
}

ECode DatagramSocket::GetReceiveBufferSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);

    synchronized(this) {
        FAIL_RETURN(CheckOpen());
        AutoPtr<IInteger32> optVal;
        ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
        FAIL_RETURN(option->GetOption(ISocketOptions::_SO_RCVBUF, (IInterface**)&optVal));
        return optVal->GetValue(size);
    }
    return NOERROR;
}

ECode DatagramSocket::GetSendBufferSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);

    synchronized(this) {
        FAIL_RETURN(CheckOpen());
        AutoPtr<IInteger32> optVal;
        ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
        FAIL_RETURN(option->GetOption(ISocketOptions::_SO_SNDBUF, (IInterface**)&optVal));
        return optVal->GetValue(size);
    }
    return NOERROR;
}

ECode DatagramSocket::GetSoTimeout(
    /* [out] */ Int32* timeout)
{
    VALIDATE_NOT_NULL(timeout);

    synchronized(this) {
        FAIL_RETURN(CheckOpen());
        AutoPtr<IInteger32> optVal;
        ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
        FAIL_RETURN(option->GetOption(ISocketOptions::_SO_TIMEOUT, (IInterface**)&optVal));
        return optVal->GetValue(timeout);
    }
    return NOERROR;
}

ECode DatagramSocket::Receive(
    /* [in] */ IDatagramPacket* pack)
{
    synchronized(this) {
        FAIL_RETURN(CheckOpen());
        FAIL_RETURN(EnsureBound());
        if (pack == NULL) {
            ALOGD("Error in DatagramSocket: pack == null!");
            return E_NULL_POINTER_EXCEPTION;
        }
        if (FAILED(mPendingConnectException)) {
            return E_SOCKET_EXCEPTION;
        }
        pack->ResetLengthForReceive();
        return mImpl->Receive(pack);
    }
    return NOERROR;
}

ECode DatagramSocket::Send(
    /* [in] */ IDatagramPacket* pack)
{
    FAIL_RETURN(CheckOpen());
    FAIL_RETURN(EnsureBound());

    AutoPtr<IInetAddress> packAddr;
    pack->GetAddress((IInetAddress**)&packAddr);
    if (mAddress != NULL) { // The socket is connected
        if (packAddr != NULL) {
            Int32 port;
            pack->GetPort(&port);
            Boolean flag = FALSE;
            IObject* o = (IObject*)mAddress->Probe(EIID_IObject);
            o->Equals(packAddr, &flag);
            if (!flag || mPort != port) {
                ALOGD("Error in DatagramSocket: Packet address mismatch with connected address!");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        else {
            pack->SetAddress(mAddress);
            pack->SetPort(mPort);
        }
    }
    else {
        // not connected so the target address is not allowed to be null
        if (packAddr == NULL) {
            return E_NULL_POINTER_EXCEPTION;
        }
    }
    return mImpl->Send(pack);
}

ECode DatagramSocket::SetNetworkInterface(
    /* [in] */ INetworkInterface* netInterface)
{
    if (netInterface == NULL) {
        ALOGD("Error in DatagramSocket: netInterface == null!");
        return E_NULL_POINTER_EXCEPTION;
    }
    //try {
    AutoPtr<IFileDescriptor> outfd;
    mImpl->GetFileDescriptor((IFileDescriptor**)&outfd);
    String netname;
    netInterface->GetName(&netname);
    ECode ec = CLibcore::sOs->SetsockoptIfreq(outfd, OsConstants::_SOL_SOCKET, OsConstants::_SO_BINDTODEVICE, netname);
    if (FAILED(ec)) {
        return E_SOCKET_EXCEPTION;
    }

    return NOERROR;
    //} catch (ErrnoException errnoException) {
            //throw errnoException.rethrowAsSocketException();
    //}
}

ECode DatagramSocket::SetSendBufferSize(
    /* [in] */ Int32 size)
{
    synchronized(this) {
        if (size < 1) {
            ALOGD("Error in DatagramSocket: send buffer size < 1!");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        FAIL_RETURN(CheckOpen());
        AutoPtr<IInteger32> optVal;
        CInteger32::New(size, (IInteger32**)&optVal);
        ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
        return option->SetOption(ISocketOptions::_SO_SNDBUF, optVal);
    }
    return NOERROR;
}

ECode DatagramSocket::SetReceiveBufferSize(
    /* [in] */ Int32 size)
{
    synchronized(this) {
        if (size < 1) {
            ALOGD("Error in DatagramSocket: receive buffer size < 1!");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        FAIL_RETURN(CheckOpen());
        AutoPtr<IInteger32> optVal;
        CInteger32::New(size, (IInteger32**)&optVal);
        ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
        return option->SetOption(ISocketOptions::_SO_RCVBUF, optVal);
    }
    return NOERROR;
}

ECode DatagramSocket::SetSoTimeout(
    /* [in] */ Int32 timeout)
{
    synchronized(this) {
        if (timeout < 0) {
            ALOGD("Error in DatagramSocket: timeout < 0!");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        FAIL_RETURN(CheckOpen());
        AutoPtr<IInteger32> optVal;
        CInteger32::New(timeout, (IInteger32**)&optVal);
        ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
        return option->SetOption(ISocketOptions::_SO_TIMEOUT, optVal);
        }
    return NOERROR;
}

ECode DatagramSocket::SetDatagramSocketImplFactory(
    /* [in] */ IDatagramSocketImplFactory* fac)
{
    synchronized(sLock) {
        if (mFactory != NULL) {
            ALOGD("Error in DatagramSocket: Factory already set!");
            return E_SOCKET_EXCEPTION;
        }
        mFactory = fac;
    }
    return NOERROR;
}

ECode DatagramSocket::CheckOpen()
{
    Boolean isClosed = FALSE;
    if (IsClosed(&isClosed), isClosed) {
        ALOGD("Error in DatagramSocket: Socket is closed!");
        return E_SOCKET_EXCEPTION;
    }
    return NOERROR;
}

ECode DatagramSocket::EnsureBound()
{
    Boolean isBound = FALSE;
    if (IsBound(&isBound), !isBound) {
        FAIL_RETURN(mImpl->Bind(0, CInet4Address::ANY));
        mIsBound = TRUE;
    }
    return NOERROR;
}

ECode DatagramSocket::Bind(
    /* [in] */ ISocketAddress* localAddr)
{
    FAIL_RETURN(CheckOpen());
    Int32 localPort = 0;
    AutoPtr<IInetAddress> addr = CInet4Address::ANY;
    if (localAddr != NULL) {
        IInetSocketAddress* inetAddr = IInetSocketAddress::Probe(localAddr);
        if (inetAddr == NULL) {
            ALOGD("Error in DatagramSocket: Socket is closedLocal address not an InetSocketAddress!");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        addr = NULL;
        inetAddr->GetAddress((IInetAddress**)&addr);
        if (addr == NULL) {
            String hostName;
            inetAddr->GetHostName(&hostName);
            ALOGD("Error in DatagramSocket: Host is unresolved: %s!", hostName.string());
            return E_SOCKET_EXCEPTION;
        }
        inetAddr->GetPort(&localPort);
        FAIL_RETURN(CheckPort(localPort));
    }
    FAIL_RETURN(mImpl->Bind(localPort, addr));
    mIsBound = TRUE;
    return NOERROR;
}

ECode DatagramSocket::OnBind(
    /* [in] */ IInetAddress* localAddress,
    /* [in] */ Int32 localPort)
{
    mIsBound = TRUE;
    return ((DatagramSocketImpl*)mImpl.Get())->OnBind(localAddress, localPort);
}

ECode DatagramSocket::Connect(
    /* [in] */ ISocketAddress* peer)
{
    if (peer == NULL) {
        ALOGD("Error in DatagramSocket: remoteAddr == null!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    IInetSocketAddress* isa = IInetSocketAddress::Probe(peer);
    if (isa == NULL) {
        ALOGD("Error in DatagramSocket: Remote address not an InetSocketAddress!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInetAddress> addr;
    isa->GetAddress((IInetAddress**)&addr);
    if (addr == NULL) {
        String hostName;
        isa->GetHostName(&hostName);
        ALOGD("Error in DatagramSocket: Host is unresolved: %s!", hostName.string());
        return E_SOCKET_EXCEPTION;
    }

    synchronized(mLock) {
        FAIL_RETURN(CheckOpen());
        FAIL_RETURN(EnsureBound());

        mAddress = addr;
        Int32 port;
        isa->GetPort(&port);
        mPort = port;
        mIsConnected = TRUE;

        return mImpl->Connect(addr, port);
    }
    return NOERROR;
}

ECode DatagramSocket::OnConnect(
    /* [in] */ IInetAddress* remoteAddress,
    /* [in] */ Int32 remotePort)
{
    mIsConnected = true;
    mAddress = remoteAddress;
    mPort = remotePort;
    ((DatagramSocketImpl*)mImpl.Get())->OnConnect(remoteAddress, remotePort);
    return NOERROR;
}

ECode DatagramSocket::Connect(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 port)
{
    if (address == NULL) {
        ALOGD("Error in DatagramSocket: failed to connect, address == null!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    AutoPtr<IInetSocketAddress> addr;
    CInetSocketAddress::New(address, port, (IInetSocketAddress**)&addr);
    mPendingConnectException = Connect((ISocketAddress*)addr->Probe(EIID_ISocketAddress));
    // } catch (SocketException connectException) {
    //     // TODO: or just use SneakyThrow? There's a clear API bug here.
    //     pendingConnectException = connectException;
    // }
    return NOERROR;
}

ECode DatagramSocket::IsBound(
    /* [out] */ Boolean* isBound)
{
    VALIDATE_NOT_NULL(isBound);
    *isBound = mIsBound;
    return NOERROR;
}

ECode DatagramSocket::IsConnected(
    /* [out] */ Boolean* isConnected)
{
    VALIDATE_NOT_NULL(isConnected);
    *isConnected = mIsConnected;
    return NOERROR;
}

ECode DatagramSocket::GetRemoteSocketAddress(
    /* [out] */ ISocketAddress** address)
{
    VALIDATE_NOT_NULL(address);
    *address = NULL;

    Boolean isConnected;
    if (IsConnected(&isConnected), !isConnected) {
        return NOERROR;
    }
    AutoPtr<IInetAddress> addr;
    Int32 port;
    GetInetAddress((IInetAddress**)&addr);
    GetPort(&port);
    return CInetSocketAddress::New(addr, port, address);
}

ECode DatagramSocket::GetLocalSocketAddress(
    /* [out] */ ISocketAddress** address)
{
    VALIDATE_NOT_NULL(address);
    *address = NULL;

    Boolean bval;
    IsClosed(&bval);
    if (bval) return NOERROR;

    IsBound(&bval);
    if (bval) return NOERROR;

    AutoPtr<IInetAddress> addr;
    Int32 port;
    GetLocalAddress((IInetAddress**)&addr);
    GetLocalPort(&port);
    return CInetSocketAddress::New(addr, port, address);
}

ECode DatagramSocket::SetReuseAddress(
    /* [in] */ Boolean reuse)
{
    FAIL_RETURN(CheckOpen());
    AutoPtr<IBoolean> optVal;
    CBoolean::New(reuse, (IBoolean**)&optVal);
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    return option->SetOption(ISocketOptions::_SO_REUSEADDR, optVal);
}

ECode DatagramSocket::GetReuseAddress(
    /* [out] */ Boolean* reuse)
{
    VALIDATE_NOT_NULL(reuse);

    FAIL_RETURN(CheckOpen());
    AutoPtr<IBoolean> optVal;
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    FAIL_RETURN(option->GetOption(ISocketOptions::_SO_REUSEADDR, (IInterface**)&optVal));
    return optVal->GetValue(reuse);
}

ECode DatagramSocket::SetBroadcast(
    /* [in] */ Boolean broadcast)
{
    FAIL_RETURN(CheckOpen());
    AutoPtr<IBoolean> optVal;
    CBoolean::New(broadcast, (IBoolean**)&optVal);
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    return option->SetOption(ISocketOptions::_SO_BROADCAST, optVal);
}

ECode DatagramSocket::GetBroadcast(
    /* [out] */ Boolean* broadcast)
{
    VALIDATE_NOT_NULL(broadcast);

    FAIL_RETURN(CheckOpen());
    AutoPtr<IBoolean> optVal;
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    FAIL_RETURN(option->GetOption(ISocketOptions::_SO_BROADCAST, (IInterface**)&optVal));
    return optVal->GetValue(broadcast);
}

ECode DatagramSocket::SetTrafficClass(
    /* [in] */ Int32 value)
{
    FAIL_RETURN(CheckOpen());
    if (value < 0 || value > 255) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInteger32> optVal;
    CInteger32::New(value, (IInteger32**)&optVal);
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    return option->SetOption(ISocketOptions::_IP_TOS, optVal);
}

ECode DatagramSocket::GetTrafficClass(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    FAIL_RETURN(CheckOpen());
    AutoPtr<IInteger32> optVal;
    ISocketOptions* option = (ISocketOptions*)mImpl->Probe(EIID_ISocketOptions);
    FAIL_RETURN(option->GetOption(ISocketOptions::_IP_TOS, (IInterface**)&optVal));
    return optVal->GetValue(value);
}

ECode DatagramSocket::IsClosed(
    /* [out] */ Boolean* isClosed)
{
    VALIDATE_NOT_NULL(isClosed);
    *isClosed = mIsClosed;
    return NOERROR;
}

ECode DatagramSocket::GetChannel(
    /* [out] */ IDatagramChannel** channel)
{
    VALIDATE_NOT_NULL(channel);
    *channel = NULL;
    return NOERROR;
}

ECode DatagramSocket::GetFileDescriptor(
    /* [out] */ IFileDescriptor** fd)
{
    return mImpl->GetFileDescriptor((IFileDescriptor**)&fd);;
}

} // namespace Net
} // namespace Elastos
