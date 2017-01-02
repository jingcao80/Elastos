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

#ifndef __ELASTOS_DROID_TEXT_FORMAT_TIME_H__
#define __ELASTOS_DROID_TEXT_FORMAT_TIME_H__

#include <elastos/core/Object.h>

using Elastos::Droid::Text::Format::ITime;
using Elastos::Utility::ILocale;
using Libcore::Utility::IZoneInfo;
using Libcore::Utility::IZoneInfoWallTime;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Format {

/**
 * The Time class is a faster replacement for the java.util.Calendar and
 * java.util.GregorianCalendar classes. An instance of the Time class represents
 * a moment in time, specified with second precision. It is modelled after
 * struct tm, and in fact, uses struct tm to implement most of the
 * functionality.
 */
class Time
    : public Object
    , public ITime
{
private:
    class TimeCalculator
        : public Object
    {
    public:
        TimeCalculator(const String& timezoneId);

        CARAPI_(Int64) ToMillis(
            /* [in] */ Boolean ignoreDst);

        CARAPI SetTimeInMillis(
            /* [in] */ Int64 millis);

        CARAPI_(String) Format(
            /* [in] */ String& format);

        CARAPI SwitchTimezone(
            /* [in] */ const String& timezone);

        CARAPI_(String) Format2445(
            /* [in] */ Boolean hasTime);

        CARAPI_(String) ToStringInternal();

        static CARAPI_(Int32) Compare(
            /* [in] */ TimeCalculator* aObject,
            /* [in] */ TimeCalculator* bObject);

        CARAPI CopyFieldsToTime(
            /* [in] */ ITime* time);

        CARAPI CopyFieldsFromTime(
            /* [in] */ ITime* time);

    private:
        CARAPI UpdateZoneInfoFromTimeZone();

        CARAPI LookupZoneInfo(
            /* [in] */ const String& timezoneId,
            /* [out] */ IZoneInfo** ret);

        CARAPI_(Char32) ToChar(
            /* [in] */ Int32 n);

    public:
        const AutoPtr<IZoneInfoWallTime> mWallTime;
        String mTimeZone;

    private:
        // Information about the current timezone.
        AutoPtr<IZoneInfo> mZoneInfo;
    };
public:
    CAR_INTERFACE_DECL()

    Time();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& timezone);

    CARAPI constructor(
        /* [in] */ ITime* other);

    /**
     * Ensures the values in each field are in range. For example if the
     * current value of this calendar is March 32, normalize() will convert it
     * to April 1. It also fills in weekDay, yearDay, isDst and gmtoff.
     *
     * <p>
     * If "ignoreDst" is true, then this method sets the "isDst" field to -1
     * (the "unknown" value) before normalizing.  It then computes the
     * correct value for "isDst".
     *
     * <p>
     * See {@link #toMillis(Boolean)} for more information about when to
     * use <tt>true</tt> or <tt>false</tt> for "ignoreDst".
     *
     * @return the UTC milliseconds since the epoch
     */
    CARAPI Normalize(
        /* [in] */ Boolean ignoreDst,
        /* [out] */ Int64* ret);

    /**
     * Convert this time object so the time represented remains the same, but is
     * instead located in a different timezone. This method automatically calls
     * normalize() in some cases
     */
    CARAPI SwitchTimezone(
        /* [in] */ const String& timezone);

    /**
     * Return the maximum possible value for the given field given the value of
     * the other fields. Requires that it be normalized for MONTH_DAY and
     * YEAR_DAY.
     * @param field one of the constants for HOUR, MINUTE, SECOND, etc.
     * @return the maximum value for the field.
     */
    CARAPI GetActualMaximum(
        /* [in] */ Int32 field,
        /* [out] */ Int32* ret);

    /**
     * Clears all values, setting the timezone to the given timezone. Sets isDst
     * to a negative value to mean "unknown".
     * @param timezone the timezone to use.
     */
    CARAPI Clear(
        /* [in] */ const String& timezone);

    /**
     * return a negative number if a is less than b, a positive number if a is
     * greater than b, and 0 if they are equal.
     */
    static CARAPI Compare(
        /* [in] */ ITime* a,
        /* [in] */ ITime* b,
        /* [out] */ Int32* result);

    /**
     * Print the current value given the format string provided. See man
     * strftime for what means what. The final string must be less than 256
     * characters.
     * @param format a string containing the desired format.
     * @return a String containing the current time expressed in the current locale.
     */
    CARAPI Format(
        /* [in] */ const String& format,
        /* [out] */ String* ret);

    /**
     * Return the current time in YYYYMMDDTHHMMSS<tz> format
     */
    CARAPI ToString(
        /* [out] */ String* ret);

    /**
     * Parses a date-time string in either the RFC 2445 format or an abbreviated
     * format that does not include the "time" field.  For example, all of the
     * following strings are valid:
     *
     * <ul>
     *   <li>"20081013T160000Z"</li>
     *   <li>"20081013T160000"</li>
     *   <li>"20081013"</li>
     * </ul>
     *
     * Returns whether or not the time is in UTC (ends with Z).  If the string
     * ends with "Z" then the timezone is set to UTC.  If the date-time string
     * included only a date and no time field, then the <code>allDay</code>
     * field of this Time class is set to true and the <code>hour</code>,
     * <code>minute</code>, and <code>second</code> fields are set to zero;
     * otherwise (a time field was included in the date-time string)
     * <code>allDay</code> is set to false. The fields <code>weekDay</code>,
     * <code>yearDay</code>, and <code>gmtoff</code> are always set to zero,
     * and the field <code>isDst</code> is set to -1 (unknown).  To set those
     * fields, call {@link #normalize(Boolean)} after parsing.
     *
     * To parse a date-time string and convert it to UTC milliseconds, do
     * something like this:
     *
     * <pre>
     *   Time time = new Time();
     *   String date = "20081013T160000Z";
     *   time.parse(date);
     *   Int64 millis = time.normalize(false);
     * </pre>
     *
     * @param s the string to parse
     * @return true if the resulting time value is in UTC time
     * @throws android.util.TimeFormatException if s cannot be parsed.
     */
    CARAPI Parse(
        /* [in] */ const String& s,
        /* [out] */ Boolean* ret);

    /**
     * Parse a time in RFC 3339 format.  This method also parses simple dates
     * (that is, strings that contain no time or time offset).  For example,
     * all of the following strings are valid:
     *
     * <ul>
     *   <li>"2008-10-13T16:00:00.000Z"</li>
     *   <li>"2008-10-13T16:00:00.000+07:00"</li>
     *   <li>"2008-10-13T16:00:00.000-07:00"</li>
     *   <li>"2008-10-13"</li>
     * </ul>
     *
     * <p>
     * If the string contains a time and time offset, then the time offset will
     * be used to convert the time value to UTC.
     * </p>
     *
     * <p>
     * If the given string contains just a date (with no time field), then
     * the {@link #allDay} field is set to true and the {@link #hour},
     * {@link #minute}, and  {@link #second} fields are set to zero.
     * </p>
     *
     * <p>
     * Returns true if the resulting time value is in UTC time.
     * </p>
     *
     * @param s the string to parse
     * @return true if the resulting time value is in UTC time
     * @throws android.util.TimeFormatException if s cannot be parsed.
     */
    CARAPI Parse3339(
        /* [in] */ const String& s,
        /* [out] */ Boolean* ret);

    static CARAPI_(String) GetCurrentTimezone();

    /**
     * Sets the time of the given Time object to the current time.
     */
    CARAPI SetToNow();

    /**
     * Converts this time to milliseconds. Suitable for interacting with the
     * standard java libraries. The time is in UTC milliseconds since the epoch.
     * This does an implicit normalization to compute the milliseconds but does
     * <em>not</em> change any of the fields in this Time object.  If you want
     * to normalize the fields in this Time object and also get the milliseconds
     * then use {@link #normalize(Boolean)}.
     *
     * <p>
     * If "ignoreDst" is false, then this method uses the current setting of the
     * "isDst" field and will adjust the returned time if the "isDst" field is
     * wrong for the given time.  See the sample code below for an example of
     * this.
     *
     * <p>
     * If "ignoreDst" is true, then this method ignores the current setting of
     * the "isDst" field in this Time object and will instead figure out the
     * correct value of "isDst" (as best it can) from the fields in this
     * Time object.  The only case where this method cannot figure out the
     * correct value of the "isDst" field is when the time is inherently
     * ambiguous because it falls in the hour that is repeated when switching
     * from Daylight-Saving Time to Standard Time.
     *
     * <p>
     * Here is an example where <tt>toMillis(true)</tt> adjusts the time,
     * assuming that DST changes at 2am on Sunday, Nov 4, 2007.
     *
     * <pre>
     * Time time = new Time();
     * time.set(2007, 10, 4);  // set the date to Nov 4, 2007, 12am
     * time.normalize();       // this sets isDst = 1
     * time.monthDay += 1;     // changes the date to Nov 5, 2007, 12am
     * millis = time.toMillis(false);   // millis is Nov 4, 2007, 11pm
     * millis = time.toMillis(true);    // millis is Nov 5, 2007, 12am
     * </pre>
     *
     * <p>
     * To avoid this problem, use <tt>toMillis(true)</tt>
     * after adding or subtracting days or explicitly setting the "monthDay"
     * field.  On the other hand, if you are adding
     * or subtracting hours or minutes, then you should use
     * <tt>toMillis(false)</tt>.
     *
     * <p>
     * You should also use <tt>toMillis(false)</tt> if you want
     * to read back the same milliseconds that you set with {@link #set(Int64)}
     * or {@link #set(Time)} or after parsing a date string.
     */
    CARAPI ToMillis(
        /* [in] */ Boolean ignoreDst,
        /* [out] */ Int64* ret);

    /**
     * Sets the fields in this Time object given the UTC milliseconds.  After
     * this method returns, all the fields are normalized.
     * This also sets the "isDst" field to the correct value.
     *
     * @param millis the time in UTC milliseconds since the epoch.
     */
    CARAPI Set(
        /* [in] */ Int64 millis);

    /**
     * Format according to RFC 2445 DATETIME type.
     *
     * <p>
     * The same as format("%Y%m%dT%H%M%S").
     */
    CARAPI Format2445(
        /* [out] */ String* ret);

    /**
     * Copy the value of that to this Time object. No normalization happens.
     */
    CARAPI Set(
        /* [in] */ ITime* that);

    /**
     * Sets the fields. Sets weekDay, yearDay and gmtoff to 0, and isDst to -1.
     * Call {@link #normalize(Boolean)} if you need those.
     */
    CARAPI Set(
        /* [in] */ Int32 second,
        /* [in] */ Int32 minute,
        /* [in] */ Int32 hour,
        /* [in] */ Int32 monthDay,
        /* [in] */ Int32 month,
        /* [in] */ Int32 year);

    /**
     * Sets the date from the given fields.  Also sets allDay to true.
     * Sets weekDay, yearDay and gmtoff to 0, and isDst to -1.
     * Call {@link #normalize(Boolean)} if you need those.
     *
     * @param monthDay the day of the month (in the range [1,31])
     * @param month the zero-based month number (in the range [0,11])
     * @param year the year
     */
    CARAPI Set(
        /* [in] */ Int32 monthDay,
        /* [in] */ Int32 month,
        /* [in] */ Int32 year);

    /**
     * Returns true if the time represented by this Time object occurs before
     * the given time.
     *
     * @param that a given Time object to compare against
     * @return true if this time is less than the given time
     */
    CARAPI Before(
        /* [in] */ ITime* that,
        /* [out] */ Boolean* ret);

    /**
     * Returns true if the time represented by this Time object occurs after
     * the given time.
     *
     * @param that a given Time object to compare against
     * @return true if this time is greater than the given time
     */
    CARAPI After(
        /* [in] */ ITime* that,
        /* [out] */ Boolean* ret);

    /**
     * Computes the week number according to ISO 8601.  The current Time
     * object must already be normalized because this method uses the
     * yearDay and weekDay fields.
     *
     * <p>
     * In IS0 8601, weeks start on Monday.
     * The first week of the year (week 1) is defined by ISO 8601 as the
     * first week with four or more of its days in the starting year.
     * Or equivalently, the week containing January 4.  Or equivalently,
     * the week with the year's first Thursday in it.
     * </p>
     *
     * <p>
     * The week number can be calculated by counting Thursdays.  Week N
     * contains the Nth Thursday of the year.
     * </p>
     *
     * @return the ISO week number.
     */
    CARAPI GetWeekNumber(
        /* [out] */ Int32* ret);

    /**
     * Return a string in the RFC 3339 format.
     * <p>
     * If allDay is true, expresses the time as Y-M-D</p>
     * <p>
     * Otherwise, if the timezone is UTC, expresses the time as Y-M-D-T-H-M-S UTC</p>
     * <p>
     * Otherwise the time is expressed the time as Y-M-D-T-H-M-S +- GMT</p>
     * @param allDay
     * @return string in the RFC 3339 format.
     */
    CARAPI Format3339(
        /* [in] */ Boolean allDay,
        /* [out] */ String* ret);

    /**
     * Returns true if the day of the given time is the epoch on the Julian Calendar
     * (January 1, 1970 on the Gregorian calendar).
     *
     * @param time the time to test
     * @return true if epoch.
     */
    static CARAPI_(Boolean) IsEpoch(
         /* [in] */ ITime* time);

    /**
     * Computes the Julian day number, given the UTC milliseconds
     * and the offset (in seconds) from UTC.  The Julian day for a given
     * date will be the same for every timezone.  For example, the Julian
     * day for July 1, 2008 is 2454649.  This is the same value no matter
     * what timezone is being used.  The Julian day is useful for testing
     * if two events occur on the same day and for determining the relative
     * time of an event from the present ("yesterday", "3 days ago", etc.).
     *
     * <p>
     * Use {@link #toMillis(Boolean)} to get the milliseconds.
     *
     * @param millis the time in UTC milliseconds
     * @param gmtoff the offset from UTC in seconds
     * @return the Julian day
     */
    static CARAPI_(Int32) GetJulianDay(
        /* [in] */ Int64 millis,
        /* [in] */ Int64 gmtoff);

    /**
     * <p>Sets the time from the given Julian day number, which must be based on
     * the same timezone that is set in this Time object.  The "gmtoff" field
     * need not be initialized because the given Julian day may have a different
     * GMT offset than whatever is currently stored in this Time object anyway.
     * After this method returns all the fields will be normalized and the time
     * will be set to 12am at the beginning of the given Julian day.
     * </p>
     *
     * <p>
     * The only exception to this is if 12am does not exist for that day because
     * of daylight saving time.  For example, Cairo, Eqypt moves time ahead one
     * hour at 12am on April 25, 2008 and there are a few other places that
     * also change daylight saving time at 12am.  In those cases, the time
     * will be set to 1am.
     * </p>
     *
     * @param julianDay the Julian day in the timezone for this Time object
     * @return the UTC milliseconds for the beginning of the Julian day
     */
    CARAPI SetJulianDay(
        /* [in] */ Int32 julianDay,
        /* [out] */ Int64* ret);

    /**
     * Returns the week since {@link #EPOCH_JULIAN_DAY} (Jan 1, 1970) adjusted
     * for first day of week. This takes a julian day and the week start day and
     * calculates which week since {@link #EPOCH_JULIAN_DAY} that day occurs in,
     * starting at 0. *Do not* use this to compute the ISO week number for the
     * year.
     *
     * @param julianDay The julian day to calculate the week number for
     * @param firstDayOfWeek Which week day is the first day of the week, see
     *            {@link #SUNDAY}
     * @return Weeks since the epoch
     */
    static CARAPI_(Int32) GetWeeksSinceEpochFromJulianDay(
        /* [in] */ Int32 julianDay,
        /* [in] */ Int32 firstDayOfWeek);

    /**
     * Takes a number of weeks since the epoch and calculates the Julian day of
     * the Monday for that week. This assumes that the week containing the
     * {@link #EPOCH_JULIAN_DAY} is considered week 0. It returns the Julian day
     * for the Monday week weeks after the Monday of the week containing the
     * epoch.
     *
     * @param week Number of weeks since the epoch
     * @return The julian day for the Monday of the given week since the epoch
     */
    static CARAPI_(Int32) GetJulianMondayFromWeeksSinceEpoch(
        /* [in] */ Int32 week);

    CARAPI SetAllDay(
        /* [in] */ Boolean bAllDay);

    CARAPI GetAllDay(
        /* [out] */ Boolean* bAllDay);

    CARAPI SetSecond(
        /* [in] */ Int32 second);

    CARAPI GetSecond(
        /* [out] */ Int32* second);

    CARAPI SetMinute(
        /* [in] */ Int32 minute);

    CARAPI GetMinute(
        /* [out] */ Int32* minute);

    CARAPI SetHour(
        /* [in] */ Int32 hour);

    CARAPI GetHour(
        /* [out] */ Int32* hour);

    CARAPI SetMonthDay(
        /* [in] */ Int32 monthDay);

    CARAPI GetMonthDay(
        /* [out] */ Int32* monthDay);

    CARAPI SetMonth(
        /* [in] */ Int32 month);

    CARAPI GetMonth(
        /* [out] */ Int32* month);

    CARAPI SetYear(
        /* [in] */ Int32 year);

    CARAPI GetYear(
        /* [out] */ Int32* year);

    CARAPI SetWeekDay(
        /* [in] */ Int32 weekDay);

    CARAPI GetWeekDay(
        /* [out] */ Int32* weekDay);

    CARAPI SetYearDay(
        /* [in] */ Int32 yearDay);

    CARAPI GetYearDay(
        /* [out] */ Int32* yearDay);

    CARAPI SetIsDst(
        /* [in] */ Int32 isDst);

    CARAPI GetIsDst(
        /* [out] */ Int32* isDst);

    CARAPI SetGmtoff(
        /* [in] */ Int64 gmtoff);

    CARAPI GetGmtoff(
        /* [out] */ Int64* gmtoff);

    CARAPI SetTimeZone(
        /* [in] */ const String& timezone);

    CARAPI GetTimeZone(
        /* [out] */ String* timezone);

private:
    CARAPI Initialize(
        /* [in] */ const String& timezoneId);

    /**
     * Parse a time in the current zone in YYYYMMDDTHHMMSS format.
     */
    CARAPI_(Boolean) ParseInternal(
        /* [in] */ const String& s);

    CARAPI CheckChar(
        /* [in] */ const String& s,
        /* [in] */ Int32 spos,
        /* [in] */ Char32 expected);

    static CARAPI_(Int32) GetChar(
        /* [in] */ const String& s,
        /* [in] */ Int32 spos,
        /* [in] */ Int32 mul);

    CARAPI_(Boolean) Parse3339Internal(
        /* [in] */ const String& s);

public:
    /**
     * True if this is an allDay event. The hour, minute, second fields are
     * all zero, and the date is displayed the same in all time zones.
     */
    Boolean mAllDay;

    /**
     * Seconds [0-61] (2 leap seconds allowed)
     */
    Int32 mSecond;

    /**
     * Minute [0-59]
     */
    Int32 mMinute;

    /**
     * Hour of day [0-23]
     */
    Int32 mHour;

    /**
     * Day of month [1-31]
     */
    Int32 mMonthDay;

    /**
     * Month [0-11]
     */
    Int32 mMonth;

    /**
     * Year. TBD. Is this years since 1900 like in struct tm?
     */
    Int32 mYear;

    /**
     * Day of week [0-6]
     */
    Int32 mWeekDay;

    /**
     * Day of year [0-365]
     */
    Int32 mYearDay;

    /**
     * This time is in daylight savings time. One of:
     * <ul>
     * <li><b>positive</b> - in dst</li>
     * <li><b>0</b> - not in dst</li>
     * <li><b>negative</b> - unknown</li>
     * </ul>
     */
    Int32 mIsDst;

    /**
     * Offset in seconds from UTC including any DST offset.
     */
    Int64 mGmtoff;

    /**
     * The timezone for this Time.  Should not be NULL.
     */
    String mTimezone;

public:
    static const Int32 SECOND;
    static const Int32 MINUTE;
    static const Int32 HOUR;
    static const Int32 MONTH_DAY;
    static const Int32 MONTH;
    static const Int32 YEAR;
    static const Int32 WEEK_DAY;
    static const Int32 YEAR_DAY;
    static const Int32 WEEK_NUM;

    static const Int32 SUNDAY;
    static const Int32 MONDAY;
    static const Int32 TUESDAY;
    static const Int32 WEDNESDAY;
    static const Int32 THURSDAY;
    static const Int32 FRIDAY;
    static const Int32 SATURDAY;

private:
    static const String Y_M_D_T_H_M_S_000;
    static const String Y_M_D_T_H_M_S_000_Z;
    static const String Y_M_D;

    AutoPtr<TimeCalculator> mCalculator;
    static const Int32 DAYS_PER_MONTH[];

    /**
     * This array is indexed by the weekDay field (SUNDAY=0, MONDAY=1, etc.)
     * and gives a number that can be added to the yearDay to give the
     * closest Thursday yearDay.
     */
    static const Int32 sThursdayOffset[];

    static Object sTimeClassLock;

};

} // namespace Format
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_FORMAT_TIME_H__
