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

#include "elastos/droid/server/trust/TrustAgentWrapper.h"
#include "elastos/droid/server/trust/TrustArchive.h"
#include "elastos/droid/server/trust/TrustManagerService.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Net.h>
#include <elastos/core/Math.h>
#include <elastos/droid/Manifest.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::Math;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::App::CPendingIntent;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Manifest;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IPatternMatcher;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Service::Trust::EIID_IITrustAgentServiceCallback;
using Elastos::Droid::Service::Trust::ITrustAgentService;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Trust {

//=============================================================================
// TrustAgentWrapper::InnerSub_BroadcastReceiver
//=============================================================================
TrustAgentWrapper::InnerSub_BroadcastReceiver::InnerSub_BroadcastReceiver(
    /* [in] */ TrustAgentWrapper* host)
    : mHost(host)
{}

ECode TrustAgentWrapper::InnerSub_BroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoPtr<IParcelable> parcel;
    intent->GetParcelableExtra(EXTRA_COMPONENT_NAME, (IParcelable**)&parcel);
    AutoPtr<IComponentName> component = IComponentName::Probe(parcel);
    String action;
    intent->GetAction(&action);
    if (mHost->TRUST_EXPIRED_ACTION.Equals(action) && Object::Equals(mHost->mName, component)) {
        mHost->mHandler->RemoveMessages(MSG_TRUST_TIMEOUT);
        Boolean tmp;
        mHost->mHandler->SendEmptyMessage(MSG_TRUST_TIMEOUT, &tmp);
    }
    return NOERROR;
}

//=============================================================================
// TrustAgentWrapper::InnerSub_Handler
//=============================================================================
TrustAgentWrapper::InnerSub_Handler::InnerSub_Handler(
    /* [in] */ TrustAgentWrapper* host)
    : mHost(host)
{}

ECode TrustAgentWrapper::InnerSub_Handler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    if (what == TrustAgentWrapper::MSG_GRANT_TRUST) {
        Boolean connected;
        if (mHost->IsConnected(&connected), !connected) {
            String component;
            mHost->mName->FlattenToShortString(&component);
            Logger::W(TAG, "Agent is not connected, cannot grant trust: %s"
                    , component.string());
            return NOERROR;
        }
        mHost->mTrusted = TRUE;
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        mHost->mMessage = ICharSequence::Probe(obj);
        Int32 arg1;
        msg->GetArg1(&arg1);
        Boolean initiatedByUser = arg1 != 0;
        AutoPtr<IBundle> data;
        msg->GetData((IBundle**)&data);
        Int64 durationMs;
        data->GetInt64(DATA_DURATION, &durationMs);
        if (durationMs > 0) {
            Int64 duration;
            if (mHost->mMaximumTimeToLock != 0) {
                // Enforce DevicePolicyManager timeout.  This is here as a safeguard to
                // ensure trust agents are evaluating trust state at least as often as
                // the policy dictates. Admins that want more guarantees should be using
                // DevicePolicyManager#KEYGUARD_DISABLE_TRUST_AGENTS.
                duration = Elastos::Core::Math::Min(durationMs, mHost->mMaximumTimeToLock);
                if (DEBUG) {
                    Logger::V(TAG, "DPM lock timeout in effect. Timeout adjusted from %lld to %lld",
                            durationMs, duration);
                }
            }
            else {
                duration = durationMs;
            }
            Int64 expiration = SystemClock::GetElapsedRealtime() + duration;
            AutoPtr<IPendingIntentHelper> helper;
            CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
            helper->GetBroadcast(mHost->mContext, 0, mHost->mAlarmIntent,
                    IPendingIntent::FLAG_CANCEL_CURRENT, (IPendingIntent**)&mHost->mAlarmPendingIntent);
            mHost->mAlarmManager->Set(IAlarmManager::ELAPSED_REALTIME_WAKEUP, expiration,
                    mHost->mAlarmPendingIntent);
        }
        mHost->mTrustManagerService->mArchive->LogGrantTrust(mHost->mUserId, mHost->mName,
                (mHost->mMessage != NULL ? Object::ToString(mHost->mMessage) : String(NULL)),
                durationMs, initiatedByUser);
        mHost->mTrustManagerService->UpdateTrust(mHost->mUserId, initiatedByUser);
    }
    else if (what == TrustAgentWrapper::MSG_TRUST_TIMEOUT
            || what == TrustAgentWrapper::MSG_REVOKE_TRUST) {
        if (what == TrustAgentWrapper::MSG_TRUST_TIMEOUT) {
            if (DEBUG) {
                String component;
                mHost->mName->FlattenToShortString(&component);
                Slogger::V(TAG, "Trust timed out : %s", component.string());
            }
            mHost->mTrustManagerService->mArchive->LogTrustTimeout(mHost->mUserId, mHost->mName);
            mHost->OnTrustTimeout();
            // Fall through.
        }
        mHost->mTrusted = FALSE;
        mHost->mMessage = NULL;
        mHost->mHandler->RemoveMessages(TrustAgentWrapper::MSG_TRUST_TIMEOUT);
        if (what == TrustAgentWrapper::MSG_REVOKE_TRUST) {
            mHost->mTrustManagerService->mArchive->LogRevokeTrust(mHost->mUserId, mHost->mName);
        }
        mHost->mTrustManagerService->UpdateTrust(mHost->mUserId, FALSE);
    }
    else if (what == TrustAgentWrapper::MSG_RESTART_TIMEOUT) {
        mHost->Unbind();
        mHost->mTrustManagerService->ResetAgent(mHost->mName, mHost->mUserId);
    }
    else if (what == TrustAgentWrapper::MSG_SET_TRUST_AGENT_FEATURES_COMPLETED) {
        AutoPtr<IInterface> token;
        msg->GetObj((IInterface**)&token);
        Int32 arg1;
        msg->GetArg1(&arg1);
        Boolean result = arg1 != 0;
        if (mHost->mSetTrustAgentFeaturesToken.Get() == IBinder::Probe(token)) {
            mHost->mSetTrustAgentFeaturesToken = NULL;
            if (mHost->mTrustDisabledByDpm && result) {
                if (DEBUG) Logger::V(TAG, "Re-enabling agent because it acknowledged "
                        "enabled features: %s", TO_CSTR(mHost->mName));
                mHost->mTrustDisabledByDpm = FALSE;
                mHost->mTrustManagerService->UpdateTrust(mHost->mUserId, FALSE);
            }
        }
        else {
            if (DEBUG) Logger::W(TAG, "Ignoring MSG_SET_TRUST_AGENT_FEATURES_COMPLETED "
                    "with obsolete token: %s", TO_CSTR(mHost->mName));
        }
    }
    else if (what == TrustAgentWrapper::MSG_MANAGING_TRUST) {
        Int32 arg1;
        msg->GetArg1(&arg1);
        mHost->mManagingTrust = arg1 != 0;
        if (!mHost->mManagingTrust) {
            mHost->mTrusted = FALSE;
            mHost->mMessage = NULL;
        }
        mHost->mTrustManagerService->mArchive->LogManagingTrust(mHost->mUserId, mHost->mName, mHost->mManagingTrust);
        mHost->mTrustManagerService->UpdateTrust(mHost->mUserId, FALSE);
    }
    return NOERROR;
}

//=============================================================================
// TrustAgentWrapper::InnerSub_ITrustAgentServiceCallback
//=============================================================================
CAR_INTERFACE_IMPL(TrustAgentWrapper::InnerSub_ITrustAgentServiceCallback, Object, IITrustAgentServiceCallback)

TrustAgentWrapper::InnerSub_ITrustAgentServiceCallback::InnerSub_ITrustAgentServiceCallback(
    /* [in] */ TrustAgentWrapper* host)
    : mHost(host)
{}

ECode TrustAgentWrapper::InnerSub_ITrustAgentServiceCallback::GrantTrust(
    /* [in] */ ICharSequence* userMessage,
    /* [in] */ Int64 durationMs,
    /* [in] */ Boolean initiatedByUser)
{
    if (DEBUG) Slogger::V(TAG, "enableTrust(%s, durationMs = %lld, initiatedByUser = %s" ")",
            TO_CSTR(userMessage), durationMs, initiatedByUser ? "TRUE" : "FALSE");
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(
            MSG_GRANT_TRUST, initiatedByUser ? 1 : 0, 0, userMessage, (IMessage**)&msg);
    AutoPtr<IBundle> data;
    msg->GetData((IBundle**)&data);
    data->PutInt64(DATA_DURATION, durationMs);
    msg->SendToTarget();
    return NOERROR;
}

ECode TrustAgentWrapper::InnerSub_ITrustAgentServiceCallback::RevokeTrust()
{
    if (DEBUG) Slogger::V(TAG, "revokeTrust()");
    Boolean tmp;
    mHost->mHandler->SendEmptyMessage(MSG_REVOKE_TRUST, &tmp);
    return NOERROR;
}

ECode TrustAgentWrapper::InnerSub_ITrustAgentServiceCallback::SetManagingTrust(
    /* [in] */ Boolean managingTrust)
{
    if (DEBUG) Slogger::V(TAG, "managingTrust()");
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(MSG_MANAGING_TRUST, managingTrust ? 1 : 0, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode TrustAgentWrapper::InnerSub_ITrustAgentServiceCallback::OnSetTrustAgentFeaturesEnabledCompleted(
    /* [in] */ Boolean result,
    /* [in] */ IBinder* token)
{
    if (DEBUG) Slogger::V(TAG, "onSetTrustAgentFeaturesEnabledCompleted(result=%s", result ? "TRUE" : "FALSE");
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(MSG_SET_TRUST_AGENT_FEATURES_COMPLETED,
            result ? 1 : 0, 0, token, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

//=============================================================================
// TrustAgentWrapper::InnerSub_ServiceConnection
//=============================================================================
CAR_INTERFACE_IMPL(TrustAgentWrapper::InnerSub_ServiceConnection, Object, IServiceConnection)

TrustAgentWrapper::InnerSub_ServiceConnection::InnerSub_ServiceConnection(
    /* [in] */ TrustAgentWrapper* host)
    : mHost(host)
{}

ECode TrustAgentWrapper::InnerSub_ServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    if (DEBUG) {
        String component;
        name->FlattenToShortString(&component);
        Logger::V(TAG, "TrustAgent started : %s", component.string());
    }
    mHost->mHandler->RemoveMessages(MSG_RESTART_TIMEOUT);
    mHost->mTrustAgentService = IITrustAgentService::Probe(service);
    mHost->mTrustManagerService->mArchive->LogAgentConnected(mHost->mUserId, name);
    mHost->SetCallback(mHost->mCallback);
    Boolean tmp;
    mHost->UpdateDevicePolicyFeatures(&tmp);
    return NOERROR;
}

ECode TrustAgentWrapper::InnerSub_ServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    if (DEBUG) {
        String component;
        name->FlattenToShortString(&component);
        Logger::V(TAG, "TrustAgent disconnected : %s", component.string());
    }
    mHost->mTrustAgentService = NULL;
    mHost->mManagingTrust = FALSE;
    mHost->mSetTrustAgentFeaturesToken = NULL;
    mHost->mTrustManagerService->mArchive->LogAgentDied(mHost->mUserId, name);
    Boolean tmp;
    mHost->mHandler->SendEmptyMessage(MSG_REVOKE_TRUST, &tmp);
    if (mHost->mBound) {
        mHost->ScheduleRestart();
    }
    // mTrustDisabledByDpm maintains state
    return NOERROR;
}

//=============================================================================
// TrustAgentWrapper
//=============================================================================
const String TrustAgentWrapper::EXTRA_COMPONENT_NAME("componentName");
const String TrustAgentWrapper::TRUST_EXPIRED_ACTION("android.server.trust.TRUST_EXPIRED_ACTION");
const String TrustAgentWrapper::PERMISSION = Manifest::permission::PROVIDE_TRUST_AGENT;
const Boolean TrustAgentWrapper::DEBUG = FALSE;
const String TrustAgentWrapper::TAG("TrustAgentWrapper");
const Int32 TrustAgentWrapper::MSG_GRANT_TRUST = 1;
const Int32 TrustAgentWrapper::MSG_REVOKE_TRUST = 2;
const Int32 TrustAgentWrapper::MSG_TRUST_TIMEOUT = 3;
const Int32 TrustAgentWrapper::MSG_RESTART_TIMEOUT = 4;
const Int32 TrustAgentWrapper::MSG_SET_TRUST_AGENT_FEATURES_COMPLETED = 5;
const Int32 TrustAgentWrapper::MSG_MANAGING_TRUST = 6;
const Int64 TrustAgentWrapper::RESTART_TIMEOUT_MILLIS = 5 * 60000;
const String TrustAgentWrapper::DATA_DURATION("duration");

TrustAgentWrapper::TrustAgentWrapper()
    : mUserId(0)
    , mBound(FALSE)
    , mScheduledRestartUptimeMillis(0)
    , mMaximumTimeToLock(0)
    , mTrusted(FALSE)
    , mTrustDisabledByDpm(FALSE)
    , mManagingTrust(FALSE)
{}

ECode TrustAgentWrapper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ TrustManagerService* trustManagerService,
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user)
{
    mBroadcastReceiver = new InnerSub_BroadcastReceiver(this);
    mHandler = new InnerSub_Handler(this);
    mCallback = new InnerSub_ITrustAgentServiceCallback(this);

    mContext = context;
    mTrustManagerService = trustManagerService;
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
    mAlarmManager = IAlarmManager::Probe(obj);
    user->GetIdentifier(&mUserId);
    intent->GetComponent((IComponentName**)&mName);
    CIntent::New(TRUST_EXPIRED_ACTION, (IIntent**)&mAlarmIntent);
    mAlarmIntent->PutExtra(EXTRA_COMPONENT_NAME, IParcelable::Probe(mName));
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    String s;
    mAlarmIntent->ToUri(IIntent::URI_INTENT_SCHEME, &s);
    AutoPtr<IUri> uri;
    helper->Parse(s, (IUri**)&uri);
    mAlarmIntent->SetData(uri);
    String package;
    context->GetPackageName(&package);
    mAlarmIntent->SetPackage(package);
    AutoPtr<IIntentFilter> alarmFilter;
    CIntentFilter::New(TRUST_EXPIRED_ACTION, (IIntentFilter**)&alarmFilter);
    String scheme;
    mAlarmIntent->GetScheme(&scheme);
    alarmFilter->AddDataScheme(scheme);
    String pathUri;
    mAlarmIntent->ToUri(IIntent::URI_INTENT_SCHEME, &pathUri);
    alarmFilter->AddDataPath(pathUri, IPatternMatcher::PATTERN_LITERAL);
    AutoPtr<IIntent> tmp;
    mContext->RegisterReceiver(mBroadcastReceiver, alarmFilter, PERMISSION, NULL, (IIntent**)&tmp);

    // Schedules a restart for when connecting times out. If the connection succeeds,
    // the restart is canceled in mCallback's onConnected.
    ScheduleRestart();
    context->BindServiceAsUser(intent, mConnection, IContext::BIND_AUTO_CREATE, user, &mBound);
    if (!mBound) {
        String component;
        mName->FlattenToShortString(&component);
        Logger::E(TAG, "Can't bind to TrustAgent %s", component.string());
    }
    return NOERROR;
}

ECode TrustAgentWrapper::OnError(
    /* [in] */ ECode ec)
{
    Slogger::W(TAG , "Remote Exception %d", ec);
    return NOERROR;
}

ECode TrustAgentWrapper::OnTrustTimeout()
{
    // try {
    ECode ec = NOERROR;
    if (mTrustAgentService != NULL) ec = mTrustAgentService->OnTrustTimeout();
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode)E_REMOTE_EXCEPTION == ec)
            OnError(ec);
        else
            return ec;
    }
    // }
    return NOERROR;
}

ECode TrustAgentWrapper::OnUnlockAttempt(
    /* [in] */ Boolean successful)
{
    // try {
    ECode ec = NOERROR;
    if (mTrustAgentService != NULL) ec = mTrustAgentService->OnUnlockAttempt(successful);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode)E_REMOTE_EXCEPTION == ec)
            OnError(ec);
        else
            return ec;
    }
    // }
    return NOERROR;
}

ECode TrustAgentWrapper::SetCallback(
    /* [in] */ IITrustAgentServiceCallback* callback)
{
    // try {
    ECode ec = NOERROR;
    if (mTrustAgentService != NULL) {
        ec = mTrustAgentService->SetCallback(callback);
    }
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode)E_REMOTE_EXCEPTION == ec)
            OnError(ec);
        else
            return ec;
    }
    // }
    return NOERROR;
}

ECode TrustAgentWrapper::UpdateDevicePolicyFeatures(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean trustDisabled = FALSE;
    if (DEBUG) Slogger::V(TAG, "updateDevicePolicyFeatures(%s)", TO_CSTR(mName));
    // try {
    ECode ec = NOERROR;
    do {
        if (mTrustAgentService != NULL) {
            AutoPtr<IInterface> obj;
            ec = mContext->GetSystemService(IContext::DEVICE_POLICY_SERVICE, (IInterface**)&obj);
            if (FAILED(ec)) break;
            AutoPtr<IDevicePolicyManager> dpm = IDevicePolicyManager::Probe(obj);
            Int32 feature;
            ec = dpm->GetKeyguardDisabledFeatures(NULL, &feature);
            if (FAILED(ec)) break;
            if ((feature & IDevicePolicyManager::KEYGUARD_DISABLE_TRUST_AGENTS) != 0) {
                AutoPtr<IList> features;
                ec = dpm->GetTrustAgentFeaturesEnabled(NULL, mName, (IList**)&features);
                if (FAILED(ec)) break;
                trustDisabled = TRUE;
                if (DEBUG) Slogger::V(TAG, "Detected trust agents disabled. Features = %s", TO_CSTR(features));
                Int32 size;
                if (features != NULL && (features->GetSize(&size), size > 0)) {
                    AutoPtr<IBundle> bundle;
                    CBundle::New((IBundle**)&bundle);
                    ec = bundle->PutStringArrayList(ITrustAgentService::KEY_FEATURES,
                            IArrayList::Probe(features));
                    if (FAILED(ec)) break;
                    if (DEBUG) {
                        String component;
                        mName->FlattenToShortString(&component);
                        Slogger::V(TAG, "TrustAgent %s disabled until it acknowledges %s",
                                component.string(), TO_CSTR(features));
                    }
                    CBinder::New((IBinder**)&mSetTrustAgentFeaturesToken);
                    ec = mTrustAgentService->SetTrustAgentFeaturesEnabled(bundle,
                            mSetTrustAgentFeaturesToken);
                    if (FAILED(ec)) break;
                }
            }
            Int64 maxTimeToLock;
            ec = dpm->GetMaximumTimeToLock(NULL, &maxTimeToLock);
            if (FAILED(ec)) break;
            if (maxTimeToLock != mMaximumTimeToLock) {
                // If the timeout changes, cancel the alarm and send a timeout event to have
                // the agent re-evaluate trust.
                mMaximumTimeToLock = maxTimeToLock;
                if (mAlarmPendingIntent != NULL) {
                    ec = mAlarmManager->Cancel(mAlarmPendingIntent);
                    if (FAILED(ec)) break;
                    mAlarmPendingIntent = NULL;
                    Boolean tmp;
                    ec = mHandler->SendEmptyMessage(MSG_TRUST_TIMEOUT, &tmp);
                    if (FAILED(ec)) break;
                }
            }
        }
    } while(FALSE);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode)E_REMOTE_EXCEPTION == ec)
            OnError(ec);
        else
            return ec;
    }
    // }
    if (mTrustDisabledByDpm != trustDisabled) {
        mTrustDisabledByDpm = trustDisabled;
        mTrustManagerService->UpdateTrust(mUserId, FALSE);
    }
    *result = trustDisabled;
    return NOERROR;
}

ECode TrustAgentWrapper::IsTrusted(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mTrusted && mManagingTrust && !mTrustDisabledByDpm;
    return NOERROR;
}

ECode TrustAgentWrapper::IsManagingTrust(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mManagingTrust && !mTrustDisabledByDpm;
    return NOERROR;
}

ECode TrustAgentWrapper::GetMessage(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mMessage;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode TrustAgentWrapper::Unbind()
{
    if (!mBound) {
        return NOERROR;
    }
    if (DEBUG) {
        String component;
        mName->FlattenToShortString(&component);
        Logger::V(TAG, "TrustAgent unbound : %s", component.string());
    }
    mTrustManagerService->mArchive->LogAgentStopped(mUserId, mName);
    mContext->UnbindService(mConnection);
    mBound = FALSE;
    mTrustAgentService = NULL;
    mSetTrustAgentFeaturesToken = NULL;
    Boolean tmp;
    mHandler->SendEmptyMessage(MSG_REVOKE_TRUST, &tmp);
    mHandler->RemoveMessages(MSG_RESTART_TIMEOUT);
    return NOERROR;
}

ECode TrustAgentWrapper::IsConnected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mTrustAgentService != NULL;
    return NOERROR;
}

ECode TrustAgentWrapper::IsBound(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mBound;
    return NOERROR;
}

ECode TrustAgentWrapper::GetScheduledRestartUptimeMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mScheduledRestartUptimeMillis;
    return NOERROR;
}

ECode TrustAgentWrapper::ScheduleRestart()
{
    mHandler->RemoveMessages(MSG_RESTART_TIMEOUT);
    mScheduledRestartUptimeMillis = SystemClock::GetUptimeMillis() + RESTART_TIMEOUT_MILLIS;
    Boolean tmp;
    mHandler->SendEmptyMessageAtTime(MSG_RESTART_TIMEOUT, mScheduledRestartUptimeMillis, &tmp);
    return NOERROR;
}

} // namespace Trust
} // namespace Server
} // namespace Droid
} // namespace Elastos
