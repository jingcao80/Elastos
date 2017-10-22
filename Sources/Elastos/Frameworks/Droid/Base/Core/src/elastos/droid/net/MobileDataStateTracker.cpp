//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/net/MobileDataStateTracker.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/internal/utility/CAsyncChannel.h"
#include "elastos/droid/net/BaseNetworkStateTracker.h"
#include "elastos/droid/net/CLinkProperties.h"
#include "elastos/droid/net/MobileLinkQualityInfo.h"
#include "elastos/droid/net/CNetwork.h"
#include "elastos/droid/net/CNetworkCapabilities.h"
#include "elastos/droid/net/CNetworkInfo.h"
#include "elastos/droid/net/LinkProperties.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/CMessage.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/R.h"
// #include "elastos/droid/telephony/CTelephonyManager.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::IDctConstants;
using Elastos::Droid::Internal::Telephony::IITelephony;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Telephony::PhoneConstantsDataState_CONNECTED;
using Elastos::Droid::Internal::Telephony::PhoneConstantsDataState_CONNECTING;
using Elastos::Droid::Internal::Telephony::PhoneConstantsDataState_DISCONNECTED;
using Elastos::Droid::Internal::Telephony::PhoneConstantsDataState_SUSPENDED;
using Elastos::Droid::Internal::Utility::CAsyncChannel;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Net::CLinkProperties;
using Elastos::Droid::Net::CNetworkInfo;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Telephony::IPhoneStateListener;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::ISlog;

using Elastos::Core::StringUtils;
using Elastos::IO::CCharArrayWriter;
using Elastos::IO::CPrintWriter;
using Elastos::IO::ICharArrayWriter;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IWriter;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Net {

//==============================================================
// MobileDataStateTracker::MdstHandler
//==============================================================
MobileDataStateTracker::MdstHandler::MdstHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ MobileDataStateTracker* mdst)
    : Handler(looper)
{
    mMdst = mdst;
}

ECode MobileDataStateTracker::MdstHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    Int32 arg1;
    msg->GetArg1(&arg1);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (what) {
        case IAsyncChannel::CMD_CHANNEL_HALF_CONNECTED:
            if (arg1 == IAsyncChannel::STATUS_SUCCESSFUL) {
                if (VDBG) {
                    mMdst->Log("MdstHandler connected");
                }
                mMdst->mDataConnectionTrackerAc = IAsyncChannel::Probe(obj);
            }
            else {
                if (VDBG) {
                    mMdst->Log("MdstHandler \%s NOT connected error=%d", arg1);
                }
            }
            break;
        case IAsyncChannel::CMD_CHANNEL_DISCONNECTED:
            if (VDBG) mMdst->Log("Disconnected from DataStateTracker");
            mMdst->mDataConnectionTrackerAc = NULL;
            break;
        default: {
            String s;
            IObject::Probe(msg)->ToString(&s);
            if (VDBG) mMdst->Log("Ignorning unknown message=%s", s.string());
            break;
        }
    }
    return NOERROR;
}

//==============================================================
// MobileDataStateTracker::MobileDataStateReceiver
//==============================================================
MobileDataStateTracker::MobileDataStateReceiver::MobileDataStateReceiver(
    /* [in] */ MobileDataStateTracker* host)
    : mHost(host)
{}

ECode MobileDataStateTracker::MobileDataStateReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String act;
    intent->GetAction(&act);
    if (act.Equals(ITelephonyIntents::ACTION_DATA_CONNECTION_CONNECTED_TO_PROVISIONING_APN)) {
        String apnName;
        intent->GetStringExtra(IPhoneConstants::DATA_APN_KEY, &apnName);
        String apnType;
        intent->GetStringExtra(IPhoneConstants::DATA_APN_TYPE_KEY, &apnType);
        if (!TextUtils::Equals(mHost->mApnType, apnType)) {
            return NOERROR;
        }
        String action;
        intent->GetAction(&action);
        if (DBG) {
            mHost->Log("Broadcast received: %s apnType=%s apnName=%s", action.string(), apnType.string(), apnName.string());
        }

        // Make us in the connecting state until we make a new TYPE_MOBILE_PROVISIONING
        mHost->mMobileDataState = PhoneConstantsDataState_CONNECTING;
        mHost->UpdateLinkProperitesAndCapatilities(intent);
        mHost->mNetworkInfo->SetIsConnectedToProvisioningNetwork(TRUE);

        // Change state to SUSPENDED so setDetailedState
        // sends EVENT_STATE_CHANGED to connectivityService
        String s("");
        mHost->SetDetailedState(NetworkInfoDetailedState_SUSPENDED, s, apnName);
    }
    else if (act.Equals(ITelephonyIntents::ACTION_ANY_DATA_CONNECTION_STATE_CHANGED)) {
        String apnType;
        intent->GetStringExtra(IPhoneConstants::DATA_APN_TYPE_KEY, &apnType);
        if (VDBG) {
            mHost->Log("Broadcast received: ACTION_ANY_DATA_CONNECTION_STATE_CHANGED mApnType=%s %s received apnType=%s",
                    mHost->mApnType.string(),
                    TextUtils::Equals(apnType, mHost->mApnType) ? "==" : "!=", apnType.string());
        }
        if (!TextUtils::Equals(apnType, mHost->mApnType)) {
            return NOERROR;
        }

        // Assume this isn't a provisioning network.
        mHost->mNetworkInfo->SetIsConnectedToProvisioningNetwork(FALSE);
        if (DBG) {
            String action;
            intent->GetAction(&action);
            mHost->Log("Broadcast received: %s apnType=%s", action.string(), apnType.string());
        }

        Int32 oldSubtype;
        mHost->mNetworkInfo->GetSubtype(&oldSubtype);

        Int32 newSubType;
        AutoPtr<ITelephonyManager> telManager;
        // TODO: Waiting for TelephonyManager
        assert(0);
        // CTelephonyManager::GetDefault((ITelephonyManager**)&telManager);
        // telManager->GetNetworkType(&newSubType);

        String subTypeName;
        // telManager->GetNetworkTypeName(&subTypeName);

        mHost->mNetworkInfo->SetSubtype(newSubType, subTypeName);
        Boolean bol;
        mHost->mNetworkInfo->IsConnected(&bol);
        if (newSubType != oldSubtype && bol) {
            AutoPtr<IMessage> msg;
            mHost->mTarget->ObtainMessage(EVENT_NETWORK_SUBTYPE_CHANGED,
                    oldSubtype, 0, mHost->mNetworkInfo, (IMessage**)&msg);
            msg->SendToTarget();
        }

        // TODO: Waiting for Enum
        assert(0);
        PhoneConstantsDataState state;
        // state = Enum::ValueOf(PhoneConstantsDataState_class,
        //        intent->GetStringExtra(IPhoneConstants::STATE_KEY));
        String reason;
        intent->GetStringExtra(IPhoneConstants::STATE_CHANGE_REASON_KEY, &reason);
        String apnName;
        intent->GetStringExtra(IPhoneConstants::DATA_APN_KEY, &apnName);
        intent->GetBooleanExtra(IPhoneConstants::DATA_NETWORK_ROAMING_KEY, FALSE, &bol);
        mHost->mNetworkInfo->SetRoaming(bol);
//        if (VDBG) {
//            Log(mApnType + " setting isAvailable to " +
//                    intent.getBooleanExtra(IPhoneConstants::NETWORK_UNAVAILABLE_KEY,FALSE));
//        }
        intent->GetBooleanExtra(IPhoneConstants::NETWORK_UNAVAILABLE_KEY, FALSE, &bol);
        mHost->mNetworkInfo->SetIsAvailable(!bol);

//        if (DBG) {
//            Log("Received state=" + state + ", old=" + mMobileDataState +
//                ", reason=" + (reason == NULL ? "(unspecified)" : reason));
//        }
        if (mHost->mMobileDataState != state) {
            mHost->mMobileDataState = state;
            switch (state) {
                case PhoneConstantsDataState_DISCONNECTED:
                    Boolean bol;
                    mHost->IsTeardownRequested(&bol);
                    if(bol) {
                        mHost->SetTeardownRequested(FALSE);
                    }

                    mHost->SetDetailedState(NetworkInfoDetailedState_DISCONNECTED, reason, apnName);
                    // can't do this here - ConnectivityService needs it to clear stuff
                    // it's ok though - just leave it to be refreshed next time
                    // we connect.
                    //if (DBG) Log("clearing mInterfaceName for "+ mApnType +
                    //        " as it DISCONNECTED");
                    //mInterfaceName = NULL;
                    break;
                case PhoneConstantsDataState_CONNECTING:
                    mHost->SetDetailedState(NetworkInfoDetailedState_CONNECTING, reason, apnName);
                    break;
                case PhoneConstantsDataState_SUSPENDED:
                    mHost->SetDetailedState(NetworkInfoDetailedState_SUSPENDED, reason, apnName);
                    break;
                case PhoneConstantsDataState_CONNECTED:
                    mHost->UpdateLinkProperitesAndCapatilities(intent);
                    mHost->SetDetailedState(NetworkInfoDetailedState_CONNECTED, reason, apnName);
                    break;
            }
            if (VDBG) {
                Slogger::D(TAG, "TelephonyMgr.DataConnectionStateChanged");
                if (mHost->mNetworkInfo != NULL) {
                    String networkInfo;
                    IObject::Probe(mHost->mNetworkInfo)->ToString(&networkInfo);
                    Int32 subtype;
                    mHost->mNetworkInfo->GetSubtype(&subtype);
                    String subtypename;
                    mHost->mNetworkInfo->GetSubtypeName(&subtypename);
                    Slogger::D(TAG, "NetworkInfo = %s", networkInfo.string());
                    Slogger::D(TAG, "subType = %d", subtype);
                    Slogger::D(TAG, "subType = %s", subtypename.string());
                }
                if (mHost->mLinkProperties != NULL) {
                    String linkProperties;
                    IObject::Probe(mHost->mLinkProperties)->ToString(&linkProperties);
                    Slogger::D(TAG, "LinkProperties = %s", linkProperties.string());
                }
                else {
                    Slogger::D(TAG, "LinkProperties = ");
                }

                if (mHost->mNetworkCapabilities != NULL) {
                    String s;
                    IObject::Probe(mHost->mNetworkCapabilities)->ToString(&s);
                    Slogger::D(TAG, s.string());
                }
                else {
                    Slogger::D(TAG, "NetworkCapabilities = ");
                }
            }
        }
        else {
            // There was no state change. Check if LinkProperties has been updated.
            if (TextUtils::Equals(reason, IPhoneConstants::REASON_LINK_PROPERTIES_CHANGED)) {
                mHost->mLinkProperties = NULL;
                AutoPtr<IParcelable> parcelable;
                intent->GetParcelableExtra(
                        IPhoneConstants::DATA_LINK_PROPERTIES_KEY,
                        (IParcelable**)&(parcelable));
                mHost->mLinkProperties = ILinkProperties::Probe(parcelable);
                if (mHost->mLinkProperties == NULL) {
                    mHost->Loge("No link property in LINK_PROPERTIES change event.");
                    CLinkProperties::New((ILinkProperties**)&(mHost->mLinkProperties));
                }
                // Just update reason field in this NetworkInfo
                NetworkInfoDetailedState detailedState;
                mHost->mNetworkInfo->GetDetailedState(&detailedState);
                String extraInfo;
                mHost->mNetworkInfo->GetExtraInfo(&extraInfo);
                mHost->mNetworkInfo->SetDetailedState(detailedState, reason, extraInfo);
                AutoPtr<IMessage> msg;
                mHost->mTarget->ObtainMessage(EVENT_CONFIGURATION_CHANGED, mHost->mNetworkInfo, (IMessage**)&msg);
                msg->SendToTarget();
            }
        }
    }
    else {
        String action;
        intent->GetAction(&action);
        if (action.Equals(ITelephonyIntents::ACTION_DATA_CONNECTION_FAILED)) {
            String apnType;
            intent->GetStringExtra(IPhoneConstants::DATA_APN_TYPE_KEY, &apnType);
            if (!TextUtils::Equals(apnType, mHost->mApnType)) {
                if (DBG) {
                    mHost->Log("Broadcast received: ACTION_ANY_DATA_CONNECTION_FAILED ignore, mApnType=%s != received apnType=%s",
                        mHost->mApnType.string(), apnType.string());
                }
                return NOERROR;
            }
            // Assume this isn't a provisioning network.
            mHost->mNetworkInfo->SetIsConnectedToProvisioningNetwork(FALSE);
            String reason;
            intent->GetStringExtra(IPhoneConstants::FAILURE_REASON_KEY, &reason);
            String apnName;
            intent->GetStringExtra(IPhoneConstants::DATA_APN_KEY, &apnName);
            if (DBG) {
                mHost->Log("Broadcast received: %s reason=%s", action.string(), reason == NULL ? "null" : reason.string());
            }
            mHost->SetDetailedState(NetworkInfoDetailedState_FAILED, reason, apnName);
        }
        else {
            if (DBG) mHost->Log("Broadcast received: ignore %s", action.string());
        }
    }
    return NOERROR;
}

//====================================================================
// MobileDataStateTracker
//====================================================================
CAR_INTERFACE_IMPL(MobileDataStateTracker, BaseNetworkStateTracker, IMobileDataStateTracker)

AutoPtr<ArrayOf<MobileDataStateTracker::NetworkDataEntry*> > MobileDataStateTracker::mTheoreticalBWTable = MobileDataStateTracker::CreateTheoreticalBWTable();

const String MobileDataStateTracker::TAG("MobileDataStateTracker");
const Boolean MobileDataStateTracker::DBG = FALSE;
const Boolean MobileDataStateTracker::VDBG = FALSE;
const Int32 MobileDataStateTracker::UNKNOWN = ILinkQualityInfo::UNKNOWN_INT;

MobileDataStateTracker::MobileDataStateTracker()
    : mUserDataEnabled(TRUE)
    , mPolicyDataEnabled(TRUE)
    , mTeardownRequested(FALSE)
    , mPrivateDnsRouteSet(FALSE)
    , mDefaultRouteSet(FALSE)
{
    // There are still some vars need to be initialized. Like "private final PhoneStateListener mPhoneStateListener = new PhoneStateListener() {"
}

ECode MobileDataStateTracker::constructor(
    /* [in] */ Int32 netType,
    /* [in] */ const String& tag)
{
    AutoPtr<ITelephonyManager> telManager;
    // TODO: Waiting for TelephonyManager
    // CTelephonyManager::GetDefault((ITelephonyManager**)&telManager);
    Int32 n = 0;
    String str;
    // telManager->GetNetworkType(&n);
    // telManager->GetNetworkTypeName(&str);
    CNetworkInfo::New(netType, n, tag, str, (INetworkInfo**)&mNetworkInfo);
    NetworkTypeToApnType(netType, &mApnType);
    return NOERROR;
}

ECode MobileDataStateTracker::StartMonitoring(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* target)
{
    mTarget = target;
    mContext = context;

    AutoPtr<ILooper> looper;
    target->GetLooper((ILooper**)&looper);
    mHandler = new MdstHandler(looper, this);

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(ITelephonyIntents::ACTION_ANY_DATA_CONNECTION_STATE_CHANGED);
    filter->AddAction(ITelephonyIntents::ACTION_DATA_CONNECTION_FAILED);
    // TODO: Waiting for CDctConstants
    assert(0);
    // filter->AddAction(CDctConstants::ACTION_DATA_CONNECTION_TRACKER_MESSENGER);

    AutoPtr<MobileDataStateReceiver> dataReceiver = new MobileDataStateReceiver(this);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(dataReceiver, filter, (IIntent**)&intent);
    mMobileDataState = PhoneConstantsDataState_DISCONNECTED;
    AutoPtr<IInterface> tm;
    mContext->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&tm);
    // TODO: Waiting for TelephonyManager
    assert(0);
    // ITelephonyManager::Probe(tm)->Listen(mPhoneStateListener, PhoneStateListener.LISTEN_SIGNAL_STRENGTHS);
    return NOERROR;
}

ECode MobileDataStateTracker::IsPrivateDnsRouteSet(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mPrivateDnsRouteSet;
    return NOERROR;
}

ECode MobileDataStateTracker::PrivateDnsRouteSet(
    /* [in] */ Boolean enabled)
{
    mPrivateDnsRouteSet = enabled;
    return NOERROR;
}

ECode MobileDataStateTracker::GetNetworkInfo(
    /* [out] */ INetworkInfo** result)
{
    VALIDATE_NOT_NULL(result);

    *result = mNetworkInfo;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MobileDataStateTracker::IsDefaultRouteSet(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mDefaultRouteSet;
    return NOERROR;
}

ECode MobileDataStateTracker::DefaultRouteSet(
    /* [in] */ Boolean enabled)
{
    mDefaultRouteSet = enabled;
    return NOERROR;
}

ECode MobileDataStateTracker::ReleaseWakeLock()
{
    return NOERROR;
}

ECode MobileDataStateTracker::UpdateLinkProperitesAndCapatilities(
    /* [in] */ IIntent* intent)
{
    AutoPtr<IParcelable> parcelable;
    intent->GetParcelableExtra(
            IPhoneConstants::DATA_LINK_PROPERTIES_KEY, (IParcelable**)&parcelable);
    mLinkProperties = ILinkProperties::Probe(parcelable);
    if (mLinkProperties == NULL) {
        Loge("CONNECTED event did not supply link properties.");
        CLinkProperties::New((ILinkProperties**)&mLinkProperties);
    }
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Int32 integer;
    res->GetInteger(Elastos::Droid::R::integer::config_mobile_mtu, &integer);
    mLinkProperties->SetMtu(integer);
    AutoPtr<INetworkCapabilities> mNetworkCapabilities;
    parcelable = NULL;
    intent->GetParcelableExtra(IPhoneConstants::DATA_NETWORK_CAPABILITIES_KEY, (IParcelable**)&parcelable);
    mNetworkCapabilities = INetworkCapabilities::Probe(parcelable);
    if (mNetworkCapabilities == NULL) {
        Loge("CONNECTED event did not supply network capabilities.");
        CNetworkCapabilities::New((INetworkCapabilities**)&mNetworkCapabilities);
    }
    return NOERROR;
}

ECode MobileDataStateTracker::GetPhoneService(
    /* [in] */ Boolean forceRefresh)
{
    if ((mPhoneService == NULL) || forceRefresh) {
        AutoPtr<IInterface> service = ServiceManager::GetService(IContext::TELEPHONY_SERVICE);
        mPhoneService = IITelephony::Probe(service);
    }
    return NOERROR;
}

ECode MobileDataStateTracker::IsAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mNetworkInfo->IsAvailable(result);
}

ECode MobileDataStateTracker::GetTcpBufferSizesPropName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    String networkTypeStr("unknown");
    AutoPtr<ITelephonyManager> tm;
    // TODO: Waiting for TelephnoyManager
    assert(0);
    // CTelephonyManager::New(mContext, (ITelephonyManager**)&tm);
    //TODO We have to edit the parameter for getNetworkType regarding CDMA
    Int32 nType;
    // tm->GetNetworkType(&nType);
    // switch(nType) {
    // case ITelephonyManager::NETWORK_TYPE_GPRS:
    //     networkTypeStr = "gprs";
    //     break;
    // case ITelephonyManager::NETWORK_TYPE_EDGE:
    //     networkTypeStr = "edge";
    //     break;
    // case ITelephonyManager::NETWORK_TYPE_UMTS:
    //     networkTypeStr = "umts";
    //     break;
    // case ITelephonyManager::NETWORK_TYPE_HSDPA:
    //     networkTypeStr = "hsdpa";
    //     break;
    // case ITelephonyManager::NETWORK_TYPE_HSUPA:
    //     networkTypeStr = "hsupa";
    //     break;
    // case ITelephonyManager::NETWORK_TYPE_HSPA:
    //     networkTypeStr = "hspa";
    //     break;
    // case ITelephonyManager::NETWORK_TYPE_HSPAP:
    //     networkTypeStr = "hspap";
    //     break;
    // case ITelephonyManager::NETWORK_TYPE_CDMA:
    //     networkTypeStr = "cdma";
    //     break;
    // case ITelephonyManager::NETWORK_TYPE_1xRTT:
    //     networkTypeStr = "1xrtt";
    //     break;
    // case ITelephonyManager::NETWORK_TYPE_EVDO_0:
    //     networkTypeStr = "evdo";
    //     break;
    // case ITelephonyManager::NETWORK_TYPE_EVDO_A:
    //     networkTypeStr = "evdo";
    //     break;
    // case ITelephonyManager::NETWORK_TYPE_EVDO_B:
    //     networkTypeStr = "evdo";
    //     break;
    // case ITelephonyManager::NETWORK_TYPE_IDEN:
    //     networkTypeStr = "iden";
    //     break;
    // case ITelephonyManager::NETWORK_TYPE_LTE:
    // case ITelephonyManager::NETWORK_TYPE_IWLAN:
    //     networkTypeStr = "lte";
    //     break;
    // case ITelephonyManager::NETWORK_TYPE_EHRPD:
    //     networkTypeStr = "ehrpd";
    //     break;
    // default:
    //     Loge("unknown network type: %d", nType);
    // }
    *result = "net.tcp.buffersize.";
    *result += networkTypeStr;
    return NOERROR;
}

ECode MobileDataStateTracker::Teardown(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    SetTeardownRequested(TRUE);
    *result = (SetEnableApn(mApnType, FALSE) != IPhoneConstants::APN_REQUEST_FAILED);
    return NOERROR;
}

ECode MobileDataStateTracker::IsReady(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mDataConnectionTrackerAc != NULL;
    return NOERROR;
}

ECode MobileDataStateTracker::CaptivePortalCheckCompleted(
    /* [in] */ Boolean isCaptivePortal)
{
    Boolean b;
    mIsCaptivePortal->GetAndSet(isCaptivePortal, &b);
    if ( b != isCaptivePortal) {
        // Captive portal change enable/disable failing fast
        // TODO: Waiting for CDctConstants
        assert(0);
        // SetEnableFailFastMobileData(
        //         isCaptivePortal ? CDctConstants::ENABLED : CDctConstants::DISABLED);
    }
    return NOERROR;
}

ECode MobileDataStateTracker::SetDetailedState(
    /* [in] */ NetworkInfoDetailedState state,
    /* [in, out] */ String& reason,
    /* [in] */ const String& extraInfo)
{
    NetworkInfoDetailedState old_state;
    mNetworkInfo->GetDetailedState(&old_state);
    if (DBG) Log("setDetailed state, old =%d and new state=%d", old_state, state);
    if (state != old_state) {
        NetworkInfoState normal_state;
        mNetworkInfo->GetState(&normal_state);
        Boolean wasConnecting = (normal_state == NetworkInfoState_CONNECTING);
        String lastReason;
        mNetworkInfo->GetReason(&lastReason);
        /*
         * If a reason was supplied when the CONNECTING state was entered, and no
         * reason was supplied for entering the CONNECTED state, then retain the
         * reason that was supplied when going to CONNECTING.
         */
        if (wasConnecting && state == NetworkInfoDetailedState_CONNECTED && reason == NULL
                && lastReason != NULL)
            reason = lastReason;
        mNetworkInfo->SetDetailedState(state, reason, extraInfo);
        AutoPtr<INetworkInfo> newNetworkInfo;
        CNetworkInfo::New(mNetworkInfo, (INetworkInfo**)&newNetworkInfo);
        AutoPtr<IMessage> msg;
        mTarget->ObtainMessage(EVENT_STATE_CHANGED, newNetworkInfo, (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode MobileDataStateTracker::SetTeardownRequested(
    /* [in] */ Boolean isRequested)
{
    mTeardownRequested = isRequested;
    return NOERROR;
}

ECode MobileDataStateTracker::IsTeardownRequested(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mTeardownRequested;
    return NOERROR;
}

ECode MobileDataStateTracker::Reconnect(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean retValue = FALSE; //connected or expect to be?
    SetTeardownRequested(FALSE);
    String s(NULL);
    switch (SetEnableApn(mApnType, TRUE)) {
        case IPhoneConstants::APN_ALREADY_ACTIVE:
            // need to set self to CONNECTING so the below message is handled.
            retValue = TRUE;
            break;
        case IPhoneConstants::APN_REQUEST_STARTED:
            // set IDLE here , avoid the following second FAILED not sent out
            mNetworkInfo->SetDetailedState(NetworkInfoDetailedState_IDLE, s, String(NULL));
            retValue = TRUE;
            break;
        case IPhoneConstants::APN_REQUEST_FAILED:
        case IPhoneConstants::APN_TYPE_NOT_AVAILABLE:
            break;
        default:
            Loge("Error in reconnect - unexpected response.");
            break;
    }
    *result = retValue;
    return NOERROR;
}

ECode MobileDataStateTracker::SetRadio(
    /* [in] */ Boolean turnOn,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    GetPhoneService(FALSE);
    /*
     * If the phone process has crashed in the past, we'll get a
     * RemoteException and need to re-reference the service.
     */
    for (Int32 retry = 0; retry < 2; retry++) {
        if (mPhoneService == NULL) {
            Loge("Ignoring mobile radio request because could not acquire PhoneService");
            break;
        }

//        try {
        ECode ec = mPhoneService->SetRadio(turnOn, result);
//        } catch (RemoteException e) {
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            if (retry == 0) GetPhoneService(TRUE);
        }
        else if (FAILED(ec)) return ec;
//        }
    }

    Loge(TAG, "Could not set radio power to %s", (turnOn ? "on" : "off"));
    *result = FALSE;
    return NOERROR;
}

ECode MobileDataStateTracker::SetInternalDataEnable(
    /* [in] */ Boolean enabled)
{
    if (DBG) Log("setInternalDataEnable: E enabled=%s", enabled ? "TRUE" : "FALSE");
    const AutoPtr<IAsyncChannel> channel = mDataConnectionTrackerAc;
    if (channel != NULL) {
        // TODO: Waiting for CDctConstants
        assert(0);
        // channel->SendMessage(CDctConstants::EVENT_SET_INTERNAL_DATA_ENABLE,
        //         enabled ? CDctConstants::ENABLED : CDctConstants::DISABLED);
    }
    if (VDBG) Log("setInternalDataEnable: X enabled=%s", enabled ? "TRUE" : "FALSE");
    return NOERROR;
}

ECode MobileDataStateTracker::SetUserDataEnable(
    /* [in] */ Boolean enabled)
{
    if (DBG) Log("setUserDataEnable: E enabled=%s", enabled ? "TRUE" : "FALSE");
    AutoPtr<IAsyncChannel> channel = mDataConnectionTrackerAc;
    if (channel != NULL) {
        // TODO: Waiting for CDctConstants
        assert(0);
        // channel->SendMessage(CDctConstants::CMD_SET_USER_DATA_ENABLE,
        //         enabled ? CDctConstants::ENABLED : CDctConstants::DISABLED);
        mUserDataEnabled = enabled;
    }
    if (VDBG) Log("setUserDataEnable: X enabled=%s", enabled ? "TRUE" : "FALSE");
    return NOERROR;
}

ECode MobileDataStateTracker::SetPolicyDataEnable(
    /* [in] */ Boolean enabled)
{
    if (DBG) Log("setPolicyDataEnable(enabled=%s)", enabled ? "TRUE" : "FALSE");
    const AutoPtr<IAsyncChannel> channel = mDataConnectionTrackerAc;
    if (channel != NULL) {
        // TODO: Waiting for CDctConstants
        assert(0);
        // channel->SendMessage(CDctConstants::CMD_SET_POLICY_DATA_ENABLE,
        //         enabled ? CDctConstants::ENABLED : CDctConstants::DISABLED);
        mPolicyDataEnabled = enabled;
    }
    return NOERROR;
}

ECode MobileDataStateTracker::SetEnableFailFastMobileData(
    /* [in] */ Int32 enabled)
{
    if (DBG) Log("setEnableFailFastMobileData(enabled=%s)", enabled ? "TRUE" : "FALSE");
    const AutoPtr<IAsyncChannel> channel = mDataConnectionTrackerAc;
    if (channel != NULL) {
        channel->SendMessage(IDctConstants::CMD_SET_ENABLE_FAIL_FAST_MOBILE_DATA, enabled);
    }
    return NOERROR;
}

ECode MobileDataStateTracker::SetDependencyMet(
    /* [in] */ Boolean met)
{
    // TODO: Waiting for CDctConstants
    assert(0);
    // AutoPtr<IBundle> bundle = CBundle::ForPair(CDctConstants::APN_TYPE_KEY, mApnType);
    if (DBG) Log("setDependencyMet: E met=%s", met ? "TRUE" : "FALSE");
    AutoPtr<IMessage> msg = CMessage::Obtain();
    // TODO: Waiting for CDctConstants
    assert(0);
    // msg->SetWhat(CDctConstants::CMD_SET_DEPENDENCY_MET);
    // msg->SetArg1(met ? CDctConstants::ENABLED : CDctConstants::DISABLED);
    // msg->SetData(bundle);
    mDataConnectionTrackerAc->SendMessage(msg);
    if (VDBG) Log("setDependencyMet: X met=%s", met ? "TRUE" : "FALSE");
    return NOERROR;
}

ECode MobileDataStateTracker::EnableMobileProvisioning(
    /* [in] */ const String& url)
{
    if (DBG) Log("enableMobileProvisioning(url=%s)", url.string());
    const AutoPtr<IAsyncChannel> channel = mDataConnectionTrackerAc;
    if (channel != NULL) {
        AutoPtr<IMessage> msg = CMessage::Obtain();
        msg->SetWhat(IDctConstants::CMD_ENABLE_MOBILE_PROVISIONING);
        AutoPtr<IBundle> bundle = CBundle::ForPair(IDctConstants::PROVISIONING_URL_KEY, url);
        msg->SetData(bundle);
        channel->SendMessage(msg);
    }
    return NOERROR;
}

ECode MobileDataStateTracker::IsProvisioningNetwork(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

        // try {
    AutoPtr<IMessage> msg = CMessage::Obtain();
    msg->SetWhat(IDctConstants::CMD_IS_PROVISIONING_APN);
    // TODO: Waiting for CDctConstants
    assert(0);
    // AutoPtr<IBundle> bundle = CBundle::ForPair(CDctConstants::APN_TYPE_KEY, mApnType);
    // msg->SetData(bundle);
    AutoPtr<IMessage> result_msg;
    ECode ec = mDataConnectionTrackerAc->SendMessageSynchronously(msg, (IMessage**)&result_msg);
    if (!FAILED(ec)) {
        Int32 arg1;
        result_msg->GetArg1(&arg1);
        // TODO: Waiting for CDctConstants
        assert(0);
        // *result = arg1 == CDctConstants::ENABLED;
    }
        // } catch (NullPointerException e) {
    if (ec == (ECode)E_NULL_POINTER_EXCEPTION) {
        Loge("isProvisioningNetwork: X %08X", ec);
        *result = FALSE;
    }
    else if (FAILED(ec)) return ec;
        // }
    if (DBG) Log("isProvisioningNetwork: retVal=%s", *result ? "TRUE" : "FALSE");
    return NOERROR;
}

ECode MobileDataStateTracker::AddStackedLink(
    /* [in] */ ILinkProperties* link)
{
    Boolean b;
    mLinkProperties->AddStackedLink(link, &b);
    return NOERROR;
}

ECode MobileDataStateTracker::RemoveStackedLink(
    /* [in] */ ILinkProperties* link)
{
    Boolean b;
    mLinkProperties->RemoveStackedLink(link, &b);
    return NOERROR;
}

ECode MobileDataStateTracker::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICharArrayWriter> writer;
    CCharArrayWriter::New((ICharArrayWriter**)&writer);
    AutoPtr<IPrintWriter> pw;
    CPrintWriter::New(IWriter::Probe(writer), (IPrintWriter**)&pw);
    pw->Print(String("Mobile data state: "));
    pw->Println(mMobileDataState);
    pw->Print(String("Data enabled: user="));
    pw->Print(mUserDataEnabled);
    pw->Print(String(", policy="));
    pw->Println(mPolicyDataEnabled);
    IObject::Probe(writer)->ToString(result);
    return NOERROR;
}

Int32 MobileDataStateTracker::SetEnableApn(
    /* [in] */ const String& apnType,
    /* [in] */ Boolean enable)
{
    GetPhoneService(FALSE);
    /*
     * If the phone process has crashed in the past, we'll get a
     * RemoteException and need to re-reference the service.
     */
    for (Int32 retry = 0; retry < 2; retry++) {
        if (mPhoneService == NULL) {
            Loge("Ignoring feature request because could not acquire PhoneService");
            break;
        }

//        try {
//            if (enable) {
//                return mPhoneService->enableApnType(apnType);
//            } else {
//                return mPhoneService->disableApnType(apnType);
//            }
//        } catch (RemoteException e) {
//            if (retry == 0) GetPhoneService(TRUE);
//        }
    }

    Loge("Could not %s APN type \"%s\"", (enable ? "enable" : "disable"), apnType.string());
    return IPhoneConstants::APN_REQUEST_FAILED;
}

ECode MobileDataStateTracker::NetworkTypeToApnType(
    /* [in] */ Int32 netType,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    switch(netType) {
        case IConnectivityManager::TYPE_MOBILE:
            *result = IPhoneConstants::APN_TYPE_DEFAULT;  // TODO - use just one of these
            break;
        case IConnectivityManager::TYPE_MOBILE_MMS:
            *result = IPhoneConstants::APN_TYPE_MMS;
            break;
        case IConnectivityManager::TYPE_MOBILE_SUPL:
            *result = IPhoneConstants::APN_TYPE_SUPL;
            break;
        case IConnectivityManager::TYPE_MOBILE_DUN:
            *result = IPhoneConstants::APN_TYPE_DUN;
            break;
        case IConnectivityManager::TYPE_MOBILE_HIPRI:
            *result = IPhoneConstants::APN_TYPE_HIPRI;
            break;
        case IConnectivityManager::TYPE_MOBILE_FOTA:
            *result = IPhoneConstants::APN_TYPE_FOTA;
            break;
        case IConnectivityManager::TYPE_MOBILE_IMS:
            *result = IPhoneConstants::APN_TYPE_IMS;
            break;
        case IConnectivityManager::TYPE_MOBILE_CBS:
            *result = IPhoneConstants::APN_TYPE_CBS;
            break;
        case IConnectivityManager::TYPE_MOBILE_IA:
            *result = IPhoneConstants::APN_TYPE_IA;
            break;
        case IConnectivityManager::TYPE_MOBILE_EMERGENCY:
            *result = IPhoneConstants::APN_TYPE_EMERGENCY;
            break;
        default:
            Sloge("Error mapping networkType %d to apnType.", netType);
            *result = NULL;
    }
    return NOERROR;
}

ECode MobileDataStateTracker::GetLinkProperties(
    /* [out] */ ILinkProperties** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ILinkProperties> linkProp;
    CLinkProperties::New(mLinkProperties, (ILinkProperties**)&linkProp);
    *result = linkProp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MobileDataStateTracker::SupplyMessenger(
    /* [in] */ IMessenger* messenger)
{
    if (VDBG) Log("%s got supplyMessenger", mApnType.string());
    AutoPtr<IAsyncChannel> ac;
    CAsyncChannel::New((IAsyncChannel**)&ac);
    return ac->Connect(mContext, mHandler, messenger);
}

ECode MobileDataStateTracker::Log(
    /* [in] */ const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    Slogger::D(TAG, fmt, ap);
    va_end(ap);
    return NOERROR;
}

ECode MobileDataStateTracker::Loge(
    /* [in] */ const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    Slogger::E(TAG, fmt, ap);
    va_end(ap);
    return NOERROR;
}

ECode MobileDataStateTracker::Sloge(
    /* [in] */ const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    Slogger::E(TAG, fmt, ap);
    va_end(ap);
    return NOERROR;
}

ECode MobileDataStateTracker::GetLinkQualityInfo(
    /* [out] */ ILinkQualityInfo** result)
{
    VALIDATE_NOT_NULL(result);

    Int32 type;
    mNetworkInfo->GetType(&type);
    if (mNetworkInfo == NULL || type == IConnectivityManager::TYPE_NONE) {
        // no data available yet; just return
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<IMobileLinkQualityInfo> li = new MobileLinkQualityInfo();
    ILinkQualityInfo::Probe(li)->SetNetworkType(type);
    mSamplingDataTracker->SetCommonLinkQualityInfoFields(ILinkQualityInfo::Probe(li));
    Int32 subtype;
    mNetworkInfo->GetSubtype(&subtype);
    // TODO: Waiting for TelephonyManager
    assert(0);
    // if (subtype != ITelephonyManager::NETWORK_TYPE_UNKNOWN) {
    //     li->SetMobileNetworkType(subtype);
    //     AutoPtr<NetworkDataEntry> entry = GetNetworkDataEntry(subtype);
    //     if (entry != NULL) {
    //         li->SetTheoreticalRxBandwidth(entry->mDownloadBandwidth);
    //         li->SetTheoreticalRxBandwidth(entry->mUploadBandwidth);
    //         li->SetTheoreticalLatency(entry->mLatency);
    //     }
    //     if (mSignalStrength != NULL) {
    //         Int32 mobileNetworkType;
    //         li->GetMobileNetworkType(&mobileNetworkType);
    //         li->SetNormalizedSignalStrength(GetNormalizedSignalStrength(
    //                 mobileNetworkType, mSignalStrength));
    //     }
    // }
    AutoPtr<ISignalStrength> ss = mSignalStrength;
    if (ss != NULL) {
        // TODO: Waiting for SignalStrength
        assert(0);
        // Int32 gsmSignalStrength;
        // ss->GetGsmSignalStrength(&gsmSignalStrength);
        // li->SetRssi(gsmSignalStrength);
        // Int32 gsmBitErrorRate;
        // ss->GetGsmBitErrorRate(&gsmBitErrorRate);
        // li->SetGsmErrorRate(gsmBitErrorRate);
        // Int32 cdmaDbm;
        // ss->GetCdmaDbm(&cdmaDbm);
        // li->SetCdmaDbm(cdmaDbm);
        // Int32 cdmaEcio;
        // ss->GetCdmaEcio(&cdmaEcio);
        // li->SetCdmaEcio(cdmaEcio);
        // Int32 evdoDbm;
        // ss->GetEvdoDbm(&evdoDbm);
        // li->SetEvdoDbm(evdoDbm);
        // Int32 evdoEcio;
        // ss->GetEvdoEcio(&evdoEcio);
        // li->SetEvdoEcio(evdoEcio);
        // Int32 evdoSnr;
        // ss->GetEvdoSnr(&evdoSnr);
        // li->SetEvdoSnr(evdoSnr);
        // Int32 lteSignalStrength;
        // ss->GetLteSignalStrength(&lteSignalStrength);
        // li->SetLteSignalStrength(lteSignalStrength);
        // Int32 lteRsrp;
        // ss->GetLteRsrp(&lteRsrp);
        // li->SetLteRsrp(lteRsrp);
        // Int32 lteRsrq;
        // ss->GetLteRsrq(&lteRsrq);
        // li->SetLteRsrq(lteRsrq);
        // Int32 lteRssnr;
        // ss->GetLteRssnr(&lteRssnr);
        // li->SetLteRssnr(lteRssnr);
        // Int32 lteCqi;
        // ss->GetLteCqi(&lteCqi);
        // li->SetLteCqi(lteCqi);
    }
    if (VDBG) {
        Int32 mobileNetworkType;
        li->GetMobileNetworkType(&mobileNetworkType);
        Int32 theoreticalRxBandwidth;
        ILinkQualityInfo::Probe(li)->GetTheoreticalRxBandwidth(&theoreticalRxBandwidth);
        Int32 rssi;
        li->GetRssi(&rssi);
        Int32 cdmaDbm;
        li->GetCdmaDbm(&cdmaDbm);
        Int32 evdoDbm;
        li->GetEvdoDbm(&evdoDbm);
        Int32 lteSignalStrength;
        li->GetLteSignalStrength(&lteSignalStrength);
        Slogger::D(TAG, "Returning LinkQualityInfo with MobileNetworkType = %s Theoretical Rx BW = %s gsm Signal Strength = %s cdma Signal Strength = %s evdo Signal Strength = %s Lte Signal Strength = %s",
                StringUtils::ToString(mobileNetworkType).string(),
                StringUtils::ToString(theoreticalRxBandwidth).string(),
                StringUtils::ToString(rssi).string(),
                StringUtils::ToString(cdmaDbm).string(),
                StringUtils::ToString(evdoDbm).string(),
                StringUtils::ToString(lteSignalStrength).string());
    }
    *result = ILinkQualityInfo::Probe(li);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<MobileDataStateTracker::NetworkDataEntry> MobileDataStateTracker::GetNetworkDataEntry(
    /* [in] */ Int32 networkType)
{
    for (Int32 i = 0; i < mTheoreticalBWTable->GetLength(); ++i) {
        AutoPtr<NetworkDataEntry> entry = (*mTheoreticalBWTable)[i];
        if (entry->mNetworkType == networkType) {
            return entry;
        }
    }
    Slogger::E(TAG, "Could not find Theoretical BW entry for %d", networkType);
    return NULL;
}

Int32 MobileDataStateTracker::GetNormalizedSignalStrength(
    /* [in] */ Int32 networkType,
    /* [in] */ ISignalStrength* ss)
{
    Int32 level;
    // TODO: Waiting for TelephonyManager
    assert(0);
    // switch(networkType) {
    //     case ITelephonyManager::NETWORK_TYPE_EDGE:
    //     case ITelephonyManager::NETWORK_TYPE_GPRS:
    //     case ITelephonyManager::NETWORK_TYPE_UMTS:
    //     case ITelephonyManager::NETWORK_TYPE_HSDPA:
    //     case ITelephonyManager::NETWORK_TYPE_HSUPA:
    //     case ITelephonyManager::NETWORK_TYPE_HSPA:
    //     case ITelephonyManager::NETWORK_TYPE_HSPAP:
    //         ss->GetGsmLevel(&level);
    //         break;
    //     case ITelephonyManager::NETWORK_TYPE_CDMA:
    //     case ITelephonyManager::NETWORK_TYPE_1xRTT:
    //         ss.GetCdmaLevel(&level);
    //         break;
    //     case ITelephonyManager::NETWORK_TYPE_EVDO_0:
    //     case ITelephonyManager::NETWORK_TYPE_EVDO_A:
    //     case ITelephonyManager::NETWORK_TYPE_EVDO_B:
    //         ss.GetEvdoLevel(&level);
    //         break;
    //     case ITelephonyManager::NETWORK_TYPE_LTE:
    //         ss.GetLteLevel(&level);
    //         break;
    //     case ITelephonyManager::NETWORK_TYPE_IDEN:
    //     case ITelephonyManager::NETWORK_TYPE_EHRPD:
    //     default:
    //         return UNKNOWN;
    // }
    // return (level * ILinkQualityInfo::NORMALIZED_SIGNAL_STRENGTH_RANGE) /
    //         ISignalStrength::NUM_SIGNAL_STRENGTH_BINS;
    return 0;
}

ECode MobileDataStateTracker::StartSampling(
    /* [in] */ ISamplingDataTrackerSamplingSnapshot* s)
{
    return mSamplingDataTracker->StartSampling(s);
}

ECode MobileDataStateTracker::StopSampling(
    /* [in] */ ISamplingDataTrackerSamplingSnapshot* s)
{
    return mSamplingDataTracker->StopSampling(s);
}

AutoPtr<ArrayOf<MobileDataStateTracker::NetworkDataEntry*> > MobileDataStateTracker::CreateTheoreticalBWTable()
{
    AutoPtr<ArrayOf<NetworkDataEntry*> > rev;
    // TODO: Waiting for ITelephonyManager
    // NetworkDataEntry* array[] = {
    //     new NetworkDataEntry(ITelephonyManager::NETWORK_TYPE_EDGE,      237,     118, UNKNOWN),
    //     new NetworkDataEntry(ITelephonyManager::NETWORK_TYPE_GPRS,       48,      40, UNKNOWN),
    //     new NetworkDataEntry(ITelephonyManager::NETWORK_TYPE_UMTS,      384,      64, UNKNOWN),
    //     new NetworkDataEntry(ITelephonyManager::NETWORK_TYPE_HSDPA,   14400, UNKNOWN, UNKNOWN),
    //     new NetworkDataEntry(ITelephonyManager::NETWORK_TYPE_HSUPA,   14400,    5760, UNKNOWN),
    //     new NetworkDataEntry(ITelephonyManager::NETWORK_TYPE_HSPA,    14400,    5760, UNKNOWN),
    //     new NetworkDataEntry(ITelephonyManager::NETWORK_TYPE_HSPAP,   21000,    5760, UNKNOWN),
    //     new NetworkDataEntry(ITelephonyManager::NETWORK_TYPE_CDMA,  UNKNOWN, UNKNOWN, UNKNOWN),
    //     new NetworkDataEntry(ITelephonyManager::NETWORK_TYPE_1xRTT, UNKNOWN, UNKNOWN, UNKNOWN),
    //     new NetworkDataEntry(ITelephonyManager::NETWORK_TYPE_EVDO_0,   2468,     153, UNKNOWN),
    //     new NetworkDataEntry(ITelephonyManager::NETWORK_TYPE_EVDO_A,   3072,    1800, UNKNOWN),
    //     new NetworkDataEntry(ITelephonyManager::NETWORK_TYPE_EVDO_B,  14700,    1800, UNKNOWN),
    //     new NetworkDataEntry(ITelephonyManager::NETWORK_TYPE_IDEN,  UNKNOWN, UNKNOWN, UNKNOWN),
    //     new NetworkDataEntry(ITelephonyManager::NETWORK_TYPE_LTE,    100000,   50000, UNKNOWN),
    //     new NetworkDataEntry(ITelephonyManager::NETWORK_TYPE_EHRPD, UNKNOWN, UNKNOWN, UNKNOWN),
    // };
    // rev = ArrayOf<NetworkDataEntry>::Alloc(array, sizeof(array)/sizeof(NetworkDataEntry));
    return rev;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
