
#include "elastos/droid/systemui/statusbar/policy/CNextAlarmControllerBroadcastReceiver.h"
#include "elastos/droid/systemui/statusbar/policy/NextAlarmController.h"
#include "Elastos.Droid.App.h"

using Elastos::Droid::App::IAlarmManager;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CAR_OBJECT_IMPL(CNextAlarmControllerBroadcastReceiver);
ECode CNextAlarmControllerBroadcastReceiver::constructor(
    /* [in] */ INextAlarmController* controller)
{
    mHost = (NextAlarmController*)controller;
    return BroadcastReceiver::constructor();
}

ECode CNextAlarmControllerBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_USER_SWITCHED)
            || action.Equals(IAlarmManager::ACTION_NEXT_ALARM_CLOCK_CHANGED)) {
        mHost->UpdateNextAlarm();
    }
    return NOERROR;
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
