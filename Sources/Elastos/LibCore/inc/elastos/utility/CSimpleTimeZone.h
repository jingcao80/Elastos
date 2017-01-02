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

#ifndef __ELASTOS_UTILITY_CSIMPLETIMEZONE_H__
#define __ELASTOS_UTILITY_CSIMPLETIMEZONE_H__

#include "_Elastos_Utility_CSimpleTimeZone.h"
#include "TimeZone.h"

namespace Elastos{
namespace Utility{

CarClass(CSimpleTimeZone)
    , public TimeZone
    , public ISimpleTimeZone
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSimpleTimeZone();

    /**
     * Constructs a {@code SimpleTimeZone} with the given base time zone offset from GMT
     * and time zone ID. Timezone IDs can be obtained from
     * {@code TimeZone.getAvailableIDs}. Normally you should use {@code TimeZone.getDefault} to
     * construct a {@code TimeZone}.
     *
     * @param offset
     *            the given base time zone offset to GMT.
     * @param name
     *            the time zone ID which is obtained from
     *            {@code TimeZone.getAvailableIDs}.
     */
    CARAPI constructor(
        /* [in] */ Int32 offset,
        /* [in] */ const String& name);

    /**
     * Constructs a {@code SimpleTimeZone} with the given base time zone offset from GMT,
     * time zone ID, and times to start and end the daylight savings time. Timezone IDs can
     * be obtained from {@code TimeZone.getAvailableIDs}. Normally you should use
     * {@code TimeZone.getDefault} to create a {@code TimeZone}. For a time zone that does not
     * use daylight saving time, do not use this constructor; instead you should
     * use {@code SimpleTimeZone(rawOffset, ID)}.
     * <p>
     * By default, this constructor specifies day-of-week-in-month rules. That
     * is, if the {@code startDay} is 1, and the {@code startDayOfWeek} is {@code SUNDAY}, then this
     * indicates the first Sunday in the {@code startMonth}. A {@code startDay} of -1 likewise
     * indicates the last Sunday. However, by using negative or zero values for
     * certain parameters, other types of rules can be specified.
     * <p>
     * Day of month: To specify an exact day of the month, such as March 1, set
     * {@code startDayOfWeek} to zero.
     * <p>
     * Day of week after day of month: To specify the first day of the week
     * occurring on or after an exact day of the month, make the day of the week
     * negative. For example, if {@code startDay} is 5 and {@code startDayOfWeek} is {@code -MONDAY},
     * this indicates the first Monday on or after the 5th day of the
     * {@code startMonth}.
     * <p>
     * Day of week before day of month: To specify the last day of the week
     * occurring on or before an exact day of the month, make the day of the
     * week and the day of the month negative. For example, if {@code startDay} is {@code -21}
     * and {@code startDayOfWeek} is {@code -WEDNESDAY}, this indicates the last Wednesday on or
     * before the 21st of the {@code startMonth}.
     * <p>
     * The above examples refer to the {@code startMonth}, {@code startDay}, and {@code startDayOfWeek};
     * the same applies for the {@code endMonth}, {@code endDay}, and {@code endDayOfWeek}.
     * <p>
     * The daylight savings time difference is set to the default value: one hour.
     *
     * @param offset
     *            the given base time zone offset to GMT.
     * @param name
     *            the time zone ID which is obtained from
     *            {@code TimeZone.getAvailableIDs}.
     * @param startMonth
     *            the daylight savings starting month. The month indexing is 0-based. eg, 0
     *            for January.
     * @param startDay
     *            the daylight savings starting day-of-week-in-month. Please see
     *            the member description for an example.
     * @param startDayOfWeek
     *            the daylight savings starting day-of-week. Please see the
     *            member description for an example.
     * @param startTime
     *            the daylight savings starting time in local wall time, which
     *            is standard time in this case. Please see the member
     *            description for an example.
     * @param endMonth
     *            the daylight savings ending month. The month indexing is 0-based. eg, 0 for
     *            January.
     * @param endDay
     *            the daylight savings ending day-of-week-in-month. Please see
     *            the member description for an example.
     * @param endDayOfWeek
     *            the daylight savings ending day-of-week. Please see the member
     *            description for an example.
     * @param endTime
     *            the daylight savings ending time in local wall time, which is
     *            daylight time in this case. Please see the member description
     *            for an example.
     * @throws IllegalArgumentException
     *             if the month, day, dayOfWeek, or time parameters are out of
     *             range for the start or end rule.
     */
    CARAPI constructor(
        /* [in] */ Int32 offset,
        /* [in] */ const String& name,
        /* [in] */ Int32 startMonth,
        /* [in] */ Int32 startDay,
        /* [in] */ Int32 startDayOfWeek,
        /* [in] */ Int32 startTime,
        /* [in] */ Int32 endMonth,
        /* [in] */ Int32 endDay,
        /* [in] */ Int32 endDayOfWeek,
        /* [in] */ Int32 endTime);

    /**
     * Constructs a {@code SimpleTimeZone} with the given base time zone offset from GMT,
     * time zone ID, times to start and end the daylight savings time, and
     * the daylight savings time difference in milliseconds.
     *
     * @param offset
     *            the given base time zone offset to GMT.
     * @param name
     *            the time zone ID which is obtained from
     *            {@code TimeZone.getAvailableIDs}.
     * @param startMonth
     *            the daylight savings starting month. Month is 0-based. eg, 0
     *            for January.
     * @param startDay
     *            the daylight savings starting day-of-week-in-month. Please see
     *            the description of {@link #SimpleTimeZone(int, String, int, int, int, int, int, int, int, int)} for an example.
     * @param startDayOfWeek
     *            the daylight savings starting day-of-week. Please see the
     *            description of {@link #SimpleTimeZone(int, String, int, int, int, int, int, int, int, int)} for an example.
     * @param startTime
     *            The daylight savings starting time in local wall time, which
     *            is standard time in this case. Please see the description of
     *            {@link #SimpleTimeZone(int, String, int, int, int, int, int, int, int, int)} for an example.
     * @param endMonth
     *            the daylight savings ending month. Month is 0-based. eg, 0 for
     *            January.
     * @param endDay
     *            the daylight savings ending day-of-week-in-month. Please see
     *            the description of {@link #SimpleTimeZone(int, String, int, int, int, int, int, int, int, int)} for an example.
     * @param endDayOfWeek
     *            the daylight savings ending day-of-week. Please see the description of
     *            {@link #SimpleTimeZone(int, String, int, int, int, int, int, int, int, int)} for an example.
     * @param endTime
     *            the daylight savings ending time in local wall time, which is
     *            daylight time in this case. Please see the description of {@link #SimpleTimeZone(int, String, int, int, int, int, int, int, int, int)}
     *            for an example.
     * @param daylightSavings
     *            the daylight savings time difference in milliseconds.
     * @throws IllegalArgumentException
     *                if the month, day, dayOfWeek, or time parameters are out of
     *                range for the start or end rule.
     */
    CARAPI constructor(
        /* [in] */ Int32 offset,
        /* [in] */ const String& name,
        /* [in] */ Int32 startMonth,
        /* [in] */ Int32 startDay,
        /* [in] */ Int32 startDayOfWeek,
        /* [in] */ Int32 startTime,
        /* [in] */ Int32 endMonth,
        /* [in] */ Int32 endDay,
        /* [in] */ Int32 endDayOfWeek,
        /* [in] */ Int32 endTime,
        /* [in] */ Int32 daylightSavings);

    /**
     * Construct a {@code SimpleTimeZone} with the given base time zone offset from GMT,
     * time zone ID, times to start and end the daylight savings time including a
     * mode specifier, the daylight savings time difference in milliseconds.
     * The mode specifies either {@link #WALL_TIME}, {@link #STANDARD_TIME}, or
     * {@link #UTC_TIME}.
     *
     * @param offset
     *            the given base time zone offset to GMT.
     * @param name
     *            the time zone ID which is obtained from
     *            {@code TimeZone.getAvailableIDs}.
     * @param startMonth
     *            the daylight savings starting month. The month indexing is 0-based. eg, 0
     *            for January.
     * @param startDay
     *            the daylight savings starting day-of-week-in-month. Please see
     *            the description of {@link #SimpleTimeZone(int, String, int, int, int, int, int, int, int, int)} for an example.
     * @param startDayOfWeek
     *            the daylight savings starting day-of-week. Please see the
     *            description of {@link #SimpleTimeZone(int, String, int, int, int, int, int, int, int, int)} for an example.
     * @param startTime
     *            the time of day in milliseconds on which daylight savings
     *            time starts, based on the {@code startTimeMode}.
     * @param startTimeMode
     *            the mode (UTC, standard, or wall time) of the start time
     *            value.
     * @param endDay
     *            the day of the week on which daylight savings time ends.
     * @param endMonth
     *            the daylight savings ending month. The month indexing is 0-based. eg, 0 for
     *            January.
     * @param endDayOfWeek
     *            the daylight savings ending day-of-week. Please see the description of
     *            {@link #SimpleTimeZone(int, String, int, int, int, int, int, int, int, int)} for an example.
     * @param endTime
     *            the time of day in milliseconds on which daylight savings
     *            time ends, based on the {@code endTimeMode}.
     * @param endTimeMode
     *            the mode (UTC, standard, or wall time) of the end time value.
     * @param daylightSavings
     *            the daylight savings time difference in milliseconds.
     * @throws IllegalArgumentException
     *             if the month, day, dayOfWeek, or time parameters are out of
     *             range for the start or end rule.
     */
    CARAPI constructor(
        /* [in] */ Int32 offset,
        /* [in] */ const String& name,
        /* [in] */ Int32 startMonth,
        /* [in] */ Int32 startDay,
        /* [in] */ Int32 startDayOfWeek,
        /* [in] */ Int32 startTime,
        /* [in] */ Int32 startTimeMode,
        /* [in] */ Int32 endMonth,
        /* [in] */ Int32 endDay,
        /* [in] */ Int32 endDayOfWeek,
        /* [in] */ Int32 endTime,
        /* [in] */ Int32 endTimeMode,
        /* [in] */ Int32 daylightSavings);

    /**
     * Sets the daylight savings offset in milliseconds for this {@code SimpleTimeZone}.
     *
     * @param milliseconds
     *            the daylight savings offset in milliseconds.
     */
    CARAPI SetDSTSavings(
        /* [in] */ Int32 milliseconds);

    /**
     * Sets the rule which specifies the end of daylight savings time.
     *
     * @param month
     *            the {@code Calendar} month in which daylight savings time ends.
     * @param dayOfMonth
     *            the {@code Calendar} day of the month on which daylight savings time
     *            ends.
     * @param time
     *            the time of day in milliseconds standard time on which
     *            daylight savings time ends.
     */
    CARAPI SetEndRule(
        /* [in] */ Int32 month,
        /* [in] */ Int32 dayOfMonth,
        /* [in] */ Int32 time);

    /**
     * Sets the rule which specifies the end of daylight savings time.
     *
     * @param month
     *            the {@code Calendar} month in which daylight savings time ends.
     * @param day
     *            the occurrence of the day of the week on which daylight
     *            savings time ends.
     * @param dayOfWeek
     *            the {@code Calendar} day of the week on which daylight savings time
     *            ends.
     * @param time
     *            the time of day in milliseconds standard time on which
     *            daylight savings time ends.
     */
    CARAPI SetEndRule(
        /* [in] */ Int32 month,
        /* [in] */ Int32 day,
        /* [in] */ Int32 dayOfWeek,
        /* [in] */ Int32 time);

    /**
     * Sets the rule which specifies the end of daylight savings time.
     *
     * @param month
     *            the {@code Calendar} month in which daylight savings time ends.
     * @param day
     *            the {@code Calendar} day of the month.
     * @param dayOfWeek
     *            the {@code Calendar} day of the week on which daylight savings time
     *            ends.
     * @param time
     *            the time of day in milliseconds on which daylight savings time
     *            ends.
     * @param after
     *            selects the day after or before the day of month.
     */
    CARAPI SetEndRule(
        /* [in] */ Int32 month,
        /* [in] */ Int32 day,
        /* [in] */ Int32 dayOfWeek,
        /* [in] */ Int32 time,
        /* [in] */ Boolean after);

    /**
     * Sets the rule which specifies the start of daylight savings time.
     *
     * @param month
     *            the {@code Calendar} month in which daylight savings time starts.
     * @param dayOfMonth
     *            the {@code Calendar} day of the month on which daylight savings time
     *            starts.
     * @param time
     *            the time of day in milliseconds on which daylight savings time
     *            starts.
     */
    CARAPI SetStartRule(
        /* [in] */ Int32 month,
        /* [in] */ Int32 dayOfMonth,
        /* [in] */ Int32 time);

    /**
     * Sets the rule which specifies the start of daylight savings time.
     *
     * @param month
     *            the {@code Calendar} month in which daylight savings time starts.
     * @param day
     *            the occurrence of the day of the week on which daylight
     *            savings time starts.
     * @param dayOfWeek
     *            the {@code Calendar} day of the week on which daylight savings time
     *            starts.
     * @param time
     *            the time of day in milliseconds on which daylight savings time
     *            starts.
     */
    CARAPI SetStartRule(
        /* [in] */ Int32 month,
        /* [in] */ Int32 day,
        /* [in] */ Int32 dayOfWeek,
        /* [in] */ Int32 time);

    /**
     * Sets the rule which specifies the start of daylight savings time.
     *
     * @param month
     *            the {@code Calendar} month in which daylight savings time starts.
     * @param day
     *            the {@code Calendar} day of the month.
     * @param dayOfWeek
     *            the {@code Calendar} day of the week on which daylight savings time
     *            starts.
     * @param time
     *            the time of day in milliseconds on which daylight savings time
     *            starts.
     * @param after
     *            selects the day after or before the day of month.
     */
    CARAPI SetStartRule(
        /* [in] */ Int32 month,
        /* [in] */ Int32 day,
        /* [in] */ Int32 dayOfWeek,
        /* [in] */ Int32 time,
        /* [in] */ Boolean after);

    /**
     * Sets the starting year for daylight savings time in this {@code SimpleTimeZone}.
     * Years before this start year will always be in standard time.
     *
     * @param year
     *            the starting year.
     */
    CARAPI SetStartYear(
        /* [in] */ Int32 year);

    CARAPI Clone(
            /* [out] */ ITimeZone** newObj);

    CARAPI Clone(
            /* [out] */ IInterface** newObj);

    CARAPI CloneImpl(
            /* [in] */ ISimpleTimeZone* newObj);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* isEqual);

    CARAPI GetDSTSavings(
        /* [out] */ Int32* pSavings);

    CARAPI GetOffset(
        /* [in] */ Int64 time,
        /* [out] */ Int32* pOffset);

    CARAPI GetOffset(
        /* [in] */ Int32 era,
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day,
        /* [in] */ Int32 dayOfWeek,
        /* [in] */ Int32 time,
        /* [out] */ Int32* pOffset);

    CARAPI HasSameRules(
        /* [in] */ ITimeZone* pTimeZone,
        /* [out] */ Boolean* pHasRules);

    CARAPI InDaylightTime(
        /* [in] */ IDate * pTime,
        /* [out] */ Boolean * pIsIn);

    CARAPI SetRawOffset(
        /* [in] */ Int32 offsetMillis);

    CARAPI GetRawOffset(
        /* [out] */ Int32 * pRawOffset);

    CARAPI UseDaylightTime(
        /* [out] */ Boolean * pIsUsed);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI CheckRange(
        /* [in] */ Int32 month,
        /* [in] */ Int32 dayOfWeek,
        /* [in] */ Int32 time);

    CARAPI CheckDay(
        /* [in] */ Int32 month,
        /* [in] */ Int32 day);

    Boolean IsLeapYear(
        /* [in] */ Int32 year);

    Int32 Mod7(
        /* [in] */ Int32 num1);

    CARAPI SetStartMode();

    CARAPI SetEndMode();

private:
    static const Int32 DOM_MODE = 1;
    static const Int32 DOW_IN_MONTH_MODE = 2;
    static const Int32 DOW_GE_DOM_MODE = 3;
    static const Int32 DOW_LE_DOM_MODE = 4;

private:
    Int32 mRawOffset;

    Int32 mStartYear;
    Int32 mStartMonth;
    Int32 mStartDay;
    Int32 mStartDayOfWeek;
    Int32 mStartTime;

    Int32 mEndMonth;
    Int32 mEndDay;
    Int32 mEndDayOfWeek;
    Int32 mEndTime;

    Int32 mStartMode;
    Int32 mEndMode;

    Int32 mDstSavings;
    Boolean mUseDaylight;
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_CSIMPLETIMEZONE_H__
