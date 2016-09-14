#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_CZENMODESETTINGSDOWNTIMEDAYSSELECTION_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_CZENMODESETTINGSDOWNTIMEDAYSSELECTION_H__

#include "_Elastos_Droid_Settings_Notification_CZenModeSettingsDowntimeDaysSelection.h"
#include "elastos/droid/settings/notification/CZenModeSettings.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

CarClass(CZenModeSettingsDowntimeDaysSelection)
    , public CZenModeSettings::OnCreateZenModeDowntimeDaysSelection
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_CZENMODESETTINGSDOWNTIMEDAYSSELECTION_H__