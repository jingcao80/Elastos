
#ifndef __ELASTOS_DROID_SETTINGS_CSETTINGSSTORAGESETTINGSACTIVITY_H__
#define __ELASTOS_DROID_SETTINGS_CSETTINGSSTORAGESETTINGSACTIVITY_H__

#include "elastos/droid/settings/CSettings.h"
#include "_Elastos_Droid_Settings_CSettingsStorageSettingsActivity.h"

using Elastos::Droid::Settings::CSettings;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CSettingsStorageSettingsActivity)
    , public CSettings::StorageSettingsActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CSETTINGSSTORAGESETTINGSACTIVITY_H__
