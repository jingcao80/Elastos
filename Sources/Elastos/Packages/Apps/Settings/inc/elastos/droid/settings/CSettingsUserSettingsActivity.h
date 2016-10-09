
#ifndef __ELASTOS_DROID_SETTINGS_CSETTINGSUSERSETTINGSACTIVITY_H__
#define __ELASTOS_DROID_SETTINGS_CSETTINGSUSERSETTINGSACTIVITY_H__

#include "elastos/droid/settings/CSettings.h"
#include "_Elastos_Droid_Settings_CSettingsUserSettingsActivity.h"

using Elastos::Droid::Settings::CSettings;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CSettingsUserSettingsActivity)
    , public CSettings::UserSettingsActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CSETTINGSUSERSETTINGSACTIVITY_H__
