
#ifndef __ELASTOS_APPS_CONTACTS_COMMON_LOCATION_CCOUNTRYDETECTORHELPER_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_LOCATION_CCOUNTRYDETECTORHELPER_H__

#include "_Elastos_Apps_Contacts_Common_Location_CCountryDetectorHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Location {

CarClass(CCountryDetectorHelper)
    , public singleton
    , public ICountryDetectorHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI RegisterForLocationUpdates(
        /* [in] */ IContext* context,
        /* [in] */ ILocationManager* locationManager);

     /**
     * Returns the instance of the country detector. {@link #initialize(Context)} must have been
     * called previously.
     *
     * @return the initialized country detector.
     */
    CARAPI GetInstance(
        /* [in] */ IContext* context,
        /* [out] */ ICountryDetector** detector);
};

} // Location
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_LOCATION_CCOUNTRYDETECTORHELPER_H__
