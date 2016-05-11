
#ifndef __ELASTOS_UTILITY_DATE_H__
#define __ELASTOS_UTILITY_DATE_H__

#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::StringBuilder;
using Elastos::Core::ICloneable;
using Elastos::Core::IComparable;

namespace Elastos {
namespace Utility {

class Date
    : public Object
    , public IDate
    , public ISerializable
    , public ICloneable
    , public IComparable

{
public:
    CAR_INTERFACE_DECL()

    Date();

    Date(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day);

    Date(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day,
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute);

    Date(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day,
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute,
        /* [in] */ Int32 second);

    Date(
        /* [in] */ Int64 milliseconds);

    Date(
        /* [in] */ const String& string);

    /**
     * Initializes this {@code Date} instance to the current time.
     */
    CARAPI constructor();

    /**
     * Constructs a new {@code Date} initialized to midnight in the default {@code TimeZone} on
     * the specified date.
     *
     * @param year
     *            the year, 0 is 1900.
     * @param month
     *            the month, 0 - 11.
     * @param day
     *            the day of the month, 1 - 31.
     *
     * @deprecated use
     *             {@link GregorianCalendar#GregorianCalendar(int, int, int)}
     */
    //@Deprecated
    CARAPI constructor(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day);

    /**
     * Constructs a new {@code Date} initialized to the specified date and time in the
     * default {@code TimeZone}.
     *
     * @param year
     *            the year, 0 is 1900.
     * @param month
     *            the month, 0 - 11.
     * @param day
     *            the day of the month, 1 - 31.
     * @param hour
     *            the hour of day, 0 - 23.
     * @param minute
     *            the minute of the hour, 0 - 59.
     *
     * @deprecated use
     *             {@link GregorianCalendar#GregorianCalendar(int, int, int, int, int)}
     */
    //@Deprecated
    CARAPI constructor(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day,
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute);

        /**
     * Constructs a new {@code Date} initialized to the specified date and time in the
     * default {@code TimeZone}.
     *
     * @param year
     *            the year, 0 is 1900.
     * @param month
     *            the month, 0 - 11.
     * @param day
     *            the day of the month, 1 - 31.
     * @param hour
     *            the hour of day, 0 - 23.
     * @param minute
     *            the minute of the hour, 0 - 59.
     * @param second
     *            the second of the minute, 0 - 59.
     *
     * @deprecated use
     *             {@link GregorianCalendar#GregorianCalendar(int, int, int, int, int, int)}
     */
    //@Deprecated
    CARAPI constructor(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day,
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute,
        /* [in] */ Int32 second);

    /**
     * Initializes this {@code Date} instance using the specified millisecond value. The
     * value is the number of milliseconds since Jan. 1, 1970 GMT.
     *
     * @param milliseconds
     *            the number of milliseconds since Jan. 1, 1970 GMT.
     */
    CARAPI constructor(
        /* [in] */ Int64 milliseconds);

    /**
     * Constructs a new {@code Date} initialized to the date and time parsed from the
     * specified String.
     *
     * @param string
     *            the String to parse.
     *
     * @deprecated use {@link DateFormat}
     */
    //@Deprecated
    CARAPI constructor(
        /* [in] */ const String& string);

    /**
     * Returns if this {@code Date} is after the specified Date.
     *
     * @param date
     *            a Date instance to compare.
     * @return {@code true} if this {@code Date} is after the specified {@code Date},
     *         {@code false} otherwise.
     */
    CARAPI IsAfter(
        /* [in] */ IDate* date,
        /* [out] */ Boolean* isAfter);

    /**
     * Returns if this {@code Date} is before the specified Date.
     *
     * @param date
     *            a {@code Date} instance to compare.
     * @return {@code true} if this {@code Date} is before the specified {@code Date},
     *         {@code false} otherwise.
     */
    CARAPI IsBefore(
        /* [in] */ IDate* date,
        /* [out] */ Boolean* isBefore);

    /**
     * Returns a new {@code Date} with the same millisecond value as this {@code Date}.
     *
     * @return a shallow copy of this {@code Date}.
     *
     * @see java.lang.Cloneable
     */
    CARAPI Clone(
        /* [out] */ IInterface** date);

    /**
     * Compare the receiver to the specified {@code Date} to determine the relative
     * ordering.
     *
     * @param date
     *            a {@code Date} to compare against.
     * @return an {@code int < 0} if this {@code Date} is less than the specified {@code Date}, {@code 0} if
     *         they are equal, and an {@code int > 0} if this {@code Date} is greater.
     */
    CARAPI CompareTo(
        /* [in] */ IInterface* date,
        /* [out] */ Int32* result);

    /**
     * Compares the specified object to this {@code Date} and returns if they are equal.
     * To be equal, the object must be an instance of {@code Date} and have the same millisecond
     * value.
     *
     * @param object
     *            the object to compare with this object.
     * @return {@code true} if the specified object is equal to this {@code Date}, {@code false}
     *         otherwise.
     *
     * @see #hashCode
     */
    CARAPI Equals(
        /* [in] */ IInterface* date,
        /* [out] */ Boolean* result);

    /**
     * Returns the gregorian calendar day of the month for this {@code Date} object.
     *
     * @return the day of the month.
     *
     * @deprecated use {@code Calendar.get(Calendar.DATE)}
     */
    //@Deprecated
    CARAPI GetDate(
        /* [out] */ Int32* date);

    /**
     * Returns the gregorian calendar day of the week for this {@code Date} object.
     *
     * @return the day of the week.
     *
     * @deprecated use {@code Calendar.get(Calendar.DAY_OF_WEEK)}
     */
    //@Deprecated
    CARAPI GetDay(
        /* [out] */ Int32* day);

    /**
     * Returns the gregorian calendar hour of the day for this {@code Date} object.
     *
     * @return the hour of the day.
     *
     * @deprecated use {@code Calendar.get(Calendar.HOUR_OF_DAY)}
     */
    //@Deprecated
    CARAPI GetHours(
        /* [out] */ Int32* hours);

    /**
     * Returns the gregorian calendar minute of the hour for this {@code Date} object.
     *
     * @return the minutes.
     *
     * @deprecated use {@code Calendar.get(Calendar.MINUTE)}
     */
    //@Deprecated
    CARAPI GetMinutes(
        /* [out] */ Int32* minutes);

    /**
     * Returns the gregorian calendar month for this {@code Date} object.
     *
     * @return the month.
     *
     * @deprecated use {@code Calendar.get(Calendar.MONTH)}
     */
    //@Deprecated
    CARAPI GetMonth(
        /* [out] */ Int32* month);

    /**
     * Returns the gregorian calendar second of the minute for this {@code Date} object.
     *
     * @return the seconds.
     *
     * @deprecated use {@code Calendar.get(Calendar.SECOND)}
     */
    //@Deprecated
    CARAPI GetSeconds(
        /* [out] */ Int32* seconds);

    /**
     * Returns this {@code Date} as a millisecond value. The value is the number of
     * milliseconds since Jan. 1, 1970, midnight GMT.
     *
     * @return the number of milliseconds since Jan. 1, 1970, midnight GMT.
     */
    CARAPI GetTime(
        /* [out] */ Int64* time);

    /**
     * Returns the timezone offset in minutes of the default {@code TimeZone}.
     *
     * @return the timezone offset in minutes of the default {@code TimeZone}.
     *
     * @deprecated use
     *             {@code (Calendar.get(Calendar.ZONE_OFFSET) + Calendar.get(Calendar.DST_OFFSET)) / 60000}
     */
    //@Deprecated
    CARAPI GetTimezoneOffset(
        /* [out] */ Int32* timezoneOffset);

    /**
     * Returns the gregorian calendar year since 1900 for this {@code Date} object.
     *
     * @return the year - 1900.
     *
     * @deprecated use {@code Calendar.get(Calendar.YEAR) - 1900}
     */
    //@Deprecated
    CARAPI GetYear(
        /* [out] */ Int32* year);

    /**
     * Sets the gregorian calendar day of the month for this {@code Date} object.
     *
     * @param day
     *            the day of the month.
     *
     * @deprecated use {@code Calendar.set(Calendar.DATE, day)}
     */
    //@Deprecated
    CARAPI SetDate(
        /* [in] */ Int32 day);

    /**
     * Sets the gregorian calendar hour of the day for this {@code Date} object.
     *
     * @param hour
     *            the hour of the day.
     *
     * @deprecated use {@code Calendar.set(Calendar.HOUR_OF_DAY, hour)}
     */
    //@Deprecated
    CARAPI SetHours(
        /* [in] */ Int32 hour);

    /**
     * Sets the gregorian calendar minute of the hour for this {@code Date} object.
     *
     * @param minute
     *            the minutes.
     *
     * @deprecated use {@code Calendar.set(Calendar.MINUTE, minute)}
     */
    //@Deprecated
    CARAPI SetMinutes(
        /* [in] */ Int32 minute);

    /**
     * Sets the gregorian calendar month for this {@code Date} object.
     *
     * @param month
     *            the month.
     *
     * @deprecated use {@code Calendar.set(Calendar.MONTH, month)}
     */
    //@Deprecated
    CARAPI SetMonth(
        /* [in] */ Int32 month);

    /**
     * Sets the gregorian calendar second of the minute for this {@code Date} object.
     *
     * @param second
     *            the seconds.
     *
     * @deprecated use {@code Calendar.set(Calendar.SECOND, second)}
     */
    //@Deprecated
    CARAPI SetSeconds(
        /* [in] */ Int32 second);

    /**
     * Sets this {@code Date} to the specified millisecond value. The value is the
     * number of milliseconds since Jan. 1, 1970 GMT.
     *
     * @param milliseconds
     *            the number of milliseconds since Jan. 1, 1970 GMT.
     */
    CARAPI SetTime(
        /* [in] */ Int64 milliseconds);

    /**
     * Sets the gregorian calendar year since 1900 for this {@code Date} object.
     *
     * @param year
     *            the year since 1900.
     *
     * @deprecated use {@code Calendar.set(Calendar.YEAR, year + 1900)}
     */
    //@Deprecated
    CARAPI SetYear(
        /* [in] */ Int32 year);

    /**
     * Returns the string representation of this {@code Date} in GMT in the format: 22
     * Jun 1999 13:02:00 GMT
     *
     * @return the string representation of this {@code Date} in GMT.
     *
     * @deprecated use {@link DateFormat}
     */
    //@Deprecated
    CARAPI ToGMTString(
        /* [out] */ String* gmtStr);

    /**
     * Returns the string representation of this {@code Date} for the default {@code Locale}.
     *
     * @return the string representation of this {@code Date} for the default {@code Locale}.
     *
     * @deprecated use {@link DateFormat}
     */
    //@Deprecated
    CARAPI ToLocaleString(
        /* [out] */ String* localeStr);

    /**
     * Returns a string representation of this {@code Date}.
     * The formatting is equivalent to using a {@code SimpleDateFormat} with
     * the format string "EEE MMM dd HH:mm:ss zzz yyyy", which looks something
     * like "Tue Jun 22 13:07:00 PDT 1999". The current default time zone and
     * locale are used. If you need control over the time zone or locale,
     * use {@code SimpleDateFormat} instead.
     */
    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

public:
    /**
     * Returns the millisecond value of the date and time parsed from the
     * specified {@code String}. Many date/time formats are recognized, including IETF
     * standard syntax, i.e. Tue, 22 Jun 1999 12:16:00 GMT-0500
     *
     * @param string
     *            the String to parse.
     * @return the millisecond value parsed from the String.
     *
     * @deprecated use {@link DateFormat}
     */
    //@Deprecated
    static CARAPI Parse(
        /* [in] */ const String& string,
        /* [out] */ Int64* value);

    /**
     * Returns the millisecond value of the specified date and time in GMT.
     *
     * @param year
     *            the year, 0 is 1900.
     * @param month
     *            the month, 0 - 11.
     * @param day
     *            the day of the month, 1 - 31.
     * @param hour
     *            the hour of day, 0 - 23.
     * @param minute
     *            the minute of the hour, 0 - 59.
     * @param second
     *            the second of the minute, 0 - 59.
     * @return the date and time in GMT in milliseconds.
     *
     * @deprecated use: <code>
     *  Calendar cal = new GregorianCalendar(TimeZone.getTimeZone("GMT"));
     *  cal.set(year + 1900, month, day, hour, minute, second);
     *  cal.getTime().getTime();</code>
     */
    //@Deprecated
    static CARAPI_(Int64) UTC(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day,
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute,
        /* [in] */ Int32 second);

private:
    static CARAPI_(void) AppendTwoDigits(
        /* [in] */ StringBuilder& sb,
        /* [in] */ Int32 n);

    static CARAPI_(Int32) Zone(
        /* [in] */ const String& text);

    static CARAPI_(Int32) Parse(
        /* [in] */ const String& string,
        /* [in] */ const ArrayOf<String>& array);

    static CARAPI_(Int32) GetCreationYear();

private:
    // Used by parse()
    static Int32 CREATION_YEAR;

    Int64 mMilliseconds;
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_DATE_H__
