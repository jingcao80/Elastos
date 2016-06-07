#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CHOTSPOTCONTROLLERRECEIVER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CHOTSPOTCONTROLLERRECEIVER_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Policy_CHotspotControllerReceiver.h"
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class HotspotControllerImpl;

CarClass(CHotspotControllerReceiver)
    , public BroadcastReceiver
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IHotspotController* controller);

    CARAPI SetListening(
        /* [in] */ Boolean listening);

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    Boolean mRegistered;
    HotspotControllerImpl* mHost;
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CHOTSPOTCONTROLLERRECEIVER_H__
