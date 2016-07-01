#ifndef __ELASTOS_APPS_CONTACTS_COMMON_LOCATION_CCOUNTRYDETECTORLOCALEPROVIDER_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_LOCATION_CCOUNTRYDETECTORLOCALEPROVIDER_H__

#include "_Elastos_Apps_Contacts_Common_Location_CCountryDetectorLocaleProvider.h"
#include "CountryDetector.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Location {

CarClass(CCountryDetectorLocaleProvider)
    , public CountryDetector::LocaleProvider
{
public:
    CAR_OBJECT_DECL()
};

} // Location
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_LOCATION_CCOUNTRYDETECTORLOCALEPROVIDER_H__
