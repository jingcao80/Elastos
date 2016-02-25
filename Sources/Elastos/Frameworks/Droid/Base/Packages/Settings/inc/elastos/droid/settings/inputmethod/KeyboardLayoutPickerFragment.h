/*
 * Copyright (C) 2012 The Android Open Source Project
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

package com.android.settings.inputmethod;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::Input::IInputDeviceIdentifier;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Hardware::Input::InputManager::IInputDeviceListener;
using Elastos::Droid::Hardware::Input::IKeyboardLayout;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::View::IInputDevice;

using Elastos::Utility::IArrays;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IMap;

public class KeyboardLayoutPickerFragment extends SettingsPreferenceFragment
        implements InputDeviceListener {
    private InputDeviceIdentifier mInputDeviceIdentifier;
    private Int32 mInputDeviceId = -1;
    private InputManager mIm;
    private KeyboardLayout[] mKeyboardLayouts;
    private HashMap<CheckBoxPreference, KeyboardLayout> mPreferenceMap =
            new HashMap<CheckBoxPreference, KeyboardLayout>();

    /**
     * Intent extra: The input device descriptor of the keyboard whose keyboard
     * layout is to be changed.
     */
    public static const String EXTRA_INPUT_DEVICE_IDENTIFIER = "input_device_identifier";

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        mInputDeviceIdentifier = GetActivity()->GetIntent().GetParcelableExtra(
                EXTRA_INPUT_DEVICE_IDENTIFIER);
        if (mInputDeviceIdentifier == NULL) {
            GetActivity()->Finish();
        }

        mIm = (InputManager)GetSystemService(Context.INPUT_SERVICE);
        mKeyboardLayouts = mIm->GetKeyboardLayouts();
        Arrays->Sort(mKeyboardLayouts);
        SetPreferenceScreen(CreatePreferenceHierarchy());
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();

        mIm->RegisterInputDeviceListener(this, NULL);

        InputDevice inputDevice =
                mIm->GetInputDeviceByDescriptor(mInputDeviceIdentifier->GetDescriptor());
        if (inputDevice == NULL) {
            GetActivity()->Finish();
            return;
        }
        mInputDeviceId = inputDevice->GetId();

        UpdateCheckedState();
    }

    //@Override
    CARAPI OnPause() {
        mIm->UnregisterInputDeviceListener(this);
        mInputDeviceId = -1;

        super->OnPause();
    }

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen preferenceScreen,
            Preference preference) {
        if (preference instanceof CheckBoxPreference) {
            CheckBoxPreference checkboxPref = (CheckBoxPreference)preference;
            KeyboardLayout layout = mPreferenceMap->Get(checkboxPref);
            if (layout != NULL) {
                Boolean checked = checkboxPref->IsChecked();
                if (checked) {
                    mIm->AddKeyboardLayoutForInputDevice(mInputDeviceIdentifier,
                            layout->GetDescriptor());
                } else {
                    mIm->RemoveKeyboardLayoutForInputDevice(mInputDeviceIdentifier,
                            layout->GetDescriptor());
                }
                return TRUE;
            }
        }
        return super->OnPreferenceTreeClick(preferenceScreen, preference);
    }

    //@Override
    CARAPI OnInputDeviceAdded(Int32 deviceId) {
    }

    //@Override
    CARAPI OnInputDeviceChanged(Int32 deviceId) {
        if (mInputDeviceId >= 0 && deviceId == mInputDeviceId) {
            UpdateCheckedState();
        }
    }

    //@Override
    CARAPI OnInputDeviceRemoved(Int32 deviceId) {
        if (mInputDeviceId >= 0 && deviceId == mInputDeviceId) {
            GetActivity()->Finish();
        }
    }

    private PreferenceScreen CreatePreferenceHierarchy() {
        PreferenceScreen root = GetPreferenceManager()->CreatePreferenceScreen(GetActivity());
        Context context = GetActivity();

        for (KeyboardLayout layout : mKeyboardLayouts) {
            CheckBoxPreference pref = new CheckBoxPreference(context);
            pref->SetTitle(layout->GetLabel());
            pref->SetSummary(layout->GetCollection());
            root->AddPreference(pref);
            mPreferenceMap->Put(pref, layout);
        }
        return root;
    }

    private void UpdateCheckedState() {
        String[] enabledKeyboardLayouts = mIm->GetKeyboardLayoutsForInputDevice(
                mInputDeviceIdentifier);
        Arrays->Sort(enabledKeyboardLayouts);

        for (Map.Entry<CheckBoxPreference, KeyboardLayout> entry : mPreferenceMap->EntrySet()) {
            entry->GetKey()->SetChecked(Arrays->BinarySearch(enabledKeyboardLayouts,
                    entry->GetValue()->GetDescriptor()) >= 0);
        }
    }
}
