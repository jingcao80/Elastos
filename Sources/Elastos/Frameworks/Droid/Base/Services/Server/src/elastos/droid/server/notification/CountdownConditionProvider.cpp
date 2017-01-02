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

#include <Elastos.Droid.Text.h>
#include "elastos/droid/server/notification/CountdownConditionProvider.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include "elastos/droid/text/format/DateUtils.h"

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Service::Notification::CCondition;
using Elastos::Droid::Service::Notification::IZenModeConfigHelper;
using Elastos::Droid::Service::Notification::CZenModeConfigHelper;
using Elastos::Droid::Text::Format::DateUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Utility::ILogHelper;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CDate;
using Elastos::Utility::IDate;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

static AutoPtr<IComponentName> InitCOMPONENT()
{
    AutoPtr<IComponentName> name;
    CComponentName::New(String("android"),
            String("Elastos.Droid.Server.Notification.CountdownConditionProvider"),
            (IComponentName**)&name);
    return name;
}

const AutoPtr<IComponentName> CountdownConditionProvider::COMPONENT = InitCOMPONENT();

const String CountdownConditionProvider::TAG("CountdownConditions");
const Boolean CountdownConditionProvider::DEBUG = Logger::IsLoggable("CountdownConditions", ILogHelper::DEBUG);

const String CountdownConditionProvider::ACTION("Elastos.Droid.Server.Notification.CountdownConditionProvider");
const Int32 CountdownConditionProvider::REQUEST_CODE = 100;
const String CountdownConditionProvider::EXTRA_CONDITION_ID("condition_id");

//===============================================================================
//                  CountdownConditionProvider::Receiver
//===============================================================================

CountdownConditionProvider::Receiver::Receiver(
    /* [in] */ CountdownConditionProvider* host)
    : mHost(host)
{}

CountdownConditionProvider::Receiver::~Receiver()
{}

ECode CountdownConditionProvider::Receiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (ACTION.Equals(action)) {
        AutoPtr<IParcelable> value;
        intent->GetParcelableExtra(EXTRA_CONDITION_ID, (IParcelable**)&value);
        AutoPtr<IUri> conditionId = IUri::Probe(value);
        AutoPtr<IZenModeConfigHelper> helper;
        CZenModeConfigHelper::AcquireSingleton((IZenModeConfigHelper**)&helper);
        Int64 time;
        helper->TryParseCountdownConditionId(conditionId, &time);
        if (DEBUG) Slogger::D(TAG, "Countdown condition fired: %p", conditionId.Get());
        if (time > 0) {
            mHost->NotifyCondition(mHost->NewCondition(time, ICondition::STATE_FALSE));
        }
    }
    return NOERROR;
}

//===============================================================================
//                  CountdownConditionProvider
//===============================================================================

CountdownConditionProvider::CountdownConditionProvider()
    : mConnected(FALSE)
    , mTime(0)
{
    if (DEBUG) Slogger::D(TAG, "new CountdownConditionProvider()");
    mContext = this;
    mReceiver = new Receiver(this);
}

CountdownConditionProvider::~CountdownConditionProvider()
{}

void CountdownConditionProvider::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ DumpFilter* filter)
{
    pw->Println(String("    CountdownConditionProvider:"));
    pw->Print(String("      mConnected="));
    pw->Println(mConnected);
    pw->Print(String("      mTime="));
    pw->Println(mTime);
}

ECode CountdownConditionProvider::OnConnected()
{
    if (DEBUG) Slogger::D(TAG, "onConnected");
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(ACTION, (IIntentFilter**)&filter);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(mReceiver, filter, (IIntent**)&intent);
    mConnected = TRUE;
    return NOERROR;
}

ECode CountdownConditionProvider::OnDestroy()
{
    ConditionProviderService::OnDestroy();
    if (DEBUG) Slogger::D(TAG, "onDestroy");
    if (mConnected) {
        mContext->UnregisterReceiver(mReceiver);
    }
    mConnected = FALSE;
    return NOERROR;
}

ECode CountdownConditionProvider::OnRequestConditions(
    /* [in] */ Int32 relevance)
{
    // by convention
    return NOERROR;
}

ECode CountdownConditionProvider::OnSubscribe(
    /* [in] */ IUri* conditionId)
{
    if (DEBUG) Slogger::D(TAG, "onSubscribe %p", conditionId);
    AutoPtr<IZenModeConfigHelper> helper;
    CZenModeConfigHelper::AcquireSingleton((IZenModeConfigHelper**)&helper);
    helper->TryParseCountdownConditionId(conditionId, &mTime);

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
    AutoPtr<IAlarmManager> alarms = IAlarmManager::Probe(obj);
    String str;
    IObject::Probe(conditionId)->ToString(&str);
    AutoPtr<IIntent> intent;
    CIntent::New(ACTION, (IIntent**)&intent);
    intent->PutExtra(EXTRA_CONDITION_ID, str);
    intent->SetFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);

    AutoPtr<IPendingIntentHelper> pendingIntentHelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pendingIntentHelper);
    AutoPtr<IPendingIntent> pendingIntent;
    pendingIntentHelper->GetBroadcast(mContext, REQUEST_CODE,
            intent, IPendingIntent::FLAG_UPDATE_CURRENT, (IPendingIntent**)&pendingIntent);
    alarms->Cancel(pendingIntent);
    if (mTime > 0) {
        AutoPtr<ISystem> sys;
        CSystem::AcquireSingleton((ISystem**)&sys);
        Int64 now;
        sys->GetCurrentTimeMillis(&now);

        AutoPtr<ICharSequence> span =
                DateUtils::GetRelativeTimeSpanString(mTime, now, IDateUtils::MINUTE_IN_MILLIS);
        if (mTime <= now) {
            // in the past, already false
            NotifyCondition(NewCondition(mTime, ICondition::STATE_FALSE));
        }
        else {
            // in the future, set an alarm
            alarms->SetExact(IAlarmManager::RTC_WAKEUP, mTime, pendingIntent);
        }

        String spanStr;
        span->ToString(&spanStr);

        str = "";
        str.AppendFormat("%s %s for %s, %s in the future (%s), now=%s",
                (mTime <= now ? "Not scheduling" : "Scheduling"),
                ACTION.string(),
                Ts(mTime).string(),
                StringUtils::ToString(mTime - now).string(),
                spanStr.string(),
                Ts(now).string());
        if (DEBUG) Slogger::D(TAG, str);
    }
    return NOERROR;
}

ECode CountdownConditionProvider::OnUnsubscribe(
    /* [in] */ IUri* conditionId)
{
    // noop
    return NOERROR;
}

AutoPtr<ICondition> CountdownConditionProvider::NewCondition(
    /* [in] */ Int64 time,
    /* [in] */ Int32 state)
{
    AutoPtr<IZenModeConfigHelper> helper;
    CZenModeConfigHelper::AcquireSingleton((IZenModeConfigHelper**)&helper);
    AutoPtr<IUri> uri;
    helper->ToCountdownConditionId(time, (IUri**)&uri);
    AutoPtr<ICondition> condition;
    CCondition::New(uri,
            String(""), String(""), String(""), 0, state,
            ICondition::FLAG_RELEVANT_NOW, (ICondition**)&condition);
    return condition;
}

String CountdownConditionProvider::TryParseDescription(
    /* [in] */ IUri* conditionUri)
{
    AutoPtr<IZenModeConfigHelper> helper;
    CZenModeConfigHelper::AcquireSingleton((IZenModeConfigHelper**)&helper);
    Int64 time;
    helper->TryParseCountdownConditionId(conditionUri, &time);
    if (time == 0) return String(NULL);

    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 now;
    sys->GetCurrentTimeMillis(&now);

    AutoPtr<ICharSequence> span =
            DateUtils::GetRelativeTimeSpanString(time, now, IDateUtils::MINUTE_IN_MILLIS);

    String spanStr;
    span->ToString(&spanStr);

    String str;
    str = "";
    str.AppendFormat("Scheduled for %s, %s in the future (%s), now=%s",
            Ts(time).string(),
            StringUtils::ToString(time - now).string(),
            spanStr.string(),
            Ts(now).string());

    return str;
}

String CountdownConditionProvider::Ts(
    /* [in] */ Int64 time)
{
    AutoPtr<IDate> date;
    CDate::New(time, (IDate**)&date);
    StringBuilder buider;
    buider += date;
    buider += " (";
    buider += time;
    buider += ")";
    return buider.ToString();
}

void CountdownConditionProvider::AttachBase(
    /* [in] */ IContext* base)
{
    AttachBaseContext(base);
}

} // Notification
} // Server
} // Droid
} // Elastos
