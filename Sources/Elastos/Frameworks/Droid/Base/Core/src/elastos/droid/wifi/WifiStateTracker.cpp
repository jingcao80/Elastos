
#include "elastos/droid/wifi/WifiStateTracker.h"
#ifdef DROID_CORE
#include "elastos/droid/CNetworkInfo.h"
#include "elastos/droid/CLinkCapabilities.h"
#include "elastos/droid/CLinkProperties.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/os/CMessage.h"
#endif
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Net::EIID_INetworkInfo;
using Elastos::Droid::Net::EIID_ILinkProperties;
using Elastos::Droid::Net::EIID_ILinkCapabilities;
using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Droid::Wifi::P2p::IWifiP2pManager;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::CMessage;

namespace Elastos {
namespace Droid {
namespace Wifi {

WifiStateTracker::WifiStateReceiver::WifiStateReceiver(
    /* [in] */ WifiStateTracker* host)
    : mHost(host)
{}

ECode WifiStateTracker::WifiStateReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    if (action.Equals(IWifiP2pManager::WIFI_P2P_CONNECTION_CHANGED_ACTION)) {
        Boolean bval;
        Int32 type;
        if (mHost->mNetworkInfo != NULL
            && (mHost->mNetworkInfo->IsConnected(&bval), bval)
            && ((mHost->mNetworkInfo->GetType(&type), type) == IConnectivityManager::TYPE_WIFI) ) {
            // Slog.d(TAG, "sta is connected,p2p mustn't update mLinkProperties!!!!!!");
        }
        else {
            AutoPtr<IParcelable> parcelable;
            intent->GetParcelableExtra(IWifiManager::EXTRA_NETWORK_INFO, (IParcelable**)&parcelable);
            mHost->mNetworkInfo = INetworkInfo::Probe(parcelable);

            parcelable = NULL;
            mHost->mLinkProperties = NULL;
            intent->GetParcelableExtra(IWifiManager::EXTRA_LINK_PROPERTIES, (IParcelable**)&parcelable);
            if (parcelable != NULL) {
                mHost->mLinkProperties = ILinkProperties::Probe(parcelable);
            }
            if (mHost->mLinkProperties == NULL) {
                CLinkProperties::New((ILinkProperties**)&mHost->mLinkProperties);
            }

            parcelable = NULL;
            mHost->mLinkCapabilities = NULL;
            intent->GetParcelableExtra(IWifiManager::EXTRA_LINK_CAPABILITIES, (IParcelable**)&parcelable);
            if (parcelable != NULL) {
                mHost->mLinkCapabilities = ILinkCapabilities::Probe(parcelable);
            }
            if (mHost->mLinkCapabilities == NULL) {
                CLinkCapabilities::New((ILinkCapabilities**)&mHost->mLinkCapabilities);
            }
        }
    }
    else if (action.Equals(IWifiManager::NETWORK_STATE_CHANGED_ACTION)) {
        AutoPtr<IParcelable> parcelable;
        intent->GetParcelableExtra(IWifiManager::EXTRA_NETWORK_INFO, (IParcelable**)&parcelable);
        mHost->mNetworkInfo = INetworkInfo::Probe(parcelable);

        parcelable = NULL;
        mHost->mLinkProperties = NULL;
        intent->GetParcelableExtra(IWifiManager::EXTRA_LINK_PROPERTIES, (IParcelable**)&parcelable);
        if (parcelable != NULL) {
            mHost->mLinkProperties = ILinkProperties::Probe(parcelable);
        }
        if (mHost->mLinkProperties == NULL) {
            CLinkProperties::New((ILinkProperties**)&mHost->mLinkProperties);
        }

        parcelable = NULL;
        mHost->mLinkCapabilities = NULL;
        intent->GetParcelableExtra(IWifiManager::EXTRA_LINK_CAPABILITIES, (IParcelable**)&parcelable);
        if (parcelable != NULL) {
            mHost->mLinkCapabilities = ILinkCapabilities::Probe(parcelable);
        }
        if (mHost->mLinkCapabilities == NULL) {
            CLinkCapabilities::New((ILinkCapabilities**)&mHost->mLinkCapabilities);
        }

        // don't want to send redundent state messages
        // but send portal check detailed state notice
        NetworkInfoState state;
        mHost->mNetworkInfo->GetState(&state);
        NetworkInfoDetailedState detailState;
        if (mHost->mLastState == state &&
                (mHost->mNetworkInfo->GetDetailedState(&detailState), detailState != NetworkInfoDetailedState_CAPTIVE_PORTAL_CHECK)) {
            return NOERROR;
        }
        else {
            mHost->mLastState = state;
        }
        AutoPtr<IMessage> msg;
        CMessage::New((IMessage**)&msg);
        msg->SetWhat(EVENT_STATE_CHANGED);
        AutoPtr<INetworkInfo> networkInfo;
        CNetworkInfo::New(mHost->mNetworkInfo, (INetworkInfo**)&networkInfo);
        msg->SetObj((IInterface*)networkInfo.Get());
        Boolean result;
        mHost->mCsHandler->SendMessage(msg, &result);
    }
    else if (intent->GetAction(&action), action.Equals(IWifiManager::LINK_CONFIGURATION_CHANGED_ACTION)) {
        AutoPtr<IParcelable> parcelable;
        intent->GetParcelableExtra(IWifiManager::EXTRA_LINK_PROPERTIES, (IParcelable**)&parcelable);
        mHost->mLinkProperties = ILinkProperties::Probe(parcelable);
        AutoPtr<IMessage> msg;
        CMessage::New((IMessage**)&msg);
        msg->SetWhat(EVENT_CONFIGURATION_CHANGED);
        msg->SetObj((IInterface*)mHost->mNetworkInfo.Get());
        Boolean result;
        mHost->mCsHandler->SendMessage(msg, &result);
    }
    return NOERROR;
}


const String WifiStateTracker::NETWORKTYPE("WIFI");
const String WifiStateTracker::TAG("WifiStateTracker");
const Boolean WifiStateTracker::LOGV = TRUE;

WifiStateTracker::WifiStateTracker(
    /* [in] */ Int32 netType,
    /* [in] */ const String& networkName)
    : mLastState(NetworkInfoState_UNKNOWN)
{
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mTeardownRequested);
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mPrivateDnsRouteSet);
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mDefaultRouteSet);

    CNetworkInfo::New(netType, 0, networkName, String(""), (INetworkInfo**)&mNetworkInfo);
    CLinkProperties::New((ILinkProperties**)&mLinkProperties);
    CLinkCapabilities::New((ILinkCapabilities**)&mLinkCapabilities);;

    mNetworkInfo->SetIsAvailable(FALSE);
    SetTeardownRequested(FALSE);
}

PInterface WifiStateTracker::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(INetworkStateTracker*)this;
    }
    return NULL;
}

UInt32 WifiStateTracker::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 WifiStateTracker::Release()
{
    return ElRefBase::Release();
}

ECode WifiStateTracker::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return NOERROR;
}

ECode WifiStateTracker::SetTeardownRequested(
    /* [in] */ Boolean isRequested)
{
    return mTeardownRequested->Set(isRequested);
}

ECode WifiStateTracker::IsTeardownRequested(
    /* [out] */ Boolean* isRequested)
{
    return mTeardownRequested->Get(isRequested);
}

ECode WifiStateTracker::StartMonitoring(
    /* [in] */ IContext* ctx,
    /* [in] */ IHandler* target)
{
    mCsHandler = target;
    mContext = ctx;

    mContext->GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&mWifiManager);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IWifiManager::NETWORK_STATE_CHANGED_ACTION);
    filter->AddAction(IWifiManager::LINK_CONFIGURATION_CHANGED_ACTION);
    filter->AddAction(IWifiP2pManager::WIFI_P2P_CONNECTION_CHANGED_ACTION);

    mWifiStateReceiver = (IBroadcastReceiver*)new WifiStateReceiver(this);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(mWifiStateReceiver, filter, (IIntent**)&intent);
    return NOERROR;
}

ECode WifiStateTracker::Teardown(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mTeardownRequested->Set(TRUE);
    mWifiManager->StopWifi(result);
    *result = TRUE;
    return NOERROR;
}

ECode WifiStateTracker::Reconnect(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mTeardownRequested->Set(FALSE);
    mWifiManager->StartWifi(result);
    *result = TRUE;
    return NOERROR;
}

ECode WifiStateTracker::CaptivePortalCheckComplete()
{
    return mWifiManager->CaptivePortalCheckComplete();
}

ECode WifiStateTracker::SetRadio(
    /* [in] */ Boolean turnOn,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mWifiManager->SetWifiEnabled(turnOn, result);
    *result = TRUE;
    return NOERROR;
}

ECode WifiStateTracker::IsAvailable(
    /* [out] */ Boolean* isAvailable)
{
    return mNetworkInfo->IsAvailable(isAvailable);
}

ECode WifiStateTracker::SetUserDataEnable(
    /* [in] */ Boolean enabled)
{
    Slogger::W(TAG, "ignoring setUserDataEnable(%d)", enabled);
    return NOERROR;
}

ECode WifiStateTracker::SetPolicyDataEnable(
    /* [in] */ Boolean enabled)
{
    // ignored
    return NOERROR;
}

ECode WifiStateTracker::IsPrivateDnsRouteSet(
    /* [out] */ Boolean* isSet)
{
    return mPrivateDnsRouteSet->Get(isSet);
}

ECode WifiStateTracker::SetPrivateDnsRoute(
    /* [in] */ Boolean enabled)
{
    return mPrivateDnsRouteSet->Set(enabled);
}

ECode WifiStateTracker::GetNetworkInfo(
    /* [out] */ INetworkInfo** info)
{
    VALIDATE_NOT_NULL(info);
    return CNetworkInfo::New(mNetworkInfo, info);
}

ECode WifiStateTracker::GetLinkProperties(
    /* [out] */ ILinkProperties** linkProperties)
{
    VALIDATE_NOT_NULL(linkProperties);
    return CLinkProperties::New(mLinkProperties, linkProperties);
}

ECode WifiStateTracker::GetLinkCapabilities(
    /* [out] */ ILinkCapabilities** linkCapabilities)
{
    VALIDATE_NOT_NULL(linkCapabilities);
    return CLinkCapabilities::New(mLinkCapabilities, linkCapabilities);
}

ECode WifiStateTracker::IsDefaultRouteSet(
    /* [out] */ Boolean* isSet)
{
    return mDefaultRouteSet->Get(isSet);
}

ECode WifiStateTracker::SetDefaultRoute(
    /* [in] */ Boolean enabled)
{
    return mDefaultRouteSet->Set(enabled);
}

ECode WifiStateTracker::GetTcpBufferSizesPropName(
    /* [out] */ String* propName)
{
    VALIDATE_NOT_NULL(propName);
    *propName = "net.tcp.buffersize.wifi";
    return NOERROR;
}

ECode WifiStateTracker::SetDependencyMet(
    /* [in] */ Boolean met)
{
    // not supported on this network
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
