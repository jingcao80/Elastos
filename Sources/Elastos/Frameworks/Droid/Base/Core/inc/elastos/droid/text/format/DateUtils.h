#ifndef __ELASTOS_DROID_TEXT_FORMAT_DATEUTILS_H__
#define __ELASTOS_DROID_TEXT_FORMAT_DATEUTILS_H__

#include "Elastos.Droid.Core.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Text::IDateFormat;
using Elastos::Utility::ICalendar;
using Elastos::Utility::IFormatter;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Format {

/**
 * This class contains various date-related utilities for creating text for things like
 * elapsed time and date ranges, strings for days of the week and months, and AM/PM text etc.
 */
class ECO_PUBLIC DateUtils
{
public:
    /**
     * Return a string for the day of the week.
     * @param dayOfWeek One of {@link Calendar#SUNDAY Calendar.SUNDAY},
     *               {@link Calendar#MONDAY Calendar.MONDAY}, etc.
     * @param abbrev One of {@link #LENGTH_LONG}, {@link #LENGTH_SHORT},
     *               {@link #LENGTH_MEDIUM}, or {@link #LENGTH_SHORTEST}.
     *               Note that in most languages, {@link #LENGTH_SHORT}
     *               will return the same as {@link #LENGTH_MEDIUM}.
     *               Undefined lengths will return {@link #LENGTH_MEDIUM}
     *               but may return something different in the future.
     * @throws IndexOutOfBoundsException if the dayOfWeek is out of bounds.
     * @deprecated use {@link java.text.SimpleDateFormat} instead.
     */
    //@Deprecated
    static CARAPI_(String) GetDayOfWeekString(
        /* [in] */ Int32 dayOfWeek,
        /* [in] */ Int32 abbrev);

    /**
     * Return a localized string for AM or PM.
     * @param ampm Either {@link Calendar#AM Calendar.AM} or {@link Calendar#PM Calendar.PM}.
     * @throws IndexOutOfBoundsException if the ampm is out of bounds.
     * @return Localized version of "AM" or "PM".
     * @deprecated use {@link java.text.SimpleDateFormat} instead.
     */
    //@Deprecated
    static CARAPI_(String) GetAMPMString(
        /* [in] */ Int32 ampm);

    /**
     * Return a localized string for the month of the year.
     * @param month One of {@link Calendar#JANUARY Calendar.JANUARY},
     *               {@link Calendar#FEBRUARY Calendar.FEBRUARY}, etc.
     * @param abbrev One of {@link #LENGTH_LONG}, {@link #LENGTH_MEDIUM},
     *               or {@link #LENGTH_SHORTEST}.
     *               Undefined lengths will return {@link #LENGTH_MEDIUM}
     *               but may return something different in the future.
     * @return Localized month of the year.
     * @deprecated Use {@link java.text.SimpleDateFormat} instead.
     */
    //@Deprecated
    static CARAPI_(String) GetMonthString(
        /* [in] */ Int32 month,
        /* [in] */ Int32 abbrev);

    /**
     * Return a localized string for the month of the year, for
     * contexts where the month is not formatted together with
     * a day of the month.
     *
     * @param month One of {@link Calendar#JANUARY Calendar.JANUARY},
     *               {@link Calendar#FEBRUARY Calendar.FEBRUARY}, etc.
     * @param abbrev One of {@link #LENGTH_LONG}, {@link #LENGTH_MEDIUM},
     *               or {@link #LENGTH_SHORTEST}.
     *               Undefined lengths will return {@link #LENGTH_MEDIUM}
     *               but may return something different in the future.
     * @return Localized month of the year.
     * @hide Pending API council approval
     * @deprecated use {@link java.text.SimpleDateFormat} instead.
     */
    //@Deprecated
    static CARAPI_(String) GetStandaloneMonthString(
        /* [in] */ Int32 month,
        /* [in] */ Int32 abbrev);

    /**
     * Returns a string describing the elapsed time since startTime.
     * @param startTime some time in the past.
     * @return a String object containing the elapsed time.
     * @see #getRelativeTimeSpanString(long, long, long)
     */
    static CARAPI_(AutoPtr<ICharSequence>) GetRelativeTimeSpanString(
        /* [in] */ Int64 startTime);

    /**
     * Returns a string describing 'time' as a time relative to 'now'.
     * <p>
     * Time spans in the past are formatted like "42 minutes ago".
     * Time spans in the future are formatted like "in 42 minutes".
     *
     * @param time the time to describe, in milliseconds
     * @param now the current time in milliseconds
     * @param minResolution the minimum timespan to report. For example, a time 3 seconds in the
     *     past will be reported as "0 minutes ago" if this is set to MINUTE_IN_MILLIS. Pass one of
     *     0, MINUTE_IN_MILLIS, HOUR_IN_MILLIS, DAY_IN_MILLIS, WEEK_IN_MILLIS
     */
    static CARAPI_(AutoPtr<ICharSequence>) GetRelativeTimeSpanString(
        /* [in] */ Int64 time,
        /* [in] */ Int64 now,
        /* [in] */ Int64 minResolution);

    /**
     * Returns a string describing 'time' as a time relative to 'now'.
     * <p>
     * Time spans in the past are formatted like "42 minutes ago". Time spans in
     * the future are formatted like "in 42 minutes".
     * <p>
     * Can use {@link #FORMAT_ABBREV_RELATIVE} flag to use abbreviated relative
     * times, like "42 mins ago".
     *
     * @param time the time to describe, in milliseconds
     * @param now the current time in milliseconds
     * @param minResolution the minimum timespan to report. For example, a time
     *            3 seconds in the past will be reported as "0 minutes ago" if
     *            this is set to MINUTE_IN_MILLIS. Pass one of 0,
     *            MINUTE_IN_MILLIS, HOUR_IN_MILLIS, DAY_IN_MILLIS,
     *            WEEK_IN_MILLIS
     * @param flags a bit mask of formatting options, such as
     *            {@link #FORMAT_NUMERIC_DATE} or
     *            {@link #FORMAT_ABBREV_RELATIVE}
     */
    static CARAPI_(AutoPtr<ICharSequence>) GetRelativeTimeSpanString(
        /* [in] */ Int64 time,
        /* [in] */ Int64 now,
        /* [in] */ Int64 minResolution,
        /* [in] */ Int32 flags);

    /**
     * Return string describing the elapsed time since startTime formatted like
     * "[relative time/date], [time]".
     * <p>
     * Example output strings for the US date format.
     * <ul>
     * <li>3 mins ago, 10:15 AM</li>
     * <li>yesterday, 12:20 PM</li>
     * <li>Dec 12, 4:12 AM</li>
     * <li>11/14/2007, 8:20 AM</li>
     * </ul>
     *
     * @param time some time in the past.
     * @param minResolution the minimum elapsed time (in milliseconds) to report
     *            when showing relative times. For example, a time 3 seconds in
     *            the past will be reported as "0 minutes ago" if this is set to
     *            {@link #MINUTE_IN_MILLIS}.
     * @param transitionResolution the elapsed time (in milliseconds) at which
     *            to stop reporting relative measurements. Elapsed times greater
     *            than this resolution will default to normal date formatting.
     *            For example, will transition from "6 days ago" to "Dec 12"
     *            when using {@link #WEEK_IN_MILLIS}.
     */
    static CARAPI_(AutoPtr<ICharSequence>) GetRelativeDateTimeString(
        /* [in] */ IContext* c,
        /* [in] */ Int64 time,
        /* [in] */ Int64 minResolution,
        /* [in] */ Int64 transitionResolution,
        /* [in] */ Int32 flags);

    /**
     * Format a time so it appears like it would in the status bar clock.
     * @deprecated use {@link #DateFormat.getTimeFormat(Context)} instead.
     * @hide
     */
    static /*final*/ CARAPI_(AutoPtr<ICharSequence>) TimeString(
        /* [in] */ Int64 millis);

    /**
     * Formats an elapsed time in the form "MM:SS" or "H:MM:SS"
     * for display on the call-in-progress screen.
     * @param elapsedSeconds the elapsed time in seconds.
     */
    static CARAPI_(String) FormatElapsedTime(
        /* [in] */ Int64 elapsedSeconds);

    /**
     * Formats an elapsed time in the form "MM:SS" or "H:MM:SS"
     * for display on the call-in-progress screen.
     *
     * @param recycle {@link StringBuilder} to recycle, if possible
     * @param elapsedSeconds the elapsed time in seconds.
     */
    static CARAPI_(String) FormatElapsedTime(
        /* [in] */ const String& recycle,
        /* [in] */ Int64 elapsedSeconds);

    /**
     * Format a date / time such that if the then is on the same day as now, it shows
     * just the time and if it's a different day, it shows just the date.
     *
     * <p>The parameters dateFormat and timeFormat should each be one of
     * {@link java.text.DateFormat#DEFAULT},
     * {@link java.text.DateFormat#FULL},
     * {@link java.text.DateFormat#LONG},
     * {@link java.text.DateFormat#MEDIUM}
     * or
     * {@link java.text.DateFormat#SHORT}
     *
     * @param then the date to format
     * @param now the base time
     * @param dateStyle how to format the date portion.
     * @param timeStyle how to format the time portion.
     */
    static /*final*/ CARAPI_(AutoPtr<ICharSequence>) FormatSameDayTime(
        /* [in] */ Int64 then,
        /* [in] */ Int64 now,
        /* [in] */ Int32 dateStyle,
        /* [in] */ Int32 timeStyle);

    /**
     * @return true if the supplied when is today else false
     */
    static CARAPI_(Boolean) IsToday(
        /* [in] */ Int64 when);

    /**
     * Formats a date or a time range according to the local conventions.
     * <p>
     * Note that this is a convenience method. Using it involves creating an
     * internal {@link java.util.Formatter} instance on-the-fly, which is
     * somewhat costly in terms of memory and time. This is probably acceptable
     * if you use the method only rarely, but if you rely on it for formatting a
     * large number of dates, consider creating and reusing your own
     * {@link java.util.Formatter} instance and use the version of
     * {@link #formatDateRange(Context, long, long, int) formatDateRange}
     * that takes a {@link java.util.Formatter}.
     *
     * @param context the context is required only if the time is shown
     * @param startMillis the start time in UTC milliseconds
     * @param endMillis the end time in UTC milliseconds
     * @param flags a bit mask of options See
     * {@link #formatDateRange(Context, Formatter, long, long, int, String) formatDateRange}
     * @return a string containing the formatted date/time range.
     */
    static CARAPI_(String) FormatDateRange(
        /* [in] */ IContext* context,
        /* [in] */ Int64 startMillis,
        /* [in] */ Int64 endMillis,
        /* [in] */ Int32 flags);

    /**
     * Formats a date or a time range according to the local conventions.
     * <p>
     * Note that this is a convenience method for formatting the date or
     * time range in the local time zone. If you want to specify the time
     * zone please use
     * {@link #formatDateRange(Context, Formatter, long, long, int, String) formatDateRange}.
     *
     * @param context the context is required only if the time is shown
     * @param formatter the Formatter used for formatting the date range.
     * Note: be sure to call setLength(0) on StringBuilder passed to
     * the Formatter constructor unless you want the results to accumulate.
     * @param startMillis the start time in UTC milliseconds
     * @param endMillis the end time in UTC milliseconds
     * @param flags a bit mask of options See
     * {@link #formatDateRange(Context, Formatter, long, long, int, String) formatDateRange}
     * @return a string containing the formatted date/time range.
     */
    static CARAPI_(AutoPtr<Elastos::Utility::IFormatter>) FormatDateRange(
        /* [in] */ IContext* context,
        /* [in] */ Elastos::Utility::IFormatter* formatter,
        /* [in] */ Int64 startMillis,
        /* [in] */ Int64 endMillis,
        /* [in] */ Int32 flags);

    /**
     * Formats a date or a time range according to the local conventions.
     *
     * <p>
     * Example output strings (date formats in these examples are shown using
     * the US date format convention but that may change depending on the
     * local settings):
     * <ul>
     *   <li>10:15am</li>
     *   <li>3:00pm - 4:00pm</li>
     *   <li>3pm - 4pm</li>
     *   <li>3PM - 4PM</li>
     *   <li>08:00 - 17:00</li>
     *   <li>Oct 9</li>
     *   <li>Tue, Oct 9</li>
     *   <li>October 9, 2007</li>
     *   <li>Oct 9 - 10</li>
     *   <li>Oct 9 - 10, 2007</li>
     *   <li>Oct 28 - Nov 3, 2007</li>
     *   <li>Dec 31, 2007 - Jan 1, 2008</li>
     *   <li>Oct 9, 8:00am - Oct 10, 5:00pm</li>
     *   <li>12/31/2007 - 01/01/2008</li>
     * </ul>
     *
     * <p>
     * The flags argument is a bitmask of options from the following list:
     *
     * <ul>
     *   <li>FORMAT_SHOW_TIME</li>
     *   <li>FORMAT_SHOW_WEEKDAY</li>
     *   <li>FORMAT_SHOW_YEAR</li>
     *   <li>FORMAT_NO_YEAR</li>
     *   <li>FORMAT_SHOW_DATE</li>
     *   <li>FORMAT_NO_MONTH_DAY</li>
     *   <li>FORMAT_12HOUR</li>
     *   <li>FORMAT_24HOUR</li>
     *   <li>FORMAT_CAP_AMPM</li>
     *   <li>FORMAT_NO_NOON</li>
     *   <li>FORMAT_CAP_NOON</li>
     *   <li>FORMAT_NO_MIDNIGHT</li>
     *   <li>FORMAT_CAP_MIDNIGHT</li>
     *   <li>FORMAT_UTC</li>
     *   <li>FORMAT_ABBREV_TIME</li>
     *   <li>FORMAT_ABBREV_WEEKDAY</li>
     *   <li>FORMAT_ABBREV_MONTH</li>
     *   <li>FORMAT_ABBREV_ALL</li>
     *   <li>FORMAT_NUMERIC_DATE</li>
     * </ul>
     *
     * <p>
     * If FORMAT_SHOW_TIME is set, the time is shown as part of the date range.
     * If the start and end time are the same, then just the start time is
     * shown.
     *
     * <p>
     * If FORMAT_SHOW_WEEKDAY is set, then the weekday is shown.
     *
     * <p>
     * If FORMAT_SHOW_YEAR is set, then the year is always shown.
     * If FORMAT_NO_YEAR is set, then the year is not shown.
     * If neither FORMAT_SHOW_YEAR nor FORMAT_NO_YEAR are set, then the year
     * is shown only if it is different from the current year, or if the start
     * and end dates fall on different years.  If both are set,
     * FORMAT_SHOW_YEAR takes precedence.
     *
     * <p>
     * Normally the date is shown unless the start and end day are the same.
     * If FORMAT_SHOW_DATE is set, then the date is always shown, even for
     * same day ranges.
     *
     * <p>
     * If FORMAT_NO_MONTH_DAY is set, then if the date is shown, just the
     * month name will be shown, not the day of the month.  For example,
     * "January, 2008" instead of "January 6 - 12, 2008".
     *
     * <p>
     * If FORMAT_CAP_AMPM is set and 12-hour time is used, then the "AM"
     * and "PM" are capitalized.  You should not use this flag
     * because in some locales these terms cannot be capitalized, and in
     * many others it doesn't make sense to do so even though it is possible.
     *
     * <p>
     * If FORMAT_NO_NOON is set and 12-hour time is used, then "12pm" is
     * shown instead of "noon".
     *
     * <p>
     * If FORMAT_CAP_NOON is set and 12-hour time is used, then "Noon" is
     * shown instead of "noon".  You should probably not use this flag
     * because in many locales it will not make sense to capitalize
     * the term.
     *
     * <p>
     * If FORMAT_NO_MIDNIGHT is set and 12-hour time is used, then "12am" is
     * shown instead of "midnight".
     *
     * <p>
     * If FORMAT_CAP_MIDNIGHT is set and 12-hour time is used, then "Midnight"
     * is shown instead of "midnight".  You should probably not use this
     * flag because in many locales it will not make sense to capitalize
     * the term.
     *
     * <p>
     * If FORMAT_12HOUR is set and the time is shown, then the time is
     * shown in the 12-hour time format. You should not normally set this.
     * Instead, let the time format be chosen automatically according to the
     * system settings. If both FORMAT_12HOUR and FORMAT_24HOUR are set, then
     * FORMAT_24HOUR takes precedence.
     *
     * <p>
     * If FORMAT_24HOUR is set and the time is shown, then the time is
     * shown in the 24-hour time format. You should not normally set this.
     * Instead, let the time format be chosen automatically according to the
     * system settings. If both FORMAT_12HOUR and FORMAT_24HOUR are set, then
     * FORMAT_24HOUR takes precedence.
     *
     * <p>
     * If FORMAT_UTC is set, then the UTC time zone is used for the start
     * and end milliseconds unless a time zone is specified. If a time zone
     * is specified it will be used regardless of the FORMAT_UTC flag.
     *
     * <p>
     * If FORMAT_ABBREV_TIME is set and 12-hour time format is used, then the
     * start and end times (if shown) are abbreviated by not showing the minutes
     * if they are zero.  For example, instead of "3:00pm" the time would be
     * abbreviated to "3pm".
     *
     * <p>
     * If FORMAT_ABBREV_WEEKDAY is set, then the weekday (if shown) is
     * abbreviated to a 3-letter string.
     *
     * <p>
     * If FORMAT_ABBREV_MONTH is set, then the month (if shown) is abbreviated
     * to a 3-letter string.
     *
     * <p>
     * If FORMAT_ABBREV_ALL is set, then the weekday and the month (if shown)
     * are abbreviated to 3-letter strings.
     *
     * <p>
     * If FORMAT_NUMERIC_DATE is set, then the date is shown in numeric format
     * instead of using the name of the month.  For example, "12/31/2008"
     * instead of "December 31, 2008".
     *
     * <p>
     * If the end date ends at 12:00am at the beginning of a day, it is
     * formatted as the end of the previous day in two scenarios:
     * <ul>
     *   <li>For single day events. This results in "8pm - midnight" instead of
     *       "Nov 10, 8pm - Nov 11, 12am".</li>
     *   <li>When the time is not displayed. This results in "Nov 10 - 11" for
     *       an event with a start date of Nov 10 and an end date of Nov 12 at
     *       00:00.</li>
     * </ul>
     *
     * @param context the context is required only if the time is shown
     * @param formatter the Formatter used for formatting the date range.
     * Note: be sure to call setLength(0) on StringBuilder passed to
     * the Formatter constructor unless you want the results to accumulate.
     * @param startMillis the start time in UTC milliseconds
     * @param endMillis the end time in UTC milliseconds
     * @param flags a bit mask of options
     * @param timeZone the time zone to compute the string in. Use null for local
     * or if the FORMAT_UTC flag is being used.
     *
     * @return the formatter with the formatted date/time range appended to the string buffer.
     */
    static CARAPI_(AutoPtr<Elastos::Utility::IFormatter>) FormatDateRange(
        /* [in] */ IContext* context,
        /* [in] */ Elastos::Utility::IFormatter* formatter,
        /* [in] */ Int64 startMillis,
        /* [in] */ Int64 endMillis,
        /* [in] */ Int32 flags,
        /* [in] */ const String& timeZone);

    /**
     * Formats a date or a time according to the local conventions. There are
     * lots of options that allow the caller to control, for example, if the
     * time is shown, if the day of the week is shown, if the month name is
     * abbreviated, if noon is shown instead of 12pm, and so on. For the
     * complete list of options, see the documentation for
     * {@link #formatDateRange}.
     * <p>
     * Example output strings (date formats in these examples are shown using
     * the US date format convention but that may change depending on the
     * local settings):
     * <ul>
     *   <li>10:15am</li>
     *   <li>3:00pm</li>
     *   <li>3pm</li>
     *   <li>3PM</li>
     *   <li>08:00</li>
     *   <li>17:00</li>
     *   <li>noon</li>
     *   <li>Noon</li>
     *   <li>midnight</li>
     *   <li>Midnight</li>
     *   <li>Oct 31</li>
     *   <li>Oct 31, 2007</li>
     *   <li>October 31, 2007</li>
     *   <li>10am, Oct 31</li>
     *   <li>17:00, Oct 31</li>
     *   <li>Wed</li>
     *   <li>Wednesday</li>
     *   <li>10am, Wed, Oct 31</li>
     *   <li>Wed, Oct 31</li>
     *   <li>Wednesday, Oct 31</li>
     *   <li>Wed, Oct 31, 2007</li>
     *   <li>Wed, October 31</li>
     *   <li>10/31/2007</li>
     * </ul>
     *
     * @param context the context is required only if the time is shown
     * @param millis a point in time in UTC milliseconds
     * @param flags a bit mask of formatting options
     * @return a string containing the formatted date/time.
     */
    static CARAPI_(String) FormatDateTime(
        /* [in] */ IContext* context,
        /* [in] */ Int64 millis,
        /* [in] */ Int32 flags);

    /**
     * @return a relative time string to display the time expressed by millis.  Times
     * are counted starting at midnight, which means that assuming that the current
     * time is March 31st, 0:30:
     * <ul>
     *   <li>"millis=0:10 today" will be displayed as "0:10"</li>
     *   <li>"millis=11:30pm the day before" will be displayed as "Mar 30"</li>
     * </ul>
     * If the given millis is in a different year, then the full date is
     * returned in numeric format (e.g., "10/12/2008").
     *
     * @param withPreposition If true, the string returned will include the correct
     * preposition ("at 9:20am", "on 10/12/2008" or "on May 29").
     */
    static CARAPI_(AutoPtr<ICharSequence>) GetRelativeTimeSpanString(
        /* [in] */ IContext* c,
        /* [in] */ Int64 millis,
        /* [in] */ Boolean withPreposition);

    /**
     * Convenience function to return relative time string without preposition.
     * @param c context for resources
     * @param millis time in milliseconds
     * @return {@link CharSequence} containing relative time.
     * @see #getRelativeTimeSpanString(Context, long, boolean)
     */
    static CARAPI_(AutoPtr<ICharSequence>) GetRelativeTimeSpanString(
        /* [in] */ IContext* c,
        /* [in] */ Int64 millis);

    static CARAPI_(AutoPtr<ICharSequence>) FormatDuration(
        /* [in] */ Int64 millis);

private:
    ECO_LOCAL DateUtils();
    ECO_LOCAL DateUtils(const DateUtils&);

    /**
     * Returns a string describing a day relative to the current day. For example if the day is
     * today this function returns "Today", if the day was a week ago it returns "7 days ago", and
     * if the day is in 2 weeks it returns "in 14 days".
     *
     * @param r the resources to get the strings from
     * @param day the relative day to describe in UTC milliseconds
     * @param today the current time in UTC milliseconds
     * @return a formatting string
     */
    ECO_LOCAL static /*final*/ CARAPI_(String) GetRelativeDayString(
        /* [in] */ IResources* r,
        /* [in] */ Int64 day,
        /* [in] */ Int64 today);

    ECO_LOCAL static CARAPI InitFormatStrings();

    ECO_LOCAL static CARAPI InitFormatStringsLocked();

    ECO_LOCAL static CARAPI Append(
        /* [in,out] */ StringBuilder* sb,
        /* [in] */ Int64 value,
        /* [in] */  Boolean pad,
        /* [in] */  Char32 zeroDigit);

public:
    // This table is used to lookup the resource string id of a format string
    // used for formatting a start and end date that fall in the same year.
    // The index is constructed from a bit-wise OR of the boolean values:
    // {showTime, showYear, showWeekDay}.  For example, if showYear and
    // showWeekDay are both true, then the index would be 3.
    /** @deprecated do not use. */
    static const AutoPtr<ArrayOf<Int32> > sSameYearTable;

    // This table is used to lookup the resource string id of a format string
    // used for formatting a start and end date that fall in the same month.
    // The index is constructed from a bit-wise OR of the boolean values:
    // {showTime, showYear, showWeekDay}.  For example, if showYear and
    // showWeekDay are both true, then the index would be 3.
    /** @deprecated do not use. */
    static const AutoPtr<ArrayOf<Int32> > sSameMonthTable;

private:
    static Object sLock;
    static AutoPtr<IConfiguration> sLastConfig;
    static String sElapsedFormatMMSS;
    static String sElapsedFormatHMMSS;

    static AutoPtr<ITime> sNowTime;
    static AutoPtr<ITime> sThenTime;

    static Object sLockDateUtilsClass;
};

} // namespace Format
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_FORMAT_DATEUTILS_H__
