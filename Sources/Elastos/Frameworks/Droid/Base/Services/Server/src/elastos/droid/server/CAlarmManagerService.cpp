
#include "CAlarmManagerService.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/UserHandle.h"
//#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <linux/ioctl.h>
#include <linux/android_alarm.h>
#include <fcntl.h>

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::IO::IBufferedReader;
using Elastos::IO::CBufferedReader;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Text::Format::CTime;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Content::EIID_IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilterComparison;
using Elastos::Droid::Content::CIntentFilterComparison;
using Elastos::Droid::Content::EIID_IBroadcastReceiver;
using Elastos::Droid::Content::IIIntentSender;
using Elastos::Droid::Content::IIntentSender;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;

namespace Elastos {
namespace Droid {
namespace Server {

//=====================================================================================
//              CAlarmManagerService::AlarmHandler
//=====================================================================================

const Int32 CAlarmManagerService::AlarmHandler::ALARM_EVENT;
const Int32 CAlarmManagerService::AlarmHandler::MINUTE_CHANGE_EVENT;
const Int32 CAlarmManagerService::AlarmHandler::DATE_CHANGE_EVENT;

ECode CAlarmManagerService::AlarmHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    if (what != ALARM_EVENT) {
        return NOERROR;
    }

    AlarmList triggerList;

    {
        AutoLock lock(mHost->mLock);
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

        Int64 nowRTC;
        system->GetCurrentTimeMillis(&nowRTC);
        mHost->TriggerAlarmsLocked(mHost->mRtcWakeupShutdownAlarms, &triggerList, nowRTC);
        mHost->TriggerAlarmsLocked(mHost->mRtcWakeupAlarms, &triggerList, nowRTC);
        mHost->TriggerAlarmsLocked(mHost->mRtcAlarms, &triggerList, nowRTC);
        mHost->TriggerAlarmsLocked(mHost->mElapsedRealtimeWakeupAlarms, &triggerList, nowRTC);
        mHost->TriggerAlarmsLocked(mHost->mElapsedRealtimeAlarms, &triggerList, nowRTC);
    }

    // now trigger the alarms without the lock held
    AlarmListIterator it = triggerList.Begin();

    while (it != triggerList.End()) {
        AutoPtr<Alarm> alarm = *it;
        ECode ec = alarm->mOperation->Send();

        if (FAILED(ec)) {
            if (alarm->mRepeatInterval > 0) {
                // This IntentSender is no longer valid, but this
                // is a repeating alarm, so toss the hoser.
                mHost->Remove(alarm->mOperation);
            }
        }
        it++;
    }

    return NOERROR;
}

//=====================================================================================
//              CAlarmManagerService::InFlight
//=====================================================================================
CAlarmManagerService::InFlight::InFlight(
    /* [in] */ CAlarmManagerService* service,
    /* [in] */ IPendingIntent* pendingIntent)
    : mPendingIntent(pendingIntent)
{
    AutoPtr<IIntent> intent;
    pendingIntent->GetIntent((IIntent**)&intent);

    if (intent != NULL) {
        String action;
        intent->GetAction(&action);
        AutoPtr<IComponentName> cn;
        intent->GetComponent((IComponentName**)&cn);
        mTarget.mFirst = action;
        mTarget.mSecond = cn;
    }

    mBroadcastStats = service->GetStatsLocked(pendingIntent);

    AutoPtr<FilterStats> fs;
    HashMap<Pair<String, AutoPtr<IComponentName> >, AutoPtr<FilterStats> >::Iterator it;
    it = mBroadcastStats->mFilterStats.Find(mTarget);
    if (it != mBroadcastStats->mFilterStats.End()) {
        fs = it->mSecond;
    }

    if (fs == NULL) {
        fs = new FilterStats(mBroadcastStats, mTarget);
        mBroadcastStats->mFilterStats[mTarget] = fs;
    }
    mFilterStats = fs;
}

//=====================================================================================
//              CAlarmManagerService::FilterStats
//=====================================================================================

CAlarmManagerService::FilterStats::FilterStats(
    /* [in] */ BroadcastStats* broadcastStats,
    /* [in] */ Pair<String, AutoPtr<IComponentName> > target)
    : mBroadcastStats(broadcastStats)
    , mTarget(target)
    , mAggregateTime(0)
    , mCount(0)
    , mNumWakeup(0)
    , mStartTime(0)
    , mNesting(0)
{
}

//=====================================================================================
//              CAlarmManagerService::BroadcastStats
//=====================================================================================
CAlarmManagerService::BroadcastStats::BroadcastStats(
    /* [in] */ const String& packageName)
    : mAggregateTime(0)
    , mCount(0)
    , mNumWakeup(0)
    , mStartTime(0)
    , mNesting(0)
{
}

CAlarmManagerService::BroadcastStats::~BroadcastStats()
{
    mFilterStats.Clear();
}

// ECode CAlarmManagerService::IncreasingTimeOrder::Compare(
//     /* [in] */ Alarm* a1,
//     /* [in] */ Alarm* a2,
//     /* [out] */ Int32* result)
// {
//     VALIDATE_NOT_NULL(result);
//     Int64 when1 = a1->mWhen;
//     Int64 when2 = a2->mWhen;
//     if (when1 - when2 > 0) {
//         *result = 1;
//         return NOERROR;
//     }
//     if (when1 - when2 < 0) {
//         *result = -1;
//         return NOERROR;
//     }
//     *result = 0;
//     return NOERROR;
// }


//=====================================================================================
//              CAlarmManagerService::Alarm
//=====================================================================================
CAlarmManagerService::Alarm::Alarm()
    : mType(0)
    , mCount(0)
    , mWhen(0)
    , mRepeatInterval(0)
    , mOperation(NULL)
{
}

//=====================================================================================
//              CAlarmManagerService::AlarmThread
//=====================================================================================
CAlarmManagerService::AlarmThread::AlarmThread(
    /* [in] */ CAlarmManagerService* host)
    : mHost(host)
{
    Thread::constructor(String("AlarmManager"));
}

ECode CAlarmManagerService::AlarmThread::Run()
{
    while (TRUE) {
        Int32 result = mHost->NativeWaitForAlarm(mHost->mDescriptor);
        AlarmList triggerList;

        if ((result & TIME_CHANGED_MASK) != 0) {
            mHost->Remove(mHost->mTimeTickSender);
            mHost->mClockReceiver->ScheduleTimeTickEvent();

            AutoPtr<IIntent> intent;
            CIntent::New(IIntent::ACTION_TIME_CHANGED, (IIntent**)&intent);
            intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING | IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
            AutoPtr<IUserHandleHelper> handleHelper;
            ASSERT_SUCCEEDED(CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handleHelper));
            AutoPtr<IUserHandle> userAll;
            handleHelper->GetALL((IUserHandle**)&userAll);
            mHost->mContext->SendBroadcastAsUser(intent, userAll);
        }

        {
            AutoLock lock(mHost->mLock);
            AutoPtr<ISystem> system;
            Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
            Int64 nowRTC;
            system->GetCurrentTimeMillis(&nowRTC);
            Int64 nowELAPSED = SystemClock::GetElapsedRealtime();

            if (localLOGV) {
                StringBuilder buf;
                buf += "Checking for alarms... rtc=";
                buf += nowRTC;
                buf += ", elapsed=";
                buf += nowELAPSED;
                Slogger::V(TAG, buf.ToString());
            }

            if ((result & RTC_SHUTDOWN_WAKEUP_MASK) != 0)
                mHost->TriggerAlarmsLocked(mHost->mRtcWakeupShutdownAlarms, &triggerList, nowRTC);


            if ((result & RTC_WAKEUP_MASK) != 0) {
                mHost->TriggerAlarmsLocked(mHost->mRtcWakeupAlarms, &triggerList, nowRTC);
            }

            if ((result & RTC_MASK) != 0) {
                mHost->TriggerAlarmsLocked(mHost->mRtcAlarms, &triggerList, nowRTC);
            }

            if ((result & ELAPSED_REALTIME_WAKEUP_MASK) != 0) {
                mHost->TriggerAlarmsLocked(mHost->mElapsedRealtimeWakeupAlarms, &triggerList, nowELAPSED);
            }

            if ((result & ELAPSED_REALTIME_MASK) != 0) {
                mHost->TriggerAlarmsLocked(mHost->mElapsedRealtimeAlarms, &triggerList, nowELAPSED);
            }

            // now trigger the alarms
            AlarmListIterator it = triggerList.Begin();
            for (; it != triggerList.End(); ++it) {
                AutoPtr<Alarm> alarm = *it;
                //if (localLOGV) Slog.v(TAG, "sending alarm " + alarm);
                mHost->mBackgroundIntent->PutExtra(IIntent::EXTRA_ALARM_COUNT, alarm->mCount);

                ECode ec = alarm->mOperation->Send(mHost->mContext, 0, mHost->mBackgroundIntent, mHost->mResultReceiver, mHost->mHandler);

                if (FAILED(ec)) {
                    if (alarm->mRepeatInterval > 0) {
                        // This IntentSender is no longer valid, but this
                        // is a repeating alarm, so toss the hoser.
                        mHost->Remove(alarm->mOperation);
                    }

                    Slogger::W(TAG, "Failure sending alarm.");
                    continue;
                }

                // we have an active broadcast so stay awake.
                if (mHost->mBroadcastRefCount == 0 && mHost->mWakeLock) {
                    mHost->SetWakelockWorkSource(alarm->mOperation);
                    ec = mHost->mWakeLock->AcquireLock();

                    if (FAILED(ec)) {
                        if (alarm->mRepeatInterval > 0) {
                            // This IntentSender is no longer valid, but this
                            // is a repeating alarm, so toss the hoser.
                            mHost->Remove(alarm->mOperation);
                        }

                        Slogger::W(TAG, "Failure sending alarm.");
                        continue;
                    }
                }

                AutoPtr<InFlight> inflight = new InFlight(mHost, alarm->mOperation.Get());
                mHost->mInFlight.PushBack(inflight);
                mHost->mBroadcastRefCount++;

                AutoPtr<BroadcastStats> bs = inflight->mBroadcastStats;
                if (bs->mNesting == 0) {
                    bs->mNesting = 1;
                    bs->mStartTime = nowELAPSED;
                } else {
                    bs->mNesting++;
                }

                AutoPtr<FilterStats> fs = inflight->mFilterStats;
                fs->mCount++;
                if (fs->mNesting == 0) {
                    fs->mNesting = 1;
                    fs->mStartTime = nowELAPSED;
                } else {
                    fs->mNesting++;
                }
                if (alarm->mType == IAlarmManager::ELAPSED_REALTIME_WAKEUP
                        || alarm->mType == IAlarmManager::RTC_WAKEUP
                        || alarm->mType == IAlarmManager::RTC_SHUTDOWN_WAKEUP) {
                    bs->mNumWakeup++;
                    fs->mNumWakeup++;
                    ActivityManagerNative::NoteWakeupAlarm(alarm->mOperation);
                    if (FAILED(ec)) {
                        if (alarm->mRepeatInterval > 0) {
                            // This IntentSender is no longer valid, but this
                            // is a repeating alarm, so toss the hoser.
                            mHost->Remove(alarm->mOperation);
                        }

                        Slogger::W(TAG, "Failure sending alarm.");
                        continue;
                    }
                }
            }
        }
    }

    return NOERROR;
}

//=====================================================================================
//              CAlarmManagerService::ClockReceiver
//=====================================================================================
CAlarmManagerService::ClockReceiver::ClockReceiver(
    /* [in] */ CAlarmManagerService* host)
    : mHost(host)
{
}

ECode CAlarmManagerService::ClockReceiver::Register()
{
    AutoPtr<IIntentFilter> filter;
    ASSERT_SUCCEEDED(CIntentFilter::New((IIntentFilter**)&filter));
    filter->AddAction(IIntent::ACTION_TIME_TICK);
    filter->AddAction(IIntent::ACTION_DATE_CHANGED);

    AutoPtr<IIntent> stickyIntent;
    assert(mHost->mContext != NULL);
    return mHost->mContext->RegisterReceiver(
        (IBroadcastReceiver*)this, filter, (IIntent**)&stickyIntent);
}

ECode CAlarmManagerService::ClockReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    if (action.Equals(IIntent::ACTION_TIME_TICK)) {
        //PFL_EX("[[ACTION_TIME_TICK]] Broadcast which called by alarm has received ,time now is [%lld]",
        // System::GetCurrentTimeMillis());
        ScheduleTimeTickEvent();
    }
    else if (action.Equals(IIntent::ACTION_DATE_CHANGED)) {
        //PFL_EX("[[ACTION_DATE_CHANGED]] Broadcast which called by alarm has received ,time now is [%lld]",
        // System::GetCurrentTimeMillis());
        // Since the kernel does not keep track of DST, we need to
        // reset the TZ information at the beginning of each day
        // based off of the current Zone gmt offset + userspace tracked
        // daylight savings information.
        AutoPtr<ITimeZoneHelper> timezoneHelper;
        CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&timezoneHelper);

        AutoPtr<ITimeZone> zone;
        AutoPtr<ISystemProperties> sysProp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
        String value;
        sysProp->Get(TIMEZONE_PROPERTY, &value);
        timezoneHelper->GetTimeZone(value, (ITimeZone**)&zone);

        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int64 now;
        system->GetCurrentTimeMillis(&now);
        Int32 gmtOffset;
        zone->GetOffset(now, &gmtOffset);
        mHost->NativeSetKernelTimezone(mHost->mDescriptor, -(gmtOffset / 60000));
        ScheduleDateChangedEvent();
    }

    return NOERROR;
}

void CAlarmManagerService::ClockReceiver::ScheduleTimeTickEvent()
{
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);

    AutoPtr<ICalendar> calendar;
    helper->GetInstance((ICalendar**)&calendar);
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

    Int64 currentTime;
    system->GetCurrentTimeMillis(&currentTime);

    calendar->SetTimeInMillis(currentTime);
    calendar->Add(ICalendar::MINUTE, 1);
    calendar->Set(ICalendar::SECOND, 0);
    calendar->Set(ICalendar::MILLISECOND, 0);

    // Schedule this event for the amount of time that it would take to get to
    // the top of the next minute.
    Int64 tmpMillis;
    calendar->GetTimeInMillis(&tmpMillis);
    Int64 tickEventDelay = tmpMillis - currentTime;
    tickEventDelay += SystemClock::GetElapsedRealtime();
    mHost->Set(IAlarmManager::ELAPSED_REALTIME, tickEventDelay,
        mHost->mTimeTickSender);
}

void CAlarmManagerService::ClockReceiver::ScheduleDateChangedEvent()
{
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);

    AutoPtr<ICalendar> calendar;
    helper->GetInstance((ICalendar**)&calendar);
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);

    calendar->SetTimeInMillis(now);
    calendar->Set(ICalendar::HOUR, 0);
    calendar->Set(ICalendar::MINUTE, 0);
    calendar->Set(ICalendar::SECOND, 0);
    calendar->Set(ICalendar::MILLISECOND, 0);
    calendar->Add(ICalendar::DAY_OF_MONTH, 1);

    Int64 millis;
    calendar->GetTimeInMillis(&millis);

    Int64 ctm;
    system->GetCurrentTimeMillis(&ctm);
    mHost->Set(IAlarmManager::RTC, ctm + millis,
        mHost->mDateChangeSender);
}

//=====================================================================================
//              CAlarmManagerService::UninstallReceiver
//=====================================================================================
CAlarmManagerService::UninstallReceiver::UninstallReceiver(
    /* [in] */ CAlarmManagerService* host)
    : mHost(host)
{
}

ECode CAlarmManagerService::UninstallReceiver::Register()
{
    AutoPtr<IIntentFilter> filter;
    ASSERT_SUCCEEDED(CIntentFilter::New((IIntentFilter**)&filter));
    filter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
    filter->AddAction(IIntent::ACTION_PACKAGE_RESTARTED);
    filter->AddAction(IIntent::ACTION_QUERY_PACKAGE_RESTART);
    filter->AddDataScheme(String("package"));

    AutoPtr<IIntent> stickyIntent;
    mHost->mContext->RegisterReceiver(
        (IBroadcastReceiver*)this, filter, (IIntent**)&stickyIntent);

    // Register for events related to sdcard installation.
    AutoPtr<IIntentFilter> sdFilter;
    ASSERT_SUCCEEDED(CIntentFilter::New((IIntentFilter**)&sdFilter));
    sdFilter->AddAction(IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE);
    sdFilter->AddAction(IIntent::ACTION_USER_STOPPED);

    AutoPtr<IIntent> sdStickyIntent;
    return mHost->mContext->RegisterReceiver(
        (IBroadcastReceiver*)this, sdFilter, (IIntent**)&sdStickyIntent);
}

ECode CAlarmManagerService::UninstallReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoLock lock(mHost->mLock);

    String action;
    intent->GetAction(&action);

    AutoPtr< ArrayOf<String> > pkgList;

    if (action.Equals(IIntent::ACTION_QUERY_PACKAGE_RESTART)) {
        intent->GetStringArrayExtra(IIntent::EXTRA_PACKAGES, (ArrayOf<String>**)&pkgList);

        if (pkgList) {
            for (Int32 i = 0; i < pkgList->GetLength(); ++i) {
                String packageName = (*pkgList)[i];

                if (mHost->LookForPackageLocked(packageName)) {
                    SetResultCode(IActivity::RESULT_OK);
                    return NOERROR;
                }
            }
        }
        return NOERROR;
    }
    else if (action.Equals(IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE)) {
        intent->GetStringArrayExtra(IIntent::EXTRA_CHANGED_PACKAGE_LIST, (ArrayOf<String>**)&pkgList);
    }
    else if (action.Equals(IIntent::ACTION_USER_STOPPED)) {
        Int32 userHandle;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, -1, &userHandle);

        if (userHandle >= 0) {
            mHost->RemoveUserLocked(userHandle);
        }
    }
    else {
        Boolean replacing;
        intent->GetBooleanExtra(IIntent::EXTRA_REPLACING, FALSE, &replacing);

        if (action.Equals(IIntent::ACTION_PACKAGE_REMOVED) && replacing) {
            // This package is being updated; don't kill its alarms.
            return NOERROR;
        }

        AutoPtr<IUri> data;
        intent->GetData((IUri**)&data);

        if (data != NULL) {
            String pkg;
            data->GetSchemeSpecificPart(&pkg);

            if (!pkg.IsNull()) {
                pkgList = ArrayOf<String>::Alloc(1);
                (*pkgList)[0] = pkg;
            }
        }
    }

    if (pkgList != NULL && (pkgList->GetLength() > 0)) {
        for (Int32 i = 0, size = pkgList->GetLength(); i < size; ++i) {
            String pkg = (*pkgList)[i];
            mHost->RemoveLocked(pkg);
            mHost->mBroadcastStats.Erase(pkg);
        }
    }

    return NOERROR;
}

//=====================================================================================
//              CAlarmManagerService::ResultReceiver
//=====================================================================================
CAR_INTERFACE_IMPL(CAlarmManagerService::ResultReceiver, IPendingIntentOnFinished);

CAlarmManagerService::ResultReceiver::ResultReceiver(
    /* [in] */ CAlarmManagerService* host)
    : mHost(host)
{
}

ECode CAlarmManagerService::ResultReceiver::OnSendFinished(
    /* [in] */ IPendingIntent* pendingIntent,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& resultData,
    /* [in] */ IBundle* resultExtras)
{
    AutoLock lock(mHost->mLock);

    Boolean bval;
    AutoPtr<InFlight> inflight;
    List<AutoPtr<InFlight> >::Iterator it = mHost->mInFlight.Begin();
    for (; it != mHost->mInFlight.End(); ++it) {
        AutoPtr<InFlight> flight = *it;
        flight->mPendingIntent->Equals(pendingIntent->Probe(EIID_IInterface), &bval);
        if (bval) {
            inflight = flight;
            mHost->mInFlight.Erase(it);
            break;
        }
    }

    if (inflight != NULL) {
        Int64 nowELAPSED = SystemClock::GetElapsedRealtime();
        AutoPtr<BroadcastStats> bs = inflight->mBroadcastStats;
        bs->mNesting--;
        if (bs->mNesting <= 0) {
            bs->mNesting = 0;
            bs->mAggregateTime += nowELAPSED - bs->mStartTime;
        }

        AutoPtr<FilterStats> fs = inflight->mFilterStats;
        fs->mNesting--;
        if (fs->mNesting <= 0) {
            fs->mNesting = 0;
            fs->mAggregateTime += nowELAPSED - fs->mStartTime;
        }
    } else {
        // mLog.w("No in-flight alarm for " + pi + " " + intent);
    }

    mHost->mBroadcastRefCount--;
    if (mHost->mBroadcastRefCount == 0) {
        if (mHost->mWakeLock)
            mHost->mWakeLock->ReleaseLock();

        if (mHost->mInFlight.IsEmpty() == FALSE) {
            // mLog.w("Finished all broadcasts with " + mInFlight.size()
            //         + " remaining inflights");
            // for (int i=0; i<mInFlight.size(); i++) {
            //     mLog.w("  Remaining #" + i + ": " + mInFlight.get(i));
            // }
            mHost->mInFlight.Clear();
        }
    }
    else {
        // the next of our alarms is now in flight.  reattribute the wakelock.
        if (mHost->mInFlight.IsEmpty() == FALSE) {
            mHost->SetWakelockWorkSource(mHost->mInFlight[0]->mPendingIntent);
        }
        else {
            // should never happen
            Slogger::E(TAG, "Alarm wakelock still held but sent queue empty");
            if (mHost->mWakeLock)
                mHost->mWakeLock->SetWorkSource(NULL);
        }
    }

    return NOERROR;
}

//=====================================================================================
//              CAlarmManagerService
//=====================================================================================

// The threshold for how long an alarm can be late before we print a
// warning message.  The time duration is in milliseconds.
const Int64 CAlarmManagerService::LATE_ALARM_THRESHOLD = 10 * 1000;

const Int32 CAlarmManagerService::RTC_WAKEUP_MASK = 1 << IAlarmManager::RTC_WAKEUP;
const Int32 CAlarmManagerService::RTC_MASK = 1 << IAlarmManager::RTC;
const Int32 CAlarmManagerService::ELAPSED_REALTIME_WAKEUP_MASK = 1 << IAlarmManager::ELAPSED_REALTIME_WAKEUP;
const Int32 CAlarmManagerService::ELAPSED_REALTIME_MASK = 1 << IAlarmManager::ELAPSED_REALTIME;
const Int32 CAlarmManagerService::TIME_CHANGED_MASK = 1 << 16;
const Int32 CAlarmManagerService::RTC_SHUTDOWN_WAKEUP_MASK = 1 << IAlarmManager::RTC_SHUTDOWN_WAKEUP;

// Alignment quantum for inexact repeating alarms
const Int64 CAlarmManagerService::QUANTUM = IAlarmManager::INTERVAL_FIFTEEN_MINUTES;

const String CAlarmManagerService::TAG("AlarmManager");
const String CAlarmManagerService::ClockReceiver_TAG = String("ClockReceiver");
const Boolean CAlarmManagerService::localLOGV = FALSE;
const Int32 CAlarmManagerService::ALARM_EVENT = 1;
const String CAlarmManagerService::TIMEZONE_PROPERTY = String("persist.sys.timezone");

static AutoPtr<IIntent> InitAlarmManagerServiceBackgroundIntent()
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);

    intent->AddFlags(IIntent::FLAG_FROM_BACKGROUND);
    return intent;
}

const AutoPtr<IIntent> CAlarmManagerService::mBackgroundIntent = InitAlarmManagerServiceBackgroundIntent();

CAlarmManagerService::CAlarmManagerService()
    : mDescriptor(0)
    , mBroadcastRefCount(0)
{}

ECode CAlarmManagerService::constructor(
    /* [in] */ IContext* context)
{
    mRtcWakeupShutdownAlarms = new AlarmList();
    mRtcWakeupAlarms = new AlarmList();
    mRtcAlarms = new AlarmList();
    mElapsedRealtimeWakeupAlarms = new AlarmList();
    mElapsedRealtimeAlarms = new AlarmList();

    mBroadcastRefCount = 0;
    mContext = context;
    mDescriptor = NativeInit();

    // We have to set current TimeZone info to kernel
    // because kernel doesn't keep this after reboot
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String tz;
    sysProp->Get(TIMEZONE_PROPERTY, &tz);
    if (!tz.IsNullOrEmpty()) {
        SetTimeZone(tz);
    }

    AutoPtr<IPowerManager> pm;
    mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&pm);
    if (pm != NULL) {
        pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, TAG, (IPowerManagerWakeLock**)&mWakeLock);
    }

    AutoPtr<IIntent> timeTickIntent;
    CIntent::New(IIntent::ACTION_TIME_TICK, (IIntent**)&timeTickIntent);
    timeTickIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);

    AutoPtr<IUserHandleHelper> handleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handleHelper);
    AutoPtr<IUserHandle> userAll;
    handleHelper->GetALL((IUserHandle**)&userAll);

    AutoPtr<IPendingIntentHelper> intentHelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&intentHelper);
    intentHelper->GetBroadcastAsUser(context, 0, timeTickIntent, 0, userAll,
        (IPendingIntent**)&mTimeTickSender);

    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_DATE_CHANGED, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    intentHelper->GetBroadcastAsUser(context, 0, intent, IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT,
        userAll, (IPendingIntent**)&mDateChangeSender);

    // now that we have initied the driver schedule the alarm
    mClockReceiver = new ClockReceiver(this);
    mClockReceiver->Register();
    mClockReceiver->ScheduleTimeTickEvent();
    mClockReceiver->ScheduleDateChangedEvent();

    mUninstallReceiver = new UninstallReceiver(this);
    mUninstallReceiver->Register();

    mWaitThread = new AlarmThread(this);
    mHandler = new AlarmHandler(this);
    mResultReceiver = new ResultReceiver(this);

    if (mDescriptor != -1) {
        mWaitThread->Start();
    } else {
        Slogger::W(TAG, "Failed to open alarm driver. Falling back to a handler.");
    }

    return NOERROR;
}

CAlarmManagerService::~CAlarmManagerService()
{
    NativeClose(mDescriptor);
}

ECode CAlarmManagerService::Set(
    /* [in] */ Int32 type,
    /* [in] */ Int64 triggerAtTime,
    /* [in] */ IPendingIntent* operation)
{
    return SetRepeating(type, triggerAtTime, 0, operation);
}

ECode CAlarmManagerService::SetRepeating(
    /* [in] */ Int32 type,
    /* [in] */ Int64 triggerAtTime,
    /* [in] */ Int64 interval,
    /* [in] */ IPendingIntent* operation)
{
    if (operation == NULL) {
        Slogger::W(TAG, "set/setRepeating ignored because there is no intent");
        return NOERROR;
    }

    // TODO: delete
    //
    String pkg;
    operation->GetTargetPackage(&pkg);
    if (pkg.IsNull())
        return NOERROR;

    if (localLOGV) {

        StringBuilder buf;
        buf += "setRepeating type is ";
        buf += type;
        buf += " package ";
        buf += pkg.string();
        Slogger::D(TAG, buf.ToString());
    }

    {
        AutoLock lock(mLock);

        AutoPtr<Alarm> alarm = new Alarm();

        alarm->mType = type;
        alarm->mWhen = triggerAtTime;
        alarm->mRepeatInterval = interval;
        alarm->mOperation = operation;

        // Remove this alarm if already scheduled.
        RemoveLocked(operation);

        Int32 index = AddAlarmLocked(alarm);

        if (index == 0 || alarm->mType == 4) {
            SetLocked(alarm);
        }
    }

    return NOERROR;
}

ECode CAlarmManagerService::SetInexactRepeating(
    /* [in] */ Int32 type,
    /* [in] */ Int64 triggerAtTime,
    /* [in] */ Int64 interval,
    /* [in] */ IPendingIntent* operation)
{
    if (operation == NULL) {
        Slogger::W(TAG, "setInexactRepeating ignored because there is no intent");
        return NOERROR;
    }

    if (interval <= 0) {
        StringBuilder buf;
        buf += "setInexactRepeating ignored because interval ";
        buf += interval;
        buf += " is invalid";
        Slogger::W(TAG, buf.ToString());

        return NOERROR;
    }

    // If the requested interval isn't a multiple of 15 minutes, just treat it as exact
    if (interval % QUANTUM != 0) {
        if (localLOGV) {
            StringBuilder buf;
            buf += "Interval ";
            buf += interval;
            buf += " not a quantum multiple";
            Slogger::V(TAG, buf.ToString());
        }

        SetRepeating(type, triggerAtTime, interval, operation);
        return NOERROR;
    }

    // Translate times into the ELAPSED timebase for alignment purposes so that
    // alignment never tries to match against wall clock times.
    Boolean isRtc = (type == IAlarmManager::RTC
        || type == IAlarmManager::RTC_WAKEUP
        || type == IAlarmManager::RTC_SHUTDOWN_WAKEUP);

    Int64 skew = 0;
    if (isRtc) {
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int64 ctm;
        system->GetCurrentTimeMillis(&ctm);
        skew = ctm - SystemClock::GetElapsedRealtime();
    }

    // Slip forward to the next ELAPSED-timebase quantum after the stated time.  If
    // we're *at* a quantum point, leave it alone.
    Int64 adjustedTriggerTime;
    Int64 offset = (triggerAtTime - skew) % QUANTUM;

    if (offset != 0) {
        adjustedTriggerTime = triggerAtTime - offset + QUANTUM;
    } else {
        adjustedTriggerTime = triggerAtTime;
    }

    // Set the alarm based on the quantum-aligned start time
    if (localLOGV) {
        StringBuilder buf;
        buf += "setInexactRepeating: type=";
        buf += type;
        buf += " interval=";
        buf += interval;
        buf += " trigger=";
        buf += adjustedTriggerTime;
        buf += " orig=";
        buf += triggerAtTime;
        Slogger::V(TAG, buf.ToString());
    }

    SetRepeating(type, adjustedTriggerTime, interval, operation);
    return NOERROR;
}

ECode CAlarmManagerService::SetTime(
    /* [in] */ Int64 millis)
{
    PEL("CAlarmManagerService::SetTime")
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::SET_TIME, String("SetTime")));
    SystemClock::SetCurrentTimeMillis(millis);
    return NOERROR;
}

ECode CAlarmManagerService::SetTimeZone(
    /* [in] */ const String& tz)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::SET_TIME_ZONE, String("SetTimeZone")));

    AutoPtr<IBinderHelper> binderHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);

    Int64 oldId;
    binderHelper->ClearCallingIdentity(&oldId);

    if (tz.IsNullOrEmpty()) {
        return NOERROR;
    }

    AutoPtr<ITimeZoneHelper> timezoneHelper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&timezoneHelper);

    AutoPtr<ITimeZone> zone;
    timezoneHelper->GetTimeZone(tz, (ITimeZone**)&zone);

    // Prevent reentrant calls from stepping on each other when writing
    // the time zone property
    Boolean timeZoneWasChanged = FALSE;
    String id;
    zone->GetID(&id);

    {
        AutoLock lock(mThisLock);
        AutoPtr<ISystemProperties> sysProp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
        String current;
        sysProp->Get(TIMEZONE_PROPERTY, &current);
        // Slogger::I(TAG,"GetTimeZone now is : %s",current.string());

        if (current.IsNullOrEmpty() || !current.Equals(id)) {
            if (localLOGV) {
                StringBuilder buf;
                buf += "timezone changed: ";
                buf += current;
                buf += ", new=";
                buf += id;
                Slogger::V(TAG, buf.ToString());
            }

            timeZoneWasChanged = TRUE;
            sysProp->Set(TIMEZONE_PROPERTY, id);
        }

        // Update the kernel timezone information
        // Kernel tracks time offsets as 'minutes west of GMT'
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int64 now;
        system->GetCurrentTimeMillis(&now);
        Int32 gmtOffset;
        zone->GetOffset(now, &gmtOffset);
        NativeSetKernelTimezone(mDescriptor, -(gmtOffset / 60000));
    }

    timezoneHelper->SetDefault(NULL);

    if (timeZoneWasChanged) {
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_TIMEZONE_CHANGED, (IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
        intent->PutExtra(String("time-zone"), id);
        AutoPtr<IUserHandleHelper> handleHelper;
        ASSERT_SUCCEEDED(CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handleHelper));
        AutoPtr<IUserHandle> userAll;
        handleHelper->GetALL((IUserHandle**)&userAll);
        mContext->SendBroadcastAsUser(intent, userAll);
    }

    binderHelper->RestoreCallingIdentity(oldId);
    // Slogger::I(TAG,"SetTimeZone now is : %s",SystemProperties::Get(TIMEZONE_PROPERTY).string());
    return NOERROR;
}

ECode CAlarmManagerService::Remove(
    /* [in] */ IPendingIntent* operation)
{
    if (operation == NULL) {
        return NOERROR;
    }

    {
        AutoLock lock(mLock);
        RemoveLocked(operation);
        return NOERROR;
    }
}

void CAlarmManagerService::RemoveLocked(
    /* [in] */ IPendingIntent* operation)
{
    RemoveLocked(mRtcWakeupShutdownAlarms, operation);
    RemoveLocked(mRtcWakeupAlarms, operation);
    RemoveLocked(mRtcAlarms, operation);
    RemoveLocked(mElapsedRealtimeWakeupAlarms, operation);
    RemoveLocked(mElapsedRealtimeAlarms, operation);
}

void CAlarmManagerService::RemoveLocked(
    /* [in] */ const String& packageName)
{
    RemoveLocked(mRtcWakeupShutdownAlarms, packageName);
    RemoveLocked(mRtcWakeupAlarms, packageName);
    RemoveLocked(mRtcAlarms, packageName);
    RemoveLocked(mElapsedRealtimeWakeupAlarms, packageName);
    RemoveLocked(mElapsedRealtimeAlarms, packageName);
}

void CAlarmManagerService::RemoveUserLocked(
    /* [in] */ Int32 userHandle)
{
    RemoveUserLocked(mRtcWakeupAlarms, userHandle);
    RemoveUserLocked(mRtcAlarms, userHandle);
    RemoveUserLocked(mElapsedRealtimeWakeupAlarms, userHandle);
    RemoveUserLocked(mElapsedRealtimeAlarms, userHandle);
}

Boolean CAlarmManagerService::LookForPackageLocked(
    /* [in] */ const String& packageName)
{
    return LookForPackageLocked(mRtcWakeupShutdownAlarms, packageName)
            || LookForPackageLocked(mRtcWakeupAlarms, packageName)
            || LookForPackageLocked(mRtcAlarms, packageName)
            || LookForPackageLocked(mElapsedRealtimeWakeupAlarms, packageName)
            || LookForPackageLocked(mElapsedRealtimeAlarms, packageName);
}

Int64 CAlarmManagerService::TimeToNextAlarm()
{
    Int64 nextAlarm = Elastos::Core::Math::INT64_MAX_VALUE;

    {
        AutoLock lock(mLock);
        AutoPtr< AlarmList > alarmList;
        for (Int32 i = IAlarmManager::RTC_WAKEUP; i <= IAlarmManager::RTC_SHUTDOWN_WAKEUP; i++) {
            alarmList = GetAlarmList(i);

            if (alarmList->IsEmpty() == FALSE) {
                AutoPtr<Alarm> a = (*alarmList)[0];

                if (a->mWhen < nextAlarm) {
                    nextAlarm = a->mWhen;
                }
            }
        }
    }

    return nextAlarm;
}

void CAlarmManagerService::RemoveLocked(
    /* [in] */ List<AutoPtr<CAlarmManagerService::Alarm> >* alarmList,
    /* [in] */ IPendingIntent* operation)
{
    if (alarmList->IsEmpty()) {
        return;
    }

    // iterator over the list removing any it where the intent match
    AlarmListIterator it = alarmList->Begin();
    Boolean result;
    while (it != alarmList->End()) {
        (*it)->mOperation->Equals((IInterface*)operation, &result);
        if (result) {
            it = alarmList->Erase(it);
        } else {
            ++it;
        }
    }
}

void CAlarmManagerService::RemoveLocked(
    /* [in] */ List<AutoPtr<CAlarmManagerService::Alarm> >* alarmList,
    /* [in] */ const String& packageName)
{
    if (alarmList->IsEmpty()) {
        return;
    }

    // iterator over the list removing any it where the intent match
    AlarmListIterator it = alarmList->Begin();
    while (it != alarmList->End()) {
        String pkg;
        (*it)->mOperation->GetTargetPackage(&pkg);

        if (pkg.Equals(packageName)) {
            it = alarmList->Erase(it);
        }
        else {
            ++it;
        }
    }
}

void CAlarmManagerService::RemoveUserLocked(
    /* [in] */ List<AutoPtr<CAlarmManagerService::Alarm> >* alarmList,
    /* [in] */ Int32 userHandle)
{
    if (alarmList->IsEmpty()) {
        return;
    }

    // iterator over the list removing any it where the intent match
    AlarmListIterator it = alarmList->Begin();
    while (it != alarmList->End()) {
        Int32 uid;
        (*it)->mOperation->GetCreatorUid(&uid);
        if (UserHandle::GetUserId(uid) == userHandle) {
            it = alarmList->Erase(it);
        }
        else {
            ++it;
        }
    }
}

Boolean CAlarmManagerService::LookForPackageLocked(
    /* [in] */ List< AutoPtr<CAlarmManagerService::Alarm> >* alarmList,
    /* [in] */ const String& packageName)
{
    AlarmListIterator it = alarmList->Begin();
    for (; it != alarmList->End(); ++it) {
        String pkg;
        (*it)->mOperation->GetTargetPackage(&pkg);

        if (pkg.Equals(packageName)) {
            return TRUE;
        }
    }

    return FALSE;
}

AutoPtr< List<AutoPtr<CAlarmManagerService::Alarm> > > CAlarmManagerService::GetAlarmList(
    /* [in] */ Int32 type)
{
    switch (type) {
        case IAlarmManager::RTC_WAKEUP:              return mRtcWakeupAlarms;
        case IAlarmManager::RTC:                     return mRtcAlarms;
        case IAlarmManager::ELAPSED_REALTIME_WAKEUP: return mElapsedRealtimeWakeupAlarms;
        case IAlarmManager::ELAPSED_REALTIME:        return mElapsedRealtimeAlarms;
        case IAlarmManager::RTC_SHUTDOWN_WAKEUP:     return mRtcWakeupShutdownAlarms;
    }
    return NULL;
}

Int32 CAlarmManagerService::AddAlarmLocked(
    /* [in] */ AutoPtr<CAlarmManagerService::Alarm> alarm)
{
    AutoPtr< AlarmList > alarmList = GetAlarmList(alarm->mType);

    Int32 index = 0;
    AlarmListIterator it = alarmList->Begin();
    // for (; it != alarmList->End(); ++it, ++index) {
    //     if ((*it).Get() == alarm) {
    //         break;
    //     }
    // }

    // if (it == alarmList->End()) {
    //     alarmList->PushBack(alarm);
    // }
    // else {
    //     alarmList->Insert(it, alarm);
    // }

    if (alarmList->IsEmpty()) {
        alarmList->PushBack(alarm);
        index = 0;
    } else if (alarm->mWhen <= (*it)->mWhen) {
        alarmList->Insert(it, alarm);
        index = 0;
    } else {
        it++;
        index++;
        while (it != alarmList->End()){
            if (alarm->mWhen <= (*it)->mWhen){
                alarmList->Insert(it, alarm);
                break;
            } else {
                it++;
                index++;
            }
        }

        if (index == alarmList->GetSize()) {
            alarmList->PushBack(alarm);
        }
    }

    // if (localLOGV) {
    //     // Display the list of alarms for this alarm type
    //     StringBuilder buf;
    //     buf += "alarms: ";
    //     buf += alarmList->GetSize();
    //     buf += " type: ";
    //     buf += alarm->mType;
    //     Slogger::V(TAG, buf.ToString());

    //     Int32 position = 0;

    //     AlarmListIterator it;
    //     for (it = alarmList->Begin(); it != alarmList->End(); ++it) {
    //         AutoPtr<ITime> time;
    //         CTime::New((ITime**)&time);
    //         time->Set((*it)->mWhen);

    //         String timeStr;
    //         time->Format(String("%b %d %I:%M:%S %p"), &timeStr);
    //         String str;
    //         (*it)->mOperation->GetTargetPackage(&str);

    //         StringBuilder msg;
    //         msg += position;
    //         msg += ": ";
    //         msg += timeStr;
    //         msg += " ";
    //         msg += str;
    //         Slogger::V(TAG, msg.ToString());

    //         position += 1;
    //     }
    // }

    return index;
}

void CAlarmManagerService::SetLocked(
    /* [in] */ AutoPtr<CAlarmManagerService::Alarm> alarm)
{
    if (mDescriptor != -1) {
        // The kernel never triggers alarms with negative wakeup times
        // so we ensure they are positive.
        Int64 alarmSeconds, alarmNanoseconds;

        if (alarm->mWhen < 0) {
            alarmSeconds = 0;
            alarmNanoseconds = 0;
        }
        else {
            alarmSeconds = alarm->mWhen / 1000;
            alarmNanoseconds = (alarm->mWhen % 1000) * 1000 * 1000;
        }

        NativeSet(mDescriptor, alarm->mType, alarmSeconds, alarmNanoseconds);
    }
    else {
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(AlarmHandler::ALARM_EVENT, (IMessage**)&msg);

        mHandler->RemoveMessages(AlarmHandler::ALARM_EVENT);
        Boolean result;
        mHandler->SendMessageAtTime(msg, alarm->mWhen, &result);
    }
}

void CAlarmManagerService::TriggerAlarmsLocked(
    /* [in] */ List<AutoPtr<CAlarmManagerService::Alarm> >* alarmList,
    /* [in] */ List<AutoPtr<CAlarmManagerService::Alarm> >* triggerList,
    /* [in] */ Int64 now)
{
    AlarmListIterator it = alarmList->Begin();
    AlarmList repeats;

    while (it != alarmList->End()) {
        AutoPtr<Alarm> alarm = *it;

        //if (localLOGV) Slog.v(TAG, "Checking active alarm when=" + alarm.when + " " + alarm);

        if (alarm->mWhen > now) {
            // don't fire alarms in the future
            break;
        }

        // If the alarm is late, then print a warning message.
        // Note that this can happen if the user creates a new event on
        // the Calendar app with a reminder that is in the past. In that
        // case, the reminder alarm will fire immediately.
        if (localLOGV && now - alarm->mWhen > LATE_ALARM_THRESHOLD) {
            StringBuilder buf;
            buf += "alarm is late! alarm time: ";
            buf += alarm->mWhen;
            buf += " now: ";
            buf += now;
            buf += " delay (in seconds): ";
            buf += ((now - alarm->mWhen) / 1000);
            Slogger::V(TAG, buf.ToString());
        }

        // Recurring alarms may have passed several alarm intervals while the
        // phone was asleep or off, so pass a trigger count when sending them.
        //if (localLOGV) Slog.v(TAG, "Alarm triggering: " + alarm);
        alarm->mCount = 1;

        if (alarm->mRepeatInterval > 0) {
            // this adjustment will be zero if we're late by
            // less than one full repeat interval
            alarm->mCount += (now - alarm->mWhen) / alarm->mRepeatInterval;
        }

        triggerList->PushBack(alarm);

        // remove the alarm from the list
        it = alarmList->Erase(it);

        // if it repeats queue it up to be read-added to the list
        if (alarm->mRepeatInterval > 0) {
            repeats.PushBack(alarm);
        }
    }

    // reset any repeating alarms.
    AlarmListIterator repeatsIt = repeats.Begin();
    while (repeatsIt != repeats.End()) {
        AutoPtr<Alarm> alarm = *repeatsIt;
        alarm->mWhen += alarm->mCount * alarm->mRepeatInterval;
        AddAlarmLocked(alarm);
        repeatsIt++;
    }

    if (alarmList->IsEmpty() == FALSE) {
        SetLocked((*alarmList)[0]);
    }
}

void CAlarmManagerService::SetWakelockWorkSource(
    /* [in] */ IPendingIntent* pi)
{
    Int32 uid;
    AutoPtr<IIIntentSender> sender;
    AutoPtr<IIActivityManager> activityMgr;
    activityMgr = ActivityManagerNative::GetDefault();
    pi->GetTarget((IIIntentSender**)&sender);
    activityMgr->GetUidForIntentSender(sender, &uid);

    if (uid >= 0) {
        AutoPtr<IWorkSource> src;
        CWorkSource::New(uid, (IWorkSource**)&src);
        mWakeLock->SetWorkSource(src);
        return;
    }

    // Something went wrong; fall back to attributing the lock to the OS
    if (mWakeLock) mWakeLock->SetWorkSource(NULL);
}

AutoPtr<CAlarmManagerService::BroadcastStats> CAlarmManagerService::GetStatsLocked(
    /* [in] */ IPendingIntent* pi)
{
    String pkg;
    pi->GetTargetPackage(&pkg);

    HashMap<String, AutoPtr<BroadcastStats> >::Iterator it = mBroadcastStats.Find(pkg);

    if (it == mBroadcastStats.End()) {
        AutoPtr<BroadcastStats> bs = new BroadcastStats(pkg);
        mBroadcastStats.Insert(HashMap<String, AutoPtr<BroadcastStats> >::ValueType(pkg, bs));
        return bs;
    }

    return (it->mSecond);
}

ECode CAlarmManagerService::ToString(
        /* [out] */ String* str)
{
    return E_NOT_IMPLEMENTED;
}

Int32 CAlarmManagerService::NativeInit()
{
    return open("/dev/alarm", O_RDWR);
}

void CAlarmManagerService::NativeClose(
    /* [in] */ Int32 fd)
{
    close(fd);
}

void CAlarmManagerService::NativeSet(
    /* [in] */ Int32 fd,
    /* [in] */ Int32 type,
    /* [in] */ Int64 seconds,
    /* [in] */ Int64 nanoseconds)
{
    struct timespec ts;
    ts.tv_sec = seconds;
    ts.tv_nsec = nanoseconds;

    Int32 result = ioctl(fd, ANDROID_ALARM_SET(type), &ts);
    if (result < 0) {
        //ALOGE("Unable to set alarm to %lld.%09lld: %s\n", seconds, nanoseconds, strerror(errno));
    }
}

Int32 CAlarmManagerService::NativeWaitForAlarm(
    /* [in] */ Int32 fd)
{
    Int32 result = 0;

    do{
        result = ioctl(fd, ANDROID_ALARM_WAIT);
    } while (result < 0 && errno == EINTR);

    if (result < 0) {
        //ALOGE("Unable to wait on alarm: %s\n", strerror(errno));
        return 0;
    }

    return result;
}

Int32 CAlarmManagerService::NativeSetKernelTimezone(
    /* [in] */ Int32 fd,
    /* [in] */ Int32 minuteswest)
{
    struct timezone tz;

    tz.tz_minuteswest = minuteswest;
    tz.tz_dsttime = 0;

    Int32 result = settimeofday(NULL, &tz);
    if (result < 0) {
        //ALOGE("Unable to set kernel timezone to %d: %s\n", minswest, strerror(errno));
        return -1;
    } else {
        //ALOGD("Kernel timezone updated to %d minutes west of GMT\n", minswest);
    }
    return 0;
}

} // Server
} // Droid
} // Elastos
