#ifndef __ELASTOS_APPS_CONTACTS_COMMON_GEOUTIL_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_GEOUTIL_H__

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {

/**
 * Static methods related to Geo.
 */
class ECO_PUBLIC GeoUtil
    : public Object
{
public:
    static CARAPI_(String) GetCurrentCountryIso(
        /* [in] */ IContext* context);

    static CARAPI_(String) GetGeocodedLocationFor(
        /* [in] */ IContext* context,
        /* [in] */ const String& phoneNumber);
};

} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_GEOUTIL_H__