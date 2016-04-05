/*
 * Copyright (C) 2007 The Android Open Source Project
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
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::Preference::IOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Security::IKeyStore;
using Elastos::Droid::Service::Trust::ITrustAgentService;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Settings::TrustAgentUtils::ITrustAgentComponentInfo;
using Elastos::Droid::Settings::Search::IBaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndex;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

using static android::Provider::Settings::System::ISCREEN_OFF_TIMEOUT;

/**
 * Gesture lock pattern settings.
 */
public class SecuritySettings extends SettingsPreferenceFragment
        implements OnPreferenceChangeListener, DialogInterface.OnClickListener, Indexable {
    private static const String TRUST_AGENT_CLICK_INTENT = "trust_agent_click_intent";
    static const String TAG = "SecuritySettings";
    private static const Intent TRUST_AGENT_INTENT =
            new Intent(TrustAgentService.SERVICE_INTERFACE);

    // Lock Settings
    private static const String KEY_UNLOCK_SET_OR_CHANGE = "unlock_set_or_change";
    private static const String KEY_BIOMETRIC_WEAK_IMPROVE_MATCHING =
            "biometric_weak_improve_matching";
    private static const String KEY_BIOMETRIC_WEAK_LIVELINESS = "biometric_weak_liveliness";
    private static const String KEY_LOCK_ENABLED = "lockenabled";
    private static const String KEY_VISIBLE_PATTERN = "visiblepattern";
    private static const String KEY_SECURITY_CATEGORY = "security_category";
    private static const String KEY_DEVICE_ADMIN_CATEGORY = "device_admin_category";
    private static const String KEY_LOCK_AFTER_TIMEOUT = "lock_after_timeout";
    private static const String KEY_OWNER_INFO_SETTINGS = "owner_info_settings";
    private static const String KEY_ADVANCED_SECURITY = "advanced_security";
    private static const String KEY_MANAGE_TRUST_AGENTS = "manage_trust_agents";

    private static const Int32 SET_OR_CHANGE_LOCK_METHOD_REQUEST = 123;
    private static const Int32 CONFIRM_EXISTING_FOR_BIOMETRIC_WEAK_IMPROVE_REQUEST = 124;
    private static const Int32 CONFIRM_EXISTING_FOR_BIOMETRIC_WEAK_LIVELINESS_OFF = 125;
    private static const Int32 CHANGE_TRUST_AGENT_SETTINGS = 126;

    // Misc Settings
    private static const String KEY_SIM_LOCK = "sim_lock";
    private static const String KEY_SHOW_PASSWORD = "show_password";
    private static const String KEY_CREDENTIAL_STORAGE_TYPE = "credential_storage_type";
    private static const String KEY_RESET_CREDENTIALS = "credentials_reset";
    private static const String KEY_CREDENTIALS_INSTALL = "credentials_install";
    private static const String KEY_TOGGLE_INSTALL_APPLICATIONS = "toggle_install_applications";
    private static const String KEY_POWER_INSTANTLY_LOCKS = "power_button_instantly_locks";
    private static const String KEY_CREDENTIALS_MANAGER = "credentials_management";
    private static const String PACKAGE_MIME_TYPE = "application/vnd.android.package-archive";
    private static const String KEY_TRUST_AGENT = "trust_agent";
    private static const String KEY_SCREEN_PINNING = "screen_pinning_settings";

    // These switch preferences need special handling since they're not all stored in Settings.
    private static const String SWITCH_PREFERENCE_KEYS[] = { KEY_LOCK_AFTER_TIMEOUT,
            KEY_LOCK_ENABLED, KEY_VISIBLE_PATTERN, KEY_BIOMETRIC_WEAK_LIVELINESS,
            KEY_POWER_INSTANTLY_LOCKS, KEY_SHOW_PASSWORD, KEY_TOGGLE_INSTALL_APPLICATIONS };

    // Only allow one trust agent on the platform.
    private static const Boolean ONLY_ONE_TRUST_AGENT = TRUE;

    private DevicePolicyManager mDPM;

    private ChooseLockSettingsHelper mChooseLockSettingsHelper;
    private LockPatternUtils mLockPatternUtils;
    private ListPreference mLockAfter;

    private SwitchPreference mBiometricWeakLiveliness;
    private SwitchPreference mVisiblePattern;

    private SwitchPreference mShowPassword;

    private KeyStore mKeyStore;
    private Preference mResetCredentials;

    private SwitchPreference mToggleAppInstallation;
    private DialogInterface mWarnInstallApps;
    private SwitchPreference mPowerButtonInstantlyLocks;

    private Boolean mIsPrimary;

    private Intent mTrustAgentClickIntent;

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        mLockPatternUtils = new LockPatternUtils(GetActivity());

        mDPM = (DevicePolicyManager)GetSystemService(Context.DEVICE_POLICY_SERVICE);

        mChooseLockSettingsHelper = new ChooseLockSettingsHelper(GetActivity());

        if (savedInstanceState != NULL
                && savedInstanceState->ContainsKey(TRUST_AGENT_CLICK_INTENT)) {
            mTrustAgentClickIntent = savedInstanceState->GetParcelable(TRUST_AGENT_CLICK_INTENT);
        }
    }

    private static Int32 GetResIdForLockUnlockScreen(Context context,
            LockPatternUtils lockPatternUtils) {
        Int32 resid = 0;
        if (!lockPatternUtils->IsSecure()) {
            // if there are multiple users, disable "None" setting
            UserManager mUm = (UserManager) context. GetSystemService(Context.USER_SERVICE);
            List<UserInfo> users = mUm->GetUsers(TRUE);
            final Boolean singleUser = users->Size() == 1;

            if (singleUser && lockPatternUtils->IsLockScreenDisabled()) {
                resid = R.xml.security_settings_lockscreen;
            } else {
                resid = R.xml.security_settings_chooser;
            }
        } else if (lockPatternUtils->UsingBiometricWeak() &&
                lockPatternUtils->IsBiometricWeakInstalled()) {
            resid = R.xml.security_settings_biometric_weak;
        } else {
            switch (lockPatternUtils->GetKeyguardStoredPasswordQuality()) {
                case DevicePolicyManager.PASSWORD_QUALITY_SOMETHING:
                    resid = R.xml.security_settings_pattern;
                    break;
                case DevicePolicyManager.PASSWORD_QUALITY_NUMERIC:
                case DevicePolicyManager.PASSWORD_QUALITY_NUMERIC_COMPLEX:
                    resid = R.xml.security_settings_pin;
                    break;
                case DevicePolicyManager.PASSWORD_QUALITY_ALPHABETIC:
                case DevicePolicyManager.PASSWORD_QUALITY_ALPHANUMERIC:
                case DevicePolicyManager.PASSWORD_QUALITY_COMPLEX:
                    resid = R.xml.security_settings_password;
                    break;
            }
        }
        return resid;
    }

    /**
     * Important!
     *
     * Don't forget to update the SecuritySearchIndexProvider if you are doing any change in the
     * logic or adding/removing preferences here.
     */
    private PreferenceScreen CreatePreferenceHierarchy() {
        PreferenceScreen root = GetPreferenceScreen();
        if (root != NULL) {
            root->RemoveAll();
        }
        AddPreferencesFromResource(R.xml.security_settings);
        root = GetPreferenceScreen();

        // Add options for lock/unlock screen
        final Int32 resid = GetResIdForLockUnlockScreen(GetActivity(), mLockPatternUtils);
        AddPreferencesFromResource(resid);

        // Add options for device encryption
        mIsPrimary = UserHandle->MyUserId() == UserHandle.USER_OWNER;

        if (!mIsPrimary) {
            // Rename owner info settings
            Preference ownerInfoPref = FindPreference(KEY_OWNER_INFO_SETTINGS);
            if (ownerInfoPref != NULL) {
                if (UserManager->Get(GetActivity()).IsLinkedUser()) {
                    ownerInfoPref->SetTitle(R::string::profile_info_settings_title);
                } else {
                    ownerInfoPref->SetTitle(R::string::user_info_settings_title);
                }
            }
        }

        if (mIsPrimary) {
            if (LockPatternUtils->IsDeviceEncryptionEnabled()) {
                // The device is currently encrypted.
                AddPreferencesFromResource(R.xml.security_settings_encrypted);
            } else {
                // This device supports encryption but isn't encrypted.
                AddPreferencesFromResource(R.xml.security_settings_unencrypted);
            }
        }

        // Trust Agent preferences
        PreferenceGroup securityCategory = (PreferenceGroup)
                root->FindPreference(KEY_SECURITY_CATEGORY);
        if (securityCategory != NULL) {
            final Boolean hasSecurity = mLockPatternUtils->IsSecure();
            ArrayList<TrustAgentComponentInfo> agents =
                    GetActiveTrustAgents(GetPackageManager(), mLockPatternUtils);
            for (Int32 i = 0; i < agents->Size(); i++) {
                final TrustAgentComponentInfo agent = agents->Get(i);
                Preference trustAgentPreference =
                        new Preference(securityCategory->GetContext());
                trustAgentPreference->SetKey(KEY_TRUST_AGENT);
                trustAgentPreference->SetTitle(agent.title);
                trustAgentPreference->SetSummary(agent.summary);
                // Create intent for this preference.
                Intent intent = new Intent();
                intent->SetComponent(agent.componentName);
                intent->SetAction(IIntent::ACTION_MAIN);
                trustAgentPreference->SetIntent(intent);
                // Add preference to the settings menu.
                securityCategory->AddPreference(trustAgentPreference);
                if (!hasSecurity) {
                    trustAgentPreference->SetEnabled(FALSE);
                    trustAgentPreference->SetSummary(R::string::disabled_because_no_backup_security);
                }
            }
        }

        // lock after preference
        mLockAfter = (ListPreference) root->FindPreference(KEY_LOCK_AFTER_TIMEOUT);
        if (mLockAfter != NULL) {
            SetupLockAfterPreference();
            UpdateLockAfterPreferenceSummary();
        }

        // biometric weak liveliness
        mBiometricWeakLiveliness =
                (SwitchPreference) root->FindPreference(KEY_BIOMETRIC_WEAK_LIVELINESS);

        // visible pattern
        mVisiblePattern = (SwitchPreference) root->FindPreference(KEY_VISIBLE_PATTERN);

        // lock instantly on power key press
        mPowerButtonInstantlyLocks = (SwitchPreference) root->FindPreference(
                KEY_POWER_INSTANTLY_LOCKS);
        Preference trustAgentPreference = root->FindPreference(KEY_TRUST_AGENT);
        if (mPowerButtonInstantlyLocks != NULL &&
                trustAgentPreference != NULL &&
                trustAgentPreference->GetTitle()->Length() > 0) {
            mPowerButtonInstantlyLocks->SetSummary(GetString(
                    R::string::lockpattern_settings_power_button_instantly_locks_summary,
                    trustAgentPreference->GetTitle()));
        }

        // don't display visible pattern if biometric and backup is not pattern
        if (resid == R.xml.security_settings_biometric_weak &&
                mLockPatternUtils->GetKeyguardStoredPasswordQuality() !=
                DevicePolicyManager.PASSWORD_QUALITY_SOMETHING) {
            if (securityCategory != NULL && mVisiblePattern != NULL) {
                securityCategory->RemovePreference(root->FindPreference(KEY_VISIBLE_PATTERN));
            }
        }

        // Append the rest of the settings
        AddPreferencesFromResource(R.xml.security_settings_misc);

        // Do not display SIM lock for devices without an Icc card
        TelephonyManager tm = TelephonyManager->GetDefault();
        if (!mIsPrimary || !tm->HasIccCard()) {
            root->RemovePreference(root->FindPreference(KEY_SIM_LOCK));
        } else {
            // Disable SIM lock if sim card is missing or unknown
            if ((TelephonyManager->GetDefault()->GetSimState() ==
                                 TelephonyManager.SIM_STATE_ABSENT) ||
                (TelephonyManager->GetDefault()->GetSimState() ==
                                 TelephonyManager.SIM_STATE_UNKNOWN)) {
                root->FindPreference(KEY_SIM_LOCK).SetEnabled(FALSE);
            }
        }
        if (Settings.System->GetInt(GetContentResolver(),
                Settings.System.LOCK_TO_APP_ENABLED, 0) != 0) {
            root->FindPreference(KEY_SCREEN_PINNING).SetSummary(
                    GetResources()->GetString(R::string::switch_on_text));
        }

        // Show password
        mShowPassword = (SwitchPreference) root->FindPreference(KEY_SHOW_PASSWORD);
        mResetCredentials = root->FindPreference(KEY_RESET_CREDENTIALS);

        // Credential storage
        final UserManager um = (UserManager) GetActivity()->GetSystemService(Context.USER_SERVICE);
        mKeyStore = KeyStore->GetInstance(); // needs to be initialized for OnResume()
        if (!um->HasUserRestriction(UserManager.DISALLOW_CONFIG_CREDENTIALS)) {
            Preference credentialStorageType = root->FindPreference(KEY_CREDENTIAL_STORAGE_TYPE);

            final Int32 storageSummaryRes =
                mKeyStore->IsHardwareBacked() ? R::string::credential_storage_type_hardware
                        : R::string::credential_storage_type_software;
            credentialStorageType->SetSummary(storageSummaryRes);
        } else {
            PreferenceGroup credentialsManager = (PreferenceGroup)
                    root->FindPreference(KEY_CREDENTIALS_MANAGER);
            credentialsManager->RemovePreference(root->FindPreference(KEY_RESET_CREDENTIALS));
            credentialsManager->RemovePreference(root->FindPreference(KEY_CREDENTIALS_INSTALL));
            credentialsManager->RemovePreference(root->FindPreference(KEY_CREDENTIAL_STORAGE_TYPE));
        }

        // Application install
        PreferenceGroup deviceAdminCategory = (PreferenceGroup)
                root->FindPreference(KEY_DEVICE_ADMIN_CATEGORY);
        mToggleAppInstallation = (SwitchPreference) FindPreference(
                KEY_TOGGLE_INSTALL_APPLICATIONS);
        mToggleAppInstallation->SetChecked(IsNonMarketAppsAllowed());
        // Side loading of apps.
        mToggleAppInstallation->SetEnabled(mIsPrimary);
        if (um->HasUserRestriction(UserManager.DISALLOW_INSTALL_UNKNOWN_SOURCES)
                || um->HasUserRestriction(UserManager.DISALLOW_INSTALL_APPS)) {
            mToggleAppInstallation->SetEnabled(FALSE);
        }

        // Advanced Security features
        PreferenceGroup advancedCategory =
                (PreferenceGroup)root->FindPreference(KEY_ADVANCED_SECURITY);
        if (advancedCategory != NULL) {
            Preference manageAgents = advancedCategory->FindPreference(KEY_MANAGE_TRUST_AGENTS);
            if (manageAgents != NULL && !mLockPatternUtils->IsSecure()) {
                manageAgents->SetEnabled(FALSE);
                manageAgents->SetSummary(R::string::disabled_because_no_backup_security);
            }
        }

        // The above preferences come and go based on security state, so we need to update
        // the index. This call is expected to be fairly cheap, but we may want to do something
        // smarter in the future.
        Index->GetInstance(GetActivity())
                .UpdateFromClassNameResource(SecuritySettings.class->GetName(), TRUE, TRUE);

        for (Int32 i = 0; i < SWITCH_PREFERENCE_KEYS.length; i++) {
            final Preference pref = FindPreference(SWITCH_PREFERENCE_KEYS[i]);
            if (pref != NULL) pref->SetOnPreferenceChangeListener(this);
        }
        return root;
    }

    private static ArrayList<TrustAgentComponentInfo> GetActiveTrustAgents(
            PackageManager pm, LockPatternUtils utils) {
        ArrayList<TrustAgentComponentInfo> result = new ArrayList<TrustAgentComponentInfo>();
        List<ResolveInfo> resolveInfos = pm->QueryIntentServices(TRUST_AGENT_INTENT,
                PackageManager.GET_META_DATA);
        List<ComponentName> enabledTrustAgents = utils->GetEnabledTrustAgents();
        if (enabledTrustAgents != NULL && !enabledTrustAgents->IsEmpty()) {
            for (Int32 i = 0; i < resolveInfos->Size(); i++) {
                ResolveInfo resolveInfo = resolveInfos->Get(i);
                if (resolveInfo.serviceInfo == NULL) continue;
                if (!TrustAgentUtils->CheckProvidePermission(resolveInfo, pm)) continue;
                TrustAgentComponentInfo trustAgentComponentInfo =
                        TrustAgentUtils->GetSettingsComponent(pm, resolveInfo);
                if (trustAgentComponentInfo.componentName == NULL ||
                        !enabledTrustAgents->Contains(
                                TrustAgentUtils->GetComponentName(resolveInfo)) ||
                        TextUtils->IsEmpty(trustAgentComponentInfo.title)) continue;
                result->Add(trustAgentComponentInfo);
                if (ONLY_ONE_TRUST_AGENT) break;
            }
        }
        return result;
    }

    private Boolean IsNonMarketAppsAllowed() {
        return Settings::Global::>GetInt(GetContentResolver(),
                                      Settings::Global::INSTALL_NON_MARKET_APPS, 0) > 0;
    }

    private void SetNonMarketAppsAllowed(Boolean enabled) {
        final UserManager um = (UserManager) GetActivity()->GetSystemService(Context.USER_SERVICE);
        if (um->HasUserRestriction(UserManager.DISALLOW_INSTALL_UNKNOWN_SOURCES)) {
            return;
        }
        // Change the system setting
        Settings::Global::>PutInt(GetContentResolver(), Settings::Global::INSTALL_NON_MARKET_APPS,
                                enabled ? 1 : 0);
    }

    private void WarnAppInstallation() {
        // TODO: DialogFragment?
        mWarnInstallApps = new AlertDialog->Builder(GetActivity()).SetTitle(
                GetResources()->GetString(R::string::error_title))
                .SetIcon(R.drawable.ic_dialog_alert)
                .SetMessage(GetResources()->GetString(R::string::install_all_warning))
                .SetPositiveButton(android.R::string::yes, this)
                .SetNegativeButton(android.R::string::no, this)
                .Show();
    }

    //@Override
    CARAPI OnClick(DialogInterface dialog, Int32 which) {
        if (dialog == mWarnInstallApps) {
            Boolean turnOn = which == DialogInterface.BUTTON_POSITIVE;
            SetNonMarketAppsAllowed(turnOn);
            if (mToggleAppInstallation != NULL) {
                mToggleAppInstallation->SetChecked(turnOn);
            }
        }
    }

    //@Override
    CARAPI OnDestroy() {
        super->OnDestroy();
        if (mWarnInstallApps != NULL) {
            mWarnInstallApps->Dismiss();
        }
    }

    private void SetupLockAfterPreference() {
        // Compatible with pre-Froyo
        Int64 currentTimeout = Settings.Secure->GetLong(GetContentResolver(),
                Settings.Secure.LOCK_SCREEN_LOCK_AFTER_TIMEOUT, 5000);
        mLockAfter->SetValue(String->ValueOf(currentTimeout));
        mLockAfter->SetOnPreferenceChangeListener(this);
        final Int64 adminTimeout = (mDPM != NULL ? mDPM->GetMaximumTimeToLock(NULL) : 0);
        final Int64 displayTimeout = Math->Max(0,
                Settings.System->GetInt(GetContentResolver(), SCREEN_OFF_TIMEOUT, 0));
        if (adminTimeout > 0) {
            // This setting is a slave to display timeout when a device policy is enforced.
            // As such, maxLockTimeout = adminTimeout - displayTimeout.
            // If there isn't enough time, shows "immediately" setting.
            DisableUnusableTimeouts(Math->Max(0, adminTimeout - displayTimeout));
        }
    }

    private void UpdateLockAfterPreferenceSummary() {
        // Update summary message with current value
        Int64 currentTimeout = Settings.Secure->GetLong(GetContentResolver(),
                Settings.Secure.LOCK_SCREEN_LOCK_AFTER_TIMEOUT, 5000);
        final CharSequence[] entries = mLockAfter->GetEntries();
        final CharSequence[] values = mLockAfter->GetEntryValues();
        Int32 best = 0;
        for (Int32 i = 0; i < values.length; i++) {
            Int64 timeout = Long->ValueOf(values[i].ToString());
            if (currentTimeout >= timeout) {
                best = i;
            }
        }

        Preference preference = GetPreferenceScreen()->FindPreference(KEY_TRUST_AGENT);
        if (preference != NULL && preference->GetTitle()->Length() > 0) {
            mLockAfter->SetSummary(GetString(R::string::lock_after_timeout_summary_with_exception,
                    entries[best], preference->GetTitle()));
        } else {
            mLockAfter->SetSummary(GetString(R::string::lock_after_timeout_summary, entries[best]));
        }
    }

    private void DisableUnusableTimeouts(Int64 maxTimeout) {
        final CharSequence[] entries = mLockAfter->GetEntries();
        final CharSequence[] values = mLockAfter->GetEntryValues();
        ArrayList<CharSequence> revisedEntries = new ArrayList<CharSequence>();
        ArrayList<CharSequence> revisedValues = new ArrayList<CharSequence>();
        for (Int32 i = 0; i < values.length; i++) {
            Int64 timeout = Long->ValueOf(values[i].ToString());
            if (timeout <= maxTimeout) {
                revisedEntries->Add(entries[i]);
                revisedValues->Add(values[i]);
            }
        }
        if (revisedEntries->Size() != entries.length || revisedValues->Size() != values.length) {
            mLockAfter->SetEntries(
                    revisedEntries->ToArray(new CharSequence[revisedEntries->Size()]));
            mLockAfter->SetEntryValues(
                    revisedValues->ToArray(new CharSequence[revisedValues->Size()]));
            final Int32 userPreference = Integer->ValueOf(mLockAfter->GetValue());
            if (userPreference <= maxTimeout) {
                mLockAfter->SetValue(String->ValueOf(userPreference));
            } else {
                // There will be no highlighted selection since nothing in the list matches
                // maxTimeout. The user can still select anything less than maxTimeout.
                // TODO: maybe append maxTimeout to the list and mark selected.
            }
        }
        mLockAfter->SetEnabled(revisedEntries->Size() > 0);
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle outState) {
        super->OnSaveInstanceState(outState);
        if (mTrustAgentClickIntent != NULL) {
            outState->PutParcelable(TRUST_AGENT_CLICK_INTENT, mTrustAgentClickIntent);
        }
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();

        // Make sure we reload the preference hierarchy since some of these settings
        // depend on others...
        CreatePreferenceHierarchy();

        final LockPatternUtils lockPatternUtils = mChooseLockSettingsHelper->Utils();
        if (mBiometricWeakLiveliness != NULL) {
            mBiometricWeakLiveliness->SetChecked(
                    lockPatternUtils->IsBiometricWeakLivelinessEnabled());
        }
        if (mVisiblePattern != NULL) {
            mVisiblePattern->SetChecked(lockPatternUtils->IsVisiblePatternEnabled());
        }
        if (mPowerButtonInstantlyLocks != NULL) {
            mPowerButtonInstantlyLocks->SetChecked(lockPatternUtils->GetPowerButtonInstantlyLocks());
        }

        if (mShowPassword != NULL) {
            mShowPassword->SetChecked(Settings.System->GetInt(GetContentResolver(),
                    Settings.System.TEXT_SHOW_PASSWORD, 1) != 0);
        }

        if (mResetCredentials != NULL) {
            mResetCredentials->SetEnabled(!mKeyStore->IsEmpty());
        }
    }

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
        final String key = preference->GetKey();
        if (KEY_UNLOCK_SET_OR_CHANGE->Equals(key)) {
            StartFragment(this, "com.android.settings.ChooseLockGeneric$ChooseLockGenericFragment",
                    R::string::lock_settings_picker_title, SET_OR_CHANGE_LOCK_METHOD_REQUEST, NULL);
        } else if (KEY_BIOMETRIC_WEAK_IMPROVE_MATCHING->Equals(key)) {
            ChooseLockSettingsHelper helper =
                    new ChooseLockSettingsHelper(this->GetActivity(), this);
            if (!helper->LaunchConfirmationActivity(
                    CONFIRM_EXISTING_FOR_BIOMETRIC_WEAK_IMPROVE_REQUEST, NULL, NULL)) {
                // If this returns FALSE, it means no password confirmation is required, so
                // go ahead and start improve.
                // Note: currently a backup is required for biometric_weak so this code path
                // can't be reached, but is here in case things change in the future
                StartBiometricWeakImprove();
            }
        } else if (KEY_TRUST_AGENT->Equals(key)) {
            ChooseLockSettingsHelper helper =
                    new ChooseLockSettingsHelper(this->GetActivity(), this);
            mTrustAgentClickIntent = preference->GetIntent();
            if (!helper->LaunchConfirmationActivity(CHANGE_TRUST_AGENT_SETTINGS, NULL, NULL) &&
                    mTrustAgentClickIntent != NULL) {
                // If this returns FALSE, it means no password confirmation is required.
                StartActivity(mTrustAgentClickIntent);
                mTrustAgentClickIntent = NULL;
            }
        } else {
            // If we didn't handle it, let preferences handle it.
            return super->OnPreferenceTreeClick(preferenceScreen, preference);
        }
        return TRUE;
    }

    /**
     * see confirmPatternThenDisableAndClear
     */
    //@Override
    CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        super->OnActivityResult(requestCode, resultCode, data);
        if (requestCode == CONFIRM_EXISTING_FOR_BIOMETRIC_WEAK_IMPROVE_REQUEST &&
                resultCode == Activity.RESULT_OK) {
            StartBiometricWeakImprove();
            return;
        } else if (requestCode == CONFIRM_EXISTING_FOR_BIOMETRIC_WEAK_LIVELINESS_OFF &&
                resultCode == Activity.RESULT_OK) {
            final LockPatternUtils lockPatternUtils = mChooseLockSettingsHelper->Utils();
            lockPatternUtils->SetBiometricWeakLivelinessEnabled(FALSE);
            // Setting the mBiometricWeakLiveliness checked value to FALSE is handled when onResume
            // is called by grabbing the value from lockPatternUtils.  We can't set it here
            // because mBiometricWeakLiveliness could be NULL
            return;
        } else if (requestCode == CHANGE_TRUST_AGENT_SETTINGS && resultCode == Activity.RESULT_OK) {
            if (mTrustAgentClickIntent != NULL) {
                StartActivity(mTrustAgentClickIntent);
                mTrustAgentClickIntent = NULL;
            }
            return;
        }
        CreatePreferenceHierarchy();
    }

    //@Override
    public Boolean OnPreferenceChange(Preference preference, Object value) {
        Boolean result = TRUE;
        final String key = preference->GetKey();
        final LockPatternUtils lockPatternUtils = mChooseLockSettingsHelper->Utils();
        if (KEY_LOCK_AFTER_TIMEOUT->Equals(key)) {
            Int32 timeout = Integer->ParseInt((String) value);
            try {
                Settings.Secure->PutInt(GetContentResolver(),
                        Settings.Secure.LOCK_SCREEN_LOCK_AFTER_TIMEOUT, timeout);
            } catch (NumberFormatException e) {
                Logger::E("SecuritySettings", "could not persist lockAfter timeout setting", e);
            }
            UpdateLockAfterPreferenceSummary();
        } else if (KEY_LOCK_ENABLED->Equals(key)) {
            lockPatternUtils->SetLockPatternEnabled((Boolean) value);
        } else if (KEY_VISIBLE_PATTERN->Equals(key)) {
            lockPatternUtils->SetVisiblePatternEnabled((Boolean) value);
        } else  if (KEY_BIOMETRIC_WEAK_LIVELINESS->Equals(key)) {
            if ((Boolean) value) {
                lockPatternUtils->SetBiometricWeakLivelinessEnabled(TRUE);
            } else {
                // In this case the user has just unchecked the checkbox, but this action requires
                // them to confirm their password.  We need to re-check the checkbox until
                // they've confirmed their password
                mBiometricWeakLiveliness->SetChecked(TRUE);
                ChooseLockSettingsHelper helper =
                        new ChooseLockSettingsHelper(this->GetActivity(), this);
                if (!helper->LaunchConfirmationActivity(
                                CONFIRM_EXISTING_FOR_BIOMETRIC_WEAK_LIVELINESS_OFF, NULL, NULL)) {
                    // If this returns FALSE, it means no password confirmation is required, so
                    // go ahead and uncheck it here.
                    // Note: currently a backup is required for biometric_weak so this code path
                    // can't be reached, but is here in case things change in the future
                    lockPatternUtils->SetBiometricWeakLivelinessEnabled(FALSE);
                    mBiometricWeakLiveliness->SetChecked(FALSE);
                }
            }
        } else if (KEY_POWER_INSTANTLY_LOCKS->Equals(key)) {
            mLockPatternUtils->SetPowerButtonInstantlyLocks((Boolean) value);
        } else if (KEY_SHOW_PASSWORD->Equals(key)) {
            Settings.System->PutInt(GetContentResolver(), Settings.System.TEXT_SHOW_PASSWORD,
                    ((Boolean) value) ? 1 : 0);
        } else if (KEY_TOGGLE_INSTALL_APPLICATIONS->Equals(key)) {
            if ((Boolean) value) {
                mToggleAppInstallation->SetChecked(FALSE);
                WarnAppInstallation();
                // Don't change Switch status until user makes choice in dialog, so return FALSE.
                result = FALSE;
            } else {
                SetNonMarketAppsAllowed(FALSE);
            }
        }
        return result;
    }

    //@Override
    protected Int32 GetHelpResource() {
        return R::string::help_url_security;
    }

    CARAPI StartBiometricWeakImprove(){
        Intent intent = new Intent();
        intent->SetClassName("com.android.facelock", "com.android.facelock.AddToSetup");
        StartActivity(intent);
    }

    /**
     * For Search. Please keep it in sync when updating "CreatePreferenceHierarchy()"
     */
    public static const SearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
            new SecuritySearchIndexProvider();

    private static class SecuritySearchIndexProvider extends BaseSearchIndexProvider {

        Boolean mIsPrimary;

        public SecuritySearchIndexProvider() {
            Super();

            mIsPrimary = UserHandle->MyUserId() == UserHandle.USER_OWNER;
        }

        //@Override
        public List<SearchIndexableResource> GetXmlResourcesToIndex(
                Context context, Boolean enabled) {

            List<SearchIndexableResource> result = new ArrayList<SearchIndexableResource>();

            LockPatternUtils lockPatternUtils = new LockPatternUtils(context);
            // Add options for lock/unlock screen
            Int32 resId = GetResIdForLockUnlockScreen(context, lockPatternUtils);

            SearchIndexableResource sir = new SearchIndexableResource(context);
            sir.xmlResId = resId;
            result->Add(sir);

            if (mIsPrimary) {
                DevicePolicyManager dpm = (DevicePolicyManager)
                        context->GetSystemService(Context.DEVICE_POLICY_SERVICE);

                switch (dpm->GetStorageEncryptionStatus()) {
                    case DevicePolicyManager.ENCRYPTION_STATUS_ACTIVE:
                        // The device is currently encrypted.
                        resId = R.xml.security_settings_encrypted;
                        break;
                    case DevicePolicyManager.ENCRYPTION_STATUS_INACTIVE:
                        // This device supports encryption but isn't encrypted.
                        resId = R.xml.security_settings_unencrypted;
                        break;
                }

                sir = new SearchIndexableResource(context);
                sir.xmlResId = resId;
                result->Add(sir);
            }

            // Append the rest of the settings
            sir = new SearchIndexableResource(context);
            sir.xmlResId = R.xml.security_settings_misc;
            result->Add(sir);

            return result;
        }

        //@Override
        public List<SearchIndexableRaw> GetRawDataToIndex(Context context, Boolean enabled) {
            final List<SearchIndexableRaw> result = new ArrayList<SearchIndexableRaw>();
            final Resources res = context->GetResources();

            final String screenTitle = res->GetString(R::string::security_settings_title);

            SearchIndexableRaw data = new SearchIndexableRaw(context);
            data.title = screenTitle;
            data.screenTitle = screenTitle;
            result->Add(data);

            if (!mIsPrimary) {
                Int32 resId = (UserManager->Get(context).IsLinkedUser()) ?
                        R::string::profile_info_settings_title : R::string::user_info_settings_title;

                data = new SearchIndexableRaw(context);
                data.title = res->GetString(resId);
                data.screenTitle = screenTitle;
                result->Add(data);
            }

            // Credential storage
            final UserManager um = (UserManager) context->GetSystemService(Context.USER_SERVICE);

            if (!um->HasUserRestriction(UserManager.DISALLOW_CONFIG_CREDENTIALS)) {
                KeyStore keyStore = KeyStore->GetInstance();

                final Int32 storageSummaryRes = keyStore->IsHardwareBacked() ?
                        R::string::credential_storage_type_hardware :
                        R::string::credential_storage_type_software;

                data = new SearchIndexableRaw(context);
                data.title = res->GetString(storageSummaryRes);
                data.screenTitle = screenTitle;
                result->Add(data);
            }

            // Advanced
            final LockPatternUtils lockPatternUtils = new LockPatternUtils(context);
            if (lockPatternUtils->IsSecure()) {
                ArrayList<TrustAgentComponentInfo> agents =
                        GetActiveTrustAgents(context->GetPackageManager(), lockPatternUtils);
                for (Int32 i = 0; i < agents->Size(); i++) {
                    final TrustAgentComponentInfo agent = agents->Get(i);
                    data = new SearchIndexableRaw(context);
                    data.title = agent.title;
                    data.screenTitle = screenTitle;
                    result->Add(data);
                }
            }
            return result;
        }

        //@Override
        public List<String> GetNonIndexableKeys(Context context) {
            final List<String> keys = new ArrayList<String>();

            LockPatternUtils lockPatternUtils = new LockPatternUtils(context);
            // Add options for lock/unlock screen
            Int32 resId = GetResIdForLockUnlockScreen(context, lockPatternUtils);

            // don't display visible pattern if biometric and backup is not pattern
            if (resId == R.xml.security_settings_biometric_weak &&
                    lockPatternUtils->GetKeyguardStoredPasswordQuality() !=
                            DevicePolicyManager.PASSWORD_QUALITY_SOMETHING) {
                keys->Add(KEY_VISIBLE_PATTERN);
            }

            // Do not display SIM lock for devices without an Icc card
            TelephonyManager tm = TelephonyManager->GetDefault();
            if (!mIsPrimary || !tm->HasIccCard()) {
                keys->Add(KEY_SIM_LOCK);
            }

            final UserManager um = (UserManager) context->GetSystemService(Context.USER_SERVICE);
            if (um->HasUserRestriction(UserManager.DISALLOW_CONFIG_CREDENTIALS)) {
                keys->Add(KEY_CREDENTIALS_MANAGER);
            }

            // TrustAgent settings disappear when the user has no primary security.
            if (!lockPatternUtils->IsSecure()) {
                keys->Add(KEY_TRUST_AGENT);
                keys->Add(KEY_MANAGE_TRUST_AGENTS);
            }

            return keys;
        }
    }

}
