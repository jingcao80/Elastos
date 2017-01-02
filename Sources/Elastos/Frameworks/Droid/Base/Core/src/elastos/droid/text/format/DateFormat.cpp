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

#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/text/format/DateFormat.h"
#include "elastos/droid/text/format/CDateUtils.h"
#include "elastos/droid/text/CSpannedString.h"
#include "elastos/droid/text/CSpannableStringBuilder.h"
#include "elastos/droid/provider/CSettingsSystem.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Object.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Text::ISpannableStringBuilder;
using Elastos::Droid::Text::CSpannableStringBuilder;
using Elastos::Droid::Text::ISpannedString;
using Elastos::Droid::Text::CSpannedString;
using Elastos::Droid::Text::IEditable;
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Text::IDateFormatHelper;
using Elastos::Text::CDateFormatHelper;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Utility::IDate;
using Elastos::Utility::CDate;
using Elastos::Utility::ICalendar;
using Elastos::Utility::CGregorianCalendar;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Libcore::ICU::ILocaleDataHelper;
using Libcore::ICU::CLocaleDataHelper;
using Libcore::ICU::CLocaleData;
using Libcore::ICU::IICUUtil;
using Libcore::ICU::CICUUtil;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Format {

Object DateFormat::sLocaleLock;
AutoPtr<ILocale> DateFormat::sIs24HourLocale;
Boolean DateFormat::sIs24Hour = FALSE;

Boolean DateFormat::Is24HourFormat(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsSystem> settingsSystem;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSystem);
    String value;
    settingsSystem->GetString(resolver, ISettingsSystem::TIME_12_24, &value);

    if (value.IsNull()) {
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        AutoPtr<IConfiguration> config;
        res->GetConfiguration((IConfiguration**)&config);
        AutoPtr<ILocale> locale;
        config->GetLocale((ILocale**)&locale);
        {    AutoLock syncLock(sLocaleLock);
            Boolean bIs24HourLocale;
            if (sIs24HourLocale != NULL &&
                    (sIs24HourLocale->Equals(locale, &bIs24HourLocale), bIs24HourLocale)) {
                return sIs24Hour;
            }
        }

        AutoPtr<IDateFormatHelper> helper;
        CDateFormatHelper::AcquireSingleton((IDateFormatHelper**)&helper);
        AutoPtr<Elastos::Text::IDateFormat> natural;
        helper->GetTimeInstance(Elastos::Text::IDateFormat::LONG, locale, (Elastos::Text::IDateFormat**)&natural);

        if (ISimpleDateFormat::Probe(natural) != NULL) {
            AutoPtr<ISimpleDateFormat> sdf = ISimpleDateFormat::Probe(natural);
            String pattern;
            sdf->ToPattern(&pattern);
            if (pattern.IndexOf('H') >= 0) {
                value = "24";
            }
            else {
                value = "12";
            }
        }
        else {
            value = "12";
        }

        {    AutoLock syncLock(sLocaleLock);
            sIs24HourLocale = locale;
            sIs24Hour = value.Equals("24");
        }

        return sIs24Hour;
    }

    return value.Equals("24");
}

String DateFormat::GetBestDateTimePattern(
    /* [in] */ ILocale* locale,
    /* [in] */ const String& skeleton)
{
    AutoPtr<IICUUtil> icu;
    CICUUtil::AcquireSingleton((IICUUtil**)&icu);
    String ret;
    icu->GetBestDateTimePattern(skeleton, locale, &ret);
    return ret;
}

AutoPtr<Elastos::Text::IDateFormat> DateFormat::GetTimeFormat(
    /* [in] */ IContext* context)
{
    String timeFormat = GetTimeFormatString(context);
    AutoPtr<ISimpleDateFormat> sdf;
    CSimpleDateFormat::New(timeFormat, (ISimpleDateFormat**)&sdf);
    return Elastos::Text::IDateFormat::Probe(sdf);
}


String DateFormat::GetTimeFormatString(
    /* [in] */ IContext* context)
{
    AutoPtr<ILocaleDataHelper> helper;
    CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&helper);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    AutoPtr<ILocale> locale;
    config->GetLocale((ILocale**)&locale);
    AutoPtr<ILocaleData> d;
    helper->Get(locale, (ILocaleData**)&d);

    String format;
    if (Is24HourFormat(context)) {
        d->GetTimeFormat24(&format);
    }
    else {
        d->GetTimeFormat12(&format);
    }
    return format;
}

AutoPtr<Elastos::Text::IDateFormat> DateFormat::GetDateFormat(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsSystem> settingsSystem;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSystem);
    String value;
    settingsSystem->GetString(resolver, ISettingsSystem::DATE_FORMAT, &value);
    return GetDateFormatForSetting(context, value);
}

AutoPtr<Elastos::Text::IDateFormat> DateFormat::GetDateFormatForSetting(
    /* [in] */ IContext* context,
    /* [in] */ const String& value)
{
    String format = GetDateFormatStringForSetting(context, value);

    AutoPtr<ISimpleDateFormat> sdf;
    CSimpleDateFormat::New(format, (ISimpleDateFormat**)&sdf);
    return Elastos::Text::IDateFormat::Probe(sdf);
}

String DateFormat::GetDateFormatStringForSetting(
    /* [in] */ IContext* context,
    /* [in] */ const String& value)
{
     if (!value.IsNull()) {
        Int32 month = value.IndexOf('M');
        Int32 day = value.IndexOf('d');
        Int32 year = value.IndexOf('y');

        if (month >= 0 && day >= 0 && year >= 0) {
            String temp;
            context->GetString(R::string::numeric_date_template, &temp);

            String value_("");
            if (year < month && year < day) {
                if (month < day) {
                    value_.AppendFormat(temp.string(), "yyyy", "MM", "dd");
                }
                else {
                    value_.AppendFormat(temp.string(), "yyyy", "dd", "MM");
                }
            }
            else if (month < day) {
                if (day < year) {
                    value_.AppendFormat(temp.string(), "MM", "dd", "yyyy");
                }
                else { // unlikely
                    value_.AppendFormat(temp.string(), "MM", "yyyy", "dd");
                }
            }
            else { // day < month
                if (month < year) {
                    value_.AppendFormat(temp.string(), "dd", "MM", "yyyy");
                }
                else { // unlikely
                    value_.AppendFormat(temp.string(), "dd", "yyyy", "MM");
                }
            }

            return value_;
        }
    }

    // The setting is not set; use the locale's default.
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IConfiguration> config;
    resources->GetConfiguration((IConfiguration**)&config);
    AutoPtr<ILocale> locale;
    config->GetLocale((ILocale**)&locale);
    AutoPtr<ILocaleDataHelper> localeDataHelper;
    CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&localeDataHelper);
    AutoPtr<ILocaleData> d;
    localeDataHelper->Get(locale, (ILocaleData**)&d);
    String ret;
    d->GetShortDateFormat4(&ret);
    return ret;
}

AutoPtr<Elastos::Text::IDateFormat> DateFormat::GetLongDateFormat(
    /* [in] */ IContext* context)
{
    AutoPtr<IDateFormatHelper> helper;
    CDateFormatHelper::AcquireSingleton((IDateFormatHelper**)&helper);
    AutoPtr<Elastos::Text::IDateFormat> df;
    helper->GetDateInstance(Elastos::Text::IDateFormat::LONG, (Elastos::Text::IDateFormat**)&df);
    return df;
}

AutoPtr<Elastos::Text::IDateFormat> DateFormat::GetMediumDateFormat(
    /* [in] */ IContext* context)
{
    AutoPtr<IDateFormatHelper> helper;
    CDateFormatHelper::AcquireSingleton((IDateFormatHelper**)&helper);
    AutoPtr<Elastos::Text::IDateFormat> df;
    helper->GetDateInstance(Elastos::Text::IDateFormat::MEDIUM, (Elastos::Text::IDateFormat**)&df);
    return df;
}

ECode DateFormat::GetDateFormatOrder(
    /* [in] */ IContext* context,
    /* [out] */ ArrayOf<Char32>** ret)
{
    VALIDATE_NOT_NULL(ret)
    AutoPtr<IICUUtil> icu;
    CICUUtil::AcquireSingleton((IICUUtil**)&icu);
    AutoPtr<ArrayOf<Char32> > locales;
    return icu->GetDateFormatOrder(GetDateFormatString(context), ret);
}

String DateFormat::GetDateFormatString(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsSystem> settingsSystem;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSystem);
    String value;
    settingsSystem->GetString(resolver, ISettingsSystem::DATE_FORMAT, &value);
    return GetDateFormatStringForSetting(context, value);
}

AutoPtr<ICharSequence> DateFormat::Format(
    /* [in] */ ICharSequence* inFormat,
    /* [in] */ Int64 inTimeInMillis)
{
    AutoPtr<IDate> date;
    ASSERT_SUCCEEDED(CDate::New(inTimeInMillis, (IDate**)&date))
    return Format(inFormat, date);
}

AutoPtr<ICharSequence> DateFormat::Format(
    /* [in] */ ICharSequence* inFormat,
    /* [in] */ IDate* inDate)
{
    assert(inDate != NULL);
    AutoPtr<ICalendar> c;
    CGregorianCalendar::New((ICalendar**)&c);
    c->SetTime(inDate);

    return Format(inFormat, c);
}

AutoPtr<ICharSequence> DateFormat::Format(
    /* [in] */ const String& format,
    /* [in] */ IDate* inDate)
{
    assert(inDate != NULL);
    AutoPtr<ICharSequence> inFormat;
    CString::New(format, (ICharSequence**)&inFormat);

    AutoPtr<ICalendar> c;
    CGregorianCalendar::New((ICalendar**)&c);
    c->SetTime(inDate);

    return Format(inFormat, c);
}

Boolean DateFormat::HasSeconds(
    /* [in] */ ICharSequence* inFormat)
{
    return HasDesignator(inFormat, IDateFormat::SECONDS);
}

Boolean DateFormat::HasDesignator(
    /* [in] */ ICharSequence* inFormat,
    /* [in] */ Char32 designator)
{
    if (inFormat == NULL) return FALSE;

    Int32 length;
    inFormat->GetLength(&length);

    Char32 c;
    Int32 count;

    for (Int32 i = 0; i < length; i += count) {
        count = 1;
        inFormat->GetCharAt(i, &c);

        if (c == Elastos::Droid::Text::Format::IDateFormat::QUOTE) {
            count = SkipQuotedText(inFormat, i, length);
        }
        else if (c == Elastos::Droid::Text::Format::IDateFormat::SECONDS) {
            return TRUE;
        }
    }

    return FALSE;
}

Int32 DateFormat::SkipQuotedText(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 i,
    /* [in] */ Int32 len)
{
    Char32 cc;
    if(i + 1 < len && (s->GetCharAt(i + 1, &cc),
            cc == Elastos::Droid::Text::Format::IDateFormat::QUOTE)) {
        return 2;
    }

    Int32 count = 1;
    // skip leading quote
    i++;

    while (i < len) {
        Char32 c;
        s->GetCharAt(i, &c);

        if (c == Elastos::Droid::Text::Format::IDateFormat::QUOTE) {
            count++;
            //  QUOTEQUOTE -> QUOTE
            if (i + 1 < len && (s->GetCharAt(i + 1, &cc),
                    cc == Elastos::Droid::Text::Format::IDateFormat::QUOTE)) {
                i++;
            } else {
                break;
            }
        } else {
            i++;
            count++;
        }
    }

    return count;
}

AutoPtr<ICharSequence> DateFormat::Format(
    /* [in] */ ICharSequence* inFormat,
    /* [in] */ ICalendar* inDate)
{
    assert(inDate != NULL);
    AutoPtr<ISpannableStringBuilder> s;
    CSpannableStringBuilder::New(inFormat, (ISpannableStringBuilder**)&s);
    Int32 count;

    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
    AutoPtr<ILocale> defaultLocale;
    localeHelper->GetDefault((ILocale**)&defaultLocale);
    AutoPtr<ILocaleDataHelper> localDataHelper;
    CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&localDataHelper);
    AutoPtr<ILocaleData> localeData;
    localDataHelper->Get(defaultLocale, (ILocaleData**)&localeData);

    ICharSequence* csq = ICharSequence::Probe(s);

    Int32 len, csqLength;
    inFormat->GetLength(&len);
    csq->GetLength(&csqLength);

    for (Int32 i = 0; i < len; i += count) {
        Int32 temp;
        Char32 c;

        count = 1;
        csq->GetCharAt(i, &c);

        if (c == Elastos::Droid::Text::Format::IDateFormat::QUOTE) {
            count = AppendQuotedText(s, i, len);
            len = csqLength;
            continue;
        }

        Char32 cc;
        while ((i + count < len) && (csq->GetCharAt(i + count, &cc), cc == c)) {
            count++;
        }

        String replacement;

        switch (c) {
            case 'A':
            case 'a':
                {
                    inDate->Get(ICalendar::AM_PM, &temp);
                    AutoPtr<ArrayOf<String> > amPm;
                    localeData->GetAmPm((ArrayOf<String>**)&amPm);
                    replacement = (*amPm)[temp - ICalendar::AM];
                }
                break;
            case 'd':
                {
                    inDate->Get(ICalendar::DATE, &temp);
                    replacement = ZeroPad(temp, count);
                }
                break;
            case 'c':
            case 'E':
                {
                    inDate->Get(ICalendar::DAY_OF_WEEK, &temp);
                    replacement = GetDayOfWeekString(localeData, temp, count, c);
                }
                break;
            case 'K': // hour in am/pm (0-11)
            case 'h': // hour in am/pm (1-12)
                {
                    Int32 hour;
                    inDate->Get(ICalendar::HOUR, &hour);
                    if (c == 'h' && hour == 0) {
                        hour = 12;
                    }
                    replacement = ZeroPad(hour, count);
                }
                break;
            case 'H': // hour in day (0-23)
            case 'k': // hour in day (1-24) [but see note below]
                {
                    Int32 hour;
                    inDate->Get(ICalendar::HOUR_OF_DAY, &hour);
                    // Historically on Android 'k' was interpreted as 'H', which wasn't
                    // implemented, so pretty much all callers that want to format 24-hour
                    // times are abusing 'k'. http://b/8359981.
                    if (FALSE && c == 'k' && hour == 0) {
                        hour = 24;
                    }
                    replacement = ZeroPad(hour, count);
                }
                break;
            case 'L':
            case 'M':
                {
                    inDate->Get(ICalendar::MONTH, &temp);
                    replacement = GetMonthString(localeData, temp, count, c);
                }
                break;
            case 'm':
                {
                    inDate->Get(ICalendar::MINUTE, &temp);
                    replacement = ZeroPad(temp, count);
                }
                break;
            case 's':
                {
                    inDate->Get(ICalendar::SECOND, &temp);
                    replacement = ZeroPad(temp, count);
                }
                break;
            case 'y':
                {
                    inDate->Get(ICalendar::YEAR, &temp);
                    replacement = GetYearString(temp, count);
                }
                break;
            case 'z':
                {
                    replacement = GetTimeZoneString(inDate, count);
                }
                break;
            default:
                replacement = NULL;
                break;
            }

        if (!replacement.IsNull()) {
            AutoPtr<ICharSequence> cs;
            CString::New(replacement, (ICharSequence**)&cs);
            IEditable::Probe(s)->Replace(i, i + count, cs);
            count = replacement.GetLength(); // CARE: count is used in the for loop above
            ICharSequence::Probe(s)->GetLength(&len);
        }
    }

    if (ISpanned::Probe(inFormat) != NULL) {
        AutoPtr<ICharSequence> cseq = ICharSequence::Probe(s);
        AutoPtr<ICharSequence> spannedStr;
        CSpannedString::New(cseq, (ICharSequence**)&spannedStr);
        return spannedStr;
    } else {
        return ICharSequence::Probe(s);
    }
}

String DateFormat::GetDayOfWeekString(
    /* [in] */ ILocaleData* ld,
    /* [in] */ Int32 day,
    /* [in] */ Int32 count,
    /* [in] */ Int32 kind)
{
    Boolean standalone = (kind == 'c');
    if (count == 5) {
        AutoPtr<ArrayOf<String> > tsaWeekdayNames;
        ld->GetTinyStandAloneWeekdayNames((ArrayOf<String>**)&tsaWeekdayNames);
        AutoPtr<ArrayOf<String> > tWeekdayNames;
        ld->GetTinyWeekdayNames((ArrayOf<String>**)&tWeekdayNames);
        return standalone ? (*tsaWeekdayNames)[day] : (*tWeekdayNames)[day];

    } else if (count == 4) {
        AutoPtr<ArrayOf<String> > lsaWeekdayNames;
        ld->GetLongStandAloneWeekdayNames((ArrayOf<String>**)&lsaWeekdayNames);
        AutoPtr<ArrayOf<String> > lWeekdayNames;
        ld->GetLongWeekdayNames((ArrayOf<String>**)&lWeekdayNames);
        return standalone ? (*lsaWeekdayNames)[day] : (*lWeekdayNames)[day];
    } else {
        AutoPtr<ArrayOf<String> > ssaWeekdayNames;
        ld->GetShortStandAloneWeekdayNames((ArrayOf<String>**)&ssaWeekdayNames);
        AutoPtr<ArrayOf<String> > sWeekdayNames;
        ld->GetShortWeekdayNames((ArrayOf<String>**)&sWeekdayNames);
        return standalone ? (*ssaWeekdayNames)[day] : (*sWeekdayNames)[day];
    }
}

String DateFormat::GetMonthString(
    /* [in] */ ILocaleData* ld,
    /* [in] */ Int32 month,
    /* [in] */ Int32 count,
    /* [in] */ Int32 kind)
{
    Boolean standalone = (kind == 'L');
    AutoPtr<ArrayOf<String> > tsaMonthNames;
    ld->GetTinyStandAloneMonthNames((ArrayOf<String>**)&tsaMonthNames);
    AutoPtr<ArrayOf<String> > tMonthNames;
    ld->GetTinyMonthNames((ArrayOf<String>**)&tMonthNames);
    AutoPtr<ArrayOf<String> > lsaMonthNames;
    ld->GetLongStandAloneMonthNames((ArrayOf<String>**)&lsaMonthNames);
    AutoPtr<ArrayOf<String> > lMonthNames;
    ld->GetLongMonthNames((ArrayOf<String>**)&lMonthNames);
    AutoPtr<ArrayOf<String> > ssaMonthNames;
    ld->GetShortStandAloneMonthNames((ArrayOf<String>**)&ssaMonthNames);
    AutoPtr<ArrayOf<String> > sMonthNames;
    ld->GetShortMonthNames((ArrayOf<String>**)&sMonthNames);

    if (count == 5) {
        return standalone ? (*tsaMonthNames)[month] : (*tMonthNames)[month];
    } else if (count == 4) {
        return standalone ? (*lsaMonthNames)[month] : (*lMonthNames)[month];
    } else if (count == 3) {
        return standalone ? (*ssaMonthNames)[month] : (*sMonthNames)[month];
    } else {
        // Calendar.JANUARY == 0, so add 1 to month.
        return ZeroPad(month+1, count);
    }
}

String DateFormat::GetTimeZoneString(
    /* [in] */ ICalendar* inDate,
    /* [in] */ Int32 count)
{
    AutoPtr<ITimeZone> tz;
    inDate->GetTimeZone((ITimeZone**)&tz);

    if (count < 2) { // FIXME: shouldn't this be <= 2 ?
        Int32 dstOffset, zoneOffset;
        inDate->Get(ICalendar::DST_OFFSET, &dstOffset);
        inDate->Get(ICalendar::ZONE_OFFSET, &zoneOffset);
        return FormatZoneOffset(dstOffset + zoneOffset, count);
    }
    else {
        Int32 dstOffset;
        inDate->Get(ICalendar::DST_OFFSET, &dstOffset);
        Boolean dst = dstOffset != 0;
        String str;
        tz->GetDisplayName(dst, ITimeZone::SHORT, &str);
        return str;
    }
}

String DateFormat::FormatZoneOffset(
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    offset /= 1000; // milliseconds to seconds
    StringBuilder tb;

    if (offset < 0) {
        tb.InsertChar(0, Char32("-"));
        offset = -offset;
    }
    else {
        tb.InsertChar(0, Char32("+"));
    }

    Int32 hours = offset / 3600;
    Int32 minutes = (offset % 3600) / 60;

    tb.Append(ZeroPad(hours, 2));
    tb.Append(ZeroPad(minutes, 2));
    return tb.ToString();
}

String DateFormat::GetYearString(
    /* [in] */ Int32 year,
    /* [in] */ Int32 count)
{
    if(count <= 2) {
        return ZeroPad(year % 100, 2);
    }
    else {
        AutoPtr<ILocaleHelper> localeHelper;
        CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
        AutoPtr<ILocale> defaultLocale;
        localeHelper->GetDefault((ILocale**)&defaultLocale);
        //String.format(defaultLocale, "%d", year);
        return StringUtils::ToOctalString(year);
    }
}

Int32 DateFormat::AppendQuotedText(
    /* [in] */ ISpannableStringBuilder* s,
    /* [in] */ Int32 i,
    /* [in] */ Int32 len)
{
    Char32 cc;
    if (i + 1 < len && (ICharSequence::Probe(s)->GetCharAt(i + 1, &cc),
            cc == Elastos::Droid::Text::Format::IDateFormat::QUOTE)) {
        IEditable::Probe(s)->Delete(i, i + 1);
        return 1;
    }

    Int32 count = 0;

    // delete leading quote
    IEditable::Probe(s)->Delete(i, i + 1);
    len--;

    while (i < len) {
        Char32 c;
        ICharSequence::Probe(s)->GetCharAt(i, &c);

        if (c == Elastos::Droid::Text::Format::IDateFormat::QUOTE) {
            //  QUOTEQUOTE -> QUOTE
            if (i + 1 < len && (ICharSequence::Probe(s)->GetCharAt(i + 1, &cc),
                    cc == Elastos::Droid::Text::Format::IDateFormat::QUOTE)) {
                IEditable::Probe(s)->Delete(i, i + 1);
                len--;
                count++;
                i++;
            }
            else {
                //  Closing QUOTE ends quoted text copying
                IEditable::Probe(s)->Delete(i, i + 1);
                break;
            }
        }
        else {
            i++;
            count++;
        }
    }

    return count;
}

String DateFormat::ZeroPad(
    /* [in] */ Int32 inValue,
    /* [in] */ Int32 inMinDigits)
{
    String format("%0");
    format += StringUtils::ToString(inMinDigits) + String("d");
    String value;
    value.AppendFormat(format.string(), inValue);
    return value;
}

} // namespace Format
} // namespace Text
} // namepsace Droid
} // namespace Elastos
