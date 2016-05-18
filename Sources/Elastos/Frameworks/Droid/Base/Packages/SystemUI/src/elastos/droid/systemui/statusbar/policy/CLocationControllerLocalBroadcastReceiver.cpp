
#include "elastos/droid/systemui/statusbar/policy/CLocationControllerLocalBroadcastReceiver.h"
#include "elastos/droid/systemui/statusbar/policy/LocationControllerImpl.h"
#include "Elastos.Droid.Location.h"

using Elastos::Droid::Location::ILocationManager;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CAR_OBJECT_IMPL(CLocationControllerLocalBroadcastReceiver)
ECode CLocationControllerLocalBroadcastReceiver::constructor(
    /* [in] */ ILocationController* controller)
{
    mHost = (LocationControllerImpl*)controller;
    return BroadcastReceiver::constructor();
}

ECode CLocationControllerLocalBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (ILocationManager::MODE_CHANGED_ACTION.Equals(action)) {
        mHost->LocationSettingsChanged();
    }
    return NOERROR;
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
