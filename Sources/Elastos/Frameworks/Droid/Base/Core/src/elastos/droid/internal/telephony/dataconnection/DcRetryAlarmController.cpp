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

#include "elastos/droid/internal/telephony/dataconnection/DcRetryAlarmController.h"
#include "elastos/droid/app/CPendingIntent.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/internal/telephony/dataconnection/DataConnection.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/text/TextUtils.h"
#include <Elastos.Droid.App.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CPendingIntent;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IRILConstants;
using Elastos::Droid::Internal::Utility::IStateMachine;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::Math;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

//=============================================================================
// DcRetryAlarmController::SubBroadcastReceiver
//=============================================================================
DcRetryAlarmController::SubBroadcastReceiver::SubBroadcastReceiver(
    /* [in] */ DcRetryAlarmController* host)
    : mHost(host)
{}

ECode DcRetryAlarmController::SubBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (TextUtils::IsEmpty(action)) {
        // Our mActionXxxx's could be null when disposed this could match an empty action.
        mHost->Log("onReceive: ignore empty action='%s'", action.string());
        return NOERROR;
    }
    if (TextUtils::Equals(action, mHost->mActionRetry)) {
        Boolean hasExtra;
        intent->HasExtra(INTENT_RETRY_ALARM_WHAT, &hasExtra);
        if (!hasExtra) {
            Logger::E("DcRetryAlarmController::SubBroadcastReceiver", mHost->mActionRetry + " has no INTENT_RETRY_ALRAM_WHAT");
            return E_RUNTIME_EXCEPTION;
        }
        intent->HasExtra(INTENT_RETRY_ALARM_TAG, &hasExtra);
        if (!hasExtra) {
            Logger::E("DcRetryAlarmController::SubBroadcastReceiver", "%s has no INTENT_RETRY_ALRAM_TAG", mHost->mActionRetry.string());
            return E_RUNTIME_EXCEPTION;
        }
        Int32 what;
        intent->GetInt32Extra(INTENT_RETRY_ALARM_WHAT, Elastos::Core::Math::INT32_MAX_VALUE, &what);
        Int32 tag;
        intent->GetInt32Extra(INTENT_RETRY_ALARM_TAG, Elastos::Core::Math::INT32_MAX_VALUE, &tag);
        if (DBG) {
            String whatToString;
            assert(0 && "TODO: IStateMachine");
            // IStateMachine::Probe(mHost->mDc)->GetWhatToString(what, &whatToString);
            mHost->Log("onReceive: action=%s sendMessage(what:%s, tag:%d)",
                    action.string(), whatToString.string(), tag);
        }
        AutoPtr<IMessage> msg;
        IStateMachine::Probe(mHost->mDc)->ObtainMessage(what, tag, 0, (IMessage**)&msg);
        IStateMachine::Probe(mHost->mDc)->SendMessage(msg);
    } else {
        if (DBG) mHost->Log("onReceive: unknown action=%s", action.string());
    }
    return NOERROR;
}

//=============================================================================
// DcRetryAlarmController
//=============================================================================
CAR_INTERFACE_IMPL(DcRetryAlarmController, Object, IDcRetryAlarmController)

const Boolean DcRetryAlarmController::DBG = TRUE;
const String DcRetryAlarmController::INTENT_RETRY_ALARM_WHAT("what");
const String DcRetryAlarmController::INTENT_RETRY_ALARM_TAG("tag");

DcRetryAlarmController::DcRetryAlarmController()
    : mLogTag("DcRac")
{
    mIntentReceiver = new SubBroadcastReceiver(this);
}

ECode DcRetryAlarmController::constructor(
    /* [in] */ IPhoneBase* phone,
    /* [in] */ IDataConnection* dc)
{
    IStateMachine::Probe(dc)->GetName(&mLogTag);
    mPhone = phone;
    mDc = dc;
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
    mAlarmManager = IAlarmManager::Probe(obj);
    String canonicalName("CDataConnection"); // = mDc->GetClass()->GetCanonicalName();
    String name;
    IStateMachine::Probe(mDc)->GetName(&name);
    mActionRetry = canonicalName + "." + name + ".action_retry";
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(mActionRetry);
    Log("DcRetryAlarmController: register for intent action=%s", mActionRetry.string());
    AutoPtr<IHandler> handler;
    IStateMachine::Probe(mDc)->GetHandler((IHandler**)&handler);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(mIntentReceiver, filter, String(NULL), handler, (IIntent**)&intent);
    return NOERROR;
}

ECode DcRetryAlarmController::Dispose()
{
    if (DBG) Log("dispose");
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    context->UnregisterReceiver(mIntentReceiver);
    mPhone = NULL;
    mDc = NULL;
    mAlarmManager = NULL;
    mActionRetry = NULL;
    return NOERROR;
}

ECode DcRetryAlarmController::GetSuggestedRetryTime(
    /* [in] */ IDataConnection* dc,
    /* [in] */ IAsyncResult* ar,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 retryDelay;
    AutoPtr<IDataCallResponse> response = IDataCallResponse::Probe(((AsyncResult*) ar)->mResult);
    response->GetSuggestedRetryTime(&retryDelay);
    Boolean isRetryNeeded;
    ((DataConnection*) dc)->mRetryManager->IsRetryNeeded(&isRetryNeeded);
    if (retryDelay == IRILConstants::MAX_INT) {
        if (DBG) Log("getSuggestedRetryTime: suggestedRetryTime is MAX_INT, retry NOT needed");
        retryDelay = -1;
    } else if (retryDelay >= 0) {
        if (DBG) Log("getSuggestedRetryTime: suggestedRetryTime is >= 0 use it");
    } else if (isRetryNeeded) {
        ((DataConnection*) dc)->mRetryManager->GetRetryTimer(&retryDelay);
        if (retryDelay < 0) {
            retryDelay = 0;
        }
        if (DBG) Log("getSuggestedRetryTime: retry is needed");
    } else {
        if (DBG) Log("getSuggestedRetryTime: retry is NOT needed");
        retryDelay = -1;
    }
    if (DBG) {
        Log("getSuggestedRetryTime: %d response=%s dc=%s", retryDelay, TO_CSTR(response), TO_CSTR(dc));
    }
    *result = retryDelay;
    return NOERROR;
}

ECode DcRetryAlarmController::StartRetryAlarm(
    /* [in] */ Int32 what,
    /* [in] */ Int32 tag,
    /* [in] */ Int32 delay)
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->PutExtra(INTENT_RETRY_ALARM_WHAT, what);
    intent->PutExtra(INTENT_RETRY_ALARM_TAG, tag);
    if (DBG) {
        Log("startRetryAlarm: next attempt in %ds what=%d tag=%d",
                (delay / 1000), what, tag);
    }
    AutoPtr<IContext> context;
    IPhone::Probe(mPhone)->GetContext((IContext**)&context);
    AutoPtr<IPendingIntent> retryIntent;
    CPendingIntent::GetBroadcast(context, 0,
            intent, IPendingIntent::FLAG_UPDATE_CURRENT, (IPendingIntent**)&retryIntent);
    mAlarmManager->Set(IAlarmManager::ELAPSED_REALTIME_WAKEUP,
            SystemClock::GetElapsedRealtime() + delay, retryIntent);
    return NOERROR;
}

ECode DcRetryAlarmController::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    StringBuilder sb;
    sb.Append(mLogTag);
    sb.Append(" [dcRac] ");
    sb.Append(" mPhone=");
    sb.Append(mPhone);
    sb.Append(" mDc=");
    sb.Append(mDc);
    sb.Append(" mAlaramManager=");
    sb.Append(mAlarmManager);
    sb.Append(" mActionRetry=");
    sb.Append(mActionRetry);
    *result = sb.ToString();
    return NOERROR;
}

#define MSG_BUF_SIZE    1024
ECode DcRetryAlarmController::Log(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return Logger::D(mLogTag, "[dcRac] %s", msgBuf);
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
