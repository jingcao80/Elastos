
#ifndef __ELASTOS_NET_DATAGRAMSOCKET_H__
#define __ELASTOS_NET_DATAGRAMSOCKET_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Net.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::Channels::IDatagramChannel;
using Elastos::Net::IDatagramSocket;
using Elastos::Net::IDatagramSocket;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInetAddress;

namespace Elastos {
namespace Net {

/**
 * This class implements a UDP socket for sending and receiving {@code
 * DatagramPacket}. A {@code DatagramSocket} object can be used for both
 * endpoints of a connection for a packet delivery service.
 *
 * @see DatagramPacket
 * @see DatagramSocketImplFactory
 */
class DatagramSocket
    : public Object
    , public IDatagramSocket
    , public ICloseable
{
public:
    CAR_INTERFACE_DECL()

    DatagramSocket();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 aPort);

    CARAPI constructor(
        /* [in] */ Int32 aport,
        /* [in] */ IInetAddress* addr);

    CARAPI constructor(
        /* [in] */ ISocketAddress* localAddr);

    CARAPI constructor(
        /* [in] */ IDatagramSocketImpl* socketImpl);

    CARAPI Close();

    CARAPI OnClose();

    CARAPI Connect(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 aPort);

    CARAPI OnConnect(
        /* [in] */ IInetAddress* remoteAddress,
        /* [in] */ Int32 remotePort);

    CARAPI Disconnect();

    CARAPI OnDisconnect();

    CARAPI CreateSocket(
        /* [in] */ Int32 aPort,
        /* [in] */ IInetAddress* addr);

    CARAPI GetInetAddress(
        /* [out] */ IInetAddress** address);

    CARAPI GetLocalAddress(
        /* [out] */ IInetAddress** address);

    CARAPI GetLocalPort(
        /* [out] */ Int32* port);

    CARAPI GetPort(
        /* [out] */ Int32* port);

    CARAPI_(Boolean) IsMulticastSocket();

    CARAPI GetReceiveBufferSize(
        /* [out] */ Int32* size);

    CARAPI GetSendBufferSize(
        /* [out] */ Int32* size);

    CARAPI GetSoTimeout(
        /* [out] */ Int32* timeout);

    CARAPI Receive(
        /* [in, out] */ IDatagramPacket* pack);

    CARAPI Send(
        /* [in] */ IDatagramPacket* pack);

    CARAPI SetSendBufferSize(
        /* [in] */ Int32 size);

    CARAPI SetReceiveBufferSize(
        /* [in] */ Int32 size);

    CARAPI SetSoTimeout(
        /* [in] */ Int32 timeout);

    static CARAPI SetDatagramSocketImplFactory(
        /* [in] */ IDatagramSocketImplFactory* fac);

    CARAPI CheckOpen();

    CARAPI Bind(
        /* [in] */ ISocketAddress* localAddr);

    CARAPI OnBind(
        /* [in] */ IInetAddress* localAddress,
        /* [in] */ Int32 localPort);

    CARAPI Connect(
        /* [in] */ ISocketAddress* peer);

    CARAPI IsBound(
        /* [out] */ Boolean* isBound);

    CARAPI IsConnected(
        /* [out] */ Boolean* isConnected);

    CARAPI GetRemoteSocketAddress(
        /* [out] */ ISocketAddress** address);

    CARAPI GetLocalSocketAddress(
        /* [out] */ ISocketAddress** address);

    CARAPI SetReuseAddress(
        /* [in] */ Boolean reuse);

    CARAPI GetReuseAddress(
        /* [out] */ Boolean* reuse);

    CARAPI SetBroadcast(
        /* [in] */ Boolean broadcast);

    CARAPI GetBroadcast(
        /* [out] */ Boolean* broadcast);

    CARAPI SetTrafficClass(
        /* [in] */ Int32 value);

    CARAPI GetTrafficClass(
        /* [out] */ Int32* value);

    CARAPI IsClosed(
        /* [out] */ Boolean* isClosed);

    CARAPI GetChannel(
        /* [out] */ IDatagramChannel** channel);

    CARAPI SetNetworkInterface(
        /* [in] */ INetworkInterface* netInterface);

    CARAPI GetFileDescriptor(
        /* [out] */ IFileDescriptor** fd);

private:
    CARAPI CheckPort(
        /* [in] */ Int32 aPort);

    CARAPI EnsureBound();

public:
    static const String TAG;

    AutoPtr<IDatagramSocketImpl> mImpl;

    AutoPtr<IInetAddress> mAddress;

    Int32 mPort;

    static AutoPtr<IDatagramSocketImplFactory> mFactory;
    static Object sLock;

    Boolean mIsBound;

protected:
    Boolean mIsConnected;

    ECode mPendingConnectException;

    Boolean mIsClosed;

    Object mLock;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_DATAGRAMSOCKET_H__
