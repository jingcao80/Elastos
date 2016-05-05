
#include "CSimpleTimeZone.h"
#include "Grego.h"
#include "Calendar.h"
#include "CGregorianCalendar.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;

namespace Elastos{
namespace Utility{

const Int32 CSimpleTimeZone::DOM_MODE;
const Int32 CSimpleTimeZone::DOW_IN_MONTH_MODE;
const Int32 CSimpleTimeZone::DOW_GE_DOM_MODE;
const Int32 CSimpleTimeZone::DOW_LE_DOM_MODE;

CAR_INTERFACE_IMPL(CSimpleTimeZone, TimeZone, ISimpleTimeZone)

CAR_OBJECT_IMPL(CSimpleTimeZone)

CSimpleTimeZone::CSimpleTimeZone()
    : mRawOffset(0)
    , mStartYear(0)
    , mStartMonth(0)
    , mStartDay(0)
    , mStartDayOfWeek(0)
    , mStartTime(0)
    , mEndMonth(0)
    , mEndDay(0)
    , mEndDayOfWeek(0)
    , mEndTime(0)
    , mStartMode(0)
    , mEndMode(0)
    , mDstSavings(3600000)
    , mUseDaylight(FALSE)
{
}

ECode CSimpleTimeZone::constructor(
    /* [in] */ Int32 offset,
    /* [in] */ const String& name)
{
    FAIL_RETURN(SetID(name));
    mRawOffset = offset;
    return NOERROR;
}

ECode CSimpleTimeZone::constructor(
    /* [in] */ Int32 offset,
    /* [in] */ const String& name,
    /* [in] */ Int32 startMonth,
    /* [in] */ Int32 startDay,
    /* [in] */ Int32 startDayOfWeek,
    /* [in] */ Int32 startTime,
    /* [in] */ Int32 endMonth,
    /* [in] */ Int32 endDay,
    /* [in] */ Int32 endDayOfWeek,
    /* [in] */ Int32 endTime)
{
    return this->constructor(offset, name, startMonth,
        startDay, startDayOfWeek, startTime,
        endMonth, endDay, endDayOfWeek, endTime, 360000);
}

ECode CSimpleTimeZone::constructor(
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
    /* [in] */ Int32 daylightSavings)
{
    FAIL_RETURN(this->constructor(offset, name));

    if (daylightSavings <= 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mDstSavings = daylightSavings;

    mStartMonth = startMonth;
    mStartDay = startDay;
    mStartDayOfWeek = startDayOfWeek;
    mStartTime = startTime;
    FAIL_RETURN(SetStartMode());
    mEndMonth = endMonth;
    mEndDay = endDay;
    mEndDayOfWeek = endDayOfWeek;
    mEndTime = endTime;
    FAIL_RETURN(SetEndMode());
    return NOERROR;
}

ECode CSimpleTimeZone::constructor(
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
    /* [in] */ Int32 daylightSavings)
{
    FAIL_RETURN(this->constructor(offset, name, startMonth,
        startDay, startDayOfWeek, startTime,
        endMonth, endDay, endDayOfWeek, endTime, daylightSavings));
    mStartMode = startTimeMode;
    mEndMode = endTimeMode;
    return NOERROR;
}

ECode CSimpleTimeZone::Clone(
    /* [out] */ ITimeZone** newObj)
{
    VALIDATE_NOT_NULL(newObj);

    AutoPtr<ISimpleTimeZone> st;
    FAIL_RETURN(CSimpleTimeZone::New(mRawOffset, mID, (ISimpleTimeZone**)&st));

    CloneImpl(st);

    *newObj = ITimeZone::Probe(st);
    REFCOUNT_ADD(*newObj);
    return NOERROR;
}

ECode CSimpleTimeZone::Clone(
    /* [out] */ IInterface** newObj)
{
    VALIDATE_NOT_NULL(newObj);

    AutoPtr<ISimpleTimeZone> st;
    FAIL_RETURN(CSimpleTimeZone::New(mRawOffset, mID, (ISimpleTimeZone**)&st));

    CloneImpl(st);

    *newObj = TO_IINTERFACE(st);
    REFCOUNT_ADD(*newObj);
    return NOERROR;
}

ECode CSimpleTimeZone::CloneImpl(
        /* [in] */ ISimpleTimeZone* newObj)
{
    CSimpleTimeZone* st = (CSimpleTimeZone*)newObj;

    TimeZone::CloneImpl((ITimeZone*)st);

    st->mRawOffset = mRawOffset;
    st->mStartYear = mStartYear;
    st->mStartMonth = mStartMonth;
    st->mStartDay = mStartDay;
    st->mStartDayOfWeek = mStartDayOfWeek;
    st->mStartTime = mStartTime;
    st->mEndMonth = mEndMonth;
    st->mEndDay = mEndDay;
    st->mEndDayOfWeek = mEndDayOfWeek;
    st->mEndTime = mEndTime;
    st->mStartMode = mStartMode;
    st->mEndMode = mEndMode;
    st->mDstSavings = mDstSavings;
    st->mUseDaylight = mUseDaylight;

    return NOERROR;
}

ECode CSimpleTimeZone::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual);
    *isEqual = FALSE;
    VALIDATE_NOT_NULL(other);

    ISimpleTimeZone* istz = ISimpleTimeZone::Probe(other);
    if (!istz) {
        return NOERROR;
    }

    CSimpleTimeZone* tz = (CSimpleTimeZone*)istz;
    if (tz == this) {
        *isEqual = TRUE;
        return NOERROR;
    }

    *isEqual = mID == tz->mID && mRawOffset == tz->mRawOffset
            && mUseDaylight == tz->mUseDaylight
            && (!mUseDaylight || (mStartYear == tz->mStartYear
                && mStartMonth == tz->mStartMonth
                && mStartDay == tz->mStartDay && mStartMode == tz->mStartMode
                && mStartDayOfWeek == tz->mStartDayOfWeek
                && mStartTime == tz->mStartTime && mEndMonth == tz->mEndMonth
                && mEndDay == tz->mEndDay
                && mEndDayOfWeek == tz->mEndDayOfWeek
                && mEndTime == tz->mEndTime && mEndMode == tz->mEndMode
                && mDstSavings == tz->mDstSavings));
    return NOERROR;
}

ECode CSimpleTimeZone::GetDSTSavings(
    /* [out] */ Int32* pSavings)
{
    VALIDATE_NOT_NULL(pSavings);
    if (!mUseDaylight) {
        *pSavings = 0;
    }
    else {
        *pSavings = mDstSavings;
    }

    return NOERROR;
}

ECode CSimpleTimeZone::GetOffset(
    /* [in] */ Int32 era,
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 day,
    /* [in] */ Int32 dayOfWeek,
    /* [in] */ Int32 time,
    /* [out] */ Int32* pOffset)
{
    VALIDATE_NOT_NULL(pOffset);
    *pOffset = 0;

    if (era != IGregorianCalendar::BC && era != IGregorianCalendar::AD) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(CheckRange(month, dayOfWeek, time));
    if (month != ICalendar::FEBRUARY || day != 29 || !IsLeapYear(year)) {
        FAIL_RETURN(CheckDay(month, day));
    }

    Boolean isUsed;
    UseDaylightTime(&isUsed);
    if (!isUsed || era != IGregorianCalendar::AD || year < mStartYear) {
        *pOffset = mRawOffset;
        return NOERROR;
    }

    if (mEndMonth < mStartMonth) {
        if (month > mEndMonth && month < mStartMonth) {
            *pOffset = mRawOffset;
            return NOERROR;
        }
    }
    else {
        if (month < mStartMonth || month > mEndMonth) {
            *pOffset = mRawOffset;
            return NOERROR;
        }
    }

    Int32 ruleDay = 0, daysInMonth, firstDayOfMonth = Mod7(dayOfWeek - day);
    if (month == mStartMonth) {
        switch (mStartMode) {
            case DOM_MODE:
                ruleDay = mStartDay;
                break;
            case DOW_IN_MONTH_MODE:
                if (mStartDay >= 0) {
                    ruleDay = Mod7(mStartDayOfWeek - firstDayOfMonth) + 1
                            + (mStartDay - 1) * 7;
                }
                else {
                    daysInMonth = (*CGregorianCalendar::sDaysInMonth)[mStartMonth];
                    if (mStartMonth == ICalendar::FEBRUARY && IsLeapYear(
                            year)) {
                        daysInMonth += 1;
                    }
                    ruleDay = daysInMonth
                            + 1
                            + Mod7(mStartDayOfWeek
                            - (firstDayOfMonth + daysInMonth))
                            + mStartDay * 7;
                }
                break;
            case DOW_GE_DOM_MODE:
                ruleDay = mStartDay
                        + Mod7(mStartDayOfWeek
                        - (firstDayOfMonth + mStartDay - 1));
                break;
            case DOW_LE_DOM_MODE:
                ruleDay = mStartDay
                        + Mod7(mStartDayOfWeek
                        - (firstDayOfMonth + mStartDay - 1));
                if (ruleDay != mStartDay) {
                    ruleDay -= 7;
                }
                break;
        }
        if ((ruleDay > day) || (ruleDay == day && time < mStartTime)) {
            *pOffset = mRawOffset;
            return NOERROR;
        }
    }

    Int32 ruleTime = mEndTime - mDstSavings;
    Int32 nextMonth = (month + 1) % 12;
    if ((month == mEndMonth) || (ruleTime < 0 && nextMonth == mEndMonth)) {
        switch (mEndMode) {
            case DOM_MODE:
                ruleDay = mEndDay;
                break;
            case DOW_IN_MONTH_MODE:
                if (mEndDay >= 0) {
                    ruleDay = Mod7(mEndDayOfWeek - firstDayOfMonth) + 1
                            + (mEndDay - 1) * 7;
                }
                else {
                    daysInMonth = (*CGregorianCalendar::sDaysInMonth)[mEndMonth];
                    if (mEndMonth == ICalendar::FEBRUARY && IsLeapYear(year)) {
                        daysInMonth++;
                    }
                    ruleDay = daysInMonth
                            + 1
                            + Mod7(mEndDayOfWeek
                            - (firstDayOfMonth + daysInMonth)) + mEndDay
                            * 7;
                }
                break;
            case DOW_GE_DOM_MODE:
                ruleDay = mEndDay
                        + Mod7(
                        mEndDayOfWeek - (firstDayOfMonth + mEndDay - 1));
                break;
            case DOW_LE_DOM_MODE:
                ruleDay = mEndDay
                        + Mod7(
                        mEndDayOfWeek - (firstDayOfMonth + mEndDay - 1));
                if (ruleDay != mEndDay) {
                    ruleDay -= 7;
                }
                break;
        }

        Int32 ruleMonth = mEndMonth;
        if (ruleTime < 0) {
            int changeDays = 1 - (ruleTime / 86400000);
            ruleTime = (ruleTime % 86400000) + 86400000;
            ruleDay -= changeDays;
            if (ruleDay <= 0) {
                if (--ruleMonth < ICalendar::JANUARY) {
                    ruleMonth = ICalendar::DECEMBER;
                }
                ruleDay += (*CGregorianCalendar::sDaysInMonth)[ruleMonth];
                if (ruleMonth == ICalendar::FEBRUARY && IsLeapYear(year)) {
                    ruleDay++;
                }
            }
        }

        if (month == ruleMonth) {
            if ((ruleDay < day) || (ruleDay == day && time >= ruleTime)) {
                *pOffset = mRawOffset;
                return NOERROR;
            }
        }
        else if (nextMonth != ruleMonth) {
                *pOffset = mRawOffset;
                return NOERROR;
        }
    }

    *pOffset = mRawOffset + mDstSavings;
    return NOERROR;
}

ECode CSimpleTimeZone::GetOffset(
    /* [in] */ Int64 time,
    /* [out] */ Int32* pOffset)
{
    VALIDATE_NOT_NULL(pOffset);
    Boolean isUsed;
    UseDaylightTime(&isUsed);
    if (!isUsed) {
        *pOffset = mRawOffset;
        return NOERROR;
    }

    AutoPtr<ArrayOf<Int32> >fields = Grego::TimeToFields(time + mRawOffset, NULL);
    return GetOffset(IGregorianCalendar::AD, (*fields)[0], (*fields)[1], (*fields)[2],
            (*fields)[3], (*fields)[5], pOffset);
}

ECode CSimpleTimeZone::GetRawOffset(
    /* [out] */ Int32* pRawOffset)
{
    VALIDATE_NOT_NULL(pRawOffset);
    *pRawOffset = mRawOffset;
    return NOERROR;
}

ECode CSimpleTimeZone::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    String id;
    GetID(&id);
    *hashCode = id.GetHashCode() + mRawOffset;
    if (mUseDaylight) {
        *hashCode += mStartYear + mStartMonth + mStartDay + mStartDayOfWeek
            + mStartTime + mStartMode + mEndMonth + mEndDay + mEndDayOfWeek
            + mEndTime + mEndMode + mDstSavings;
    }
    return NOERROR;
}

ECode CSimpleTimeZone::HasSameRules(
    /* [in] */ ITimeZone* pTimeZone,
    /* [out] */ Boolean* pHasRules)
{
    VALIDATE_NOT_NULL(pHasRules);
    *pHasRules = FALSE;

    ISimpleTimeZone* istz = ISimpleTimeZone::Probe(pTimeZone);
    if (!istz) {
        return NOERROR;
    }
    CSimpleTimeZone* tz = (CSimpleTimeZone*)(istz);
    if (mUseDaylight!= tz->mUseDaylight) {
        return NOERROR;
    }

    if (!mUseDaylight) {
        *pHasRules = (mRawOffset == tz->mRawOffset);
        return NOERROR;
    }

    *pHasRules = (mRawOffset == tz->mRawOffset && mDstSavings == tz->mDstSavings
            && mStartYear == tz->mStartYear && mStartMonth == tz->mStartMonth
            && mStartDay == tz->mStartDay && mStartMode == tz->mStartMode
            && mStartDayOfWeek == tz->mStartDayOfWeek
            && mStartTime == tz->mStartTime && mEndMonth == tz->mEndMonth
            && mEndDay == tz->mEndDay && mEndDayOfWeek == tz->mEndDayOfWeek
            && mEndTime == tz->mEndTime && mEndMode == tz->mEndMode);
    return NOERROR;
}

ECode CSimpleTimeZone::InDaylightTime(
    /* [in] */ IDate* pTime,
    /* [out] */ Boolean* pIsIn)
{
    VALIDATE_NOT_NULL(pTime);
    VALIDATE_NOT_NULL(pIsIn);

    Boolean isUsed;
    UseDaylightTime(&isUsed);

    Int64 ti;
    pTime->GetTime(&ti);

    Int32 offset;
    GetOffset(ti, &offset);

    *pIsIn = isUsed && (offset != mRawOffset);
    return NOERROR;
}

Boolean CSimpleTimeZone::IsLeapYear(
    /* [in] */ Int32 year)
{
    if (year > 1582) {
        return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
    }
    return year % 4 == 0;
}

Int32 CSimpleTimeZone::Mod7(
    /* [in] */ Int32 num1)
{
    Int32 rem = num1 % 7;
    return (num1 < 0 && rem < 0) ? 7 + rem : rem;
}

ECode CSimpleTimeZone::SetDSTSavings(
    /* [in] */ Int32 milliseconds)
{
    if (milliseconds <= 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mDstSavings = milliseconds;
    return NOERROR;
}

ECode CSimpleTimeZone::CheckRange(
    /* [in] */ Int32 month,
    /* [in] */ Int32 dayOfWeek,
    /* [in] */ Int32 time)
{
    if (month < ICalendar::JANUARY || month > ICalendar::DECEMBER) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (dayOfWeek < ICalendar::SUNDAY || dayOfWeek > ICalendar::SATURDAY) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (time < 0 || time >= 24 * 3600000) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CSimpleTimeZone::CheckDay(
    /* [in] */ Int32 month,
    /* [in] */ Int32 day)
{
    if (day <= 0 || day > (*CGregorianCalendar::sDaysInMonth)[month]) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CSimpleTimeZone::SetEndMode()
{
    if (mEndDayOfWeek == 0) {
        mEndMode = DOM_MODE;
    }
    else if (mEndDayOfWeek < 0) {
        mEndDayOfWeek = -mEndDayOfWeek;
        if (mEndDay < 0) {
            mEndDay = -mEndDay;
            mEndMode = DOW_LE_DOM_MODE;
        }
        else {
            mEndMode = DOW_GE_DOM_MODE;
        }
    }
    else {
        mEndMode = DOW_IN_MONTH_MODE;
    }
    mUseDaylight = mStartDay != 0 && mEndDay != 0;
    if (mEndDay != 0) {
        FAIL_RETURN(CheckRange(mEndMonth, mEndMode == DOM_MODE ? 1 : mEndDayOfWeek,
                mEndTime));
        if (mEndMode != DOW_IN_MONTH_MODE) {
            FAIL_RETURN(CheckDay(mEndMonth, mEndDay));
        }
        else {
            if (mEndDay < -5 || mEndDay > 5) {
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
    }
    if (mEndMode != DOM_MODE) {
        mEndDayOfWeek--;
    }
    return NOERROR;
}

ECode CSimpleTimeZone::SetEndRule(
    /* [in] */ Int32 month,
    /* [in] */ Int32 dayOfMonth,
    /* [in] */ Int32 time)
{
    mEndMonth = month;
    mEndDay = dayOfMonth;
    mEndDayOfWeek = 0; // Initialize this value for hasSameRules()
    mEndTime = time;

    return SetEndMode();
}

ECode CSimpleTimeZone::SetEndRule(
    /* [in] */ Int32 month,
    /* [in] */ Int32 day,
    /* [in] */ Int32 dayOfWeek,
    /* [in] */ Int32 time)
{
    mEndMonth = month;
    mEndDay = day;
    mEndDayOfWeek = dayOfWeek;
    mEndTime = time;

    return SetEndMode();
}

ECode CSimpleTimeZone::SetEndRule(
    /* [in] */ Int32 month,
    /* [in] */ Int32 day,
    /* [in] */ Int32 dayOfWeek,
    /* [in] */ Int32 time,
    /* [in] */ Boolean after)
{
    mEndMonth = month;
    mEndDay = after ? day : -day;
    mEndDayOfWeek = -dayOfWeek;
    mEndTime = time;

    return SetEndMode();
}

ECode CSimpleTimeZone::SetRawOffset(
    /* [in] */ Int32 offsetMillis)
{
    mRawOffset = offsetMillis;
    return NOERROR;
}

ECode CSimpleTimeZone::SetStartMode()
{
    if (mStartDayOfWeek == 0) {
        mStartMode = DOM_MODE;
    }
    else if (mStartDayOfWeek < 0) {
        mStartDayOfWeek = -mStartDayOfWeek;
        if (mStartDay < 0) {
            mStartDay = -mStartDay;
            mStartMode = DOW_LE_DOM_MODE;
        }
        else {
            mStartMode = DOW_GE_DOM_MODE;
        }
    }
    else {
        mStartMode = DOW_IN_MONTH_MODE;
    }
    mUseDaylight = mStartDay != 0 && mEndDay != 0;
    if (mStartDay != 0) {
        FAIL_RETURN(CheckRange(mStartMonth, mStartMode == DOM_MODE ? 1 : mStartDayOfWeek,
                mStartTime));
        if (mStartMode != DOW_IN_MONTH_MODE) {
            FAIL_RETURN(CheckDay(mStartMonth, mStartDay));
        }
        else {
            if (mStartDay < -5 || mStartDay > 5) {
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
    }
    if (mStartMode != DOM_MODE) {
        mStartDayOfWeek--;
    }
    return NOERROR;
}

ECode CSimpleTimeZone::SetStartRule(
    /* [in] */ Int32 month,
    /* [in] */ Int32 dayOfMonth,
    /* [in] */ Int32 time)
{
    mStartMonth = month;
    mStartDay = dayOfMonth;
    mStartDayOfWeek = 0; // Initialize this value for hasSameRules()
    mStartTime = time;

    return SetStartMode();
}

ECode CSimpleTimeZone::SetStartRule(
    /* [in] */ Int32 month,
    /* [in] */ Int32 day,
    /* [in] */ Int32 dayOfWeek,
    /* [in] */ Int32 time)
{
    mStartMonth = month;
    mStartDay = day;
    mStartDayOfWeek = dayOfWeek;
    mStartTime = time;

    return SetStartMode();
}

ECode CSimpleTimeZone::SetStartRule(
    /* [in] */ Int32 month,
    /* [in] */ Int32 day,
    /* [in] */ Int32 dayOfWeek,
    /* [in] */ Int32 time,
    /* [in] */ Boolean after)
{
    mStartMonth = month;
    mStartDay = after ? day : -day;
    mStartDayOfWeek = -dayOfWeek;
    mStartTime = time;

    return SetStartMode();
}

ECode CSimpleTimeZone::SetStartYear(
    /* [in] */ Int32 year)
{
    mStartYear = year;
    mUseDaylight = TRUE;
    return NOERROR;
}

ECode CSimpleTimeZone::ToString(
        /* [out] */ String* str)
{
    StringBuilder sb("CSimpleTimeZone");
    sb += "[id=";
    String id;
    sb += (GetID(&id), id);
    sb += ",offset=";
    sb += mRawOffset;
    sb += ",dstSavings=";
    sb += mDstSavings;
    sb += ",useDaylight=";
    sb += mUseDaylight;
    sb += ",startYear=";
    sb += mStartYear;
    sb += ",startMode=";
    sb += mStartMode;
    sb += ",startMonth=";
    sb += mStartMonth;
    sb += ",startDay=";
    sb += mStartDay;
    sb += ",startDayOfWeek=";
    sb += (mUseDaylight && (mStartMode != DOM_MODE) ? mStartDayOfWeek + 1 : 0);
    sb += ",startTime=";
    sb += mStartTime;
    sb += ",endMode=";
    sb += mEndMode ;
    sb += ",endMonth=" ;
    sb += mEndMonth ;
    sb += ",endDay=" ;
    sb += mEndDay;
    sb += ",endDayOfWeek=";
    sb += (mUseDaylight && (mEndMode != DOM_MODE) ? mEndDayOfWeek + 1 : 0);
    sb += ",endTime=";
    sb += mEndTime;
    sb += "]";
    *str = sb.ToString();
    return NOERROR;
}

ECode CSimpleTimeZone::UseDaylightTime(
    /* [out] */ Boolean* pIsUsed)
{
    VALIDATE_NOT_NULL(pIsUsed);
    *pIsUsed = mUseDaylight;
    return NOERROR;
}

// private void writeObject(ObjectOutputStream stream) throws IOException {
//     int sEndDay = endDay, sEndDayOfWeek = endDayOfWeek + 1, sStartDay = startDay, sStartDayOfWeek = startDayOfWeek + 1;
//     if (useDaylight
//             && (startMode != DOW_IN_MONTH_MODE || endMode != DOW_IN_MONTH_MODE)) {
//         Calendar cal = new GregorianCalendar(this);
//         if (endMode != DOW_IN_MONTH_MODE) {
//             cal.set(Calendar.MONTH, endMonth);
//             cal.set(Calendar.DATE, endDay);
//             sEndDay = cal.get(Calendar.DAY_OF_WEEK_IN_MONTH);
//             if (endMode == DOM_MODE) {
//                 sEndDayOfWeek = cal.getFirstDayOfWeek();
//             }
//         }
//         if (startMode != DOW_IN_MONTH_MODE) {
//             cal.set(Calendar.MONTH, startMonth);
//             cal.set(Calendar.DATE, startDay);
//             sStartDay = cal.get(Calendar.DAY_OF_WEEK_IN_MONTH);
//             if (startMode == DOM_MODE) {
//                 sStartDayOfWeek = cal.getFirstDayOfWeek();
//             }
//         }
//     }
//     ObjectOutputStream.PutField fields = stream.putFields();
//     fields.put("dstSavings", dstSavings);
//     fields.put("endDay", sEndDay);
//     fields.put("endDayOfWeek", sEndDayOfWeek);
//     fields.put("endMode", endMode);
//     fields.put("endMonth", endMonth);
//     fields.put("endTime", endTime);
//     fields.put("monthLength", GregorianCalendar.DaysInMonth);
//     fields.put("rawOffset", rawOffset);
//     fields.put("serialVersionOnStream", 1);
//     fields.put("startDay", sStartDay);
//     fields.put("startDayOfWeek", sStartDayOfWeek);
//     fields.put("startMode", startMode);
//     fields.put("startMonth", startMonth);
//     fields.put("startTime", startTime);
//     fields.put("startYear", startYear);
//     fields.put("useDaylight", useDaylight);
//     stream.writeFields();
//     stream.writeInt(4);
//     byte[] values = new byte[4];
//     values[0] = (byte) startDay;
//     values[1] = (byte) (startMode == DOM_MODE ? 0 : startDayOfWeek + 1);
//     values[2] = (byte) endDay;
//     values[3] = (byte) (endMode == DOM_MODE ? 0 : endDayOfWeek + 1);
//     stream.write(values);
// }

// private void readObject(ObjectInputStream stream) throws IOException,
//         ClassNotFoundException {
//     ObjectInputStream.GetField fields = stream.readFields();
//     rawOffset = fields.get("rawOffset", 0);
//     useDaylight = fields.get("useDaylight", false);
//     if (useDaylight) {
//         endMonth = fields.get("endMonth", 0);
//         endTime = fields.get("endTime", 0);
//         startMonth = fields.get("startMonth", 0);
//         startTime = fields.get("startTime", 0);
//         startYear = fields.get("startYear", 0);
//     }
//     if (fields.get("serialVersionOnStream", 0) == 0) {
//         if (useDaylight) {
//             startMode = endMode = DOW_IN_MONTH_MODE;
//             endDay = fields.get("endDay", 0);
//             endDayOfWeek = fields.get("endDayOfWeek", 0) - 1;
//             startDay = fields.get("startDay", 0);
//             startDayOfWeek = fields.get("startDayOfWeek", 0) - 1;
//         }
//     } else {
//         dstSavings = fields.get("dstSavings", 0);
//         if (useDaylight) {
//             endMode = fields.get("endMode", 0);
//             startMode = fields.get("startMode", 0);
//             int length = stream.readInt();
//             byte[] values = new byte[length];
//             stream.readFully(values);
//             if (length >= 4) {
//                 startDay = values[0];
//                 startDayOfWeek = values[1];
//                 if (startMode != DOM_MODE) {
//                     startDayOfWeek--;
//                 }
//                 endDay = values[2];
//                 endDayOfWeek = values[3];
//                 if (endMode != DOM_MODE) {
//                     endDayOfWeek--;
//                 }
//             }
//         }
//     }
// }

} // namespace Utility
} // namespace Elastos
