#ifndef __ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTCALENDARALERTS_H__
#define __ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTCALENDARALERTS_H__

#include "_Elastos_Droid_Provider_CCalendarContractCalendarAlerts.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

/**
 * Fields and helpers for accessing calendar alerts information. These
 * fields are for tracking which alerts have been fired. Scheduled alarms
 * will generate an intent using {@link #ACTION_EVENT_REMINDER}. Apps that
 * receive this action may update the {@link #STATE} for the reminder when
 * they have finished handling it. Apps that have their notifications
 * disabled should not modify the table to ensure that they do not conflict
 * with another app that is generating a notification. In general, apps
 * should not need to write to this table directly except to update the
 * state of a reminder.
 */
CarClass(CCalendarContractCalendarAlerts)
    , public Singleton
    , public ICalendarContractCalendarAlerts
    , public IBaseColumns
    , public ICalendarContractCalendarAlertsColumns
    , public ICalendarContractEventsColumns
    , public ICalendarContractCalendarColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The Uri for querying calendar alert information
     */
    //@SuppressWarnings("hiding")
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * This URI is for grouping the query results by event_id and begin
     * time.  This will return one result per instance of an event.  So
     * events with multiple alarms will appear just once, but multiple
     * instances of a repeating event will show up multiple times.
     */
    CARAPI GetCONTENT_URI_BY_INSTANCE(
        /* [out] */ IUri** uri);

    /**
     * Helper for inserting an alarm time associated with an event TODO move
     * to Provider
     *
     * @hide
     */
    CARAPI Insert(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Int64 eventId,
        /* [in] */ Int64 begin,
        /* [in] */ Int64 end,
        /* [in] */ Int64 alarmTime,
        /* [in] */ Int32 minutes,
        /* [out] */ IUri** uri);

    /**
     * Finds the next alarm after (or equal to) the given time and returns
     * the time of that alarm or -1 if no such alarm exists. This is a
     * blocking call and should not be done on the UI thread. TODO move to
     * provider
     *
     * @param cr the ContentResolver
     * @param millis the time in UTC milliseconds
     * @return the next alarm time greater than or equal to "millis", or -1
     *         if no such alarm exists.
     * @hide
     */
    CARAPI FindNextAlarmTime(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Int64 millis,
        /* [out] */ Int64* alarm);

    /**
     * Searches the CalendarAlerts table for alarms that should have fired
     * but have not and then reschedules them. This method can be called at
     * boot time to restore alarms that may have been lost due to a phone
     * reboot. TODO move to provider
     *
     * @param cr the ContentResolver
     * @param context the Context
     * @param manager the AlarmManager
     * @hide
     */
    CARAPI RescheduleMissedAlarms(
        /* [in] */ IContentResolver* cr,
        /* [in] */ IContext* context,
        /* [in] */ IAlarmManager* manager);

    /**
     * Schedules an alarm intent with the system AlarmManager that will
     * notify listeners when a reminder should be fired. The provider will
     * keep scheduled reminders up to date but apps may use this to
     * implement snooze functionality without modifying the reminders table.
     * Scheduled alarms will generate an intent using
     * {@link #ACTION_EVENT_REMINDER}. TODO Move to provider
     *
     * @param context A context for referencing system resources
     * @param manager The AlarmManager to use or null
     * @param alarmTime The time to fire the intent in UTC millis since
     *            epoch
     * @hide
     */
    CARAPI ScheduleAlarm(
        /* [in] */ IContext* context,
        /* [in] */ IAlarmManager* manager,
        /* [in] */ Int64 alarmTime);

    /**
     * Searches for an entry in the CalendarAlerts table that matches the
     * given event id, begin time and alarm time. If one is found then this
     * alarm already exists and this method returns true. TODO Move to
     * provider
     *
     * @param cr the ContentResolver
     * @param eventId the event id to match
     * @param begin the start time of the event in UTC millis
     * @param alarmTime the alarm time of the event in UTC millis
     * @return true if there is already an alarm for the given event with
     *         the same start time and alarm time.
     * @hide
     */
    CARAPI IsAlarmExists(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Int64 eventId,
        /* [in] */ Int64 begin,
        /* [in] */ Int64 alarmTime,
        /* [out] */ Boolean* isExit);

private:
    static const String WHERE_ALARM_EXISTS;

    static const String WHERE_FINDNEXTALARMTIME;;
    static const String SORT_ORDER_ALARMTIME_ASC;;

    static const String WHERE_RESCHEDULE_MISSED_ALARMS;;

    static const Boolean DEBUG;
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCALENDARCONTRACTCALENDARALERTS_H__
