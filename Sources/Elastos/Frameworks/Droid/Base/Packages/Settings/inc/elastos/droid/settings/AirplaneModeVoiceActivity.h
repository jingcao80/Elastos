

package com.android.settings;

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Utility::ILog;

/**
 * Activity for modifying the {@link Settings::Global::AIRPLANE_MODE_ON AIRPLANE_MODE_ON}
 * setting using the Voice Interaction API.
 */
public class AirplaneModeVoiceActivity extends VoiceSettingsActivity {
    private static const String TAG = "AirplaneModeVoiceActivity";

    protected void OnVoiceSettingInteraction(Intent intent) {
        if (intent->HasExtra(Settings.EXTRA_AIRPLANE_MODE_ENABLED)) {
            Boolean enabled =
                    intent->GetBooleanExtra(Settings.EXTRA_AIRPLANE_MODE_ENABLED, FALSE);
            Settings::Global::>PutInt(GetContentResolver(),
                    Settings::Global::AIRPLANE_MODE_ON, enabled ? 1 : 0);
        } else {
            Logger::V(TAG, "Missing airplane mode extra");
        }
    }
}
