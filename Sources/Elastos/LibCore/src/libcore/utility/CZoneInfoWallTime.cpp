#include "CZoneInfoWallTime.h"
#include "CGregorianCalendar.h"
#include "TimeZone.h"
#include "ZoneInfo.h"
#include "Math.h"
#include "CInteger32.h"
#include "Arrays.h"

using Elastos::Core::Math;
using Elastos::Core::CInteger32;
using Elastos::Utility::Arrays;
using Elastos::Utility::TimeZone;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::ICalendar;
using Elastos::Utility::CGregorianCalendar;

namespace Libcore {
namespace Utility {

CAR_OBJECT_IMPL(CZoneInfoWallTime)

CAR_INTERFACE_IMPL(CZoneInfoWallTime, Object, IZoneInfoWallTime)

CZoneInfoWallTime::CZoneInfoWallTime()
{
    CGregorianCalendar::New(FALSE, (IGregorianCalendar**)&mCalendar);
    AutoPtr<ITimeZone> timeZone;
    TimeZone::GetTimeZone(String("UTC"), (ITimeZone**)&timeZone);
    AutoPtr<ICalendar> cld = ICalendar::Probe(mCalendar);
    if (cld != NULL)
        cld->SetTimeZone(timeZone);
}

ECode CZoneInfoWallTime::Localtime(
    /* [in] */ Int32 timeSeconds,
    /* [in] */ IZoneInfo* zoneInfo)
{
    // try {
    ZoneInfo* zone = (ZoneInfo*)zoneInfo;
    if (zone == NULL) {
        return NOERROR;
    }
    Int32 offsetSeconds = zone->mRawOffset / 1000;

    // Find out the timezone DST state and adjustment.
    Byte isDst;
    if (zone->mTransitions->GetLength() == 0) {
        isDst = 0;
    } else {
        // transitionIndex can be in the range -1..zoneInfo.mTransitions.length - 1
        Int32 transitionIndex;
        FindTransitionIndex(zone, timeSeconds, &transitionIndex);
        if (transitionIndex < 0) {
            // -1 means timeSeconds is "before the first recorded transition". The first
            // recorded transition is treated as a transition from non-DST and the raw
            // offset.
            isDst = 0;
        } else {
            Byte transitionType = (*zone->mTypes)[transitionIndex];
            offsetSeconds += (*zone->mOffsets)[transitionType];
            isDst = (*zone->mIsDsts)[transitionType];
        }
    }

    // Perform arithmetic that might underflow before setting fields.
    Int32 wallTimeSeconds;
    FAIL_RETURN(ZoneInfo::CheckedAdd(timeSeconds, offsetSeconds, &wallTimeSeconds));

    // Set fields.
    AutoPtr<ICalendar> cld = ICalendar::Probe(mCalendar);
    cld->SetTimeInMillis(wallTimeSeconds * 1000LL);
    CopyFieldsFromCalendar();
    mIsDst = isDst;
    mGmtOffsetSeconds = offsetSeconds;
    // } catch (CheckedArithmeticException e) {
        // Just stop, leaving fields untouched.
    // }
    return NOERROR;
}

ECode CZoneInfoWallTime::Mktime(
    /* [in] */ IZoneInfo* zoneInfo,
    /* [out] */ Int32* rst)
{
    VALIDATE_NOT_NULL(rst)
    *rst = -1;

    ZoneInfo* zone = (ZoneInfo*)zoneInfo;
    if (zone == NULL) {
        return NOERROR;
    }

    // Normalize isDst to -1, 0 or 1 to simplify isDst equality checks below.
    mIsDst = mIsDst > 0 ? 1 : mIsDst < 0 ? -1 : 0;

    CopyFieldsToCalendar();
    Int64 timeInMillis;
    AutoPtr<ICalendar> cld = ICalendar::Probe(mCalendar);
    cld->GetTimeInMillis(&timeInMillis);
    Int64 longWallTimeSeconds = timeInMillis  / 1000;
    if (Elastos::Core::Math::INT32_MIN_VALUE > longWallTimeSeconds
            || longWallTimeSeconds > Elastos::Core::Math::INT32_MAX_VALUE) {
        // For compatibility with the old native 32-bit implementation we must treat
        // this as an error. Note: -1 could be confused with a real time.
        return NOERROR;
    }

    // try {
        Int32 wallTimeSeconds =  (Int32) longWallTimeSeconds;
        Int32 rawOffsetSeconds = zone->mRawOffset / 1000;
        Int32 rawTimeSeconds;
        FAIL_RETURN(ZoneInfo::CheckedSubtract(wallTimeSeconds, rawOffsetSeconds, &rawTimeSeconds));

        if (zone->mTransitions->GetLength() == 0) {
            // There is no transition information. There is just a raw offset for all time.
            if (mIsDst > 0) {
                // Caller has asserted DST, but there is no DST information available.
                return NOERROR;
            }
            CopyFieldsFromCalendar();
            mIsDst = 0;
            mGmtOffsetSeconds = rawOffsetSeconds;
            *rst = rawTimeSeconds;
            return NOERROR;
        }

        // We cannot know for sure what instant the wall time will map to. Unfortunately, in
        // order to know for sure we need the timezone information, but to get the timezone
        // information we need an instant. To resolve this we use the raw offset to find an
        // OffsetInterval; this will get us the OffsetInterval we need or very close.

        // The initialTransition can be between -1 and (zoneInfo.mTransitions - 1). -1
        // indicates the rawTime is before the first transition and is handled gracefully by
        // createOffsetInterval().
        Int32 initialTransitionIndex;
        FindTransitionIndex(zoneInfo, rawTimeSeconds, &initialTransitionIndex);

        AutoPtr<IInteger32> result;
        if (mIsDst < 0) {
            // This is treated as a special case to get it out of the way:
            // When a caller has set isDst == -1 it means we can return the first match for
            // the wall time we find. If the caller has specified a wall time that cannot
            // exist this always returns -1.

            DoWallTimeSearch(zoneInfo, initialTransitionIndex,
                    wallTimeSeconds, TRUE /* mustMatchDst */, (IInteger32**)&result);
            if (result == NULL) {
                *rst = -1;
            } else {
                result->GetValue(rst);
            }
            return NOERROR;
        }

        // If the wall time asserts a DST (isDst == 0 or 1) the search is performed twice:
        // 1) The first attempts to find a DST offset that matches isDst exactly.
        // 2) If it fails, isDst is assumed to be incorrect and adjustments are made to see
        // if a valid wall time can be created. The result can be somewhat arbitrary.

        DoWallTimeSearch(zoneInfo, initialTransitionIndex, wallTimeSeconds,
                TRUE /* mustMatchDst */, (IInteger32**)&result);
        if (result == NULL) {
            DoWallTimeSearch(zoneInfo, initialTransitionIndex, wallTimeSeconds,
                    FALSE /* mustMatchDst */, (IInteger32**)&result);
        }
        if (result == NULL) {
            return NOERROR;
        }
        result->GetValue(rst);
        return NOERROR;
    // } catch (CheckedArithmeticException e) {
    // }
}

ECode CZoneInfoWallTime::SetYear(
    /* [in] */ Int32 year)
{
    mYear = year;
    return NOERROR;
}

ECode CZoneInfoWallTime::SetMonth(
    /* [in] */ Int32 month)
{
    mMonth = month;
    return NOERROR;
}

ECode CZoneInfoWallTime::SetMonthDay(
    /* [in] */ Int32 monthDay)
{
    mMonthDay = monthDay;
    return NOERROR;
}

ECode CZoneInfoWallTime::SetHour(
    /* [in] */ Int32 hour)
{
    mHour = hour;
    return NOERROR;
}

ECode CZoneInfoWallTime::SetMinute(
    /* [in] */ Int32 minute)
{
    mMinute = minute;
    return NOERROR;
}

ECode CZoneInfoWallTime::SetSecond(
    /* [in] */ Int32 second)
{
    mSecond = second;
    return NOERROR;
}

ECode CZoneInfoWallTime::SetWeekDay(
    /* [in] */ Int32 weekDay)
{
    mWeekDay = weekDay;
    return NOERROR;
}

ECode CZoneInfoWallTime::SetYearDay(
    /* [in] */ Int32 yearDay)
{
    mYearDay = yearDay;
    return NOERROR;
}

ECode CZoneInfoWallTime::SetIsDst(
    /* [in] */ Int32 isDst)
{
    mIsDst = isDst;
    return NOERROR;
}

ECode CZoneInfoWallTime::SetGmtOffset(
    /* [in] */ Int32 gmtOffsetSeconds)
{
    mGmtOffsetSeconds = gmtOffsetSeconds;
    return NOERROR;
}

ECode CZoneInfoWallTime::GetYear(
    /* [out] */ Int32* year)
{
    VALIDATE_NOT_NULL(year)
    *year = mYear;
    return NOERROR;
}

ECode CZoneInfoWallTime::GetMonth(
    /* [out] */ Int32* month)
{
    VALIDATE_NOT_NULL(month)
    *month = mMonth;
    return NOERROR;
}

ECode CZoneInfoWallTime::GetMonthDay(
    /* [out] */ Int32* monthDay)
{
    VALIDATE_NOT_NULL(monthDay)
    *monthDay = mMonthDay;
    return NOERROR;
}

ECode CZoneInfoWallTime::GetHour(
    /* [out] */ Int32* hour)
{
    VALIDATE_NOT_NULL(hour)
    *hour = mHour;
    return NOERROR;
}

ECode CZoneInfoWallTime::GetMinute(
    /* [out] */ Int32* minute)
{
    VALIDATE_NOT_NULL(minute)
    *minute = mMinute;
    return NOERROR;
}

ECode CZoneInfoWallTime::GetSecond(
    /* [out] */ Int32* second)
{
    VALIDATE_NOT_NULL(second)
    *second = mSecond;
    return NOERROR;
}

ECode CZoneInfoWallTime::GetWeekDay(
    /* [out] */ Int32* weekDay)
{
    VALIDATE_NOT_NULL(weekDay)
    *weekDay = mWeekDay;
    return NOERROR;
}

ECode CZoneInfoWallTime::GetYearDay(
    /* [out] */ Int32* yearDay)
{
    VALIDATE_NOT_NULL(yearDay)
    *yearDay = mYearDay;
    return NOERROR;
}

ECode CZoneInfoWallTime::GetIsDst(
    /* [out] */ Int32* isDst)
{
    VALIDATE_NOT_NULL(isDst)
    *isDst = mIsDst;
    return NOERROR;
}

ECode CZoneInfoWallTime::GetGmtOffset(
    /* [out] */ Int32* gmtOffsetSeconds)
{
    VALIDATE_NOT_NULL(gmtOffsetSeconds)
    *gmtOffsetSeconds = mGmtOffsetSeconds;
    return NOERROR;
}

ECode CZoneInfoWallTime::TryOffsetAdjustments(
    /* [in] */ IZoneInfo* zoneInfo,
    /* [in] */ Int32 oldWallTimeSeconds,
    /* [in] */ IOffsetInterval* targetInterval,
    /* [in] */ Int32 transitionIndex,
    /* [in] */ Int32 isDstToFind,
    /* [out] */ IInteger32** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (zoneInfo == NULL){
        return NOERROR;
    }

    AutoPtr<ArrayOf<Int32> > offsetsToTry;
    GetOffsetsOfType(zoneInfo, transitionIndex, isDstToFind, (ArrayOf<Int32>**)&offsetsToTry);
    for (Int32 j = 0; j < offsetsToTry->GetLength(); j++) {
        ZoneInfo* zone = (ZoneInfo*)zoneInfo;
        Int32 rawOffsetSeconds = zone->mRawOffset / 1000;
        Int32 jOffsetSeconds = rawOffsetSeconds + (*offsetsToTry)[j];
        Int32 targetIntervalOffsetSeconds;
        targetInterval->GetTotalOffsetSeconds(&targetIntervalOffsetSeconds);
        Int32 adjustmentSeconds = targetIntervalOffsetSeconds - jOffsetSeconds;
        Int32 adjustedWallTimeSeconds;
        FAIL_RETURN(ZoneInfo::CheckedAdd(oldWallTimeSeconds, adjustmentSeconds, &adjustedWallTimeSeconds))
        Boolean containsWallTime;
        targetInterval->ContainsWallTime(adjustedWallTimeSeconds, &containsWallTime);
        if (containsWallTime) {
            // Perform any arithmetic that might overflow.
            Int32 returnValue;
            FAIL_RETURN(ZoneInfo::CheckedSubtract(adjustedWallTimeSeconds,
                    targetIntervalOffsetSeconds, &returnValue))

            // Modify field state and return the result.
            AutoPtr<ICalendar> calendar = ICalendar::Probe(mCalendar);
            calendar->SetTimeInMillis(adjustedWallTimeSeconds * 1000LL);
            CopyFieldsFromCalendar();
            targetInterval->GetIsDst(&mIsDst);
            mGmtOffsetSeconds = targetIntervalOffsetSeconds;
            CInteger32::New(returnValue, result);
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CZoneInfoWallTime::DoWallTimeSearch(
    /* [in] */ IZoneInfo* zoneInfo,
    /* [in] */ Int32 initialTransitionIndex,
    /* [in] */ Int32 wallTimeSeconds,
    /* [in] */ Boolean mustMatchDst,
    /* [out] */ IInteger32** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    // The loop below starts at the initialTransitionIndex and radiates out from that point
    // up to 24 hours in either direction by applying transitionIndexDelta to inspect
    // adjacent transitions (0, -1, +1, -2, +2). 24 hours is used because we assume that no
    // total offset from UTC is ever > 24 hours. clampTop and clampBottom are used to
    // indicate whether the search has either searched > 24 hours or exhausted the
    // transition data in that direction. The search stops when a match is found or if
    // clampTop and clampBottom are both true.
    // The match logic employed is determined by the mustMatchDst parameter.
    Int32 MAX_SEARCH_SECONDS = 24 * 60 * 60;
    Boolean clampTop = FALSE, clampBottom = FALSE;
    Int32 loop = 0;
    do {
        // transitionIndexDelta = { 0, -1, 1, -2, 2,..}
        Int32 transitionIndexDelta = (loop + 1) / 2;
        if (loop % 2 == 1) {
            transitionIndexDelta *= -1;
        }
        loop++;

        // Only do any work in this iteration if we need to.
        if ((transitionIndexDelta > 0 && clampTop)
                || (transitionIndexDelta < 0 && clampBottom)) {
            continue;
        }

        // Obtain the OffsetInterval to use.
        Int32 currentTransitionIndex = initialTransitionIndex + transitionIndexDelta;
        AutoPtr<IOffsetInterval> offsetInterval;
        ZoneInfo::OffsetInterval::Create(zoneInfo, currentTransitionIndex, (IOffsetInterval**)&offsetInterval);
        if (offsetInterval == NULL) {
            // No transition exists with the index we tried: Stop searching in the
            // current direction.
            clampTop |= (transitionIndexDelta > 0);
            clampBottom |= (transitionIndexDelta < 0);
            continue;
        }

        // Match the wallTimeSeconds against the OffsetInterval.
        if (mustMatchDst) {
            // Work out if the interval contains the wall time the caller specified and
            // matches their isDst value.
            Boolean containsWallTime;
            offsetInterval->ContainsWallTime(wallTimeSeconds, &containsWallTime);
            if (containsWallTime) {
                Int32 isDst;
                offsetInterval->GetIsDst(&isDst);
                if (mIsDst == -1 || isDst == mIsDst) {
                    // This always returns the first OffsetInterval it finds that matches
                    // the wall time and isDst requirements. If this.isDst == -1 this means
                    // the result might be a DST or a non-DST answer for wall times that can
                    // exist in two OffsetIntervals.
                    Int32 totalOffsetSeconds;
                    offsetInterval->GetTotalOffsetSeconds(&totalOffsetSeconds);
                    Int32 returnValue;
                    FAIL_RETURN(ZoneInfo::CheckedSubtract(wallTimeSeconds,
                            totalOffsetSeconds, &returnValue))

                    CopyFieldsFromCalendar();
                    offsetInterval->GetIsDst(&mIsDst);
                    mGmtOffsetSeconds = totalOffsetSeconds;
                    CInteger32::New(returnValue, result);
                    return NOERROR;
                }
            }
        } else {
            // To retain similar behavior to the old native implementation: if the caller is
            // asserting the same isDst value as the OffsetInterval we are looking at we do
            // not try to find an adjustment from another OffsetInterval of the same isDst
            // type. If you remove this you get different results in situations like a
            // DST -> DST transition or STD -> STD transition that results in an interval of
            // "skipped" wall time. For example: if 01:30 (DST) is invalid and between two
            // DST intervals, and the caller has passed isDst == 1, this results in a -1
            // being returned.
            Int32 isDst;
            offsetInterval->GetIsDst(&isDst);
            if (mIsDst != isDst) {
                Int32 isDstToFind = mIsDst;
                TryOffsetAdjustments(zoneInfo, wallTimeSeconds,
                        offsetInterval, currentTransitionIndex, isDstToFind, result);
                if (*result != NULL) {
                    return NOERROR;
                }
            }
        }

        // See if we can avoid another loop in the current direction.
        if (transitionIndexDelta > 0) {
            // If we are searching forward and the OffsetInterval we have ends
            // > MAX_SEARCH_SECONDS after the wall time, we don't need to look any further
            // forward.
            Int64 seconds;
            offsetInterval->GetEndWallTimeSeconds(&seconds);
            Boolean endSearch = seconds - wallTimeSeconds > MAX_SEARCH_SECONDS;
            if (endSearch) {
                clampTop = TRUE;
            }
        } else if (transitionIndexDelta < 0) {
            Int64 seconds;
            offsetInterval->GetStartWallTimeSeconds(&seconds);
            Boolean endSearch = wallTimeSeconds - seconds >= MAX_SEARCH_SECONDS;
            if (endSearch) {
                // If we are searching backward and the OffsetInterval starts
                // > MAX_SEARCH_SECONDS before the wall time, we don't need to look any
                // further backwards.
                clampBottom = TRUE;
            }
        }
    } while (!(clampTop && clampBottom));
    *result = NULL;
    return NOERROR;
}

ECode CZoneInfoWallTime::CopyFieldsToCalendar()
{
    AutoPtr<ICalendar> calendar = ICalendar::Probe(mCalendar);
    calendar->Set(ICalendar::YEAR, mYear);
    calendar->Set(ICalendar::MONTH, mMonth);
    calendar->Set(ICalendar::DAY_OF_MONTH, mMonthDay);
    calendar->Set(ICalendar::HOUR_OF_DAY, mHour);
    calendar->Set(ICalendar::MINUTE, mMinute);
    calendar->Set(ICalendar::SECOND, mSecond);
    return NOERROR;
}

ECode CZoneInfoWallTime::CopyFieldsFromCalendar()
{
    AutoPtr<ICalendar> calendar = ICalendar::Probe(mCalendar);
    calendar->Get(ICalendar::YEAR, &mYear);
    calendar->Get(ICalendar::MONTH, &mMonth);
    calendar->Get(ICalendar::DAY_OF_MONTH, &mMonthDay);
    calendar->Get(ICalendar::HOUR_OF_DAY, &mHour);
    calendar->Get(ICalendar::MINUTE, &mMinute);
    calendar->Get(ICalendar::SECOND, &mSecond);

    // Calendar uses Sunday == 1. Android Time uses Sunday = 0.
    calendar->Get(ICalendar::DAY_OF_WEEK, &mWeekDay);
    mWeekDay -= 1;
    // Calendar enumerates from 1, Android Time enumerates from 0.
    calendar->Get(ICalendar::DAY_OF_YEAR, &mYearDay);
    mYearDay -= 1;
    return NOERROR;
}

ECode CZoneInfoWallTime::GetOffsetsOfType(
    /* [in] */ IZoneInfo* zoneInfo,
    /* [in] */ Int32 startIndex,
    /* [in] */ Int32 isDst,
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    // +1 to account for the synthetic transition we invent before the first recorded one.
    ZoneInfo* zone = (ZoneInfo*)zoneInfo;
    AutoPtr<ArrayOf<Int32> > offsets = ArrayOf<Int32>::Alloc(zone->mOffsets->GetLength() + 1);
    AutoPtr<ArrayOf<Boolean> > seen = ArrayOf<Boolean>::Alloc(zone->mOffsets->GetLength());
    Int32 numFound = 0;

    Int32 delta = 0;
    Boolean clampTop = FALSE;
    Boolean clampBottom = FALSE;
    do {
        // delta = { 1, -1, 2, -2, 3, -3...}
        delta *= -1;
        if (delta >= 0) {
            delta++;
        }

        Int32 transitionIndex = startIndex + delta;
        if (delta < 0 && transitionIndex < -1) {
            clampBottom = TRUE;
            continue;
        } else if (delta > 0 && transitionIndex >=  zone->mTypes->GetLength()) {
            clampTop = TRUE;
            continue;
        }

        if (transitionIndex == -1) {
            if (isDst == 0) {
                // Synthesize a non-DST transition before the first transition we have
                // data for.
                (*offsets)[numFound++] = 0; // offset of 0 from raw offset
            }
            continue;
        }
        Byte type = (*zone->mTypes)[transitionIndex];
        if (!(*seen)[type]) {
            if ((*zone->mIsDsts)[type] == isDst) {
                (*offsets)[numFound++] = (*zone->mOffsets)[type];
            }
            (*seen)[type] = TRUE;
        }
    } while (!(clampTop && clampBottom));

    AutoPtr<ArrayOf<Int32> > toReturn = ArrayOf<Int32>::Alloc(numFound);
    toReturn->Copy(0, offsets, 0, numFound);
    *result = toReturn;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CZoneInfoWallTime::FindTransitionIndex(
    /* [in] */ IZoneInfo* timeZone,
    /* [in] */ Int32 timeSeconds,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    ZoneInfo* zoneInfo = (ZoneInfo*)timeZone;
    Int32 matchingRawTransition;
    Arrays::BinarySearch(zoneInfo->mTransitions, timeSeconds, &matchingRawTransition);
    if (matchingRawTransition < 0) {
        matchingRawTransition = ~matchingRawTransition - 1;
    }
    *result = matchingRawTransition;
    return NOERROR;
}

} // namespace Libcore
} // namespace Utility