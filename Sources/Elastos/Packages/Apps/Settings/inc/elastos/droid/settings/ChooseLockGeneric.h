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

using Elastos::Droid::Accessibilityservice::IAccessibilityServiceInfo;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManagerNative;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Security::IKeyStore;
using Elastos::Droid::Utility::IEventLog;
using Elastos::Droid::Utility::IMutableBoolean;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::Widget::IListView;

using Elastos::Droid::Internal::Widget::ILockPatternUtils;

using Elastos::Utility::IList;

public class ChooseLockGeneric extends SettingsActivity {
    public static const String CONFIRM_CREDENTIALS = "confirm_credentials";

    //@Override
    public Intent GetIntent() {
        Intent modIntent = new Intent(super->GetIntent());
        modIntent->PutExtra(EXTRA_SHOW_FRAGMENT, ChooseLockGenericFragment.class->GetName());
        return modIntent;
    }

    //@Override
    protected Boolean IsValidFragment(String fragmentName) {
        if (ChooseLockGenericFragment.class->GetName()->Equals(fragmentName)) return TRUE;
        return FALSE;
    }

    public static class InternalActivity extends ChooseLockGeneric {
    }

    public static class ChooseLockGenericFragment extends SettingsPreferenceFragment {
        private static const Int32 MIN_PASSWORD_LENGTH = 4;
        private static const String KEY_UNLOCK_BACKUP_INFO = "unlock_backup_info";
        private static const String KEY_UNLOCK_SET_OFF = "unlock_set_off";
        private static const String KEY_UNLOCK_SET_NONE = "unlock_set_none";
        private static const String KEY_UNLOCK_SET_BIOMETRIC_WEAK = "unlock_set_biometric_weak";
        private static const String KEY_UNLOCK_SET_PIN = "unlock_set_pin";
        private static const String KEY_UNLOCK_SET_PASSWORD = "unlock_set_password";
        private static const String KEY_UNLOCK_SET_PATTERN = "unlock_set_pattern";
        private static const Int32 CONFIRM_EXISTING_REQUEST = 100;
        private static const Int32 FALLBACK_REQUEST = 101;
        private static const Int32 ENABLE_ENCRYPTION_REQUEST = 102;
        private static const String PASSWORD_CONFIRMED = "password_confirmed";

        private static const String WAITING_FOR_CONFIRMATION = "waiting_for_confirmation";
        private static const String FINISH_PENDING = "finish_pending";
        public static const String MINIMUM_QUALITY_KEY = "minimum_quality";
        public static const String ENCRYPT_REQUESTED_QUALITY = "encrypt_requested_quality";
        public static const String ENCRYPT_REQUESTED_DISABLED = "encrypt_requested_disabled";

        private static const Boolean ALWAY_SHOW_TUTORIAL = TRUE;

        private ChooseLockSettingsHelper mChooseLockSettingsHelper;
        private DevicePolicyManager mDPM;
        private KeyStore mKeyStore;
        private Boolean mPasswordConfirmed = FALSE;
        private Boolean mWaitingForConfirmation = FALSE;
        private Boolean mFinishPending = FALSE;
        private Int32 mEncryptionRequestQuality;
        private Boolean mEncryptionRequestDisabled;
        private Boolean mRequirePassword;
        private LockPatternUtils mLockPatternUtils;

        //@Override
        CARAPI OnCreate(Bundle savedInstanceState) {
            super->OnCreate(savedInstanceState);

            mDPM = (DevicePolicyManager) GetSystemService(Context.DEVICE_POLICY_SERVICE);
            mKeyStore = KeyStore->GetInstance();
            mChooseLockSettingsHelper = new ChooseLockSettingsHelper(this->GetActivity());
            mLockPatternUtils = new LockPatternUtils(GetActivity());

            // Defaults to needing to confirm credentials
            final Boolean confirmCredentials = GetActivity()->GetIntent()
                .GetBooleanExtra(CONFIRM_CREDENTIALS, TRUE);
            if (GetActivity() instanceof ChooseLockGeneric.InternalActivity) {
                mPasswordConfirmed = !confirmCredentials;
            }

            if (savedInstanceState != NULL) {
                mPasswordConfirmed = savedInstanceState->GetBoolean(PASSWORD_CONFIRMED);
                mWaitingForConfirmation = savedInstanceState->GetBoolean(WAITING_FOR_CONFIRMATION);
                mFinishPending = savedInstanceState->GetBoolean(FINISH_PENDING);
                mEncryptionRequestQuality = savedInstanceState->GetInt(ENCRYPT_REQUESTED_QUALITY);
                mEncryptionRequestDisabled = savedInstanceState->GetBoolean(
                        ENCRYPT_REQUESTED_DISABLED);
            }

            if (mPasswordConfirmed) {
                UpdatePreferencesOrFinish();
            } else if (!mWaitingForConfirmation) {
                ChooseLockSettingsHelper helper =
                        new ChooseLockSettingsHelper(this->GetActivity(), this);
                if (!helper->LaunchConfirmationActivity(CONFIRM_EXISTING_REQUEST, NULL, NULL)) {
                    mPasswordConfirmed = TRUE; // no password set, so no need to confirm
                    UpdatePreferencesOrFinish();
                } else {
                    mWaitingForConfirmation = TRUE;
                }
            }
        }

        //@Override
        CARAPI OnResume() {
            super->OnResume();
            if (mFinishPending) {
                mFinishPending = FALSE;
                Finish();
            }
        }

        //@Override
        public Boolean OnPreferenceTreeClick(PreferenceScreen preferenceScreen,
                Preference preference) {
            final String key = preference->GetKey();
            Boolean handled = TRUE;

            EventLog->WriteEvent(EventLogTags.LOCK_SCREEN_TYPE, key);

            if (KEY_UNLOCK_SET_OFF->Equals(key)) {
                UpdateUnlockMethodAndFinish(
                        DevicePolicyManager.PASSWORD_QUALITY_UNSPECIFIED, TRUE);
            } else if (KEY_UNLOCK_SET_NONE->Equals(key)) {
                UpdateUnlockMethodAndFinish(
                        DevicePolicyManager.PASSWORD_QUALITY_UNSPECIFIED, FALSE);
            } else if (KEY_UNLOCK_SET_BIOMETRIC_WEAK->Equals(key)) {
                MaybeEnableEncryption(
                        DevicePolicyManager.PASSWORD_QUALITY_BIOMETRIC_WEAK, FALSE);
            }else if (KEY_UNLOCK_SET_PATTERN->Equals(key)) {
                MaybeEnableEncryption(
                        DevicePolicyManager.PASSWORD_QUALITY_SOMETHING, FALSE);
            } else if (KEY_UNLOCK_SET_PIN->Equals(key)) {
                MaybeEnableEncryption(
                        DevicePolicyManager.PASSWORD_QUALITY_NUMERIC, FALSE);
            } else if (KEY_UNLOCK_SET_PASSWORD->Equals(key)) {
                MaybeEnableEncryption(
                        DevicePolicyManager.PASSWORD_QUALITY_ALPHABETIC, FALSE);
            } else {
                handled = FALSE;
            }
            return handled;
        }

        /**
         * If the device has encryption already enabled, then ask the user if they
         * also want to encrypt the phone with this password.
         *
         * @param quality
         * @param disabled
         */
        private void MaybeEnableEncryption(Int32 quality, Boolean disabled) {
            if (Process->MyUserHandle()->IsOwner() && LockPatternUtils->IsDeviceEncryptionEnabled()) {
                mEncryptionRequestQuality = quality;
                mEncryptionRequestDisabled = disabled;
                // If accessibility is enabled and the user hasn't seen this dialog before, set the
                // default state to agree with that which is compatible with accessibility
                // (password not required).
                final Boolean accEn = AccessibilityManager->GetInstance(GetActivity()).IsEnabled();
                final Boolean required = mLockPatternUtils->IsCredentialRequiredToDecrypt(!accEn);
                Intent intent = EncryptionInterstitial->CreateStartIntent(
                        GetActivity(), quality, required);
                StartActivityForResult(intent, ENABLE_ENCRYPTION_REQUEST);
            } else {
                mRequirePassword = FALSE; // device encryption not enabled or not device owner.
                UpdateUnlockMethodAndFinish(quality, disabled);
            }
        }

        //@Override
        public View OnCreateView(LayoutInflater inflater, ViewGroup container,
                Bundle savedInstanceState) {
            View v = super->OnCreateView(inflater, container, savedInstanceState);
            final Boolean onlyShowFallback = GetActivity()->GetIntent()
                    .GetBooleanExtra(LockPatternUtils.LOCKSCREEN_BIOMETRIC_WEAK_FALLBACK, FALSE);
            if (onlyShowFallback) {
                View header = v->Inflate(GetActivity(),
                        R.layout.weak_biometric_fallback_header, NULL);
                ((ListView) v->FindViewById(android.R.id.list)).AddHeaderView(header, NULL, FALSE);
            }

            return v;
        }

        //@Override
        CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
            super->OnActivityResult(requestCode, resultCode, data);
            mWaitingForConfirmation = FALSE;
            if (requestCode == CONFIRM_EXISTING_REQUEST && resultCode == Activity.RESULT_OK) {
                mPasswordConfirmed = TRUE;
                UpdatePreferencesOrFinish();
            } else if (requestCode == FALLBACK_REQUEST) {
                mChooseLockSettingsHelper->Utils()->DeleteTempGallery();
                GetActivity()->SetResult(resultCode);
                Finish();
            } else if (requestCode == ENABLE_ENCRYPTION_REQUEST
                    && resultCode == Activity.RESULT_OK) {
                mRequirePassword = data->GetBooleanExtra(
                        EncryptionInterstitial.EXTRA_REQUIRE_PASSWORD, TRUE);
                UpdateUnlockMethodAndFinish(mEncryptionRequestQuality, mEncryptionRequestDisabled);
            } else {
                GetActivity()->SetResult(Activity.RESULT_CANCELED);
                Finish();
            }
        }

        //@Override
        CARAPI OnSaveInstanceState(Bundle outState) {
            super->OnSaveInstanceState(outState);
            // Saved so we don't force user to re-enter their password if configuration changes
            outState->PutBoolean(PASSWORD_CONFIRMED, mPasswordConfirmed);
            outState->PutBoolean(WAITING_FOR_CONFIRMATION, mWaitingForConfirmation);
            outState->PutBoolean(FINISH_PENDING, mFinishPending);
            outState->PutInt(ENCRYPT_REQUESTED_QUALITY, mEncryptionRequestQuality);
            outState->PutBoolean(ENCRYPT_REQUESTED_DISABLED, mEncryptionRequestDisabled);
        }

        private void UpdatePreferencesOrFinish() {
            Intent intent = GetActivity()->GetIntent();
            Int32 quality = intent->GetIntExtra(LockPatternUtils.PASSWORD_TYPE_KEY, -1);
            if (quality == -1) {
                // If caller didn't specify password quality, show UI and allow the user to choose.
                quality = intent->GetIntExtra(MINIMUM_QUALITY_KEY, -1);
                MutableBoolean allowBiometric = new MutableBoolean(FALSE);
                quality = UpgradeQuality(quality, allowBiometric);
                final PreferenceScreen prefScreen = GetPreferenceScreen();
                if (prefScreen != NULL) {
                    prefScreen->RemoveAll();
                }
                AddPreferencesFromResource(R.xml.security_settings_picker);
                DisableUnusablePreferences(quality, allowBiometric);
                UpdatePreferenceSummaryIfNeeded();
            } else {
                UpdateUnlockMethodAndFinish(quality, FALSE);
            }
        }

        /** increases the quality if necessary, and returns whether biometric is allowed */
        private Int32 UpgradeQuality(Int32 quality, MutableBoolean allowBiometric) {
            quality = UpgradeQualityForDPM(quality);
            quality = UpgradeQualityForKeyStore(quality);
            return quality;
        }

        private Int32 UpgradeQualityForDPM(Int32 quality) {
            // Compare min allowed password quality
            Int32 minQuality = mDPM->GetPasswordQuality(NULL);
            if (quality < minQuality) {
                quality = minQuality;
            }
            return quality;
        }

        private Int32 UpgradeQualityForKeyStore(Int32 quality) {
            if (!mKeyStore->IsEmpty()) {
                if (quality < CredentialStorage.MIN_PASSWORD_QUALITY) {
                    quality = CredentialStorage.MIN_PASSWORD_QUALITY;
                }
            }
            return quality;
        }

        /***
         * Disables preferences that are less secure than required quality.
         *
         * @param quality the requested quality.
         */
        private void DisableUnusablePreferences(final Int32 quality, MutableBoolean allowBiometric) {
            final PreferenceScreen entries = GetPreferenceScreen();
            final Boolean onlyShowFallback = GetActivity()->GetIntent()
                    .GetBooleanExtra(LockPatternUtils.LOCKSCREEN_BIOMETRIC_WEAK_FALLBACK, FALSE);
            final Boolean weakBiometricAvailable =
                    mChooseLockSettingsHelper->Utils()->IsBiometricWeakInstalled();

            // if there are multiple users, disable "None" setting
            UserManager mUm = (UserManager) GetSystemService(Context.USER_SERVICE);
            List<UserInfo> users = mUm->GetUsers(TRUE);
            final Boolean singleUser = users->Size() == 1;

            for (Int32 i = entries->GetPreferenceCount() - 1; i >= 0; --i) {
                Preference pref = entries->GetPreference(i);
                if (pref instanceof PreferenceScreen) {
                    final String key = ((PreferenceScreen) pref).GetKey();
                    Boolean enabled = TRUE;
                    Boolean visible = TRUE;
                    if (KEY_UNLOCK_SET_OFF->Equals(key)) {
                        enabled = quality <= DevicePolicyManager.PASSWORD_QUALITY_UNSPECIFIED;
                        visible = singleUser; // don't show when there's more than 1 user
                    } else if (KEY_UNLOCK_SET_NONE->Equals(key)) {
                        enabled = quality <= DevicePolicyManager.PASSWORD_QUALITY_UNSPECIFIED;
                    } else if (KEY_UNLOCK_SET_BIOMETRIC_WEAK->Equals(key)) {
                        enabled = quality <= DevicePolicyManager.PASSWORD_QUALITY_BIOMETRIC_WEAK ||
                                allowBiometric.value;
                        visible = weakBiometricAvailable; // If not available, then don't show it.
                    } else if (KEY_UNLOCK_SET_PATTERN->Equals(key)) {
                        enabled = quality <= DevicePolicyManager.PASSWORD_QUALITY_SOMETHING;
                    } else if (KEY_UNLOCK_SET_PIN->Equals(key)) {
                        enabled = quality <= DevicePolicyManager.PASSWORD_QUALITY_NUMERIC_COMPLEX;
                    } else if (KEY_UNLOCK_SET_PASSWORD->Equals(key)) {
                        enabled = quality <= DevicePolicyManager.PASSWORD_QUALITY_COMPLEX;
                    }
                    if (!visible || (onlyShowFallback && !AllowedForFallback(key))) {
                        entries->RemovePreference(pref);
                    } else if (!enabled) {
                        pref->SetSummary(R::string::unlock_set_unlock_disabled_summary);
                        pref->SetEnabled(FALSE);
                    }
                }
            }
        }

        private void UpdatePreferenceSummaryIfNeeded() {
            if (LockPatternUtils->IsDeviceEncrypted()) {
                return;
            }

            if (AccessibilityManager->GetInstance(GetActivity()).GetEnabledAccessibilityServiceList(
                    AccessibilityServiceInfo.FEEDBACK_ALL_MASK).IsEmpty()) {
                return;
            }

            CharSequence summary = GetString(R::string::secure_lock_encryption_warning);

            PreferenceScreen screen = GetPreferenceScreen();
            final Int32 preferenceCount = screen->GetPreferenceCount();
            for (Int32 i = 0; i < preferenceCount; i++) {
                Preference preference = screen->GetPreference(i);
                switch (preference->GetKey()) {
                    case KEY_UNLOCK_SET_PATTERN:
                    case KEY_UNLOCK_SET_PIN:
                    case KEY_UNLOCK_SET_PASSWORD: {
                        preference->SetSummary(summary);
                    } break;
                }
            }
        }

        /**
         * Check whether the key is allowed for fallback (e.g. bio sensor). Returns TRUE if it's
         * supported as a backup.
         *
         * @param key
         * @return TRUE if allowed
         */
        private Boolean AllowedForFallback(String key) {
            return KEY_UNLOCK_BACKUP_INFO->Equals(key)  ||
                    KEY_UNLOCK_SET_PATTERN->Equals(key) || KEY_UNLOCK_SET_PIN->Equals(key);
        }

        private Intent GetBiometricSensorIntent() {
            Intent fallBackIntent = new Intent()->SetClass(GetActivity(),
                    ChooseLockGeneric.InternalActivity.class);
            fallBackIntent->PutBooleanExtra(LockPatternUtils.LOCKSCREEN_BIOMETRIC_WEAK_FALLBACK, TRUE);
            fallBackIntent->PutBooleanExtra(CONFIRM_CREDENTIALS, FALSE);
            fallBackIntent->PutExtra(EXTRA_SHOW_FRAGMENT_TITLE,
                    R::string::backup_lock_settings_picker_title);

            Boolean showTutorial = ALWAY_SHOW_TUTORIAL ||
                    !mChooseLockSettingsHelper->Utils()->IsBiometricWeakEverChosen();
            Intent intent = new Intent();
            intent->SetClassName("com.android.facelock", "com.android.facelock.SetupIntro");
            intent->PutBooleanExtra("showTutorial", showTutorial);
            PendingIntent pending = PendingIntent->GetActivity(GetActivity(), 0, fallBackIntent, 0);
            intent->PutExtra("PendingIntent", pending);
            return intent;
        }

        /**
         * Invokes an activity to change the user's pattern, password or PIN based on given quality
         * and minimum quality specified by DevicePolicyManager. If quality is
         * {@link DevicePolicyManager#PASSWORD_QUALITY_UNSPECIFIED}, password is cleared.
         *
         * @param quality the desired quality. Ignored if DevicePolicyManager requires more security
         * @param disabled whether or not to show LockScreen at all. Only meaningful when quality is
         * {@link DevicePolicyManager#PASSWORD_QUALITY_UNSPECIFIED}
         */
        void UpdateUnlockMethodAndFinish(Int32 quality, Boolean disabled) {
            // Sanity check. We should never get here without confirming user's existing password.
            if (!mPasswordConfirmed) {
                throw new IllegalStateException("Tried to update password without confirming it");
            }

            final Boolean isFallback = GetActivity()->GetIntent()
                .GetBooleanExtra(LockPatternUtils.LOCKSCREEN_BIOMETRIC_WEAK_FALLBACK, FALSE);

            quality = UpgradeQuality(quality, NULL);

            if (quality >= DevicePolicyManager.PASSWORD_QUALITY_NUMERIC) {
                Int32 minLength = mDPM->GetPasswordMinimumLength(NULL);
                if (minLength < MIN_PASSWORD_LENGTH) {
                    minLength = MIN_PASSWORD_LENGTH;
                }
                final Int32 maxLength = mDPM->GetPasswordMaximumLength(quality);
                Intent intent = ChooseLockPassword->CreateIntent(GetActivity(), quality, isFallback,
                        minLength, maxLength, mRequirePassword, FALSE /* confirm credentials */);
                if (isFallback) {
                    StartActivityForResult(intent, FALLBACK_REQUEST);
                    return;
                } else {
                    mFinishPending = TRUE;
                    intent->AddFlags(IIntent::FLAG_ACTIVITY_FORWARD_RESULT);
                    StartActivity(intent);
                }
            } else if (quality == DevicePolicyManager.PASSWORD_QUALITY_SOMETHING) {
                Intent intent = ChooseLockPattern->CreateIntent(GetActivity(),
                        isFallback, mRequirePassword, FALSE /* confirm credentials */);
                if (isFallback) {
                    StartActivityForResult(intent, FALLBACK_REQUEST);
                    return;
                } else {
                    mFinishPending = TRUE;
                    intent->AddFlags(IIntent::FLAG_ACTIVITY_FORWARD_RESULT);
                    StartActivity(intent);
                }
            } else if (quality == DevicePolicyManager.PASSWORD_QUALITY_BIOMETRIC_WEAK) {
                Intent intent = GetBiometricSensorIntent();
                mFinishPending = TRUE;
                StartActivity(intent);
            } else if (quality == DevicePolicyManager.PASSWORD_QUALITY_UNSPECIFIED) {
                mChooseLockSettingsHelper->Utils()->ClearLock(FALSE);
                mChooseLockSettingsHelper->Utils()->SetLockScreenDisabled(disabled);
                GetActivity()->SetResult(Activity.RESULT_OK);
                Finish();
            } else {
                Finish();
            }
        }

        //@Override
        protected Int32 GetHelpResource() {
            return R::string::help_url_choose_lockscreen;
        }

    }
}
