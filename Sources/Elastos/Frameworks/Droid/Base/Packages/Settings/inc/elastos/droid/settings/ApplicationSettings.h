

package com.android.settings;

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::Preference::IOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Provider::ISettings;

public class ApplicationSettings extends SettingsPreferenceFragment {

    private static const String KEY_TOGGLE_ADVANCED_SETTINGS = "toggle_advanced_settings";
    private static const String KEY_APP_INSTALL_LOCATION = "app_install_location";

    // App installation location. Default is ask the user.
    private static const Int32 APP_INSTALL_AUTO = 0;
    private static const Int32 APP_INSTALL_DEVICE = 1;
    private static const Int32 APP_INSTALL_SDCARD = 2;

    private static const String APP_INSTALL_DEVICE_ID = "device";
    private static const String APP_INSTALL_SDCARD_ID = "sdcard";
    private static const String APP_INSTALL_AUTO_ID = "auto";

    private CheckBoxPreference mToggleAdvancedSettings;
    private ListPreference mInstallLocation;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        AddPreferencesFromResource(R.xml.application_settings);

        mToggleAdvancedSettings = (CheckBoxPreference)FindPreference(
                KEY_TOGGLE_ADVANCED_SETTINGS);
        mToggleAdvancedSettings->SetChecked(IsAdvancedSettingsEnabled());
        GetPreferenceScreen()->RemovePreference(mToggleAdvancedSettings);

        // not ready for prime time yet
        if (FALSE) {
            GetPreferenceScreen()->RemovePreference(mInstallLocation);
        }

        mInstallLocation = (ListPreference) FindPreference(KEY_APP_INSTALL_LOCATION);
        // Is app default install location set?
        Boolean userSetInstLocation = (Settings::Global::>GetInt(GetContentResolver(),
                Settings::Global::SET_INSTALL_LOCATION, 0) != 0);
        if (!userSetInstLocation) {
            GetPreferenceScreen()->RemovePreference(mInstallLocation);
        } else {
            mInstallLocation->SetValue(GetAppInstallLocation());
            mInstallLocation->SetOnPreferenceChangeListener(new OnPreferenceChangeListener() {
                public Boolean OnPreferenceChange(Preference preference, Object newValue) {
                    String value = (String) newValue;
                    HandleUpdateAppInstallLocation(value);
                    return FALSE;
                }
            });
        }
    }

    protected void HandleUpdateAppInstallLocation(final String value) {
        If(APP_INSTALL_DEVICE_ID->Equals(value)) {
            Settings::Global::>PutInt(GetContentResolver(),
                    Settings::Global::DEFAULT_INSTALL_LOCATION, APP_INSTALL_DEVICE);
        } else if (APP_INSTALL_SDCARD_ID->Equals(value)) {
            Settings::Global::>PutInt(GetContentResolver(),
                    Settings::Global::DEFAULT_INSTALL_LOCATION, APP_INSTALL_SDCARD);
        } else if (APP_INSTALL_AUTO_ID->Equals(value)) {
            Settings::Global::>PutInt(GetContentResolver(),
                    Settings::Global::DEFAULT_INSTALL_LOCATION, APP_INSTALL_AUTO);
        } else {
            // Should not happen, default to prompt...
            Settings::Global::>PutInt(GetContentResolver(),
                    Settings::Global::DEFAULT_INSTALL_LOCATION, APP_INSTALL_AUTO);
        }
        mInstallLocation->SetValue(value);
    }

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
        if (preference == mToggleAdvancedSettings) {
            Boolean value = mToggleAdvancedSettings->IsChecked();
            SetAdvancedSettingsEnabled(value);
        }

        return super->OnPreferenceTreeClick(preferenceScreen, preference);
    }

    private Boolean IsAdvancedSettingsEnabled() {
        return Settings.System->GetInt(GetContentResolver(),
                                      Settings.System.ADVANCED_SETTINGS,
                                      Settings.System.ADVANCED_SETTINGS_DEFAULT) > 0;
    }

    private void SetAdvancedSettingsEnabled(Boolean enabled) {
        Int32 value = enabled ? 1 : 0;
        // Change the system setting
        Settings.Secure->PutInt(GetContentResolver(), Settings.System.ADVANCED_SETTINGS, value);
        // TODO: the settings thing should broadcast this for thread safety purposes.
        Intent intent = new Intent(IIntent::ACTION_ADVANCED_SETTINGS_CHANGED);
        intent->PutExtra("state", value);
        GetActivity()->SendBroadcast(intent);
    }

    private String GetAppInstallLocation() {
        Int32 selectedLocation = Settings::Global::>GetInt(GetContentResolver(),
                Settings::Global::DEFAULT_INSTALL_LOCATION, APP_INSTALL_AUTO);
        if (selectedLocation == APP_INSTALL_DEVICE) {
            return APP_INSTALL_DEVICE_ID;
        } else if (selectedLocation == APP_INSTALL_SDCARD) {
            return APP_INSTALL_SDCARD_ID;
        } else  if (selectedLocation == APP_INSTALL_AUTO) {
            return APP_INSTALL_AUTO_ID;
        } else {
            // Default value, should not happen.
            return APP_INSTALL_AUTO_ID;
        }
    }
}
