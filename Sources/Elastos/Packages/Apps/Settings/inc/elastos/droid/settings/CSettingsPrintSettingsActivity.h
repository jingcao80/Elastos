
#ifndef __ELASTOS_DROID_SETTINGS_CSETTINGSPRINTSETTINGSACTIVITY_H__
#define __ELASTOS_DROID_SETTINGS_CSETTINGSPRINTSETTINGSACTIVITY_H__

#include "elastos/droid/settings/CSettings.h"
#include "_Elastos_Droid_Settings_CSettingsPrintSettingsActivity.h"

using Elastos::Droid::Settings::CSettings;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CSettingsPrintSettingsActivity)
    , public CSettings::PrintSettingsActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CSETTINGSPRINTSETTINGSACTIVITY_H__
