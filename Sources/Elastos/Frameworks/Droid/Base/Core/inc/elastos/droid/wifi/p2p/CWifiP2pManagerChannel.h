#ifndef __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PMANAGERCHANNEL_H__
#define __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PMANAGERCHANNEL_H__

#include "_Elastos_Droid_Wifi_P2p_CWifiP2pManagerChannel.h"
#include "elastos/droid/internal/utility/AsyncChannel.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/Pair.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::Pair;
using Elastos::Utility::Etl::HashMap;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Utility::AsyncChannel;
using Elastos::Droid::Wifi::P2p::Nsd::IWifiP2pServiceResponse;
using Elastos::Droid::Wifi::P2p::Nsd::IWifiP2pUpnpServiceResponse;
using Elastos::Droid::Wifi::P2p::Nsd::IWifiP2pDnsSdServiceResponse;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {

CarClass(CWifiP2pManagerChannel)
    , public Object
    , public IWifiP2pManagerChannel
{
private:
    class P2pHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CWifiP2pManagerChannel::P2pHandler")

        P2pHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ CWifiP2pManagerChannel* context);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        CWifiP2pManagerChannel* mHost;

    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CWifiP2pManagerChannel();

    ~CWifiP2pManagerChannel();

    CARAPI constructor(
        /* [in] */ IContext * context,
        /* [in] */ ILooper * looper,
        /* [in] */ IWifiP2pManagerChannelListener* l);

    CARAPI GetHandler(
        /* [out] */ IHandler** handler);

    CARAPI SetHandler(
        /* [in] */ IHandler* handler);

    CARAPI GetContext(
        /* [out] */ IContext** context);

    CARAPI SetContext(
        /* [in] */ IContext* context);

    CARAPI GetChannelListener(
        /* [out] */ IWifiP2pManagerChannelListener** listener);

    CARAPI SetChannelListener(
        /* [in] */ IWifiP2pManagerChannelListener* listener);

    CARAPI GetServRspListener(
        /* [out] */ IWifiP2pManagerServiceResponseListener** listener);

    CARAPI SetServRspListener(
        /* [in] */ IWifiP2pManagerServiceResponseListener* listener);

    CARAPI GetDnsSdServRspListener(
        /* [out] */ IWifiP2pManagerDnsSdServiceResponseListener** listener);

    CARAPI SetDnsSdServRspListener(
        /* [in] */ IWifiP2pManagerDnsSdServiceResponseListener* listener);

    CARAPI GetDnsSdTxtListener(
        /* [out] */ IWifiP2pManagerDnsSdTxtRecordListener** listener);

    CARAPI SetDnsSdTxtListener(
        /* [in] */ IWifiP2pManagerDnsSdTxtRecordListener* listener);

    CARAPI GetUpnpServRspListener(
        /* [out] */ IWifiP2pManagerUpnpServiceResponseListener** listener);

    CARAPI SetUpnpServRspListener(
        /* [in] */ IWifiP2pManagerUpnpServiceResponseListener* listener);

    CARAPI HandleServiceResponse(
        /* [in] */ IWifiP2pServiceResponse* resp);

    CARAPI HandleUpnpServiceResponse(
        /* [in] */ IWifiP2pUpnpServiceResponse* resp);

    CARAPI HandleDnsSdServiceResponse(
        /* [in] */ IWifiP2pDnsSdServiceResponse* resp);

    CARAPI PutListener(
        /* [in] */ IInterface* listener,
        /* [out] */ Int32* val);

    CARAPI GetListener(
        /* [in] */ Int32 key,
        /* [out] */ IInterface** obj);

public:
    static const Int32 INVALID_LISTENER_KEY;
    AutoPtr<IWifiP2pManagerChannelListener> mChannelListener;
    AutoPtr<IWifiP2pManagerServiceResponseListener> mServRspListener;
    AutoPtr<IWifiP2pManagerDnsSdServiceResponseListener> mDnsSdServRspListener;
    AutoPtr<IWifiP2pManagerDnsSdTxtRecordListener> mDnsSdTxtListener;
    AutoPtr<IWifiP2pManagerUpnpServiceResponseListener> mUpnpServRspListener;
    HashMap<Int32, AutoPtr<IInterface> > mListenerMap;
    Object mListenerMapLock;
    Int32 mListenerKey;

    AutoPtr<AsyncChannel> mAsyncChannel;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IContext> mContext;
};

} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PMANAGERCHANNEL_H__
