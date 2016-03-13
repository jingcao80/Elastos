
#ifndef __ELASTOS_DROID_SETTINGS_SUBSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_SUBSETTINGS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/settings/SettingsActivity.h"

namespace Elastos {
namespace Droid {
namespace Settings {

/**
 * Stub class for showing sub-settings; we can't use the main Settings class
 * since for our app it is a special singleTask class.
 */
class SubSettings
    : public SettingsActivity
{
public:
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

#endif //__ELASTOS_DROID_SETTINGS_SUBSETTINGS_H__
