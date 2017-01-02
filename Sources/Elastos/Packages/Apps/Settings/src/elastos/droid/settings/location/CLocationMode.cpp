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

#include "Elastos.Droid.Provider.h"
#include "elastos/droid/settings/location/CLocationMode.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Location {

//===============================================================================
//                  CLocationMode::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL(CLocationMode::InnerListener, Object, IRadioButtonPreferenceOnClickListener)

CLocationMode::InnerListener::InnerListener(
    /* [in] */ CLocationMode* host)
    : mHost(host)
{}

ECode CLocationMode::InnerListener::OnRadioButtonClicked(
    /* [in] */ IRadioButtonPreference* emiter)
{
    return mHost->OnRadioButtonClicked(emiter);
}

//===============================================================================
//                  CLocationMode
//===============================================================================

const String CLocationMode::KEY_HIGH_ACCURACY("high_accuracy");
const String CLocationMode::KEY_BATTERY_SAVING("battery_saving");
const String CLocationMode::KEY_SENSORS_ONLY("sensors_only");

CAR_OBJECT_IMPL(CLocationMode)

CLocationMode::CLocationMode()
{}

CLocationMode::~CLocationMode()
{}

ECode CLocationMode::constructor()
{
    return LocationSettingsBase::constructor();
}

ECode CLocationMode::OnResume()
{
    LocationSettingsBase::OnResume();
    CreatePreferenceHierarchy();
    return NOERROR;
}

ECode CLocationMode::OnPause()
{
    return LocationSettingsBase::OnPause();
}

AutoPtr<IPreferenceScreen> CLocationMode::CreatePreferenceHierarchy()
{
    AutoPtr<IPreferenceScreen> root;
    GetPreferenceScreen((IPreferenceScreen**)&root);
    if (root != NULL) {
        IPreferenceGroup::Probe(root)->RemoveAll();
    }
    AddPreferencesFromResource(R::xml::location_mode);
    root = NULL;
    GetPreferenceScreen((IPreferenceScreen**)&root);

    IPreferenceGroup* _root = IPreferenceGroup::Probe(root);

    AutoPtr<IPreference> pref;
    _root->FindPreference(CoreUtils::Convert(KEY_HIGH_ACCURACY), (IPreference**)&pref);
    mHighAccuracy = IRadioButtonPreference::Probe(pref);
    pref = NULL;
    _root->FindPreference(CoreUtils::Convert(KEY_BATTERY_SAVING), (IPreference**)&pref);
    mBatterySaving = IRadioButtonPreference::Probe(pref);
    pref = NULL;
    _root->FindPreference(CoreUtils::Convert(KEY_SENSORS_ONLY), (IPreference**)&pref);
    mSensorsOnly = IRadioButtonPreference::Probe(pref);
    AutoPtr<InnerListener> listener = new InnerListener(this);
    mHighAccuracy->SetOnClickListener(listener);
    mBatterySaving->SetOnClickListener(listener);
    mSensorsOnly->SetOnClickListener(listener);

    RefreshLocationMode();
    return root;
}

void CLocationMode::UpdateRadioButtons(
    /* [in] */ IRadioButtonPreference* activated)
{
    if (activated == NULL) {
        ITwoStatePreference::Probe(mHighAccuracy)->SetChecked(FALSE);
        ITwoStatePreference::Probe(mBatterySaving)->SetChecked(FALSE);
        ITwoStatePreference::Probe(mSensorsOnly)->SetChecked(FALSE);
    }
    else if (activated == mHighAccuracy.Get()) {
        ITwoStatePreference::Probe(mHighAccuracy)->SetChecked(TRUE);
        ITwoStatePreference::Probe(mBatterySaving)->SetChecked(FALSE);
        ITwoStatePreference::Probe(mSensorsOnly)->SetChecked(FALSE);
    }
    else if (activated == mBatterySaving.Get()) {
        ITwoStatePreference::Probe(mHighAccuracy)->SetChecked(FALSE);
        ITwoStatePreference::Probe(mBatterySaving)->SetChecked(TRUE);
        ITwoStatePreference::Probe(mSensorsOnly)->SetChecked(FALSE);
    }
    else if (activated == mSensorsOnly.Get()) {
        ITwoStatePreference::Probe(mHighAccuracy)->SetChecked(FALSE);
        ITwoStatePreference::Probe(mBatterySaving)->SetChecked(FALSE);
        ITwoStatePreference::Probe(mSensorsOnly)->SetChecked(TRUE);
    }
}

ECode CLocationMode::OnRadioButtonClicked(
    /* [in] */ IRadioButtonPreference* emiter)
{
    Int32 mode = ISettingsSecure::LOCATION_MODE_OFF;
    if (emiter == mHighAccuracy.Get()) {
        mode = ISettingsSecure::LOCATION_MODE_HIGH_ACCURACY;
    }
    else if (emiter == mBatterySaving.Get()) {
        mode = ISettingsSecure::LOCATION_MODE_BATTERY_SAVING;
    }
    else if (emiter == mSensorsOnly.Get()) {
        mode = ISettingsSecure::LOCATION_MODE_SENSORS_ONLY;
    }
    return SetLocationMode(mode);
}

ECode CLocationMode::OnModeChanged(
    /* [in] */ Int32 mode,
    /* [in] */ Boolean restricted)
{
    switch (mode) {
        case ISettingsSecure::LOCATION_MODE_OFF:
            UpdateRadioButtons(NULL);
            break;
        case ISettingsSecure::LOCATION_MODE_SENSORS_ONLY:
            UpdateRadioButtons(mSensorsOnly);
            break;
        case ISettingsSecure::LOCATION_MODE_BATTERY_SAVING:
            UpdateRadioButtons(mBatterySaving);
            break;
        case ISettingsSecure::LOCATION_MODE_HIGH_ACCURACY:
            UpdateRadioButtons(mHighAccuracy);
            break;
        default:
            break;
    }

    Boolean enabled = (mode != ISettingsSecure::LOCATION_MODE_OFF) && !restricted;
    IPreference::Probe(mHighAccuracy)->SetEnabled(enabled);
    IPreference::Probe(mBatterySaving)->SetEnabled(enabled);
    IPreference::Probe(mSensorsOnly)->SetEnabled(enabled);
    return NOERROR;
}

Int32 CLocationMode::GetHelpResource()
{
    return R::string::help_url_location_access;
}

} // namespace Location
} // namespace Settings
} // namespace Droid
} // namespace Elastos