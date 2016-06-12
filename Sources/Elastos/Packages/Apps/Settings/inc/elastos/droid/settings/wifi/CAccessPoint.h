
#ifndef __ELASTOS_DROID_SETTINGS_WIFI_CACCESSPOINT_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_CACCESSPOINT_H__

#include "_Elastos_Droid_Settings_Wifi_CAccessPoint.h"
#include "elastos/droid/settings/wifi/AccessPoint.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

CarClass(CAccessPoint)
    , public AccessPoint
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIFI_CACCESSPOINT_H__
