#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CBATTERYCONTROLLERBROADCASTRECEIVER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CBATTERYCONTROLLERBROADCASTRECEIVER_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Policy_CBatteryControllerBroadcastReceiver.h"
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class BatteryController;

CarClass(CBatteryControllerBroadcastReceiver), public BroadcastReceiver
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IBatteryController* controller);

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    BatteryController* mHost;
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CBATTERYCONTROLLERBROADCASTRECEIVER_H__
