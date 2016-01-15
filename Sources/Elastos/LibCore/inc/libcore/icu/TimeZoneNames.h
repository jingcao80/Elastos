
#ifndef __LIBCORE_ICU_ELASTOS_UTILITY_TIMEZONENAMES_H__
#define __LIBCORE_ICU_ELASTOS_UTILITY_TIMEZONENAMES_H__

#include "Object.h"
#include <elastos/utility/etl/HashMap.h>
#include "BasicLruCache.h"

using Elastos::Core::IArrayOf;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::ILocale;
using Libcore::Utility::BasicLruCache;

namespace Libcore {
namespace ICU {

class TimeZoneNames
{
public:
    class ZoneStringsCache
        : public BasicLruCache
    {
    public:
        ZoneStringsCache();

        ~ZoneStringsCache();

    protected:
        virtual CARAPI_(AutoPtr<IInterface>) Create(
            /* [in] */ IInterface* key);

    private:
        // De-duplicate the strings (http://b/2672057).
        CARAPI_(void) InternStrings(
            /* [in] */ ArrayOf<StringArray >* result);

        // static final Comparator<String[]> ZONE_STRINGS_COMPARATOR = new Comparator<String[]>() {
        // public int compare(String[] lhs, String[] rhs) {
        //     return lhs[OLSON_NAME].compareTo(rhs[OLSON_NAME]);
        // }
        friend class TimeZoneNames;
    };

public:
    static CARAPI GetDisplayName(
        /* [in] */ ArrayOf<IArrayOf*>*  zoneStrings,
        /* [in] */ const String& id,
        /* [in] */ Boolean daylight,
        /* [in] */ Int32 style ,
        /* [out] */ String * str);

    static CARAPI GetZoneStrings(
        /* [in] */ ILocale * locale,
        /* [out, callee] */ ArrayOf<IArrayOf*>** outarray);

    static CARAPI ForLocale(
        /* [in] */ ILocale * locale ,
        /* [out, callee] */ ArrayOf<String>** outarray);

    static CARAPI GetExemplarLocation(
        /* [in] */ const String& locale,
        /* [in] */ const String& tz,
        /* [out] */ String* location);

private:
    TimeZoneNames();
    TimeZoneNames(const TimeZoneNames&);

private:
    static CARAPI FillZoneStrings(
        /* [in] */ const String& locale,
        /* [in] */ ArrayOf<StringArray >* result);

    static CARAPI_(AutoPtr<IInterface>) ArrayOfToInterface(
            /* [in] */ ArrayOf< AutoPtr< ArrayOf<String> > >* array);

    static CARAPI_(AutoPtr<ArrayOf< AutoPtr< ArrayOf<String> > > >) InterfaceToArray(
            /* [in] */ IArrayOf* iArray);
private:
    static AutoPtr<ArrayOf<String> > sAvailableTimeZoneIds;
    static AutoPtr<ZoneStringsCache> sCachedZoneStrings;
};

} // namespace Utility
} // namespace Elastos

#endif //__LIBCORE_ICU_ELASTOS_UTILITY_TIMEZONENAMES_H__
