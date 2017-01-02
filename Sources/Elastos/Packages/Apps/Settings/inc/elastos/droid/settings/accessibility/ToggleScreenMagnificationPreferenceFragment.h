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

using Elastos::Droid::Provider::ISettings;

using Elastos::Droid::Settings::Widget::IToggleSwitch;
using Elastos::Droid::Settings::Widget::ToggleSwitch::IOnBeforeCheckedChangeListener;

public class ToggleScreenMagnificationPreferenceFragment
        extends ToggleFeaturePreferenceFragment {
    //@Override
    protected void OnPreferenceToggled(String preferenceKey, Boolean enabled) {
        Settings.Secure->PutInt(GetContentResolver(),
                Settings.Secure.ACCESSIBILITY_DISPLAY_MAGNIFICATION_ENABLED, enabled ? 1 : 0);
    }

    //@Override
    protected void OnInstallSwitchBarToggleSwitch() {
        super->OnInstallSwitchBarToggleSwitch();
        mToggleSwitch->SetOnBeforeCheckedChangeListener(new OnBeforeCheckedChangeListener() {
                //@Override
            public Boolean OnBeforeCheckedChanged(ToggleSwitch toggleSwitch, Boolean checked) {
                mSwitchBar->SetCheckedInternal(checked);
                GetArguments()->PutBoolean(AccessibilitySettings.EXTRA_CHECKED, checked);
                OnPreferenceToggled(mPreferenceKey, checked);
                return FALSE;
            }
        });
    }
}
