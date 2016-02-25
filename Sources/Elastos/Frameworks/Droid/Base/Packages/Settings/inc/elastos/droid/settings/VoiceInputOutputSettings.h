/*
 * Copyright (C) 2010 The Android Open Source Project
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

package com.android.settings;

using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Speech::Tts::ITtsEngines;

using Elastos::Droid::Settings::Voice::IVoiceInputHelper;

/**
 * Settings screen for voice input/output.
 */
public class VoiceInputOutputSettings {

    private static const String TAG = "VoiceInputOutputSettings";

    private static const String KEY_VOICE_CATEGORY = "voice_category";
    private static const String KEY_VOICE_INPUT_SETTINGS = "voice_input_settings";
    private static const String KEY_TTS_SETTINGS = "tts_settings";

    private PreferenceGroup mParent;
    private PreferenceCategory mVoiceCategory;
    private Preference mVoiceInputSettingsPref;
    private Preference mTtsSettingsPref;
    private final SettingsPreferenceFragment mFragment;
    private final TtsEngines mTtsEngines;

    public VoiceInputOutputSettings(SettingsPreferenceFragment fragment) {
        mFragment = fragment;
        mTtsEngines = new TtsEngines(fragment->GetPreferenceScreen()->GetContext());
    }

    CARAPI OnCreate() {

        mParent = mFragment->GetPreferenceScreen();
        mVoiceCategory = (PreferenceCategory) mParent->FindPreference(KEY_VOICE_CATEGORY);
        mVoiceInputSettingsPref = mVoiceCategory->FindPreference(KEY_VOICE_INPUT_SETTINGS);
        mTtsSettingsPref = mVoiceCategory->FindPreference(KEY_TTS_SETTINGS);

        PopulateOrRemovePreferences();
    }

    private void PopulateOrRemovePreferences() {
        Boolean hasVoiceInputPrefs = PopulateOrRemoveVoiceInputPrefs();
        Boolean hasTtsPrefs = PopulateOrRemoveTtsPrefs();
        if (!hasVoiceInputPrefs && !hasTtsPrefs) {
            // There were no TTS settings and no recognizer settings,
            // so it should be safe to hide the preference category
            // entirely.
            mFragment->GetPreferenceScreen()->RemovePreference(mVoiceCategory);
        }
    }

    private Boolean PopulateOrRemoveVoiceInputPrefs() {
        VoiceInputHelper helper = new VoiceInputHelper(mFragment->GetActivity());
        if (!helper->HasItems()) {
            mVoiceCategory->RemovePreference(mVoiceInputSettingsPref);
            return FALSE;
        }

        return TRUE;
    }

    private Boolean PopulateOrRemoveTtsPrefs() {
        if (mTtsEngines->GetEngines()->IsEmpty()) {
            mVoiceCategory->RemovePreference(mTtsSettingsPref);
            return FALSE;
        }

        return TRUE;
    }
}
