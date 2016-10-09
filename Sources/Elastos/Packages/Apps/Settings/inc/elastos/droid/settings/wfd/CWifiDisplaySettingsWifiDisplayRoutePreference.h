
#ifndef __ELASTOS_DROID_SETTINGS_WFD_CWIFIDISPLAYSETTINGSWIFIDISPLAYROUTEPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_WFD_CWIFIDISPLAYSETTINGSWIFIDISPLAYROUTEPREFERENCE_H__

#include "_Elastos_Droid_Settings_Wfd_CWifiDisplaySettingsWifiDisplayRoutePreference.h"
#include "elastos/droid/settings/wfd/CWifiDisplaySettings.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wfd {

CarClass(CWifiDisplaySettingsWifiDisplayRoutePreference)
    , public CWifiDisplaySettings::WifiDisplayRoutePreference
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Wfd
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WFD_CWIFIDISPLAYSETTINGSWIFIDISPLAYROUTEPREFERENCE_H__
