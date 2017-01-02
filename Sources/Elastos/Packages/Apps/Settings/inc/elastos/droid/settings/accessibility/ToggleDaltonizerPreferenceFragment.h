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

package com.android.settings.accessibility;

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;

using Elastos::Droid::Widget::ISwitch;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::Widget::ISwitchBar;

public class ToggleDaltonizerPreferenceFragment extends ToggleFeaturePreferenceFragment
        implements Preference.OnPreferenceChangeListener, SwitchBar.OnSwitchChangeListener {
    private static const String ENABLED = Settings.Secure.ACCESSIBILITY_DISPLAY_DALTONIZER_ENABLED;
    private static const String TYPE = Settings.Secure.ACCESSIBILITY_DISPLAY_DALTONIZER;
    private static const Int32 DEFAULT_TYPE = AccessibilityManager.DALTONIZER_CORRECT_DEUTERANOMALY;

    private ListPreference mType;

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        AddPreferencesFromResource(R.xml.accessibility_daltonizer_settings);

        mType = (ListPreference) FindPreference("type");

        InitPreferences();
    }

    //@Override
    protected void OnPreferenceToggled(String preferenceKey, Boolean enabled) {
        Settings.Secure->PutInt(GetContentResolver(), ENABLED, enabled ? 1 : 0);
    }

    //@Override
    public Boolean OnPreferenceChange(Preference preference, Object newValue) {
        if (preference == mType) {
            Settings.Secure->PutInt(GetContentResolver(), TYPE, Integer->ParseInt((String) newValue));
            preference->SetSummary("%s");
        }

        return TRUE;
    }

    //@Override
    CARAPI OnViewCreated(View view, Bundle savedInstanceState) {
        super->OnViewCreated(view, savedInstanceState);

        SetTitle(GetString(R::string::accessibility_display_daltonizer_preference_title));
    }

    //@Override
    protected void OnInstallSwitchBarToggleSwitch() {
        super->OnInstallSwitchBarToggleSwitch();

        mSwitchBar->SetCheckedInternal(
                Settings.Secure->GetInt(GetContentResolver(), ENABLED, 0) == 1);
        mSwitchBar->AddOnSwitchChangeListener(this);
    }

    //@Override
    protected void OnRemoveSwitchBarToggleSwitch() {
        super->OnRemoveSwitchBarToggleSwitch();
        mSwitchBar->RemoveOnSwitchChangeListener(this);
    }

    private void InitPreferences() {
        final String value = Integer->ToString(
                Settings.Secure->GetInt(GetContentResolver(), TYPE, DEFAULT_TYPE));
        mType->SetValue(value);
        mType->SetOnPreferenceChangeListener(this);
        final Int32 index = mType->FindIndexOfValue(value);
        if (index < 0) {
            // We're using a mode controlled by developer preferences.
            mType->SetSummary(GetString(R::string::daltonizer_type_overridden,
                    GetString(R::string::simulate_color_space)));
        }
    }

    //@Override
    CARAPI OnSwitchChanged(Switch switchView, Boolean isChecked) {
        OnPreferenceToggled(mPreferenceKey, isChecked);
    }
}
