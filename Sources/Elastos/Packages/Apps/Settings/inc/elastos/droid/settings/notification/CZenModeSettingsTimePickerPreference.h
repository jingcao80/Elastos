#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_CZENMODESETTINGSTIMEPICKERPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_CZENMODESETTINGSTIMEPICKERPREFERENCE_H__

#include "_Elastos_Droid_Settings_Notification_CZenModeSettingsTimePickerPreference.h"
#include "elastos/droid/settings/notification/CZenModeSettings.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

CarClass(CZenModeSettingsTimePickerPreference)
    , public CZenModeSettings::TimePickerPreference
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_CZENMODESETTINGSTIMEPICKERPREFERENCE_H__