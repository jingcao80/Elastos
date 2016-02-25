
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/net/BaseNetworkStateTracker.h"
#include "elastos/droid/net/CNetwork.h"
#include "elastos/droid/net/CConnectivityManager.h"
#include "elastos/droid/net/CNetworkInfo.h"
#include "elastos/droid/net/CLinkProperties.h"
#include "elastos/droid/net/CNetworkInfo.h"
#include "elastos/droid/net/CNetworkCapabilities.h"

using Elastos::Droid::Os::IMessage;

using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL_2(BaseNetworkStateTracker, Object, INetworkStateTracker, IBaseNetworkStateTracker)

BaseNetworkStateTracker::BaseNetworkStateTracker()
    : mContext(NULL)
    , mNetworkInfo(NULL)
    , mLinkProperties(NULL)
    , mNetworkCapabilities(NULL)
    , mTarget(NULL)
{
    CNetwork::New(IConnectivityManager::NETID_UNSET, (INetwork**)&mNetwork);
    CAtomicBoolean::New((IAtomicBoolean**)&mTeardownRequested);
    CAtomicBoolean::New((IAtomicBoolean**)&mPrivateDnsRouteSet);
    CAtomicBoolean::New((IAtomicBoolean**)&mDefaultRouteSet);
}

ECode BaseNetworkStateTracker::constructor()
{
    // By default, let the sub classes construct everything
    return NOERROR;
}

ECode BaseNetworkStateTracker::constructor(
    /* [in] */ Int32 networkType)
{

    String typeName;
    CConnectivityManager::GetNetworkTypeName(networkType, &typeName);
    CNetworkInfo::New(networkType, -1, typeName, String(NULL), (INetworkInfo**)&mNetworkInfo);
    CLinkProperties::New((ILinkProperties**)&mLinkProperties);
    CNetworkCapabilities::New((INetworkCapabilities**)&mNetworkCapabilities);

    return NOERROR;
}

ECode BaseNetworkStateTracker::GetTargetHandler(
    /* [out] */ IHandler** handler)
{
    VALIDATE_NOT_NULL(handler)

    *handler = mTarget;
    REFCOUNT_ADD(*handler)
    return NOERROR;
}

ECode BaseNetworkStateTracker::DispatchStateChanged()
{
    // TODO: include snapshot of other fields when sending
    AutoPtr<INetworkInfo> netInfo;
    GetNetworkInfo((INetworkInfo**)&netInfo);
    AutoPtr<IMessage> msg;
    mTarget->ObtainMessage(EVENT_STATE_CHANGED, netInfo, (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode BaseNetworkStateTracker::DispatchConfigurationChanged()
{
    // TODO: include snapshot of other fields when sending
    AutoPtr<INetworkInfo> netInfo;
    GetNetworkInfo((INetworkInfo**)&netInfo);
    AutoPtr<IMessage> msg;
    mTarget->ObtainMessage(EVENT_CONFIGURATION_CHANGED, netInfo, (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode BaseNetworkStateTracker::StartMonitoring(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* target)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(target);

    mContext = context;
    mTarget = target;
    return StartMonitoringInternal();
}

ECode BaseNetworkStateTracker::StartMonitoringInternal()
{
    return NOERROR;
}

ECode BaseNetworkStateTracker::GetNetworkInfo(
    /* [out] */ INetworkInfo** info)
{
    VALIDATE_NOT_NULL(info);

    return CNetworkInfo::New(mNetworkInfo, info);
}

ECode BaseNetworkStateTracker::GetLinkProperties(
    /* [out] */ ILinkProperties** result)
{
    VALIDATE_NOT_NULL(result);

    return CLinkProperties::New(mLinkProperties, result);
}

ECode BaseNetworkStateTracker::GetLinkQualityInfo(
    /* [out] */ ILinkQualityInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    return NOERROR;
}

ECode BaseNetworkStateTracker::CaptivePortalCheckCompleted(
    /* [in] */ Boolean isCaptive)
{
    // not implemented
    return NOERROR;
}

ECode BaseNetworkStateTracker::SetRadio(
    /* [in] */ Boolean turnOn,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // Base tracker doesn't handle radios
    *result = TRUE;
    return NOERROR;
}

ECode BaseNetworkStateTracker::IsAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    return mNetworkInfo->IsAvailable(result);
}

ECode BaseNetworkStateTracker::SetUserDataEnable(
    /* [in] */ Boolean enabled)
{
    // Base tracker doesn't handle enabled flags
    return NOERROR;
}

ECode BaseNetworkStateTracker::SetPolicyDataEnable(
    /* [in] */ Boolean enabled)
{
    // Base tracker doesn't handle enabled flags
    return NOERROR;
}

ECode BaseNetworkStateTracker::IsPrivateDnsRouteSet(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    return mPrivateDnsRouteSet->Get(result);
}

ECode BaseNetworkStateTracker::PrivateDnsRouteSet(
    /* [in] */ Boolean enabled)
{
    return mPrivateDnsRouteSet->Set(enabled);
}

ECode BaseNetworkStateTracker::IsDefaultRouteSet(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    return mDefaultRouteSet->Get(result);
}

ECode BaseNetworkStateTracker::DefaultRouteSet(
    /* [in] */ Boolean enabled)
{
    return mDefaultRouteSet->Set(enabled);
}

ECode BaseNetworkStateTracker::IsTeardownRequested(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    return mTeardownRequested->Get(result);
}

ECode BaseNetworkStateTracker::SetTeardownRequested(
    /* [in] */ Boolean isRequested)
{
    return mTeardownRequested->Set(isRequested);
}

ECode BaseNetworkStateTracker::SetDependencyMet(
    /* [in] */ Boolean met)
{
    // Base tracker doesn't handle dependencies
    return NOERROR;
}

ECode BaseNetworkStateTracker::AddStackedLink(
    /* [in] */ ILinkProperties* link)
{
    VALIDATE_NOT_NULL(link)

    Boolean b;
    return mLinkProperties->AddStackedLink(link, &b);
}

ECode BaseNetworkStateTracker::RemoveStackedLink(
    /* [in] */ ILinkProperties* link)
{
    VALIDATE_NOT_NULL(link)

    Boolean b;
    return mLinkProperties->RemoveStackedLink(link, &b);
}


ECode BaseNetworkStateTracker::SupplyMessenger(
    /* [in] */ IMessenger* messenger)
{
    VALIDATE_NOT_NULL(messenger)

    // not supported on this network
    return NOERROR;
}


ECode BaseNetworkStateTracker::GetNetworkInterfaceName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    if (mLinkProperties != NULL) {
        return mLinkProperties->GetInterfaceName(result);
    }
    return NOERROR;
}


ECode BaseNetworkStateTracker::StartSampling(
    /* [in] */ ISamplingDataTrackerSamplingSnapshot* s)
{
    VALIDATE_NOT_NULL(s)

    // nothing to do
    return NOERROR;
}


ECode BaseNetworkStateTracker::StopSampling(
    /* [in] */ ISamplingDataTrackerSamplingSnapshot* s)
{
    VALIDATE_NOT_NULL(s)

    // nothing to do
    return NOERROR;
}


ECode BaseNetworkStateTracker::SetNetId(
    /* [in] */ Int32 netId)
{
    mNetwork = NULL;
    return CNetwork::New(netId, (INetwork**)&mNetwork);
}

ECode BaseNetworkStateTracker::GetNetwork(
    /* [out] */ INetwork** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mNetwork;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode BaseNetworkStateTracker::GetNetworkCapabilities(
    /* [out] */ INetworkCapabilities** result)
{
    VALIDATE_NOT_NULL(result)

    return CNetworkCapabilities::New(mNetworkCapabilities, result);
}

} // namespace Net
} // namepsace Droid
} // namespace Elastos
