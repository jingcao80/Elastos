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

#ifndef __ELASTOS_UTILITY_CGREGORIANCALENDER_H__
#define __ELASTOS_UTILITY_CGREGORIANCALENDER_H__

#include "_Elastos_Utility_CGregorianCalendar.h"
#include "Calendar.h"

namespace Elastos {
namespace Utility {

CarClass(CGregorianCalendar)
    , public Calendar
    , public IGregorianCalendar
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CGregorianCalendar();

    /**
     * Constructs a new {@code GregorianCalendar} initialized to the current date and
     * time with the default {@code Locale} and {@code TimeZone}.
     */
    CARAPI constructor();

    /**
     * Constructs a new {@code GregorianCalendar} initialized to midnight in the default
     * {@code TimeZone} and {@code Locale} on the specified date.
     *
     * @param year
     *            the year.
     * @param month
     *            the month.
     * @param day
     *            the day of the month.
     */
    CARAPI constructor(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day);

    /**
     * Constructs a new {@code GregorianCalendar} initialized to the specified date and
     * time in the default {@code TimeZone} and {@code Locale}.
     *
     * @param year
     *            the year.
     * @param month
     *            the month.
     * @param day
     *            the day of the month.
     * @param hour
     *            the hour.
     * @param minute
     *            the minute.
     */
    CARAPI constructor(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day,
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute);

    /**
     * Constructs a new {@code GregorianCalendar} initialized to the specified date and
     * time in the default {@code TimeZone} and {@code Locale}.
     *
     * @param year
     *            the year.
     * @param month
     *            the month.
     * @param day
     *            the day of the month.
     * @param hour
     *            the hour.
     * @param minute
     *            the minute.
     * @param second
     *            the second.
     */
    CARAPI constructor(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day,
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute,
        /* [in] */ Int32 second);

    CARAPI constructor(
        /* [in] */ Int64 milliseconds);

    /**
     * Constructs a new {@code GregorianCalendar} initialized to the current date and
     * time and using the specified {@code Locale} and the default {@code TimeZone}.
     *
     * @param locale
     *            the {@code Locale}.
     */
    CARAPI constructor(
        /* [in] */ ILocale* locale);

    /**
     * Constructs a new {@code GregorianCalendar} initialized to the current date and
     * time and using the specified {@code TimeZone} and the default {@code Locale}.
     *
     * @param timezone
     *            the {@code TimeZone}.
     */
    CARAPI constructor(
        /* [in] */ ITimeZone* ITimeZone);

    /**
     * Constructs a new {@code GregorianCalendar} initialized to the current date and
     * time and using the specified {@code TimeZone} and {@code Locale}.
     *
     * @param timezone
     *            the {@code TimeZone}.
     * @param locale
     *            the {@code Locale}.
     */
    CARAPI constructor(
        /* [in] */ ITimeZone* ITimeZone,
        /* [in] */ ILocale* locale);

    CARAPI constructor(
        /* [in] */ Boolean ignored);

    /**
     * Returns true if {@code year} is a leap year.
     */
    CARAPI IsLeapYear(
        /* [in] */ Int32 year,
        /* [out] */ Boolean* isLeap);

    /**
     * Sets the gregorian change date of this calendar.
     */
    CARAPI SetGregorianChange(
        /* [in] */ IDate* date);

    /**
     * Returns the gregorian change date of this calendar. This is the date on
     * which the gregorian calendar came into effect.
     *
     * @return a {@code Date} which represents the gregorian change date.
     */
    CARAPI GetGregorianChange(
        /* [out] */ IDate** date);

    /**
     * Adds the specified amount to a {@code Calendar} field.
     *
     * @param field
     *            the {@code Calendar} field to modify.
     * @param value
     *            the amount to add to the field.
     *
     * @throws IllegalArgumentException
     *                if the specified field is DST_OFFSET or ZONE_OFFSET.
     */
    //@Override
    CARAPI Add(
        /* [in] */ Int32 field,
        /* [in] */ Int32 value);

    /**
     * Compares the specified object to this {@code Calendar} and returns whether they are
     * equal. The object must be an instance of {@code Calendar} and have the same
     * properties.
     *
     * @param object
     *            the object to compare with this object.
     * @return {@code true} if the specified object is equal to this {@code Calendar}, {@code false}
     *         otherwise.
     */
    CARAPI Equals(
        /* [in] */ ICalendar* other,
        /* [out] */ Boolean* isEqual);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* isEqual);

    CARAPI GetActualMaximum(
        /* [in] */ Int32 field,
        /* [out] */ Int32* value);

    /**
     * Gets the minimum value of the specified field for the current date. For
     * the gregorian calendar, this value is the same as
     * {@code getMinimum()}.
     *
     * @param field
     *            the field.
     * @return the minimum value of the specified field.
     */
    //@Override
    CARAPI GetActualMinimum(
        /* [in] */ Int32 field,
        /* [out] */ Int32* value);

    /**
     * Gets the greatest minimum value of the specified field. For the gregorian
     * calendar, this value is the same as {@code getMinimum()}.
     *
     * @param field
     *            the field.
     * @return the greatest minimum value of the specified field.
     */
    //@Override
    CARAPI GetGreatestMinimum(
        /* [in] */ Int32 field,
        /* [out] */ Int32* minimum);

    /**
     * Gets the smallest maximum value of the specified field. For example, 28
     * for the day of month field.
     *
     * @param field
     *            the field.
     * @return the smallest maximum value of the specified field.
     */
    //@Override
    CARAPI GetLeastMaximum(
        /* [in] */ Int32 field,
        /* [out] */ Int32* maximum);

    /**
     * Gets the greatest maximum value of the specified field. For example, 31
     * for the day of month field.
     *
     * @param field
     *            the field.
     * @return the greatest maximum value of the specified field.
     */
    //@Override
    CARAPI GetMaximum(
        /* [in] */ Int32 field,
        /* [out] */ Int32* maximum);

    /**
     * Gets the smallest minimum value of the specified field.
     *
     * @param field
     *            the field.
     * @return the smallest minimum value of the specified field.
     */
    //@Override
    CARAPI GetMinimum(
        /* [in] */ Int32 field,
        /* [out] */ Int32* minimum);

    /**
     * Adds the specified amount the specified field and wraps the value of the
     * field when it goes beyond the maximum or minimum value for the current
     * date. Other fields will be adjusted as required to maintain a consistent
     * date.
     *
     * @param field
     *            the field to roll.
     * @param value
     *            the amount to add.
     *
     * @throws IllegalArgumentException
     *                if an invalid field is specified.
     */
    //@Override
    CARAPI Roll(
        /* [in] */ Int32 field,
        /* [in] */ Int32 value);

    /**
     * Increments or decrements the specified field and wraps the value of the
     * field when it goes beyond the maximum or minimum value for the current
     * date. Other fields will be adjusted as required to maintain a consistent
     * date. For example, March 31 will roll to April 30 when rolling the month
     * field.
     *
     * @param field
     *            the field to roll.
     * @param increment
     *            {@code true} to increment the field, {@code false} to
     *            decrement.
     * @throws IllegalArgumentException
     *                if an invalid field is specified.
     */
    //@Override
    CARAPI Roll(
        /* [in] */ Int32 field,
        /* [in] */ Boolean increment);

    CARAPI Clone(
        /* [out] */ IInterface** cloned);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI ToString(
        /* [out] */ String* str);

protected:
    CARAPI ComputeFields();

    CARAPI ComputeTime();

protected:
    CARAPI CloneImpl(
        /* [in] */ IGregorianCalendar* object);

private:
    CARAPI_(void) FullFieldsCalc();

    CARAPI_(Int32) ComputeYearAndDay(
        /* [in] */ Int64 dayCount,
        /* [in] */ Int64 localTime);

    CARAPI_(Int64) DaysFromBaseYear(
        /* [in] */ Int64 iyear);

    CARAPI_(Int32) DaysInMonth();

    CARAPI_(Int32) DaysInMonth(
        /* [in] */ Boolean leapYear,
        /* [in] */ Int32 month);

    CARAPI_(Int32) DaysInYear(
        /* [in] */ Int32 year);

    CARAPI_(Int32) DaysInYear(
        /* [in] */ Boolean leapYear,
        /* [in] */ Int32 month);

    CARAPI_(Int32) GetOffset(
        /* [in] */ Int64 localTime);

    CARAPI_(Int32) JulianError();

    CARAPI_(Int32) Mod(
        /* [in] */ Int32 value,
        /* [in] */ Int32 mod);

    CARAPI_(Int32) Mod7(
        /* [in] */ Int64 num1);

    using Calendar::GetFirstDayOfWeek;

    CARAPI_(Int32) GetFirstDayOfWeek();

    CARAPI_(Boolean) IsLeapYear(
        /* [in] */ Int32 year);

public:
    static AutoPtr<ArrayOf<Byte> > sDaysInMonth;

private:
    static const Int64 sDefaultGregorianCutover;

    Int64 mGregorianCutover;
    Int32 mChangeYear;
    Int32 mJulianSkew;

    const static Int32 sDaysInYear[12];
    const static Int32 sMaximums[17];
    const static Int32 sMinimums[17];
    const static Int32 sLeastMaximums[17];

    Int32 mCurrentYearSkew;
    Int32 mLastYearSkew;
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_CGREGORIANCALENDER_H__
