
#include "DateIntervalFormat.h"
#include "TimeZone.h"
#include "CLocale.h"
#include "CCalendarHelper.h"
#include "AutoLock.h"
#include "CInteger64.h"
#include "CString.h"
#include <unicode/utypes.h>
#include <unicode/uobject.h>
#include <unicode/locid.h>
#include <unicode/uniset.h>
#include <unicode/dtitvfmt.h>
#include <ElStringByteSink.h>

using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::Utility::CLocale;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;

namespace Libcore {
namespace ICU {

const Int32 DateIntervalFormat::DAY_IN_MS = 24 * 60 * 60 * 1000;
const Int32 DateIntervalFormat::EPOCH_JULIAN_DAY = 2440588;
const AutoPtr<DateIntervalFormat::FormatterCache> DateIntervalFormat::CACHED_FORMATTERS = new DateIntervalFormat::FormatterCache;

DateIntervalFormat::FormatterCache::FormatterCache()
    : BasicLruCache(8)
{}

void DateIntervalFormat::FormatterCache::EntryEvicted(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value)
{
    // TODO: Wait for class BasicLruCache complete
    AutoPtr<IInteger64> iValue = IInteger64::Probe(value);
    if (iValue == NULL) return;
    Int64 para;
    iValue->GetValue(&para);
    DestroyDateIntervalFormat(para);
}

String DateIntervalFormat::FormatDateRange(
    /* [in] */ Int64 startMs,
    /* [in] */ Int64 endMs,
    /* [in] */ Int32 flags,
    /* [in] */ const String& olsonId)
{
    String id = olsonId;
    if ((flags & IDateIntervalFormat::FORMAT_UTC) != 0) {
        id = String("UTC");
    }
    AutoPtr<ITimeZone> tz;
    if (!olsonId.IsNull()) {
        Elastos::Utility::TimeZone::GetTimeZone(olsonId, (ITimeZone**)&tz);
    } else {
        tz = Elastos::Utility::TimeZone::GetDefault();
    }
    return FormatDateRange(CLocale::GetDefault(), tz, startMs, endMs, flags);
}

String DateIntervalFormat::FormatDateRange(
    /* [in] */ ILocale* locale,
    /* [in] */ ITimeZone* tz,
    /* [in] */ Int64 startMs,
    /* [in] */ Int64 endMs,
    /* [in] */ Int32 flags)
{
    AutoPtr<CCalendarHelper> calendarHelper = NULL;
    CCalendarHelper::AcquireSingletonByFriend((CCalendarHelper**)&calendarHelper);
    AutoPtr<ICalendar> startCallendar = NULL;
    calendarHelper->GetInstance(tz, (ICalendar**)&startCallendar);
    startCallendar->SetTimeInMillis(startMs);

    AutoPtr<ICalendar> endCallendar = NULL;
    if (startMs == endMs) {
        endCallendar = startCallendar;
    } else {
        calendarHelper->GetInstance(tz, (ICalendar**)&endCallendar);
        endCallendar->SetTimeInMillis(endMs);
    }

    Boolean endsAtMidnight = IsMidnight(endCallendar);

    // If we're not showing the time or the start and end times are on the same day, and the
    // end time is midnight, fudge the end date so we don't count the day that's about to start.
    // This is not the behavior of icu4c's DateIntervalFormat, but it's the historical behavior
    // of Android's DateUtils.formatDateRange.
    if (startMs != endMs && endsAtMidnight &&
        (((flags & IDateIntervalFormat::FORMAT_SHOW_TIME) == 0) || (DayDistance(startCallendar, endCallendar) <= 1))) {
        endCallendar->Roll(ICalendar::DAY_OF_MONTH, FALSE);
        endMs -= DAY_IN_MS;
    }

    String skeleton = ToSkeleton(startCallendar, endCallendar, flags);
    String rev;
    synchronized(CACHED_FORMATTERS) {
        String s;
        locale->ToString(&s);
        String id;
        tz->GetID(&id);
        rev = FormatDateInterval(GetFormatter(skeleton, s, id), startMs, endMs);
    }
    return rev;
}

Int64 DateIntervalFormat::GetFormatter(
    /* [in] */ const String& skeleton,
    /* [in] */ const String& localeName,
    /* [in] */ const String& tzName)
{
    String key = skeleton + "\t" + localeName + "\t" + tzName;
    AutoPtr<IInterface> tmp;
    AutoPtr<ICharSequence> csKey;
    CString::New(key, (ICharSequence**)&csKey);
    CACHED_FORMATTERS->Get(csKey, (IInterface**)&tmp);
    AutoPtr<IInteger64> iValue = IInteger64::Probe(tmp);

    if (iValue != NULL) {
        Int64 formatter = 0;
        iValue->GetValue(&formatter);
        return formatter;
    }
    Int64 address = CreateDateIntervalFormat(skeleton, localeName, tzName);
    CInteger64::New(address, (IInteger64**)&iValue);
    AutoPtr<IInterface> rstTmp;
    CACHED_FORMATTERS->Put(csKey, iValue, (IInterface**)&rstTmp);
    return address;
}

String DateIntervalFormat::ToSkeleton(
    /* [in] */ ICalendar* startCalendar,
    /* [in] */ ICalendar* endCalendar,
    /* [in] */ Int32 flags)
{
    if ((flags & IDateIntervalFormat::FORMAT_ABBREV_ALL) != 0) {
        flags |= IDateIntervalFormat::FORMAT_ABBREV_MONTH | IDateIntervalFormat::FORMAT_ABBREV_TIME | IDateIntervalFormat::FORMAT_ABBREV_WEEKDAY;
    }

    String monthPart("MMMM");
    if ((flags & IDateIntervalFormat::FORMAT_NUMERIC_DATE) != 0) {
        monthPart = "M";
    } else if ((flags & IDateIntervalFormat::FORMAT_ABBREV_MONTH) != 0) {
        monthPart = "MMM";
    }

    String weekPart("EEEE");
    if ((flags & IDateIntervalFormat::FORMAT_ABBREV_WEEKDAY) != 0) {
        weekPart = "EEE";
    }

    String timePart("j"); // "j" means choose 12 or 24 hour based on current locale.
    if ((flags & IDateIntervalFormat::FORMAT_24HOUR) != 0) {
        timePart = "H";
    } else if ((flags & IDateIntervalFormat::FORMAT_12HOUR) != 0) {
        timePart = "h";
    }

    // If we've not been asked to abbreviate times, or we're using the 24-hour clock (where it
    // never makes sense to leave out the minutes), include minutes. This gets us times like
    // "4 PM" while avoiding times like "16" (for "16:00").
    if ((flags & IDateIntervalFormat::FORMAT_ABBREV_TIME) == 0 || (flags & IDateIntervalFormat::FORMAT_24HOUR) != 0) {
        timePart += "m";
    } else {
        // Otherwise, we're abbreviating a 12-hour time, and should only show the minutes
        // if they're not both "00".
        if (!(OnTheHour(startCalendar) && OnTheHour(endCalendar))) {
            timePart = timePart + "m";
        }
    }

    if (FallOnDifferentDates(startCalendar, endCalendar)) {
        flags |= IDateIntervalFormat::FORMAT_SHOW_DATE;
    }

    if (FallInSameMonth(startCalendar, endCalendar) && (flags & IDateIntervalFormat::FORMAT_NO_MONTH_DAY) != 0) {
        flags &= (~IDateIntervalFormat::FORMAT_SHOW_WEEKDAY);
        flags &= (~IDateIntervalFormat::FORMAT_SHOW_TIME);
    }

    if ((flags & (IDateIntervalFormat::FORMAT_SHOW_DATE | IDateIntervalFormat::FORMAT_SHOW_TIME | IDateIntervalFormat::FORMAT_SHOW_WEEKDAY)) == 0) {
        flags |= IDateIntervalFormat::FORMAT_SHOW_DATE;
    }

    // If we've been asked to show the date, work out whether we think we should show the year.
    if ((flags & IDateIntervalFormat::FORMAT_SHOW_DATE) != 0) {
        if ((flags & IDateIntervalFormat::FORMAT_SHOW_YEAR) != 0) {
            // The caller explicitly wants us to show the year.
        } else if ((flags & IDateIntervalFormat::FORMAT_NO_YEAR) != 0) {
            // The caller explicitly doesn't want us to show the year, even if we otherwise would.
        } else if (!FallInSameYear(startCalendar, endCalendar) || !IsThisYear(startCalendar)) {
            flags |= IDateIntervalFormat::FORMAT_SHOW_YEAR;
        }
    }

    AutoPtr<StringBuilder> builder = new StringBuilder();
    if ((flags & (IDateIntervalFormat::FORMAT_SHOW_DATE | IDateIntervalFormat::FORMAT_NO_MONTH_DAY)) != 0) {
        if ((flags & IDateIntervalFormat::FORMAT_SHOW_YEAR) != 0) {
            builder->Append("y");
        }
        builder->Append(monthPart);
        if ((flags & IDateIntervalFormat::FORMAT_NO_MONTH_DAY) == 0) {
            builder->Append("d");
        }
    }
    if ((flags & IDateIntervalFormat::FORMAT_SHOW_WEEKDAY) != 0) {
        builder->Append(weekPart);
    }
    if ((flags & IDateIntervalFormat::FORMAT_SHOW_TIME) != 0) {
        builder->Append(timePart);
    }
    String s;
    builder->ToString(&s);
    return s;
}

static inline Int32 GetField(
    /* [in] */ ICalendar* c,
    /* [in] */ Int32 field)
{
    Int32 value;
    c->Get(field, &value);
    return value;
}

Boolean DateIntervalFormat::IsMidnight(
    /* [in] */ ICalendar* c)
{
    return GetField(c, ICalendar::HOUR_OF_DAY) ==0 &&
        GetField(c, ICalendar::MINUTE) == 0 &&
        GetField(c, ICalendar::SECOND) == 0 &&
        GetField(c, ICalendar::MILLISECOND);
}

Boolean DateIntervalFormat::OnTheHour(
    /* [in] */ ICalendar* c)
{
    return GetField(c, ICalendar::MINUTE) == 0 && GetField(c, ICalendar::SECOND) == 0;
}

Boolean DateIntervalFormat::FallOnDifferentDates(
    /* [in] */ ICalendar* c1,
    /* [in] */ ICalendar* c2)
{
    return GetField(c1, ICalendar::YEAR) != GetField(c2, ICalendar::YEAR) ||
        GetField(c1, ICalendar::MONTH) != GetField(c2, ICalendar::MONTH) ||
        GetField(c1, ICalendar::DAY_OF_MONTH) != GetField(c2, ICalendar::DAY_OF_MONTH);
}

Boolean DateIntervalFormat::FallInSameMonth(
    /* [in] */ ICalendar* c1,
    /* [in] */ ICalendar* c2)
{
    return GetField(c1, ICalendar::MONTH) == GetField(c2, ICalendar::MONTH);
}

Boolean DateIntervalFormat::FallInSameYear(
    /* [in] */ ICalendar* c1,
    /* [in] */ ICalendar* c2)
{
    return GetField(c1, ICalendar::YEAR) == GetField(c2, ICalendar::YEAR);
}

Boolean DateIntervalFormat::IsThisYear(
    /* [in] */ ICalendar* c)
{
    AutoPtr<ITimeZone> timeZone;
    c->GetTimeZone((ITimeZone**)&timeZone);
    AutoPtr<ICalendar> now;
    AutoPtr<CCalendarHelper> calendarHelper;
    CCalendarHelper::AcquireSingletonByFriend((CCalendarHelper**)&calendarHelper);
    calendarHelper->GetInstance(timeZone, (ICalendar**)&now);
    return GetField(c, ICalendar::YEAR) == GetField(now, ICalendar::YEAR);
}

Int32 DateIntervalFormat::DayDistance(
    /* [in] */ ICalendar* c1,
    /* [in] */ ICalendar* c2)
{
    return JulianDay(c2) - JulianDay(c1);
}

Int32 DateIntervalFormat::JulianDay(
    /* [in] */ ICalendar* c)
{
    Int64 utcMs;
    c->GetTimeInMillis(&utcMs);
    utcMs += GetField(c, ICalendar::ZONE_OFFSET);
    utcMs += GetField(c, ICalendar::DST_OFFSET);
    return (Int32) (utcMs / DateIntervalFormat::DAY_IN_MS) + DateIntervalFormat::EPOCH_JULIAN_DAY;
}

Int64 DateIntervalFormat::CreateDateIntervalFormat(
    /* [in] */ const String& skeleton,
    /* [in] */ const String& localeName,
    /* [in] */ const String& tzName)
{
    if (localeName == NULL){
        return 0;
    }
    NATIVE(Locale) icuLocale;
    icuLocale.setToBogus();
    icuLocale = NATIVE(Locale)::createFromName(localeName);
    if (icuLocale.isBogus()) {
        return 0;
    }

    if (skeleton.IsNull()) {
        return 0;
    }

    UErrorCode status = U_ZERO_ERROR;
    NATIVE(DateIntervalFormat)* formatter(NATIVE(DateIntervalFormat)::createInstance(UnicodeString::fromUTF8(skeleton.string()), icuLocale, status));
    if (!U_SUCCESS(status)) {
    return 0;
    }

    if (tzName.IsNull()) {
      return 0;
    }
    formatter->adoptTimeZone(NATIVE(TimeZone)::createTimeZone(UnicodeString::fromUTF8(tzName.string())));

    return reinterpret_cast<uintptr_t>(formatter);
}

void DateIntervalFormat::DestroyDateIntervalFormat(
    /* [in] */ Int64 address)
{
    delete reinterpret_cast<NATIVE(DateIntervalFormat)*>(address);
}

String DateIntervalFormat::FormatDateInterval(
    /* [in] */ Int64 address,
    /* [in] */ Int64 fromDate,
    /* [in] */ Int64 toDate)
{
    NATIVE(DateIntervalFormat)* formatter(reinterpret_cast<NATIVE(DateIntervalFormat)*>(address));
    DateInterval date_interval(fromDate, toDate);

    UnicodeString s;
    FieldPosition pos = 0;
    UErrorCode status = U_ZERO_ERROR;
    formatter->format(&date_interval, s, pos, status);
    if (!U_SUCCESS(status)) {
        return String(NULL);
    }

    String rev;
    ElStringByteSink sink(&rev);
    s.toUTF8(sink);
    return rev;
}

} // ICU
} // Libcore