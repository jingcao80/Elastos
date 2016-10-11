
#include "elastos/droid/systemui/statusbar/policy/CLocationControllerLocationModeChangedReceiver.h"
#include "elastos/droid/systemui/statusbar/policy/LocationControllerImpl.h"
#include "Elastos.Droid.Location.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Location::ILocationManager;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CAR_OBJECT_IMPL(CLocationControllerLocationModeChangedReceiver)

ECode CLocationControllerLocationModeChangedReceiver::constructor(
    /* [in] */ ILocationController* controller)
{
    mHost = (LocationControllerImpl*)controller;
    return BroadcastReceiver::constructor();
}

ECode CLocationControllerLocationModeChangedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Logger::I("LocationController", " >> CLocationControllerLocationModeChangedReceiver:OnReceive");
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
