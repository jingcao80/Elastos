
#ifndef __ELASTOS_DROID_SETTINGS_CSETTINGSWIFIP2PSETTINGSACTIVITY_H__
#define __ELASTOS_DROID_SETTINGS_CSETTINGSWIFIP2PSETTINGSACTIVITY_H__

#include "elastos/droid/settings/CSettings.h"
#include "_Elastos_Droid_Settings_CSettingsWifiP2pSettingsActivity.h"

using Elastos::Droid::Settings::CSettings;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CSettingsWifiP2pSettingsActivity)
    , public CSettings::WifiP2pSettingsActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CSETTINGSWIFIP2PSETTINGSACTIVITY_H__
