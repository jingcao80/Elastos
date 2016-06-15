
#ifndef __ELASTOS_DROID_SETTINGS_WIFI_CADVANCEDWIFISETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_CADVANCEDWIFISETTINGS_H__

#include "_Elastos_Droid_Settings_Wifi_CAdvancedWifiSettings.h"
#include "elastos/droid/settings/wifi/AdvancedWifiSettings.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

CarClass(CAdvancedWifiSettings)
    , public AdvancedWifiSettings
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIFI_CADVANCEDWIFISETTINGS_H__
