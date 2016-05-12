
#include "CGregorianCalendar.h"
#include "TimeZone.h"
#include "CSystem.h"
#include <Math.h>
//#include <cutils/log.h>

using Elastos::Core::Math;
using Elastos::Core::ISystem;
using Elastos::Utility::ICalendar;

namespace Elastos{
namespace Utility{

const Int64 CGregorianCalendar::sDefaultGregorianCutover = -12219292800000ll;

static AutoPtr<ArrayOf<Byte> > InitDaysInMonth()
{
    AutoPtr<ArrayOf<Byte> > daysArray = ArrayOf<Byte>::Alloc(12);
    daysArray->Set(0, 31);
    daysArray->Set(1, 28);
    daysArray->Set(2, 31);
    daysArray->Set(3, 30);
    daysArray->Set(4, 31);
    daysArray->Set(5, 30);
    daysArray->Set(6, 31);
    daysArray->Set(7, 31);
    daysArray->Set(8, 30);
    daysArray->Set(9, 31);
    daysArray->Set(10, 30);
    daysArray->Set(11, 31);
    return daysArray;
}

AutoPtr<ArrayOf<Byte> > CGregorianCalendar::sDaysInMonth = InitDaysInMonth();

const Int32 CGregorianCalendar::sDaysInYear[12] =
    { 0, 31, 59, 90, 120, 151, 181,212, 243, 273, 304, 334 };

const Int32 CGregorianCalendar::sMaximums[17] =
    { 1, 292278994, 11, 53, 6, 31, 366, 7, 6, 1, 11, 23,
        59, 59, 999, 14 * 3600 * 1000, 7200000 };

const Int32 CGregorianCalendar::sMinimums[17] =
    { 0, 1, 0, 1, 0, 1, 1, 1, 1, 0,0, 0, 0, 0, 0, -13 * 3600 * 1000, 0 };

const Int32 CGregorianCalendar::sLeastMaximums[17] =
    { 1, 292269054, 11, 50, 3, 28, 355, 7, 3, 1, 11, 23,
        59, 59, 999, 50400000, 1200000 };

CAR_INTERFACE_IMPL(CGregorianCalendar, Calendar, IGregorianCalendar)

CAR_OBJECT_IMPL(CGregorianCalendar)

CGregorianCalendar::CGregorianCalendar()
    : mGregorianCutover(sDefaultGregorianCutover)
    , mChangeYear(1582)
    , mCurrentYearSkew(10)
    , mLastYearSkew(0)
{
    mJulianSkew = ((mChangeYear - 2000) / 400)
            + JulianError() - ((mChangeYear - 2000) / 100);
}

ECode CGregorianCalendar::constructor()
{
    AutoPtr<ITimeZone> timezone = TimeZone::GetDefault();
    AutoPtr<ILocale> locale = GetDefaultLocale();
    return this->constructor(timezone, locale);
}

ECode CGregorianCalendar::constructor(
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 day)
{
    AutoPtr<ITimeZone> timezone = TimeZone::GetDefault();
    AutoPtr<ILocale> locale = GetDefaultLocale();
    FAIL_RETURN(Calendar::constructor(timezone, locale));
    return Set(year, month, day);
}

ECode CGregorianCalendar::constructor(
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 day,
    /* [in] */ Int32 hour,
    /* [in] */ Int32 minute)
{
    AutoPtr<ITimeZone> timezone = TimeZone::GetDefault();
    AutoPtr<ILocale> locale = GetDefaultLocale();
    FAIL_RETURN(Calendar::constructor(timezone, locale));
    return Set(year, month, day, hour, minute);
}

ECode CGregorianCalendar::constructor(
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 day,
    /* [in] */ Int32 hour,
    /* [in] */ Int32 minute,
    /* [in] */ Int32 second)
{
    AutoPtr<ITimeZone> timezone = TimeZone::GetDefault();
    AutoPtr<ILocale> locale = GetDefaultLocale();
    FAIL_RETURN(Calendar::constructor(timezone, locale));
    return Set(year, month, day, hour, minute, second);
}

ECode CGregorianCalendar::constructor(
    /* [in] */ Int64 milliseconds)
{
    FAIL_RETURN(this->constructor(FALSE));
    return SetTimeInMillis(milliseconds);
}

ECode CGregorianCalendar::constructor(
    /* [in] */ ILocale* locale)
{
    VALIDATE_NOT_NULL(locale);
    AutoPtr<ITimeZone> timezone = TimeZone::GetDefault();
    return this->constructor(timezone, locale);
}

ECode CGregorianCalendar::constructor(
    /* [in] */ ITimeZone* timeZone)
{
    VALIDATE_NOT_NULL(timeZone);
    AutoPtr<ILocale> locale = GetDefaultLocale();
    return this->constructor(timeZone, locale);
}

ECode CGregorianCalendar::constructor(
    /* [in] */ ITimeZone* timeZone,
    /* [in] */ ILocale* locale)
{
    VALIDATE_NOT_NULL(timeZone);
    VALIDATE_NOT_NULL(locale);
    FAIL_RETURN(Calendar::constructor(timeZone, locale));

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 millis;
    system->GetCurrentTimeMillis(&millis);
    return SetTimeInMillis(millis);
}

ECode CGregorianCalendar::constructor(
    /* [in] */ Boolean ignored)
{
    AutoPtr<ITimeZone> tz = TimeZone::GetDefault();
    FAIL_RETURN(Calendar::constructor(tz));
    SetFirstDayOfWeek(SUNDAY);
    return SetMinimalDaysInFirstWeek(1);
}

ECode CGregorianCalendar::Add(
    /* [in] */ Int32 field,
    /* [in] */ Int32 value)
{
    if (value == 0) {
        return NOERROR;
    }
    if (field < 0 || field >= ZONE_OFFSET) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (field == ERA) {
        Complete();
        if ((*mFields)[ERA] == IGregorianCalendar::AD) {
            if (value >= 0) {
                return NOERROR;
            }
            Set(ERA, IGregorianCalendar::BC);
        }
        else {
            if (value <= 0) {
                return NOERROR;
            }
            Set(ERA, IGregorianCalendar::AD);
        }
        Complete();
        return NOERROR;
    }

    if (field == YEAR || field == MONTH) {
        Complete();
        if (field == MONTH) {
            Int32 month = (*mFields)[MONTH] + value;
            if (month < 0) {
                value = (month - 11) / 12;
                month = 12 + (month % 12);
            }
            else {
                value = month / 12;
            }
            Set(MONTH, month % 12);
        }
        Set(YEAR, (*mFields)[YEAR] + value);

        Boolean leapYear = IsLeapYear((*mFields)[YEAR]);
        Int32 days = DaysInMonth(leapYear, (*mFields)[MONTH]);
        if ((*mFields)[DATE] > days) {
            Set(DATE, days);
        }
        Complete();
        return NOERROR;
    }

    Int64 multiplier = 0;
    Int64 timeMillis;
    GetTimeInMillis(&timeMillis); // Update the time

    switch (field) {
        case MILLISECOND:
            mTime += value;
            break;
        case SECOND:
            mTime += value * 1000LL;
            break;
        case MINUTE:
            mTime += value * 60000LL;
            break;
        case HOUR:
        case HOUR_OF_DAY:
            mTime += value * 3600000LL;
            break;
        case AM_PM:
            multiplier = 43200000LL;
            break;
        case DATE:
        case DAY_OF_YEAR:
        case DAY_OF_WEEK:
            multiplier = 86400000LL;
            break;
        case WEEK_OF_YEAR:
        case WEEK_OF_MONTH:
        case DAY_OF_WEEK_IN_MONTH:
            multiplier = 604800000LL;
            break;
    }

    if (multiplier == 0) {
        mAreFieldsSet = false;
        Complete();
        return NOERROR;
    }

    Int64 delta = value * multiplier;
    AutoPtr<ITimeZone> timezone;
    GetTimeZone((ITimeZone**)&timezone);

    /*
     * Attempt to keep the hour and minute constant when we've crossed a DST
     * boundary and the user's units are AM_PM or larger. The typical
     * consequence is that calls to add(DATE, 1) will add 23, 24 or 25 hours
     * depending on whether the DST goes forward, constant, or backward.
     *
     * We know we've crossed a DST boundary if the new time will have a
     * different timezone offset. Adjust by adding the difference of the two
     * offsets. We don't adjust when doing so prevents the change from
     * crossing the boundary.
     */
    Int32 zoneOffset;
    timezone->GetRawOffset(&zoneOffset);
    Int32 offsetBefore = GetOffset(mTime + zoneOffset);
    Int32 offsetAfter = GetOffset(mTime + zoneOffset + delta);
    Int32 dstDelta = offsetBefore - offsetAfter;
    Int32 newOffset = GetOffset(mTime + zoneOffset + delta + dstDelta);
    if (newOffset == offsetAfter) {
        delta += dstDelta;
    }
    mTime += delta;
    mAreFieldsSet = FALSE;
    Complete();
    return NOERROR;
}

void CGregorianCalendar::FullFieldsCalc()
{
    Int32 millis = (Int32) (mTime % 86400000);
    Int64 days = mTime / 86400000;
    if (millis < 0) {
        millis += 86400000;
        days--;
    }

    // Adding fields[ZONE_OFFSET] to time might make it overflow, so we add
    // it to millis (the number of milliseconds in the current day) instead.
    millis += (*mFields)[ICalendar::ZONE_OFFSET];
    while (millis < 0) {
        millis += 86400000;
        days--;
    }
    while (millis >= 86400000) {
        millis -= 86400000;
        days++;
    }

    Int32 dayOfYear = ComputeYearAndDay(days, mTime + (*mFields)[ICalendar::ZONE_OFFSET]);
    (*mFields)[DAY_OF_YEAR] = dayOfYear;
    if ((*mFields)[YEAR] == mChangeYear && mGregorianCutover <= mTime + (*mFields)[ICalendar::ZONE_OFFSET]){
        dayOfYear += mCurrentYearSkew;
    }

    Int32 month = dayOfYear / 32;
    Boolean leapYear = IsLeapYear((*mFields)[YEAR]);
    Int32 date = dayOfYear - DaysInYear(leapYear, month);
    if (date > DaysInMonth(leapYear, month)) {
        date -= DaysInMonth(leapYear, month);
        month++;
    }
    (*mFields)[DAY_OF_WEEK] = Mod7(days - 3) + 1;
    Int32 dstOffset(0);
    if ((*mFields)[YEAR] <= 0) {
        dstOffset = 0;
    }
    else {
        AutoPtr<ITimeZone> tz;
        GetTimeZone((ITimeZone**)&tz);
        tz->GetOffset(IGregorianCalendar::AD,(*mFields)[YEAR], month, date,
                (*mFields)[DAY_OF_WEEK], millis, &dstOffset);
    }
    if ((*mFields)[YEAR] > 0) {
        dstOffset -= (*mFields)[ICalendar::ZONE_OFFSET];
    }
    (*mFields)[DST_OFFSET] = dstOffset;
    if (dstOffset != 0) {
        Int64 oldDays = days;
        millis += dstOffset;
        if (millis < 0) {
            millis += 86400000;
            days--;
        }
        else if (millis >= 86400000) {
            millis -= 86400000;
            days++;
        }
        if (oldDays != days) {
            dayOfYear = ComputeYearAndDay(days, mTime - (*mFields)[ICalendar::ZONE_OFFSET]
                    + dstOffset);
            (*mFields)[DAY_OF_YEAR] = dayOfYear;
            if((*mFields)[YEAR] == mChangeYear
             && mGregorianCutover <= mTime - (*mFields)[ICalendar::ZONE_OFFSET] + dstOffset){
                dayOfYear += mCurrentYearSkew;
            }
            month = dayOfYear / 32;
            leapYear = IsLeapYear((*mFields)[YEAR]);
            date = dayOfYear - DaysInYear(leapYear, month);
            if (date > DaysInMonth(leapYear, month)) {
                date -= DaysInMonth(leapYear, month);
                month++;
            }
            (*mFields)[DAY_OF_WEEK] = Mod7(days - 3) + 1;
        }
    }
    (*mFields)[MILLISECOND] = (millis % 1000);
    millis /= 1000;
    (*mFields)[SECOND] = (millis % 60);
    millis /= 60;
    (*mFields)[MINUTE] = (millis % 60);
    millis /= 60;
    (*mFields)[HOUR_OF_DAY] = (millis % 24);
    (*mFields)[AM_PM] = (*mFields)[HOUR_OF_DAY] > 11 ? 1 : 0;
    (*mFields)[HOUR] = (*mFields)[HOUR_OF_DAY] % 12;

    if ((*mFields)[YEAR] <= 0) {
        (*mFields)[ERA] = IGregorianCalendar::BC;
        (*mFields)[YEAR] = -(*mFields)[YEAR] + 1;
    }
    else {
        (*mFields)[ERA] = IGregorianCalendar::AD;
    }
    (*mFields)[MONTH] = month;
    (*mFields)[DATE] = date;
    (*mFields)[DAY_OF_WEEK_IN_MONTH] = (date - 1) / 7 + 1;
    Int32 firstDayOfWeek = GetFirstDayOfWeek();
    (*mFields)[WEEK_OF_MONTH] = (date - 1 + Mod7(days - date - 2
            - (firstDayOfWeek - 1))) / 7 + 1;
    Int32 daysFromStart = Mod7(days - 3 - ((*mFields)[DAY_OF_YEAR] - 1)
            - (firstDayOfWeek - 1));
    Int32 minimalDaysInFirstWeek;
    GetMinimalDaysInFirstWeek(&minimalDaysInFirstWeek);
    Int32 week = ((*mFields)[DAY_OF_YEAR] - 1 + daysFromStart) / 7
            + (7 - daysFromStart >= minimalDaysInFirstWeek ? 1 : 0);
    if (week == 0) {
        leapYear = IsLeapYear((*mFields)[YEAR] - 1);
        (*mFields)[WEEK_OF_YEAR] = 7 - Mod7(daysFromStart
                - (leapYear ? 2 : 1)) >= minimalDaysInFirstWeek ? 53
                : 52;
    }
    else if ((*mFields)[DAY_OF_YEAR] >= (leapYear ? 367 : 366)
            - Mod7(daysFromStart + (leapYear ? 2 : 1))) {
        (*mFields)[WEEK_OF_YEAR] = 7 - Mod7(daysFromStart + (leapYear ? 2 : 1)) >= minimalDaysInFirstWeek ? 1
                : week;
    }
    else {
        (*mFields)[WEEK_OF_YEAR] = week;
    }
}

ECode CGregorianCalendar::ComputeFields()
{
    AutoPtr<ITimeZone> timeZone;
    GetTimeZone((ITimeZone**)&timeZone);

    AutoPtr<IDate> date;
    CDate::New(mTime, (IDate**)&date);
    Boolean isInDaylightTime(FALSE);
    timeZone->InDaylightTime(date, &isInDaylightTime);
    Int32 savings = 0;
    timeZone->GetDSTSavings(&savings);
    Int32 dstOffset =  isInDaylightTime ? savings : 0;
    Int32 zoneOffset = 0;
    timeZone->GetRawOffset(&zoneOffset);


    // We unconditionally overwrite DST_OFFSET and ZONE_OFFSET with
    // values from the timezone that's currently in use. This gives us
    // much more consistent behavior, and matches ICU4J behavior (though
    // it is inconsistent with the RI).
    //
    // Anything callers can do with ZONE_OFFSET they can do by constructing
    // a SimpleTimeZone with the required offset.
    //
    // DST_OFFSET is a bit of a WTF, given that it's dependent on the rest
    // of the fields. There's no sensible reason we'd want to allow it to
    // be set, nor can we implement consistent full-fields calculation after
    // this field is set without maintaining a large deal of additional state.
    //
    // At the very least, we will need isSet to differentiate between fields
    // set by the user and fields set by our internal field calculation.
    (*mFields)[DST_OFFSET] = dstOffset;
    (*mFields)[ZONE_OFFSET] = zoneOffset;
    FullFieldsCalc();

    for (Int32 i = 0; i < FIELD_COUNT; i++) {
        (*mIsSet)[i] = TRUE;
    }

    return NOERROR;
}

ECode CGregorianCalendar::ComputeTime()
{
    Boolean isLenient;
    IsLenient(&isLenient);
    if (!isLenient) {
        if ((*mIsSet)[HOUR_OF_DAY]) {
            if ((*mFields)[HOUR_OF_DAY] < 0 || (*mFields)[HOUR_OF_DAY] > 23) {
                return E_INVALID_ARGUMENT;
            }
        }
        else if ((*mIsSet)[HOUR] && ((*mFields)[HOUR] < 0 || (*mFields)[HOUR] > 11)) {
            return E_INVALID_ARGUMENT;
        }
        if ((*mIsSet)[MINUTE] && ((*mFields)[MINUTE] < 0 || (*mFields)[MINUTE] > 59)) {
            return E_INVALID_ARGUMENT;
        }
        if ((*mIsSet)[SECOND] && ((*mFields)[SECOND] < 0 || (*mFields)[SECOND] > 59)) {
            return E_INVALID_ARGUMENT;
        }
        if ((*mIsSet)[MILLISECOND]
                && ((*mFields)[MILLISECOND] < 0 || (*mFields)[MILLISECOND] > 999)) {
            return E_INVALID_ARGUMENT;
        }
        if ((*mIsSet)[WEEK_OF_YEAR]
                && ((*mFields)[WEEK_OF_YEAR] < 1 || (*mFields)[WEEK_OF_YEAR] > 53)) {
            return E_INVALID_ARGUMENT;
        }
        if ((*mIsSet)[DAY_OF_WEEK]
                && ((*mFields)[DAY_OF_WEEK] < 1 || (*mFields)[DAY_OF_WEEK] > 7)) {
            return E_INVALID_ARGUMENT;
        }
        if ((*mIsSet)[DAY_OF_WEEK_IN_MONTH]
                && ((*mFields)[DAY_OF_WEEK_IN_MONTH] < 1 || (*mFields)[DAY_OF_WEEK_IN_MONTH] > 6)) {
            return E_INVALID_ARGUMENT;
        }
        if ((*mIsSet)[WEEK_OF_MONTH]
                && ((*mFields)[WEEK_OF_MONTH] < 1 || (*mFields)[WEEK_OF_MONTH] > 6)) {
            return E_INVALID_ARGUMENT;
        }
        if ((*mIsSet)[AM_PM] && (*mFields)[AM_PM] != AM && (*mFields)[AM_PM] != PM) {
            return E_INVALID_ARGUMENT;
        }
        if ((*mIsSet)[HOUR] && ((*mFields)[HOUR] < 0 || (*mFields)[HOUR] > 11)) {
            return E_INVALID_ARGUMENT;
        }
        if ((*mIsSet)[YEAR]) {
            if ((*mIsSet)[ERA] && (*mFields)[ERA] == BC
                    && ((*mFields)[YEAR] < 1 || (*mFields)[YEAR] > 292269054)) {
                return E_INVALID_ARGUMENT;
            }
            else if ((*mFields)[YEAR] < 1 || (*mFields)[YEAR] > 292278994) {
                return E_INVALID_ARGUMENT;
            }
        }
        if ((*mIsSet)[MONTH] && ((*mFields)[MONTH] < 0 || (*mFields)[MONTH] > 11)) {
            return E_INVALID_ARGUMENT;
        }
    }

    Int64 timeVal;
    Int64 hour = 0;
    if ((*mIsSet)[HOUR_OF_DAY] && mLastTimeFieldSet != HOUR) {
        hour = (*mFields)[HOUR_OF_DAY];
    }
    else if ((*mIsSet)[HOUR]) {
        hour = ((*mFields)[AM_PM] * 12) + (*mFields)[HOUR];
    }
    timeVal = hour * 3600000;

    if ((*mIsSet)[MINUTE]) {
        timeVal += ((Int64) (*mFields)[MINUTE]) * 60000;
    }
    if ((*mIsSet)[SECOND]) {
        timeVal += ((Int64) (*mFields)[SECOND]) * 1000;
    }
    if ((*mIsSet)[MILLISECOND]) {
        timeVal += (*mFields)[MILLISECOND];
    }

    Int64 days;
    Int32 year = (*mIsSet)[YEAR] ? (*mFields)[YEAR] : 1970;
    if ((*mIsSet)[ERA]) {
        // Always test for valid ERA, even if the Calendar is lenient
        if ((*mFields)[ERA] != BC && (*mFields)[ERA] != AD) {
            return E_INVALID_ARGUMENT;
        }
        if ((*mFields)[ERA] == BC) {
            year = 1 - year;
        }
    }

    Boolean weekMonthSet = (*mIsSet)[WEEK_OF_MONTH]
            || (*mIsSet)[DAY_OF_WEEK_IN_MONTH];
    Boolean useMonth = ((*mIsSet)[DATE] || (*mIsSet)[MONTH] || weekMonthSet)
            && mLastDateFieldSet != DAY_OF_YEAR;
    if (useMonth
            && (mLastDateFieldSet == DAY_OF_WEEK || mLastDateFieldSet == WEEK_OF_YEAR)) {
        if ((*mIsSet)[WEEK_OF_YEAR] && (*mIsSet)[DAY_OF_WEEK]) {
            if (mLastDateFieldSet == WEEK_OF_YEAR) {
                useMonth = FALSE;
            } else if (mLastDateFieldSet == DAY_OF_WEEK) {
                // DAY_OF_WEEK belongs to both the Month + Week + Day and the
                // WeekOfYear + Day combinations. We're supposed to use the most
                // recent combination, as specified by the single set field. We can't
                // know for sure in this case, so we always prefer the week-month-day
                // combination if week-month is already set.
                useMonth = weekMonthSet;
            }
        }
        else if ((*mIsSet)[DAY_OF_YEAR]) {
            useMonth = (*mIsSet)[DATE] && (*mIsSet)[MONTH];
        }
    }

    if (useMonth) {
        Int32 month = (*mFields)[MONTH];
        year += month / 12;
        month %= 12;
        if (month < 0) {
            year--;
            month += 12;
        }
        Boolean leapYear = IsLeapYear(year);
        days = DaysFromBaseYear(year) + DaysInYear(leapYear, month);
        Boolean useDate = (*mIsSet)[DATE];
        if (useDate
                && (mLastDateFieldSet == DAY_OF_WEEK
                        || mLastDateFieldSet == WEEK_OF_MONTH || mLastDateFieldSet == DAY_OF_WEEK_IN_MONTH)) {
            useDate = !((*mIsSet)[DAY_OF_WEEK] && weekMonthSet);
        }
        if (useDate) {
            IsLenient(&isLenient);
            if (!isLenient
                    && ((*mFields)[DATE] < 1 || (*mFields)[DATE] > DaysInMonth(
                            leapYear, month))) {
                return E_INVALID_ARGUMENT;
            }
            days += (*mFields)[DATE] - 1;
        }
        else {
            Int32 dayOfWeek;
            if ((*mIsSet)[DAY_OF_WEEK]) {
                dayOfWeek = (*mFields)[DAY_OF_WEEK] - 1;
            }
            else {
                dayOfWeek = GetFirstDayOfWeek();
                dayOfWeek = dayOfWeek - 1;
            }
            if ((*mIsSet)[WEEK_OF_MONTH]
                    && mLastDateFieldSet != DAY_OF_WEEK_IN_MONTH) {
                Int32 fdow = GetFirstDayOfWeek();
                Int32 skew = Mod7(days - 3 - (fdow - 1));
                days += ((*mFields)[WEEK_OF_MONTH] - 1) * 7
                        + Mod7(skew + dayOfWeek - (days - 3)) - skew;
            }
            else if ((*mIsSet)[DAY_OF_WEEK_IN_MONTH]) {
                if ((*mFields)[DAY_OF_WEEK_IN_MONTH] >= 0) {
                    days += Mod7(dayOfWeek - (days - 3))
                            + ((*mFields)[DAY_OF_WEEK_IN_MONTH] - 1) * 7;
                }
                else {
                    days += DaysInMonth(leapYear, month)
                            + Mod7(dayOfWeek
                                    - (days + DaysInMonth(leapYear, month) - 3))
                            + (*mFields)[DAY_OF_WEEK_IN_MONTH] * 7;
                }
            }
            else if ((*mIsSet)[DAY_OF_WEEK]) {
                Int32 fdow = GetFirstDayOfWeek();
                Int32 skew = Mod7(days - 3 - (fdow - 1));
                days += Mod7(Mod7(skew + dayOfWeek - (days - 3)) - skew);
            }
        }
    }
    else {
        Boolean useWeekYear = (*mIsSet)[WEEK_OF_YEAR]
                && mLastDateFieldSet != DAY_OF_YEAR;
        if (useWeekYear && (*mIsSet)[DAY_OF_YEAR]) {
            useWeekYear = (*mIsSet)[DAY_OF_WEEK];
        }
        days = DaysFromBaseYear(year);
        if (useWeekYear) {
            Int32 dayOfWeek;
            if ((*mIsSet)[DAY_OF_WEEK]) {
                dayOfWeek = (*mFields)[DAY_OF_WEEK] - 1;
            }
            else {
                dayOfWeek = GetFirstDayOfWeek();
                dayOfWeek = dayOfWeek - 1;
            }

            Int32 fdow = GetFirstDayOfWeek();
            Int32 skew = Mod7(days - 3 - (fdow - 1));
            days += ((*mFields)[WEEK_OF_YEAR] - 1) * 7
                    + Mod7(skew + dayOfWeek - (days - 3)) - skew;
            Int32 mdifw;
            GetMinimalDaysInFirstWeek(&mdifw);
            if (7 - skew < mdifw) {
                days += 7;
            }
        }
        else if ((*mIsSet)[DAY_OF_YEAR]) {
            IsLenient(&isLenient);
            Boolean isLeapYear = IsLeapYear(year);
            if (!isLenient
                    && ((*mFields)[DAY_OF_YEAR] < 1 || (*mFields)[DAY_OF_YEAR] > (365 + (isLeapYear ? 1
                            : 0)))) {
                return E_INVALID_ARGUMENT;
            }
            days += (*mFields)[DAY_OF_YEAR] - 1;
        }
        else if ((*mIsSet)[DAY_OF_WEEK]) {
            days += Mod7((*mFields)[DAY_OF_WEEK] - 1 - (days - 3));
        }
    }
    mLastDateFieldSet = 0;

    timeVal += days * 86400000;
    // Use local time to compare with the gregorian change
    if (year == mChangeYear
            && timeVal >= mGregorianCutover + JulianError() * 86400000LL) {
        timeVal -= JulianError() * 86400000LL;
    }

    // It is not possible to simply subtract getOffset(timeVal) from timeVal
    // to get UTC.
    // The trick is needed for the moment when DST transition occurs,
    // say 1:00 is a transition time when DST offset becomes +1 hour,
    // then wall time in the interval 1:00 - 2:00 is invalid and is
    // treated as UTC time.
    AutoPtr<ITimeZone> tz;
    GetTimeZone((ITimeZone**)&tz);
    Int32 rawOffset;
    tz->GetRawOffset(&rawOffset);

    Int64 timeValWithoutDST = timeVal - GetOffset(timeVal) + rawOffset;
    timeVal -= GetOffset(timeValWithoutDST);
    // Need to update wall time in fields, since it was invalid due to DST
    // transition
    mTime = timeVal;
    if (timeValWithoutDST != timeVal) {
        ComputeFields();
        mAreFieldsSet = TRUE;
    }
    return NOERROR;
}

Int32 CGregorianCalendar::ComputeYearAndDay(
    /* [in] */ Int64 dayCount,
    /* [in] */ Int64 localTime)
{
    Int32 year = 1970;
    Int64 days = dayCount;
    if (localTime < mGregorianCutover) {
        days -= mJulianSkew;
    }
    Int32 approxYears;

    while ((approxYears = (Int32) (days / 365)) != 0) {
        year = year + approxYears;
        days = dayCount - DaysFromBaseYear(year);
    }
    if (days < 0) {
        year = year - 1;
        days = days + DaysInYear(year);
    }
    (*mFields)[YEAR] = year;
    return (Int32) days + 1;
}

Int64 CGregorianCalendar::DaysFromBaseYear(
    /* [in] */ Int64 year)
{
    if (year >= 1970) {
        Int64 days = (year - 1970) * 365 + ((year - 1969) / 4);
        if (year > mChangeYear) {
            days -= ((year - 1901) / 100) - ((year - 1601) / 400);
        }
        else {
            if(year == mChangeYear){
                days += mCurrentYearSkew;
            }
            else if(year == mChangeYear -1){
                days += mLastYearSkew;
            }
            else{
                days += mJulianSkew;
            }
        }
        return days;
    }
    else if (year <= mChangeYear) {
        return (year - 1970) * 365 + ((year - 1972) / 4) + mJulianSkew;
    }
    return (year - 1970) * 365 + ((year - 1972) / 4)
            - ((year - 2000) / 100) + ((year - 2000) / 400);
}

Int32 CGregorianCalendar::DaysInMonth()
{
    Boolean isLeapYear = IsLeapYear((*mFields)[YEAR]);
    return DaysInMonth(isLeapYear, (*mFields)[MONTH]);
}

Int32 CGregorianCalendar::DaysInMonth(
    /* [in] */ Boolean leapYear,
    /* [in] */ Int32 month)
{
    if (leapYear && month == FEBRUARY) {
        return (*sDaysInMonth)[month] + 1;
    }

    return (*sDaysInMonth)[month];
}

Int32 CGregorianCalendar::DaysInYear(
    /* [in] */ Int32 year)
{
    Boolean isLeapYear = IsLeapYear(year);
    Int32 daysInYear = isLeapYear ? 366 : 365;
    if (year == mChangeYear) {
        daysInYear -= mCurrentYearSkew;
    }
    if (year == mChangeYear - 1) {
        daysInYear -= mLastYearSkew;
    }
    return daysInYear;
}

Int32 CGregorianCalendar::DaysInYear(
    /* [in] */ Boolean leapYear,
    /* [in] */ Int32 month)
{
    if (leapYear && month > FEBRUARY) {
        return sDaysInYear[month] + 1;
    }
    return sDaysInYear[month];
}

ECode CGregorianCalendar::Equals(
    /* [in] */ ICalendar* other,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual);
    *isEqual = FALSE;
    VALIDATE_NOT_NULL(other);

    IGregorianCalendar*  gc = (IGregorianCalendar*)(other->Probe(EIID_IGregorianCalendar));
    if (!gc) {
        return NOERROR;
    }
    if ((CGregorianCalendar*)gc == this) {
        *isEqual = TRUE;
        return NOERROR;
    }
    Calendar::Equals(other, isEqual);
    if (*isEqual) {
        *isEqual = (mGregorianCutover == ((CGregorianCalendar*)gc)->mGregorianCutover);
    }
    return NOERROR;
}

ECode CGregorianCalendar::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual);
    *isEqual = FALSE;
    VALIDATE_NOT_NULL(other);

    CGregorianCalendar::Equals(ICalendar::Probe(other), isEqual);
    return NOERROR;
}

ECode CGregorianCalendar::GetActualMaximum(
    /* [in] */ Int32 field,
    /* [out] */ Int32* pValue)
{
    VALIDATE_NOT_NULL(pValue);
    Int32 value;
    if ((value = sMaximums[field]) == sLeastMaximums[field]) {
        *pValue = value;
        return NOERROR;
    }

    Complete();
    Int64 orgTime = mTime;
    Int32 result = 0;
    switch (field) {
        case WEEK_OF_YEAR:
            Set(DATE, 31);
            Set(MONTH, DECEMBER);
            Get(WEEK_OF_YEAR, &result);
            if (result == 1) {
                Set(DATE, 31 - 7);
                Get(WEEK_OF_YEAR, &result);
            }
            mAreFieldsSet = FALSE;
            break;

        case WEEK_OF_MONTH:
            Set(DATE, DaysInMonth());
            Get(WEEK_OF_MONTH, &result);
            mAreFieldsSet = FALSE;
            break;

        case DATE:
            *pValue = DaysInMonth();
            return NOERROR;

        case DAY_OF_YEAR:
            *pValue = DaysInYear((*mFields)[YEAR]);
            return NOERROR;

        case DAY_OF_WEEK_IN_MONTH:
            Int32 rtmp;
            Get(DAY_OF_WEEK_IN_MONTH, &result);
            Get(DATE, &rtmp);
            result += ((DaysInMonth()-rtmp) / 7);
            break;

        case YEAR:
        {
            AutoPtr<IInterface> inter;
            Clone((IInterface**)&inter);
            AutoPtr<IGregorianCalendar> clone = IGregorianCalendar::Probe(inter);

            Int32 ret;
            Get(ERA, &ret);
            if (ret == AD) {
                (ICalendar::Probe(clone))->SetTimeInMillis(Elastos::Core::Math::INT64_MAX_VALUE);
            }
            else {
                (ICalendar::Probe(clone))->SetTimeInMillis(Elastos::Core::Math::INT64_MIN_VALUE);
            }
            (ICalendar::Probe(clone))->Get(YEAR, &result);
            Int32 hretmp;
            Get(YEAR, &hretmp);
            (ICalendar::Probe(clone))->Set(YEAR, hretmp);
            Boolean isBefore;
            (ICalendar::Probe(clone))->IsBefore(this, &isBefore);
            if (isBefore) {
                result--;
            }
            break;
        }

        case DST_OFFSET:
            GetMaximum(DST_OFFSET, &result);
            break;
    }

    mTime = orgTime;
    *pValue = result;
    return NOERROR;
}

ECode CGregorianCalendar::GetActualMinimum(
    /* [in] */ Int32 field,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    return GetMinimum(field, value);
}

ECode CGregorianCalendar::GetGreatestMinimum(
    /* [in] */ Int32 field,
    /* [out] */ Int32* minimum)
{
    VALIDATE_NOT_NULL(minimum);
    *minimum = sMinimums[field];
    return NOERROR;
}

ECode CGregorianCalendar::GetGregorianChange(
    /* [out] */ IDate** date)
{
    VALIDATE_NOT_NULL(date);
    return CDate::New(mGregorianCutover, date);
}

ECode CGregorianCalendar::GetLeastMaximum(
    /* [in] */ Int32 field,
    /* [out] */ Int32* maximum)
{
    VALIDATE_NOT_NULL(maximum);
    if (mGregorianCutover != sDefaultGregorianCutover
            && field == WEEK_OF_YEAR) {
        Int64 currentTimeInMillis = mTime;
        SetTimeInMillis(mGregorianCutover);
        Int32 actual;
        GetActualMaximum(field, &actual);
        SetTimeInMillis(currentTimeInMillis);
        *maximum = actual;
        return NOERROR;
    }

    *maximum = sLeastMaximums[field];
    return NOERROR;
}

ECode CGregorianCalendar::GetMaximum(
    /* [in] */ Int32 field,
    /* [out] */ Int32* maximum)
{
    VALIDATE_NOT_NULL(maximum);
    *maximum = sMaximums[field];
    return NOERROR;
}

ECode CGregorianCalendar::GetMinimum(
    /* [in] */ Int32 field,
    /* [out] */ Int32* minimum)
{
    VALIDATE_NOT_NULL(minimum);
    *minimum = sMinimums[field];
    return NOERROR;
}

Int32 CGregorianCalendar::GetOffset(
    /* [in] */ Int64 localTime)
{
    AutoPtr<ITimeZone> timeZone;
    GetTimeZone((ITimeZone**)&timeZone);
    Int32 rawOffset;

    Int64 dayCount = localTime / 86400000;
    Int32 millis = (Int32) (localTime % 86400000);
    if (millis < 0) {
        millis += 86400000;
        dayCount--;
    }

    Int32 year = 1970;
    Int64 days = dayCount;
    if (localTime < mGregorianCutover) {
        days -= mJulianSkew;
    }
    Int32 approxYears;

    while ((approxYears = (Int32) (days / 365)) != 0) {
        year = year + approxYears;
        days = dayCount - DaysFromBaseYear(year);
    }
    if (days < 0) {
        year = year - 1;
        days = days + 365 + (IsLeapYear(year) ? 1 : 0);
        if (year == mChangeYear && localTime < mGregorianCutover) {
            days -= JulianError();
        }
    }
    if (year <= 0) {
        timeZone->GetRawOffset(&rawOffset);
        return rawOffset;
    }
    Int32 dayOfYear = (Int32) days + 1;

    Int32 month = dayOfYear / 32;
    Boolean leapYear = IsLeapYear(year);
    Int32 date = dayOfYear - DaysInYear(leapYear, month);
    if (date > DaysInMonth(leapYear, month)) {
        date -= DaysInMonth(leapYear, month);
        month++;
    }
    Int32 dayOfWeek = Mod7(dayCount - 3) + 1;
    Int32 offset;
    timeZone->GetOffset(AD, year, month, date, dayOfWeek,
            millis, &offset);
    return offset;
}

ECode CGregorianCalendar::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = Calendar::GetHashCode() + ((Int32)((UInt64)mGregorianCutover >> 32) ^ (Int32)mGregorianCutover);
    return NOERROR;
}

Boolean CGregorianCalendar::IsLeapYear(
    /* [in] */ Int32 year)
{
    if (year > mChangeYear) {
        return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
    }
    else {
        return (year % 4 == 0);
    }
}
ECode CGregorianCalendar::IsLeapYear(
    /* [in] */ Int32 year,
    /* [out] */ Boolean* isLeap)
{
    VALIDATE_NOT_NULL(isLeap);
    *isLeap = IsLeapYear(year);
    return NOERROR;
}

Int32 CGregorianCalendar::JulianError()
{
    return mChangeYear / 100 - mChangeYear / 400 - 2;
}

Int32 CGregorianCalendar::Mod(
    /* [in] */ Int32 value,
    /* [in] */ Int32 mod)
{
    Int32 rem = value % mod;
    if (value < 0 && rem < 0) {
        return rem + mod;
    }
    return rem;
}

Int32 CGregorianCalendar::Mod7(
    /* [in] */ Int64 num1)
{
    Int32 rem = (Int32) (num1 % 7);
    if (num1 < 0 && rem < 0) {
        return rem + 7;
    }
    return rem;
}

ECode CGregorianCalendar::Roll(
    /* [in] */ Int32 field,
    /* [in] */ Int32 value)
{
    if (value == 0) {
        return NOERROR;
    }
    if (field < 0 || field >= ZONE_OFFSET) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Complete();
    Int32 days, day, mod, maxWeeks, newWeek;
    Int32 max = -1;
    switch (field) {
    case YEAR:
        max = sMaximums[field];
        break;
    case WEEK_OF_YEAR:
        days = DaysInYear((*mFields)[YEAR]);
        day = DAY_OF_YEAR;
        mod = Mod7((*mFields)[DAY_OF_WEEK] - (*mFields)[day]
                - (GetFirstDayOfWeek() - 1));
        maxWeeks = (days - 1 + mod) / 7 + 1;
        newWeek = Mod((*mFields)[field] - 1 + value, maxWeeks) + 1;
        if (newWeek == maxWeeks) {
            Int32 addDays = (newWeek - (*mFields)[field]) * 7;
            if ((*mFields)[day] > addDays && (*mFields)[day] + addDays > days) {
                Set(field, 1);
            }
            else {
                Set(field, newWeek - 1);
            }
        }
        else if (newWeek == 1) {
            int week = ((*mFields)[day] - (((*mFields)[day] - 1) / 7 * 7) - 1 + mod) / 7 + 1;
            if (week > 1) {
                Set(field, 1);
            }
            else {
                Set(field, newWeek);
            }
        }
        else {
            Set(field, newWeek);
        }
        break;
    case WEEK_OF_MONTH:
        days = DaysInMonth();
        day = DATE;
        mod = Mod7((*mFields)[DAY_OF_WEEK] - (*mFields)[day]
                - (GetFirstDayOfWeek() - 1));
        maxWeeks = (days - 1 + mod) / 7 + 1;
        newWeek = Mod((*mFields)[field] - 1 + value, maxWeeks) + 1;
        if (newWeek == maxWeeks) {
            if ((*mFields)[day] + (newWeek - (*mFields)[field]) * 7 > days) {
                Set(day, days);
            }
            else {
                Set(field, newWeek);
            }
        }
        else if (newWeek == 1) {
            Int32 week = ((*mFields)[day] - (((*mFields)[day] - 1) / 7 * 7) - 1 + mod) / 7 + 1;
            if (week > 1) {
                Set(day, 1);
            }
            else {
                Set(field, newWeek);
            }
        }
        else {
            Set(field, newWeek);
        }
        break;
    case DATE:
        max = DaysInMonth();
        break;
    case DAY_OF_YEAR:
        max = DaysInYear((*mFields)[YEAR]);
        break;
    case DAY_OF_WEEK:
        max = sMaximums[field];
        mLastDateFieldSet = WEEK_OF_MONTH;
        break;
    case DAY_OF_WEEK_IN_MONTH:
        max = ((*mFields)[DATE] + ((DaysInMonth() - (*mFields)[DATE]) / 7 * 7) - 1) / 7 + 1;
        break;

    case ERA:
    case MONTH:
    case AM_PM:
    case HOUR:
    case HOUR_OF_DAY:
    case MINUTE:
    case SECOND:
    case MILLISECOND:
        Set(field, Mod((*mFields)[field] + value, sMaximums[field] + 1));
        if (field == MONTH && (*mFields)[DATE] > DaysInMonth()) {
            Set(DATE, DaysInMonth());
        }
        else if (field == AM_PM) {
            mLastTimeFieldSet = HOUR;
        }
        break;
    }
    if (max != -1) {
        Set(field, Mod((*mFields)[field] - 1 + value, max) + 1);
    }
    Complete();
    return NOERROR;
}

ECode CGregorianCalendar::Roll(
    /* [in] */ Int32 field,
    /* [in] */ Boolean increment)
{
    return Roll(field, increment ? 1 : -1);
}

ECode CGregorianCalendar::SetGregorianChange(
    /* [in] */ IDate* date)
{
    VALIDATE_NOT_NULL(date);
    date->GetTime(&mGregorianCutover);
    AutoPtr<IGregorianCalendar> cal;
    AutoPtr<ITimeZone> timezone = TimeZone::GMT();
    CGregorianCalendar::New(timezone, (IGregorianCalendar**)&cal);
    (ICalendar::Probe(cal))->SetTime(date);
    (ICalendar::Probe(cal))->Get(YEAR, &mChangeYear);
    Int32 value;
    (ICalendar::Probe(cal))->Get(ERA, &value);
    if (value == BC) {
        mChangeYear = 1 - mChangeYear;
    }
    mJulianSkew = ((mChangeYear - 2000) / 400) + JulianError()
            - ((mChangeYear - 2000) / 100);

    Int32 dayOfYear;
    (ICalendar::Probe(cal))->Get(DAY_OF_YEAR, &dayOfYear);
    if (dayOfYear < mJulianSkew) {
        mCurrentYearSkew = dayOfYear-1;
        mLastYearSkew = mJulianSkew - dayOfYear + 1;
    }
    else {
        mLastYearSkew = 0;
        mCurrentYearSkew = mJulianSkew;
    }
    return NOERROR;
}

Int32 CGregorianCalendar::GetFirstDayOfWeek()
{
    Int32 value;
    Calendar::GetFirstDayOfWeek(&value);
    return value;
}

ECode CGregorianCalendar::Clone(
    /* [out] */ IInterface** cloned)
{
    VALIDATE_NOT_NULL(cloned)

    AutoPtr<IGregorianCalendar> temp;
    CGregorianCalendar::New((IGregorianCalendar**)&temp);
    CloneImpl(temp);
    *cloned = temp;
    REFCOUNT_ADD(*cloned);
    return NOERROR;
}

ECode CGregorianCalendar::CloneImpl(
    /* [in] */ IGregorianCalendar* object)
{
    return Calendar::CloneImpl(ICalendar::Probe(object));
}

ECode CGregorianCalendar::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = Calendar::ToString();
    return NOERROR;
}

} // namespace Utility
} // namespace Elastos
