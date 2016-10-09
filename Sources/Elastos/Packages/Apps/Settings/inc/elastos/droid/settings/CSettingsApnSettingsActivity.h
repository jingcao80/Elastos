
#ifndef __ELASTOS_DROID_SETTINGS_CSETTINGSAPNSETTINGSACTIVITY_H__
#define __ELASTOS_DROID_SETTINGS_CSETTINGSAPNSETTINGSACTIVITY_H__

#include "elastos/droid/settings/CSettings.h"
#include "_Elastos_Droid_Settings_CSettingsApnSettingsActivity.h"

using Elastos::Droid::Settings::CSettings;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CSettingsApnSettingsActivity)
    , public CSettings::ApnSettingsActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CSETTINGSAPNSETTINGSACTIVITY_H__
