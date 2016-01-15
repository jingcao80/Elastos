#include "CTimeZoneNames.h"
#include "TimeZoneNames.h"

namespace Libcore {
namespace ICU {

CAR_SINGLETON_IMPL(CTimeZoneNames)

CAR_INTERFACE_IMPL(CTimeZoneNames, Singleton, ITimeZoneNames)

/**
 * Returns the appropriate string from 'zoneStrings'. Used with getZoneStrings.
 */
ECode CTimeZoneNames::GetDisplayName(
    /* [in] */ ArrayOf<IArrayOf*>*  zoneStrings,
    /* [in] */ const String& id,
    /* [in] */ Boolean daylight,
    /* [in] */ Int32 style ,
    /* [out] */ String * str)
{
    return TimeZoneNames::GetDisplayName(zoneStrings, id, daylight, style, str);
}

ECode CTimeZoneNames::GetZoneStrings(
    /* [in] */ ILocale * locale,
    /* [out, callee] */ ArrayOf<IArrayOf*> ** outarray)
{
    return TimeZoneNames::GetZoneStrings(locale, outarray);
}

ECode CTimeZoneNames::ForLocale(
    /* [in] */ ILocale * locale ,
    /* [out, callee] */ ArrayOf<String> ** outarray)
{
    return TimeZoneNames::ForLocale(locale, outarray);
}

ECode CTimeZoneNames::GetExemplarLocation(
    /* [in] */ const String& locale,
    /* [in] */ const String& tz,
    /* [out] */ String* location)
{
    return TimeZoneNames::GetExemplarLocation(locale, tz, location);
}


} // namespace ICU
} // namespace Libcore