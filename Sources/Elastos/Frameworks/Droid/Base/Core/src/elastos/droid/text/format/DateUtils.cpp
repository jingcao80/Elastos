
#include <Elastos.CoreLibrary.Libcore.h>
#include "elastos/droid/text/format/DateUtils.h"
#include "elastos/droid/text/format/DateFormat.h"
#include "elastos/droid/text/format/CTime.h"
#include "elastos/droid/content/res/CResourcesHelper.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Libcore::ICU::ILocaleData;
using Libcore::ICU::ILocaleDataHelper;
using Libcore::ICU::CLocaleDataHelper;
using Libcore::ICU::IDateIntervalFormat;
using Libcore::ICU::CDateIntervalFormat;
using Elastos::Core::CString;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::IAppendable;
using Elastos::Core::CInteger64;
using Elastos::Core::IInteger64;
using Elastos::Core::EIID_IAppendable;
using Elastos::Text::IDateFormat;
using Elastos::Text::IDateFormatHelper;
using Elastos::Text::CDateFormatHelper;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocale;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::IFormatter;
using Elastos::Utility::CFormatter;   //java.util.Formatter
using Elastos::Utility::CGregorianCalendar;
using Elastos::Utility::IGregorianCalendar;
using Elastos::Utility::IDate;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::ITimeZone;


namespace Elastos {
namespace Droid {
namespace Text {
namespace Format {

// This table is used to lookup the resource string id of a format string
// used for formatting a start and end date that fall in the same year.
// The index is constructed from a bit-wise OR of the boolean values:
// {showTime, showYear, showWeekDay}.  For example, if showYear and
// showWeekDay are both true, then the index would be 3.
/** @deprecated do not use. */
const AutoPtr<ArrayOf<Int32> > DateUtils::sSameYearTable = NULL;

// This table is used to lookup the resource string id of a format string
// used for formatting a start and end date that fall in the same month.
// The index is constructed from a bit-wise OR of the boolean values:
// {showTime, showYear, showWeekDay}.  For example, if showYear and
// showWeekDay are both true, then the index would be 3.
/** @deprecated do not use. */
const AutoPtr<ArrayOf<Int32> > DateUtils::sSameMonthTable = NULL;

Object DateUtils::sLock;
AutoPtr<IConfiguration> DateUtils::sLastConfig = NULL;
String DateUtils::sElapsedFormatMMSS = String(NULL);
String DateUtils::sElapsedFormatHMMSS = String(NULL);

AutoPtr<ITime> DateUtils::sNowTime = NULL;
AutoPtr<ITime> DateUtils::sThenTime = NULL;

Object DateUtils::sLockDateUtilsClass;

String DateUtils::GetDayOfWeekString(
/* [in] */ Int32 dayOfWeek,
/* [in] */ Int32 abbrev)
{
    AutoPtr<ILocaleHelper> localeHelp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelp);
    AutoPtr<ILocale> defaultLocale;
    localeHelp->GetDefault((ILocale**)&defaultLocale);

    AutoPtr<ILocaleDataHelper> ldh;
    CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&ldh);
    AutoPtr<ILocaleData> d;
    ldh->Get(defaultLocale, (ILocaleData**)&d);

    AutoPtr< ArrayOf<String> > names;
    switch (abbrev) {
        case IDateUtils::LENGTH_LONG:{
            d->GetLongWeekdayNames((ArrayOf<String>**)&names);
            break;
        }
        case IDateUtils::LENGTH_MEDIUM:{
            d->GetShortWeekdayNames((ArrayOf<String>**)&names);
            break;
        }
        case IDateUtils::LENGTH_SHORT:{
            d->GetShortWeekdayNames((ArrayOf<String>**)&names);
            break;
        }
        case IDateUtils::LENGTH_SHORTER:{
            d->GetShortWeekdayNames((ArrayOf<String>**)&names);
            break;
        }
        case IDateUtils::LENGTH_SHORTEST:{
            d->GetTinyWeekdayNames((ArrayOf<String>**)&names);
            break;
        }
        default:{
            d->GetShortWeekdayNames((ArrayOf<String>**)&names);
            break;
        }
    }
    return (*(names))[dayOfWeek];
}

String DateUtils::GetAMPMString(
        /* [in] */ Int32 ampm)
{
    AutoPtr<ILocaleHelper> localeHelp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelp);
    AutoPtr<ILocale> defaultLocale;
    localeHelp->GetDefault((ILocale**)&defaultLocale);

    AutoPtr<ILocaleDataHelper> ldh;
    CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&ldh);
    AutoPtr<ILocaleData> d;
    ldh->Get(defaultLocale, (ILocaleData**)&d);

    AutoPtr< ArrayOf<String> > amPm;
    d->GetAmPm( (ArrayOf<String>**)&amPm );
    return (*amPm)[ampm - ICalendar::AM];
}

String DateUtils::GetMonthString(
    /* [in] */ Int32 month,
    /* [in] */ Int32 abbrev)
{
    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
    AutoPtr<ILocale> defaultLocale;
    localeHelper->GetDefault((ILocale**)&defaultLocale);

    AutoPtr<ILocaleDataHelper> ldh;
    CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&ldh);
    AutoPtr<ILocaleData> d;
    ldh->Get(defaultLocale, (ILocaleData**)&d);

    AutoPtr< ArrayOf<String> > names;
    switch (abbrev) {
        case IDateUtils::LENGTH_LONG:{
            d->GetLongMonthNames((ArrayOf<String>**)&names);
            break;
        }
        case IDateUtils::LENGTH_MEDIUM:{
            d->GetShortMonthNames((ArrayOf<String>**)&names);
            break;
        }
        case IDateUtils::LENGTH_SHORT:{
            d->GetShortMonthNames((ArrayOf<String>**)&names);
            break;
        }
        case IDateUtils::LENGTH_SHORTER:{
            d->GetShortMonthNames((ArrayOf<String>**)&names);
            break;
        }
        case IDateUtils::LENGTH_SHORTEST:{
            d->GetTinyMonthNames((ArrayOf<String>**)&names);
            break;
        }
        default:{
            d->GetShortMonthNames((ArrayOf<String>**)&names);
            break;
        }
    }
    if(names != NULL)
    {
        Int32 namesLen = names->GetLength();
        if( 0 <= month && month < namesLen ) {
            return (*names)[month];
        }
    }
    return String("");
}

String DateUtils::GetStandaloneMonthString(
    /* [in] */ Int32 month,
    /* [in] */ Int32 abbrev)
{
    // Note that here we use d.shortMonthNames for MEDIUM, SHORT and SHORTER.
    // This is a shortcut to not spam the translators with too many variations
    // of the same string.  If we find that in a language the distinction
    // is necessary, we can can add more without changing this API.
    AutoPtr<ILocaleHelper> localeHelp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelp);
    AutoPtr<ILocale> defaultLocale;
    localeHelp->GetDefault((ILocale**)&defaultLocale);

    AutoPtr<ILocaleDataHelper> ldh;
    CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&ldh);
    AutoPtr<ILocaleData> d;
    ldh->Get(defaultLocale, (ILocaleData**)&d);

    AutoPtr< ArrayOf<String> > names;
    switch (abbrev) {
        case IDateUtils::LENGTH_LONG:{
            d->GetLongStandAloneMonthNames((ArrayOf<String>**)&names);
            break;
        }
        case IDateUtils::LENGTH_MEDIUM:{
            d->GetShortMonthNames((ArrayOf<String>**)&names);
            break;
        }
        case IDateUtils::LENGTH_SHORT:{
            d->GetShortMonthNames((ArrayOf<String>**)&names);
            break;
        }
        case IDateUtils::LENGTH_SHORTER:{
            d->GetShortMonthNames((ArrayOf<String>**)&names);
            break;
        }
        case IDateUtils::LENGTH_SHORTEST:{
            d->GetTinyMonthNames((ArrayOf<String>**)&names);
            break;
        }
        default:{
            d->GetShortMonthNames((ArrayOf<String>**)&names);
            break;
        }
    }
    return (*names)[month];
}

AutoPtr<ICharSequence> DateUtils::GetRelativeTimeSpanString(
    /* [in] */ Int64 startTime)
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 millis;
    system->GetCurrentTimeMillis(&millis);
    return GetRelativeTimeSpanString(startTime, millis, IDateUtils::MINUTE_IN_MILLIS);
}

AutoPtr<ICharSequence> DateUtils::GetRelativeTimeSpanString(
    /* [in] */ Int64 time,
    /* [in] */ Int64 now,
    /* [in] */ Int64 minResolution)
{
    Int32 flags = IDateUtils::FORMAT_SHOW_DATE | IDateUtils::FORMAT_SHOW_YEAR | IDateUtils::FORMAT_ABBREV_MONTH;
    return GetRelativeTimeSpanString(time, now, minResolution, flags);
}

AutoPtr<ICharSequence> DateUtils::GetRelativeTimeSpanString(
    /* [in] */ Int64 time,
    /* [in] */ Int64 now,
    /* [in] */ Int64 minResolution,
    /* [in] */ Int32 flags)
{
    AutoPtr<IResources> r = CResources::GetSystem();
    Boolean abbrevRelative = ( (flags & (IDateUtils::FORMAT_ABBREV_RELATIVE | IDateUtils::FORMAT_ABBREV_ALL)) != 0 );

    Boolean past = (now >= time);
    Int64 duration = Elastos::Core::Math::Abs(now - time);

    Int32 resId;
    Int64 count;
    AutoPtr<ICharSequence> cs;
    if ( (duration < IDateUtils::MINUTE_IN_MILLIS) && (minResolution < IDateUtils::MINUTE_IN_MILLIS) ) {
        count = duration / IDateUtils::SECOND_IN_MILLIS;
        if (past) {
            if (abbrevRelative) {
                resId = R::plurals::abbrev_num_seconds_ago;
            }
            else {
                resId = R::plurals::num_seconds_ago;
            }
        }
        else {
            if (abbrevRelative) {
                resId = R::plurals::abbrev_in_num_seconds;
            }
            else {
                resId = R::plurals::in_num_seconds;
            }
        }
    }
    else if ( (duration < IDateUtils::HOUR_IN_MILLIS) && (minResolution < IDateUtils::HOUR_IN_MILLIS) ) {
        count = duration / IDateUtils::MINUTE_IN_MILLIS;
        if (past) {
            if (abbrevRelative) {
                resId = R::plurals::abbrev_num_minutes_ago;
            }
            else {
                resId = R::plurals::num_minutes_ago;
            }
        }
        else {
            if (abbrevRelative) {
                resId = R::plurals::abbrev_in_num_minutes;
            }
            else {
                resId = R::plurals::in_num_minutes;
            }
        }
    }
    else if (duration < IDateUtils::DAY_IN_MILLIS && minResolution < IDateUtils::DAY_IN_MILLIS) {
        count = duration / IDateUtils::HOUR_IN_MILLIS;
        if (past) {
            if (abbrevRelative) {
                resId = R::plurals::abbrev_num_hours_ago;
            }
            else {
                resId = R::plurals::num_hours_ago;
            }
        }
        else {
            if (abbrevRelative) {
                resId = R::plurals::abbrev_in_num_hours;
            }
            else {
                resId = R::plurals::in_num_hours;
            }
        }
    }
    else if (duration < IDateUtils::WEEK_IN_MILLIS && minResolution < IDateUtils::WEEK_IN_MILLIS) {
        String rdret = GetRelativeDayString(r, time, now);
        CString::New(rdret, (ICharSequence**)&cs);
        return cs;
    } else {
        // We know that we won't be showing the time, so it is safe to pass
        // in a null context.
        String fdrRet = FormatDateRange(NULL, time, time, flags);
        CString::New(fdrRet, (ICharSequence**)&cs);
        return cs;
    }

    String format;
    r->GetQuantityString(resId, (Int32) count, &format);

    String retVal;
    retVal.AppendFormat(format.string(), count);

    CString::New(retVal, (ICharSequence**)&cs);
    return cs;
}

AutoPtr<ICharSequence> DateUtils::GetRelativeDateTimeString(
    /* [in] */ IContext* c,
    /* [in] */ Int64 time,
    /* [in] */ Int64 minResolution,
    /* [in] */ Int64 transitionResolution,
    /* [in] */ Int32 flags)
{
    AutoPtr<IResources> r = CResources::GetSystem();
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

    Int64 now;
    system->GetCurrentTimeMillis(&now);
    Int64 duration = Elastos::Core::Math::Abs(now - time);

    // getRelativeTimeSpanString() doesn't correctly format relative dates
    // above a week or exact dates below a day, so clamp
    // transitionResolution as needed.
    if (transitionResolution > IDateUtils::WEEK_IN_MILLIS) {
        transitionResolution = IDateUtils::WEEK_IN_MILLIS;
    }
    else if (transitionResolution < IDateUtils::DAY_IN_MILLIS) {
        transitionResolution = IDateUtils::DAY_IN_MILLIS;
    }

    String fdr = FormatDateRange(c, time, time, IDateUtils::FORMAT_SHOW_TIME);
    AutoPtr<ICharSequence> timeClause;
    CString::New(fdr, (ICharSequence**)&timeClause);

    String result;
    if (duration < transitionResolution) {
        AutoPtr<ICharSequence> relativeClause = GetRelativeTimeSpanString(time, now, minResolution, flags);
        String raw;
        r->GetString(R::string::relative_time, &raw);
        String strRelativeClause, strTimeClause;
        relativeClause->ToString(&strRelativeClause);
        timeClause->ToString(&strTimeClause);
        assert(0 && "TODO");
//        result = String.format(raw, strRelativeClause, strTimeClause);
    }
    else {
        AutoPtr<ICharSequence> dateClause = GetRelativeTimeSpanString(c, time, FALSE);
        String raw;
        r->GetString(R::string::date_time, &raw);
        String strDateClause, strTimeClause;
        dateClause->ToString(&strDateClause);
        timeClause->ToString(&strTimeClause);
//        result = String.format(raw, strDateClause, strTimeClause);
    }

    AutoPtr<ICharSequence> cs;
    CString::New(result, (ICharSequence**)&cs);
    return cs;
}

String DateUtils::GetRelativeDayString(
    /* [in] */ IResources* r,
    /* [in] */ Int64 day,
    /* [in] */ Int64 today)
{
    AutoPtr<IConfiguration> config;
    r->GetConfiguration((IConfiguration**)&config);
    AutoPtr<ILocale> locale;
    config->GetLocale((ILocale**)&locale);
    if (NULL == locale) {
        AutoPtr<ILocaleHelper> localeHelper;
        CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
        localeHelper->GetDefault((ILocale**)&locale);
    }
    // TODO: use TimeZone.getOffset instead.
    AutoPtr<ITime> startTime;
    CTime::New((ITime**)&startTime);
    startTime->Set(day);
    Int64 startTimeGmtoff;
    startTime->GetGmtoff(&startTimeGmtoff);

    AutoPtr<ITime> currentTime;
    CTime::New((ITime**)&currentTime);
    currentTime->Set(today);
    Int64 currentTimeGmtoff;
    currentTime->GetGmtoff(&currentTimeGmtoff);

    Int32 startDay = CTime::GetJulianDay(day, startTimeGmtoff);
    Int32 currentDay = CTime::GetJulianDay(today, currentTimeGmtoff);

    Int32 days = Elastos::Core::Math::Abs(currentDay - startDay);
    Boolean past = (today > day);

    // TODO: some locales name other days too, such as de_DE's "Vorgestern" (today - 2).
    AutoPtr<ILocaleDataHelper> ldHelper;
    CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&ldHelper);
    AutoPtr<ILocaleData> ld;
    ldHelper->Get(locale, (ILocaleData**)&ld);
    if (days == 1) {
        if (past) {
            String strYesterday;
            ld->GetYesterday(&strYesterday);
            return strYesterday;
        }
        else {
            String strTomorrow;
            ld->GetTomorrow(&strTomorrow);
            return strTomorrow;
        }
    }
    else if (days == 0) {
        String strToday;
        ld->GetToday(&strToday);
        return strToday;
    }

    Int32 resId;
    if (past) {
        resId = R::plurals::num_days_ago;
    }
    else {
        resId = R::plurals::in_num_days;
    }

    String format;
    r->GetQuantityString(resId, days, &format);

    String retVal;
   // retVal = String.format(format, days);
    return retVal;
}

ECode DateUtils::InitFormatStrings()
{
    AutoLock lock(sLock);
    InitFormatStringsLocked();
    return NOERROR;
}

ECode DateUtils::InitFormatStringsLocked()
{
    AutoPtr<IResources> r = CResources::GetSystem();
    AutoPtr<IConfiguration> cfg;
    r->GetConfiguration((IConfiguration**)&cfg);
    Boolean bLastConfigEquals;
    if (sLastConfig == NULL || (sLastConfig->Equals(cfg.Get(), &bLastConfigEquals), !bLastConfigEquals))
    {
       sLastConfig = cfg;
       r->GetString(R::string::elapsed_time_short_format_mm_ss, &sElapsedFormatMMSS);
       r->GetString(R::string::elapsed_time_short_format_h_mm_ss, &sElapsedFormatHMMSS);
    }
    return NOERROR;
}

AutoPtr<ICharSequence> DateUtils::FormatDuration(
    /* [in] */ Int64 millis)
{
    AutoPtr<IResourcesHelper> helper;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&helper);
    AutoPtr<IResources> res;
    helper->GetSystem((IResources**)&res);
    String rStr;
    if (millis >= IDateUtils::HOUR_IN_MILLIS) {
        Int32 hours = (Int32) ((millis + 1800000) / IDateUtils::HOUR_IN_MILLIS);
        res->GetQuantityString(
                R::plurals::duration_hours, hours, &rStr);
    } else if (millis >= IDateUtils::MINUTE_IN_MILLIS) {
        Int32 minutes = (Int32) ((millis + 30000) / IDateUtils::MINUTE_IN_MILLIS);
        res->GetQuantityString(
                R::plurals::duration_minutes, minutes, &rStr);
    } else {
        Int32 seconds = (Int32) ((millis + 500) / IDateUtils::SECOND_IN_MILLIS);
        res->GetQuantityString(
                R::plurals::duration_seconds, seconds, &rStr);
    }
    AutoPtr<ICharSequence> rst;
    CString::New(rStr, (ICharSequence**)&rst);
    return rst;
}

String DateUtils::FormatElapsedTime(
    /* [in] */ Int64 elapsedSeconds)
{
    return FormatElapsedTime(String(""), elapsedSeconds);
}

String DateUtils::FormatElapsedTime(
    /* [in] */ const String& recycle,
    /* [in] */ Int64 elapsedSeconds)
{
    Int64 hours = 0;
    Int64 minutes = 0;
    Int64 seconds = 0;

    if (elapsedSeconds >= 3600) {
        hours = elapsedSeconds / 3600;
        elapsedSeconds -= hours * 3600;
    }
    if (elapsedSeconds >= 60) {
        minutes = elapsedSeconds / 60;
        elapsedSeconds -= minutes * 60;
    }
    seconds = elapsedSeconds;

    // Create a StringBuilder if we weren't given one to recycle.
    // TODO: if we cared, we could have a thread-local temporary StringBuilder.
    AutoPtr<StringBuilder> sb = new StringBuilder(recycle);
    Int32 sbLen;
    sb->GetLength(&sbLen);
    if (sbLen == 0) {
        sb = new StringBuilder(8);
    } else {
        sb->SetLength(0);
    }

    // Format the broken-down time in a locale-appropriate way.
    // TODO: use icu4c when http://unicode.org/cldr/trac/ticket/3407 is fixed.
    AutoPtr<ILocaleHelper> localeHelp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelp);
    AutoPtr<ILocale> defaultLocale;
    localeHelp->GetDefault((ILocale**)&defaultLocale);
    using Elastos::Utility::IFormatter;
    AutoPtr<IFormatter> f;
    CFormatter::New(IAppendable::Probe(sb), defaultLocale, (IFormatter**)&f);
    InitFormatStrings();

    AutoPtr<IInteger64> hours_, minutes_, seconds_;
    CInteger64::New(hours, (IInteger64**)&hours_);
    CInteger64::New(minutes, (IInteger64**)&minutes_);
    CInteger64::New(seconds, (IInteger64**)&seconds_);

    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    args->Set(0, hours_);
    args->Set(1, minutes_);
    args->Set(2, seconds_);

    AutoPtr<ArrayOf<IInterface*> > args_ = ArrayOf<IInterface*>::Alloc(2);
    args_->Set(0, hours_);
    args_->Set(1, minutes_);
    String ret;
    if (hours > 0) {
        f->Format(sElapsedFormatHMMSS, args);
        f->ToString(&ret);
        return ret;
    } else {
        f->Format(sElapsedFormatMMSS, args_);
        f->ToString(&ret);
        return ret;
    }
}

AutoPtr<ICharSequence> DateUtils::FormatSameDayTime(
    /* [in] */ Int64 then,
    /* [in] */ Int64 now,
    /* [in] */ Int32 dateStyle,
    /* [in] */ Int32 timeStyle)
{
    AutoPtr<ICalendar> thenCal;
    CGregorianCalendar::New((ICalendar**)&thenCal);
    thenCal->SetTimeInMillis(then);
    AutoPtr<IDate> thenDate;
    thenCal->GetTime((IDate**)&thenDate);
    AutoPtr<ICalendar> nowCal;
    CGregorianCalendar::New((ICalendar**)&nowCal);
    nowCal->SetTimeInMillis(now);

    AutoPtr<Elastos::Text::IDateFormat> f;

    Int32 thenYear,nowYear,thenMonth,nowMonth,thenDay,nowDay;
    thenCal->Get(ICalendar::YEAR, &thenYear);
    nowCal->Get(ICalendar::YEAR, &nowYear);
    thenCal->Get(ICalendar::MONTH, &thenMonth);
    nowCal->Get(ICalendar::MONTH, &nowMonth);
    thenCal->Get(ICalendar::DAY_OF_MONTH, &thenDay);
    nowCal->Get(ICalendar::DAY_OF_MONTH, &nowDay);

    AutoPtr<IDateFormatHelper> helper;
    CDateFormatHelper::AcquireSingleton((IDateFormatHelper**)&helper);
    if (thenYear == nowYear && thenMonth == nowMonth && thenDay == nowDay) {
        helper->GetTimeInstance(timeStyle, (Elastos::Text::IDateFormat**)&f);
    }
    else {
        helper->GetDateInstance(dateStyle, (Elastos::Text::IDateFormat**)&f);
    }

    String ret;
    f->Format(thenDate, &ret);
    AutoPtr<ICharSequence> cs;
    CString::New(ret, (ICharSequence**)&cs);
    return cs;
}

Boolean DateUtils::IsToday(
    /* [in] */ Int64 when)
{
    AutoPtr<ITime> time;
    CTime::New((ITime**)&time);
    time->Set(when);

    Int32 thenYear,thenMonth,thenMonthDay;
    time->GetYear(&thenYear);
    time->GetMonth(&thenMonth);
    time->GetMonthDay(&thenMonthDay);

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 millis;
    system->GetCurrentTimeMillis(&millis);

    time->Set(millis);
    Int32 newYear, newMonth, newMonthDay;
    time->GetYear(&newYear);
    time->GetMonth(&newMonth);
    time->GetMonthDay(&newMonthDay);

    return (thenYear == newYear)
            && (thenMonth == newMonth)
            && (thenMonthDay == newMonthDay);
}

String DateUtils::FormatDateRange(
    /* [in] */ IContext* context,
    /* [in] */ Int64 startMillis,
    /* [in] */ Int64 endMillis,
    /* [in] */ Int32 flags)
{
    StringBuilder s(50);
    AutoPtr<ILocaleHelper> localeHelp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelp);
    AutoPtr<ILocale> locale;
    localeHelp->GetDefault((ILocale**)&locale);
    AutoPtr<Elastos::Utility::IFormatter> f;
    AutoPtr<IAppendable> appendable = (IAppendable*)s.Probe(EIID_IAppendable);
    CFormatter::New(appendable.Get(), locale.Get(), (Elastos::Utility::IFormatter**)&f);
    AutoPtr<Elastos::Utility::IFormatter> fRet = FormatDateRange(context, f, startMillis, endMillis, flags);
    String ret;
    fRet->ToString(&ret);
    return ret;
}

AutoPtr<Elastos::Utility::IFormatter> DateUtils::FormatDateRange(
    /* [in] */ IContext* context,
    /* [in] */ Elastos::Utility::IFormatter* formatter,
    /* [in] */ Int64 startMillis,
    /* [in] */ Int64 endMillis,
    /* [in] */ Int32 flags)
{
    return FormatDateRange(context, formatter, startMillis, endMillis, flags, String(NULL));
}

AutoPtr<Elastos::Utility::IFormatter> DateUtils::FormatDateRange(
    /* [in] */ IContext* context,
    /* [in] */ Elastos::Utility::IFormatter* formatter,
    /* [in] */ Int64 startMillis,
    /* [in] */ Int64 endMillis,
    /* [in] */ Int32 flags,
    /* [in] */ const String& timeZone)
{
    // If we're being asked to format a time without being explicitly told whether to use
    // the 12- or 24-hour clock, icu4c will fall back to the locale's preferred 12/24 format,
    // but we want to fall back to the user's preference.
    if ((flags & (IDateUtils::FORMAT_SHOW_TIME
        | IDateUtils::FORMAT_12HOUR
        | IDateUtils::FORMAT_24HOUR))
        == IDateUtils::FORMAT_SHOW_TIME) {
        flags |= DateFormat::Is24HourFormat(context)
              ? IDateUtils::FORMAT_24HOUR
              : IDateUtils::FORMAT_12HOUR;
    }

    AutoPtr<IDateIntervalFormat> dIntervalFormat;
    CDateIntervalFormat::AcquireSingleton((IDateIntervalFormat**)&dIntervalFormat);
    String range;
    dIntervalFormat->FormatDateRange(startMillis, endMillis, flags, timeZone, &range);
    // try {
        AutoPtr<IAppendable> appendable;
        formatter->GetOut((IAppendable**)&appendable);
        AutoPtr<ICharSequence> cs;
        CString::New(range, (ICharSequence**)&cs);
        appendable->Append(cs);
    // } catch () {
        // throw new AssertionError(impossible);
        // return E_ASSERTION_ERROR;
    // }
    return formatter;
}

String DateUtils::FormatDateTime(
    /* [in] */ IContext* context,
    /* [in] */ Int64 millis,
    /* [in] */ Int32 flags)
{
    return FormatDateRange(context, millis, millis, flags);
}

AutoPtr<ICharSequence> DateUtils::GetRelativeTimeSpanString(
    /* [in] */ IContext* c,
    /* [in] */ Int64 millis,
    /* [in] */ Boolean withPreposition)
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

    String result;
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    Int64 span = Elastos::Core::Math::Abs(now - millis);

    synchronized(sLockDateUtilsClass) {
        if (sNowTime == NULL) {
            CTime::New( (ITime**)&sNowTime );
        }

        if (sThenTime == NULL) {
            CTime::New( (ITime**)&sThenTime );
        }

        sNowTime->Set(now);
        sThenTime->Set(millis);

        Int32 nowWeekDay, thenWeekDay, nowYear, thenYear;
        sNowTime->GetWeekDay(&nowWeekDay);
        sThenTime->GetWeekDay(&thenWeekDay);
        sNowTime->GetYear(&nowYear);
        sThenTime->GetYear(&thenYear);

        Int32 prepositionId;
        if (span < IDateUtils::DAY_IN_MILLIS && nowWeekDay == thenWeekDay) {
            // Same day
            Int32 flags = IDateUtils::FORMAT_SHOW_TIME;
            result = FormatDateRange(c, millis, millis, flags);
            assert(0 && "TODO");
            prepositionId = R::string::preposition_for_time;
        }
        else if (nowYear != thenYear) {
            // Different years
            Int32 flags = IDateUtils::FORMAT_SHOW_DATE | IDateUtils::FORMAT_SHOW_YEAR | IDateUtils::FORMAT_NUMERIC_DATE;
            result = FormatDateRange(c, millis, millis, flags);

            // This is a date (like "10/31/2008" so use the date preposition)
            prepositionId = R::string::preposition_for_date;
        }
        else {
            // Default
            Int32 flags = IDateUtils::FORMAT_SHOW_DATE | IDateUtils::FORMAT_ABBREV_MONTH;
            result = FormatDateRange(c, millis, millis, flags);
            prepositionId = R::string::preposition_for_date;
        }
        if (withPreposition) {
            AutoPtr<IResources> res;
            c->GetResources((IResources**)&res);
            String raw;
            res->GetString(prepositionId, &raw);
            result = raw;
        }
    }

    AutoPtr<ICharSequence> cs;
    CString::New(result, (ICharSequence**)&cs);
    return cs;
}

AutoPtr<ICharSequence> DateUtils::GetRelativeTimeSpanString(
    /* [in] */ IContext* c,
    /* [in] */ Int64 millis)
{
    return GetRelativeTimeSpanString(c, millis, FALSE /* no preposition */);
}

} // namespace Format
} // namespace Text
} // namepsace Droid
} // namespace Elastos
