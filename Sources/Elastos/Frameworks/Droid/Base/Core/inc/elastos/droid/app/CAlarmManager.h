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

#ifndef __ELASTOS_DROID_APP_CALARMMANAGER_H__
#define __ELASTOS_DROID_APP_CALARMMANAGER_H__

#include "_Elastos_Droid_App_CAlarmManager.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Content::IContext;


namespace Elastos {
namespace Droid {
namespace App {

/**
 * This class provides access to the system alarm services.  These allow you
 * to schedule your application to be run at some point in the future.  When
 * an alarm goes off, the {@link Intent} that had been registered for it
 * is broadcast by the system, automatically starting the target application
 * if it is not already running.  Registered alarms are retained while the
 * device is asleep (and can optionally wake the device up if they go off
 * during that time), but will be cleared if it is turned off and rebooted.
 *
 * <p>The Alarm Manager holds a CPU wake lock as long as the alarm receiver's
 * onReceive() method is executing. This guarantees that the phone will not sleep
 * until you have finished handling the broadcast. Once onReceive() returns, the
 * Alarm Manager releases this wake lock. This means that the phone will in some
 * cases sleep as soon as your onReceive() method completes.  If your alarm receiver
 * called {@link android.content.Context#startService Context.startService()}, it
 * is possible that the phone will sleep before the requested service is launched.
 * To prevent this, your BroadcastReceiver and Service will need to implement a
 * separate wake lock policy to ensure that the phone continues running until the
 * service becomes available.
 *
 * <p><b>Note: The Alarm Manager is intended for cases where you want to have
 * your application code run at a specific time, even if your application is
 * not currently running.  For normal timing operations (ticks, timeouts,
 * etc) it is easier and much more efficient to use
 * {@link android.os.Handler}.</b>
 *
 * <p class="caution"><strong>Note:</strong> Beginning with API 19
 * ({@link android.os.Build.VERSION_CODES#KITKAT}) alarm delivery is inexact:
 * the OS will shift alarms in order to minimize wakeups and battery use.  There are
 * new APIs to support applications which need strict delivery guarantees; see
 * {@link #setWindow(int, long, long, PendingIntent)} and
 * {@link #setExact(int, long, PendingIntent)}.  Applications whose {@code targetSdkVersion}
 * is earlier than API 19 will continue to see the previous behavior in which all
 * alarms are delivered exactly when requested.
 *
 * <p>You do not
 * instantiate this class directly; instead, retrieve it through
 * {@link android.content.Context#getSystemService
 * Context.getSystemService(Context.ALARM_SERVICE)}.
 */
CarClass(CAlarmManager)
    , public Object
    , public IAlarmManager
{
public:

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAlarmManager();

    /**
     * package private on purpose
     */
    CARAPI constructor(
        /* [in] */ IIAlarmManager* service,
        /* [in] */ IContext* ctx);

    /**
     * Schedule an alarm.  <b>Note: for timing operations (ticks, timeouts,
     * etc) it is easier and much more efficient to use
     * {@link android.os.Handler}.</b>  If there is already an alarm scheduled
     * for the same IntentSender, it will first be canceled.
     *
     * <p>If the time occurs in the past, the alarm will be triggered
     * immediately.  If there is already an alarm for this Intent
     * scheduled (with the equality of two intents being defined by
     * {@link Intent#filterEquals}), then it will be removed and replaced by
     * this one.
     *
     * <p>
     * The alarm is an intent broadcast that goes to a broadcast receiver that
     * you registered with {@link android.content.Context#registerReceiver}
     * or through the &lt;receiver&gt; tag in an AndroidManifest.xml file.
     *
     * <p>
     * Alarm intents are delivered with a data extra of type int called
     * {@link Intent#EXTRA_ALARM_COUNT Intent.EXTRA_ALARM_COUNT} that indicates
     * how many past alarm events have been accumulated into this intent
     * broadcast.  Recurring alarms that have gone undelivered because the
     * phone was asleep may have a count greater than one when delivered.
     *
     * @param type One of ELAPSED_REALTIME, ELAPSED_REALTIME_WAKEUP, RTC or
     *             RTC_WAKEUP.
     * @param triggerAtMillis time in milliseconds that the alarm should go
     * off, using the appropriate clock (depending on the alarm type).
     * @param operation Action to perform when the alarm goes off;
     * typically comes from {@link PendingIntent#getBroadcast
     * IntentSender.getBroadcast()}.
     *
     * @see android.os.Handler
     * @see #setRepeating
     * @see #cancel
     * @see android.content.Context#sendBroadcast
     * @see android.content.Context#registerReceiver
     * @see android.content.Intent#filterEquals
     * @see #ELAPSED_REALTIME
     * @see #ELAPSED_REALTIME_WAKEUP
     * @see #RTC
     * @see #RTC_WAKEUP
     */
    CARAPI Set(
        /* [in] */ Int32 type,
        /* [in] */ Int64 triggerAtMillis,
        /* [in] */ IPendingIntent* operation);

    /**
     * Schedule a repeating alarm.  <b>Note: for timing operations (ticks,
     * timeouts, etc) it is easier and much more efficient to use
     * {@link android.os.Handler}.</b>  If there is already an alarm scheduled
     * for the same IntentSender, it will first be canceled.
     *
     * <p>Like {@link #set}, except you can also
     * supply a rate at which the alarm will repeat.  This alarm continues
     * repeating until explicitly removed with {@link #cancel}.  If the time
     * occurs in the past, the alarm will be triggered immediately, with an
     * alarm count depending on how far in the past the trigger time is relative
     * to the repeat interval.
     *
     * <p>If an alarm is delayed (by system sleep, for example, for non
     * _WAKEUP alarm types), a skipped repeat will be delivered as soon as
     * possible.  After that, future alarms will be delivered according to the
     * original schedule; they do not drift over time.  For example, if you have
     * set a recurring alarm for the top of every hour but the phone was asleep
     * from 7:45 until 8:45, an alarm will be sent as soon as the phone awakens,
     * then the next alarm will be sent at 9:00.
     *
     * <p>If your application wants to allow the delivery times to drift in
     * order to guarantee that at least a certain time interval always elapses
     * between alarms, then the approach to take is to use one-time alarms,
     * scheduling the next one yourself when handling each alarm delivery.
     *
     * @param type One of ELAPSED_REALTIME, ELAPSED_REALTIME_WAKEUP}, RTC or
     *             RTC_WAKEUP.
     * @param triggerAtMillis time in milliseconds that the alarm should first
     * go off, using the appropriate clock (depending on the alarm type).
     * @param intervalMillis interval in milliseconds between subsequent repeats
     * of the alarm.
     * @param operation Action to perform when the alarm goes off;
     * typically comes from {@link PendingIntent#getBroadcast
     * IntentSender.getBroadcast()}.
     *
     * @see android.os.Handler
     * @see #set
     * @see #cancel
     * @see android.content.Context#sendBroadcast
     * @see android.content.Context#registerReceiver
     * @see android.content.Intent#filterEquals
     * @see #ELAPSED_REALTIME
     * @see #ELAPSED_REALTIME_WAKEUP
     * @see #RTC
     * @see #RTC_WAKEUP
     */
    CARAPI SetRepeating(
        /* [in] */ Int32 type,
        /* [in] */ Int64 triggerAtMillis,
        /* [in] */ Int64 intervalMillis,
        /* [in] */ IPendingIntent* operation);

    /**
     * Schedule an alarm to be delivered within a given window of time.  This method
     * is similar to {@link #set(int, long, PendingIntent)}, but allows the
     * application to precisely control the degree to which its delivery might be
     * adjusted by the OS. This method allows an application to take advantage of the
     * battery optimizations that arise from delivery batching even when it has
     * modest timeliness requirements for its alarms.
     *
     * <p>
     * This method can also be used to achieve strict ordering guarantees among
     * multiple alarms by ensuring that the windows requested for each alarm do
     * not intersect.
     *
     * <p>
     * When precise delivery is not required, applications should use the standard
     * {@link #set(int, long, PendingIntent)} method.  This will give the OS the most
     * flexibility to minimize wakeups and battery use.  For alarms that must be delivered
     * at precisely-specified times with no acceptable variation, applications can use
     * {@link #setExact(int, long, PendingIntent)}.
     *
     * @param type One of {@link #ELAPSED_REALTIME}, {@link #ELAPSED_REALTIME_WAKEUP},
     *        {@link #RTC}, or {@link #RTC_WAKEUP}.
     * @param windowStartMillis The earliest time, in milliseconds, that the alarm should
     *        be delivered, expressed in the appropriate clock's units (depending on the alarm
     *        type).
     * @param windowLengthMillis The length of the requested delivery window,
     *        in milliseconds.  The alarm will be delivered no later than this many
     *        milliseconds after {@code windowStartMillis}.  Note that this parameter
     *        is a <i>duration,</i> not the timestamp of the end of the window.
     * @param operation Action to perform when the alarm goes off;
     *        typically comes from {@link PendingIntent#getBroadcast
     *        IntentSender.getBroadcast()}.
     *
     * @see #set
     * @see #setExact
     * @see #setRepeating
     * @see #cancel
     * @see android.content.Context#sendBroadcast
     * @see android.content.Context#registerReceiver
     * @see android.content.Intent#filterEquals
     * @see #ELAPSED_REALTIME
     * @see #ELAPSED_REALTIME_WAKEUP
     * @see #RTC
     * @see #RTC_WAKEUP
     */
    CARAPI SetWindow(
        /* [in] */ Int32 type,
        /* [in] */ Int64 windowStartMillis,
        /* [in] */ Int64 windowLengthMillis,
        /* [in] */ IPendingIntent* operation);

    /**
     * Schedule an alarm to be delivered precisely at the stated time.
     *
     * <p>
     * This method is like {@link #set(int, long, PendingIntent)}, but does not permit
     * the OS to adjust the delivery time.  The alarm will be delivered as nearly as
     * possible to the requested trigger time.
     *
     * <p>
     * <b>Note:</b> only alarms for which there is a strong demand for exact-time
     * delivery (such as an alarm clock ringing at the requested time) should be
     * scheduled as exact.  Applications are strongly discouraged from using exact
     * alarms unnecessarily as they reduce the OS's ability to minimize battery use.
     *
     * @param type One of {@link #ELAPSED_REALTIME}, {@link #ELAPSED_REALTIME_WAKEUP},
     *        {@link #RTC}, or {@link #RTC_WAKEUP}.
     * @param triggerAtMillis time in milliseconds that the alarm should go
     *        off, using the appropriate clock (depending on the alarm type).
     * @param operation Action to perform when the alarm goes off;
     *        typically comes from {@link PendingIntent#getBroadcast
     *        IntentSender.getBroadcast()}.
     *
     * @see #set
     * @see #setRepeating
     * @see #setWindow
     * @see #cancel
     * @see android.content.Context#sendBroadcast
     * @see android.content.Context#registerReceiver
     * @see android.content.Intent#filterEquals
     * @see #ELAPSED_REALTIME
     * @see #ELAPSED_REALTIME_WAKEUP
     * @see #RTC
     * @see #RTC_WAKEUP
     */
    CARAPI SetExact(
        /* [in] */ Int32 type,
        /* [in] */ Int64 triggerAtMillis,
        /* [in] */ IPendingIntent* operation);

    /**
     * Schedule an alarm that represents an alarm clock.
     *
     * The system may choose to display information about this alarm to the user.
     *
     * <p>
     * This method is like {@link #setExact(int, long, PendingIntent)}, but implies
     * {@link #RTC_WAKEUP}.
     *
     * @param info
     * @param operation Action to perform when the alarm goes off;
     *        typically comes from {@link PendingIntent#getBroadcast
     *        IntentSender.getBroadcast()}.
     *
     * @see #set
     * @see #setRepeating
     * @see #setWindow
     * @see #setExact
     * @see #cancel
     * @see #getNextAlarmClock()
     * @see android.content.Context#sendBroadcast
     * @see android.content.Context#registerReceiver
     * @see android.content.Intent#filterEquals
     */
    CARAPI SetAlarmClock(
        /* [in] */ IAlarmClockInfo* info,
        /* [in] */ IPendingIntent* operation);

    /** @hide */
    CARAPI Set(
        /* [in] */ Int32 type,
        /* [in] */ Int64 triggerAtMillis,
        /* [in] */ Int64 windowMillis,
        /* [in] */ Int64 intervalMillis,
        /* [in] */ IPendingIntent* operation,
        /* [in] */ IWorkSource* workSource);

    CARAPI SetImpl(
        /* [in] */ Int32 type,
        /* [in] */ Int64 triggerAtMillis,
        /* [in] */ Int64 windowMillis,
        /* [in] */ Int64 intervalMillis,
        /* [in] */ IPendingIntent* operation,
        /* [in] */ IWorkSource* workSource,
        /* [in] */ IAlarmClockInfo* alarmClock);

    /**
     * Schedule a repeating alarm that has inexact trigger time requirements;
     * for example, an alarm that repeats every hour, but not necessarily at
     * the top of every hour.  These alarms are more power-efficient than
     * the strict recurrences supplied by {@link #setRepeating}, since the
     * system can adjust alarms' phase to cause them to fire simultaneously,
     * avoiding waking the device from sleep more than necessary.
     *
     * <p>Your alarm's first trigger will not be before the requested time,
     * but it might not occur for almost a full interval after that time.  In
     * addition, while the overall period of the repeating alarm will be as
     * requested, the time between any two successive firings of the alarm
     * may vary.  If your application demands very low jitter, use
     * {@link #setRepeating} instead.
     *
     * @param type One of ELAPSED_REALTIME, ELAPSED_REALTIME_WAKEUP}, RTC or
     *             RTC_WAKEUP.
     * @param triggerAtMillis time in milliseconds that the alarm should first
     * go off, using the appropriate clock (depending on the alarm type).  This
     * is inexact: the alarm will not fire before this time, but there may be a
     * delay of almost an entire alarm interval before the first invocation of
     * the alarm.
     * @param intervalMillis interval in milliseconds between subsequent repeats
     * of the alarm.  If this is one of INTERVAL_FIFTEEN_MINUTES,
     * INTERVAL_HALF_HOUR, INTERVAL_HOUR, INTERVAL_HALF_DAY, or INTERVAL_DAY
     * then the alarm will be phase-aligned with other alarms to reduce the
     * number of wakeups.  Otherwise, the alarm will be set as though the
     * application had called {@link #setRepeating}.
     * @param operation Action to perform when the alarm goes off;
     * typically comes from {@link PendingIntent#getBroadcast
     * IntentSender.getBroadcast()}.
     *
     * @see android.os.Handler
     * @see #set
     * @see #cancel
     * @see android.content.Context#sendBroadcast
     * @see android.content.Context#registerReceiver
     * @see android.content.Intent#filterEquals
     * @see #ELAPSED_REALTIME
     * @see #ELAPSED_REALTIME_WAKEUP
     * @see #RTC
     * @see #RTC_WAKEUP
     * @see #INTERVAL_FIFTEEN_MINUTES
     * @see #INTERVAL_HALF_HOUR
     * @see #INTERVAL_HOUR
     * @see #INTERVAL_HALF_DAY
     * @see #INTERVAL_DAY
     */
    CARAPI SetInexactRepeating(
        /* [in] */ Int32 type,
        /* [in] */ Int64 triggerAtMillis,
        /* [in] */ Int64 intervalMillis,
        /* [in] */ IPendingIntent* operation);

    /**
     * Remove any alarms with a matching {@link Intent}.
     * Any alarm, of any type, whose Intent matches this one (as defined by
     * {@link Intent#filterEquals}), will be canceled.
     *
     * @param operation IntentSender which matches a previously added
     * IntentSender.
     *
     * @see #set
     */
    CARAPI Cancel(
        /* [in] */ IPendingIntent* operation);

    /**
     * Set the system wall clock time.
     * Requires the permission android.permission.SET_TIME.
     *
     * @param millis time in milliseconds since the Epoch
     */
    CARAPI SetTime(
        /* [in] */ Int64 millis);

    /**
     * Set the system default time zone.
     * Requires the permission android.permission.SET_TIME_ZONE.
     *
     * @param timeZone in the format understood by {@link java.util.TimeZone}
     */
    CARAPI SetTimeZone(
        /* [in] */ const String& timeZone);

    CARAPI GetNextAlarmClock(
        /* [out] */ IAlarmClockInfo** info);

    /**
     * Gets information about the next alarm clock currently scheduled.
     *
     * The alarm clocks considered are those scheduled by {@link #setAlarmClock}
     * from any package of the given {@parm userId}.
     *
     * @see #setAlarmClock
     * @see AlarmClockInfo
     *
     * @hide
     */
    CARAPI GetNextAlarmClock(
        /* [in] */ Int32 userId,
        /* [out] */ IAlarmClockInfo** info);

private:
    CARAPI_(Int64) LegacyExactLength();

private:
    static const String TAG;

    AutoPtr<IIAlarmManager> mService;
    Boolean mAlwaysExact;
};

}
}
}

#endif //__ELASTOS_DROID_APP_CALARMMANAGER_H__
