#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_CZENMODEAUTOMATICCONDITIONSELECTIONCONDITIONLISTENER_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_CZENMODEAUTOMATICCONDITIONSELECTIONCONDITIONLISTENER_H__

#include "_Elastos_Droid_Settings_Notification_CZenModeAutomaticConditionSelectionConditionListener.h"
#include "elastos/droid/settings/notification/CZenModeAutomaticConditionSelection.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

CarClass(CZenModeAutomaticConditionSelectionConditionListener)
    , public CZenModeAutomaticConditionSelection::ConditionListener
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_CZENMODEAUTOMATICCONDITIONSELECTIONCONDITIONLISTENER_H__