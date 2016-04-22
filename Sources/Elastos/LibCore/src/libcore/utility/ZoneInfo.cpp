
#include "ZoneInfo.h"
#include "Arrays.h"
#include "CFormatter.h"
#include "CInteger32.h"
#include "CBoolean.h"
#include "CString.h"
#include "CDate.h"
#include "text/CSimpleDateFormat.h"
#include "CSystem.h"
#include <Math.h>
// #include <assert.h>

namespace Libcore {
namespace Utility {

using Elastos::Core::Math;
using Elastos::Core::ISystem;
using Elastos::Core::IInteger32;
using Elastos::Core::IBoolean;
using Elastos::Core::CInteger32;
using Elastos::Core::CBoolean;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Utility::CDate;
using Elastos::Text::IDateFormat;
using Elastos::Utility::Arrays;
using Elastos::Utility::IFormatter;
using Elastos::Utility::CFormatter;
using Elastos::Core::CSystem;

CAR_INTERFACE_IMPL(ZoneInfo::OffsetInterval, Object, IOffsetInterval)

ECode ZoneInfo::OffsetInterval::ContainsWallTime(
    /* [in] */ Int64 wallTimeSeconds,
    /* [out] */ Boolean* result)
{
    *result = wallTimeSeconds >= mStartWallTimeSeconds && wallTimeSeconds < mEndWallTimeSeconds;
    return NOERROR;
}

ECode ZoneInfo::OffsetInterval::GetIsDst(
    /* [out] */ Int32* result)
{
    *result = mIsDst;
    return NOERROR;
}

ECode ZoneInfo::OffsetInterval::GetTotalOffsetSeconds(
    /* [out] */ Int32* result)
{
    *result = mTotalOffsetSeconds;
    return NOERROR;
}

ECode ZoneInfo::OffsetInterval::GetEndWallTimeSeconds(
    /* [out] */ Int64* result)
{
    *result = mEndWallTimeSeconds;
    return NOERROR;
}

ECode ZoneInfo::OffsetInterval::GetStartWallTimeSeconds(
    /* [out] */ Int64* result)
{
    *result = mStartWallTimeSeconds;
    return NOERROR;
}

ZoneInfo::OffsetInterval::OffsetInterval(
    /* [in] */ Int32 startWallTimeSeconds,
    /* [in] */ Int32 endWallTimeSeconds,
    /* [in] */ Int32 isDst,
    /* [in] */ Int32 totalOffsetSeconds)
{
    mStartWallTimeSeconds = startWallTimeSeconds;
    mEndWallTimeSeconds = endWallTimeSeconds;
    mIsDst = isDst;
    mTotalOffsetSeconds = totalOffsetSeconds;
}

ECode ZoneInfo::OffsetInterval::Create(
    /* [in] */ IZoneInfo* timeZone,
    /* [in] */ Int32 transitionIndex,
    /* [out] */ IOffsetInterval** result)
{
    ZoneInfo* zoneInfo = (ZoneInfo*)timeZone;
    if (transitionIndex < -1 || transitionIndex >= zoneInfo->mTransitions->GetLength()) {
        *result = NULL;
        return NOERROR;
    }

    Int32 rawOffsetSeconds = zoneInfo->mRawOffset / 1000;
    if (transitionIndex == -1) {
        Int32 endWallTimeSeconds;
        CheckedAdd((*zoneInfo->mTransitions)[0], rawOffsetSeconds, &endWallTimeSeconds);
        *result = new OffsetInterval(Math::INT32_MIN_VALUE, endWallTimeSeconds, 0 /* isDst */,
                rawOffsetSeconds);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    Byte type = (*zoneInfo->mTypes)[transitionIndex];
    Int32 totalOffsetSeconds = (*zoneInfo->mOffsets)[type] + rawOffsetSeconds;
    Int32 endWallTimeSeconds;
    if (transitionIndex == zoneInfo->mTransitions->GetLength() - 1) {
        // If this is the last transition, make up the end time.
        endWallTimeSeconds = Math::INT32_MAX_VALUE;
    } else {
        CheckedAdd((*zoneInfo->mTransitions)[transitionIndex + 1],
                totalOffsetSeconds, &endWallTimeSeconds);
    }
    Int32 isDst = (*zoneInfo->mIsDsts)[type];
    Int32 startWallTimeSeconds;
    CheckedAdd((*zoneInfo->mTransitions)[transitionIndex], totalOffsetSeconds, &startWallTimeSeconds);
    *result = new OffsetInterval(
            startWallTimeSeconds, endWallTimeSeconds, isDst, totalOffsetSeconds);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

const Int64 ZoneInfo::MILLISECONDS_PER_DAY;
const Int64 ZoneInfo::MILLISECONDS_PER_400_YEARS;
const Int64 ZoneInfo::UNIX_OFFSET;

const Int32 ZoneInfo::NORMAL[] = {
        0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334,
};

const Int32 ZoneInfo::LEAP[] = {
        0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335,
};

CAR_INTERFACE_IMPL(ZoneInfo, TimeZone, IZoneInfo)

ZoneInfo::ZoneInfo(
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<Int32>* transitions,
    /* [in] */ ArrayOf<Byte>* type,
    /* [in] */ ArrayOf<Int32>* gmtOffsets,
    /* [in] */ ArrayOf<Byte>* isDsts)
{
    mTransitions = transitions;
    mTypes = type;
    mOffsets = gmtOffsets;
    mIsDsts = isDsts;
    SetID(name);

    // Find the latest GMT and non-GMT offsets for their abbreviations
    Int32 lastStd = 0;
    Boolean haveStd = FALSE;
    Int32 lastDst = 0;
    Boolean haveDst = FALSE;
    for (Int32 i = mTransitions->GetLength() - 1;
            (!haveStd || !haveDst) && i >= 0; --i) {
        Int32 type = (*mTypes)[i] & 0xff;
        if (!haveStd && (*mIsDsts)[type] == 0) {
            haveStd = TRUE;
            lastStd = i;
        }
        if (!haveDst && (*mIsDsts)[type] != 0) {
            haveDst = TRUE;
            lastDst = i;
        }
    }

    // Use the latest non-daylight offset (if any) as the raw offset.
    if (lastStd >= mTypes->GetLength()) {
        mRawOffset = (*mOffsets)[0];
    }
    else {
        mRawOffset = (*mOffsets)[(*mTypes)[lastStd] & 0xff];
    }

    // Use the latest transition's pair of offsets to compute the DST savings.
    // This isn't generally useful, but it's exposed by TimeZone.getDSTSavings.
    if (lastDst >= mTypes->GetLength()) {
        mDstSavings = 0;
    }
    else {
        mDstSavings = Math::Abs((*mOffsets)[(*mTypes)[lastStd] & 0xff] - (*mOffsets)[(*mTypes)[lastDst] & 0xff]) * 1000;
    }

    // Cache the oldest known raw offset, in case we're asked about times that predate our
    // transition data.
    Int32 firstStd = -1;
    for (Int32 i = 0; i < mTransitions->GetLength(); ++i) {
        if ((*mIsDsts)[(*mTypes)[i] & 0xff] == 0) {
            firstStd = i;
            break;
        }
    }
    Int32 earliestRawOffset = (firstStd != -1) ?
            (*mOffsets)[(*mTypes)[firstStd] & 0xff] : mRawOffset;

    // Rather than keep offsets from UTC, we use offsets from local time, so the raw offset
    // can be changed and automatically affect all the offsets.
    for (Int32 i = 0; i < mOffsets->GetLength(); i++) {
        (*mOffsets)[i] -= mRawOffset;
    }

    // Is this zone still observing DST?
    // We don't care if they've historically used it: most places have at least once.
    // We want to know whether the last "schedule info" (the unix times in the mTransitions
    // array) is in the future. If it is, DST is still relevant.
    // See http://code.google.com/p/android/issues/detail?id=877.
    // This test means that for somewhere like Morocco, which tried DST in 2009 but has
    // no future plans (and thus no future schedule info) will report "true" from
    // useDaylightTime at the start of 2009 but "false" at the end. This seems appropriate.

    AutoPtr<CSystem> system;
    Elastos::Core::CSystem::AcquireSingletonByFriend((CSystem**)&system);

    Boolean usesDst = FALSE;
    Int64 currentUnixTime;
    system->GetCurrentTimeMillis(&currentUnixTime);
    currentUnixTime = currentUnixTime / 1000;
    if (mTransitions->GetLength() > 0) {
        // (We're really dealing with uint32_t values, so long is most convenient in Java.)
        Int64 latestScheduleTime = ((Int64) (*mTransitions)[mTransitions->GetLength() - 1]) & 0xffffffff;
        if (currentUnixTime < latestScheduleTime) {
            usesDst = TRUE;
        }
    }
    mUseDst = usesDst;

    // tzdata uses seconds, but Java uses milliseconds.
    mRawOffset *= 1000;
    mEarliestRawOffset = earliestRawOffset * 1000;
}

AutoPtr<IZoneInfo> ZoneInfo::MakeTimeZone(
    /* [in] */ const String& id,
    /* [in] */ IBufferIterator* it)
{
    // Variable names beginning tzh_ correspond to those in "tzfile.h".

    // Check tzh_magic.
    Int32 iValue;
    it->ReadInt32(&iValue);
    if (iValue != 0x545a6966) { // "TZif"
        return NULL;
    }

    // Skip the uninteresting part of the header.
    it->Skip(28);

    // Read the sizes of the arrays we're about to read.
    Int32 tzh_timecnt;
    it->ReadInt32(&tzh_timecnt);
    Int32 tzh_typecnt;
    it->ReadInt32(&tzh_typecnt);

    it->Skip(4); // Skip tzh_charcnt.

    AutoPtr<ArrayOf<Int32> > transitions = ArrayOf<Int32>::Alloc(tzh_timecnt);
    it->ReadInt32Array(transitions, 0, transitions->GetLength());

    AutoPtr<ArrayOf<Byte> > type = ArrayOf<Byte>::Alloc(tzh_timecnt);
    it->ReadByteArray(type, 0, type->GetLength());

    AutoPtr<ArrayOf<Int32> > gmtOffsets = ArrayOf<Int32>::Alloc(tzh_typecnt);
    AutoPtr<ArrayOf<Byte> > isDsts = ArrayOf<Byte>::Alloc(tzh_typecnt);
    for (Int32 i = 0; i < tzh_typecnt; ++i) {
        Int32 iTmp;
        it->ReadInt32(&iTmp);
        (*gmtOffsets)[i] = iTmp;
        Byte bTmp;
        it->ReadByte(&bTmp);
        (*isDsts)[i] = iTmp;
        // We skip the abbreviation index. This would let us provide historically-accurate
        // time zone abbreviations (such as "AHST", "YST", and "AKST" for standard time in
        // America/Anchorage in 1982, 1983, and 1984 respectively). ICU only knows the current
        // names, though, so even if we did use this data to provide the correct abbreviations
        // for en_US, we wouldn't be able to provide correct abbreviations for other locales,
        // nor would we be able to provide correct long forms (such as "Yukon Standard Time")
        // for any locale. (The RI doesn't do any better than us here either.)
        it->Skip(1);
    }

    AutoPtr<IZoneInfo> rst;
    return new ZoneInfo(id, transitions, type, gmtOffsets, isDsts);
}

ECode ZoneInfo::GetOffset(
    /* [in] */ Int32 era,
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 day,
    /* [in] */ Int32 dayOfWeek,
    /* [in] */ Int32 millis,
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);

    // XXX This assumes Gregorian always; Calendar switches from
    // Julian to Gregorian in 1582.  What calendar system are the
    // arguments supposed to come from?

    Int64 calc = (year / 400) * MILLISECONDS_PER_400_YEARS;
    year %= 400;

    calc += year * (365 * MILLISECONDS_PER_DAY);
    calc += ((year + 3) / 4) * MILLISECONDS_PER_DAY;

    if (year > 0)
        calc -= ((year - 1) / 100) * MILLISECONDS_PER_DAY;

    Boolean isLeap = (year == 0 || (year % 4 == 0 && year % 100 != 0));
    const Int32* mlen = isLeap ? LEAP : NORMAL;

    calc += mlen[month] * MILLISECONDS_PER_DAY;
    calc += (day - 1) * MILLISECONDS_PER_DAY;
    calc += millis;

    calc -= mRawOffset;
    calc -= UNIX_OFFSET;

    return GetOffset(calc, offset);
    return NOERROR;
}

ECode ZoneInfo::GetOffset(
    /* [in] */ Int64 when,
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);

    Int32 ix = (Int32)(when / 1000);
    Int32 transition;
    FAIL_RETURN(Arrays::BinarySearch(mTransitions, ix, &transition))
    if (transition < 0) {
        transition = ~transition - 1;
        if (transition < 0) {
            // Assume that all times before our first transition correspond to the
            // oldest-known non-daylight offset. The obvious alternative would be to
            // use the current raw offset, but that seems like a greater leap of faith.
            *offset = mEarliestRawOffset;
            return NOERROR;
        }
    }

    *offset = mRawOffset + (*mOffsets)[(*mTypes)[transition] & 0xff] * 1000;
    return NOERROR;
}

ECode ZoneInfo::InDaylightTime(
    /* [in] */ IDate* time,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(time);
    VALIDATE_NOT_NULL(value);

    Int64 when;
    time->GetTime(&when);

    Int32 ix = (Int32)(when / 1000);
    Int32 transition;
    FAIL_RETURN(Arrays::BinarySearch(mTransitions, ix, &transition))
    if (transition < 0) {
        transition = ~transition - 1;
        if (transition < 0) {
            // Assume that all times before our first transition are non-daylight.
            // Transition data tends to start with a transition to daylight, so just
            // copying the first transition would assume the opposite.
            // http://code.google.com/p/android/issues/detail?id=14395
            *value = FALSE;
            return NOERROR;
        }
    }

    *value = (*mIsDsts)[(*mTypes)[transition] & 0xff] == 1;
    return NOERROR;
}

ECode ZoneInfo::GetRawOffset(
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);
     *offset = mRawOffset;
     return NOERROR;
}

ECode ZoneInfo::SetRawOffset(
    /* [in] */ Int32 off)
{
    mRawOffset = off;
    return NOERROR;
}

ECode ZoneInfo::GetDSTSavings(
    /* [out] */ Int32* savings)
{
    VALIDATE_NOT_NULL(savings);
    *savings = mUseDst ? mDstSavings: 0;
    return NOERROR;
}

ECode ZoneInfo::UseDaylightTime(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mUseDst;
    return NOERROR;
}

ECode ZoneInfo::HasSameRules(
    /* [in] */ ITimeZone* timeZone,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(timeZone);
    VALIDATE_NOT_NULL(value);

    *value = FALSE;

    if (!IZoneInfo::Probe(timeZone)) {
        return NOERROR;
    }

    ZoneInfo* other = (ZoneInfo*)timeZone;
    if (mUseDst != other->mUseDst) {
        return NOERROR;
    }

    if (!mUseDst) {
        *value = (mRawOffset == other->mRawOffset);
        return NOERROR;
    }

    if (mRawOffset != other->mRawOffset) {
        return NOERROR;
    }

    // Arrays.equals returns true if both arrays are null
    *value = Arrays::Equals(mOffsets, other->mOffsets)
            && Arrays::Equals(mIsDsts, other->mIsDsts)
            && Arrays::Equals(mTypes, other->mTypes)
            && Arrays::Equals(mTransitions, other->mTransitions);
    return NOERROR;
}

ECode ZoneInfo::Clone(
    /* [out] */ ITimeZone** newObj)
{
    VALIDATE_NOT_NULL(newObj);
    AutoPtr<ArrayOf<Int32> > offsets = mOffsets->Clone();
    for (Int32 i = 0; i < offsets->GetLength(); i++) {
        (*offsets)[i] += mRawOffset / 1000;
    }
    AutoPtr<ZoneInfo> info = new ZoneInfo(mID, mTransitions->Clone(), mTypes->Clone(), offsets, mIsDsts->Clone());
    info->mID = mID;
    *newObj = info;
    REFCOUNT_ADD(*newObj);
    return NOERROR;
}

ECode ZoneInfo::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual);
    *isEqual = FALSE;
    VALIDATE_NOT_NULL(other);

    IZoneInfo* zoneInfo = IZoneInfo::Probe(other);
    if (!zoneInfo)
        return NOERROR;

    String thisID, otherID;
    GetID(&thisID);
    ITimeZone* timeZone = ITimeZone::Probe(zoneInfo);
    if (!timeZone)
        return NOERROR;
    timeZone->GetID(&otherID);
    if (thisID != otherID) {
        return NOERROR;
    }

    return HasSameRules(timeZone, isEqual);
}

ECode ZoneInfo::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    const Int32 prime = 31;
    Int32 result = 1;
    String id;
    GetID(&id);
    result = prime * result + id.GetHashCode();
    result = prime * result + Arrays::GetHashCode(mOffsets);
    result = prime * result + Arrays::GetHashCode(mIsDsts);
    result = prime * result + mRawOffset;
    result = prime * result + Arrays::GetHashCode(mTransitions);
    result = prime * result + Arrays::GetHashCode(mTypes);
    result = prime * result + (mUseDst ? 1231 : 1237);
    *hashCode = result;
    return NOERROR;
}

ECode ZoneInfo::ToString(
    /* [out] */ String* str)
{
    StringBuilder sb;

    String id;
    GetID(&id);
    sb += "ZoneInfo[";
    sb += id;
    sb += ",mRawOffset=";
    sb += mRawOffset;
    sb += ",mUseDst=";
    sb += mUseDst;
    sb += ",mDstSavings=";
    sb += mDstSavings;
    sb += ",transitions=";
    sb += mTransitions->GetLength();
    sb += "]";
    *str = sb.ToString();
    return NOERROR;
}

String ZoneInfo::FormatTime(
    /* [in] */ Int32 s,
    /* [in] */ ITimeZone* tz)
{
    AutoPtr<ISimpleDateFormat> sdf;
    CSimpleDateFormat::New(String("EEE MMM dd HH:mm:ss yyyy zzz"), (ISimpleDateFormat**)&sdf);
    AutoPtr<IDateFormat> df = IDateFormat::Probe(sdf);
    df->SetTimeZone(tz);
    Int64 ms = ((Int64)s) * 1000LL;
    AutoPtr<IDate> date;
    CDate::New(ms, (IDate**)&date);
    String str;
    df->Format(date, &str);
    return str;
}

ECode ZoneInfo::CheckedAdd(
    /* [in] */ Int32 a,
    /* [in] */ Int32 b,
    /* [out] */ Int32* result)
{
    // Adapted from Guava IntMath.checkedAdd();
    Int64 tmp = (Int64) a + b;
    if (tmp != (Int32) tmp) {
        *result = 0;
        return E_CHECKED_ARITHMETIC_EXCEPTION;
    }
    *result = (Int32) tmp;
    return NOERROR;
}

ECode ZoneInfo::CheckedSubtract(
    /* [in] */ Int32 a,
    /* [in] */ Int32 b,
    /* [out] */ Int32* result)
{
    // Adapted from Guava IntMath.checkedSubtract();
    Int64 tmp = (Int64) a - b;
    if (tmp != (Int32) tmp) {
        return E_CHECKED_ARITHMETIC_EXCEPTION;
    }
    *result = (Int32) tmp;
    return NOERROR;
}

ECode ZoneInfo::Clone(
    /* [out] */ IInterface** newObj)
{
    VALIDATE_NOT_NULL(newObj);
    AutoPtr<ITimeZone> timeZone;
    FAIL_RETURN(Clone((ITimeZone**)&timeZone));
    *newObj = timeZone->Probe(EIID_IInterface);
    REFCOUNT_ADD(*newObj)
    return NOERROR;
}

} // Utility
} // Libcore
