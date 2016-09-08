#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_CMANAGEDSERVICESETTINGSSCARYWARNINGDIALOGFRAGMENT_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_CMANAGEDSERVICESETTINGSSCARYWARNINGDIALOGFRAGMENT_H__

#include "_Elastos_Droid_Settings_Notification_CManagedServiceSettingsScaryWarningDialogFragment.h"
#include "elastos/droid/settings/notification/ManagedServiceSettings.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

CarClass(CManagedServiceSettingsScaryWarningDialogFragment)
    , public ManagedServiceSettings::ScaryWarningDialogFragment
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_CMANAGEDSERVICESETTINGSSCARYWARNINGDIALOGFRAGMENT_H__