
#ifndef __ELASTOS_DROID_SETTINGS_WFD_CWIFIDISPLAYSETTINGSUNPAIREDWIFIDISPLAYPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_WFD_CWIFIDISPLAYSETTINGSUNPAIREDWIFIDISPLAYPREFERENCE_H__

#include "_Elastos_Droid_Settings_Wfd_CWifiDisplaySettingsUnpairedWifiDisplayPreference.h"
#include "elastos/droid/settings/wfd/CWifiDisplaySettings.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wfd {

CarClass(CWifiDisplaySettingsUnpairedWifiDisplayPreference)
    , public CWifiDisplaySettings::UnpairedWifiDisplayPreference
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Wfd
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WFD_CWIFIDISPLAYSETTINGSUNPAIREDWIFIDISPLAYPREFERENCE_H__
