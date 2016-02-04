
#include <Elastos.Droid.Text.h>
#include <Elastos.CoreLibrary.Text.h>
#include "elastos/droid/server/notification/DowntimeConditionProvider.h"
#include "elastos/droid/server/notification/ZenLog.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/Objects.h>

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Service::Notification::CCondition;
using Elastos::Droid::Service::Notification::CConditionHelper;
using Elastos::Droid::Service::Notification::IConditionHelper;
using Elastos::Droid::Service::Notification::IZenModeConfigHelper;
using Elastos::Droid::Service::Notification::CZenModeConfigHelper;
using Elastos::Droid::Text::Format::IDateFormat;
using Elastos::Droid::Text::Format::CDateFormat;
using Elastos::Droid::Utility::ILogHelper;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::R;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CDate;
using Elastos::Utility::IDate;
using Elastos::Utility::ILocale;
using Elastos::Utility::ISet;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

static AutoPtr<IComponentName> InitCOMPONENT()
{
    AutoPtr<IComponentName> name;
    CComponentName::New(String("android"),
            String("Elastos.Droid.Server.Notification.DowntimeConditionProvider"),
            (IComponentName**)&name);
    return name;
}

const AutoPtr<IComponentName> DowntimeConditionProvider::COMPONENT = InitCOMPONENT();

const String DowntimeConditionProvider::TAG("DowntimeConditions");
const Boolean DowntimeConditionProvider::DEBUG = Logger::IsLoggable("DowntimeConditions", ILogHelper::DEBUG);

const String DowntimeConditionProvider::ENTER_ACTION = TAG + String(".enter");
const Int32 DowntimeConditionProvider::ENTER_CODE = 100;
const String DowntimeConditionProvider::EXIT_ACTION = TAG + String(".exit");
const Int32 DowntimeConditionProvider::EXIT_CODE = 101;
const String DowntimeConditionProvider::EXTRA_TIME("time");

//===============================================================================
//                  DowntimeConditionProvider::MyBroadcastReceiver
//===============================================================================

DowntimeConditionProvider::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ DowntimeConditionProvider* host)
    : mHost(host)
{}

DowntimeConditionProvider::MyBroadcastReceiver::~MyBroadcastReceiver()
{}

ECode DowntimeConditionProvider::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 now;
    sys->GetCurrentTimeMillis(&now);
    if (ENTER_ACTION.Equals(action) || EXIT_ACTION.Equals(action)) {
        Int64 schTime;
        intent->GetInt64Extra(EXTRA_TIME, 0, &schTime);
        if (DEBUG) {
            String str("");
            str.AppendFormat("%s scheduled for %s, fired at %s, delta=%s",
                    action.string(),
                    Ts(schTime).string(),
                    Ts(now).string(),
                    StringUtils::ToString(now - schTime).string());
            Slogger::D(TAG, str);
        }
    }
    else if (IIntent::ACTION_TIMEZONE_CHANGED.Equals(action)) {
        AutoPtr<ITimeZoneHelper> helper;
        CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&helper);
        AutoPtr<ITimeZone> zone;
        helper->GetDefault((ITimeZone**)&zone);
        if (DEBUG) Slogger::D(TAG, "timezone changed to %p", zone.Get());
        mHost->mCalendar->SetTimeZone(zone);
    }
    else {
        if (DEBUG) Slogger::D(TAG, "%s fired at %lld", action.string(), now);
    }
    mHost->ReevaluateDowntime();
    mHost->UpdateAlarms();
    return NOERROR;
}

//===============================================================================
//                  DowntimeConditionProvider
//===============================================================================

DowntimeConditionProvider::DowntimeConditionProvider()
    : mConnected(FALSE)
    , mInDowntime(FALSE)
{
    if (DEBUG) Slogger::D(TAG, "new DowntimeConditionProvider()");

    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    helper->GetInstance((ICalendar**)&mCalendar);
    mContext = THIS_PROBE(IContext);
    CArraySet::New((IArraySet**)&mDays);
    mReceiver = new MyBroadcastReceiver(this);
}

DowntimeConditionProvider::~DowntimeConditionProvider()
{}

void DowntimeConditionProvider::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ DumpFilter* filter)
{
    pw->Println(String("    DowntimeConditionProvider:"));
    pw->Print(String("      mConnected="));
    pw->Println(mConnected);
    pw->Print(String("      mInDowntime="));
    pw->Println(mInDowntime);
}

void DowntimeConditionProvider::AttachBase(
    /* [in] */ IContext* base)
{
    AttachBaseContext(base);
}

void DowntimeConditionProvider::SetCallback(
    /* [in] */ IDowntimeConditionProviderCallback* callback)
{
    mCallback = callback;
}

ECode DowntimeConditionProvider::OnConnected()
{
    if (DEBUG) Slogger::D(TAG, "onConnected");
    mConnected = TRUE;
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(ENTER_ACTION);
    filter->AddAction(EXIT_ACTION);
    filter->AddAction(IIntent::ACTION_TIME_CHANGED);
    filter->AddAction(IIntent::ACTION_TIMEZONE_CHANGED);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(mReceiver, filter, (IIntent**)&intent);
    Init();
    return NOERROR;
}

ECode DowntimeConditionProvider::OnDestroy()
{
    if (DEBUG) Slogger::D(TAG, "onDestroy");
    mConnected = FALSE;
    return NOERROR;
}

ECode DowntimeConditionProvider::OnRequestConditions(
    /* [in] */ Int32 relevance)
{
    if (DEBUG) Slogger::D(TAG, "onRequestConditions relevance=%d", relevance);
    if ((relevance & ICondition::FLAG_RELEVANT_NOW) != 0) {
        if (mInDowntime && mConfig != NULL) {
            AutoPtr<IZenModeConfigDowntimeInfo> info;
            mConfig->ToDowntimeInfo((IZenModeConfigDowntimeInfo**)&info);
            NotifyCondition(CreateCondition(info, ICondition::STATE_TRUE));
        }
    }
    return NOERROR;
}

ECode DowntimeConditionProvider::OnSubscribe(
    /* [in] */ IUri* conditionId)
{
    if (DEBUG) Slogger::D(TAG, "onSubscribe conditionId=%p", conditionId);
    AutoPtr<IZenModeConfigHelper> helper;
    CZenModeConfigHelper::AcquireSingleton((IZenModeConfigHelper**)&helper);
    AutoPtr<IZenModeConfigDowntimeInfo> downtime;
    helper->TryParseDowntimeConditionId(conditionId, (IZenModeConfigDowntimeInfo**)&downtime);
    if (downtime != NULL && mConfig != NULL) {
        AutoPtr<IZenModeConfigDowntimeInfo> info;
        mConfig->ToDowntimeInfo((IZenModeConfigDowntimeInfo**)&info);
        Boolean res;
        IObject::Probe(info)->Equals(downtime, &res);
        const Int32 state = res && mInDowntime
                ? ICondition::STATE_TRUE : ICondition::STATE_FALSE;
        AutoPtr<IConditionHelper> conHelper;
        CConditionHelper::AcquireSingleton((IConditionHelper**)&conHelper);
        String str;
        conHelper->StateToString(state, &str);
        if (DEBUG) Slogger::D(TAG, "notify condition state: %s", str.string());
        NotifyCondition(CreateCondition(downtime, state));
    }
    return NOERROR;
}

ECode DowntimeConditionProvider::OnUnsubscribe(
    /* [in] */ IUri* conditionId)
{
    if (DEBUG) Slogger::D(TAG, "onUnsubscribe conditionId=%p", conditionId);
    return NOERROR;
}

void DowntimeConditionProvider::SetConfig(
    /* [in] */ IZenModeConfig* config)
{
    if (Objects::Equals(mConfig.Get(), config)) return;
    if (DEBUG) Slogger::D(TAG, "setConfig");
    mConfig = config;
    if (mConnected) {
        Init();
    }
}

Boolean DowntimeConditionProvider::IsInDowntime()
{
    return mInDowntime;
}

AutoPtr<ICondition> DowntimeConditionProvider::CreateCondition(
    /* [in] */ IZenModeConfigDowntimeInfo* downtime,
    /* [in] */ Int32 state)
{
    if (downtime == NULL) return NULL;
    AutoPtr<IZenModeConfigHelper> helper;
    CZenModeConfigHelper::AcquireSingleton((IZenModeConfigHelper**)&helper);
    AutoPtr<IUri> id;
    helper->ToDowntimeConditionId(downtime, (IUri**)&id);

    AutoPtr<IDateFormat> format;
    CDateFormat::AcquireSingleton((IDateFormat**)&format);
    Boolean res;
    format->Is24HourFormat(mContext, &res);
    String skeleton;
    if (res) {
        skeleton = "Hm";
    }
    else {
        skeleton = "hma";
    }

    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
    AutoPtr<ILocale> locale;
    localeHelper->GetDefault((ILocale**)&locale);

    String pattern;
    format->GetBestDateTimePattern(locale, skeleton, &pattern);

    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 now;
    sys->GetCurrentTimeMillis(&now);

    Int32 endHour, endMinute;
    downtime->GetEndHour(&endHour);
    downtime->GetEndMinute(&endMinute);
    const Int64 time = GetTime(now, endHour, endMinute);

    AutoPtr<IDate> date;
    CDate::New(time, (IDate**)&date);
    AutoPtr<ISimpleDateFormat> simple;
    CSimpleDateFormat::New(pattern, locale, (ISimpleDateFormat**)&simple);

    using Elastos::Text::IDateFormat;
    String formatted;
    IDateFormat::Probe(simple)->Format(date, &formatted);

    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, TO_IINTERFACE(CoreUtils::Convert(formatted)));

    String summary;
    mContext->GetString(R::string::downtime_condition_summary, args, &summary);
    AutoPtr<ICondition> con;
    CCondition::New(id, summary, String(""), String(""),
            0, state, ICondition::FLAG_RELEVANT_NOW, (ICondition**)&con);
    return con;
}

Boolean DowntimeConditionProvider::IsDowntimeCondition(
    /* [in] */ ICondition* condition)
{
    AutoPtr<IZenModeConfigHelper> helper;
    CZenModeConfigHelper::AcquireSingleton((IZenModeConfigHelper**)&helper);
    AutoPtr<IUri> id;
    condition->GetId((IUri**)&id);
    Boolean res;
    helper->IsValidDowntimeConditionId(id, &res);
    return condition != NULL && res;
}

void DowntimeConditionProvider::Init()
{
    UpdateDays();
    ReevaluateDowntime();
    UpdateAlarms();
}

void DowntimeConditionProvider::UpdateDays()
{
    ISet::Probe(mDays)->Clear();
    if (mConfig != NULL) {
        AutoPtr<IZenModeConfigHelper> helper;
        CZenModeConfigHelper::AcquireSingleton((IZenModeConfigHelper**)&helper);
        String sleepMode;
        mConfig->GetSleepMode(&sleepMode);
        AutoPtr< ArrayOf<Int32> > days;
        helper->TryParseDays(sleepMode, (ArrayOf<Int32>**)&days);
        for (Int32 i = 0; days != NULL && i < days->GetLength(); i++) {
            ISet::Probe(mDays)->Add(CoreUtils::Convert((*days)[i]));
        }
    }
}

Boolean DowntimeConditionProvider::IsInDowntime(
    /* [in] */ Int64 time)
{
    Int32 size;
    if (mConfig == NULL || (ISet::Probe(mDays)->GetSize(&size), size) == 0) return FALSE;

    Int32 sleepStartHour;
    Int32 sleepStartMinute;
    Int32 sleepEndHour;
    Int32 sleepEndMinute;
    mConfig->GetSleepStartHour(&sleepStartHour);
    mConfig->GetSleepStartMinute(&sleepStartMinute);
    mConfig->GetSleepEndHour(&sleepEndHour);
    mConfig->GetSleepEndMinute(&sleepEndMinute);

    Int64 start = GetTime(time, sleepStartHour, sleepStartMinute);
    Int64 end = GetTime(time, sleepEndHour, sleepEndMinute);
    if (start == end) return FALSE;
    if (end < start) {
        end = AddDays(end, 1);
    }
    return IsInDowntime(-1, time, start, end) || IsInDowntime(0, time, start, end);
}

Boolean DowntimeConditionProvider::IsInDowntime(
    /* [in] */ Int32 daysOffset,
    /* [in] */ Int64 time,
    /* [in] */ Int64 start,
    /* [in] */ Int64 end)
{
    const Int32 n = ICalendar::SATURDAY;
    const Int32 day = ((GetDayOfWeek(time) - 1) + (daysOffset % n) + n) % n + 1;
    start = AddDays(start, daysOffset);
    end = AddDays(end, daysOffset);
    Boolean res;
    ISet::Probe(mDays)->Contains(CoreUtils::Convert(day), &res);
    return res && time >= start && time < end;
}

void DowntimeConditionProvider::ReevaluateDowntime()
{
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 now;
    sys->GetCurrentTimeMillis(&now);

    const Boolean inDowntime = IsInDowntime(now);
    if (DEBUG) Slogger::D(TAG, "inDowntime=%d", inDowntime);
    if (inDowntime == mInDowntime) return;
    Slogger::I(TAG, (inDowntime ? String("Entering") : String("Exiting") ) + String(" downtime"));
    mInDowntime = inDowntime;
    ZenLog::TraceDowntime(mInDowntime, GetDayOfWeek(now), mDays);
    FireDowntimeChanged();
}

void DowntimeConditionProvider::FireDowntimeChanged()
{
    if (mCallback != NULL) {
        mCallback->OnDowntimeChanged(mInDowntime);
    }
}

void DowntimeConditionProvider::UpdateAlarms()
{
    if (mConfig == NULL) return;

    Int32 sleepStartHour;
    Int32 sleepStartMinute;
    Int32 sleepEndHour;
    Int32 sleepEndMinute;
    mConfig->GetSleepStartHour(&sleepStartHour);
    mConfig->GetSleepStartMinute(&sleepStartMinute);
    mConfig->GetSleepEndHour(&sleepEndHour);
    mConfig->GetSleepEndMinute(&sleepEndMinute);

    UpdateAlarm(ENTER_ACTION, ENTER_CODE, sleepStartHour, sleepStartMinute);
    UpdateAlarm(EXIT_ACTION, EXIT_CODE, sleepEndHour, sleepEndMinute);
}

Int32 DowntimeConditionProvider::GetDayOfWeek(
    /* [in] */ Int64 time)
{
    mCalendar->SetTimeInMillis(time);
    Int32 result;
    mCalendar->Get(ICalendar::DAY_OF_WEEK, &result);
    return result;
}

Int64 DowntimeConditionProvider::GetTime(
    /* [in] */ Int64 millis,
    /* [in] */ Int32 hour,
    /* [in] */ Int32 min)
{
    mCalendar->SetTimeInMillis(millis);
    mCalendar->Set(ICalendar::HOUR_OF_DAY, hour);
    mCalendar->Set(ICalendar::MINUTE, min);
    mCalendar->Set(ICalendar::SECOND, 0);
    mCalendar->Set(ICalendar::MILLISECOND, 0);
    Int64 result;
    mCalendar->GetTimeInMillis(&result);
    return result;
}

Int64 DowntimeConditionProvider::AddDays(
    /* [in] */ Int64 time,
    /* [in] */ Int32 days)
{
    mCalendar->SetTimeInMillis(time);
    mCalendar->Add(ICalendar::DATE, days);
    Int64 result;
    mCalendar->GetTimeInMillis(&result);
    return result;
}

void DowntimeConditionProvider::UpdateAlarm(
    /* [in] */ const String& action,
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 hr,
    /* [in] */ Int32 min)
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
    AutoPtr<IAlarmManager> alarms = IAlarmManager::Probe(obj);

    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 now;
    sys->GetCurrentTimeMillis(&now);

    mCalendar->SetTimeInMillis(now);
    mCalendar->Set(ICalendar::HOUR_OF_DAY, hr);
    mCalendar->Set(ICalendar::MINUTE, min);
    mCalendar->Set(ICalendar::SECOND, 0);
    mCalendar->Set(ICalendar::MILLISECOND, 0);
    Int64 time;
    mCalendar->GetTimeInMillis(&time);
    if (time <= now) {
        time = AddDays(time, 1);
    }

    AutoPtr<IIntent> intent;
    CIntent::New(action, (IIntent**)&intent);
    intent->PutExtra(EXTRA_TIME, time);

    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    AutoPtr<IPendingIntent> pendingIntent;
    helper->GetBroadcast(mContext, requestCode,
            intent, IPendingIntent::FLAG_UPDATE_CURRENT, (IPendingIntent**)&pendingIntent);
    alarms->Cancel(pendingIntent);

    String sleepMode;
    mConfig->GetSleepMode(&sleepMode);
    if (!sleepMode.IsNull()) {
        if (DEBUG) {
            String str("");
            str.AppendFormat("Scheduling %s for %s, %s in the future, now=%s",
                    action.string(),
                    Ts(time).string(),
                    StringUtils::ToString(time - now).string(),
                    Ts(now).string());
            Slogger::D(TAG, str);
        }
        alarms->SetExact(IAlarmManager::RTC_WAKEUP, time, pendingIntent);
    }
}

String DowntimeConditionProvider::Ts(
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

} // Notification
} // Server
} // Droid
} // Elastos
