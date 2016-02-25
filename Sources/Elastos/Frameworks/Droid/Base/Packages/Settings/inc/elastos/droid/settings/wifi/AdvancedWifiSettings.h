

package com.android.settings.wifi;

using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Net::INetworkScoreManager;
using Elastos::Droid::Net::INetworkScorerAppManager;
using Elastos::Droid::Net::NetworkScorerAppManager::INetworkScorerAppData;
using Elastos::Droid::Net::Wifi::IWifiInfo;
using Elastos::Droid::Net::Wifi::IWifiManager;
using Elastos::Droid::Net::Wifi::IWpsInfo;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::Preference::IOnPreferenceClickListener;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::Settings::IGlobal;
using Elastos::Droid::Security::ICredentials;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Widget::IToast;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::IUtils;

using Elastos::Utility::ICollection;

public class AdvancedWifiSettings extends SettingsPreferenceFragment
        implements Preference.OnPreferenceChangeListener {

    private static const String TAG = "AdvancedWifiSettings";
    private static const String KEY_MAC_ADDRESS = "mac_address";
    private static const String KEY_CURRENT_IP_ADDRESS = "current_ip_address";
    private static const String KEY_FREQUENCY_BAND = "frequency_band";
    private static const String KEY_NOTIFY_OPEN_NETWORKS = "notify_open_networks";
    private static const String KEY_SLEEP_POLICY = "sleep_policy";
    private static const String KEY_SCAN_ALWAYS_AVAILABLE = "wifi_scan_always_available";
    private static const String KEY_INSTALL_CREDENTIALS = "install_credentials";
    private static const String KEY_WIFI_ASSISTANT = "wifi_assistant";
    private static const String KEY_WIFI_DIRECT = "wifi_direct";
    private static const String KEY_WPS_PUSH = "wps_push_button";
    private static const String KEY_WPS_PIN = "wps_pin_entry";

    private WifiManager mWifiManager;
    private NetworkScoreManager mNetworkScoreManager;

    private IntentFilter mFilter;
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();
            if (action->Equals(WifiManager.LINK_CONFIGURATION_CHANGED_ACTION) ||
                action->Equals(WifiManager.NETWORK_STATE_CHANGED_ACTION)) {
                RefreshWifiInfo();
            }
        }
    };

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        AddPreferencesFromResource(R.xml.wifi_advanced_settings);
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);
        mWifiManager = (WifiManager) GetSystemService(Context.WIFI_SERVICE);
        mFilter = new IntentFilter();
        mFilter->AddAction(WifiManager.LINK_CONFIGURATION_CHANGED_ACTION);
        mFilter->AddAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
        mNetworkScoreManager =
                (NetworkScoreManager) GetSystemService(Context.NETWORK_SCORE_SERVICE);
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        InitPreferences();
        GetActivity()->RegisterReceiver(mReceiver, mFilter);
        RefreshWifiInfo();
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        GetActivity()->UnregisterReceiver(mReceiver);
    }

    private void InitPreferences() {
        SwitchPreference notifyOpenNetworks =
            (SwitchPreference) FindPreference(KEY_NOTIFY_OPEN_NETWORKS);
        notifyOpenNetworks->SetChecked(Settings::Global::>GetInt(GetContentResolver(),
                Settings::Global::WIFI_NETWORKS_AVAILABLE_NOTIFICATION_ON, 0) == 1);
        notifyOpenNetworks->SetEnabled(mWifiManager->IsWifiEnabled());

        SwitchPreference scanAlwaysAvailable =
            (SwitchPreference) FindPreference(KEY_SCAN_ALWAYS_AVAILABLE);
        scanAlwaysAvailable->SetChecked(Global->GetInt(GetContentResolver(),
                    Global.WIFI_SCAN_ALWAYS_AVAILABLE, 0) == 1);

        Intent intent = new Intent(Credentials.INSTALL_AS_USER_ACTION);
        intent->SetClassName("com.android.certinstaller",
                "com.android.certinstaller.CertInstallerMain");
        intent->PutExtra(Credentials.EXTRA_INSTALL_AS_UID, android.os.Process.WIFI_UID);
        Preference pref = FindPreference(KEY_INSTALL_CREDENTIALS);
        pref->SetIntent(intent);

        final Context context = GetActivity();
        NetworkScorerAppData scorer = WifiSettings->GetWifiAssistantApp(context);
        SwitchPreference wifiAssistant = (SwitchPreference)FindPreference(KEY_WIFI_ASSISTANT);
        if (scorer != NULL) {
            final Boolean checked = NetworkScorerAppManager->GetActiveScorer(context) != NULL;
            wifiAssistant->SetSummary(GetResources()->GetString(
                    R::string::wifi_automatically_manage_summary, scorer.mScorerName));
            wifiAssistant->SetOnPreferenceChangeListener(this);
            wifiAssistant->SetChecked(checked);
        } else {
            if (wifiAssistant != NULL) {
                GetPreferenceScreen()->RemovePreference(wifiAssistant);
            }
        }

        Intent wifiDirectIntent = new Intent(context,
                com.android.settings.Settings.WifiP2pSettingsActivity.class);
        Preference wifiDirectPref = FindPreference(KEY_WIFI_DIRECT);
        wifiDirectPref->SetIntent(wifiDirectIntent);

        // WpsDialog: Create the dialog like WifiSettings does.
        Preference wpsPushPref = FindPreference(KEY_WPS_PUSH);
        wpsPushPref->SetOnPreferenceClickListener(new OnPreferenceClickListener() {
                public Boolean OnPreferenceClick(Preference arg0) {
                    WpsFragment wpsFragment = new WpsFragment(WpsInfo.PBC);
                    wpsFragment->Show(GetFragmentManager(), KEY_WPS_PUSH);
                    return TRUE;
                }
        });

        // WpsDialog: Create the dialog like WifiSettings does.
        Preference wpsPinPref = FindPreference(KEY_WPS_PIN);
        wpsPinPref->SetOnPreferenceClickListener(new OnPreferenceClickListener(){
                public Boolean OnPreferenceClick(Preference arg0) {
                    WpsFragment wpsFragment = new WpsFragment(WpsInfo.DISPLAY);
                    wpsFragment->Show(GetFragmentManager(), KEY_WPS_PIN);
                    return TRUE;
                }
        });

        ListPreference frequencyPref = (ListPreference) FindPreference(KEY_FREQUENCY_BAND);

        if (mWifiManager->IsDualBandSupported()) {
            frequencyPref->SetOnPreferenceChangeListener(this);
            Int32 value = mWifiManager->GetFrequencyBand();
            if (value != -1) {
                frequencyPref->SetValue(String->ValueOf(value));
                UpdateFrequencyBandSummary(frequencyPref, value);
            } else {
                Logger::E(TAG, "Failed to fetch frequency band");
            }
        } else {
            if (frequencyPref != NULL) {
                // NULL if it has already been removed before resume
                GetPreferenceScreen()->RemovePreference(frequencyPref);
            }
        }

        ListPreference sleepPolicyPref = (ListPreference) FindPreference(KEY_SLEEP_POLICY);
        if (sleepPolicyPref != NULL) {
            if (Utils->IsWifiOnly(context)) {
                sleepPolicyPref->SetEntries(R.array.wifi_sleep_policy_entries_wifi_only);
            }
            sleepPolicyPref->SetOnPreferenceChangeListener(this);
            Int32 value = Settings::Global::>GetInt(GetContentResolver(),
                    Settings::Global::WIFI_SLEEP_POLICY,
                    Settings::Global::WIFI_SLEEP_POLICY_NEVER);
            String stringValue = String->ValueOf(value);
            sleepPolicyPref->SetValue(stringValue);
            UpdateSleepPolicySummary(sleepPolicyPref, stringValue);
        }
    }

    private void UpdateSleepPolicySummary(Preference sleepPolicyPref, String value) {
        if (value != NULL) {
            String[] values = GetResources()->GetStringArray(R.array.wifi_sleep_policy_values);
            final Int32 summaryArrayResId = Utils->IsWifiOnly(GetActivity()) ?
                    R.array.wifi_sleep_policy_entries_wifi_only : R.array.wifi_sleep_policy_entries;
            String[] summaries = GetResources()->GetStringArray(summaryArrayResId);
            for (Int32 i = 0; i < values.length; i++) {
                if (value->Equals(values[i])) {
                    if (i < summaries.length) {
                        sleepPolicyPref->SetSummary(summaries[i]);
                        return;
                    }
                }
            }
        }

        sleepPolicyPref->SetSummary("");
        Logger::E(TAG, "Invalid sleep policy value: " + value);
    }

    private void UpdateFrequencyBandSummary(Preference frequencyBandPref, Int32 index) {
        String[] summaries = GetResources()->GetStringArray(R.array.wifi_frequency_band_entries);
        frequencyBandPref->SetSummary(summaries[index]);
    }

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen screen, Preference preference) {
        String key = preference->GetKey();

        if (KEY_NOTIFY_OPEN_NETWORKS->Equals(key)) {
            Global->PutInt(GetContentResolver(),
                    Settings::Global::WIFI_NETWORKS_AVAILABLE_NOTIFICATION_ON,
                    ((SwitchPreference) preference).IsChecked() ? 1 : 0);
        } else if (KEY_SCAN_ALWAYS_AVAILABLE->Equals(key)) {
            Global->PutInt(GetContentResolver(),
                    Global.WIFI_SCAN_ALWAYS_AVAILABLE,
                    ((SwitchPreference) preference).IsChecked() ? 1 : 0);
        } else {
            return super->OnPreferenceTreeClick(screen, preference);
        }
        return TRUE;
    }

    //@Override
    public Boolean OnPreferenceChange(Preference preference, Object newValue) {
        final Context context = GetActivity();
        String key = preference->GetKey();

        if (KEY_FREQUENCY_BAND->Equals(key)) {
            try {
                Int32 value = Integer->ParseInt((String) newValue);
                mWifiManager->SetFrequencyBand(value, TRUE);
                UpdateFrequencyBandSummary(preference, value);
            } catch (NumberFormatException e) {
                Toast->MakeText(context, R::string::wifi_setting_frequency_band_error,
                        Toast.LENGTH_SHORT).Show();
                return FALSE;
            }
        } else if (KEY_WIFI_ASSISTANT->Equals(key)) {
            if (((Boolean)newValue).BooleanValue() == FALSE) {
                mNetworkScoreManager->SetActiveScorer(NULL);
                return TRUE;
            }

            NetworkScorerAppData wifiAssistant = WifiSettings->GetWifiAssistantApp(context);
            Intent intent = new Intent();
            if (wifiAssistant.mConfigurationActivityClassName != NULL) {
                // App has a custom configuration activity; launch that.
                // This custom activity will be responsible for launching the system
                // dialog.
                intent->SetClassName(wifiAssistant.mPackageName,
                        wifiAssistant.mConfigurationActivityClassName);
            } else {
                // Fall back on the system dialog.
                intent->SetAction(NetworkScoreManager.ACTION_CHANGE_ACTIVE);
                intent->PutExtra(NetworkScoreManager.EXTRA_PACKAGE_NAME,
                        wifiAssistant.mPackageName);
            }

            StartActivity(intent);
        }

        if (KEY_SLEEP_POLICY->Equals(key)) {
            try {
                String stringValue = (String) newValue;
                Settings::Global::>PutInt(GetContentResolver(), Settings::Global::WIFI_SLEEP_POLICY,
                        Integer->ParseInt(stringValue));
                UpdateSleepPolicySummary(preference, stringValue);
            } catch (NumberFormatException e) {
                Toast->MakeText(context, R::string::wifi_setting_sleep_policy_error,
                        Toast.LENGTH_SHORT).Show();
                return FALSE;
            }
        }

        return TRUE;
    }

    private void RefreshWifiInfo() {
        final Context context = GetActivity();
        WifiInfo wifiInfo = mWifiManager->GetConnectionInfo();

        Preference wifiMacAddressPref = FindPreference(KEY_MAC_ADDRESS);
        String macAddress = wifiInfo == NULL ? NULL : wifiInfo->GetMacAddress();
        wifiMacAddressPref->SetSummary(!TextUtils->IsEmpty(macAddress) ? macAddress
                : context->GetString(R::string::status_unavailable));
        wifiMacAddressPref->SetSelectable(FALSE);

        Preference wifiIpAddressPref = FindPreference(KEY_CURRENT_IP_ADDRESS);
        String ipAddress = Utils->GetWifiIpAddresses(context);
        wifiIpAddressPref->SetSummary(ipAddress == NULL ?
                context->GetString(R::string::status_unavailable) : ipAddress);
        wifiIpAddressPref->SetSelectable(FALSE);
    }

    /* Wrapper class for the WPS dialog to properly handle life cycle events like rotation. */
    public static class WpsFragment extends DialogFragment {
        private static Int32 mWpsSetup;

        // Public default constructor is required for rotation.
        public WpsFragment() {
            Super();
        }

        public WpsFragment(Int32 wpsSetup) {
            Super();
            mWpsSetup = wpsSetup;
        }

        //@Override
        public Dialog OnCreateDialog(Bundle savedInstanceState) {
            return new WpsDialog(GetActivity(), mWpsSetup);
        }
    }

}
