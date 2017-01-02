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

#ifndef __ELASTOS_DROID_SETTINGS_LOCATION_CRADIOBUTTONPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_LOCATION_CRADIOBUTTONPREFERENCE_H__

#include "_Elastos_Droid_Settings_Location_CRadioButtonPreference.h"
#include "elastos/droid/preference/CheckBoxPreference.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Location {

/**
 * Check box preference with check box replaced by radio button.
 *
 * Functionally speaking, it's actually a CheckBoxPreference. We only modified
 * the widget to RadioButton to make it "look like" a RadioButtonPreference.
 *
 * In other words, there's no "RadioButtonPreferenceGroup" in this
 * implementation. When you check one RadioButtonPreference, if you want to
 * uncheck all the other preferences, you should do that by code yourself.
 */
CarClass(CRadioButtonPreference)
    , public Elastos::Droid::Preference::CheckBoxPreference
    , public IRadioButtonPreference
{
public:
    TO_STRING_IMPL("CRadioButtonPreference")

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CRadioButtonPreference();

    ~CRadioButtonPreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context);

    virtual CARAPI SetOnClickListener(
        /* [in] */ IRadioButtonPreferenceOnClickListener* listener);

    //@Override
    CARAPI OnClick();

    //@Override
    CARAPI OnBindView(
        /* [in] */ IView* view);

private:
    AutoPtr<IRadioButtonPreferenceOnClickListener> mListener;
};

} // namespace Location
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_LOCATION_CRADIOBUTTONPREFERENCE_H__