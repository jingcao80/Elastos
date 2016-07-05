
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_GEOUTIL_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_GEOUTIL_H__

#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {

/**
 * Static methods related to Geo.
 */
class GeoUtil
{
public:
    /**
     * Returns the country code of the country the user is currently in. Before calling this
     * method, make sure that {@link CountryDetector#initialize(Context)} has already been called
     * in {@link Application#onCreate()}.
     * @return The ISO 3166-1 two letters country code of the country the user
     *         is in.
     */
    static CARAPI_(String) GetCurrentCountryIso(
        /* [in] */ IContext* context);

    static CARAPI_(String) GetGeocodedLocationFor(
        /* [in] */ IContext* context,
        /* [in] */ const String& phoneNumber);
};

} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_GEOUTIL_H__
