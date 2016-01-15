
#ifndef __ELASTOS_NET_SERVERSOCKET_H__
#define __ELASTOS_NET_SERVERSOCKET_H__

#include "CSocket.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::ICloseable;
using Elastos::IO::Channels::IServerSocketChannel;

namespace Elastos {
namespace Net {

class ServerSocket
    : public Object
    , public IServerSocket
    , public ICloseable
{
public:
    CAR_INTERFACE_DECL()

    ServerSocket();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 aPort);

    CARAPI constructor(
        /* [in] */ Int32 aPort,
        /* [in] */ Int32 backlog);

    CARAPI constructor(
        /* [in] */ Int32 aPort,
        /* [in] */ Int32 backlog,
        /* [in] */ IInetAddress* localAddr);

    CARAPI GetImpl(
        /* [out] */ ISocketImpl** impl);

    CARAPI Accept(
        /* [out] */ ISocket** socket);

    CARAPI CheckListen(
        /* [in] */ Int32 aPort);

    CARAPI Close();

    CARAPI GetInetAddress(
        /* [out] */ IInetAddress** address);

    CARAPI GetLocalPort(
        /* [out] */ Int32* port);

    CARAPI GetSoTimeout(
        /* [out] */ Int32* timeout);

    static CARAPI SetSocketFactory(
        /* [in] */ ISocketImplFactory* aFactory);

    CARAPI SetSoTimeout(
        /* [in] */ Int32 timeout);

    CARAPI ToString(
        /* [out] */ String* info);

    CARAPI Bind(
        /* [in] */ ISocketAddress* localAddr);

    CARAPI Bind(
        /* [in] */ ISocketAddress* localAddr,
        /* [in] */ Int32 backlog);

    CARAPI GetLocalSocketAddress(
        /* [out] */ ISocketAddress** address);

    CARAPI IsBound(
        /* [out] */ Boolean* isBound);

    CARAPI IsClosed(
        /* [out] */ Boolean* isClosed);

    CARAPI SetReuseAddress(
        /* [in] */ Boolean reuse);

    CARAPI GetReuseAddress(
        /* [out] */ Boolean* reuse);

    CARAPI SetReceiveBufferSize(
        /* [in] */ Int32 size);

    CARAPI GetReceiveBufferSize(
        /* [out] */ Int32* size);

    CARAPI GetChannel(
        /* [out] */ IServerSocketChannel** channel);

    CARAPI SetPerformancePreferences(
        /* [in] */ Int32 connectionTime,
        /* [in] */ Int32 latency,
        /* [in] */ Int32 bandwidth);

//protected:
    CARAPI ImplAccept(
        /* [in] */ ISocket* aSocket);

private:
    CARAPI ReadBackBindState();

    CARAPI CheckOpen();

public:
    AutoPtr<ISocketImpl> mImpl;

    static AutoPtr<ISocketImplFactory> sFactory;
    static Object sLock;

private:
    static const Int32 DEFAULT_BACKLOG;// = 50;

    Boolean mIsBound;
    Boolean mIsClosed;

    AutoPtr<IInetAddress> mLocalAddress;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_SERVERSOCKET_H__
