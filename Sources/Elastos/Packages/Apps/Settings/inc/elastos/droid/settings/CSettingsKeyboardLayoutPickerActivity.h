
#ifndef __ELASTOS_DROID_SETTINGS_CSETTINGSKEYBOARDLAYOUTPICKERACTIVITY_H__
#define __ELASTOS_DROID_SETTINGS_CSETTINGSKEYBOARDLAYOUTPICKERACTIVITY_H__

#include "elastos/droid/settings/CSettings.h"
#include "_Elastos_Droid_Settings_CSettingsKeyboardLayoutPickerActivity.h"

using Elastos::Droid::Settings::CSettings;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CSettingsKeyboardLayoutPickerActivity)
    , public CSettings::KeyboardLayoutPickerActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CSETTINGSKEYBOARDLAYOUTPICKERACTIVITY_H__
