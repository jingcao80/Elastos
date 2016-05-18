
#include "elastos/droid/systemui/statusbar/policy/CUserInfoControllerReceiver.h"
#include "elastos/droid/systemui/statusbar/policy/UserInfoController.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CAR_OBJECT_IMPL(CUserInfoControllerReceiver)
ECode CUserInfoControllerReceiver::constructor(
    /* [in] */ IUserInfoController* controller)
{
    mHost = (UserInfoController*)controller;
    return BroadcastReceiver::constructor();
}

ECode CUserInfoControllerReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_USER_SWITCHED.Equals(action)) {
        mHost->ReloadUserInfo();
    }
    else if (IIntent::ACTION_CONFIGURATION_CHANGED.Equals(action)) {
        if (mHost->mUseDefaultAvatar) {
            mHost->ReloadUserInfo();
        }
    }
    return NOERROR;
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
