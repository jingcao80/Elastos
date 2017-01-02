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

#include "elastos/droid/systemui/CBatteryMeterViewBatteryTracker.h"
#include "elastos/droid/systemui/CBatteryMeterView.h"
#include "R.h"
#include "Elastos.Droid.Os.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::IBatteryManager;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::IInteger32;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {

Int32 CBatteryMeterViewBatteryTracker::UNKNOWN_LEVEL = -1;
CAR_OBJECT_IMPL(CBatteryMeterViewBatteryTracker)
CBatteryMeterViewBatteryTracker::CBatteryMeterViewBatteryTracker()
    : mLevel(UNKNOWN_LEVEL)
    , mPlugType(0)
    , mPlugged(FALSE)
    , mHealth(0)
    , mStatus(0)
    , mVoltage(0)
    , mTemperature(0)
    , mTestmode(FALSE)
{
}

ECode CBatteryMeterViewBatteryTracker::constructor(
    /* [in] */ IBatteryMeterView* view)
{
    mHost = (CBatteryMeterView*)view;
    return BroadcastReceiver::constructor();
}

ECode CBatteryMeterViewBatteryTracker::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_BATTERY_CHANGED)) {
        Boolean bExt = FALSE;
        if (mTestmode && ! (intent->GetBooleanExtra(String("testmode"), FALSE, &bExt), bExt))
            return NOERROR;

        Int32 ext1 = 0, ext2 = 0;
        intent->GetInt32Extra(IBatteryManager::EXTRA_LEVEL, 0, &ext1);
        intent->GetInt32Extra(IBatteryManager::EXTRA_SCALE, 100, &ext2);
        mLevel = (Int32)(100.0 * ext1 / ext2);

        intent->GetInt32Extra(IBatteryManager::EXTRA_PLUGGED, 0, &mPlugType);
        mPlugged = mPlugType != 0;
        intent->GetInt32Extra(IBatteryManager::EXTRA_HEALTH,
                IBatteryManager::BATTERY_HEALTH_UNKNOWN, &mHealth);
        intent->GetInt32Extra(IBatteryManager::EXTRA_STATUS,
                IBatteryManager::BATTERY_STATUS_UNKNOWN, &mStatus);
        intent->GetStringExtra(IBatteryManager::EXTRA_TECHNOLOGY, &mTechnology);
        intent->GetInt32Extra(IBatteryManager::EXTRA_VOLTAGE, 0, &mVoltage);
        intent->GetInt32Extra(IBatteryManager::EXTRA_TEMPERATURE, 0, &mTemperature);

        AutoPtr<IInteger32> obj;
        CInteger32::New(mLevel, (IInteger32**)&obj);
        AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, obj);

        String str;
        context->GetString(R::string::accessibility_battery_level, args, &str);
        AutoPtr<ICharSequence> des;
        CString::New(str, (ICharSequence**)&des);
        mHost->SetContentDescription(des);
        mHost->PostInvalidate();
    }
    else if (action.Equals(mHost->ACTION_LEVEL_TEST)) {
        mTestmode = TRUE;
        AutoPtr<IRunnable> r = new CBatteryMeterView::Runnable_1(mLevel, mPlugType, mTestmode, mHost);
        Boolean result = FALSE;
        mHost->Post(r, &result);
    }
    return NOERROR;
}

}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
