
#include <Elastos.CoreLibrary.External.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/utility/TimeUtils.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/Math.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/os/SystemClock.h"
// #include "elastos/droid/content/res/CResourcesHelper.h"
// #include "elastos/droid/R.h"

using Libcore::Utility::IZoneInfoDB;
using Libcore::Utility::CZoneInfoDB;
using Libcore::Utility::ITzData;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IDate;
using Elastos::Utility::CDate;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Org::Xmlpull::V1::IXmlPullParser;
//using Elastos::Droid::R;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesHelper;
//using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Internal::Utility::XmlUtils;

namespace Elastos {
namespace Droid {
namespace Utility {

/** @hide Field length that can hold 999 days of time */
const Int32 TimeUtils::HUNDRED_DAY_FIELD_LEN = 19;

const Int32 TimeUtils::SECONDS_PER_MINUTE = 60;
const Int32 TimeUtils::SECONDS_PER_HOUR = 60 * 60;
const Int32 TimeUtils::SECONDS_PER_DAY = 24 * 60 * 60;
const Int64 TimeUtils::NANOS_PER_MS = 1000000;

Object TimeUtils::sFormatSync;
AutoPtr<ArrayOf<Char32> > TimeUtils::sFormatStr = ArrayOf<Char32>::Alloc(19 /* HUNDRED_DAY_FIELD_LEN */ + 5);

const Int64 TimeUtils::LARGEST_DURATION = (1000 * IDateUtils::DAY_IN_MILLIS) - 1;

const Boolean TimeUtils::DBG = FALSE;
const String TimeUtils::TAG("TimeUtils");

/** Cached results of getTineZones */
Object TimeUtils::sLastLockObj;
AutoPtr<ArrayOf<ITimeZone *> > TimeUtils::sLastZones;
String TimeUtils::sLastCountry;

/** Cached results of getTimeZonesWithUniqueOffsets */
Object TimeUtils::sLastUniqueLockObj;
AutoPtr<ArrayOf<ITimeZone *> > TimeUtils::sLastUniqueZoneOffsets;
String TimeUtils::sLastUniqueCountry;


AutoPtr<ITimeZone> TimeUtils::GetTimeZone(
    /* [in] */ Int32 offset,
    /* [in] */ Boolean dst,
    /* [in] */ Int64 when,
    /* [in] */ const String& country)
{
    AutoPtr<ITimeZone> best;

    AutoPtr<IDate> d;
    CDate::New(when, (IDate**)&d);

    AutoPtr<ITimeZoneHelper> tzHelper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzHelper);

    AutoPtr<ITimeZone> current;
    tzHelper->GetDefault((ITimeZone**)&current);

    String currentName;
    Int32 currentOffset;
    Boolean currentDst;
    current->GetID(&currentName);
    current->GetOffset(when, &currentOffset);
    current->InDaylightTime(d, &currentDst);

    String tzId;
    Int32 tzOffset;
    Boolean tzDst;
    AutoPtr<ArrayOf<ITimeZone *> > timezones = GetTimeZones(country);
    if (timezones != NULL) {
        for (Int32 i = 0; i < timezones->GetLength(); ++i) {
            AutoPtr<ITimeZone> tz = (*timezones)[i];

            // If the current time zone is from the right country
            // and meets the other known properties, keep it
            // instead of changing to another one.
            tz->GetID(&tzId);
            if (tzId.Equals(currentName)) {
                if (currentOffset == offset && currentDst == dst) {
                    return current;
                }
            }

            // Otherwise, take the first zone from the right
            // country that has the correct current offset and DST.
            // (Keep iterating instead of returning in case we
            // haven't encountered the current time zone yet.)

            if (best == NULL) {
                tz->GetOffset(when, &tzOffset);
                tz->InDaylightTime(d, &tzDst);
                if (tzOffset == offset && tzDst == dst) {
                    best = tz;
                }
            }
        }
    }

    return best;
}

AutoPtr<ArrayOf<ITimeZone *> > TimeUtils::GetTimeZonesWithUniqueOffsets(
    /* [in] */ const String& country)
{
    synchronized(sLastUniqueLockObj) {
        if ((!country.IsNull()) && country.Equals(sLastUniqueCountry)) {
            if (DBG) {
                Logger::D(TAG, "getTimeZonesWithUniqueOffsets(%s): return cached version", country.string());
            }
            return sLastUniqueZoneOffsets;
        }
    }

    AutoPtr<ArrayOf<ITimeZone *> > zones = GetTimeZones(country);

    if (!zones) {
        AutoLock lock(sLastUniqueLockObj);
        sLastUniqueZoneOffsets = NULL;
        sLastUniqueCountry = NULL;
        return NULL;
    }

    Int32 offset1, offset2, count = 0;
    String id;
    AutoPtr<ArrayOf<ITimeZone *> > uniqueTimeZones = ArrayOf<ITimeZone *>::Alloc(zones->GetLength());
    for (Int32 i = 0; i < zones->GetLength(); ++i) {
        AutoPtr<ITimeZone> tz = (*zones)[i];
        tz->GetRawOffset(&offset1);

        // See if we already have this offset,
        // Using slow but space efficient and these are small.
        Boolean found = FALSE;
        for (Int32 j = 0; j < uniqueTimeZones->GetLength(); j++) {
            AutoPtr<ITimeZone> ttz = (*uniqueTimeZones)[j];
            if (ttz != NULL) {
                ttz->GetRawOffset(&offset2);
                if (offset1 == offset2) {
                    found = TRUE;
                    break;
                }
            }
        }

        if (found == FALSE) {
            if (DBG) {
                tz->GetID(&id);
                Logger::D(TAG, "getTimeZonesWithUniqueOffsets: add unique offset=%d zone->GetID=",
                    offset1, id.string());
            }

            uniqueTimeZones->Set(count++, tz);
        }
    }

    synchronized(sLastUniqueLockObj) {
        // Cache the last result
        sLastUniqueZoneOffsets = uniqueTimeZones;
        sLastUniqueCountry = country;
    }

    return sLastUniqueZoneOffsets;
}

AutoPtr<ArrayOf<ITimeZone *> > TimeUtils::GetTimeZones(
    /* [in] */ const String& country)
{
    synchronized(sLastLockObj) {
        if (!country.IsNull() && country.Equals(sLastCountry)) {
            if (DBG) Logger::D(TAG, "getTimeZones(): return cached version", country.string());
            return sLastZones;
        }
    }

    if (country == NULL) {
        if (DBG) Logger::D(TAG, "getTimeZones(NULL): return empty list");
        return NULL;
    }

    // AutoPtr<IResourcesHelper> helper;
    // CResourcesHelper::AcquireSingleton((IResourcesHelper**)&helper);
    // AutoPtr<IResources> r;
    // helper->GetSystem((IResources**)&r);
    AutoPtr<IXmlResourceParser> xrp;
    //TODO r->GetXml(R::xml::time_zones_by_country, (IXmlResourceParser**)&xrp);
    IXmlPullParser* parser = IXmlPullParser::Probe(xrp);

    const String strTimeZones("timezones");
    const String strCode("code");
    const String strGMT("GMT");

    AutoPtr<ITimeZoneHelper> tzHelper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzHelper);

    // try {
    XmlUtils::BeginDocument(parser, strTimeZones);

    List<AutoPtr<ITimeZone> > list;
    String element, code, zoneIdString, tzId, nullStr;
    Int32 nextId;
    while (TRUE) {
        XmlUtils::NextElement(parser);

        parser->GetName(&element);
        if (element.IsNull() || !(element.Equals(strTimeZones))) {
            break;
        }

        parser->GetAttributeValue(nullStr, strCode, &code);

        if (country.Equals(code)) {
            parser->Next(&nextId);
            if (nextId == IXmlPullParser::TEXT) {
                parser->GetText(&zoneIdString);
                AutoPtr<ITimeZone> tz;
                tzHelper->GetTimeZone(zoneIdString, (ITimeZone**)&tz);

                if (tz != NULL) {
                    tz->GetID(&tzId);
                    if (tzId.StartWith(strGMT) == FALSE) {
                        // tz->GetID doesn't start not "GMT" so its valid
                        list.PushBack(tz);
                        if (DBG) {
                            Logger::D(TAG, "getTimeZone('%s'): found tz->GetID==%s",
                                country.string(), tzId.string());
                        }
                    }
                }
                else {
                    if (DBG) {
                        Logger::D(TAG, "getTimeZone('%s'): found tz->GetID==<no tz>",
                            country.string());
                    }
                }
            }
        }
    }
    // } catch (XmlPullParserException e) {
    //     Log.e(TAG, "Got xml parser exception getTimeZone('" + country + "'): e=", e);
    // } catch (IOException e) {
    //     Log.e(TAG, "Got IO exception getTimeZone('" + country + "'): e=", e);
    // } finally {
    //     parser.close();
    // }

    ICloseable::Probe(parser)->Close();

    synchronized(sLastLockObj) {

        // Cache the last result;
        sLastZones = ArrayOf<ITimeZone*>::Alloc(list.GetSize());
        List<AutoPtr<ITimeZone> >::Iterator it = list.Begin();
        for (Int32 i = 0; it != list.End(); ++it, ++i) {
            sLastZones->Set(i, *it);
        }

        sLastCountry = country;
    }
    return sLastZones;
}

String TimeUtils::GetTimeZoneDatabaseVersion()
{
    AutoPtr<IZoneInfoDB> db;
    CZoneInfoDB::AcquireSingleton((IZoneInfoDB**)&db);
    AutoPtr<ITzData> tzData;
    db->GetInstance((ITzData**)&tzData);

    String version;
    tzData->GetVersion(&version);
    return version;
}

Int32 TimeUtils::AccumField(
    /* [in] */ Int32 amt,
    /* [in] */ Int32 suffix,
    /* [in] */ Boolean always,
    /* [in] */ Int32 zeropad)
{
    if (amt > 99 || (always && zeropad >= 3)) {
        return 3 + suffix;
    }
    if (amt > 9 || (always && zeropad >= 2)) {
        return 2 + suffix;
    }
    if (always || amt > 0) {
        return 1 + suffix;
    }
    return 0;
}

Int32 TimeUtils::PrintField(
    /* [in] */ ArrayOf<Char32>* formatStr,
    /* [in] */ Int32 amt,
    /* [in] */ Char32 suffix,
    /* [in] */ Int32 pos,
    /* [in] */ Boolean always,
    /* [in] */ Int32 zeropad)
{
    if (always || amt > 0) {
        Int32 startPos = pos;
        if ((always && zeropad >= 3) || amt > 99) {
            Int32 dig = amt / 100;
            (*formatStr)[pos] = (char)(dig + '0');
            pos++;
            amt -= (dig * 100);
        }
        if ((always && zeropad >= 2) || amt > 9 || startPos != pos) {
            Int32 dig = amt/10;
            (*formatStr)[pos] = (char)(dig + '0');
            pos++;
            amt -= (dig * 10);
        }
        (*formatStr)[pos] = (char)(amt + '0');
        pos++;
        (*formatStr)[pos] = suffix;
        pos++;
    }
    return pos;
}

Int32 TimeUtils::FormatDurationLocked(
    /* [in] */ Int64 duration,
    /* [in] */ Int32 fieldLen)
{
    if (sFormatStr->GetLength() < fieldLen) {
        sFormatStr = ArrayOf<Char32>::Alloc(fieldLen);
    }

    AutoPtr<ArrayOf<Char32> > formatStr = sFormatStr;

    if (duration == 0) {
        Int32 pos = 0;
        fieldLen -= 1;
        while (pos < fieldLen) {
            (*formatStr)[pos++] = ' ';
        }
        (*formatStr)[pos] = '0';
        return pos + 1;
    }

    char prefix;
    if (duration > 0) {
        prefix = '+';
    } else {
        prefix = '-';
        duration = -duration;
    }

    if (duration > LARGEST_DURATION) {
        duration = LARGEST_DURATION;
    }

    Int32 millis = (Int32)(duration%1000);
    Int32 seconds = (Int32) Elastos::Core::Math::Floor(duration / 1000);
    Int32 days = 0, hours = 0, minutes = 0;

    if (seconds > SECONDS_PER_DAY) {
        days = seconds / SECONDS_PER_DAY;
        seconds -= days * SECONDS_PER_DAY;
    }
    if (seconds > SECONDS_PER_HOUR) {
        hours = seconds / SECONDS_PER_HOUR;
        seconds -= hours * SECONDS_PER_HOUR;
    }
    if (seconds > SECONDS_PER_MINUTE) {
        minutes = seconds / SECONDS_PER_MINUTE;
        seconds -= minutes * SECONDS_PER_MINUTE;
    }

    Int32 pos = 0;

    if (fieldLen != 0) {
        Int32 myLen = AccumField(days, 1, FALSE, 0);
        myLen += AccumField(hours, 1, myLen > 0, 2);
        myLen += AccumField(minutes, 1, myLen > 0, 2);
        myLen += AccumField(seconds, 1, myLen > 0, 2);
        myLen += AccumField(millis, 2, true, myLen > 0 ? 3 : 0) + 1;
        while (myLen < fieldLen) {
            (*formatStr)[pos] = ' ';
            pos++;
            myLen++;
        }
    }

    (*formatStr)[pos] = prefix;
    pos++;

    Int32 start = pos;
    Boolean zeropad = fieldLen != 0;
    pos = PrintField(formatStr, days, 'd', pos, FALSE, 0);
    pos = PrintField(formatStr, hours, 'h', pos, pos != start, zeropad ? 2 : 0);
    pos = PrintField(formatStr, minutes, 'm', pos, pos != start, zeropad ? 2 : 0);
    pos = PrintField(formatStr, seconds, 's', pos, pos != start, zeropad ? 2 : 0);
    pos = PrintField(formatStr, millis, 'm', pos, true, (zeropad && pos != start) ? 3 : 0);
    (*formatStr)[pos] = 's';
    return pos + 1;
}

/** @hide Just for debugging; not internationalized. */
void TimeUtils::FormatDuration(
    /* [in] */ Int64 duration,
    /* [in] */ StringBuilder& builder)
{
    AutoLock lock(sFormatSync);
    Int32 len = FormatDurationLocked(duration, 0);
    builder.Append(*sFormatStr, 0, len);
}

    /** @hide Just for debugging; not internationalized. */
void TimeUtils::FormatDuration(
    /* [in] */ Int64 duration,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 fieldLen)
{
    assert(pw != NULL);

    AutoLock lock(sFormatSync);
    Int32 len = FormatDurationLocked(duration, fieldLen);
    String str(*sFormatStr, 0, len);
    pw->Print(str);
}

/** @hide Just for debugging; not internationalized. */
void TimeUtils::FormatDuration(
    /* [in] */ Int64 duration,
    /* [in] */ IPrintWriter* pw)
{
    FormatDuration(duration, pw, 0);
}

/** @hide Just for debugging; not internationalized. */
void TimeUtils::FormatDuration(
    /* [in] */ Int64 time,
    /* [in] */ Int64 now,
    /* [in] */ IPrintWriter* pw)
{
    if (time == 0) {
        pw->Print(String("--"));
        return;
    }
    FormatDuration(time - now, pw, 0);
}

/** @hide Just for debugging; not internationalized. */
String TimeUtils::FormatUptime(
    /* [in] */ Int64 time)
{
    StringBuilder sb;
    sb += time;
    Int64 diff = time - SystemClock::GetUptimeMillis();
    if (diff > 0) {
        sb += " (in ";
        sb += diff;
        sb += " ms)";
        return sb.ToString();
    }
    if (diff < 0) {
        sb += " (";
        sb += -diff;
        sb += " ms ago";
        return sb.ToString();
    }

    sb += " (now)";
    return sb.ToString();
}

String TimeUtils::LogTimeOfDay(
    /* [in] */ Int64 millis)
{
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ICalendar> c;
    helper->GetInstance((ICalendar**)&c);
    if (millis >= 0) {
        c->SetTimeInMillis(millis);
        Int32 y, m, d, h, mi, s, ms;
        c->Get(ICalendar::YEAR, &y);
        c->Get(ICalendar::MONTH, &m);
        c->Get(ICalendar::DATE, &d);
        c->Get(ICalendar::HOUR, &h);
        c->Get(ICalendar::MINUTE, &mi);
        c->Get(ICalendar::SECOND, &s);
        c->Get(ICalendar::MILLISECOND, &ms);
        String str;
        str.AppendFormat("%d-%2d-%2d %2d:%2d:%2d %06d", y, m, d, h, mi, s, ms);
        return str;
    }
    else {
        return StringUtils::ToString(millis);
    }
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos
