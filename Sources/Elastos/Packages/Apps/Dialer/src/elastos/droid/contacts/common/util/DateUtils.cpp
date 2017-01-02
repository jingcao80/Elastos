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

#include "Elastos.Droid.Text.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/contacts/common/util/DateUtils.h"
#include "elastos/droid/contacts/common/util/CommonDateUtils.h"
#include "elastos/droid/text/format/DateFormat.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Text::Format::DateFormat;
using Elastos::Droid::Text::Format::ITimeHelper;
using Elastos::Droid::Text::Format::CTimeHelper;
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Text::CParsePosition;
using Elastos::Text::IParsePosition;
using Elastos::Text::IDateFormatHelper;
using Elastos::Text::CDateFormatHelper;
using Elastos::Utility::ILocale;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::CDate;
using Elastos::Utility::IGregorianCalendar;
using Elastos::Utility::CGregorianCalendar;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

//==========================================================
// DateUtils::StaticInitializer
//==========================================================
DateUtils::StaticInitializer::StaticInitializer()
{
    for (Int32 i = 0; i < DateUtils::DATE_FORMATS->GetLength(); ++i) {
        AutoPtr<Elastos::Text::IDateFormat> format = Elastos::Text::IDateFormat::Probe((*DateUtils::DATE_FORMATS)[i]);
        format->SetLenient(TRUE);
        format->SetTimeZone(DateUtils::UTC_TIMEZONE);
    }
    Elastos::Text::IDateFormat::Probe(CommonDateUtils::NO_YEAR_DATE_FORMAT)->SetTimeZone(UTC_TIMEZONE);
}


//==========================================================
// DateUtils
//==========================================================
static AutoPtr<ITimeZone> InitTimeZone()
{
    AutoPtr<ITimeZoneHelper> tzHelper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzHelper);
    AutoPtr<ITimeZone> tz;
    tzHelper->GetTimeZone(String("UTC"), (ITimeZone**)&tz);
    return tz;
}
const AutoPtr<ITimeZone> DateUtils::UTC_TIMEZONE = InitTimeZone();
const String DateUtils::NO_YEAR_DATE_FEB29TH("--02-29");

static AutoPtr<ArrayOf<ISimpleDateFormat*> > InitDateFormats()
{
    AutoPtr<ArrayOf<ISimpleDateFormat*> > formats = ArrayOf<ISimpleDateFormat*>::Alloc(7);
    formats->Set(0, CommonDateUtils::FULL_DATE_FORMAT);
    formats->Set(1, CommonDateUtils::DATE_AND_TIME_FORMAT);
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> us;
    helper->GetUS((ILocale**)&us);
    AutoPtr<ISimpleDateFormat> format;
    CSimpleDateFormat::New(String("yyyy-MM-dd'T'HH:mm'Z'"), us, (ISimpleDateFormat**)&format);
    formats->Set(2, format);
    AutoPtr<ISimpleDateFormat> format1;
    CSimpleDateFormat::New(String("yyyyMMdd"), us, (ISimpleDateFormat**)&format1);
    formats->Set(3, format1);
    AutoPtr<ISimpleDateFormat> format2;
    CSimpleDateFormat::New(String("yyyyMMdd'T'HHmmssSSS'Z'"), us, (ISimpleDateFormat**)&format2);
    formats->Set(4, format2);
    AutoPtr<ISimpleDateFormat> format3;
    CSimpleDateFormat::New(String("yyyyMMdd'T'HHmmss'Z'"), us, (ISimpleDateFormat**)&format3);
    formats->Set(5, format3);
    AutoPtr<ISimpleDateFormat> format4;
    CSimpleDateFormat::New(String("yyyyMMdd'T'HHmm'Z'"), us, (ISimpleDateFormat**)&format4);
    formats->Set(6, format4);
    return formats;
}
const AutoPtr<ArrayOf<ISimpleDateFormat*> > DateUtils::DATE_FORMATS = InitDateFormats();

DateUtils::StaticInitializer DateUtils::sInitializer;

AutoPtr<ICalendar> DateUtils::ParseDate(
    /* [in] */ const String& string,
    /* [in] */ Boolean mustContainYear)
{
    AutoPtr<IParsePosition> parsePosition;
    CParsePosition::New(0, (IParsePosition**)&parsePosition);
    AutoPtr<IDate> date;
    if (!mustContainYear) {
        Boolean noYearParsed;
        // Unfortunately, we can't parse Feb 29th correctly, so let's handle this day seperately
        if (NO_YEAR_DATE_FEB29TH.Equals(string)) {
            return GetUtcDate(0, ICalendar::FEBRUARY, 29);
        }
        else {
            {
                AutoLock lock(CommonDateUtils::NO_YEAR_DATE_FORMAT);
                Elastos::Text::IDateFormat::Probe(CommonDateUtils::NO_YEAR_DATE_FORMAT)
                        ->Parse(string, parsePosition, (IDate**)&date);
            }
            Int32 index;
            parsePosition->GetIndex(&index);
            noYearParsed = index == string.GetLength();
        }

        if (noYearParsed) {
            return GetUtcDate(date, TRUE);
        }
    }
    for (Int32 i = 0; i < DATE_FORMATS->GetLength(); i++) {
        AutoPtr<ISimpleDateFormat> f = (*DATE_FORMATS)[i];
        AutoLock lock(f);
        parsePosition->SetIndex(0);
        Elastos::Text::IDateFormat::Probe(f)->Parse(string, parsePosition, (IDate**)&date);
        Int32 index;
        if (parsePosition->GetIndex(&index), index == string.GetLength()) {
            return GetUtcDate(date, FALSE);
        }
    }
    return NULL;
}

AutoPtr<ICalendar> DateUtils::GetUtcDate(
    /* [in] */ IDate* date,
    /* [in] */ Boolean noYear)
{
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
    AutoPtr<ILocale> us;
    localeHelper->GetUS((ILocale**)&us);
    AutoPtr<ICalendar> calendar;
    helper->GetInstance(UTC_TIMEZONE, us, (ICalendar**)&calendar);
    calendar->SetTime(date);
    if (noYear) {
        calendar->Set(ICalendar::YEAR, 0);
    }
    return calendar;
}

AutoPtr<ICalendar> DateUtils::GetUtcDate(
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 dayOfMonth)
{
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
    AutoPtr<ILocale> us;
    localeHelper->GetUS((ILocale**)&us);
    AutoPtr<ICalendar> calendar;
    helper->GetInstance(UTC_TIMEZONE, us, (ICalendar**)&calendar);
    calendar->Clear();
    calendar->Set(ICalendar::YEAR, year);
    calendar->Set(ICalendar::MONTH, month);
    calendar->Set(ICalendar::DAY_OF_MONTH, dayOfMonth);
    return calendar;
}

Boolean DateUtils::IsYearSet(
    /* [in] */ ICalendar* cal)
{
    // use the Calendar.YEAR field to track whether or not the year is set instead of
    // Calendar.isSet() because doing Calendar.get() causes Calendar.isSet() to become
    // true irregardless of what the previous value was
    Int32 result;
    return (cal->Get(ICalendar::YEAR, &result), result > 1);
}

String DateUtils::FormatDate(
    /* [in] */ IContext* context,
    /* [in] */ const String& string)
{
    return FormatDate(context, string, TRUE);
}

String DateUtils::FormatDate(
    /* [in] */ IContext* context,
    /* [in] */ const String& _string,
    /* [in] */ Boolean longForm)
{
    String string = _string;
    if (string.IsNull()) {
        return String(NULL);
    }

    string = string.Trim();
    if (string.GetLength() == 0) {
        return string;
    }
    AutoPtr<ICalendar> cal = ParseDate(string, FALSE);

    // we weren't able to parse the string successfully so just return it unchanged
    if (cal == NULL) {
        return string;
    }

    Boolean isYearSet = IsYearSet(cal);
    AutoPtr<Elastos::Text::IDateFormat> outFormat;
    if (!isYearSet) {
        outFormat = GetLocalizedDateFormatWithoutYear(context);
    }
    else {
        outFormat =
                longForm ? DateFormat::GetLongDateFormat(context) :
                DateFormat::GetDateFormat(context);
    }
    AutoLock locl(outFormat);
    outFormat->SetTimeZone(UTC_TIMEZONE);
    AutoPtr<IDate> date;
    cal->GetTime((IDate**)&date);
    String str;
    outFormat->Format(date, &str);
    return str;
}

Boolean DateUtils::IsMonthBeforeDay(
    /* [in] */ IContext* context)
{
    AutoPtr<ArrayOf<Char32> > dateFormatOrder;
    DateFormat::GetDateFormatOrder(context, (ArrayOf<Char32>**)&dateFormatOrder);
    for (Int32 i = 0; i < dateFormatOrder->GetLength(); i++) {
        if ((*dateFormatOrder)[i] == IDateFormat::DATE) {
            return FALSE;
        }
        if ((*dateFormatOrder)[i] == IDateFormat::MONTH) {
            return TRUE;
        }
    }
    return FALSE;
}

AutoPtr<Elastos::Text::IDateFormat> DateUtils::GetLocalizedDateFormatWithoutYear(
    /* [in] */ IContext* context)
{
    AutoPtr<IDateFormatHelper> helper;
    CDateFormatHelper::AcquireSingleton((IDateFormatHelper**)&helper);
    AutoPtr<Elastos::Text::IDateFormat> dateInstance;
    helper->GetDateInstance(Elastos::Text::IDateFormat::LONG, (Elastos::Text::IDateFormat**)&dateInstance);
    String pattern;
    ISimpleDateFormat::Probe(dateInstance)->ToPattern(&pattern);
    // Determine the correct regex pattern for year.
    // Special case handling for Spanish locale by checking for "de"
    String yearPattern = pattern.Contains("de") ? String("[^Mm]*[Yy]+[^Mm]*") : String("[^DdMm]*[Yy]+[^DdMm]*");
    // try {
 // Eliminate the substring in pattern that matches the format for that of year
    String str;
    StringUtils::ReplaceAll(pattern, yearPattern.string(), "", &str);
    AutoPtr<Elastos::Text::IDateFormat> format;
    ECode ec = CSimpleDateFormat::New(str, (Elastos::Text::IDateFormat**)&format);
    if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        format = NULL;
        CSimpleDateFormat::New(
                DateUtils::IsMonthBeforeDay(context) ? String("MMMM dd") : String("dd MMMM"),
                (Elastos::Text::IDateFormat**)&format);
    }
    // } catch (IllegalArgumentException e) {
    //     return new SimpleDateFormat(
    //             DateUtils.isMonthBeforeDay(context) ? "MMMM dd" : "dd MMMM");
    // }
    return format;
}

AutoPtr<IDate> DateUtils::GetNextAnnualDate(
    /* [in] */ ICalendar* target)
{
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ICalendar> today;
    helper->GetInstance((ICalendar**)&today);
    AutoPtr<IDate> date;
    CDate::New((IDate**)&date);
    today->SetTime(date);

    // Round the current time to the exact start of today so that when we compare
    // today against the target date, both dates are set to exactly 0000H.
    today->Set(ICalendar::HOUR_OF_DAY, 0);
    today->Set(ICalendar::MINUTE, 0);
    today->Set(ICalendar::SECOND, 0);
    today->Set(ICalendar::MILLISECOND, 0);

    Boolean isYearSet = IsYearSet(target);
    Int32 targetYear;
    target->Get(ICalendar::YEAR, &targetYear);
    Int32 targetMonth;
    target->Get(ICalendar::MONTH, &targetMonth);
    Int32 targetDay;
    target->Get(ICalendar::DAY_OF_MONTH, &targetDay);
    Boolean isFeb29 = (targetMonth == ICalendar::FEBRUARY && targetDay == 29);
    AutoPtr<ICalendar> anniversary;
    CGregorianCalendar::New((ICalendar**)&anniversary);
    // Convert from the UTC date to the local date. Set the year to today's year if the
    // there is no provided year (targetYear < 1900)
    Int32 year;
    anniversary->Set(!isYearSet ? (today->Get(ICalendar::YEAR, &year), year) : targetYear,
            targetMonth, targetDay);
    // If the anniversary's date is before the start of today and there is no year set,
    // increment the year by 1 so that the returned date is always equal to or greater than
    // today. If the day is a leap year, keep going until we get the next leap year anniversary
    // Otherwise if there is already a year set, simply return the exact date.
    if (!isYearSet) {
        Int32 anniversaryYear;
        today->Get(ICalendar::YEAR, &anniversaryYear);
        Boolean before, isLeapYear;
        if ((anniversary->IsBefore(today, &before), before) ||
                (isFeb29 && (IGregorianCalendar::Probe(anniversary)->IsLeapYear(anniversaryYear, &isLeapYear), !isLeapYear))) {
            // If the target date is not Feb 29, then set the anniversary to the next year.
            // Otherwise, keep going until we find the next leap year (this is not guaranteed
            // to be in 4 years time).
            do {
                anniversaryYear +=1;
            } while (isFeb29 && (IGregorianCalendar::Probe(anniversary)->IsLeapYear(anniversaryYear, &isLeapYear), !isLeapYear));
            anniversary->Set(anniversaryYear, targetMonth, targetDay);
        }
    }
    AutoPtr<IDate> time;
    anniversary->GetTime((IDate**)&time);
    return time;
}

Int32 DateUtils::GetDayDifference(
    /* [in] */ ITime* time,
    /* [in] */ Int64 date1,
    /* [in] */ Int64 date2)
{
    time->Set(date1);
    AutoPtr<ITimeHelper> helper;
    CTimeHelper::AcquireSingleton((ITimeHelper**)&helper);
    Int64 gmtoff;
    time->GetGmtoff(&gmtoff);
    Int32 startDay;
    helper->GetJulianDay(date1, gmtoff, &startDay);

    time->Set(date2);
    Int32 currentDay;
    time->GetGmtoff(&gmtoff);
    helper->GetJulianDay(date2, gmtoff, &currentDay);

    return Elastos::Core::Math::Abs(currentDay - startDay);
}

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
