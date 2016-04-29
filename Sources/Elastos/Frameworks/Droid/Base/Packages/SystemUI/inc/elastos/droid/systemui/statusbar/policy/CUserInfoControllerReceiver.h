#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CUSERINFOCONTROLLERBROADCASTRECEIVER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CUSERINFOCONTROLLERBROADCASTRECEIVER_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Policy_CUserInfoControllerReceiver.h"
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class UserInfoController;

CarClass(CUserInfoControllerReceiver), public BroadcastReceiver
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor(
        /* [in] */ IUserInfoController* controller);

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    UserInfoController* mHost;
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CUSERINFOCONTROLLERBROADCASTRECEIVER_H__
