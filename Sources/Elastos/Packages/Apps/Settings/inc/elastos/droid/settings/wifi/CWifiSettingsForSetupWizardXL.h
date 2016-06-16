
#ifndef __ELASTOS_DROID_SETTINGS_WIFI_CWIFISETTINGSFORSETUPWIZARDXL_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_CWIFISETTINGSFORSETUPWIZARDXL_H__

#include "_Elastos_Droid_Settings_Wifi_CWifiSettingsForSetupWizardXL.h"
#include "elastos/droid/settings/wifi/WifiSettingsForSetupWizardXL.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

CarClass(CWifiSettingsForSetupWizardXL)
    , public WifiSettingsForSetupWizardXL
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIFI_CWIFISETTINGSFORSETUPWIZARDXL_H__
