#ifndef __ELASTOS_NET_CPLAINDATAGRAMSOCKETIMPL_H__
#define __ELASTOS_NET_CPLAINDATAGRAMSOCKETIMPL_H__

#include "_Elastos_Net_CPlainDatagramSocketImpl.h"
#include "DatagramSocketImpl.h"

using Elastos::Core::ICloseGuard;
using Elastos::IO::IFileDescriptor;
using Elastos::Droid::System::IStructGroupReq;

namespace Elastos {
namespace Net {

CarClass(CPlainDatagramSocketImpl)
    , public DatagramSocketImpl
    , public IPlainDatagramSocketImpl
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CPlainDatagramSocketImpl();

    ~CPlainDatagramSocketImpl();

    CARAPI constructor(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 localPort);

    CARAPI constructor();

    CARAPI Bind(
        /* [in] */ Int32 port,
        /* [in] */ IInetAddress* address);

    CARAPI OnBind(
        /* [in] */ IInetAddress* inetAddr,
        /* [in] */ Int32 port);

    CARAPI Close();

    CARAPI OnClose();

    CARAPI Create();

    CARAPI GetOption(
        /* [in] */ Int32 option,
        /* [out] */ IInterface** result);

    CARAPI GetTimeToLive(
        /* [out] */ Int32* timeToLive);

    CARAPI GetTTL(
        /* [out] */ Byte* byte);

    CARAPI Join(
        /* [in] */ IInetAddress* addr);

    CARAPI JoinGroup(
        /* [in] */ ISocketAddress* addr,
        /* [in] */ INetworkInterface* netInterface);

    CARAPI Leave(
        /* [in] */ IInetAddress* addr);

    CARAPI LeaveGroup(
        /* [in] */ ISocketAddress* addr,
        /* [in] */ INetworkInterface* netInterface);

    CARAPI Receive(
        /* [in] */ IDatagramPacket* pack);

    CARAPI PeekData(
        /* [in] */ IDatagramPacket* pack,
        /* [out] */ Int32* result);

    CARAPI Send(
        /* [in] */ IDatagramPacket* packet);

    CARAPI SetOption(
        /* [in] */ Int32 option,
        /* [in] */ IInterface* value);

    CARAPI SetTimeToLive(
        /* [in] */ Int32 ttl);

    CARAPI SetTTL(
        /* [in] */ Byte ttl);

    CARAPI Connect(
        /* [in] */ IInetAddress* inetAddr,
        /* [in] */ Int32 port);

    CARAPI OnConnect(
        /* [in] */ IInetAddress* inetAddr,
        /* [in] */ Int32 port);

    CARAPI Disconnect();

    CARAPI OnDisconnect();

protected:
    CARAPI Peek(
        /* [in] */ IInetAddress* sender,
        /* [out] */ Int32* result);

private:
    CARAPI_(AutoPtr<IStructGroupReq>) MakeGroupReq(
        /* [in] */ IInetAddress* gr_group,
        /* [in] */ INetworkInterface* networkInterface);

    CARAPI DoRecv(
        /* [in] */ IDatagramPacket* pack,
        /* [in] */ Int32 flags);

    CARAPI_(void) UpdatePacketRecvAddress(
        /* [in] */ IDatagramPacket* packet);

private:
    volatile Boolean mIsNativeConnected;
    AutoPtr<ICloseGuard> mGuard;
    AutoPtr<IInetAddress> mConnectedAddress;
    Int32 mConnectedPort;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CPLAINDATAGRAMSOCKETIMPL_H__
