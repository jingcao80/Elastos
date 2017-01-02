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

#ifndef __ELASTOS_UTILITY_TIMEZONE_H__
#define __ELASTOS_UTILITY_TIMEZONE_H__

#include "CDate.h"
#include "StringBuffer.h"
#include "Object.h"

using Elastos::Core::StringBuffer;
using Elastos::Utility::ILocale;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Utility {

class TimeZone
    : public Object
    , public ITimeZone
    , public ISerializable
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    TimeZone() {}

    virtual ~TimeZone() {}

    CARAPI CloneImpl(
        /* [out] */ ITimeZone * other);

    /**
     * Equivalent to {@code getDisplayName(false, TimeZone.LONG, Locale.getDefault())}.
     * <a href="../util/Locale.html#default_locale">Be wary of the default locale</a>.
     */
    virtual CARAPI GetDisplayName(
        /* [out] */ String* name);

    /**
     * Equivalent to {@code getDisplayName(false, TimeZone.LONG, locale)}.
     */
    virtual CARAPI GetDisplayName(
        /* [in] */ ILocale* locale,
        /* [out] */ String* name);

    /**
     * Equivalent to {@code getDisplayName(daylightTime, style, Locale.getDefault())}.
     * <a href="../util/Locale.html#default_locale">Be wary of the default locale</a>.
     */
    virtual CARAPI GetDisplayName(
        /* [in] */ Boolean daylightTime,
        /* [in] */ Int32 style,
        /* [out] */ String* name);

    /**
     * Returns the {@link #SHORT short} or {@link #LONG long} name of this time
     * zone with either standard or daylight time, as written in {@code locale}.
     * If the name is not available, the result is in the format
     * {@code GMT[+-]hh:mm}.
     *
     * @param daylightTime true for daylight time, false for standard time.
     * @param style either {@link TimeZone#LONG} or {@link TimeZone#SHORT}.
     * @param locale the display locale.
     */
    virtual CARAPI GetDisplayName(
        /* [in] */ Boolean daylightTime,
        /* [in] */ Int32 style,
        /* [in] */ ILocale* locale,
        /* [out] */ String* name);

    /**
     * Returns the ID of this {@code TimeZone}, such as
     * {@code America/Los_Angeles}, {@code GMT-08:00} or {@code UTC}.
     */
    virtual CARAPI GetID(
        /* [out] */ String* id);

    /**
     * Returns the latest daylight savings in milliseconds for this time zone, relative
     * to this time zone's regular UTC offset (as returned by {@link #getRawOffset}).
     *
     * <p>This class returns {@code 3600000} (1 hour) for time zones
     * that use daylight savings time and {@code 0} for timezones that do not,
     * leaving it to subclasses to override this method for other daylight savings
     * offsets. (There are time zones, such as {@code Australia/Lord_Howe},
     * that use other values.)
     *
     * <p>Note that this method doesn't tell you whether or not to <i>apply</i> the
     * offset: you need to call {@code inDaylightTime} for the specific time
     * you're interested in. If this method returns a non-zero offset, that only
     * tells you that this {@code TimeZone} sometimes observes daylight savings.
     *
     * <p>Note also that this method doesn't necessarily return the value you need
     * to apply to the time you're working with. This value can and does change over
     * time for a given time zone.
     *
     * <p>It's highly unlikely that you should ever call this method. You
     * probably want {@link #getOffset} instead, which tells you the offset
     * for a specific point in time, and takes daylight savings into account for you.
     */
    virtual CARAPI GetDSTSavings(
        /* [out] */ Int32* savings);

    /**
     * Returns the offset in milliseconds from UTC for this time zone at {@code
     * time}. The offset includes daylight savings time if the specified
     * date is within the daylight savings time period.
     *
     * @param time the date in milliseconds since January 1, 1970 00:00:00 UTC
     */
    virtual CARAPI GetOffset(
        /* [in] */ Int64 time,
        /* [out] */ Int32* offset);

    /**
     * Returns the offset in milliseconds from UTC for this time zone at {@code
     * time}. The offset includes daylight savings time if the specified
     * date is within the daylight savings time period.
     *
     * @param time the date in milliseconds since January 1, 1970 00:00:00 UTC
     */
    virtual CARAPI GetOffset(
        /* [in] */ Int32 era,
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day,
        /* [in] */ Int32 dayOfWeek,
        /* [in] */ Int32 timeOfDayMillis,
        /* [out] */ Int32* offset) = 0;

    /**
     * Returns the offset in milliseconds from UTC of this time zone's standard
     * time.
     */
    virtual CARAPI GetRawOffset(
        /* [out] */ Int32* rawOffset) = 0;

    /**
     * Returns true if {@code timeZone} has the same rules as this time zone.
     *
     * <p>The base implementation returns true if both time zones have the same
     * raw offset.
     */
    virtual CARAPI HasSameRules(
        /* [in] */ ITimeZone* timezone,
        /* [out] */ Boolean* hasRules);

    /**
     * Returns true if {@code time} is in a daylight savings time period for
     * this time zone.
     */
    virtual CARAPI InDaylightTime(
        /* [in] */ IDate* time,
        /* [out] */ Boolean* isIn) = 0;

    /**
     * Sets the ID of this {@code TimeZone}.
     */
    virtual CARAPI SetID(
        /* [in] */ const String& id);

    /**
     * Sets the offset in milliseconds from UTC of this time zone's standard
     * time.
     */
    virtual CARAPI SetRawOffset(
        /* [in] */ Int32 offsetMillis) = 0;

    /**
     * Returns true if this time zone has a future transition to or from
     * daylight savings time.
     *
     * <p><strong>Warning:</strong> this returns false for time zones like
     * {@code Asia/Kuala_Lumpur} that have previously used DST but do not
     * currently. A hypothetical country that has never observed daylight
     * savings before but plans to start next year would return true.
     *
     * <p><strong>Warning:</strong> this returns true for time zones that use
     * DST, even when it is not active.
     *
     * <p>Use {@link #inDaylightTime} to find out whether daylight savings is
     * in effect at a specific time.
     *
     * <p>Most applications should not use this method.
     */
    virtual CARAPI UseDaylightTime(
        /* [out] */ Boolean* isUsed) = 0;

    static CARAPI_(AutoPtr<ArrayOf<String> >) GetAvailableIDs();

    static CARAPI_(AutoPtr<ArrayOf<String> >) GetAvailableIDs(
        /* [in] */ Int32 offsetMillis);

    static CARAPI_(AutoPtr<ITimeZone>) GetDefault();

    static CARAPI_(AutoPtr<ITimeZone>) GMT();

    static CARAPI_(AutoPtr<ITimeZone>) UTC();

    static CARAPI GetTimeZone(
        /* [in] */ const String& id,
        /* [out] */ ITimeZone** timeZone);

    static CARAPI SetDefault(
        /* [in] */ ITimeZone * timezone);

    static CARAPI_(String) CreateGmtOffsetString(
        /* [in] */ Boolean includeGmt,
        /* [in] */ Boolean includeMinuteSeparator,
        /* [in] */ Int32 offsetMillis);
private:
    static void AppendNumber(
        /* [out] */ StringBuffer* buffer,
        /* [in] */ Int32 count,
        /* [in] */ Int32 value);

    /**
     * Returns a new SimpleTimeZone for an ID of the form "GMT[+|-]hh[[:]mm]", or null.
     */
    static CARAPI_(AutoPtr<ITimeZone>) GetCustomTimeZone(
        /* [in] */ const String& id);

protected:
    String mID;

private:
    static AutoPtr<ITimeZone> sGMT;
    static AutoPtr<ITimeZone> sUTC;
    static AutoPtr<ITimeZone> sCHINA;
    static AutoPtr<ITimeZone> sDefaultTimeZone;
    static AutoPtr<IPattern>  CUSTOM_ZONE_ID_PATTERN;
    static Object sLock;
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_TIMEZONE_H__
