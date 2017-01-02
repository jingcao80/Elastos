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

#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_CSETTINGACTIVITY_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_CSETTINGACTIVITY_H__

#include <elastos/droid/ext/frameworkdef.h>
#include "_Elastos_Droid_InputMethod_Pinyin_CSettingsActivity.h"
#include "elastos/droid/preference/PreferenceActivity.h"

using Elastos::Droid::Preference::PreferenceActivity;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::IPreferenceFragment;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

/**
 * Setting activity of Pinyin IME.
 */
class CSettingsActivity
    : public PreferenceActivity
{
private:
    class InnerListener
        : public Object
        , public IPreferenceOnPreferenceChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("CSettingsActivity::InnerListener")

        InnerListener(
            /* [in] */ CSettingsActivity* host);

        CARAPI OnPreferenceChange(
            /* [in] */ IPreference* preference,
            /* [in] */ IInterface* newValue,
            /* [out] */ Boolean* result);

    private:
        CSettingsActivity* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI OnPreferenceChange(
        /* [in] */ IPreference* preference,
        /* [in] */ IInterface* newValue,
        /* [out] */ Boolean* result);

    CARAPI_(void) UpdatePreference(
        /* [in] */ IPreferenceGroup* parentPref,
        /* [in] */ const String& prefKey);

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnResume();

    CARAPI OnDestroy();

    CARAPI OnPause();

private:
    CARAPI_(void) UpdateWidgets();

private:
    static const String TAG;

    AutoPtr<ICheckBoxPreference> mKeySoundPref;
    AutoPtr<ICheckBoxPreference> mVibratePref;
    AutoPtr<ICheckBoxPreference> mPredictionPref;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INPUTMETHOD_PINYIN_CSETTINGACTIVITY_H__
