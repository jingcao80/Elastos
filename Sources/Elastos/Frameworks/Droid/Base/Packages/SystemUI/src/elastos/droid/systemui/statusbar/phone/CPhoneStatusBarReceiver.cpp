
#include "elastos/droid/systemui/statusbar/phone/CPhoneStatusBarReceiver.h"
#include "elastos/droid/systemui/statusbar/phone/CPhoneStatusBar.h"

#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CAR_OBJECT_IMPL(CPhoneStatusBarReceiver)
ECode CPhoneStatusBarReceiver::constructor(
    /* [in] */ IPhoneStatusBar* controller)
{
    mHost = (CPhoneStatusBar*)controller;
    return BroadcastReceiver::constructor();
}

ECode CPhoneStatusBarReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (mHost->DEBUG) Logger::V(mHost->TAG, "onReceive: %s", TO_CSTR(intent));
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_CLOSE_SYSTEM_DIALOGS.Equals(action)) {
        Int32 flags = ICommandQueue::FLAG_EXCLUDE_NONE;
        String reason;
        intent->GetStringExtra(String("reason"), &reason);
        if (reason != NULL && reason.Equals(CPhoneStatusBar::SYSTEM_DIALOG_REASON_RECENT_APPS)) {
            flags |= ICommandQueue::FLAG_EXCLUDE_RECENTS_PANEL;
        }
        mHost->AnimateCollapsePanels(flags);
    }
    else if (IIntent::ACTION_SCREEN_OFF.Equals(action)) {
        mHost->mScreenOn = FALSE;
        mHost->NotifyNavigationBarScreenOn(FALSE);
        mHost->NotifyHeadsUpScreenOn(FALSE);
        mHost->FinishBarAnimations();
        mHost->StopNotificationLogging();
        mHost->ResetUserExpandedStates();
    }
    else if (IIntent::ACTION_SCREEN_ON.Equals(action)) {
        mHost->mScreenOn = TRUE;
        // work around problem where mDisplay.getRotation() is not stable while screen is off (bug 7086018)
        mHost->RepositionNavigationBar();
        mHost->NotifyNavigationBarScreenOn(TRUE);
        mHost->StartNotificationLoggingIfScreenOnAndVisible();
    }
    else if (CPhoneStatusBar::ACTION_DEMO.Equals(action)) {
        AutoPtr<IBundle> bundle;
        intent->GetExtras((IBundle**)&bundle);
        if (bundle != NULL) {
            String str;
            bundle->GetString(String("command"), String(""), &str);
            String command = str.Trim().ToLowerCase();
            if (command.GetLength() > 0) {
                if (FAILED(mHost->DispatchDemoCommand(command, bundle))) {
                    Logger::W(mHost->TAG, "Error running demo command, intent=%s", TO_CSTR(intent));
                }
            }
        }
    }
    else if (String("fake_artwork").Equals(action)) {
        if (CPhoneStatusBar::DEBUG_MEDIA_FAKE_ARTWORK) {
            mHost->UpdateMediaMetaData(TRUE);
        }
    }
    return NOERROR;
}

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
