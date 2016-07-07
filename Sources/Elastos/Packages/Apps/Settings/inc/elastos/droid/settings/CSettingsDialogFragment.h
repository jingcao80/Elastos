
#ifndef __ELASTOS_DROID_SETTINGS_CSETTINGSDIALOGFRAGMENT_H__
#define __ELASTOS_DROID_SETTINGS_CSETTINGSDIALOGFRAGMENT_H__

#include "_Elastos_Droid_Settings_CSettingsDialogFragment.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CSettingsDialogFragment)
    , public SettingsPreferenceFragment::SettingsDialogFragment
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CSETTINGSDIALOGFRAGMENT_H__
