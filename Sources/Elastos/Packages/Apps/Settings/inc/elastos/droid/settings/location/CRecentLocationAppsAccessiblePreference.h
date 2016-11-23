
#ifndef __ELASTOS_DROID_SETTINGS_LOCATION_CRECENTLOCATIONAPPSACCESSIBLEPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_LOCATION_CRECENTLOCATIONAPPSACCESSIBLEPREFERENCE_H__

#include "_Elastos_Droid_Settings_Location_CRecentLocationAppsAccessiblePreference.h"
#include "elastos/droid/settings/location/RecentLocationApps.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Location {

CarClass(CRecentLocationAppsAccessiblePreference)
    , public RecentLocationApps::AccessiblePreference
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Location
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_LOCATION_CRECENTLOCATIONAPPSACCESSIBLEPREFERENCE_H__
