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

#ifndef __LIBCORE_UTILITY_ZONEINFO_H__
#define __LIBCORE_UTILITY_ZONEINFO_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include "TimeZone.h"

using Elastos::Utility::IDate;
using Elastos::Utility::TimeZone;
using Libcore::IO::IBufferIterator;

namespace Libcore {
namespace Utility {

/**
 * Our concrete TimeZone implementation, backed by zoneinfo data.
 *
 * @hide - used to implement TimeZone
 */
class CZoneInfoWallTime;
class COffsetIntervalHelper;

class ZoneInfo
    : public TimeZone
    , public IZoneInfo
{
protected:
    class OffsetInterval
        : public Object
        , public IOffsetInterval
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI ContainsWallTime(
            /* [in] */ Int64 wallTimeSeconds,
            /* [out] */ Boolean* result);

        CARAPI GetIsDst(
            /* [out] */ Int32* result);

        CARAPI GetTotalOffsetSeconds(
            /* [out] */ Int32* result);

        CARAPI GetEndWallTimeSeconds(
            /* [out] */ Int64* result);

        CARAPI GetStartWallTimeSeconds(
            /* [out] */ Int64* result);

        static CARAPI Create(
            /* [in] */ IZoneInfo* timeZone,
            /* [in] */ Int32 transitionIndex,
            /* [out] */ IOffsetInterval** result);

    private:
        OffsetInterval(
            /* [in] */ Int32 startWallTimeSeconds,
            /* [in] */ Int32 endWallTimeSeconds,
            /* [in] */ Int32 isDst,
            /* [in] */ Int32 totalOffsetSeconds);

        Int32 mStartWallTimeSeconds;
        Int32 mEndWallTimeSeconds;
        Int32 mIsDst;
        Int32 mTotalOffsetSeconds;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Returns the offset in milliseconds from UTC for this time zone at {@code
     * time}. The offset includes daylight savings time if the specified
     * date is within the daylight savings time period.
     *
     * @param time the date in milliseconds since January 1, 1970 00:00:00 UTC
     */
    CARAPI GetOffset(
        /* [in] */ Int64 time,
        /* [out] */ Int32* pOffset);

    /**
     * Returns the offset in milliseconds from UTC for this time zone at {@code
     * time}. The offset includes daylight savings time if the specified
     * date is within the daylight savings time period.
     *
     * @param time the date in milliseconds since January 1, 1970 00:00:00 UTC
     */
    CARAPI GetOffset(
            /* [in] */ Int32 era,
            /* [in] */ Int32 year,
            /* [in] */ Int32 month,
            /* [in] */ Int32 day,
            /* [in] */ Int32 dayOfWeek,
            /* [in] */ Int32 timeOfDayMillis,
            /* [out] */ Int32* pOffset);

    /**
     * Returns the offset in milliseconds from UTC of this time zone's standard
     * time.
     */
    CARAPI GetRawOffset(
            /* [out] */ Int32* pRawOffset);

    /**
     * Sets the offset in milliseconds from UTC of this time zone's standard
     * time.
     */
    CARAPI SetRawOffset(
        /* [in] */ Int32 offsetMillis);

    CARAPI GetDSTSavings(
        /* [out] */ Int32* pSavings);

    /**
     * Returns true if {@code time} is in a daylight savings time period for
     * this time zone.
     */
    CARAPI InDaylightTime(
        /* [in] */ IDate *when,
        /* [out] */ Boolean* value);

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
    CARAPI UseDaylightTime(
        /* [out] */ Boolean* value);

    /**
     * Returns true if {@code timeZone} has the same rules as this time zone.
     *
     * <p>The base implementation returns true if both time zones have the same
     * raw offset.
     */
    CARAPI HasSameRules(
        /* [in] */ ITimeZone* timeZone,
        /* [out] */ Boolean* value);

    CARAPI Clone(
            /* [out] */ ITimeZone** newObj);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* isEqual);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI Clone(
        /* [out] */ IInterface** newObj);

    static AutoPtr<IZoneInfo> MakeTimeZone(
        /* [in] */ const String& id,
        /* [in] */ IBufferIterator* it);

private:
    ZoneInfo(
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<Int32>* transitions,
        /* [in] */ ArrayOf<Byte>* type,
        /* [in] */ ArrayOf<Int32>* gmtoff,
        /* [in] */ ArrayOf<Byte>* isdst);

    static String FormatTime(
        /* [in] */ Int32 s,
        /* [in] */ ITimeZone* tz);

    static CARAPI CheckedAdd(
        /* [in] */ Int32 a,
        /* [in] */ Int32 b,
        /* [out] */ Int32* result);

    static CARAPI CheckedSubtract(
        /* [in] */ Int32 a,
        /* [in] */ Int32 b,
        /* [out] */ Int32* result);

private:
    static const Int64 MILLISECONDS_PER_DAY = 24 * 60 * 60 * 1000;
    static const Int64 MILLISECONDS_PER_400_YEARS =
            MILLISECONDS_PER_DAY * (400 * 365 + 100 - 3);

    static const Int64 UNIX_OFFSET = 62167219200000LL;

    static const Int32 NORMAL[];
    static const Int32 LEAP[];

private:
    Int32 mRawOffset;
    Int32 mEarliestRawOffset;

    AutoPtr<ArrayOf<Int32> > mTransitions;
    AutoPtr<ArrayOf<Int32> > mOffsets;
    AutoPtr<ArrayOf<Byte> > mTypes;
    AutoPtr<ArrayOf<Byte> > mIsDsts;
    Boolean mUseDst;
    Int32 mDstSavings; // Implements TimeZone.getDSTSavings.

    friend class CZoneInfoWallTime;
    friend class COffsetIntervalHelper;
};

} // namespace Utility
} // namespace Libcore

#endif //__LIBCORE_UTILITY_CZONEINFO_H__
