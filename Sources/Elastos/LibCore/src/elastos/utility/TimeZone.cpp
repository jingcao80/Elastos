
#include "TimeZone.h"
#include "CSimpleTimeZone.h"
#include "CZoneInfoDB.h"
#include "StringUtils.h"
#include "CString.h"
#include "Pattern.h"
#include "CLocaleHelper.h"
#include "AutoLock.h"
#include "TimeZoneGetter.h"
#include "IoUtils.h"
#include "TimeZoneNames.h"

using Elastos::IO::EIID_ISerializable;
using Elastos::Core::IArrayOf;
using Elastos::Core::EIID_ICloneable;
using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::Regex::Pattern;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IMatchResult;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Libcore::Utility::CZoneInfoDB;
using Libcore::IO::IoUtils;
using Libcore::Utility::IZoneInfo;
using Libcore::ICU::TimeZoneNames;

namespace Elastos {
namespace Utility {

static AutoPtr<ITimeZone> CreateSimpleTimeZone(
    /* [in] */ Int32 offset,
    /* [in] */ const char* name)
{
    AutoPtr<CSimpleTimeZone> ctz;
    CSimpleTimeZone::NewByFriend(offset, String(name), (CSimpleTimeZone**)&ctz);
    assert(ctz != NULL && " Failed to CreateSimpleTimeZone");
    AutoPtr<ITimeZone> tz = (ITimeZone*)ctz;
    return tz;
}

static AutoPtr<IPattern> CreatePattern(
   /* [in] */ const char* name)
{
    AutoPtr<IPattern> pattern;
    Pattern::Compile(String(name), (IPattern**)&pattern);
    return pattern;
}

INIT_PROI_4 AutoPtr<ITimeZone> TimeZone::sGMT = CreateSimpleTimeZone(0, "GMT");
INIT_PROI_4 AutoPtr<ITimeZone> TimeZone::sUTC = CreateSimpleTimeZone(0, "UTC");
INIT_PROI_4 AutoPtr<ITimeZone> TimeZone::sCHINA = CreateSimpleTimeZone(8, "UTC");
INIT_PROI_4 AutoPtr<ITimeZone> TimeZone::sDefaultTimeZone;
INIT_PROI_4 AutoPtr<IPattern> TimeZone::CUSTOM_ZONE_ID_PATTERN = CreatePattern("^GMT[-+](\\d{1,2})(:?(\\d\\d))?$");
INIT_PROI_4 Object TimeZone::sLock;

CAR_INTERFACE_IMPL_3(TimeZone, Object, ITimeZone, ISerializable, ICloneable)

ECode TimeZone::CloneImpl(
    /* [out] */ ITimeZone * other)
{
    VALIDATE_NOT_NULL(other)

    TimeZone* tz = (TimeZone*)other;
    tz->mID = mID;
    return NOERROR;
}

AutoPtr<ArrayOf<String> > TimeZone::GetAvailableIDs()
{
    AutoLock lock(sLock);
    AutoPtr<ArrayOf<String> > array = CZoneInfoDB::GetInstance()->GetAvailableIDs();
    return array;
}

AutoPtr<ArrayOf<String> > TimeZone::GetAvailableIDs(
    /* [in] */ Int32 offsetMillis)
{
    AutoLock lock(sLock);
    AutoPtr<ArrayOf<String> > array = CZoneInfoDB::GetInstance()->GetAvailableIDs(offsetMillis);
    return array;
}

AutoPtr<ITimeZone> TimeZone::GetDefault()
{
    if (sDefaultTimeZone == NULL) {
        AutoPtr<ITimeZoneGetter> tzGetter = TimeZoneGetter::GetInstance();
        String zoneName;
        if (tzGetter != NULL) {
            tzGetter->GetId(&zoneName);
        }
        if (!zoneName.IsNull()) {
            zoneName = zoneName.Trim();
        }
        if (zoneName.IsNull() || zoneName.IsEmpty()) {
            // try {
                // On the host, we can find the configured timezone here.
            ECode ec = IoUtils::ReadFileAsString(String("/etc/timezone"), &zoneName);
            // } catch (IOException ex) {
                // "vogar --mode device" can end up here.
                // TODO: give libcore access to Android system properties and read "persist.sys.timezone".
            if (FAILED(ec)) {
                zoneName = String("GMT");
            }
            // }
        }
        TimeZone::GetTimeZone(zoneName, (ITimeZone**)&sDefaultTimeZone);
    }

    AutoPtr<ITimeZone> tz;
    sDefaultTimeZone->Clone((ITimeZone**)&tz);
    return tz;
}

ECode TimeZone::GetDisplayName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
    AutoPtr<ILocale> locale;
    localeHelper->GetDefault((ILocale**)&locale);
    return GetDisplayName(FALSE, ITimeZone::LONG, locale, name);
}

ECode TimeZone::GetDisplayName(
    /* [in] */ ILocale* locale,
    /* [out] */ String* name)
{
    return GetDisplayName(FALSE, ITimeZone::LONG, locale, name);
}

ECode TimeZone::GetDisplayName(
    /* [in] */ Boolean daylightTime,
    /* [in] */ Int32 style,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
    AutoPtr<ILocale> locale;
    localeHelper->GetDefault((ILocale**)&locale);
    return GetDisplayName(daylightTime, style, locale, name);
}

ECode TimeZone::GetDisplayName(
    /* [in] */ Boolean daylightTime,
    /* [in] */ Int32 style,
    /* [in] */ ILocale* locale,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = String(NULL);

    if (style != ITimeZone::SHORT && style != ITimeZone::LONG) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr< ArrayOf<IArrayOf*> > zoneStrings;
    FAIL_RETURN(TimeZoneNames::GetZoneStrings(locale, (ArrayOf<IArrayOf*>**)&zoneStrings))
    String result;
    FAIL_RETURN(TimeZoneNames::GetDisplayName(zoneStrings, mID, daylightTime, style, &result))
    if (!result.IsNull()) {
        *name = result;
        return NOERROR;
    }

    // If we get here, it's because icu4c has nothing for us. Most commonly, this is in the
    // case of short names. For Pacific/Fiji, for example, icu4c has nothing better to offer
    // than "GMT+12:00". Why do we re-do this work ourselves? Because we have up-to-date
    // time zone transition data, which icu4c _doesn't_ use --- it uses its own baked-in copy,
    // which only gets updated when we update icu4c. http://b/7955614 and http://b/8026776.

    // TODO: should we generate these once, in TimeZoneNames.getDisplayName? Revisit when we
    // upgrade to icu4c 50 and rewrite the underlying native code. See also the
    // "element[j] != null" check in SimpleDateFormat.parseTimeZone, and the extra work in
    // DateFormatSymbols.getZoneStrings.
    Int32 offsetMillis;
    GetRawOffset(&offsetMillis);
    if (daylightTime) {
        Int32 savings;
        GetDSTSavings(&savings);
        offsetMillis += savings;
    }

    *name = CreateGmtOffsetString(
        TRUE /* includeGmt */,
        TRUE /* includeMinuteSeparator */,
        offsetMillis);
    return NOERROR;
}

String TimeZone::CreateGmtOffsetString(
    /* [in] */ Boolean includeGmt,
    /* [in] */ Boolean includeMinuteSeparator,
    /* [in] */ Int32 offsetMillis)
{
    Int32 offsetMinutes = offsetMillis / 60000;

    Char32 sign = '+';
    if (offsetMinutes < 0) {
        sign = '-';
        offsetMinutes = -offsetMinutes;
    }

    StringBuffer builder(9);
    if (includeGmt) {
        builder.Append("GMT");
    }
    builder.AppendChar(sign);
    AppendNumber(&builder, 2, offsetMinutes / 60);
    if (includeMinuteSeparator) {
        builder.AppendChar(':');
    }
    AppendNumber(&builder, 2, offsetMinutes % 60);
    return builder.ToString();
}

void TimeZone::AppendNumber(
    /* [out] */ StringBuffer* buffer,
    /* [in] */ Int32 count,
    /* [in] */ Int32 value)
{
    String string = StringUtils::ToString(value);
    Char32 c = '0';
    for (Int32 i = 0; i < count - string.GetLength(); i++) {
        buffer->AppendChar(c);
    }

    buffer->Append(string);
}

ECode TimeZone::GetID(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mID;
    return NOERROR;
}

ECode TimeZone::GetDSTSavings(
    /* [out] */ Int32* savings)
{
    VALIDATE_NOT_NULL(savings);
    Boolean isUsed;
    UseDaylightTime(&isUsed);
    *savings = isUsed ? 3600000 : 0;
    return NOERROR;
}

ECode TimeZone::GetOffset(
    /* [in] */ Int64 time,
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);

    AutoPtr<IDate> date;
    CDate::New(time, (IDate**)&date);
    Boolean isIn;
    InDaylightTime(date, &isIn);
    Int32 oldoffset;
    GetRawOffset(&oldoffset);

    if (isIn) {
        Int32 dst;
        GetDSTSavings(&dst);
        *offset = oldoffset + dst;
    }
    else {
        *offset = oldoffset;
    }
    return NOERROR;
}

ECode TimeZone::GetTimeZone(
    /* [in] */ const String& id,
    /* [out] */ ITimeZone** timeZone)
{
    VALIDATE_NOT_NULL(timeZone);
    *timeZone = NULL;
    if (id.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoLock lock(sLock);

    // Special cases? These can clone an existing instance.
    // TODO: should we just add a cache to ZoneInfoDB instead?
    if (id.GetLength() == 3) {
        if (id.Equals("GMT")) {
            return sGMT->Clone(timeZone);
        }
        if (id.Equals("UTC")) {
            return sUTC->Clone(timeZone);
        }
    }

    // In the database?
    AutoPtr<IZoneInfo> zi;
    FAIL_RETURN(CZoneInfoDB::GetInstance()->MakeTimeZone(id, (IZoneInfo**)&zi))
    AutoPtr<ITimeZone> zone = ITimeZone::Probe(zi);
    // Custom time zone?
    if (zone == NULL && id.GetLength() > 3 && id.StartWith("GMT")) {
        zone = GetCustomTimeZone(id);
    }

    // We never return null; on failure we return the equivalent of "GMT".
    if (zone != NULL) {
        *timeZone = zone;
        REFCOUNT_ADD(*timeZone);
        return NOERROR;
    }

    return sGMT->Clone(timeZone);
}

AutoPtr<ITimeZone> TimeZone::GetCustomTimeZone(
    /* [in] */ const String& id)
{
    AutoPtr<IMatcher> m;
    CUSTOM_ZONE_ID_PATTERN->Matcher(id, (IMatcher**)&m);
    Boolean isMatches = FALSE;
    m->Matches(&isMatches);
    if (!isMatches) {
        return NULL;
    }

    Int32 hour = 0;
    Int32 minute = 0;
    String grstr;
    (IMatchResult::Probe(m))->Group(1 , &grstr);
    hour = StringUtils::ParseInt32(grstr);
    (IMatchResult::Probe(m))->Group(3, &grstr);
    if (!grstr.IsNull()) {
        minute = StringUtils::ParseInt32(grstr);
    }

    if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
       return NULL;
    }
    Char32 sign = id.GetChar(3);
    Int32 raw = (hour * 3600000) + (minute * 60000);
    if (sign == '-') {
       raw = -raw;
    }

    String cleanId("");
    cleanId.AppendFormat("GMT%c%02d:%02d", sign, hour, minute);
    AutoPtr<ITimeZone> tz;
    CSimpleTimeZone::New(raw, cleanId , (ITimeZone**)&tz);
    return tz;
}

ECode TimeZone::HasSameRules(
    /* [in] */ ITimeZone* timezone,
    /* [out] */ Boolean* hasRules)
{
    VALIDATE_NOT_NULL(hasRules);
    *hasRules = FALSE;
    VALIDATE_NOT_NULL(timezone);

    Int32 offset, toffset;
    GetRawOffset(&offset);
    timezone->GetRawOffset(&toffset);
    *hasRules = (offset == toffset);
    return NOERROR;
}

ECode TimeZone::SetDefault(
    /* [in] */ ITimeZone* timeZone)
{
    AutoLock lock(sLock);

    sDefaultTimeZone = NULL;
    if (timeZone != NULL) {
        timeZone->Clone((ITimeZone**)&sDefaultTimeZone);
    }

    return NOERROR;
}

ECode TimeZone::SetID(
    /* [in] */ const String& id)
{
    if (id.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }

    mID = id;
    return NOERROR;
}

AutoPtr<ITimeZone> TimeZone::GMT()
{
    AutoPtr<ITimeZone> tz;
    sGMT->Clone((ITimeZone**)&tz);
    return tz;
}

AutoPtr<ITimeZone> TimeZone::UTC()
{
    AutoPtr<ITimeZone> tz;
    sUTC->Clone((ITimeZone**)&tz);
    return tz;
}

} // Utility
} // Elastos
