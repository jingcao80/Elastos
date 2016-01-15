
#ifndef __ELASTOS_NET_CMULTICASTSOCKET_H__
#define __ELASTOS_NET_CMULTICASTSOCKET_H__

#include "_Elastos_Net_CMulticastSocket.h"
#include "DatagramSocket.h"

using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Net {

CarClass(CMulticastSocket)
    , public DatagramSocket
    , public IMulticastSocket
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 aPort);

    CARAPI constructor(
        /* [in] */ ISocketAddress* localAddr);

    CARAPI GetInterface(
        /* [out] */ IInetAddress** address);

    CARAPI GetNetworkInterface(
        /* [out] */ INetworkInterface** networkInterface);

    CARAPI GetTimeToLive(
        /* [out] */ Int32* ttl);

    CARAPI GetTTL(
        /* [out] */ Byte* ttl);

    CARAPI JoinGroup(
        /* [in] */ IInetAddress* groupAddr);

    CARAPI JoinGroup(
        /* [in] */ ISocketAddress* groupAddress,
        /* [in] */ INetworkInterface* netInterface);

    CARAPI LeaveGroup(
        /* [in] */ IInetAddress* groupAddr);

    CARAPI LeaveGroup(
        /* [in] */ ISocketAddress* groupAddress,
        /* [in] */ INetworkInterface* netInterface);

    CARAPI Send(
        /* [in] */ IDatagramPacket* pack,
        /* [in] */ Byte ttl);

    CARAPI SetInterface(
        /* [in] */ IInetAddress* addr);

    CARAPI SetNetworkInterface(
        /* [in] */ INetworkInterface* netInterface);

    CARAPI SetTimeToLive(
        /* [in] */ Int32 ttl);

    CARAPI SetTTL(
        /* [in] */ Byte ttl);

    CARAPI CreateSocket(
        /* [in] */ Int32 aPort,
        /* [in] */ IInetAddress* addr);

    CARAPI GetLoopbackMode(
        /* [out] */ Boolean* isDisabled);

    CARAPI SetLoopbackMode(
        /* [in] */ Boolean isDisabled);

private:
    CARAPI CheckJoinOrLeave(
        /* [in] */ ISocketAddress* groupAddress,
        /* [in] */ INetworkInterface* netInterface);

    CARAPI CheckJoinOrLeave(
        /* [in] */ IInetAddress* groupAddr);
private:
    AutoPtr<IInetAddress> mSetAddress;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CMULTICASTSOCKET_H__
