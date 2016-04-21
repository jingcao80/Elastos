
#ifndef __ELASTOS_APPS_CONTACTS_COMMON_LOCATION_CUPDATECOUNTTYSERVICEHELPER_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_LOCATION_CUPDATECOUNTTYSERVICEHELPER_H__

#include "_Elastos_Apps_Contacts_Common_Location_CUpdateCountryServiceHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Location {

CarClass(CUpdateCountryServiceHelper)
    , public singleton
    , public IUpdateCountryServiceHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI UpdateCountry(
        /* [in] */ IContext* context,
        /* [in] */ ILocation* location);
};

} // Location
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_LOCATION_CUPDATECOUNTTYSERVICEHELPER_H__
