#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CNEXTALARMCONTROLLERBROADCASTRECEIVER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CNEXTALARMCONTROLLERBROADCASTRECEIVER_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Policy_CNextAlarmControllerBroadcastReceiver.h"
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class NextAlarmController;

CarClass(CNextAlarmControllerBroadcastReceiver), public BroadcastReceiver
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ INextAlarmController* controller);

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    NextAlarmController* mHost;
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CNEXTALARMCONTROLLERBROADCASTRECEIVER_H__
