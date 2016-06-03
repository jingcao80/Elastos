
#ifndef __ELASTOS_DROID_SETTINGS_CSETTINGSACTIVITY_H__
#define __ELASTOS_DROID_SETTINGS_CSETTINGSACTIVITY_H__

#include "_Elastos_Droid_Settings_CSettingsActivity.h"
#include "elastos/droid/settings/SettingsActivity.h"

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CSettingsActivity)
    , public SettingsActivity
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CSETTINGSACTIVITY_H__
