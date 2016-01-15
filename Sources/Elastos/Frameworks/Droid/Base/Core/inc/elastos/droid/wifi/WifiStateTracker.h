
#ifndef __ELASTOS_DROID_NET_WIFI_WIFISTATETRACKER_H__
#define  __ELASTOS_DROID_NET_WIFI_WIFISTATETRACKER_H__

#include "elastos/droid/net/NetworkStateTracker.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::INetworkStateTracker;
using Elastos::Droid::Net::NetworkInfoState;


namespace Elastos {
namespace Droid {
namespace Wifi {

/**
 * Track the state of wifi for connectivity service.
 *
 * @hide
 */
class WifiStateTracker
    : public ElRefBase
    , public INetworkStateTracker
{
private:
    class WifiStateReceiver : public BroadcastReceiver
    {
    public:
        WifiStateReceiver(
            /* [in] */ WifiStateTracker* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("WifiStateTracker::WifiStateReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        WifiStateTracker* mHost;
    };

public:
    WifiStateTracker(
        /* [in] */ Int32 netType,
        /* [in] */ const String& networkName);

    CAR_INTERFACE_DECL()

    CARAPI SetTeardownRequested(
        /* [in] */ Boolean isRequested);

    CARAPI IsTeardownRequested(
        /* [out] */ Boolean* isRequested);

    /**
     * Begin monitoring wifi connectivity
     */
    CARAPI StartMonitoring(
        /* [in] */ IContext* ctx,
        /* [in] */ IHandler* target);

    /**
     * Disable connectivity to a network
     * TODO: do away with return value after making MobileDataStateTracker async
     */
    CARAPI Teardown(
        /* [out] */ Boolean* result);

    /**
     * Re-enable connectivity to a network after a {@link #teardown()}.
     */
    CARAPI Reconnect(
        /* [out] */ Boolean* result);

    /**
     * Captive check is complete, switch to network
     */
    CARAPI CaptivePortalCheckComplete();

    /**
     * Turn the wireless radio off for a network.
     * @param turnOn {@code true} to turn the radio on, {@code false}
     */
    CARAPI SetRadio(
        /* [in] */ Boolean turnOn,
        /* [out] */ Boolean* result);

    /**
     * Wi-Fi is considered available as long as we have a connection to the
     * supplicant daemon and there is at least one enabled network. If a teardown
     * was explicitly requested, then Wi-Fi can be restarted with a reconnect
     * request, so it is considered available. If the driver has been stopped
     * for any reason other than a teardown request, Wi-Fi is considered
     * unavailable.
     * @return {@code true} if Wi-Fi connections are possible
     */
    CARAPI IsAvailable(
        /* [out] */ Boolean* isAvailable);

    CARAPI SetUserDataEnable(
        /* [in] */ Boolean enabled);

    CARAPI SetPolicyDataEnable(
        /* [in] */ Boolean enabled);

    /**
     * Check if private DNS route is set for the network
     */
    CARAPI IsPrivateDnsRouteSet(
        /* [out] */ Boolean* isSet);

    /**
     * Set a flag indicating private DNS route is set
     */
    CARAPI SetPrivateDnsRoute(
        /* [in] */ Boolean enabled);

    /**
     * Fetch NetworkInfo for the network
     */
    CARAPI GetNetworkInfo(
        /* [out] */ INetworkInfo** info);

    /**
     * Fetch LinkProperties for the network
     */
    CARAPI GetLinkProperties(
        /* [out] */ ILinkProperties** linkProperties);

    /**
     * A capability is an Integer/String pair, the capabilities
     * are defined in the class LinkSocket#Key.
     *
     * @return a copy of this connections capabilities, may be empty but never null.
     */
    CARAPI GetLinkCapabilities(
        /* [out] */ ILinkCapabilities** linkCapabilities);

    /**
     * Check if default route is set
     */
    CARAPI IsDefaultRouteSet(
        /* [out] */ Boolean* isSet);

    /**
     * Set a flag indicating default route is set for the network
     */
    CARAPI SetDefaultRoute(
        /* [in] */ Boolean enabled);

    /**
     * Return the system properties name associated with the tcp buffer sizes
     * for this network.
     */
    CARAPI GetTcpBufferSizesPropName(
        /* [out] */ String* propName);

    CARAPI SetDependencyMet(
        /* [in] */ Boolean met);

private:
    static const String NETWORKTYPE;
    static const String TAG;

    static const Boolean LOGV;

    AutoPtr<IAtomicBoolean> mTeardownRequested;
    AutoPtr<IAtomicBoolean> mPrivateDnsRouteSet;
    AutoPtr<IAtomicBoolean> mDefaultRouteSet;

    AutoPtr<ILinkProperties> mLinkProperties;
    AutoPtr<ILinkCapabilities> mLinkCapabilities;
    AutoPtr<INetworkInfo> mNetworkInfo;
    NetworkInfoState mLastState;

    /* For sending events to connectivity service handler */
    AutoPtr<IHandler> mCsHandler;
    AutoPtr<IContext> mContext;
    AutoPtr<IBroadcastReceiver> mWifiStateReceiver;
    AutoPtr<IWifiManager> mWifiManager;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_NET_WIFI_WIFISTATETRACKER_H__
