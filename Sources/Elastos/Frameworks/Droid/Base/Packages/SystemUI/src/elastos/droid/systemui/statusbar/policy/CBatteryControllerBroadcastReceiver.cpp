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

#include "elastos/droid/systemui/statusbar/policy/CBatteryControllerBroadcastReceiver.h"
#include "elastos/droid/systemui/statusbar/policy/BatteryController.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::Os::IBatteryManager;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CAR_OBJECT_IMPL(CBatteryControllerBroadcastReceiver)
ECode CBatteryControllerBroadcastReceiver::constructor(
    /* [in] */ IBatteryController* controller)
{
    mHost = (BatteryController*)controller;
    return BroadcastReceiver::constructor();
}

ECode CBatteryControllerBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_BATTERY_CHANGED)) {
        Int32 v1 = 0, v2 = 0;
        mHost->mLevel = (Int32)(100.f
                * (intent->GetInt32Extra(IBatteryManager::EXTRA_LEVEL, 0, &v1), v1)
                / (intent->GetInt32Extra(IBatteryManager::EXTRA_SCALE, 100, &v2), v2));
        mHost->mPluggedIn = (intent->GetInt32Extra(IBatteryManager::EXTRA_PLUGGED, 0, &v1), v1) != 0;

        Int32 status = 0;
        intent->GetInt32Extra(IBatteryManager::EXTRA_STATUS,
                IBatteryManager::BATTERY_STATUS_UNKNOWN, &status);
        mHost->mCharged = status == IBatteryManager::BATTERY_STATUS_FULL;
        mHost->mCharging = mHost->mCharged || status == IBatteryManager::BATTERY_STATUS_CHARGING;

        mHost->FireBatteryLevelChanged();
    }
    else if (action.Equals(IPowerManager::ACTION_POWER_SAVE_MODE_CHANGED)) {
        mHost->UpdatePowerSave();
    }
    else if (action.Equals(IPowerManager::ACTION_POWER_SAVE_MODE_CHANGING)) {
        Boolean tmp = FALSE;
        intent->GetBooleanExtra(IPowerManager::EXTRA_POWER_SAVE_MODE, FALSE, &tmp);
        mHost->SetPowerSave(tmp);
    }
    return NOERROR;
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
