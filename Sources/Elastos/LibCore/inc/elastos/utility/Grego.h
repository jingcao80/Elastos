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

#ifndef __ELASTOS_UTILITY_GREGO_H__
#define __ELASTOS_UTILITY_GREGO_H__

#include <elastos.h>

namespace Elastos {
namespace Utility {

// android-changed: com.ibm.icu.impl (ICU4J 4.2)

/**
 * A utility class providing proleptic Gregorian calendar functions
 * used by time zone and calendar code.  Do not instantiate.
 *
 * Note:  Unlike GregorianCalendar, all computations performed by this
 * class occur in the pure proleptic GregorianCalendar.
 */
// android-changed: public
class Grego
{
private:
    Grego();

public:
    /**
     * Return true if the given year is a leap year.
     * @param year Gregorian year, with 0 == 1 BCE, -1 == 2 BCE, etc.
     * @return true if the year is a leap year
     */
    static CARAPI_(Boolean) IsLeapYear(
        /* [in] */ Int32 year);

    /**
     * Return the number of days in the given month.
     * @param year Gregorian year, with 0 == 1 BCE, -1 == 2 BCE, etc.
     * @param month 0-based month, with 0==Jan
     * @return the number of days in the given month
     */
    static CARAPI_(Int32) MonthLength(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month);

    /**
     * Return the length of a previous month of the Gregorian calendar.
     * @param year Gregorian year, with 0 == 1 BCE, -1 =} // namespace Utility
} // namespace Elastos
     * = 2 BCE, etc.
     * @param month 0-based month, with 0==Jan
     * @return the number of days in the month previous to the given month
     */
    static CARAPI_(Int32) PreviousMonthLength(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month);

    /**
     * Convert a year, month, and day-of-month, given in the proleptic
     * Gregorian calendar, to 1970 epoch days.
     * @param year Gregorian year, with 0 == 1 BCE, -1 == 2 BCE, etc.
     * @param month 0-based month, with 0==Jan
     * @param dom 1-based day of month
     * @return the day number, with day 0 == Jan 1 1970
     */
    static CARAPI_(Int64) FieldsToDay(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 dom);

    /**
     * Return the day of week on the 1970-epoch day
     * @param day the 1970-epoch day (integral value)
     * @return the day of week
     */
    static CARAPI_(Int32) DayOfWeek(
        /* [in] */ Int64 day);

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) DayToFields(
        /* [in] */ Int64 day,
        /* [in] */ ArrayOf<Int32>* fields);

    /*
     * Convert long time to date/time fields
     *
     * result[0] : year
     * result[1] : month
     * result[2] : dayOfMonth
     * result[3] : dayOfWeek
     * result[4] : dayOfYear
     * result[5] : millisecond in day
     */
    static CARAPI_(AutoPtr<ArrayOf<Int32> >) TimeToFields(
        /* [in] */ Int64 time,
        /* [in] */ ArrayOf<Int32>* fields);

    static CARAPI_(Int64) FloorDivide(
        /* [in] */ Int64 numerator,
        /* [in] */ Int64 denominator);

    /*
     * Returns the ordinal number for the specified day of week in the month.
     * The valid return value is 1, 2, 3, 4 or -1.
     */
    static CARAPI_(Int32) GetDayOfWeekInMonth(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 dayOfMonth);

private:
    static CARAPI_(Int64) FloorDivide(
        /* [in] */ Int64 numerator,
        /* [in] */ Int64 denominator,
        /* [in] */ Int64* remainder);

public:
    // Max/min milliseconds
    static const Int64 MIN_MILLIS = -184303902528000000ll;
    static const Int64 MAX_MILLIS = 183882168921600000ll;

    static const Int32 MILLIS_PER_SECOND = 1000;
    static const Int32 MILLIS_PER_MINUTE = 60 * MILLIS_PER_SECOND;
    static const Int32 MILLIS_PER_HOUR = 60 * MILLIS_PER_MINUTE;
    static const Int32 MILLIS_PER_DAY = 24 * MILLIS_PER_HOUR;

private:
    //  January 1, 1 CE Gregorian
    static const Int32 JULIAN_1_CE = 1721426;
    //  January 1, 1970 CE Gregorian
    static const Int32 JULIAN_1970_CE = 2440588;

    static const Int32 MONTH_LENGTH[];

    static const Int32 DAYS_BEFORE[];
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_GREGO_H__
