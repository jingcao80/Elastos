
#ifdef DROID_CORE
#include "elastos/droid/net/CLinkCapabilities.h"
#include "elastos/droid/net/CLinkProperties.h"
#include "elastos/droid/net/CNetworkInfo.h"
#include "elastos/droid/os/CMessage.h"
#endif
#include "elastos/droid/net/DummyDataStateTracker.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Net::CLinkCapabilities;
using Elastos::Droid::Net::CLinkProperties;
using Elastos::Droid::Net::CNetworkInfo;
using Elastos::Droid::Os::CMessage;
using Elastos::Droid::Os::IMessage;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Net {

const String DummyDataStateTracker::TAG("DummyDataStateTracker");
const Boolean DummyDataStateTracker::DBG = TRUE;
const Boolean DummyDataStateTracker::VDBG = FALSE;

DummyDataStateTracker::DummyDataStateTracker(
    /* [in] */ Int32 netType,
    /* [in] */ const String& tag)
    : mTeardownRequested(FALSE)
    , mPrivateDnsRouteSet(FALSE)
    , mDefaultRouteSet(FALSE)
    , mIsDefaultOrHipri(FALSE)
{
    CNetworkInfo::New(netType, (INetworkInfo**)&mNetworkInfo);
}

PInterface DummyDataStateTracker::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(INetworkStateTracker*)this;
    }
    return NULL;
}

UInt32 DummyDataStateTracker::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 DummyDataStateTracker::Release()
{
    return ElRefBase::Release();
}

ECode DummyDataStateTracker::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return NOERROR;
}

ECode DummyDataStateTracker::StartMonitoring(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* target)
{
    mTarget = target;
    mContext = context;
    return NOERROR;
}

ECode DummyDataStateTracker::IsPrivateDnsRouteSet(
    /* [out] */ Boolean* isSet)
{
    VALIDATE_NOT_NULL(isSet);
    *isSet = mPrivateDnsRouteSet;
    return NOERROR;
}

ECode DummyDataStateTracker::SetPrivateDnsRoute(
    /* [in] */ Boolean enabled)
{
    mPrivateDnsRouteSet = enabled;
    return NOERROR;
}

ECode DummyDataStateTracker::GetNetworkInfo(
    /* [out] */ INetworkInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = mNetworkInfo;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode DummyDataStateTracker::IsDefaultRouteSet(
    /* [out] */ Boolean* isSet)
{
    VALIDATE_NOT_NULL(isSet);
    *isSet = mDefaultRouteSet;
    return NOERROR;
}

ECode DummyDataStateTracker::SetDefaultRoute(
    /* [in] */ Boolean enabled)
{
    mDefaultRouteSet = enabled;
    return NOERROR;
}

void DummyDataStateTracker::ReleaseWakeLock()
{}

ECode DummyDataStateTracker::IsAvailable(
    /* [out] */ Boolean* isAvailable)
{
    *isAvailable = TRUE;
    return NOERROR;
}

ECode DummyDataStateTracker::GetTcpBufferSizesPropName(
    /* [out] */ String* propName)
{
    VALIDATE_NOT_NULL(propName);
    *propName = "net.tcp.buffersize.unknown";
    return NOERROR;
}

ECode DummyDataStateTracker::Teardown(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    SetDetailedState(NetworkInfoDetailedState_DISCONNECTING, String("disabled"), String(NULL));
    SetDetailedState(NetworkInfoDetailedState_DISCONNECTED, String("disabled"), String(NULL));
    *result = TRUE;
    return NOERROR;
}

ECode DummyDataStateTracker::CaptivePortalCheckComplete()
{
    // not implemented
    return NOERROR;
}

void DummyDataStateTracker::SetDetailedState(
    /* [in] */ NetworkInfoDetailedState state,
    /* [in] */ const String& reason,
    /* [in] */ const String& extraInfo)
{
    if (DBG) {
        NetworkInfoDetailedState detailedState;
        mNetworkInfo->GetDetailedState(&detailedState);
        Slogger::D(TAG, "setDetailed state, old =%d and new state=", detailedState, state);
    }
    mNetworkInfo->SetDetailedState(state, reason, extraInfo);
    AutoPtr<IMessage> msg;
    CMessage::New((IMessage**)&msg);
    msg->SetWhat(EVENT_STATE_CHANGED);
    msg->SetObj((IInterface*)mNetworkInfo.Get());
    Boolean result;
    mTarget->SendMessage(msg, &result);
}

ECode DummyDataStateTracker::SetTeardownRequested(
    /* [in] */ Boolean isRequested)
{
    mTeardownRequested = isRequested;
    return NOERROR;
}

ECode DummyDataStateTracker::IsTeardownRequested(
    /* [out] */ Boolean* isRequested)
{
    VALIDATE_NOT_NULL(isRequested);
    *isRequested = mTeardownRequested;
    return NOERROR;
}

ECode DummyDataStateTracker::Reconnect(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    SetDetailedState(NetworkInfoDetailedState_CONNECTING, String("enabled"), String(NULL));
    SetDetailedState(NetworkInfoDetailedState_CONNECTED, String("enabled"), String(NULL));
    SetTeardownRequested(FALSE);
    *result = TRUE;
    return NOERROR;
}

ECode DummyDataStateTracker::SetRadio(
    /* [in] */ Boolean turnOn,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode DummyDataStateTracker::SetUserDataEnable(
    /* [in] */ Boolean enabled)
{
    // ignored
    return NOERROR;
}

ECode DummyDataStateTracker::SetPolicyDataEnable(
    /* [in] */ Boolean enabled)
{
    // ignored
    return NOERROR;
}

String DummyDataStateTracker::ToString()
{
    return String("Dummy data state: none, dummy!");
}

ECode DummyDataStateTracker::GetLinkProperties(
    /* [out] */ ILinkProperties** linkProperties)
{
    VALIDATE_NOT_NULL(linkProperties);
    return CLinkProperties::New(mLinkProperties, linkProperties);
}

ECode DummyDataStateTracker::GetLinkCapabilities(
    /* [out] */ ILinkCapabilities** linkCapabilities)
{
    VALIDATE_NOT_NULL(linkCapabilities);
    return CLinkCapabilities::New(mLinkCapabilities, linkCapabilities);
}

ECode DummyDataStateTracker::SetDependencyMet(
    /* [in] */ Boolean met)
{
    // not supported on this network
    return NOERROR;
}

} // namespace Net
} // namepsace Droid
} // namespace Elastos
