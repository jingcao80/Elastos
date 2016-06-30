#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CZENMODECONTROLLER_CONDITIONLISTENER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CZENMODECONTROLLER_CONDITIONLISTENER_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Policy_CZenModeControllerConditionListener.h"
#include "elastos/droid/systemui/statusbar/policy/ZenModeControllerImpl.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {


CarClass(CZenModeControllerConditionListener)
    , public ZenModeControllerImpl::ConditionListener
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CZENMODECONTROLLER_CONDITIONLISTENER_H__
