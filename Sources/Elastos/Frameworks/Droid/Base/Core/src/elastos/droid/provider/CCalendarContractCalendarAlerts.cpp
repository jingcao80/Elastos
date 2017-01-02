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

#include "Elastos.Droid.Database.h"
#include "elastos/droid/app/CPendingIntentHelper.h"
#include "elastos/droid/content/CContentUris.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CCalendarContractCalendarAlerts.h"
#include "elastos/droid/provider/CalendarContract.h"
#include "elastos/droid/text/format/CTime.h"
#include "elastos/droid/text/format/DateUtils.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Text::Format::CTime;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Utility::Logging::Slogger;
using Elastos::IO::ICloseable;
using Elastos::Core::CInteger32;
using Elastos::Core::CInteger64;
using Elastos::Core::CString;
using Elastos::Core::CSystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::ISystem;

namespace Elastos {
namespace Droid {
namespace Provider {

const String CCalendarContractCalendarAlerts::WHERE_ALARM_EXISTS = ICalendarContractCalendarAlertsColumns::EVENT_ID + String("=?")
                + String(" AND ") + ICalendarContractCalendarAlertsColumns::BEGIN + String("=?")
                + String(" AND ") + ICalendarContractCalendarAlertsColumns::ALARM_TIME + String("=?");
const String CCalendarContractCalendarAlerts::WHERE_FINDNEXTALARMTIME = ICalendarContractCalendarAlertsColumns::ALARM_TIME + String(">=?");
const String CCalendarContractCalendarAlerts::SORT_ORDER_ALARMTIME_ASC = ICalendarContractCalendarAlertsColumns::ALARM_TIME + String(" ASC");
const String CCalendarContractCalendarAlerts::WHERE_RESCHEDULE_MISSED_ALARMS = ICalendarContractCalendarAlertsColumns::STATE + String("=")
                + StringUtils::ToString(ICalendarContractCalendarAlertsColumns::STATE_SCHEDULED)
                + String(" AND ") + ICalendarContractCalendarAlertsColumns::ALARM_TIME + String("<?")
                + String(" AND ") + ICalendarContractCalendarAlertsColumns::ALARM_TIME + String(">?")
                + String(" AND ") + END + String(">=?");
const Boolean CCalendarContractCalendarAlerts::DEBUG = FALSE;

CAR_SINGLETON_IMPL(CCalendarContractCalendarAlerts)

CAR_INTERFACE_IMPL_5(CCalendarContractCalendarAlerts, Singleton
    , ICalendarContractCalendarAlerts
    , IBaseColumns
    , ICalendarContractCalendarAlertsColumns
    , ICalendarContractEventsColumns
    , ICalendarContractCalendarColumns)

ECode CCalendarContractCalendarAlerts::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    StringBuilder builder;
    builder += "content://";
    builder += ICalendarContract::AUTHORITY;
    builder += "/calendar_alerts";
    String str = builder.ToString();
    return Uri::Parse(str, uri);
}

ECode CCalendarContractCalendarAlerts::GetCONTENT_URI_BY_INSTANCE(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    StringBuilder builder;
    builder += "content://";
    builder += ICalendarContract::AUTHORITY;
    builder += "/calendar_alerts/by_instance";
    String str = builder.ToString();
    return Uri::Parse(str, uri);
}

ECode CCalendarContractCalendarAlerts::Insert(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Int64 eventId,
    /* [in] */ Int64 begin,
    /* [in] */ Int64 end,
    /* [in] */ Int64 alarmTime,
    /* [in] */ Int32 minutes,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(ICalendarContractCalendarAlertsColumns::EVENT_ID, eventId);
    values->Put(ICalendarContractCalendarAlertsColumns::BEGIN, begin);
    values->Put(ICalendarContractCalendarAlertsColumns::END, end);
    values->Put(ICalendarContractCalendarAlertsColumns::ALARM_TIME, alarmTime);

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    values->Put(ICalendarContractCalendarAlertsColumns::CREATION_TIME, now);
    values->Put(ICalendarContractCalendarAlertsColumns::RECEIVED_TIME, 0);
    values->Put(ICalendarContractCalendarAlertsColumns::NOTIFY_TIME, 0);
    values->Put(ICalendarContractCalendarAlertsColumns::STATE, ICalendarContractCalendarAlertsColumns::STATE_SCHEDULED);
    values->Put(ICalendarContractCalendarAlertsColumns::MINUTES, minutes);

    AutoPtr<IUri> _uri;
    FAIL_RETURN(GetCONTENT_URI((IUri**)&_uri))
    return cr->Insert(_uri, values, uri);
}

ECode CCalendarContractCalendarAlerts::FindNextAlarmTime(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Int64 millis,
    /* [out] */ Int64* alarm)
{
    VALIDATE_NOT_NULL(alarm);

    StringBuilder builder;
    builder += ALARM_TIME;
    builder += ">=";
    builder += millis;
    String selection = builder.ToString();
    // TODO: construct an explicit SQL query so that we can add
    // "LIMIT 1" to the end and get just one result.
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(1);
    (*projection)[0] = ALARM_TIME;
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
    (*args)[0] = StringUtils::ToString(millis);
    AutoPtr<IUri> uri;
    FAIL_RETURN(GetCONTENT_URI((IUri**)&uri))
    AutoPtr<ICursor> cursor;
    FAIL_RETURN(cr->Query(uri, projection, WHERE_FINDNEXTALARMTIME, args, SORT_ORDER_ALARMTIME_ASC, (ICursor**)&cursor))
    Int64 alarmTime = -1;
    //try {
    Boolean result;
    if (cursor != NULL && (cursor->MoveToFirst(&result), result)) {
        FAIL_GOTO(cursor->GetInt64(0, &alarmTime), EXIT)
    }
    //} finally {
EXIT:
    if (cursor != NULL) {
        FAIL_RETURN(ICloseable::Probe(cursor)->Close())
    }
    //}
    *alarm = alarmTime;
    return NOERROR;
}

ECode CCalendarContractCalendarAlerts::RescheduleMissedAlarms(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IContext* context,
    /* [in] */ IAlarmManager* manager)
{
    // Get all the alerts that have been scheduled but have not fired
    // and should have fired by now and are not too old.
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    Int64 ancient = now - IDateUtils::DAY_IN_MILLIS;
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(1);
    (*projection)[0] = ALARM_TIME;

    // TODO: construct an explicit SQL query so that we can add
    // "GROUPBY" instead of doing a sort and de-dup
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(3);
    (*args)[0] = StringUtils::ToString(now);
    (*args)[1] = StringUtils::ToString(ancient);
    (*args)[2] = StringUtils::ToString(now);
    AutoPtr<IUri> uri;
    FAIL_RETURN(GetCONTENT_URI((IUri**)&uri))
    AutoPtr<ICursor> cursor;
    FAIL_RETURN(cr->Query(uri, projection, WHERE_RESCHEDULE_MISSED_ALARMS, args,
        SORT_ORDER_ALARMTIME_ASC, (ICursor**)&cursor))
    if (cursor == NULL) {
        return NOERROR;
    }

    if (DEBUG) {
        Int32 count;
        FAIL_RETURN(cursor->GetCount(&count))
        Slogger::D(String("Calendar")/*TAG*/, String("missed alarms found: ") + StringUtils::ToString(count));
    }

    //try {
    Int64 alarmTime = -1;

    Boolean result;
    while ((cursor->MoveToNext(&result), result)) {
        Int64 newAlarmTime;
        FAIL_GOTO(cursor->GetInt64(0, &newAlarmTime), EXIT)
        if (alarmTime != newAlarmTime) {
            if (DEBUG) {
                Slogger::W(String("Calendar")/*TAG*/, String("rescheduling missed alarm. alarmTime: ") + StringUtils::ToString(newAlarmTime));
            }
            FAIL_GOTO(ScheduleAlarm(context, manager, newAlarmTime), EXIT)
            alarmTime = newAlarmTime;
        }
    }
    //} finally {
EXIT:
    {
        return ICloseable::Probe(cursor)->Close();
    }
}

ECode CCalendarContractCalendarAlerts::ScheduleAlarm(
    /* [in] */ IContext* context,
    /* [in] */ IAlarmManager* mgr,
    /* [in] */ Int64 alarmTime)
{
    if (DEBUG) {
        AutoPtr<ITime> time;
        FAIL_RETURN(CTime::New((ITime**)&time))
        FAIL_RETURN(time->Set(alarmTime))
        String schedTime;
        FAIL_RETURN(time->Format(String(" %a, %b %d, %Y %I:%M%P"), &schedTime))
        Slogger::D(String("Calendar")/*TAG*/, String("Schedule alarm at ") + StringUtils::ToString(alarmTime) + String(" ") + schedTime);
    }

    AutoPtr<IAlarmManager> manager = mgr;
    if (manager == NULL) {
        AutoPtr<IInterface> obj;
        FAIL_RETURN(context->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj));
        manager = IAlarmManager::Probe(obj);
    }

    AutoPtr<IIntent> intent;
    FAIL_RETURN(CIntent::New(ICalendarContract::ACTION_EVENT_REMINDER, (IIntent**)&intent))
    AutoPtr<IUri> _uri;
    FAIL_RETURN(CalendarContract::GetCONTENT_URI((IUri**)&_uri))
    AutoPtr<IContentUris> urihelper;
    FAIL_RETURN(CContentUris::AcquireSingleton((IContentUris**)&urihelper))
    AutoPtr<IUri> newUri;
    FAIL_RETURN(urihelper->WithAppendedId(_uri, alarmTime, (IUri**)&newUri))
    FAIL_RETURN(intent->SetData(newUri))
    FAIL_RETURN(intent->PutExtra(ALARM_TIME, alarmTime))
    AutoPtr<IPendingIntentHelper> intenthelper;
    FAIL_RETURN(CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&intenthelper))
    AutoPtr<IPendingIntent> pi;
    FAIL_RETURN(intenthelper->GetBroadcast(context, 0, intent, 0, (IPendingIntent**)&pi))
    FAIL_RETURN(manager->Set(IAlarmManager::RTC_WAKEUP, alarmTime, pi))
    return NOERROR;
}

ECode CCalendarContractCalendarAlerts::IsAlarmExists(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Int64 eventId,
    /* [in] */ Int64 begin,
    /* [in] */ Int64 alarmTime,
    /* [out] */ Boolean* isExit)
{
    VALIDATE_NOT_NULL(isExit);

    // TODO: construct an explicit SQL query so that we can add
    // "LIMIT 1" to the end and get just one result.
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(1);
    (*projection)[0] = ALARM_TIME;
    AutoPtr<IUri> uri;
    FAIL_RETURN(GetCONTENT_URI((IUri**)&uri))
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(3);
    (*args)[0] = StringUtils::ToString(eventId);
    (*args)[1] = StringUtils::ToString(begin);
    (*args)[2] = StringUtils::ToString(alarmTime);
    AutoPtr<ICursor> cursor;
    FAIL_RETURN(cr->Query(uri, projection, WHERE_ALARM_EXISTS, args, String(NULL), (ICursor**)&cursor))
    Boolean found = FALSE;
    //try {
    Int32 count;
    if (cursor != NULL && (cursor->GetCount(&count), count) > 0) {
        found = TRUE;
    }
    //} finally {
    if (cursor != NULL) {
        FAIL_RETURN(ICloseable::Probe(cursor)->Close())
    }
    //}
    *isExit = found;
    return NOERROR;
}

} //Provider
} //Droid
} //Elastos
