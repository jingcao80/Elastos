
#ifndef __ELASTOS_DROID_NET_BASENETWORKSTATETRACKER_H__
#define __ELASTOS_DROID_NET_BASENETWORKSTATETRACKER_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessenger;

using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;

namespace Elastos {
namespace Droid {
namespace Net {

class BaseNetworkStateTracker
    : public Object
    , public INetworkStateTracker
    , public IBaseNetworkStateTracker
{
    // TODO: better document threading expectations
    // TODO: migrate to make NetworkStateTracker abstract class

public:
    CAR_INTERFACE_DECL()

    virtual CARAPI constructor(
        /* [in] */ Int32 networkType);

    // @Override
    virtual CARAPI StartMonitoring(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* target);

    virtual CARAPI GetNetworkInfo(
        /* [out] */ INetworkInfo** info);

    virtual CARAPI GetLinkProperties(
        /* [out] */ ILinkProperties** result);

    // @Override
    CARAPI GetNetworkCapabilities(
        /* [out] */ INetworkCapabilities** result);

    virtual CARAPI GetLinkQualityInfo(
        /* [out] */ ILinkQualityInfo** result);

    virtual CARAPI CaptivePortalCheckCompleted(
        /* [in] */ Boolean isCaptive);

    virtual CARAPI SetRadio(
        /* [in] */ Boolean turnOn,
        /* [out] */ Boolean* result);

    virtual CARAPI IsAvailable(
        /* [out] */ Boolean* result);

    virtual CARAPI SetUserDataEnable(
        /* [in] */ Boolean enabled);

    virtual CARAPI SetPolicyDataEnable(
        /* [in] */ Boolean enabled);

    virtual CARAPI IsPrivateDnsRouteSet(
        /* [out] */ Boolean* result);

    virtual CARAPI PrivateDnsRouteSet(
        /* [in] */ Boolean enabled);

    virtual CARAPI IsDefaultRouteSet(
        /* [out] */ Boolean* result);

    virtual CARAPI DefaultRouteSet(
        /* [in] */ Boolean enabled);

    virtual CARAPI IsTeardownRequested(
        /* [out] */ Boolean* result);

    virtual CARAPI SetTeardownRequested(
        /* [in] */ Boolean isRequested);

    virtual CARAPI SetDependencyMet(
        /* [in] */ Boolean met);

    // @Override
    virtual CARAPI AddStackedLink(
        /* [in] */ ILinkProperties* link);

    // @Override
    virtual CARAPI RemoveStackedLink(
        /* [in] */ ILinkProperties* link);

    // @Override
    virtual CARAPI SupplyMessenger(
        /* [in] */ IMessenger* messenger);

    // @Override
    virtual CARAPI GetNetworkInterfaceName(
        /* [out] */ String* result);

    // @Override
    virtual CARAPI StartSampling(
        /* [in] */ ISamplingDataTrackerSamplingSnapshot* s);

    // @Override
    virtual CARAPI StopSampling(
        /* [in] */ ISamplingDataTrackerSamplingSnapshot* s);

    // @Override
    virtual CARAPI SetNetId(
        /* [in] */ Int32 netId);

    // @Override
    virtual CARAPI GetNetwork(
        /* [out] */ INetwork** result);

protected:
    BaseNetworkStateTracker();

    virtual CARAPI constructor();

    virtual CARAPI GetTargetHandler(
        /* [out] */ IHandler** handler);

    virtual CARAPI DispatchStateChanged();

    virtual CARAPI DispatchConfigurationChanged();

    virtual CARAPI StartMonitoringInternal();

protected:
    AutoPtr<IContext> mContext;
    AutoPtr<INetworkInfo> mNetworkInfo;
    AutoPtr<ILinkProperties> mLinkProperties;
    AutoPtr<INetwork> mNetwork;
    AutoPtr<INetworkCapabilities> mNetworkCapabilities;

private:
    AutoPtr<IHandler> mTarget;
    AutoPtr<IAtomicBoolean> mTeardownRequested;
    AutoPtr<IAtomicBoolean> mPrivateDnsRouteSet;
    AutoPtr<IAtomicBoolean> mDefaultRouteSet;
};

} // namespace Net
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_BASENETWORKSTATETRACKER_H__
