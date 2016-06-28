#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CUSERSWITCHER_CONTROLLER_SETTINGS_OBSERVER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CUSERSWITCHER_CONTROLLER_SETTINGS_OBSERVER_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Policy_CUserSwitcherControllerSettingsObserver.h"
#include <elastos/droid/systemui/statusbar/policy/UserSwitcherController.h>

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CarClass(CUserSwitcherControllerSettingsObserver)
    , public UserSwitcherController::SettingsObserver
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CUSERSWITCHER_CONTROLLER_SETTINGS_OBSERVER_H__
