
#include "elastos/droid/systemui/statusbar/policy/CProfileReceiverBroadcastReceiver.h"
#include "elastos/droid/systemui/statusbar/policy/UserInfoController.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/droid/app/ActivityManagerNative.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Provider::IContactsContractIntents;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CAR_OBJECT_IMPL(CProfileReceiverBroadcastReceiver)
ECode CProfileReceiverBroadcastReceiver::constructor(
    /* [in] */ IUserInfoController* controller)
{
    mHost = (UserInfoController*)controller;
    return BroadcastReceiver::constructor();
}

ECode CProfileReceiverBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IContactsContractIntents::ACTION_PROFILE_CHANGED.Equals(action) ||
            IIntent::ACTION_USER_INFO_CHANGED.Equals(action)) {
        ECode ec = NOERROR;
        do {
            Int32 currentUser = 0;
            AutoPtr<IUserInfo> info;
            ec = ActivityManagerNative::GetDefault()->GetCurrentUser((IUserInfo**)&info);
            if (FAILED(ec)) break;

            info->GetId(&currentUser);
            Int32 changedUser = 0, id = 0;
            GetSendingUserId(&id);
            ec = intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, id, &changedUser);
            if (FAILED(ec)) break;
            if (changedUser == currentUser) {
                mHost->ReloadUserInfo();
            }
        } while (/*RemoteException e*/0);

        if (FAILED(ec)) {
            Logger::E(mHost->TAG, "Couldn't get current user id for profile change");
        }
    }
    return NOERROR;
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
