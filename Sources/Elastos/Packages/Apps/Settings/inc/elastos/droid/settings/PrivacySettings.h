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

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::Backup::IIBackupManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::Preference::IOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Provider::ISettings;

using Elastos::Droid::Settings::Search::IBaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::Indexable::ISearchIndexProvider;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

/**
 * Gesture lock pattern settings.
 */
public class PrivacySettings extends SettingsPreferenceFragment implements
        DialogInterface.OnClickListener {

    // Vendor specific
    private static const String GSETTINGS_PROVIDER = "com.google.settings";
    private static const String BACKUP_CATEGORY = "backup_category";
    private static const String BACKUP_DATA = "backup_data";
    private static const String AUTO_RESTORE = "auto_restore";
    private static const String CONFIGURE_ACCOUNT = "configure_account";
    private static const String PERSONAL_DATA_CATEGORY = "personal_data_category";
    private IBackupManager mBackupManager;
    private SwitchPreference mBackup;
    private SwitchPreference mAutoRestore;
    private Dialog mConfirmDialog;
    private PreferenceScreen mConfigure;
    private Boolean mEnabled;

    private static const Int32 DIALOG_ERASE_BACKUP = 2;
    private Int32 mDialogType;

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        // Don't allow any access if this is a secondary user
        mEnabled = Process->MyUserHandle()->IsOwner();
        if (!mEnabled) {
            return;
        }

        AddPreferencesFromResource(R.xml.privacy_settings);
        final PreferenceScreen screen = GetPreferenceScreen();
        mBackupManager = IBackupManager.Stub->AsInterface(
                ServiceManager->GetService(Context.BACKUP_SERVICE));

        mBackup = (SwitchPreference) screen->FindPreference(BACKUP_DATA);
        mBackup->SetOnPreferenceChangeListener(preferenceChangeListener);

        mAutoRestore = (SwitchPreference) screen->FindPreference(AUTO_RESTORE);
        mAutoRestore->SetOnPreferenceChangeListener(preferenceChangeListener);

        mConfigure = (PreferenceScreen) screen->FindPreference(CONFIGURE_ACCOUNT);

        if (UserManager->Get(GetActivity()).HasUserRestriction(
                UserManager.DISALLOW_FACTORY_RESET)) {
            screen->RemovePreference(FindPreference(PERSONAL_DATA_CATEGORY));
        }

        // Vendor specific
        if (GetActivity()->GetPackageManager().
                ResolveContentProvider(GSETTINGS_PROVIDER, 0) == NULL) {
            screen->RemovePreference(FindPreference(BACKUP_CATEGORY));
        }
        UpdateToggles();
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();

        // Refresh UI
        if (mEnabled) {
            UpdateToggles();
        }
    }

    //@Override
    CARAPI OnStop() {
        if (mConfirmDialog != NULL && mConfirmDialog->IsShowing()) {
            mConfirmDialog->Dismiss();
        }
        mConfirmDialog = NULL;
        mDialogType = 0;
        super->OnStop();
    }

    private OnPreferenceChangeListener preferenceChangeListener = new OnPreferenceChangeListener() {
        //@Override
        public Boolean OnPreferenceChange(Preference preference, Object newValue) {
            if (!(preference instanceof SwitchPreference)) {
                return TRUE;
            }
            Boolean nextValue = (Boolean) newValue;
            Boolean result = FALSE;
            if (preference == mBackup) {
                if (nextValue == FALSE) {
                    // Don't change Switch status until user makes choice in dialog
                    // so return FALSE here.
                    ShowEraseBackupDialog();
                } else {
                    SetBackupEnabled(TRUE);
                    result = TRUE;
                }
            } else if (preference == mAutoRestore) {
                try {
                    mBackupManager->SetAutoRestore(nextValue);
                    result = TRUE;
                } catch (RemoteException e) {
                    mAutoRestore->SetChecked(!nextValue);
                }
            }
            return result;
        }
    };

    private void ShowEraseBackupDialog() {
        mDialogType = DIALOG_ERASE_BACKUP;
        CharSequence msg = GetResources()->GetText(R::string::backup_erase_dialog_message);
        // TODO: DialogFragment?
        mConfirmDialog = new AlertDialog->Builder(GetActivity()).SetMessage(msg)
                .SetTitle(R::string::backup_erase_dialog_title)
                .SetPositiveButton(android.R::string::ok, this)
                .SetNegativeButton(android.R::string::cancel, this)
                .Show();
    }

    /*
     * Creates toggles for each available location provider
     */
    private void UpdateToggles() {
        ContentResolver res = GetContentResolver();

        Boolean backupEnabled = FALSE;
        Intent configIntent = NULL;
        String configSummary = NULL;
        try {
            backupEnabled = mBackupManager->IsBackupEnabled();
            String transport = mBackupManager->GetCurrentTransport();
            configIntent = mBackupManager->GetConfigurationIntent(transport);
            configSummary = mBackupManager->GetDestinationString(transport);
        } catch (RemoteException e) {
            // leave it 'FALSE' and disable the UI; there's no backup manager
            mBackup->SetEnabled(FALSE);
        }
        mBackup->SetChecked(backupEnabled);

        mAutoRestore->SetChecked(Settings.Secure->GetInt(res,
                Settings.Secure.BACKUP_AUTO_RESTORE, 1) == 1);
        mAutoRestore->SetEnabled(backupEnabled);

        final Boolean configureEnabled = (configIntent != NULL) && backupEnabled;
        mConfigure->SetEnabled(configureEnabled);
        mConfigure->SetIntent(configIntent);
        SetConfigureSummary(configSummary);
    }

    private void SetConfigureSummary(String summary) {
        if (summary != NULL) {
            mConfigure->SetSummary(summary);
        } else {
            mConfigure->SetSummary(R::string::backup_configure_account_default_summary);
        }
    }

    private void UpdateConfigureSummary() {
        try {
            String transport = mBackupManager->GetCurrentTransport();
            String summary = mBackupManager->GetDestinationString(transport);
            SetConfigureSummary(summary);
        } catch (RemoteException e) {
            // Not much we can do here
        }
    }

    //@Override
    CARAPI OnClick(DialogInterface dialog, Int32 which) {
        // Dialog is triggered before Switch status change, that means marking the Switch to
        // TRUE in ShowEraseBackupDialog() method will be override by following status change.
        // So we do manual switching here due to users' response.
        if (mDialogType == DIALOG_ERASE_BACKUP) {
            // Accept turning off backup
            if (which == DialogInterface.BUTTON_POSITIVE) {
                SetBackupEnabled(FALSE);
            } else if (which == DialogInterface.BUTTON_NEGATIVE) {
                // Reject turning off backup
                SetBackupEnabled(TRUE);
            }
            UpdateConfigureSummary();
        }
        mDialogType = 0;
    }

    /**
     * Informs the BackupManager of a change in backup state - if backup is disabled,
     * the data on the server will be erased.
     * @param enable whether to enable backup
     */
    private void SetBackupEnabled(Boolean enable) {
        if (mBackupManager != NULL) {
            try {
                mBackupManager->SetBackupEnabled(enable);
            } catch (RemoteException e) {
                mBackup->SetChecked(!enable);
                mAutoRestore->SetEnabled(!enable);
                return;
            }
        }
        mBackup->SetChecked(enable);
        mAutoRestore->SetEnabled(enable);
        mConfigure->SetEnabled(enable);
    }

    //@Override
    protected Int32 GetHelpResource() {
        return R::string::help_url_backup_reset;
    }

    /**
     * For Search.
     */
    public static const SearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
            new PrivacySearchIndexProvider();

    private static class PrivacySearchIndexProvider extends BaseSearchIndexProvider {

        Boolean mIsPrimary;

        public PrivacySearchIndexProvider() {
            Super();

            mIsPrimary = UserHandle->MyUserId() == UserHandle.USER_OWNER;
        }

        //@Override
        public List<SearchIndexableResource> GetXmlResourcesToIndex(
                Context context, Boolean enabled) {

            List<SearchIndexableResource> result = new ArrayList<SearchIndexableResource>();

            // For non-primary user, no backup or reset is available
            if (!mIsPrimary) {
                return result;
            }

            SearchIndexableResource sir = new SearchIndexableResource(context);
            sir.xmlResId = R.xml.privacy_settings;
            result->Add(sir);

            return result;
        }
    }

}