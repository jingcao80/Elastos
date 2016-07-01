#ifndef __ELASTOS_APPS_CONTACTS_COMMON_LOCATION_UPDATECOUNTTYSERVICE_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_LOCATION_UPDATECOUNTTYSERVICE_H__

#include "elastos/droid/app/IntentService.h"

using Elastos::Droid::App::IntentService;

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Location {

/**
 * Service used to perform asynchronous geocoding from within a broadcast receiver. Given a
 * {@link Location}, convert it into a country code, and save it in shared preferences.
 */
class UpdateCountryService
    : public IntentService
    , public IUpdateCountryService
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    static CARAPI_(void) UpdateCountry(
        /* [in] */ IContext* context,
        /* [in] */ ILocation* location);

protected:
    // @Override
    CARAPI OnHandleIntent(
        /* [in] */ IIntent* intent);

private:
    /**
     * Given a {@link Location}, return a country code.
     *
     * @return the ISO 3166-1 two letter country code
     */
    CARAPI_(String) GetCountryFromLocation(
        /* [in] */ IContext* context,
        /* [in] */ ILocation* location);

private:
    static const String TAG; // = UpdateCountryService.class.getSimpleName();

    static const String ACTION_UPDATE_COUNTRY; // = "saveCountry";

    static const String KEY_INTENT_LOCATION; // = "location";
};

} // Location
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_LOCATION_UPDATECOUNTTYSERVICE_H__
