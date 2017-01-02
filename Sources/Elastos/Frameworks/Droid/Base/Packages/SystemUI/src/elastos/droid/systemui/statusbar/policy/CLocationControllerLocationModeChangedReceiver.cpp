//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
