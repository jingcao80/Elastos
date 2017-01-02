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

#ifndef __ELASTOS_DROID_SETTINGS_FUELGAUGE_CBATTERYHISTORYPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_FUELGAUGE_CBATTERYHISTORYPREFERENCE_H__

#include "Elastos.Droid.View.h"
#include "_Elastos_Droid_Settings_Fuelgauge_CBatteryHistoryPreference.h"
#include "elastos/droid/settings/fuelgauge/CBatteryHistoryChart.h"
#include "elastos/droid/preference/Preference.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Fuelgauge {

/**
 * Custom preference for displaying power consumption as a bar and an icon on the left for the
 * subsystem/app type.
 *
 */
CarClass(CBatteryHistoryPreference)
    , public Elastos::Droid::Preference::Preference
    , public IBatteryHistoryPreference
{
public:
    TO_STRING_IMPL("CBatteryHistoryPreference")

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBatteryHistoryPreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IBatteryStats* stats,
        /* [in] */ IIntent* batteryBroadcast);

    CARAPI_(AutoPtr<IBatteryStats>) GetStats();

    CARAPI SetHideLabels(
        /* [in] */ Boolean hide);

protected:
    //@Override
    CARAPI OnBindView(
        /* [in] */ IView* view);

private:
    AutoPtr<IBatteryStats> mStats;
    AutoPtr<IIntent> mBatteryBroadcast;

    Boolean mHideLabels;
    AutoPtr<IView> mLabelHeader;
    AutoPtr<CBatteryHistoryChart> mChart;
};

} // namespace Fuelgauge
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_FUELGAUGE_CBATTERYHISTORYPREFERENCE_H__