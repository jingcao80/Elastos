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

#ifndef __ELASTOS_UTILITY_CCALENDAR_H__
#define __ELASTOS_UTILITY_CCALENDAR_H__

#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Core::IComparable;
using Elastos::Core::ICloneable;
using Elastos::Core::Object;
using Elastos::IO::ISerializable;
using Elastos::Utility::ILocale;

namespace Elastos{
namespace Utility{

class Calendar
    : public Object
    , public ICalendar
    , public ISerializable
    , public ICloneable
    , public IComparable
{
protected:
    CAR_INTERFACE_DECL()

    Calendar();

    /**
     * Constructs a {@code Calendar} instance using the default {@code TimeZone} and {@code Locale}.
     */
    CARAPI constructor();

    /**
     * Constructs a {@code Calendar} instance using the specified {@code TimeZone} and {@code Locale}.
     *
     * @param timezone
     *            the timezone.
     * @param locale
     *            the locale.
     */
    CARAPI constructor(
        /* [in] */ ITimeZone* timezone,
        /* [in] */ ILocale* locale);

    static AutoPtr<ILocale> GetDefaultLocale();

    CARAPI CloneImpl(
        /* [in] */ ICalendar* clone);

public:

    virtual ~Calendar() {}

    CARAPI constructor(
        /* [in] */ ITimeZone* timezone);

    /**
     * Adds the specified amount to a {@code Calendar} field.
     *
     * @param field
     *            the {@code Calendar} field to modify.
     * @param value
     *            the amount to add to the field.
     * @throws IllegalArgumentException
     *                if {@code field} is {@code DST_OFFSET} or {@code
     *                ZONE_OFFSET}.
     */
    virtual CARAPI Add(
        /* [in] */ Int32 field,
        /* [in] */ Int32 value) = 0;

    /**
     * Returns whether the {@code Date} specified by this {@code Calendar} instance is after the {@code Date}
     * specified by the parameter. The comparison is not dependent on the time
     * zones of the {@code Calendar}.
     *
     * @param calendar
     *            the {@code Calendar} instance to compare.
     * @return {@code true} when this Calendar is after calendar, {@code false} otherwise.
     * @throws IllegalArgumentException
     *                if the time is not set and the time cannot be computed
     *                from the current field values.
     */
    virtual CARAPI IsAfter(
        /* [in */ ICalendar* calendar,
        /* [out] */ Boolean* result);

    /**
     * Returns whether the {@code Date} specified by this {@code Calendar} instance is before the
     * {@code Date} specified by the parameter. The comparison is not dependent on the
     * time zones of the {@code Calendar}.
     *
     * @param calendar
     *            the {@code Calendar} instance to compare.
     * @return {@code true} when this Calendar is before calendar, {@code false} otherwise.
     * @throws IllegalArgumentException
     *                if the time is not set and the time cannot be computed
     *                from the current field values.
     */
    virtual CARAPI IsBefore(
        /* [in */ ICalendar* calendar,
        /* [out] */ Boolean* result);

    /**
     * Clears all of the fields of this {@code Calendar}. All fields are initialized to
     * zero.
     */
    CARAPI Clear();

    /**
     * Clears the specified field to zero and sets the isSet flag to {@code false}.
     *
     * @param field
     *            the field to clear.
     */
    CARAPI Clear(
        /* [in] */ Int32 field);

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
    virtual CARAPI Equals(
        /* [in] */ ICalendar* other,
        /* [out] */ Boolean* result);

    virtual CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    /**
     * Gets the value of the specified field after computing the field values by
     * calling {@code complete()} first.
     *
     * @param field
     *            the field to get.
     * @return the value of the specified field.
     *
     * @throws IllegalArgumentException
     *                if the fields are not set, the time is not set, and the
     *                time cannot be computed from the current field values.
     * @throws ArrayIndexOutOfBoundsException
     *                if the field is not inside the range of possible fields.
     *                The range is starting at 0 up to {@code FIELD_COUNT}.
     */
    virtual CARAPI Get(
        /* [in] */ Int32 field,
        /* [out] */ Int32* pValue);

    /**
     * Returns the maximum value of the specified field for the current date.
     * For example, the maximum number of days in the current month.
     */
    virtual CARAPI GetActualMaximum(
        /* [in] */ Int32 field,
        /* [out] */ Int32* pValue);

    /**
     * Gets the minimum value of the specified field for the current date.
     *
     * @param field
     *            the field.
     * @return the minimum value of the specified field.
     */
    virtual CARAPI GetActualMinimum(
        /* [in] */ Int32 field,
        /* [out] */ Int32* pValue);

    /**
     * Gets the first day of the week for this {@code Calendar}.
     *
     * @return the first day of the week.
     */
    virtual CARAPI GetFirstDayOfWeek(
         /* [out] */ Int32* pDay);

    /**
     * Gets the greatest minimum value of the specified field. This is the
     * biggest value that {@code getActualMinimum} can return for any possible
     * time.
     *
     * @param field
     *            the field.
     * @return the greatest minimum value of the specified field.
     */
    virtual CARAPI GetGreatestMinimum(
        /* [in] */ Int32 field,
        /* [out] */ Int32* pValue) = 0;

    /**
     * Gets the smallest maximum value of the specified field. This is the
     * smallest value that {@code getActualMaximum()} can return for any
     * possible time.
     *
     * @param field
     *            the field number.
     * @return the smallest maximum value of the specified field.
     */
    virtual CARAPI GetLeastMaximum(
        /* [in] */ Int32 field,
        /* [out] */ Int32* pValue) = 0;

    /**
     * Gets the greatest maximum value of the specified field. This returns the
     * biggest value that {@code get} can return for the specified field.
     *
     * @param field
     *            the field.
     * @return the greatest maximum value of the specified field.
     */
    virtual CARAPI GetMaximum(
        /* [in] */ Int32 field,
        /* [out] */ Int32* pValue) = 0;

    /**
     * Gets the minimal days in the first week of the year.
     *
     * @return the minimal days in the first week of the year.
     */
    virtual CARAPI GetMinimalDaysInFirstWeek(
         /* [out] */ Int32* pDay);

    /**
     * Gets the smallest minimum value of the specified field. this returns the
     * smallest value thet {@code get} can return for the specified field.
     *
     * @param field
     *            the field number.
     * @return the smallest minimum value of the specified field.
     */
    virtual CARAPI GetMinimum(
        /* [in] */ Int32 field,
        /* [out] */ Int32* pValue) = 0;

    /**
     * Gets the time of this {@code Calendar} as a {@code Date} object.
     *
     * @return a new {@code Date} initialized to the time of this {@code Calendar}.
     *
     * @throws IllegalArgumentException
     *                if the time is not set and the time cannot be computed
     *                from the current field values.
     */
    CARAPI GetTime(
         /* [out] */ IDate** ppDate);

    /**
     * Computes the time from the fields if required and returns the time.
     *
     * @return the time of this {@code Calendar}.
     *
     * @throws IllegalArgumentException
     *                if the time is not set and the time cannot be computed
     *                from the current field values.
     */
    virtual CARAPI GetTimeInMillis(
        /* [out] */ Int64* result);

    /**
     * Gets the timezone of this {@code Calendar}.
     *
     * @return the {@code TimeZone} used by this {@code Calendar}.
     */
    virtual CARAPI GetTimeZone(
         /* [out] */ ITimeZone** ppZone);

    /**
     * Returns if this {@code Calendar} accepts field values which are outside the valid
     * range for the field.
     *
     * @return {@code true} if this {@code Calendar} is lenient, {@code false} otherwise.
     */
    virtual CARAPI IsLenient(
        /* [out] */ Boolean* isLenient);

    /**
     * Returns whether the specified field is set. Note that the interpretation of "is set" is
     * somewhat technical. In particular, it does <i>not</i> mean that the field's value is up
     * to date. If you want to know whether a field contains an up-to-date value, you must also
     * check {@code areFieldsSet}, making this method somewhat useless unless you're a subclass,
     * in which case you can access the {@code isSet} array directly.
     * <p>
     * A field remains "set" from the first time its value is computed until it's cleared by one
     * of the {@code clear} methods. Thus "set" does not mean "valid". You probably want to call
     * {@code get} -- which will update fields as necessary -- rather than try to make use of
     * this method.
     *
     * @param field
     *            a {@code Calendar} field number.
     * @return {@code true} if the specified field is set, {@code false} otherwise.
     */
    CARAPI IsSet(
        /* [in] */ Int32 field,
        /* [out] */ Boolean* isSet);

    /**
     * Adds the specified amount to the specified field and wraps the value of
     * the field when it goes beyond the maximum or minimum value for the
     * current date. Other fields will be adjusted as required to maintain a
     * consistent date.
     *
     * @param field
     *            the field to roll.
     * @param value
     *            the amount to add.
     */
    virtual CARAPI Roll(
        /* [in] */ Int32 field,
        /* [in] */ Int32 value);

    /**
     * Increment or decrement the specified field and wrap the value of the
     * field when it goes beyond the maximum or minimum value for the current
     * date. Other fields will be adjusted as required to maintain a consistent
     * date.
     *
     * @param field
     *            the number indicating the field to roll.
     * @param increment
     *            {@code true} to increment the field, {@code false} to decrement.
     */
    virtual CARAPI Roll(
        /* [in] */ Int32 field,
        /* [in] */ Boolean increment) = 0;

    /**
     * Sets a field to the specified value.
     *
     * @param field
     *            the code indicating the {@code Calendar} field to modify.
     * @param value
     *            the value.
     */
    virtual CARAPI Set(
        /* [in] */ Int32 field,
        /* [in] */ Int32 value);

    /**
     * Sets the year, month and day of the month fields. Other fields are not
     * changed.
     *
     * @param year
     *            the year.
     * @param month
     *            the month.
     * @param day
     *            the day of the month.
     */
    CARAPI Set(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day);

    /**
     * Sets the year, month, day of the month, hour of day and minute fields.
     * Other fields are not changed.
     *
     * @param year
     *            the year.
     * @param month
     *            the month.
     * @param day
     *            the day of the month.
     * @param hourOfDay
     *            the hour of day.
     * @param minute
     *            the minute.
     */
    CARAPI Set(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day,
        /* [in] */ Int32 hourOfDay,
        /* [in] */ Int32 minute);

    /**
     * Sets the year, month, day of the month, hour of day, minute and second
     * fields. Other fields are not changed.
     *
     * @param year
     *            the year.
     * @param month
     *            the month.
     * @param day
     *            the day of the month.
     * @param hourOfDay
     *            the hour of day.
     * @param minute
     *            the minute.
     * @param second
     *            the second.
     */
    CARAPI Set(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day,
        /* [in] */ Int32 hourOfDay,
        /* [in] */ Int32 minute,
        /* [in] */ Int32 second);

    /**
     * Sets the first day of the week for this {@code Calendar}.
     *
     * @param value
     *            a {@code Calendar} day of the week.
     */
    virtual CARAPI SetFirstDayOfWeek(
        /* [in] */ Int32 value);

    /**
     * Sets this {@code Calendar} to accept field values which are outside the valid
     * range for the field.
     *
     * @param value
     *            a boolean value.
     */
    virtual CARAPI SetLenient(
        /* [in] */ Boolean value);

    /**
     * Sets the minimal days in the first week of the year.
     *
     * @param value
     *            the minimal days in the first week of the year.
     */
    virtual CARAPI SetMinimalDaysInFirstWeek(
        /* [in] */ Int32 value);

    /**
     * Sets the time of this {@code Calendar}.
     *
     * @param date
     *            a {@code Date} object.
     */
    CARAPI SetTime(
        /* [in] */ IDate* date);

    /**
     * Sets the time of this {@code Calendar}.
     *
     * @param milliseconds
     *            the time as the number of milliseconds since Jan. 1, 1970.
     */
    virtual CARAPI SetTimeInMillis(
        /* [in] */ Int64 milliseconds);

    /**
     * Sets the {@code TimeZone} used by this Calendar.
     *
     * @param timezone
     *            a {@code TimeZone}.
     */
    virtual CARAPI SetTimeZone(
        /* [in] */ ITimeZone* timezone);

    /**
     * Compares the times of the two {@code Calendar}, which represent the milliseconds
     * from the January 1, 1970 00:00:00.000 GMT (Gregorian).
     *
     * @param anotherCalendar
     *            another calendar that this one is compared with.
     * @return 0 if the times of the two {@code Calendar}s are equal, -1 if the time of
     *         this {@code Calendar} is before the other one, 1 if the time of this
     *         {@code Calendar} is after the other one.
     * @throws NullPointerException
     *             if the argument is null.
     * @throws IllegalArgumentException
     *             if the argument does not include a valid time
     *             value.
     */
    virtual CARAPI CompareTo(
        /* [in] */ IInterface* other,
        /* [out] */ Int32* result);

    /**
     * Returns a human-readable string for the value of {@code field}
     * using the given style and locale. If no string is available, returns null.
     * The value is retrieved by invoking {@code get(field)}.
     *
     * <p>For example, {@code getDisplayName(MONTH, SHORT, Locale.US)} will return "Jan"
     * while {@code getDisplayName(MONTH, LONG, Locale.US)} will return "January".
     *
     * @param field the field
     * @param style {@code SHORT} or {@code LONG}
     * @param locale the locale
     * @return the display name, or null
     * @throws NullPointerException if {@code locale == null}
     * @throws IllegalArgumentException if {@code field} or {@code style} is invalid
     * @since 1.6
     */
    virtual CARAPI GetDisplayName(
        /* [in] */ Int32 field,
        /* [in] */ Int32 style,
        /* [in] */ ILocale* locale,
        /* [out] */ String* name);

    /**
     * Returns a map of human-readable strings to corresponding values,
     * for the given field, style, and locale.
     * Returns null if no strings are available.
     *
     * <p>For example, {@code getDisplayNames(MONTH, ALL_STYLES, Locale.US)} would
     * contain mappings from "Jan" and "January" to {@link #JANUARY}, and so on.
     *
     * @param field the field
     * @param style {@code SHORT}, {@code LONG}, or {@code ALL_STYLES}
     * @param locale the locale
     * @return the display name, or null
     * @throws NullPointerException if {@code locale == null}
     * @throws IllegalArgumentException if {@code field} or {@code style} is invalid
     * @since 1.6
     */
    virtual CARAPI GetDisplayNames(
        /* [in] */ Int32 field,
        /* [in] */ Int32 style,
        /* [in] */ ILocale* locale,
        /* [out] */ IMap** array);

    using Object::GetHashCode;

    CARAPI_(Int32) GetHashCode();

    using Object::ToString;

    CARAPI_(String) ToString();

public:
    /**
     * Returns an array of locales for which custom {@code Calendar} instances
     * are available.
     * <p>Note that Android does not support user-supplied locale service providers.
     */
    static CARAPI GetAvailableLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** ppLocales);

    /**
     * Constructs a new instance of the {@code Calendar} subclass appropriate for the
     * default {@code Locale}.
     *
     * @return a {@code Calendar} subclass instance set to the current date and time in
     *         the default {@code Timezone}.
     */
    static CARAPI GetInstance(
        /* [out, callee] */ ICalendar** calendar);

    /**
     * Constructs a new instance of the {@code Calendar} subclass appropriate for the
     * specified {@code Locale}.
     *
     * @param locale
     *            the locale to use.
     * @return a {@code Calendar} subclass instance set to the current date and time.
     */
    static CARAPI GetInstance(
        /* [in] */ ILocale* locale,
        /* [out, callee] */ ICalendar** calendar);

    /**
     * Constructs a new instance of the {@code Calendar} subclass appropriate for the
     * default {@code Locale}, using the specified {@code TimeZone}.
     *
     * @param timezone
     *            the {@code TimeZone} to use.
     * @return a {@code Calendar} subclass instance set to the current date and time in
     *         the specified timezone.
     */
    static CARAPI GetInstance(
        /* [in] */ ITimeZone* timezone,
        /* [out, callee] */ ICalendar** calendar);

    /**
     * Constructs a new instance of the {@code Calendar} subclass appropriate for the
     * specified {@code Locale}.
     *
     * @param timezone
     *            the {@code TimeZone} to use.
     * @param locale
     *            the {@code Locale} to use.
     * @return a {@code Calendar} subclass instance set to the current date and time in
     *         the specified timezone.
     */
    static CARAPI GetInstance(
        /* [in] */ ITimeZone* timezone,
        /* [in] */ ILocale* locale,
        /* [out, callee] */ ICalendar** calendar);

protected:
    /**
     * Computes the time from the fields if the time has not already been set.
     * Computes the fields from the time if the fields are not already set.
     *
     * @throws IllegalArgumentException
     *                if the time is not set and the time cannot be computed
     *                from the current field values.
     */
    virtual CARAPI Complete();

    /**
     * Computes the {@code Calendar} fields from {@code time}.
     */
    virtual CARAPI ComputeFields() = 0;

    /**
     * Computes {@code time} from the Calendar fields.
     *
     * @throws IllegalArgumentException
     *                if the time cannot be computed from the current field
     *                values.
     */
    virtual CARAPI ComputeTime() = 0;

    /**
     * Gets the value of the specified field without recomputing.
     *
     * @param field
     *            the field.
     * @return the value of the specified field.
     */
    Int32 InternalGet(
        /* [in] */ Int32 field);

private:
    static CARAPI CheckStyle(
        /* [in] */ Int32 style);

    CARAPI GetDisplayNameArray(
        /* [in] */ Int32 field,
        /* [in] */ Int32 style,
        /* [in] */ ILocale* locale,
        /* [out, callee] */ ArrayOf<String>** array);

    static void InsertValuesInMap(
        /* [in] */ IMap* array,
        /* [in] */ ArrayOf<String>& values);

private:
    static const String FIELD_NAMES[];
    static Object sLock;

protected:
    /**
     * True iff the values in {@code fields[]} correspond to {@code time}. Despite the name, this
     * is effectively "are the values in fields[] up-to-date?" --- {@code fields[]} may contain
     * non-zero values and {@code isSet[]} may contain {@code true} values even when
     * {@code areFieldsSet} is false.
     * Accessing the fields via {@code get} will ensure the fields are up-to-date.
     */
    Boolean mAreFieldsSet;

    /**
     * Contains broken-down field values for the current value of {@code time} if
     * {@code areFieldsSet} is true, or stale data corresponding to some previous value otherwise.
     * Accessing the fields via {@code get} will ensure the fields are up-to-date.
     * The array length is always {@code FIELD_COUNT}.
     */
    AutoPtr<ArrayOf<Int32> > mFields;

    /**
     * Whether the corresponding element in {@code field[]} has been set. Initially, these are all
     * false. The first time the fields are computed, these are set to true and remain set even if
     * the data becomes stale: you <i>must</i> check {@code areFieldsSet} if you want to know
     * whether the value is up-to-date.
     * Note that {@code isSet} is <i>not</i> a safe alternative to accessing this array directly,
     * and will likewise return stale data!
     * The array length is always {@code FIELD_COUNT}.
     */
    AutoPtr<ArrayOf<Boolean> > mIsSet;

    /**
     * Whether {@code time} corresponds to the values in {@code fields[]}. If false, {@code time}
     * is out-of-date with respect to changes {@code fields[]}.
     * Accessing the time via {@code getTimeInMillis} will always return the correct value.
     */
    Boolean mIsTimeSet;

    /**
     * A time in milliseconds since January 1, 1970. See {@code isTimeSet}.
     * Accessing the time via {@code getTimeInMillis} will always return the correct value.
     */
    Int64 mTime;

protected:
    Boolean mLenient;
    Int32 mFirstDayOfWeek;
    Int32 mMinimalDaysInFirstWeek;

    AutoPtr<ITimeZone> mZone;

public:
    Int32 mLastTimeFieldSet;
    Int32 mLastDateFieldSet;
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CCALENDAR_H__
