
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/content/res/CResourcesHelper.h"
#include "elastos/droid/text/format/CTimeFormatter.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Libcore::ICU::ILocaleDataHelper;
using Libcore::ICU::CLocaleDataHelper;
using Elastos::Utility::CLocale;
using Elastos::Utility::IFormatter;
using Elastos::Utility::CFormatter;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ITimeZone;
using Elastos::IO::IBuffer;
using Elastos::IO::ICharBufferHelper;
using Elastos::IO::CCharBufferHelper;
using Elastos::Core::IAppendable;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::EIID_IAppendable;
using Elastos::Core::CString;


namespace Elastos {
namespace Droid {
namespace Text {
namespace Format {

const Int32 CTimeFormatter::FORCE_LOWER_CASE = -1;
const Int32 CTimeFormatter::SECSPERMIN = 60;
const Int32 CTimeFormatter::MINSPERHOUR = 60;
const Int32 CTimeFormatter::DAYSPERWEEK = 7;
const Int32 CTimeFormatter::MONSPERYEAR = 12;
const Int32 CTimeFormatter::HOURSPERDAY = 24;
const Int32 CTimeFormatter::DAYSPERLYEAR = 366;
const Int32 CTimeFormatter::DAYSPERNYEAR = 365;

AutoPtr<ILocale> CTimeFormatter::sLocale;
AutoPtr<ILocaleData> CTimeFormatter::sLocaleData;
String CTimeFormatter::sTimeOnlyFormat;
String CTimeFormatter::sDateOnlyFormat;
String CTimeFormatter::sDateTimeFormat;

CAR_INTERFACE_IMPL(CTimeFormatter, Object, ITimeFormatter)

CAR_OBJECT_IMPL(CTimeFormatter)

CTimeFormatter::CTimeFormatter()
{}

CTimeFormatter::~CTimeFormatter()
{}

ECode CTimeFormatter::constructor()
{
    {    AutoLock syncLock(mLock);

        AutoPtr<ILocaleHelper> localeHelper;
        CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
        AutoPtr<ILocale> locale;
        localeHelper->GetDefault((ILocale**)&locale);

        Boolean isEqual = FALSE;
        locale->Equals(sLocale, &isEqual);
        if (sLocale == NULL || !isEqual) {
            sLocale = locale;
            AutoPtr<ILocaleDataHelper> localeDataHelper;
            CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&localeDataHelper);
            localeDataHelper->Get(locale, (ILocaleData**)&sLocaleData);

            AutoPtr<IResources> r;
            AutoPtr<IResourcesHelper> resourcesHelper;
            CResourcesHelper::AcquireSingleton((IResourcesHelper**)&resourcesHelper);
            resourcesHelper->GetSystem((IResources**)&r);

            r->GetString(R::string::time_of_day, &sTimeOnlyFormat);
            r->GetString(R::string::month_day_year, &sDateOnlyFormat);
            r->GetString(R::string::date_and_time, &sDateTimeFormat);
        }

        mDateTimeFormat = sDateTimeFormat;
        mTimeOnlyFormat = sTimeOnlyFormat;
        mDateOnlyFormat = sDateOnlyFormat;
        mLocaleData = sLocaleData;
    }
    return NOERROR;
}

ECode CTimeFormatter::Format(
    /* [in] */ const String& pattern,
    /* [in] */ IZoneInfoWallTime* wallTime,
    /* [in] */ IZoneInfo* zoneInfo,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret)
    // try {
    //java
      // StringBuilder stringBuilder = new StringBuilder();

      // outputBuilder = stringBuilder;
      AutoPtr<StringBuilder> stringBuilder = new StringBuilder();

      // This uses the US locale because number localization is handled separately (see below)
      // and locale sensitive strings are output directly using outputBuilder.
      AutoPtr<ILocaleHelper> localeHelper;
      CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
      AutoPtr<ILocale> us;
      localeHelper->GetUS((ILocale**)&us);
      CFormatter::New((IAppendable*)stringBuilder.Get(), us.Get(), (Elastos::Utility::IFormatter**)&mNumberFormatter);
      FormatInternal(pattern, wallTime, zoneInfo);
      String result;
      stringBuilder->ToString(&result);
      // This behavior is the source of a bug since some formats are defined as being
      // in ASCII and not localized.
      Char32 digit;
      if ((mLocaleData->GetZeroDigit(&digit), digit) != '0') {
        result = LocalizeDigits(result);
      }
      *ret = result;
      return NOERROR;
    // } finally {
      // mOutputBuilder = NULL;
      // mNumberFormatter = NULL;
    // }
}

String CTimeFormatter::LocalizeDigits(
    /* [in] */ const String& s)
{
    Int32 length = s.GetLength();

    Char32 zrDigit;
    mLocaleData->GetZeroDigit(&zrDigit);
    Int32 offsetToLocalizedDigits = zrDigit - '0';
    AutoPtr<StringBuilder> result = new StringBuilder(length);
    for (Int32 i = 0; i < length; ++i) {
        Char32 ch = s.GetChar(i);
        if (ch >= '0' && ch <= '9') {
            ch += offsetToLocalizedDigits;
        }
        result->AppendChar(ch);
    }

    return result->ToString();
}

ECode CTimeFormatter::FormatInternal(
    /* [in] */ const String& pattern,
    /* [in] */ IZoneInfoWallTime* wallTime,
    /* [in] */ IZoneInfo* zoneInfo)
{
    AutoPtr<ICharBufferHelper> formatBufferHelper;
    CCharBufferHelper::AcquireSingleton((ICharBufferHelper**)&formatBufferHelper);
    AutoPtr<ArrayOf<Char32> > pattern_ = pattern.GetChars(0);
    AutoPtr<ICharBuffer> formatBuffer;
    formatBufferHelper->Wrap(pattern_, (ICharBuffer**)&formatBuffer);
    Int32 remaining;
    (IBuffer::Probe(formatBuffer))->GetRemaining(&remaining);
    Int32 position;
    Char32 currentChar;
    while (remaining > 0) {
        Boolean outputCurrentChar = TRUE;
        IBuffer::Probe(formatBuffer)->GetPosition(&position);
        formatBuffer->GetCharAt(position, &currentChar);
        if (currentChar == '%') {
            outputCurrentChar = HandleToken(formatBuffer, wallTime, zoneInfo);
        }

        if (outputCurrentChar) {
            IBuffer::Probe(formatBuffer)->GetPosition(&position);
            formatBuffer->GetCharAt(position, &currentChar);
            mOutputBuilder.AppendChar(currentChar);
        }
        IBuffer::Probe(formatBuffer)->GetPosition(&position);

        return IBuffer::Probe(formatBuffer)->SetPosition(position + 1);
    }
    return NOERROR;
}

Boolean CTimeFormatter::HandleToken(
    /* [in] */ ICharBuffer* formatBuffer,
    /* [in] */ IZoneInfoWallTime* wallTime,
    /* [in] */ IZoneInfo* zoneInfo)
{
    // The char at formatBuffer.position() is expected to be '%' at this point.
    Int32 modifier = 0;
    Int32 remaining;
    IBuffer::Probe(formatBuffer)->GetRemaining(&remaining);
    while (remaining > 1) {
        // Increment the position then get the new current char.
        Int32 position;
        IBuffer::Probe(formatBuffer)->GetPosition(&position);
        IBuffer::Probe(formatBuffer)->SetPosition(position + 1);
        IBuffer::Probe(formatBuffer)->GetPosition(&position);
        Char32 currentChar;
        formatBuffer->Get(position, &currentChar);
        Int32 weekDay;
        wallTime->GetWeekDay(&weekDay);
        Int32 month;
        wallTime->GetMonth(&month);
        Int32 year;
        wallTime->GetYear(&year);
        Int32 monthDay;
        wallTime->GetMonthDay(&monthDay);
        Int32 wtHour;
        wallTime->GetHour(&wtHour);
        Int32 minute;
        wallTime->GetMinute(&minute);
        Int32 second;
        wallTime->GetSecond(&second);
        Int32 wtYearDay;
        wallTime->GetYearDay(&wtYearDay);
        Int32 iDst;
        wallTime->GetIsDst(&iDst);
        AutoPtr<ArrayOf<String> > longWeekdayNames;
        mLocaleData->GetLongWeekdayNames((ArrayOf<String>**)&longWeekdayNames);
        AutoPtr<ArrayOf<String> > shortWeekdayNames;
        mLocaleData->GetShortWeekdayNames((ArrayOf<String>**)&shortWeekdayNames);
        AutoPtr<ArrayOf<String> > longStandAloneMonthNames;
        mLocaleData->GetLongStandAloneWeekdayNames((ArrayOf<String>**)&longStandAloneMonthNames);
        AutoPtr<ArrayOf<String> > longMonthNames;
        mLocaleData->GetLongMonthNames((ArrayOf<String>**)&longMonthNames);
        AutoPtr<ArrayOf<String> > shortMonthNames;
        mLocaleData->GetShortMonthNames((ArrayOf<String>**)&shortMonthNames);

        AutoPtr<ICharSequence> questionMark;
        CString::New(String("?"), (ICharSequence**)&questionMark);
        AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);

        AutoPtr<ArrayOf<String> > amPm;
        mLocaleData->GetAmPm((ArrayOf<String>**)&amPm);
        AutoPtr<ICharSequence> tmpCs;
        AutoPtr<IInteger32> integer;
        String format;
        String outStr;
        AutoPtr<ICharSequence> csOne;
        AutoPtr<ICharSequence> csTwo;
        CString::New((*amPm)[0], (ICharSequence**)&csOne);
        CString::New((*amPm)[1], (ICharSequence**)&csTwo);
        switch (currentChar) {
            case 'A':
                CString::New((*longWeekdayNames)[weekDay + 1], (ICharSequence**)&tmpCs);
                ModifyAndAppend((weekDay < 0
                                || weekDay >= DAYSPERWEEK)
                                ? questionMark.Get() : tmpCs.Get(), modifier);
                return FALSE;
            case 'a':
                CString::New((*shortWeekdayNames)[weekDay + 1], (ICharSequence**)&tmpCs);
                ModifyAndAppend((weekDay < 0
                               || weekDay >= DAYSPERWEEK)
                               ? questionMark.Get() : tmpCs.Get(), modifier);
                return FALSE;
            case 'B':
                if (modifier == '-') {
                    CString::New((*longStandAloneMonthNames)[month], (ICharSequence**)&tmpCs);
                    ModifyAndAppend((month < 0
                                   || month >= MONSPERYEAR)
                                   ? questionMark.Get()
                                   : tmpCs.Get(), modifier);
                } else {
                    CString::New(const_cast<const String&>((*longMonthNames)[month]), (ICharSequence**)&tmpCs);
                    ModifyAndAppend((month < 0
                                   || month >= MONSPERYEAR)
                                   ? questionMark.Get() : tmpCs.Get(), modifier);
                }
                return FALSE;
            case 'b':
            case 'h':
                CString::New((*shortMonthNames)[month], (ICharSequence**)&tmpCs);
                ModifyAndAppend((month < 0 || month >= MONSPERYEAR)
                               ? questionMark.Get() : tmpCs.Get(), modifier);
                return FALSE;
            case 'C':
                OutputYear(year, TRUE, FALSE, modifier);
                return FALSE;
            case 'c':
                FormatInternal(mDateTimeFormat, wallTime, zoneInfo);
                return FALSE;
            case 'D':
                FormatInternal(String("%m/%d/%y"), wallTime, zoneInfo);
                return FALSE;
            case 'd':
                format = GetFormat(modifier, String("%02d"), String("%2d"), String("%d"), String("%02d"));
                CInteger32::New(monthDay, (IInteger32**)&integer);
                array->Set(0, integer);
                mNumberFormatter->Format(format, array);
                return FALSE;
            case 'E':
            case 'O':
                // C99 locale modifiers are not supported.
                continue;
             case '_':
             case '-':
             case '0':
             case '^':
             case '#':
                modifier = currentChar;
                continue;
            case 'e':
                format = GetFormat(modifier, String("%2d"), String("%2d"), String("%d"), String("%02d"));
                CInteger32::New(monthDay, (IInteger32**)&integer);
                array->Set(0, integer);
                mNumberFormatter->Format(format, array);
                return FALSE;
            case 'F':
                FormatInternal(String("%Y-%m-%d"), wallTime, zoneInfo);
                return FALSE;
            case 'H':
                format = GetFormat(modifier, String("%02d"), String("%2d"), String("%d"), String("%02d"));
                CInteger32::New(wtHour, (IInteger32**)&integer);
                array->Set(0, integer);
                mNumberFormatter->Format(format, array);
                return FALSE;
            case 'I': {
                Int32 hour = (wtHour % 12 != 0) ? (wtHour % 12) : 12;
                format = GetFormat(modifier, String("%02d"), String("%2d"), String("%d"), String("%02d"));
                CInteger32::New(hour, (IInteger32**)&integer);
                array->Set(0, integer);
                mNumberFormatter->Format(format, array);
               return FALSE;
            }
            case 'j': {
                Int32 yearDay = wtYearDay + 1;
                format = GetFormat(modifier, String("%03d"), String("%3d"), String("%d"), String("%03d"));
                CInteger32::New(yearDay, (IInteger32**)&integer);
                array->Set(0, integer);
                mNumberFormatter->Format(format, array);
                return FALSE;
            }
            case 'k':
                format = GetFormat(modifier, String("%2d"), String("%2d"), String("%d"), String("%02d"));
                CInteger32::New(wtHour, (IInteger32**)&integer);
                array->Set(0, integer);
                mNumberFormatter->Format(format, array);
                return FALSE;
            case 'l': {
                Int32 n2 = (wtHour % 12 != 0) ? (wtHour % 12) : 12;
                format = GetFormat(modifier, String("%2d"), String("%2d"), String("%d"), String("%02d"));
                CInteger32::New(n2, (IInteger32**)&integer);
                array->Set(0, integer);
                mNumberFormatter->Format(format, array);
                return FALSE;
            }
            case 'M':
                format = GetFormat(modifier, String("%02d"), String("%2d"), String("%d"), String("%02d"));
                CInteger32::New(minute, (IInteger32**)&integer);
                array->Set(0, integer);
                mNumberFormatter->Format(format, array);
                return FALSE;
            case 'm':
                format = GetFormat(modifier, String("%02d"), String("%2d"), String("%d"), String("%02d"));
                CInteger32::New(month + 1, (IInteger32**)&integer);
                array->Set(0, integer);
                mNumberFormatter->Format(format, array);
                return FALSE;
            case 'n':
                mOutputBuilder.AppendChar('\n');
                return FALSE;
            case 'p':
                ModifyAndAppend((wtHour >= (HOURSPERDAY / 2)) ? csTwo.Get()
                       : csOne.Get(), modifier);
                return FALSE;
            case 'P':
                ModifyAndAppend((wtHour >= (HOURSPERDAY / 2)) ? csTwo.Get()
                       : csOne.Get(), FORCE_LOWER_CASE);
                return FALSE;
            case 'R':
                FormatInternal(String("%H:%M"), wallTime, zoneInfo);
                return FALSE;
            case 'r':
                FormatInternal(String("%I:%M:%S %p"), wallTime, zoneInfo);
                return FALSE;
            case 'S':
                format = GetFormat(modifier, String("%02d"), String("%2d"), String("%d"), String("%02d"));
                CInteger32::New(second, (IInteger32**)&integer);
                array->Set(0, integer);
                mNumberFormatter->Format(format, array);
                return FALSE;
            case 's': {
                Int32 timeInSeconds;
                wallTime->Mktime(zoneInfo, &timeInSeconds);
                CInteger32::New(timeInSeconds, (IInteger32**)&integer);
                ((Object*)((CInteger32*)integer.Get()))->ToString(&outStr);
                mOutputBuilder.Append(outStr);
                return FALSE;
            }
            case 'T':
                FormatInternal(String("%H:%M:%S"), wallTime, zoneInfo);
                return FALSE;
            case 't':
                mOutputBuilder.AppendChar('\t');
                return FALSE;
            case 'U':
                format = GetFormat(modifier, String("%02d"), String("%2d"), String("%d"), String("%02d"));
                CInteger32::New((wtYearDay + DAYSPERWEEK - weekDay) / DAYSPERWEEK, (IInteger32**)&integer);
                array->Set(0, integer);
                mNumberFormatter->Format(format, array);
                return FALSE;
            case 'u':
                Int32 day;
                day = (weekDay == 0) ? DAYSPERWEEK : weekDay;
                CInteger32::New(day, (IInteger32**)&integer);
                array->Set(0, integer);
                mNumberFormatter->Format(String("%d"), array);
                return FALSE;
            case 'V':   /* ISO 8601 week number */
            case 'G':   /* ISO 8601 year (four digits) */
            case 'g':   /* ISO 8601 year (two digits) */
            {
                Int32 yday;
                Int32 wday;
               wallTime->GetYearDay(&yday);
               wallTime->GetWeekDay(&wday);
               Int32 w;
               while (TRUE) {
                   Int32 len = IsLeap(year) ? DAYSPERLYEAR : DAYSPERNYEAR;
                   // What yday (-3 ... 3) does the ISO year begin on?
                   Int32 bot = ((yday + 11 - wday) % DAYSPERWEEK) - 3;
                   // What yday does the NEXT ISO year begin on?
                   Int32 top = bot - (len % DAYSPERWEEK);
                   if (top < -3) {
                       top += DAYSPERWEEK;
                   }
                   top += len;
                   if (yday >= top) {
                       ++year;
                       w = 1;
                       break;
                   }
                   if (yday >= bot) {
                       w = 1 + ((yday - bot) / DAYSPERWEEK);
                       break;
                   }
                   --year;
                   yday += IsLeap(year) ? DAYSPERLYEAR : DAYSPERNYEAR;
               }
               if (currentChar == 'V') {
                   format = GetFormat(modifier, String("%02d"), String("%2d"), String("%d"), String("%02d"));
                   CInteger32::New(w, (IInteger32**)&integer);
                   array->Set(0, integer);
                   mNumberFormatter->Format(format, array);
               } else if (currentChar == 'g') {
                   OutputYear(year, FALSE, TRUE, modifier);
               } else {
                   OutputYear(year, TRUE, TRUE, modifier);
               }
               return FALSE;
           }
           case 'v':
               FormatInternal(String("%e-%b-%Y"), wallTime, zoneInfo);
               return FALSE;
           case 'W': {
               Int32 n = (wtYearDay + DAYSPERWEEK - (
                               weekDay != 0 ? (weekDay - 1)
                               : (DAYSPERWEEK - 1))) / DAYSPERWEEK;
               format = GetFormat(modifier, String("%02d"), String("%2d"), String("%d"), String("%02d"));
               CInteger32::New(n, (IInteger32**)&integer);
               array->Set(0, integer);
               mNumberFormatter->Format(format, array);
               return FALSE;
           }
           case 'w':
               CInteger32::New(weekDay, (IInteger32**)&integer);
               array->Set(0, integer);
               mNumberFormatter->Format(String("%d"), array);
               return FALSE;
           case 'X':
               FormatInternal(mTimeOnlyFormat, wallTime, zoneInfo);
               return FALSE;
           case 'x':
               FormatInternal(mDateOnlyFormat, wallTime, zoneInfo);
               return FALSE;
           case 'y':
               OutputYear(year, FALSE, TRUE, modifier);
               return FALSE;
           case 'Y':
               OutputYear(year, TRUE, TRUE, modifier);
               return FALSE;
           case 'Z': {
               if (iDst < 0) {
                   return FALSE;
               }
               Boolean isDst;
               isDst = iDst != 0;
               ITimeZone::Probe(zoneInfo)->GetDisplayName(isDst, ITimeZone::SHORT, &outStr);
               CString::New(outStr, (ICharSequence**)&tmpCs);
               ModifyAndAppend(tmpCs.Get(), modifier);
               return FALSE;
            }
           case 'z': {
               if (iDst < 0) {
                   return FALSE;
               }
               Int32 diff;
               wallTime->GetGmtOffset(&diff);
               Char32 sign;
               if (diff < 0) {
                   sign = '-';
                   diff = -diff;
               } else {
                   sign = '+';
               }
               mOutputBuilder.AppendChar(sign);
               diff /= SECSPERMIN;
               diff = (diff / MINSPERHOUR) * 100 + (diff % MINSPERHOUR);
               format = GetFormat(modifier, String("%04d"), String("%4d"), String("%d"), String("%04d"));
               AutoPtr<IInteger32> integer;
               CInteger32::New(diff, (IInteger32**)&integer);
               array->Set(0, integer);
               mNumberFormatter->Format(format, array);
               return FALSE;
           }
           case '+':
               FormatInternal(String("%a %b %e %H:%M:%S %Z %Y"), wallTime, zoneInfo);
               return FALSE;
           case '%':
               // If conversion char is undefined, behavior is undefined. Print out the
               // character itself.
               return TRUE;
           default:
               return TRUE;
       }
   }
   return TRUE;
}

ECode CTimeFormatter::ModifyAndAppend(
    /* [in] */ ICharSequence* str,
    /* [in] */ Int32 modifier)
{
    VALIDATE_NOT_NULL(str)
    Int32 length;
    str->GetLength(&length);
    Char32 c;
    switch (modifier) {
    case FORCE_LOWER_CASE:
        for (Int32 i = 0; i < length; i++) {
            str->GetCharAt(i,&c);
            mOutputBuilder.AppendChar(BrokenToLower(c));
        }
        break;
    case '^':
        for (Int32 i = 0; i < length; i++) {
            mOutputBuilder.AppendChar(BrokenToUpper(c));
        }
        break;
    case '#':
        for (Int32 i = 0; i < length; i++) {
            str->GetCharAt(i, &c);
            if (BrokenIsUpper(c)) {
                c = BrokenToLower(c);
            } else if (BrokenIsLower(c)) {
                c = BrokenToUpper(c);
            }
            mOutputBuilder.AppendChar(c);
        }
        break;
    default:
        mOutputBuilder.Append(str);
    }
    return NOERROR;
}

ECode CTimeFormatter::OutputYear(
    /* [in] */ Int32 value,
    /* [in] */ Boolean outputTop,
    /* [in] */ Boolean outputBottom,
    /* [in] */ Int32 modifier)
{
    Int32 lead;
    Int32 trail;

    const Int32 DIVISOR = 100;
    trail = value % DIVISOR;
    lead = value / DIVISOR + trail / DIVISOR;
    trail %= DIVISOR;
    if (trail < 0 && lead > 0) {
        trail += DIVISOR;
        --lead;
    } else if (lead < 0 && trail > 0) {
        trail -= DIVISOR;
        ++lead;
    }

    const String& s1 = String("%02d");
    const String& s2 = String("%2d");
    const String& s3 = String("%d");
    const String& s4 = String("%02d");
    AutoPtr<IInteger32> tmp;
AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    if (outputTop) {
        if (lead == 0 && trail < 0) {
            mOutputBuilder.Append("-0");
        } else {
          CInteger32::New(lead, (IInteger32**)&tmp);
          args->Set(0, tmp);
          mNumberFormatter->Format(GetFormat(modifier, s1, s2, s3, s4), args);
        }
    }
    if (outputBottom) {
        Int32 n = ((trail < 0) ? -trail : trail);
        CInteger32::New(n, (IInteger32**)&tmp);
        args->Set(0, tmp);
        mNumberFormatter->Format(GetFormat(modifier, s1, s2, s3, s4), args);
    }
    return NOERROR;
}

String CTimeFormatter::GetFormat(
    /* [in] */ Int32 modifier,
    /* [in] */ const String& normal,
    /* [in] */ const String& underscore,
    /* [in] */ const String& dash,
    /* [in] */ const String& zero)
{
    switch (modifier) {
        case '_':
            return underscore;
        case '-':
            return dash;
        case '0':
            return zero;
    }
    return normal;
}

Boolean CTimeFormatter::IsLeap(
    /* [in] */ Int32 year)
{
    return (((year) % 4) == 0 && (((year) % 100) != 0 || ((year) % 400) == 0));
}

Boolean CTimeFormatter::BrokenIsUpper(
    /* [in] */ Char32 toCheck)
{
    return toCheck >= 'A' && toCheck <= 'Z';
}

Boolean CTimeFormatter::BrokenIsLower(
    /* [in] */ Char32 toCheck)
{
    return toCheck >= 'a' && toCheck <= 'z';
}

Char32 CTimeFormatter::BrokenToLower(
    /* [in] */ Char32 input)
{
    if (input >= 'A' && input <= 'Z') {
        return (Char32) (input - 'A' + 'a');
    }
    return input;
}

Char32 CTimeFormatter::BrokenToUpper(
    /* [in] */ Char32 input)
{
    if (input >= 'a' && input <= 'z') {
        return (Char32) (input - 'a' + 'A');
    }
    return input;
}

} // namespace Elastos
} // namespace Droid
} // namespace Text
} // namespace Format

