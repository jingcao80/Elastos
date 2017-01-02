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

#ifndef __ELASTOS_DROID_SYSTEMUI_CBATTERYMETERVIEWBATTERYTRACKER_H__
#define __ELASTOS_DROID_SYSTEMUI_CBATTERYMETERVIEWBATTERYTRACKER_H__

#include "_Elastos_Droid_SystemUI_CBatteryMeterViewBatteryTracker.h"
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Droid {
namespace SystemUI {

class CBatteryMeterView;

CarClass(CBatteryMeterViewBatteryTracker), public BroadcastReceiver
{
public:
    CAR_OBJECT_DECL()

    CBatteryMeterViewBatteryTracker();

    CARAPI constructor(
        /* [in] */ IBatteryMeterView* view);

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    static Int32 UNKNOWN_LEVEL;

    // current battery status
    Int32 mLevel;
    String mPercentStr;
    Int32 mPlugType;
    Boolean mPlugged;
    Int32 mHealth;
    Int32 mStatus;
    String mTechnology;
    Int32 mVoltage;
    Int32 mTemperature;
    Boolean mTestmode;

    CBatteryMeterView* mHost;
    friend class CBatteryMeterView;
};

}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_CBATTERYMETERVIEWBATTERYTRACKER_H__
