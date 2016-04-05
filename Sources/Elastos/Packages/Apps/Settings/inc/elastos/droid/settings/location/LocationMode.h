/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.settings.location;

using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Provider::ISettings;

using Elastos::Droid::Settings::IR;

/**
 * A page with 3 radio buttons to choose the location mode.
 *
 * There are 3 location modes when location access is enabled:
 *
 * High accuracy: use both GPS and network location.
 *
 * Battery saving: use network location only to reduce the power consumption.
 *
 * Sensors only: use GPS location only.
 */
public class LocationMode extends LocationSettingsBase
        implements RadioButtonPreference.OnClickListener {
    private static const String KEY_HIGH_ACCURACY = "high_accuracy";
    private RadioButtonPreference mHighAccuracy;
    private static const String KEY_BATTERY_SAVING = "battery_saving";
    private RadioButtonPreference mBatterySaving;
    private static const String KEY_SENSORS_ONLY = "sensors_only";
    private RadioButtonPreference mSensorsOnly;

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        CreatePreferenceHierarchy();
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
    }

    private PreferenceScreen CreatePreferenceHierarchy() {
        PreferenceScreen root = GetPreferenceScreen();
        if (root != NULL) {
            root->RemoveAll();
        }
        AddPreferencesFromResource(R.xml.location_mode);
        root = GetPreferenceScreen();

        mHighAccuracy = (RadioButtonPreference) root->FindPreference(KEY_HIGH_ACCURACY);
        mBatterySaving = (RadioButtonPreference) root->FindPreference(KEY_BATTERY_SAVING);
        mSensorsOnly = (RadioButtonPreference) root->FindPreference(KEY_SENSORS_ONLY);
        mHighAccuracy->SetOnClickListener(this);
        mBatterySaving->SetOnClickListener(this);
        mSensorsOnly->SetOnClickListener(this);

        RefreshLocationMode();
        return root;
    }

    private void UpdateRadioButtons(RadioButtonPreference activated) {
        if (activated == NULL) {
            mHighAccuracy->SetChecked(FALSE);
            mBatterySaving->SetChecked(FALSE);
            mSensorsOnly->SetChecked(FALSE);
        } else if (activated == mHighAccuracy) {
            mHighAccuracy->SetChecked(TRUE);
            mBatterySaving->SetChecked(FALSE);
            mSensorsOnly->SetChecked(FALSE);
        } else if (activated == mBatterySaving) {
            mHighAccuracy->SetChecked(FALSE);
            mBatterySaving->SetChecked(TRUE);
            mSensorsOnly->SetChecked(FALSE);
        } else if (activated == mSensorsOnly) {
            mHighAccuracy->SetChecked(FALSE);
            mBatterySaving->SetChecked(FALSE);
            mSensorsOnly->SetChecked(TRUE);
        }
    }

    //@Override
    CARAPI OnRadioButtonClicked(RadioButtonPreference emiter) {
        Int32 mode = Settings.Secure.LOCATION_MODE_OFF;
        if (emiter == mHighAccuracy) {
            mode = Settings.Secure.LOCATION_MODE_HIGH_ACCURACY;
        } else if (emiter == mBatterySaving) {
            mode = Settings.Secure.LOCATION_MODE_BATTERY_SAVING;
        } else if (emiter == mSensorsOnly) {
            mode = Settings.Secure.LOCATION_MODE_SENSORS_ONLY;
        }
        SetLocationMode(mode);
    }

    //@Override
    CARAPI OnModeChanged(Int32 mode, Boolean restricted) {
        switch (mode) {
            case Settings.Secure.LOCATION_MODE_OFF:
                UpdateRadioButtons(NULL);
                break;
            case Settings.Secure.LOCATION_MODE_SENSORS_ONLY:
                UpdateRadioButtons(mSensorsOnly);
                break;
            case Settings.Secure.LOCATION_MODE_BATTERY_SAVING:
                UpdateRadioButtons(mBatterySaving);
                break;
            case Settings.Secure.LOCATION_MODE_HIGH_ACCURACY:
                UpdateRadioButtons(mHighAccuracy);
                break;
            default:
                break;
        }

        Boolean enabled = (mode != Settings.Secure.LOCATION_MODE_OFF) && !restricted;
        mHighAccuracy->SetEnabled(enabled);
        mBatterySaving->SetEnabled(enabled);
        mSensorsOnly->SetEnabled(enabled);
    }

    //@Override
    public Int32 GetHelpResource() {
        return R::string::help_url_location_access;
    }
}
