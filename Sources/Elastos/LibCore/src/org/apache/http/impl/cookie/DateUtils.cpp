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

#include "org/apache/http/impl/cookie/DateUtils.h"
#include "elastos/core/CString.h"
#include "elastos/text/CSimpleDateFormat.h"
#include "elastos/utility/CTimeZoneHelper.h"
#include "elastos/utility/CCalendarHelper.h"
#include "elastos/utility/CLocale.h"
#include "elastos/utility/CHashMap.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::CString;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Text::IDateFormat;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::ICalendar;
using Elastos::Utility::CLocale;
using Elastos::Utility::IHashMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

//==============================================================================
// DateUtils::DateFormatHolder
//==============================================================================

AutoPtr<ISimpleDateFormat> DateUtils::DateFormatHolder::FormatFor(
    /* [in] */ const String& pattern)
{
    // SoftReference<Map<String, SimpleDateFormat>> ref = THREADLOCAL_FORMATS.get();
    // Map<String, SimpleDateFormat> formats = ref.get();
    AutoPtr<IHashMap> formats;
    if (formats == NULL) {
        CHashMap::New((IHashMap**)&formats);
        // THREADLOCAL_FORMATS.set(
        //         new SoftReference<Map<String, SimpleDateFormat>>(formats));
    }

    AutoPtr<CString> cs;
    CString::NewByFriend(pattern, (CString**)&cs);
    AutoPtr<IInterface> value;
    formats->Get(cs->Probe(EIID_IInterface), (IInterface**)&value);
    AutoPtr<ISimpleDateFormat> format = ISimpleDateFormat::Probe(value);
    if (format == NULL) {
        CSimpleDateFormat::New(pattern, CLocale::US, (ISimpleDateFormat**)&format);
        AutoPtr<CTimeZoneHelper> helper;
        CTimeZoneHelper::AcquireSingletonByFriend((CTimeZoneHelper**)&helper);
        AutoPtr<ITimeZone> timeZone;
        helper->GetTimeZone(String("GMT"), (ITimeZone**)&timeZone);
        IDateFormat::Probe(format)->SetTimeZone(timeZone);
        formats->Put(cs->Probe(EIID_IInterface), format);
    }

    return format;
}


//==============================================================================
// DateUtils
//==============================================================================

const String DateUtils::PATTERN_RFC1123("EEE, dd MMM yyyy HH:mm:ss zzz");
const String DateUtils::PATTERN_RFC1036("EEEE, dd-MMM-yy HH:mm:ss zzz");
const String DateUtils::PATTERN_ASCTIME("EEE MMM d HH:mm:ss yyyy");

static AutoPtr<ITimeZone> InitGMT()
{
    AutoPtr<CTimeZoneHelper> helper;
    CTimeZoneHelper::AcquireSingletonByFriend((CTimeZoneHelper**)&helper);
    AutoPtr<ITimeZone> timeZone;
    helper->GetTimeZone(String("GMT"), (ITimeZone**)&timeZone);
    return timeZone;
}
const AutoPtr<ITimeZone> DateUtils::GMT = InitGMT();

static AutoPtr< ArrayOf<String> > InitDefaultPatterns()
{
    AutoPtr< ArrayOf<String> > patterns = ArrayOf<String>::Alloc(3);
    (*patterns)[0] = DateUtils::PATTERN_RFC1036;
    (*patterns)[1] = DateUtils::PATTERN_RFC1123;
    (*patterns)[2] = DateUtils::PATTERN_ASCTIME;
    return patterns;
}
const AutoPtr< ArrayOf<String> > DateUtils::DEFAULT_PATTERNS = InitDefaultPatterns();

static AutoPtr<IDate> InitDefaultTwoDigitYearStart()
{
    AutoPtr<CCalendarHelper> helper;
    CCalendarHelper::AcquireSingletonByFriend((CCalendarHelper**)&helper);
    AutoPtr<ICalendar> calender;
    helper->GetInstance((ICalendar**)&calender);
    calender->SetTimeZone(DateUtils::GMT);
    calender->Set(2000, ICalendar::JANUARY, 1, 0, 0, 0);
    calender->Set(ICalendar::MILLISECOND, 0);
    AutoPtr<IDate> date;
    calender->GetTime((IDate**)&date);
    return date;
}
const AutoPtr<IDate> DateUtils::DEFAULT_TWO_DIGIT_YEAR_START = InitDefaultTwoDigitYearStart();

ECode DateUtils::ParseDate(
    /* [in] */ const String& _dateValue,
    /* [in] */ ArrayOf<String>* dateFormats,
    /* [in] */ IDate* startDate,
    /* [out] */ IDate** date)
{
    VALIDATE_NOT_NULL(date)
    *date = NULL;

    String dateValue = _dateValue;

    if (dateValue.IsNull()) {
        Logger::E("DateUtils", "dateValue is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (dateFormats == NULL) {
        dateFormats = DEFAULT_PATTERNS;
    }
    if (startDate == NULL) {
        startDate = DEFAULT_TWO_DIGIT_YEAR_START;
    }
    // trim single quotes around date if present
    // see issue #5279
    if ((dateValue.GetLength() > 1) && dateValue.StartWith("'")
            && dateValue.EndWith("'")) {
        dateValue = dateValue.Substring (1, dateValue.GetLength() - 1);
    }

    for (Int32 i = 0; i < dateFormats->GetLength(); ++i) {
        String dateFormat = (*dateFormats)[i];
        AutoPtr<ISimpleDateFormat> dateParser = DateFormatHolder::FormatFor(dateFormat);
        dateParser->Set2DigitYearStart(startDate);

        // try {
        return IDateFormat::Probe(dateParser)->Parse(dateValue, date);
        // } catch (ParseException pe) {
        //     // ignore this exception, we will try the next format
        // }
    }

    // we were unable to parse the date
    Logger::E("DateUtils", "Unable to parse the date %s", dateValue.string());
    return E_DATE_PARSE_EXCEPTION;
    // throw new DateParseException("Unable to parse the date " + dateValue);
}

ECode DateUtils::ParseDate(
    /* [in] */ const String& dateValue,
    /* [out] */ IDate** date)
{
    VALIDATE_NOT_NULL(date)
    return ParseDate(dateValue, NULL, NULL, date);
}

ECode DateUtils::ParseDate(
    /* [in] */ const String& dateValue,
    /* [in] */ ArrayOf<String>* dateFormats,
    /* [out] */ IDate** date)
{
    VALIDATE_NOT_NULL(date)
    return ParseDate(dateValue, dateFormats, NULL, date);
}

ECode DateUtils::FormatDate(
    /* [in] */ IDate* date,
    /* [out] */ String* formatDate)
{
    VALIDATE_NOT_NULL(formatDate)
    return FormatDate(date, PATTERN_RFC1123, formatDate);
}

ECode DateUtils::FormatDate(
    /* [in] */ IDate* date,
    /* [in] */ const String& pattern,
    /* [out] */ String* formatDate)
{
    VALIDATE_NOT_NULL(formatDate)
    *formatDate = String(NULL);

    if (date == NULL) {
        Logger::E("DateUtils", "date is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (pattern.IsNull()) {
        Logger::E("DateUtils", "pattern is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ISimpleDateFormat> formatter = DateFormatHolder::FormatFor(pattern);
    return IDateFormat::Probe(formatter)->Format(date, formatDate);
}

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org