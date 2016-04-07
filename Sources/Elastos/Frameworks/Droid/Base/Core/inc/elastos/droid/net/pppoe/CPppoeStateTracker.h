
#ifndef __ELASTOS_DROID_NET_PPPOE_CPPPOESTATETRACKER_H__
#define __ELASTOS_DROID_NET_PPPOE_CPPPOESTATETRACKER_H__

#include "_Elastos_Droid_Net_Pppoe_CPppoeStateTracker.h"
#include "CPppoeMonitor.h"
#include "elastos/droid/net/DhcpInfoInternal.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerCallback;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Net::IDhcpInfo;
using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::ILinkCapabilities;
using Elastos::Droid::Net::DhcpInfoInternal;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Pppoe {

/**
 * Track the state of Pppoe connectivity. All event handling is done here,
 * and all changes in connectivity state are initiated here.
 *
 * @hide
 */
CarClass(CPppoeStateTracker)
{
public:
    CARAPI StopInterface(
        /* [in] */ Boolean suspend,
        /* [out] */ Boolean* result);

    CARAPI ResetInterface(
        /* [out] */ Boolean* result);

    CARAPI GetTcpBufferSizesPropName(
        /* [out] */ String* name);

    CARAPI StartPolling();

    CARAPI IsAvailable(
        /* [out] */ Boolean* result);

    CARAPI Reconnect(
        /* [out] */ Boolean* result);

    CARAPI SetRadio(
        /* [in] */ Boolean turnOn,
        /* [out] */ Boolean* result);

    CARAPI StartMonitoring(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* target);

    CARAPI Teardown(
        /* [out] */ Boolean* result);

    CARAPI CaptivePortalCheckComplete();

    CARAPI GetDhcpInfo(
        /* [out] */ IDhcpInfo** dhcpInfo);

    CARAPI NotifyPppConnected(
        /* [in] */ const String& ifname);

    CARAPI NotifyStateChange(
        /* [in] */ const String& ifname,
        /* [in] */ NetworkInfoDetailedState state);

    CARAPI GetNetworkInfo(
        /* [out] */ INetworkInfo** info);

    CARAPI GetLinkProperties(
        /* [out] */ ILinkProperties** linkProperties);

    CARAPI GetLinkCapabilities(
        /* [out] */ ILinkCapabilities** linkCapabilities);

    CARAPI SetUserDataEnable(
        /* [in] */ Boolean enabled);

    CARAPI SetPolicyDataEnable(
        /* [in] */ Boolean enabled);

    CARAPI IsPrivateDnsRouteSet(
        /* [out] */ Boolean* result);

    CARAPI PrivateDnsRouteSet(
        /* [in] */ Boolean enabled);

    CARAPI IsDefaultRouteSet(
        /* [out] */ Boolean* result);

    CARAPI DefaultRouteSet(
        /* [in] */ Boolean enabled);

    CARAPI SetTeardownRequested(
        /* [in] */ Boolean isRequested);

    CARAPI IsTeardownRequested(
        /* [out] */ Boolean* result);

    CARAPI SetDependencyMet(
        /* [in] */ Boolean met);

    CARAPI constructor(
        /* [in] */ Int32 netType,
        /* [in] */ const String& networkName);
private:
    class MyHandlerCallback
        : public ElRefBase
        , public IHandlerCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MyHandlerCallback(
            /* [in] */ CPppoeStateTracker* owner);

        CARAPI HandleMessage(
           /* [in] */ IMessage* msg,
           /* [out] */ Boolean* result);
    private:
        AutoPtr<CPppoeStateTracker> mOwner;
        Object mLock;
    };

private:
    CARAPI ConfigureInterfaceStatic(
        /* [in] */ const String& ifname,
        /* [in] */ DhcpInfoInternal* dhcpInfoInternal,
        /* [out] */ Boolean* result);

    CARAPI ConfigureInterface(
        /* [in] */ IPppoeDevInfo* info);

    CARAPI PostNotification(
        /* [in] */ Int32 event,
        /* [in] */ const String& errcode);

    CARAPI SetPppoeState(
        /* [in] */ Boolean state,
        /* [in] */ Int32 event,
        /* [in] */ const String& errcode);

    static const String TAG;
    static const String PROP_PPP_ADDR;
    static const String PROP_PPP_MASK;
    static const String PROP_PPP_DNS1;
    static const String PROP_PPP_DNS2;
    static const String PROP_PPP_GW;
    static const String PROP_VAL_PPP_NOERR;
    static const String PROP_NAME_PPP_ERRCODE;

    AutoPtr<IPppoeManager> mEM;
    Boolean mServiceStarted;
    Boolean mInterfaceStopped;
    Boolean mPppInterfaceAdded;// = false;
    String mInterfaceName;// = "ppp0";
    AutoPtr<DhcpInfoInternal> mDhcpInfoInternal;
    AutoPtr<IPppoeMonitor> mMonitor;
    AutoPtr<IAtomicBoolean> mTeardownRequested; //= new AtomicBoolean(false);
    AutoPtr<IAtomicBoolean> mPrivateDnsRouteSet; //= new AtomicBoolean(false);
    AutoPtr<IAtomicBoolean> mDefaultRouteSet; //= new AtomicBoolean(false);

    AutoPtr<ILinkProperties> mLinkProperties;
    AutoPtr<ILinkCapabilities> mLinkCapabilities;
    AutoPtr<INetworkInfo> mNetworkInfo;

    AutoPtr<IHandler> mTarget;
    AutoPtr<IHandler> mTrackerTarget;
    AutoPtr<IContext> mContext;
    AutoPtr<IHandlerCallback> mTrackerHandlerCallback;
    Object mSyncLock;
};

} // Pppoe
} // Net
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_NET_PPPOE_CPPPOESTATETRACKER_H__
