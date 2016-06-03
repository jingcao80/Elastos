
#ifndef __ELASTOS_DROID_SETTINGS_CSUBSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_CSUBSETTINGS_H__

#include "elastos/droid/settings/SettingsActivity.h"
#include "_Elastos_Droid_Settings_CSubSettings.h"

namespace Elastos {
namespace Droid {
namespace Settings {

/**
 * Stub class for showing sub-settings; we can't use the main Settings class
 * since for our app it is a special singleTask class.
 */
CarClass(CSubSettings)
    , public SettingsActivity
{
public:
    CAR_OBJECT_DECL()

    //@Override
    CARAPI OnNavigateUp(
        /* [out] */ Boolean* res);

    //@Override
    CARAPI_(Boolean) IsValidFragment(
        /* [in] */ const String& fragmentName);
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CSUBSETTINGS_H__
