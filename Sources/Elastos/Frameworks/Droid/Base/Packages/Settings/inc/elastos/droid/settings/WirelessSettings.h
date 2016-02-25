/*
 * Copyright (C) 2009 The Android Open Source Project
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


using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Nfc::INfcAdapter;
using Elastos::Droid::Nfc::INfcManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::Preference::IOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Internal::Telephony::ISmsApplication;
using Elastos::Droid::Internal::Telephony::SmsApplication::ISmsApplicationData;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Settings::Nfc::INfcEnabler;
using Elastos::Droid::Settings::Search::IBaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrays;
using Elastos::Utility::ICollection;
using Elastos::Utility::IList;

public class WirelessSettings extends SettingsPreferenceFragment
        implements OnPreferenceChangeListener, Indexable {
    private static const String TAG = "WirelessSettings";

    private static const String KEY_TOGGLE_AIRPLANE = "toggle_airplane";
    private static const String KEY_TOGGLE_NFC = "toggle_nfc";
    private static const String KEY_WIMAX_SETTINGS = "wimax_settings";
    private static const String KEY_ANDROID_BEAM_SETTINGS = "android_beam_settings";
    private static const String KEY_VPN_SETTINGS = "vpn_settings";
    private static const String KEY_TETHER_SETTINGS = "tether_settings";
    private static const String KEY_PROXY_SETTINGS = "proxy_settings";
    private static const String KEY_MOBILE_NETWORK_SETTINGS = "mobile_network_settings";
    private static const String KEY_MANAGE_MOBILE_PLAN = "manage_mobile_plan";
    private static const String KEY_SMS_APPLICATION = "sms_application";
    private static const String KEY_TOGGLE_NSD = "toggle_nsd"; //network service discovery
    private static const String KEY_CELL_BROADCAST_SETTINGS = "cell_broadcast_settings";

    public static const String EXIT_ECM_RESULT = "exit_ecm_result";
    public static const Int32 REQUEST_CODE_EXIT_ECM = 1;

    private AirplaneModeEnabler mAirplaneModeEnabler;
    private SwitchPreference mAirplaneModePreference;
    private NfcEnabler mNfcEnabler;
    private NfcAdapter mNfcAdapter;
    private NsdEnabler mNsdEnabler;

    private ConnectivityManager mCm;
    private TelephonyManager mTm;
    private PackageManager mPm;
    private UserManager mUm;

    private static const Int32 MANAGE_MOBILE_PLAN_DIALOG_ID = 1;
    private static const String SAVED_MANAGE_MOBILE_PLAN_MSG = "mManageMobilePlanMessage";

    private AppListPreference mSmsApplicationPreference;

    /**
     * Invoked on each preference click in this hierarchy, overrides
     * PreferenceFragment's implementation.  Used to make sure we track the
     * preference click events.
     */
    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
        Log("onPreferenceTreeClick: preference=" + preference);
        if (preference == mAirplaneModePreference && Boolean->ParseBoolean(
                SystemProperties->Get(TelephonyProperties.PROPERTY_INECM_MODE))) {
            // In ECM mode launch ECM app dialog
            StartActivityForResult(
                new Intent(TelephonyIntents.ACTION_SHOW_NOTICE_ECM_BLOCK_OTHERS, NULL),
                REQUEST_CODE_EXIT_ECM);
            return TRUE;
        } else if (preference == FindPreference(KEY_MANAGE_MOBILE_PLAN)) {
            OnManageMobilePlanClick();
        }
        // Let the intents be launched by the Preference manager
        return super->OnPreferenceTreeClick(preferenceScreen, preference);
    }

    private String mManageMobilePlanMessage;
    CARAPI OnManageMobilePlanClick() {
        Log("onManageMobilePlanClick:");
        mManageMobilePlanMessage = NULL;
        Resources resources = GetActivity()->GetResources();

        NetworkInfo ni = mCm->GetProvisioningOrActiveNetworkInfo();
        if (mTm->HasIccCard() && (ni != NULL)) {
            // Check for carrier apps that can handle provisioning first
            Intent provisioningIntent = new Intent(TelephonyIntents.ACTION_CARRIER_SETUP);
            List<String> carrierPackages =
                    mTm->GetCarrierPackageNamesForIntent(provisioningIntent);
            if (carrierPackages != NULL && !carrierPackages->IsEmpty()) {
                if (carrierPackages->Size() != 1) {
                    Logger::W(TAG, "Multiple matching carrier apps found, launching the first.");
                }
                provisioningIntent->SetPackage(carrierPackages->Get(0));
                StartActivity(provisioningIntent);
                return;
            }

            // Get provisioning URL
            String url = mCm->GetMobileProvisioningUrl();
            if (!TextUtils->IsEmpty(url)) {
                Intent intent = Intent->MakeMainSelectorActivity(IIntent::ACTION_MAIN,
                        Intent.CATEGORY_APP_BROWSER);
                intent->SetData(Uri->Parse(url));
                intent->SetFlags(IIntent::FLAG_ACTIVITY_BROUGHT_TO_FRONT |
                        IIntent::FLAG_ACTIVITY_NEW_TASK);
                try {
                    StartActivity(intent);
                } catch (ActivityNotFoundException e) {
                    Logger::W(TAG, "onManageMobilePlanClick: startActivity failed" + e);
                }
            } else {
                // No provisioning URL
                String operatorName = mTm->GetSimOperatorName();
                if (TextUtils->IsEmpty(operatorName)) {
                    // Use NetworkOperatorName as second choice in case there is no
                    // SPN (Service Provider Name on the SIM). Such as with T-mobile.
                    operatorName = mTm->GetNetworkOperatorName();
                    if (TextUtils->IsEmpty(operatorName)) {
                        mManageMobilePlanMessage = resources->GetString(
                                R::string::mobile_unknown_sim_operator);
                    } else {
                        mManageMobilePlanMessage = resources->GetString(
                                R::string::mobile_no_provisioning_url, operatorName);
                    }
                } else {
                    mManageMobilePlanMessage = resources->GetString(
                            R::string::mobile_no_provisioning_url, operatorName);
                }
            }
        } else if (mTm->HasIccCard() == FALSE) {
            // No sim card
            mManageMobilePlanMessage = resources->GetString(R::string::mobile_insert_sim_card);
        } else {
            // NetworkInfo is NULL, there is no connection
            mManageMobilePlanMessage = resources->GetString(R::string::mobile_connect_to_internet);
        }
        if (!TextUtils->IsEmpty(mManageMobilePlanMessage)) {
            Log("onManageMobilePlanClick: message=" + mManageMobilePlanMessage);
            ShowDialog(MANAGE_MOBILE_PLAN_DIALOG_ID);
        }
    }

    private void InitSmsApplicationSetting() {
        Log("initSmsApplicationSetting:");
        Collection<SmsApplicationData> smsApplications =
                SmsApplication->GetApplicationCollection(GetActivity());

        // If the list is empty the dialog will be empty, but we will not crash.
        Int32 count = smsApplications->Size();
        String[] packageNames = new String[count];
        Int32 i = 0;
        for (SmsApplicationData smsApplicationData : smsApplications) {
            packageNames[i] = smsApplicationData.mPackageName;
            i++;
        }
        String defaultPackageName = NULL;
        ComponentName appName = SmsApplication->GetDefaultSmsApplication(GetActivity(), TRUE);
        if (appName != NULL) {
            defaultPackageName = appName->GetPackageName();
        }
        mSmsApplicationPreference->SetPackageNames(packageNames, defaultPackageName);
    }

    //@Override
    public Dialog OnCreateDialog(Int32 dialogId) {
        Log("onCreateDialog: dialogId=" + dialogId);
        switch (dialogId) {
            case MANAGE_MOBILE_PLAN_DIALOG_ID:
                return new AlertDialog->Builder(GetActivity())
                            .SetMessage(mManageMobilePlanMessage)
                            .SetCancelable(FALSE)
                            .SetPositiveButton(R::string::ok,
                                    new DialogInterface->OnClickListener() {
                                //@Override
                                CARAPI OnClick(DialogInterface dialog, Int32 id) {
                                    Log("MANAGE_MOBILE_PLAN_DIALOG.onClickListener id=" + id);
                                    mManageMobilePlanMessage = NULL;
                                }
                            })
                            .Create();
        }
        return super->OnCreateDialog(dialogId);
    }

    private void Log(String s) {
        Logger::D(TAG, s);
    }

    public static Boolean IsRadioAllowed(Context context, String type) {
        if (!AirplaneModeEnabler->IsAirplaneModeOn(context)) {
            return TRUE;
        }
        // Here we use the same logic in OnCreate().
        String toggleable = Settings::Global::>GetString(context->GetContentResolver(),
                Settings::Global::AIRPLANE_MODE_TOGGLEABLE_RADIOS);
        return toggleable != NULL && toggleable->Contains(type);
    }

    private Boolean IsSmsSupported() {
        // Some tablet has sim card but could not do telephony operations. Skip those.
        return mTm->IsSmsCapable();
    }

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        if (savedInstanceState != NULL) {
            mManageMobilePlanMessage = savedInstanceState->GetString(SAVED_MANAGE_MOBILE_PLAN_MSG);
        }
        Log("onCreate: mManageMobilePlanMessage=" + mManageMobilePlanMessage);

        mCm = (ConnectivityManager) GetSystemService(Context.CONNECTIVITY_SERVICE);
        mTm = (TelephonyManager) GetSystemService(Context.TELEPHONY_SERVICE);
        mPm = GetPackageManager();
        mUm = (UserManager) GetSystemService(Context.USER_SERVICE);

        AddPreferencesFromResource(R.xml.wireless_settings);

        final Boolean isSecondaryUser = UserHandle->MyUserId() != UserHandle.USER_OWNER;

        final Activity activity = GetActivity();
        mAirplaneModePreference = (SwitchPreference) FindPreference(KEY_TOGGLE_AIRPLANE);
        SwitchPreference nfc = (SwitchPreference) FindPreference(KEY_TOGGLE_NFC);
        PreferenceScreen androidBeam = (PreferenceScreen) FindPreference(KEY_ANDROID_BEAM_SETTINGS);
        CheckBoxPreference nsd = (CheckBoxPreference) FindPreference(KEY_TOGGLE_NSD);

        mAirplaneModeEnabler = new AirplaneModeEnabler(activity, mAirplaneModePreference);
        mNfcEnabler = new NfcEnabler(activity, nfc, androidBeam);

        mSmsApplicationPreference = (AppListPreference) FindPreference(KEY_SMS_APPLICATION);
        mSmsApplicationPreference->SetOnPreferenceChangeListener(this);
        InitSmsApplicationSetting();

        // Remove NSD checkbox by default
        GetPreferenceScreen()->RemovePreference(nsd);
        //mNsdEnabler = new NsdEnabler(activity, nsd);

        String toggleable = Settings::Global::>GetString(activity->GetContentResolver(),
                Settings::Global::AIRPLANE_MODE_TOGGLEABLE_RADIOS);

        //enable/disable wimax depending on the value in config.xml
        final Boolean isWimaxEnabled = !isSecondaryUser && this->GetResources()->GetBoolean(
                R.bool.config_wimaxEnabled);
        if (!isWimaxEnabled
                || mUm->HasUserRestriction(UserManager.DISALLOW_CONFIG_MOBILE_NETWORKS)) {
            PreferenceScreen root = GetPreferenceScreen();
            Preference ps = (Preference) FindPreference(KEY_WIMAX_SETTINGS);
            if (ps != NULL) root->RemovePreference(ps);
        } else {
            if (toggleable == NULL || !toggleable->Contains(Settings::Global::RADIO_WIMAX )
                    && isWimaxEnabled) {
                Preference ps = (Preference) FindPreference(KEY_WIMAX_SETTINGS);
                ps->SetDependency(KEY_TOGGLE_AIRPLANE);
            }
        }

        // Manually set dependencies for Wifi when not toggleable.
        if (toggleable == NULL || !toggleable->Contains(Settings::Global::RADIO_WIFI)) {
            FindPreference(KEY_VPN_SETTINGS).SetDependency(KEY_TOGGLE_AIRPLANE);
        }
        // Disable VPN.
        if (isSecondaryUser || mUm->HasUserRestriction(UserManager.DISALLOW_CONFIG_VPN)) {
            RemovePreference(KEY_VPN_SETTINGS);
        }

        // Manually set dependencies for Bluetooth when not toggleable.
        if (toggleable == NULL || !toggleable->Contains(Settings::Global::RADIO_BLUETOOTH)) {
            // No bluetooth-dependent items in the list. Code kept in case one is added later.
        }

        // Manually set dependencies for NFC when not toggleable.
        if (toggleable == NULL || !toggleable->Contains(Settings::Global::RADIO_NFC)) {
            FindPreference(KEY_TOGGLE_NFC).SetDependency(KEY_TOGGLE_AIRPLANE);
            FindPreference(KEY_ANDROID_BEAM_SETTINGS).SetDependency(KEY_TOGGLE_AIRPLANE);
        }

        // Remove NFC if not available
        mNfcAdapter = NfcAdapter->GetDefaultAdapter(activity);
        if (mNfcAdapter == NULL) {
            GetPreferenceScreen()->RemovePreference(nfc);
            GetPreferenceScreen()->RemovePreference(androidBeam);
            mNfcEnabler = NULL;
        }

        // Remove Mobile Network Settings and Manage Mobile Plan for secondary users,
        // if it's a wifi-only device, or if the settings are restricted.
        if (isSecondaryUser || Utils->IsWifiOnly(GetActivity())
                || mUm->HasUserRestriction(UserManager.DISALLOW_CONFIG_MOBILE_NETWORKS)) {
            RemovePreference(KEY_MOBILE_NETWORK_SETTINGS);
            RemovePreference(KEY_MANAGE_MOBILE_PLAN);
        }
        // Remove Mobile Network Settings and Manage Mobile Plan
        // if config_show_mobile_plan sets FALSE.
        final Boolean isMobilePlanEnabled = this->GetResources()->GetBoolean(
                R.bool.config_show_mobile_plan);
        if (!isMobilePlanEnabled) {
            Preference pref = FindPreference(KEY_MANAGE_MOBILE_PLAN);
            if (pref != NULL) {
                RemovePreference(KEY_MANAGE_MOBILE_PLAN);
            }
        }

        // Remove SMS Application if the device does not support SMS
        if (!IsSmsSupported()) {
            RemovePreference(KEY_SMS_APPLICATION);
        }

        // Remove Airplane Mode settings if it's a stationary device such as a TV.
        if (mPm->HasSystemFeature(PackageManager.FEATURE_TELEVISION)) {
            RemovePreference(KEY_TOGGLE_AIRPLANE);
        }

        // Enable Proxy selector settings if allowed.
        Preference mGlobalProxy = FindPreference(KEY_PROXY_SETTINGS);
        final DevicePolicyManager mDPM = (DevicePolicyManager)
                activity->GetSystemService(Context.DEVICE_POLICY_SERVICE);
        // proxy UI disabled until we have better app support
        GetPreferenceScreen()->RemovePreference(mGlobalProxy);
        mGlobalProxy->SetEnabled(mDPM->GetGlobalProxyAdmin() == NULL);

        // Disable Tethering if it's not allowed or if it's a wifi-only device
        final ConnectivityManager cm =
                (ConnectivityManager) activity->GetSystemService(Context.CONNECTIVITY_SERVICE);
        if (isSecondaryUser || !cm->IsTetheringSupported()
                || mUm->HasUserRestriction(UserManager.DISALLOW_CONFIG_TETHERING)) {
            GetPreferenceScreen()->RemovePreference(FindPreference(KEY_TETHER_SETTINGS));
        } else {
            Preference p = FindPreference(KEY_TETHER_SETTINGS);
            p->SetTitle(Utils->GetTetheringLabel(cm));

            // Grey out if provisioning is not available.
            p->SetEnabled(!TetherSettings
                    .IsProvisioningNeededButUnavailable(GetActivity()));
        }

        // Enable link to CMAS app settings depending on the value in config.xml.
        Boolean isCellBroadcastAppLinkEnabled = this->GetResources()->GetBoolean(
                R.bool.config_cellBroadcastAppLinks);
        try {
            if (isCellBroadcastAppLinkEnabled) {
                if (mPm->GetApplicationEnabledSetting("com.android.cellbroadcastreceiver")
                        == PackageManager.COMPONENT_ENABLED_STATE_DISABLED) {
                    isCellBroadcastAppLinkEnabled = FALSE;  // CMAS app disabled
                }
            }
        } catch (IllegalArgumentException ignored) {
            isCellBroadcastAppLinkEnabled = FALSE;  // CMAS app not installed
        }
        if (isSecondaryUser || !isCellBroadcastAppLinkEnabled
                || mUm->HasUserRestriction(UserManager.DISALLOW_CONFIG_CELL_BROADCASTS)) {
            PreferenceScreen root = GetPreferenceScreen();
            Preference ps = FindPreference(KEY_CELL_BROADCAST_SETTINGS);
            if (ps != NULL) root->RemovePreference(ps);
        }
    }

    //@Override
    CARAPI OnStart() {
        super->OnStart();

        InitSmsApplicationSetting();
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();

        mAirplaneModeEnabler->Resume();
        if (mNfcEnabler != NULL) {
            mNfcEnabler->Resume();
        }
        if (mNsdEnabler != NULL) {
            mNsdEnabler->Resume();
        }
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle outState) {
        super->OnSaveInstanceState(outState);

        if (!TextUtils->IsEmpty(mManageMobilePlanMessage)) {
            outState->PutString(SAVED_MANAGE_MOBILE_PLAN_MSG, mManageMobilePlanMessage);
        }
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();

        mAirplaneModeEnabler->Pause();
        if (mNfcEnabler != NULL) {
            mNfcEnabler->Pause();
        }
        if (mNsdEnabler != NULL) {
            mNsdEnabler->Pause();
        }
    }

    //@Override
    CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        if (requestCode == REQUEST_CODE_EXIT_ECM) {
            Boolean isChoiceYes = data->GetBooleanExtra(EXIT_ECM_RESULT, FALSE);
            // Set Airplane mode based on the return value and checkbox state
            mAirplaneModeEnabler->SetAirplaneModeInECM(isChoiceYes,
                    mAirplaneModePreference->IsChecked());
        }
        super->OnActivityResult(requestCode, resultCode, data);
    }

    //@Override
    protected Int32 GetHelpResource() {
        return R::string::help_url_more_networks;
    }

    //@Override
    public Boolean OnPreferenceChange(Preference preference, Object newValue) {
        if (preference == mSmsApplicationPreference && newValue != NULL) {
            SmsApplication->SetDefaultApplication(newValue->ToString(), GetActivity());
            return TRUE;
        }
        return FALSE;
    }

    /**
     * For Search.
     */
    public static const Indexable.SearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
        new BaseSearchIndexProvider() {
            //@Override
            public List<SearchIndexableResource> GetXmlResourcesToIndex(
                    Context context, Boolean enabled) {
                SearchIndexableResource sir = new SearchIndexableResource(context);
                sir.xmlResId = R.xml.wireless_settings;
                return Arrays->AsList(sir);
            }

            //@Override
            public List<String> GetNonIndexableKeys(Context context) {
                final ArrayList<String> result = new ArrayList<String>();

                result->Add(KEY_TOGGLE_NSD);

                final UserManager um = (UserManager) context->GetSystemService(Context.USER_SERVICE);
                final Boolean isSecondaryUser = UserHandle->MyUserId() != UserHandle.USER_OWNER;
                final Boolean isWimaxEnabled = !isSecondaryUser && context->GetResources()->GetBoolean(
                        R.bool.config_wimaxEnabled);
                if (!isWimaxEnabled
                        || um->HasUserRestriction(UserManager.DISALLOW_CONFIG_MOBILE_NETWORKS)) {
                    result->Add(KEY_WIMAX_SETTINGS);
                }

                if (isSecondaryUser) { // Disable VPN
                    result->Add(KEY_VPN_SETTINGS);
                }

                // Remove NFC if not available
                final NfcManager manager = (NfcManager) context->GetSystemService(Context.NFC_SERVICE);
                if (manager != NULL) {
                    NfcAdapter adapter = manager->GetDefaultAdapter();
                    if (adapter == NULL) {
                        result->Add(KEY_TOGGLE_NFC);
                        result->Add(KEY_ANDROID_BEAM_SETTINGS);
                    }
                }

                // Remove Mobile Network Settings and Manage Mobile Plan if it's a wifi-only device.
                if (isSecondaryUser || Utils->IsWifiOnly(context)) {
                    result->Add(KEY_MOBILE_NETWORK_SETTINGS);
                    result->Add(KEY_MANAGE_MOBILE_PLAN);
                }

                // Remove Mobile Network Settings and Manage Mobile Plan
                // if config_show_mobile_plan sets FALSE.
                final Boolean isMobilePlanEnabled = context->GetResources()->GetBoolean(
                        R.bool.config_show_mobile_plan);
                if (!isMobilePlanEnabled) {
                    result->Add(KEY_MANAGE_MOBILE_PLAN);
                }

                // Remove SMS Application if the device does not support SMS
                TelephonyManager tm =
                        (TelephonyManager) context->GetSystemService(Context.TELEPHONY_SERVICE);
                if (!tm->IsSmsCapable()) {
                    result->Add(KEY_SMS_APPLICATION);
                }

                final PackageManager pm = context->GetPackageManager();

                // Remove Airplane Mode settings if it's a stationary device such as a TV.
                if (pm->HasSystemFeature(PackageManager.FEATURE_TELEVISION)) {
                    result->Add(KEY_TOGGLE_AIRPLANE);
                }

                // proxy UI disabled until we have better app support
                result->Add(KEY_PROXY_SETTINGS);

                // Disable Tethering if it's not allowed or if it's a wifi-only device
                ConnectivityManager cm =
                        (ConnectivityManager) context->GetSystemService(Context.CONNECTIVITY_SERVICE);
                if (isSecondaryUser || !cm->IsTetheringSupported()) {
                    result->Add(KEY_TETHER_SETTINGS);
                }

                // Enable link to CMAS app settings depending on the value in config.xml.
                Boolean isCellBroadcastAppLinkEnabled = context->GetResources()->GetBoolean(
                        R.bool.config_cellBroadcastAppLinks);
                try {
                    if (isCellBroadcastAppLinkEnabled) {
                        if (pm->GetApplicationEnabledSetting("com.android.cellbroadcastreceiver")
                                == PackageManager.COMPONENT_ENABLED_STATE_DISABLED) {
                            isCellBroadcastAppLinkEnabled = FALSE;  // CMAS app disabled
                        }
                    }
                } catch (IllegalArgumentException ignored) {
                    isCellBroadcastAppLinkEnabled = FALSE;  // CMAS app not installed
                }
                if (isSecondaryUser || !isCellBroadcastAppLinkEnabled) {
                    result->Add(KEY_CELL_BROADCAST_SETTINGS);
                }

                return result;
            }
        };
}
