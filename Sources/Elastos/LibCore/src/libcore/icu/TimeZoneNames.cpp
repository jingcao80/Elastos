
#include "TimeZoneNames.h"
#include "TimeZone.h"
#include "ElStringByteSink.h"
#include "CLocaleHelper.h"
#include "CArrayOf.h"
#include "CString.h"
#include "CSystem.h"
#include "CZoneInfoDB.h"
#include "StringUtils.h"
#include <UniquePtr.h>
#include "AutoLock.h"
#include <elastos/utility/etl/Vector.h>
#include <elastos/utility/etl/List.h>
#include <unicode/smpdtfmt.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "unicode/tznames.h"
#include "unicode/locid.h"
#include "CLocale.h"
#include <cutils/log.h>

using Elastos::Core::CSystem;
using Elastos::Core::CArrayOf;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::Core::EIID_ICharSequence;
using Elastos::Core::EIID_IArrayOf;
using Elastos::Core::CString;
using Elastos::Utility::TimeZone;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::CLocale;
using Elastos::Utility::Etl::Vector;
using Elastos::Utility::Etl::List;
using Libcore::Utility::IZoneInfoDB;
using Libcore::Utility::CZoneInfoDB;
using Libcore::Utility::ITzData;

namespace Libcore {
namespace ICU {

Boolean SetStringArrayElement(ArrayOf<String>* array, Int32 i, const UnicodeString& s)
{
    // Fill in whatever we got. We don't use the display names if they're "GMT[+-]xx:xx"
    // because icu4c doesn't use the up-to-date time zone transition data, so it gets these
    // wrong. TimeZone.getDisplayName creates accurate names on demand.
    // TODO: investigate whether it's worth doing that work once in the Java wrapper instead of on-demand.
    static const UnicodeString kGmt("GMT", 3, US_INV);
    if (!s.isBogus() && !s.startsWith(kGmt)) {
        String str("");
        ElStringByteSink sink(&str);
        s.toUTF8(sink);
        (*array)[i] = str;
    }
    return true;
}

static bool isUtc(const UnicodeString& id) {
    static UnicodeString etcUct("Etc/UCT", 7, US_INV);
    static UnicodeString etcUtc("Etc/UTC", 7, US_INV);
    static UnicodeString etcUniversal("Etc/Universal", 13, US_INV);
    static UnicodeString etcZulu("Etc/Zulu", 8, US_INV);

    static UnicodeString uct("UCT", 3, US_INV);
    static UnicodeString utc("UTC", 3, US_INV);
    static UnicodeString universal("Universal", 9, US_INV);
    static UnicodeString zulu("Zulu", 4, US_INV);

    return id == etcUct || id == etcUtc || id == etcUniversal || id == etcZulu ||
            id == uct || id == utc || id == universal || id == zulu;
}

//===============================================================
// TimeZoneNames::ZoneStringsCache
//===============================================================
TimeZoneNames::ZoneStringsCache::ZoneStringsCache()
    : BasicLruCache(5)
{}

TimeZoneNames::ZoneStringsCache::~ZoneStringsCache()
{
}

AutoPtr<IInterface> TimeZoneNames::ZoneStringsCache::Create(
    /* [in] */ IInterface* key)
{
    AutoPtr<CSystem> system;
    CSystem::AcquireSingletonByFriend((CSystem**)&system);

    Int64 start = 0, nativeStart = 0;
    system->GetCurrentTimeMillis(&start);
    AutoPtr<ArrayOf<StringArray > > result = ArrayOf<StringArray >::Alloc(sAvailableTimeZoneIds->GetLength());
    for (Int32 i = 0; i < sAvailableTimeZoneIds->GetLength(); ++i) {
        AutoPtr<ArrayOf<String> > subArr = ArrayOf<String>::Alloc(5);
        subArr->Set(0, (*sAvailableTimeZoneIds)[i]);
        result->Set(i, subArr);
    }

    system->GetCurrentTimeMillis(&nativeStart);
    String locname;
    AutoPtr<ILocale> locale = ILocale::Probe(key);
    locale->ToString(&locname);
    FillZoneStrings(locname, result);

    Int64 nativeEnd;
    system->GetCurrentTimeMillis(&nativeEnd);
    InternStrings(result);
    // Ending up in this method too often is an easy way to make your app slow, so we ensure
    // it's easy to tell from the log (a) what we were doing, (b) how long it took, and
    // (c) that it's all ICU's fault.
    Int64 end;
    system->GetCurrentTimeMillis(&end);
    Int64 nativeDuration = nativeEnd - nativeStart;
    Int64 duration = end - start;
    ALOGI("Loaded time zone names for \"%s\" in %lld ms (%lld ms in ICU)", locname.string(), duration, nativeDuration);
    return ArrayOfToInterface(result);
}

void TimeZoneNames::ZoneStringsCache::InternStrings(
    /* [in] */ ArrayOf<StringArray>* result)
{
    HashMap<String, String> internTable;
    AutoLock lock(this);
    HashMap<String, String>::Iterator it;
    ArrayOf<String> * subArr;
    for (Int32 i = 0; i < result->GetLength(); ++i) {
        subArr = (*result)[i];
        for (Int32 j = 1; j < ITimeZoneNames::NAME_COUNT; ++j) {
            String original = (*subArr)[j];
            it = internTable.Find(original);
            if (it == internTable.End()) {
                internTable[original] = original;
            }
            else {
                (*subArr)[j] = it->mSecond;
            }
        }
    }
}

//===============================================================
// TimeZoneNames
//===============================================================
static AutoPtr<TimeZoneNames::ZoneStringsCache> CreateCache()
{
    AutoPtr<TimeZoneNames::ZoneStringsCache> cache = new TimeZoneNames::ZoneStringsCache();
    // // Ensure that we pull in the zone strings for the root locale, en_US, and the
    // // user's default locale. (All devices must support the root locale and en_US,
    // // and they're used for various system things like HTTP headers.) Pre-populating
    // // the cache is especially useful on Android because we'll share this via the Zygote.
    AutoPtr<IInterface> tmp;
    cache->Get(CLocale::ROOT, (IInterface**)&tmp);
    tmp = NULL;
    cache->Get(CLocale::US, (IInterface**)&tmp);
    tmp = NULL;
    cache->Get(CLocale::GetDefault(), (IInterface**)&tmp);
    return cache;
}

AutoPtr<IInterface> TimeZoneNames::ArrayOfToInterface(
    /* [in] */ ArrayOf< AutoPtr< ArrayOf<String> > >* array)
{
    if (array == NULL) return NULL;
    AutoPtr<IArrayOf> iArray;
    CArrayOf::New(EIID_IArrayOf, array->GetLength(), (IArrayOf**)&iArray);
    for(Int32 i = 0; i < array->GetLength(); i++) {
        if ((*array)[i] == NULL) {
            iArray->Set(i, NULL);
            continue;
        }
        AutoPtr<IArrayOf> subArr;
        CArrayOf::New(EIID_ICharSequence, (*array)[i]->GetLength(), (IArrayOf**)&subArr);
        for (Int32 j = 0; j < (*array)[i]->GetLength(); j++) {
            String str = (*((*array)[i]))[j];
            AutoPtr<ICharSequence> cs;
            CString::New(str, (ICharSequence**)&cs);
            subArr->Set(j, cs);
        }
        iArray->Set(i, subArr);
    }
    return iArray;
}

AutoPtr<ArrayOf< AutoPtr< ArrayOf<String> > > > TimeZoneNames::InterfaceToArray(
    /* [in] */ IArrayOf* iArray)
{
    if (iArray == NULL) return NULL;
    Int32 len;
    iArray->GetLength(&len);
    AutoPtr<ArrayOf< AutoPtr< ArrayOf<String> > > > result = ArrayOf< AutoPtr< ArrayOf<String> > >::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> tmp;
        iArray->Get(i, (IInterface**)&tmp);
        AutoPtr<IArrayOf> iSubArray = IArrayOf::Probe(tmp);
        if (iSubArray == NULL) {
            (*result)[i] = NULL;
            continue;
        }
        Int32 subLen;
        iSubArray->GetLength(&subLen);
        AutoPtr<ArrayOf<String> > subArr = ArrayOf<String>::Alloc(subLen);
        for (Int32 j = 0; j < subLen; j++) {
            tmp = NULL;
            iSubArray->Get(j, (IInterface**)&tmp);
            AutoPtr<ICharSequence> cs = ICharSequence::Probe(tmp);
            if (cs == NULL) {
                (*subArr)[j] = NULL;
                continue;
            }
            String str;
            cs->ToString(&str);
            (*subArr)[j] = str;
        }
        result->Set(i, subArr);
    }
    return result;
}

INIT_PROI_5 AutoPtr<ArrayOf<String> > TimeZoneNames::sAvailableTimeZoneIds = Elastos::Utility::TimeZone::GetAvailableIDs();

INIT_PROI_5 AutoPtr<TimeZoneNames::ZoneStringsCache> TimeZoneNames::sCachedZoneStrings = CreateCache();

ECode TimeZoneNames::GetDisplayName(
    /* [in] */ ArrayOf<IArrayOf*>*  zoneStrings,
    /* [in] */ const String& id,
    /* [in] */ Boolean daylight,
    /* [in] */ Int32 style ,
    /* [out] */ String * str)
{
    VALIDATE_NOT_NULL(str)
    *str = String(NULL);
    VALIDATE_NOT_NULL(zoneStrings)

    Int32 index = -1;

    Int32 length = zoneStrings->GetLength();
    for (Int32 i = 0; i < length; ++i) {
        AutoPtr<IArrayOf> row = (*zoneStrings)[i];
        if (row) {
            AutoPtr<IInterface> outface;
            row->Get(ITimeZoneNames::OLSON_NAME, (IInterface**)&outface);

            if (ICharSequence::Probe(outface) != NULL) {
                String outstr;
                ICharSequence::Probe(outface)->ToString(&outstr);
                if (outstr.Equals(id)) {
                    index = i;
                    break;
                }
            }
        }
    }

    if (index >= 0) {
        AutoPtr<IArrayOf> row = (*zoneStrings)[index];
        AutoPtr<IInterface> outface;
        if (daylight) {
            if (style == ITimeZone::LONG) {
                row->Get(ITimeZoneNames::LONG_NAME_DST, (IInterface**)&outface);
            }
            else {
                row->Get(ITimeZoneNames::SHORT_NAME_DST, (IInterface**)&outface);
            }
        }
        else {
            if (style == ITimeZone::LONG) {
                row->Get(ITimeZoneNames::LONG_NAME, (IInterface**)&outface);
            }
            else {
                row->Get(ITimeZoneNames::SHORT_NAME, (IInterface**)&outface);
            }
        }
        if (ICharSequence::Probe(outface) != NULL) {
            return ICharSequence::Probe(outface)->ToString(str);
        }
    }
    *str = String(NULL);
    return NOERROR;
}

ECode TimeZoneNames::GetZoneStrings(
    /* [in] */ ILocale * l,
    /* [out, callee] */ ArrayOf<IArrayOf*> ** outarray)
{
    VALIDATE_NOT_NULL(outarray)
    *outarray = NULL;

    AutoPtr<ILocale> locale = l;
    if (locale == NULL) {
        AutoPtr<CLocaleHelper> lh;
        CLocaleHelper::AcquireSingletonByFriend((CLocaleHelper**)&lh);
        lh->GetDefault((ILocale**)&locale);
    }

    AutoPtr<IInterface> cache;
    sCachedZoneStrings->Get(locale, (IInterface**)&cache);
    AutoPtr< ArrayOf< AutoPtr< ArrayOf<String> > > > result = InterfaceToArray(IArrayOf::Probe(cache));
    Int32 length = result->GetLength();
    AutoPtr< ArrayOf<IArrayOf*> > resarr = ArrayOf<IArrayOf*>::Alloc(length);
    ArrayOf<String>* subArr;
    for (Int32 i = 0; i < length; ++i) {
        subArr = (*result)[i];
        Int32 midlen = subArr->GetLength();
        AutoPtr<IArrayOf> arrof;
        FAIL_RETURN(CArrayOf::New(EIID_ICharSequence, midlen, (IArrayOf**)&arrof));
        for (Int32 j = 0; j < midlen; ++j) {
            AutoPtr<ICharSequence> sq;
            CString::New((*subArr)[j], (ICharSequence**)&sq);
            arrof->Set(j, sq);
        }
        resarr->Set(i, arrof);
    }
    *outarray = resarr;
    REFCOUNT_ADD(*outarray);

    return NOERROR;
}

ECode TimeZoneNames::ForLocale(
    /* [in] */ ILocale * locale ,
    /* [out, callee] */ ArrayOf<String> ** outarray)
{
    VALIDATE_NOT_NULL(locale)
    VALIDATE_NOT_NULL(outarray)

    String countryCode;
    locale->GetCountry(&countryCode);
    List<String> ids;
    AutoPtr<IZoneInfoDB> db;

    CZoneInfoDB::AcquireSingleton((IZoneInfoDB**)&db);
    AutoPtr<ITzData> tzData;
    db->GetInstance((ITzData**)&tzData);
    String zoneTab;
    tzData->GetZoneTab(&zoneTab);
    ArrayOf<String> * array;
    StringUtils::Split(zoneTab, "\n", &array);
    String line;
    for (Int32 i = 0; i < array->GetLength(); ++i) {
        line = (*array)[i];
        if (line.StartWith(countryCode)) {
            Int32 olsonIdStart = line.IndexOf('\t', 4) + 1;
            Int32 olsonIdEnd = line.IndexOf('\t', olsonIdStart);
            if (-1 == olsonIdEnd) {
                olsonIdEnd = line.GetLength();
            }
            ids.PushBack(line.Substring(olsonIdStart, olsonIdEnd));
        }
    }


    AutoPtr<ArrayOf<String> > res = ArrayOf<String>::Alloc(ids.GetSize());
    Int32 count = 0;
    for (List<String>::Iterator it = ids.Begin(); it != ids.End(); ++it) {
        (*res)[count++] = *it;
    }
    *outarray = res;
    REFCOUNT_ADD(*outarray);
    return NOERROR;
}

ECode TimeZoneNames::GetExemplarLocation(
    /* [in] */ const String& locale,
    /* [in] */ const String& tz,
    /* [out] */ String* location)
{
    VALIDATE_NOT_NULL(location)
    *location = String(NULL);

    if (locale.IsNull())
        return E_NULL_POINTER_EXCEPTION;

    if (tz.IsNull())
        return E_NULL_POINTER_EXCEPTION;

    NATIVE(Locale) icuLocale;
    icuLocale.setToBogus();
    icuLocale = NATIVE(Locale)::createFromName(locale);
    if (icuLocale.isBogus()) {
        return NOERROR;
    }

    UErrorCode status = U_ZERO_ERROR;
    UniquePtr<NATIVE(TimeZoneNames)> names(NATIVE(TimeZoneNames)::createInstance(icuLocale, status));
    if(!U_SUCCESS(status))
        return NOERROR;

    UnicodeString s;
    const UDate now(Calendar::getNow());
    names->getDisplayName(UnicodeString::fromUTF8(tz.string()), UTZNM_EXEMPLAR_LOCATION, now, s);
    String str("");
    ElStringByteSink sink(&str);
    s.toUTF8(sink);
    *location = str;
    return NOERROR;
}

ECode TimeZoneNames::FillZoneStrings(
    /* [in] */ const String& locale,
    /* [in] */ ArrayOf<StringArray >* result)
{
    VALIDATE_NOT_NULL(result)

    if (locale.IsNull())
        return E_NULL_POINTER_EXCEPTION;

    NATIVE(Locale) icuLocale;
    icuLocale.setToBogus();
    icuLocale = NATIVE(Locale)::createFromName(locale);
    if (icuLocale.isBogus()) {
        return NOERROR;
    }

    UErrorCode status = U_ZERO_ERROR;
    UniquePtr<NATIVE(TimeZoneNames)> names(NATIVE(TimeZoneNames)::createInstance(icuLocale, status));
    if(!U_SUCCESS(status))
        return NOERROR;

    const UDate now(Calendar::getNow());

    static const UnicodeString kUtc("UTC", 3, US_INV);

    size_t id_count = result->GetLength();
    ArrayOf<String>* java_row;
    for (size_t i = 0; i < id_count; ++i) {
        java_row = (*result)[i];
        String java_zone_id = (*java_row)[0];
        if (java_zone_id.IsNull()) {
            return NOERROR;
        }

        UnicodeString zone_id;
        AutoPtr<ArrayOf<Char16> > char16s = java_zone_id.GetChar16s();
        zone_id.setTo(false, (UChar*)char16s->GetPayload(), char16s->GetLength());

        UnicodeString long_std;
        names->getDisplayName(zone_id, UTZNM_LONG_STANDARD, now, long_std);
        UnicodeString short_std;
        names->getDisplayName(zone_id, UTZNM_SHORT_STANDARD, now, short_std);
        UnicodeString long_dst;
        names->getDisplayName(zone_id, UTZNM_LONG_DAYLIGHT, now, long_dst);
        UnicodeString short_dst;
        names->getDisplayName(zone_id, UTZNM_SHORT_DAYLIGHT, now, short_dst);

        if (isUtc(zone_id)) {
            // ICU doesn't have names for the UTC zones; it just says "GMT+00:00" for both
            // long and short names. We don't want this. The best we can do is use "UTC"
            // for everything (since we don't know how to say "Universal Coordinated Time" in
            // every language).
            // TODO: check CLDR doesn't actually have this somewhere.
            long_std = short_std = long_dst = short_dst = kUtc;
        }

        bool okay =
            SetStringArrayElement(java_row, 1, long_std) &&
            SetStringArrayElement(java_row, 2, short_std) &&
            SetStringArrayElement(java_row, 3, long_dst) &&
            SetStringArrayElement(java_row, 4, short_dst);
        if (!okay) {
            return NOERROR;
        }
    }

    return NOERROR;
}

} // namespace ICU
} // namespace Libcore
