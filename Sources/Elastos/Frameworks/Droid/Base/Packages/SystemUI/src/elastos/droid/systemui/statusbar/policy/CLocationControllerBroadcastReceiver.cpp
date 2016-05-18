
#include "elastos/droid/systemui/statusbar/policy/CLocationControllerBroadcastReceiver.h"
#include "elastos/droid/systemui/statusbar/policy/LocationControllerImpl.h"
#include "Elastos.Droid.Location.h"

using Elastos::Droid::Location::ILocationManager;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CAR_OBJECT_IMPL(CLocationControllerBroadcastReceiver)
ECode CLocationControllerBroadcastReceiver::constructor(
    /* [in] */ ILocationController* controller)
{
    mHost = (LocationControllerImpl*)controller;
    return BroadcastReceiver::constructor();
}

ECode CLocationControllerBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (ILocationManager::HIGH_POWER_REQUEST_CHANGE_ACTION.Equals(action)) {
        mHost->UpdateActiveLocationRequests();
    }
    return NOERROR;
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
