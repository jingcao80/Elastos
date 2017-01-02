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

package com.android.settings;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::ILog;

/**
 * Activity for modifying a setting using the Voice Interaction API. This activity
 * MUST only modify the setting if the intent was sent using
 * {@link android.service.voice.VoiceInteractionSession#startVoiceActivity startVoiceActivity}.
 */
abstract public class VoiceSettingsActivity extends Activity {

    private static const String TAG = "VoiceSettingsActivity";

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        if (IsVoiceInteraction()) {
            // Only permit if this is a voice interaction.
            OnVoiceSettingInteraction(GetIntent());
        } else {
            Logger::V(TAG, "Cannot modify settings without voice interaction");
        }
        Finish();
    }

    /**
     * Modify the setting as a voice interaction. The activity will finish
     * after this method is called.
     */
    abstract protected void OnVoiceSettingInteraction(Intent intent);
}
