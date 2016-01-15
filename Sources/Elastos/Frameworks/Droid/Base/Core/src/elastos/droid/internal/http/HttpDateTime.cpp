
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/internal/http/HttpDateTime.h"
#include "elastos/droid/text/format/CTime.h"
#include <elastos/core/Character.h>

using Elastos::Droid::Text::Format::CTime;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Core::Character;
using Elastos::Utility::ICalendar;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::IPattern;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IMatchResult;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Http {

const String HttpDateTime::HTTP_DATE_RFC_REGEXP =
    String("([0-9]{1,2})[- ]([A-Za-z]{3,9})[- ]([0-9]{2,4})[ ]")
    + String("([0-9]{1,2}:[0-9][0-9]:[0-9][0-9])");

const String HttpDateTime::HTTP_DATE_ANSIC_REGEXP =
    String("[ ]([A-Za-z]{3,9})[ ]+([0-9]{1,2})[ ]")
    + String("([0-9]{1,2}:[0-9][0-9]:[0-9][0-9])[ ]([0-9]{2,4})");

static AutoPtr<IPattern> InitPattern(
    /* [in] */ const String& regularExpression)
{
    AutoPtr<IPatternHelper> patternHelper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&patternHelper);
    AutoPtr<IPattern> pattern;
    patternHelper->Compile(regularExpression, (IPattern**)&pattern);
    return pattern;
}

const AutoPtr<IPattern> HttpDateTime::HTTP_DATE_RFC_PATTERN = InitPattern(HTTP_DATE_RFC_REGEXP);
const AutoPtr<IPattern> HttpDateTime::HTTP_DATE_ANSIC_PATTERN = InitPattern(HTTP_DATE_ANSIC_REGEXP);

HttpDateTime::TimeOfDay::TimeOfDay(
    /* [in] */ Int32 h,
    /* [in] */  Int32 m,
    /* [in] */  Int32 s)
    : mHour(h)
    , mMinute(m)
    , mSecond(s)
{}

ECode HttpDateTime::Parse(
    /* [in] */ const String& timeString,
    /* [out] */ Int64* time)
{
    Int32 date = 1;
    Int32 month = ICalendar::JANUARY;
    Int32 year = 1970;
    AutoPtr<TimeOfDay> timeOfDay;

    AutoPtr<IMatcher> rfcMatcher;
    HTTP_DATE_RFC_PATTERN->Matcher(timeString, (IMatcher**)&rfcMatcher);
    AutoPtr<IMatchResult> matchResult;
    Boolean isFind;
    rfcMatcher->Find(&isFind);
    if (isFind) {
        matchResult = IMatchResult::Probe(rfcMatcher);
        String sD;
        matchResult->Group(1, &sD);
        date = GetDate(sD);

        String sM;
        matchResult->Group(2, &sM);
        FAIL_RETURN(GetMonth(sM, &month));

        String sY;
        matchResult->Group(3, &sY);
        year = GetYear(sY);

        String sT;
        matchResult->Group(4, &sT);
        timeOfDay = GetTime(sT);
    }
    else {
        AutoPtr<IMatcher> ansicMatcher;
        HTTP_DATE_ANSIC_PATTERN->Matcher(timeString, (IMatcher**)&ansicMatcher);
        Boolean isFind;
        ansicMatcher->Find(&isFind);
        if (isFind) {
            matchResult = IMatchResult::Probe(ansicMatcher);
            String sM;
            matchResult->Group(1, &sM);
            FAIL_RETURN(GetMonth(sM, &month));

            String sD;
            matchResult->Group(2, &sD);
            date = GetDate(sD);

            String sT;
            matchResult->Group(3, &sT);
            timeOfDay = GetTime(sT);

            String sY;
            matchResult->Group(4, &sY);
            year = GetYear(sY);
        }
        else {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    // FIXME: Y2038 BUG!
    if (year >= 2038) {
        year = 2038;
        month = ICalendar::JANUARY;
        date = 1;
    }

    AutoPtr<ITime> itime;
    CTime::New(ITime::TIMEZONE_UTC, (ITime**)&itime);
    itime->Set(timeOfDay->mSecond, timeOfDay->mMinute, timeOfDay->mHour,
        date, month, year);

    return itime->ToMillis(FALSE /* use isDst */, time);
}

Int32 HttpDateTime::GetDate(
    /* [in] */ const String& dateString)
{
    if (dateString.GetLength() == 2) {
        return (dateString.GetChar(0) - '0') * 10
                + (dateString.GetChar(1) - '0');
    }

    return (dateString.GetChar(0) - '0');
}

ECode HttpDateTime::GetMonth(
    /* [in] */ const String& monthString,
    /* [out] */ Int32* month)
{
    VALIDATE_NOT_NULL(month);

    Int32 hash = Character::ToLowerCase(monthString.GetChar(0)) +
            Character::ToLowerCase(monthString.GetChar(1)) +
            Character::ToLowerCase(monthString.GetChar(2)) - 3 * 'a';
    switch (hash) {
        case 22:
            return ICalendar::JANUARY;
        case 10:
            return ICalendar::FEBRUARY;
        case 29:
            return ICalendar::MARCH;
        case 32:
            return ICalendar::APRIL;
        case 36:
            return ICalendar::MAY;
        case 42:
            return ICalendar::JUNE;
        case 40:
            return ICalendar::JULY;
        case 26:
            return ICalendar::AUGUST;
        case 37:
            return ICalendar::SEPTEMBER;
        case 35:
            return ICalendar::OCTOBER;
        case 48:
            return ICalendar::NOVEMBER;
        case 9:
            return ICalendar::DECEMBER;
        default:
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

Int32 HttpDateTime::GetYear(
    /* [in] */ const String& yearString)
{
    if (yearString.GetLength() == 2) {
        Int32 year = (yearString.GetChar(0) - '0') * 10
                + (yearString.GetChar(1) - '0');
        if (year >= 70) {
            return year + 1900;
        }
        else {
            return year + 2000;
        }
    }
    else if (yearString.GetLength() == 3) {
        // According to RFC 2822, three digit years should be added to 1900.
        Int32 year = (yearString.GetChar(0) - '0') * 100
                + (yearString.GetChar(1) - '0') * 10
                + (yearString.GetChar(2) - '0');
        return year + 1900;
    }
    else if (yearString.GetLength() == 4) {
         return (yearString.GetChar(0) - '0') * 1000
                + (yearString.GetChar(1) - '0') * 100
                + (yearString.GetChar(2) - '0') * 10
                + (yearString.GetChar(3) - '0');
    }

    return 1970;
}

AutoPtr<HttpDateTime::TimeOfDay> HttpDateTime::GetTime(
    /* [in] */ const String& timeString)
{
    // HH might be H
    Int32 i = 0;
    Int32 hour = timeString.GetChar(i++) - '0';
    if (timeString.GetChar(i) != ':')
        hour = hour * 10 + (timeString.GetChar(i++) - '0');
    // Skip ':'
    i++;

    Int32 minute = (timeString.GetChar(i++) - '0') * 10;
    minute += (timeString.GetChar(i++) - '0');
    // Skip ':'
    i++;

    Int32 second = (timeString.GetChar(i++) - '0') * 10;
    second += (timeString.GetChar(i++) - '0');

    AutoPtr<TimeOfDay> td = new TimeOfDay(hour, minute, second);
    return td;
}

} // namespace Http
} // namespace Internal
} // namespace Droid
} // namespace Elastos
