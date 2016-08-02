#ifndef __ELASTOS_DROID_SERVER_WIFI_WIFITRAFFICPOLLER_H__
#define __ELASTOS_DROID_SERVER_WIFI_WIFITRAFFICPOLLER_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::Handler;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IList;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

/* Polls for traffic stats and notifies the clients */
class WifiTrafficPoller
    : public Object
{
private:
    class InnerBroadcastReceiver1
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("WifiTrafficPoller::InnerBroadcastReceiver1")

        InnerBroadcastReceiver1(
            /* [in] */ WifiTrafficPoller* owner);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        WifiTrafficPoller* mOwner;
    };

    class TrafficHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("WifiTrafficPoller::TrafficHandler")

        TrafficHandler(
            /* [in] */ WifiTrafficPoller* owner)
            : mOwner(owner)
        {
            Handler::constructor();
        }

        virtual CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        WifiTrafficPoller* mOwner;
    };

public:
    WifiTrafficPoller(
        /* [in] */ IContext* context,
        /* [in] */ const String& iface);

    virtual CARAPI AddClient(
        /* [in] */ IMessenger* client);

    virtual CARAPI RemoveClient(
        /* [in] */ IMessenger* client);

    virtual CARAPI EnableVerboseLogging(
        /* [in] */ Int32 verbose);

    virtual CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI_(void) EvaluateTrafficStatsPolling();

    CARAPI_(void) NotifyOnDataActivity();

private:
    Boolean DBG;
    Boolean VDBG;
    String TAG;
    /**
      * Interval in milliseconds between polling for traffic
      * statistics
      */
    static const Int32 POLL_TRAFFIC_STATS_INTERVAL_MSECS = 1000;
    static const Int32 ENABLE_TRAFFIC_STATS_POLL = 1;
    static const Int32 TRAFFIC_STATS_POLL = 2;
    static const Int32 ADD_CLIENT = 3;
    static const Int32 REMOVE_CLIENT = 4;
    Boolean mEnableTrafficStatsPoll;
    Int32 mTrafficStatsPollToken;
    Int64 mTxPkts;
    Int64 mRxPkts;
    /* Tracks last reported data activity */
    Int32 mDataActivity;
    AutoPtr<IList> mClients;// IMessenger
    // err on the side of updating at boot since screen on broadcast may be missed
    // the first time
    AutoPtr<IAtomicBoolean> mScreenOn;
    AutoPtr<TrafficHandler> mTrafficHandler;
    AutoPtr<INetworkInfo> mNetworkInfo;
    String mInterface;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_WIFITRAFFICPOLLER_H__

