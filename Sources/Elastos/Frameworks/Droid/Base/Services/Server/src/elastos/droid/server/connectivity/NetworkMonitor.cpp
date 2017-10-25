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

#include <elastos/droid/server/connectivity/NetworkMonitor.h>
#include <elastos/droid/server/connectivity/NetworkAgentInfo.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Telephony.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Wifi.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Telephony::ICellIdentityCdma;
using Elastos::Droid::Telephony::ICellIdentityGsm;
using Elastos::Droid::Telephony::ICellIdentityLte;
using Elastos::Droid::Telephony::ICellIdentityWcdma;
using Elastos::Droid::Telephony::ICellInfo;
using Elastos::Droid::Telephony::ICellInfoCdma;
using Elastos::Droid::Telephony::ICellInfoGsm;
using Elastos::Droid::Telephony::ICellInfoLte;
using Elastos::Droid::Telephony::ICellInfoWcdma;
using Elastos::Droid::Internal::Utility::IProtocol;

using Elastos::Droid::Net::ITrafficStats;
using Elastos::Droid::Net::CTrafficStats;

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::IInputStream;
using Elastos::Utility::IMap;
using Elastos::Utility::IList;
using Elastos::Utility::IIterator;
using Elastos::Net::IURLConnection;
using Elastos::Net::IHttpURLConnection;
using Elastos::Net::IURLConnection;
using Elastos::Net::IURL;
using Elastos::Net::CURL;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Connectivity {


const String NetworkMonitor::ACTION_NETWORK_CONDITIONS_MEASURED("android.net.conn.NETWORK_CONDITIONS_MEASURED");

const String NetworkMonitor::EXTRA_CONNECTIVITY_TYPE("extra_connectivity_type");
const String NetworkMonitor::EXTRA_NETWORK_TYPE("extra_network_type");
const String NetworkMonitor::EXTRA_RESPONSE_RECEIVED("extra_response_received");
const String NetworkMonitor::EXTRA_IS_CAPTIVE_PORTAL("extra_is_captive_portal");
const String NetworkMonitor::EXTRA_CELL_ID("extra_cellid");
const String NetworkMonitor::EXTRA_SSID("extra_ssid");
const String NetworkMonitor::EXTRA_BSSID("extra_bssid");
const String NetworkMonitor::EXTRA_REQUEST_TIMESTAMP_MS("extra_request_timestamp_ms");
const String NetworkMonitor::EXTRA_RESPONSE_TIMESTAMP_MS("extra_response_timestamp_ms");

const Int32 NetworkMonitor::NETWORK_TEST_RESULT_VALID = 0;
const Int32 NetworkMonitor::NETWORK_TEST_RESULT_INVALID = 1;

const Int32 NetworkMonitor::BASE; // = IProtocol::BASE_NETWORK_MONITOR;
const Int32 NetworkMonitor::CMD_NETWORK_CONNECTED; // = BASE + 1;
const Int32 NetworkMonitor::EVENT_NETWORK_TESTED; // = BASE + 2;
const Int32 NetworkMonitor::CMD_NETWORK_LINGER; // = BASE + 3;
const Int32 NetworkMonitor::CMD_LINGER_EXPIRED; // = BASE + 4;
const Int32 NetworkMonitor::EVENT_NETWORK_LINGER_COMPLETE; // = BASE + 5;
const Int32 NetworkMonitor::CMD_REEVALUATE; // = BASE + 6;
const Int32 NetworkMonitor::CMD_NETWORK_DISCONNECTED; // = BASE + 7;
const Int32 NetworkMonitor::CMD_FORCE_REEVALUATION; // = BASE + 8;
const Int32 NetworkMonitor::CMD_CAPTIVE_PORTAL_LOGGED_IN; // = BASE + 9;
const Int32 NetworkMonitor::CMD_USER_WANTS_SIGN_IN; // = BASE + 10;
const Int32 NetworkMonitor::EVENT_PROVISIONING_NOTIFICATION; // = BASE + 11;
const Int32 NetworkMonitor::EVENT_APP_BYPASSED_CAPTIVE_PORTAL; // = BASE + 12;
const Int32 NetworkMonitor::EVENT_NO_APP_RESPONSE; // = BASE + 13;
const Int32 NetworkMonitor::EVENT_APP_INDICATES_SIGN_IN_IMPOSSIBLE; // = BASE + 14;

const Boolean NetworkMonitor::DBG = TRUE;
const String NetworkMonitor::TAG("NetworkMonitor");
const String NetworkMonitor::DEFAULT_SERVER("clients3.google.com");
const Int32 NetworkMonitor::SOCKET_TIMEOUT_MS = 10000;

const String NetworkMonitor::PERMISSION_ACCESS_NETWORK_CONDITIONS("android.permission.ACCESS_NETWORK_CONDITIONS");
const String NetworkMonitor::ACTION_CAPTIVE_PORTAL_LOGGED_IN("android.net.netmon.captive_portal_logged_in");
const String NetworkMonitor::LOGGED_IN_RESULT("result");
const String NetworkMonitor::LINGER_DELAY_PROPERTY("persist.netmon.linger");

const Int32 NetworkMonitor::DEFAULT_LINGER_DELAY_MS = 30000;
const String NetworkMonitor::REEVALUATE_DELAY_PROPERTY("persist.netmon.reeval_delay");
const Int32 NetworkMonitor::DEFAULT_REEVALUATE_DELAY_MS = 5000;

const Int32 NetworkMonitor::MAX_RETRIES = 10;
const Int32 NetworkMonitor::INVALID_UID = -1;

//==============================================================================
// NetworkMonitor::DefaultState
//==============================================================================

NetworkMonitor::DefaultState::DefaultState(
    /* [in] */ NetworkMonitor* host)
    : mHost(host)
{}

ECode NetworkMonitor::DefaultState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IState::HANDLED;

    if (NetworkMonitor::DBG) {
        StringBuilder sb(GetName());
        sb += Object::ToString(message);
        mHost->Log(sb.ToString());
    }

    Int32 what, arg1;
    message->GetWhat(&what);
    message->GetArg1(&arg1);
    switch (what) {
        case NetworkMonitor::CMD_NETWORK_LINGER:
            if (NetworkMonitor::DBG) mHost->Log(String("Lingering"));
            mHost->TransitionTo(mHost->mLingeringState);
            return NOERROR;
        case NetworkMonitor::CMD_NETWORK_CONNECTED:
            if (NetworkMonitor::DBG) mHost->Log(String("Connected"));
            mHost->TransitionTo(mHost->mEvaluatingState);
            return NOERROR;
        case NetworkMonitor::CMD_NETWORK_DISCONNECTED:
            if (NetworkMonitor::DBG) mHost->Log(String("Disconnected - quitting"));
            mHost->Quit();
            return NOERROR;
        case NetworkMonitor::CMD_FORCE_REEVALUATION:
            if (NetworkMonitor::DBG) mHost->Log(String("Forcing reevaluation"));
            mHost->mUidResponsibleForReeval = arg1;
            mHost->TransitionTo(mHost->mEvaluatingState);
            return NOERROR;
        default:
            return NOERROR;
    }
    return NOERROR;
}

String NetworkMonitor::DefaultState::GetName()
{
    return String("NetworkMonitor::DefaultState");
}

//==============================================================================
// NetworkMonitor::OfflineState
//==============================================================================

NetworkMonitor::OfflineState::OfflineState(
    /* [in] */ NetworkMonitor* host)
    : mHost(host)
{}

ECode NetworkMonitor::OfflineState::Enter()
{
    AutoPtr<IMessage> msg;
    mHost->ObtainMessage(
        NetworkMonitor::EVENT_NETWORK_TESTED,
        NetworkMonitor::NETWORK_TEST_RESULT_INVALID, 0,
        TO_IINTERFACE(mHost->mNetworkAgentInfo), (IMessage**)&msg);
    Boolean bval;
    mHost->mConnectivityServiceHandler->SendMessage(msg, &bval);
    return NOERROR;
}

ECode NetworkMonitor::OfflineState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IState::NOT_HANDLED;

    if (NetworkMonitor::DBG) {
        StringBuilder sb(GetName());
        sb += Object::ToString(message);
        mHost->Log(sb.ToString());
    }

    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case NetworkMonitor::CMD_FORCE_REEVALUATION:
            // If the user has indicated they explicitly do not want to use this network,
            // don't allow a reevaluation as this will be pointless and could result in
            // the user being annoyed with repeated unwanted notifications.
            *result = mHost->mUserDoesNotWant ? IState::HANDLED : IState::NOT_HANDLED;
        default:
            *result = IState::NOT_HANDLED;
    }
    return NOERROR;
}

String NetworkMonitor::OfflineState::GetName()
{
    return String("NetworkMonitor::OfflineState");
}

//==============================================================================
// NetworkMonitor::ValidatedState
//==============================================================================

NetworkMonitor::ValidatedState::ValidatedState(
    /* [in] */ NetworkMonitor* host)
    : mHost(host)
{}

ECode NetworkMonitor::ValidatedState::Enter()
{
    if (NetworkMonitor::DBG) mHost->Log(String("Validated"));

    AutoPtr<IMessage> msg;
    mHost->ObtainMessage(
        NetworkMonitor::EVENT_NETWORK_TESTED,
        NetworkMonitor::NETWORK_TEST_RESULT_VALID, 0,
        TO_IINTERFACE(mHost->mNetworkAgentInfo), (IMessage**)&msg);
    Boolean bval;
    mHost->mConnectivityServiceHandler->SendMessage(msg, &bval);
    return NOERROR;
}

ECode NetworkMonitor::ValidatedState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IState::NOT_HANDLED;

    if (NetworkMonitor::DBG) {
        StringBuilder sb(GetName());
        sb += Object::ToString(message);
        mHost->Log(sb.ToString());
    }

    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case NetworkMonitor::CMD_NETWORK_CONNECTED:
            mHost->TransitionTo(mHost->mValidatedState);
            *result = IState::HANDLED;
        default:
            *result = IState::NOT_HANDLED;
    }
    return NOERROR;
}

String NetworkMonitor::ValidatedState::GetName()
{
    return String("NetworkMonitor::ValidatedState");
}
//==============================================================================
// NetworkMonitor::EvaluatingState
//==============================================================================

NetworkMonitor::EvaluatingState::EvaluatingState(
    /* [in] */ NetworkMonitor* host)
    : mHost(host)
    , mRetries(0)
{}

ECode NetworkMonitor::EvaluatingState::Enter()
{
    mRetries = 0;
    mHost->SendMessage(NetworkMonitor::CMD_REEVALUATE, ++mHost->mReevaluateToken, 0);
    if (mHost->mUidResponsibleForReeval != NetworkMonitor::INVALID_UID) {
        AutoPtr<ITrafficStats> stats;
        CTrafficStats::AcquireSingleton((ITrafficStats**)&stats);
        stats->SetThreadStatsUid(mHost->mUidResponsibleForReeval);
        mHost->mUidResponsibleForReeval = NetworkMonitor::INVALID_UID;
    }
    return NOERROR;
}

ECode NetworkMonitor::EvaluatingState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IState::NOT_HANDLED;

    if (NetworkMonitor::DBG) {
        StringBuilder sb(GetName());
        sb += Object::ToString(message);
        mHost->Log(sb.ToString());
    }

    Int32 what, arg1;
    message->GetWhat(&what);
    message->GetArg1(&arg1);
    switch (what) {
        case NetworkMonitor::CMD_REEVALUATE: {
            if (arg1 != mHost->mReevaluateToken) {
                *result = IState::HANDLED;
                return NOERROR;
            }
            Boolean bval;
            mHost->mNetworkAgentInfo->IsVPN(&bval);
            if (bval) {
                mHost->TransitionTo(mHost->mValidatedState);
                *result = IState::HANDLED;
                return NOERROR;
            }
            // If network provides no internet connectivity adjust evaluation.
            mHost->mNetworkAgentInfo->mNetworkCapabilities->HasCapability(
                INetworkCapabilities::NET_CAPABILITY_INTERNET, &bval);
            if (!bval) {
                // TODO: Try to verify something works.  Do all gateways respond to pings?
                mHost->TransitionTo(mHost->mValidatedState);
                *result = IState::HANDLED;
                return NOERROR;
            }
            Int32 httpResponseCode = mHost->IsCaptivePortal();
            if (httpResponseCode == 204) {
                mHost->TransitionTo(mHost->mValidatedState);
            }
            else if (httpResponseCode >= 200 && httpResponseCode <= 399) {
                mHost->TransitionTo(mHost->mUserPromptedState);
            }
            else if (++mRetries > MAX_RETRIES) {
                mHost->TransitionTo(mHost->mOfflineState);
            }
            else if (mHost->mReevaluateDelayMs >= 0) {
                AutoPtr<IMessage> msg;
                mHost->ObtainMessage(
                    NetworkMonitor::CMD_REEVALUATE,
                    ++mHost->mReevaluateToken, 0, (IMessage**)&msg);
                mHost->SendMessageDelayed(msg, mHost->mReevaluateDelayMs);
            }
            *result = IState::HANDLED;
            return NOERROR;
        }
        case NetworkMonitor::CMD_FORCE_REEVALUATION: {
            // Ignore duplicate requests.
            *result = IState::HANDLED;
            return NOERROR;
        }
        default: {
            *result = IState::NOT_HANDLED;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode NetworkMonitor::EvaluatingState::Exit()
{
    AutoPtr<ITrafficStats> stats;
    CTrafficStats::AcquireSingleton((ITrafficStats**)&stats);
    stats->ClearThreadStatsUid();
    return NOERROR;
}

String NetworkMonitor::EvaluatingState::GetName()
{
    return String("NetworkMonitor::EvaluatingState");
}
//==============================================================================
// NetworkMonitor::CustomIntentReceiver
//==============================================================================

ECode NetworkMonitor::CustomIntentReceiver::constructor(
    /* [in] */ const String& action,
    /* [in] */ Int32 token,
    /* [in] */ Int32 message,
    /* [in] */ NetworkMonitor* host)
{
    mHost = host;
    mHost->ObtainMessage(message, token, (IMessage**)&mMessage);

    NetworkAgentInfo* nai = (NetworkAgentInfo*)mHost->mNetworkAgentInfo;
    Int32 id;
    nai->mNetwork->GetNetId(&id);
    StringBuilder sb(action);
    sb += "_";
    sb += id;
    sb += "_";
    sb += token;
    mAction = sb.ToString();
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(mAction, (IIntentFilter**)&filter);
    AutoPtr<IIntent> intent;
    return mHost->mContext->RegisterReceiver(this, filter, (IIntent**)&intent);
}

ECode NetworkMonitor::CustomIntentReceiver::GetPendingIntent(
    /* [out] */ IPendingIntent** pi)
{
    VALIDATE_NOT_NULL(pi)
    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    AutoPtr<IIntent> intent;
    CIntent::New(mAction, (IIntent**)&intent);
    return helper->GetBroadcast(mHost->mContext, 0, intent, 0, pi);
}

ECode NetworkMonitor::CustomIntentReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(mAction)) {
        mHost->SendMessage(mMessage);
    }
    return NOERROR;
}

//==============================================================================
// NetworkMonitor::UserPromptedState
//==============================================================================
// Intent broadcast when user selects sign-in notification.
const String NetworkMonitor::UserPromptedState::ACTION_SIGN_IN_REQUESTED("android.net.netmon.sign_in_requested");

NetworkMonitor::UserPromptedState::UserPromptedState(
    /* [in] */ NetworkMonitor* host)
    : mHost(host)
{}

ECode NetworkMonitor::UserPromptedState::Enter()
{
    AutoPtr<IMessage> msg;
    mHost->ObtainMessage(
        NetworkMonitor::EVENT_NETWORK_TESTED,
        NetworkMonitor::NETWORK_TEST_RESULT_INVALID, 0,
        TO_IINTERFACE(mHost->mNetworkAgentInfo), (IMessage**)&msg);
    Boolean bval;
    mHost->mConnectivityServiceHandler->SendMessage(msg, &bval);

    // Wait for user to select sign-in notifcation.
    mUserRespondedBroadcastReceiver = new CustomIntentReceiver();
    mUserRespondedBroadcastReceiver->constructor(
        ACTION_SIGN_IN_REQUESTED,
        ++mHost->mUserPromptedToken,
        NetworkMonitor::CMD_USER_WANTS_SIGN_IN, mHost);

    // Initiate notification to sign-in.
    NetworkAgentInfo* nai = (NetworkAgentInfo*)mHost->mNetworkAgentInfo;
    Int32 id;
    nai->mNetwork->GetNetId(&id);
    AutoPtr<IPendingIntent> pi;
    mUserRespondedBroadcastReceiver->GetPendingIntent((IPendingIntent**)&pi);

    AutoPtr<IMessage> message;
    mHost->ObtainMessage(
        NetworkMonitor::EVENT_PROVISIONING_NOTIFICATION, 1,
        id, pi.Get(), (IMessage**)&message);
    mHost->mConnectivityServiceHandler->SendMessage(message, &bval);
    return NOERROR;
}

ECode NetworkMonitor::UserPromptedState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IState::NOT_HANDLED;
    if (NetworkMonitor::DBG) {
        StringBuilder sb(GetName());
        sb += Object::ToString(message);
        mHost->Log(sb.ToString());
    }

    Int32 what, arg1;
    message->GetWhat(&what);
    message->GetArg1(&arg1);
    switch (what) {
        case NetworkMonitor::CMD_USER_WANTS_SIGN_IN:
            if (arg1 != mHost->mUserPromptedToken) {
                *result = IState::HANDLED;
                return NOERROR;
            }
            mHost->TransitionTo(mHost->mCaptivePortalState);
            *result = IState::HANDLED;
            return NOERROR;
        default:
            *result = IState::NOT_HANDLED;
            return NOERROR;
    }
    return NOERROR;
}

ECode NetworkMonitor::UserPromptedState::Exit()
{
    NetworkAgentInfo* nai = (NetworkAgentInfo*)mHost->mNetworkAgentInfo;
    Int32 id;
    nai->mNetwork->GetNetId(&id);

    AutoPtr<IMessage> msg;
    mHost->ObtainMessage(
        NetworkMonitor::EVENT_PROVISIONING_NOTIFICATION,
        0, id,
        NULL, (IMessage**)&msg);
    Boolean bval;
    mHost->mConnectivityServiceHandler->SendMessage(msg, &bval);

    mHost->mContext->UnregisterReceiver(mUserRespondedBroadcastReceiver);
    mUserRespondedBroadcastReceiver = NULL;
    return NOERROR;
}

String NetworkMonitor::UserPromptedState::GetName()
{
    return String("NetworkMonitor::UserPromptedState");
}

//==============================================================================
// NetworkMonitor::CaptivePortalLoggedInBroadcastReceiver
//==============================================================================
NetworkMonitor::CaptivePortalLoggedInBroadcastReceiver::CaptivePortalLoggedInBroadcastReceiver(
    /* [in] */ Int32 token,
    /* [in] */ NetworkMonitor* host)
    : mToken(token)
    , mHost(host)
{
}

ECode NetworkMonitor::CaptivePortalLoggedInBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    NetworkAgentInfo* nai = (NetworkAgentInfo*)mHost->mNetworkAgentInfo;
    Int32 id;
    nai->mNetwork->GetNetId(&id);

    String strVal;
    intent->GetStringExtra(IIntent::EXTRA_TEXT, &strVal);
    if (StringUtils::ParseInt32(strVal) == id) {
        intent->GetStringExtra(NetworkMonitor::LOGGED_IN_RESULT, &strVal);
        Int32 result = StringUtils::ParseInt32(strVal);
        AutoPtr<IMessage> msg;
        mHost->ObtainMessage(
            NetworkMonitor::CMD_CAPTIVE_PORTAL_LOGGED_IN,
            mToken, result, (IMessage**)&msg);
        mHost->SendMessage(msg);
    }
    return NOERROR;
}

//==============================================================================
// NetworkMonitor::CaptivePortalState
//==============================================================================
NetworkMonitor::CaptivePortalState::CaptivePortalState(
    /* [in] */ NetworkMonitor* host)
    : mHost(host)
{}

ECode NetworkMonitor::CaptivePortalState::Enter()
{
    NetworkAgentInfo* nai = (NetworkAgentInfo*)mHost->mNetworkAgentInfo;
    Int32 id;
    nai->mNetwork->GetNetId(&id);

    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_SEND, (IIntent**)&intent);
    intent->PutExtra(IIntent::EXTRA_TEXT, StringUtils::ToString(id));
    intent->SetType(String("text/plain"));
    AutoPtr<IComponentName> cn;
    CComponentName::New(
        String("com.android.captiveportallogin"),
        String("com.android.captiveportallogin.CaptivePortalLoginActivity"),
        (IComponentName**)&cn);
    intent->SetComponent(cn);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_BROUGHT_TO_FRONT | IIntent::FLAG_ACTIVITY_NEW_TASK);

    // Wait for result.
    mHost->mCaptivePortalLoggedInBroadcastReceiver = new CaptivePortalLoggedInBroadcastReceiver(
            ++mHost->mCaptivePortalLoggedInToken, mHost);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(NetworkMonitor::ACTION_CAPTIVE_PORTAL_LOGGED_IN, (IIntentFilter**)&filter);
    AutoPtr<IIntent> stickyIntent;
    mHost->mContext->RegisterReceiver(mHost->mCaptivePortalLoggedInBroadcastReceiver, filter, (IIntent**)&stickyIntent);
    // Initiate app to log in.
    mHost->mContext->StartActivityAsUser(intent, UserHandle::CURRENT);
    return NOERROR;
}

ECode NetworkMonitor::CaptivePortalState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IState::NOT_HANDLED;

    if (NetworkMonitor::DBG) {
        StringBuilder sb(GetName());
        sb += Object::ToString(message);
        mHost->Log(sb.ToString());
    }

    Int32 what, arg1, arg2;
    message->GetWhat(&what);
    message->GetArg1(&arg1);
    message->GetArg2(&arg2);
    switch (what) {
        case NetworkMonitor::CMD_CAPTIVE_PORTAL_LOGGED_IN:
            if (arg1 != mHost->mCaptivePortalLoggedInToken) {
                *result = IState::HANDLED;
                return NOERROR;
            }
            if (arg2 == 0) {
                mHost->mUserDoesNotWant = TRUE;
                // TODO: Should teardown network.
                mHost->TransitionTo(mHost->mOfflineState);
            }
            else {
                mHost->TransitionTo(mHost->mValidatedState);
            }
            *result = IState::HANDLED;
            return NOERROR;
        default:
            *result = IState::NOT_HANDLED;
            return NOERROR;
    }
    return NOERROR;
}

ECode NetworkMonitor::CaptivePortalState::Exit()
{
    mHost->mContext->UnregisterReceiver(mHost->mCaptivePortalLoggedInBroadcastReceiver);
    mHost->mCaptivePortalLoggedInBroadcastReceiver = NULL;
    return NOERROR;
}

String NetworkMonitor::CaptivePortalState::GetName()
{
    return String("NetworkMonitor::CaptivePortalState");
}
//==============================================================================
// NetworkMonitor::LingeringState
//==============================================================================

const String NetworkMonitor::LingeringState::ACTION_LINGER_EXPIRED("android.net.netmon.lingerExpired");

NetworkMonitor::LingeringState::LingeringState(
    /* [in] */ NetworkMonitor* host)
    : mHost(host)
{}

ECode NetworkMonitor::LingeringState::Enter()
{
    mBroadcastReceiver = new CustomIntentReceiver();
    mBroadcastReceiver->constructor(
        ACTION_LINGER_EXPIRED, ++mHost->mLingerToken,
        NetworkMonitor::CMD_LINGER_EXPIRED, mHost);
    mBroadcastReceiver->GetPendingIntent((IPendingIntent**)&mIntent);
    Int64 wakeupTime = SystemClock::GetElapsedRealtime() + mHost->mLingerDelayMs;
    mHost->mAlarmManager->SetWindow(
        IAlarmManager::ELAPSED_REALTIME_WAKEUP, wakeupTime,
        // Give a specific window so we aren't subject to unknown inexactitude.
        mHost->mLingerDelayMs / 6, mIntent);
    return NOERROR;
}

ECode NetworkMonitor::LingeringState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IState::NOT_HANDLED;
    if (NetworkMonitor::DBG) {
        StringBuilder sb(GetName());
        sb += Object::ToString(message);
        mHost->Log(sb.ToString());
    }

    Int32 what, arg1;
    message->GetWhat(&what);
    message->GetArg1(&arg1);
    switch (what) {
        case NetworkMonitor::CMD_NETWORK_CONNECTED:
            // Go straight to active as we've already evaluated.
            mHost->TransitionTo(mHost->mValidatedState);
            *result = IState::HANDLED;
            return NOERROR;
        case NetworkMonitor::CMD_LINGER_EXPIRED: {
            if (arg1 != mHost->mLingerToken) {
                *result = IState::HANDLED;
                return NOERROR;
            }
            AutoPtr<IMessage> msg;
            mHost->ObtainMessage(NetworkMonitor::EVENT_NETWORK_LINGER_COMPLETE,
                TO_IINTERFACE(mHost->mNetworkAgentInfo), (IMessage**)&msg);
            Boolean bval;
            mHost->mConnectivityServiceHandler->SendMessage(msg, &bval);
            *result = IState::HANDLED;
            return NOERROR;
        }
        case NetworkMonitor::CMD_FORCE_REEVALUATION:
            // Ignore reevaluation attempts when lingering.  A reevaluation could result
            // in a transition to the validated state which would abort the linger
            // timeout.  Lingering is the result of score assessment; validity is
            // irrelevant.
            *result = IState::HANDLED;
            return NOERROR;
        default:
            *result = IState::NOT_HANDLED;
            return NOERROR;
    }
    return NOERROR;
}

ECode NetworkMonitor::LingeringState::Exit()
{
    mHost->mAlarmManager->Cancel(mIntent);
    mHost->mContext->UnregisterReceiver(mBroadcastReceiver);
    return NOERROR;
}

String NetworkMonitor::LingeringState::GetName()
{
    return String("NetworkMonitor::LingeringState");
}

//==============================================================================
// NetworkMonitor
//==============================================================================

NetworkMonitor::NetworkMonitor(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ NetworkAgentInfo* networkAgentInfo)
    : mSystemReady(FALSE)
    , mLingerDelayMs(FALSE)
    , mLingerToken(FALSE)
    , mReevaluateDelayMs(FALSE)
    , mReevaluateToken(FALSE)
    , mUidResponsibleForReeval(INVALID_UID)
    , mCaptivePortalLoggedInToken(0)
    , mUserPromptedToken(0)
    , mIsCaptivePortalCheckEnabled(FALSE)
    , mUserDoesNotWant(FALSE)
{
    // Add suffix indicating which NetworkMonitor we're talking about.
    StateMachine::constructor(TAG + networkAgentInfo->Name());
    mDefaultState = new DefaultState(this);
    mOfflineState = new OfflineState(this);
    mValidatedState = new ValidatedState(this);
    mEvaluatingState = new EvaluatingState(this);
    mUserPromptedState = new UserPromptedState(this);
    mCaptivePortalState = new CaptivePortalState(this);
    mLingeringState = new LingeringState(this);

    mContext = context;
    mConnectivityServiceHandler = handler;
    mNetworkAgentInfo = networkAgentInfo;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
    mTelephonyManager = ITelephonyManager::Probe(obj);

    obj = NULL;
    context->GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&obj);
    mWifiManager = IWifiManager::Probe(obj);

    obj = NULL;
    context->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
    mAlarmManager = IAlarmManager::Probe(obj);

    AddState(mDefaultState);
    AddState(mOfflineState, mDefaultState);
    AddState(mValidatedState, mDefaultState);
    AddState(mEvaluatingState, mDefaultState);
    AddState(mUserPromptedState, mDefaultState);
    AddState(mCaptivePortalState, mDefaultState);
    AddState(mLingeringState, mDefaultState);
    SetInitialState(mDefaultState);

    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    sg->GetString(resolver, ISettingsGlobal::CAPTIVE_PORTAL_SERVER, &mServer);
    if (mServer.IsNull()) mServer = DEFAULT_SERVER;

    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    sysProp->GetInt32(LINGER_DELAY_PROPERTY, DEFAULT_LINGER_DELAY_MS, &mLingerDelayMs);
    sysProp->GetInt32(REEVALUATE_DELAY_PROPERTY, DEFAULT_REEVALUATE_DELAY_MS, &mReevaluateDelayMs);

    Int32 ival;
    sg->GetInt32(resolver, ISettingsGlobal::CAPTIVE_PORTAL_DETECTION_ENABLED, 0/*change from 1 leliang TODO*/, &ival);
    mIsCaptivePortalCheckEnabled = ival == 1;

    Start();
}

Int32 NetworkMonitor::IsCaptivePortal()
{
    if (!mIsCaptivePortalCheckEnabled) return 204;

    AutoPtr<IURLConnection> conn;
    AutoPtr<IHttpURLConnection> urlConnection;
    Int32 httpResponseCode = 599, length;
    Int64 requestTimestamp, responseTimestamp;
    AutoPtr<IInputStream> is;

    // try {
    AutoPtr<IURL> url;
    CURL::New(String("http"), mServer, String("/generate_204"), (IURL**)&url);
    if (DBG) {
        String str = Object::ToString(url);
        StringBuilder sb("Checking ");
        sb += str;
        sb += " on ";
        mNetworkAgentInfo->mNetworkInfo->GetExtraInfo(&str);
        sb += str;
        Log(sb.ToString());
    }

    ECode ec = mNetworkAgentInfo->mNetwork->OpenConnection(url, (IURLConnection**)&conn);
    FAIL_GOTO(ec, _EXIT_)

    urlConnection = IHttpURLConnection::Probe(conn);
    urlConnection->SetInstanceFollowRedirects(FALSE);
    conn->SetConnectTimeout(SOCKET_TIMEOUT_MS);
    conn->SetReadTimeout(SOCKET_TIMEOUT_MS);
    conn->SetUseCaches(FALSE);

    // Time how Int64 it takes to get a response to our request
    requestTimestamp = SystemClock::GetElapsedRealtime();

    ec = conn->GetInputStream((IInputStream**)&is);
    FAIL_GOTO(ec, _EXIT_)

    // Time how Int64 it takes to get a response to our request
    responseTimestamp = SystemClock::GetElapsedRealtime();

    ec = urlConnection->GetResponseCode(&httpResponseCode);
    FAIL_GOTO(ec, _EXIT_)
    if (DBG) {
        StringBuilder sb("IsCaptivePortal: ret=");
        sb += httpResponseCode;
        sb += " headers=";
        AutoPtr<IMap> map;
        conn->GetHeaderFields((IMap**)&map);
        sb += Object::ToString(map);
        Log(sb.ToString());
    }
    // NOTE: We may want to consider an "HTTP/1.0 204" response to be a captive
    // portal.  The only example of this seen so far was a captive portal.  For
    // the time being go with prior behavior of assuming it's not a captive
    // portal.  If it is considered a captive portal, a different sign-in URL
    // is needed (i.e. can't browse a 204).  This could be the result of an HTTP
    // proxy server.

    // Consider 200 response with "Content-length=0" to not be a captive portal.
    // There's no point in considering this a captive portal as the user cannot
    // sign-in to an empty page.  Probably the result of a broken transparent proxy.
    // See http://b/9972012.
    ec = conn->GetContentLength(&length);
    FAIL_GOTO(ec, _EXIT_)

    if (httpResponseCode == 200 && length == 0) {
        if (DBG) Log(String("Empty 200 response interpreted as 204 response."));
        httpResponseCode = 204;
    }

    SendNetworkConditionsBroadcast(TRUE /* response received */, httpResponseCode == 204,
        requestTimestamp, responseTimestamp);

_EXIT_:
    if (ec == (ECode)E_IO_EXCEPTION) {
        if (DBG) Log(String("Probably not a portal: IOException "));
        if (httpResponseCode == 599) {
            // TODO: Ping gateway and DNS server and log results.
        }
    }

    if (urlConnection != NULL) {
        urlConnection->Disconnect();
    }
    return httpResponseCode;
}

void NetworkMonitor::SendNetworkConditionsBroadcast(
    /* [in] */ Boolean responseReceived,
    /* [in] */ Boolean isCaptivePortal,
    /* [in] */ Int64 requestTimestampMs,
     /* [in] */ Int64 responseTimestampMs)
{
    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);

    Int32 ival;
    sg->GetInt32(resolver, ISettingsGlobal::WIFI_SCAN_ALWAYS_AVAILABLE, 0, &ival);
    if (ival == 0) {
        if (DBG) Log(String("Don't send network conditions - lacking user consent."));
        return;
    }

    if (mSystemReady == FALSE) return;

    AutoPtr<IIntent> latencyBroadcast;
    CIntent::New(ACTION_NETWORK_CONDITIONS_MEASURED, (IIntent**)&latencyBroadcast);
    Int32 type;
    mNetworkAgentInfo->mNetworkInfo->GetType(&type);
    switch (type) {
        case IConnectivityManager::TYPE_WIFI: {
            AutoPtr<IWifiInfo> currentWifiInfo;
            mWifiManager->GetConnectionInfo((IWifiInfo**)&currentWifiInfo);
            if (currentWifiInfo != NULL) {
                // NOTE: getSSID()'s behavior changed in API 17; before that, SSIDs were not
                // surrounded by double quotation marks (thus violating the Javadoc), but this
                // was changed to match the Javadoc in API 17. Since clients may have started
                // sanitizing the output of this method since API 17 was released, we should
                // not change it here as it would become impossible to tell whether the SSID is
                // simply being surrounded by quotes due to the API, or whether those quotes
                // are actually part of the SSID.
                String ssid, bssid;
                currentWifiInfo->GetSSID(&ssid);
                currentWifiInfo->GetBSSID(&bssid);
                latencyBroadcast->PutExtra(EXTRA_SSID, ssid);
                latencyBroadcast->PutExtra(EXTRA_BSSID, bssid);
            }
            else {
                if (DBG) Logw(String("network info is TYPE_WIFI but no ConnectionInfo found"));
                return;
            }
            break;
        }

        case IConnectivityManager::TYPE_MOBILE: {
            Int32 type;
            mTelephonyManager->GetNetworkType(&type);
            latencyBroadcast->PutExtra(EXTRA_NETWORK_TYPE, type);
            AutoPtr<IList> info;
            mTelephonyManager->GetAllCellInfo((IList**)&info);
            if (info == NULL) return;
            Int32 numRegisteredCellInfo = 0;
            Boolean hasNext, bval;
            AutoPtr<IIterator> it;
            info->GetIterator((IIterator**)&it);
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                ICellInfo* cellInfo = ICellInfo::Probe(obj);
                cellInfo->IsRegistered(&bval);
                if (bval) {
                    numRegisteredCellInfo++;
                    if (numRegisteredCellInfo > 1) {
                        if (DBG) Log(String("more than one registered CellInfo.  Can't tell which is active.  Bailing."));
                        return;
                    }
                    if (ICellInfoCdma::Probe(cellInfo)) {
                        AutoPtr<ICellIdentityCdma> cellId;
                        ICellInfoCdma::Probe(cellInfo)->GetCellIdentity((ICellIdentityCdma**)&cellId);
                        latencyBroadcast->PutExtra(EXTRA_CELL_ID, IParcelable::Probe(cellId));
                    }
                    else if (ICellInfoGsm::Probe(cellInfo)) {
                        AutoPtr<ICellIdentityGsm> cellId;
                        ICellInfoGsm::Probe(cellInfo)->GetCellIdentity((ICellIdentityGsm**)&cellId);
                        latencyBroadcast->PutExtra(EXTRA_CELL_ID, IParcelable::Probe(cellId));
                    }
                    else if (ICellInfoLte::Probe(cellInfo)) {
                        AutoPtr<ICellIdentityLte> cellId;
                        ICellInfoLte::Probe(cellInfo)->GetCellIdentity((ICellIdentityLte**)&cellId);
                        latencyBroadcast->PutExtra(EXTRA_CELL_ID, IParcelable::Probe(cellId));
                    }
                    else if (ICellInfoWcdma::Probe(cellInfo)) {
                        AutoPtr<ICellIdentityWcdma> cellId;
                        ICellInfoWcdma::Probe(cellInfo)->GetCellIdentity((ICellIdentityWcdma**)&cellId);
                        latencyBroadcast->PutExtra(EXTRA_CELL_ID, IParcelable::Probe(cellId));
                    }
                    else {
                        if (DBG) Logw(String("Registered cellinfo is unrecognized"));
                        return;
                    }
                }
            }
            break;
        }
        default:
            return;
    }

    latencyBroadcast->PutExtra(EXTRA_CONNECTIVITY_TYPE, type);
    latencyBroadcast->PutExtra(EXTRA_RESPONSE_RECEIVED, responseReceived);
    latencyBroadcast->PutExtra(EXTRA_REQUEST_TIMESTAMP_MS, requestTimestampMs);

    if (responseReceived) {
        latencyBroadcast->PutExtra(EXTRA_IS_CAPTIVE_PORTAL, isCaptivePortal);
        latencyBroadcast->PutExtra(EXTRA_RESPONSE_TIMESTAMP_MS, responseTimestampMs);
    }
    mContext->SendBroadcastAsUser(latencyBroadcast, UserHandle::CURRENT, PERMISSION_ACCESS_NETWORK_CONDITIONS);
}


} // Connectivity
} // Server
} // Droid
} // Elastos
