
#ifndef __ELASTOS_DROID_SETTINGS_CSETTINGSNOTIFICATIONACCESSSETTINGSACTIVITY_H__
#define __ELASTOS_DROID_SETTINGS_CSETTINGSNOTIFICATIONACCESSSETTINGSACTIVITY_H__

#include "elastos/droid/settings/CSettings.h"
#include "_Elastos_Droid_Settings_CSettingsNotificationAccessSettingsActivity.h"

using Elastos::Droid::Settings::CSettings;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CSettingsNotificationAccessSettingsActivity)
    , public CSettings::NotificationAccessSettingsActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CSETTINGSNOTIFICATIONACCESSSETTINGSACTIVITY_H__
