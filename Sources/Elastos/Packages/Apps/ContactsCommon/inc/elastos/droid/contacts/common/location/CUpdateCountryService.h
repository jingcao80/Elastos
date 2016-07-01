#ifndef __ELASTOS_APPS_CONTACTS_COMMON_LOCATION_CUPDATECOUNTRYSERVICE_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_LOCATION_CUPDATECOUNTRYSERVICE_H__

#include "_Elastos_Apps_Contacts_Common_Location_CUpdateCountryService.h"
#include "UpdateCountryService.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Location {

CarClass(CUpdateCountryService)
    , public UpdateCountryService
{
public:
    CAR_OBJECT_DECL()
};

} // Location
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_LOCATION_CUPDATECOUNTRYSERVICE_H__