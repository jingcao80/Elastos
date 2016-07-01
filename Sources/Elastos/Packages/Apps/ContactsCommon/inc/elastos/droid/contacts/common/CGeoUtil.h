#ifndef __ELASTOS_APPS_CONTACTS_COMMON_CGEOUTIL_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_CGEOUTIL_H__

#include "_Elastos_Apps_Contacts_Common_CGeoUtil.h"
#include <elastos/core/Singleton.h>

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {

/**
 * Static methods related to Geo.
 */
CarClass(CGeoUtil)
    , public singleton
    , public IGeoUtil
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns the country code of the country the user is currently in. Before calling this
     * method, make sure that {@link CountryDetector#initialize(Context)} has already been called
     * in {@link Application#onCreate()}.
     * @return The ISO 3166-1 two letters country code of the country the user
     *         is in.
     */
    CARAPI GetCurrentCountryIso(
        /* [in] */ IContext* context,
        /* [out] */ String* result);

    CARAPI GetGeocodedLocationFor(
        /* [in] */ IContext* context,
        /* [in] */ const String& phoneNumber,
        /* [out] */ String* result);
};

} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_CGEOUTIL_H__
