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

#ifndef __ELASTOS_DROID_SETTINGS_FUELGAUGE_CPOWERGAUGEPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_FUELGAUGE_CPOWERGAUGEPREFERENCE_H__

#include "_Elastos_Droid_Settings_Fuelgauge_CPowerGaugePreference.h"
#include "elastos/droid/preference/Preference.h"
#include "elastos/droid/settings/fuelgauge/BatteryEntry.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Fuelgauge {

/**
 * Custom preference for displaying power consumption as a bar and an icon on
 * the left for the subsystem/app type.
 */
CarClass(CPowerGaugePreference)
    , public Elastos::Droid::Preference::Preference
    , public IPowerGaugePreference
{
public:
    TO_STRING_IMPL("CPowerGaugePreference")

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CPowerGaugePreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IDrawable* icon,
        /* [in] */ ICharSequence* contentDescription,
        /* [in] */ IBatteryEntry* info);

    CARAPI SetPercent(
        /* [in] */ Double percentOfMax,
        /* [in] */ Double percentOfTotal);

    CARAPI_(AutoPtr<BatteryEntry>) GetInfo();

protected:
    //@Override
    CARAPI OnBindView(
        /* [in] */ IView* view);

private:
    AutoPtr<BatteryEntry> mInfo;
    Int32 mProgress;
    AutoPtr<ICharSequence> mProgressText;
    AutoPtr<ICharSequence> mContentDescription;
};

} // namespace Fuelgauge
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_FUELGAUGE_CPOWERGAUGEPREFERENCE_H__