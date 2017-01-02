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

package com.android.settings.accounts;

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Accounts::IAccountManager;
using Elastos::Droid::Accounts::IAuthenticatorDescription;
using Elastos::Droid::App::IActionBar;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::ISyncAdapterType;
using Elastos::Droid::Content::ISyncInfo;
using Elastos::Droid::Content::ISyncStatusInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::Preference::IOnPreferenceClickListener;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::IAccountPreference;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::IUtils;
using Elastos::Droid::Settings::Location::ILocationSettings;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IDate;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IList;

using static android::Content::Intent::IEXTRA_USER;

/** Manages settings for Google Account. */
public class ManageAccountsSettings extends AccountPreferenceBase
        implements AuthenticatorHelper.OnAccountsUpdateListener {
    private static const String ACCOUNT_KEY = "account"; // to pass to auth settings
    public static const String KEY_ACCOUNT_TYPE = "account_type";
    public static const String KEY_ACCOUNT_LABEL = "account_label";

    // Action name for the broadcast intent when the Google account preferences page is launching
    // the location settings.
    private static const String LAUNCHING_LOCATION_SETTINGS =
            "com.android.settings.accounts.LAUNCHING_LOCATION_SETTINGS";

    private static const Int32 MENU_SYNC_NOW_ID = Menu.FIRST;
    private static const Int32 MENU_SYNC_CANCEL_ID    = Menu.FIRST + 1;

    private static const Int32 REQUEST_SHOW_SYNC_SETTINGS = 1;

    private String[] mAuthorities;
    private TextView mErrorInfoView;

    // If an account type is set, then show only accounts of that type
    private String mAccountType;
    // Temporary hack, to deal with backward compatibility
    private Account mFirstAccount;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        Bundle args = GetArguments();
        if (args != NULL && args->ContainsKey(KEY_ACCOUNT_TYPE)) {
            mAccountType = args->GetString(KEY_ACCOUNT_TYPE);
        }
        AddPreferencesFromResource(R.xml.manage_accounts_settings);
        SetHasOptionsMenu(TRUE);
    }

    //@Override
    CARAPI OnStart() {
        super->OnStart();
        mAuthenticatorHelper->ListenToAccountUpdates();
        UpdateAuthDescriptions();
        ShowAccountsIfNeeded();
    }

    //@Override
    public View OnCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        final View view = inflater->Inflate(R.layout.manage_accounts_screen, container, FALSE);
        final ListView list = (ListView) view->FindViewById(android.R.id.list);
        Utils->PrepareCustomPreferencesList(container, view, list, FALSE);
        return view;
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);

        final Activity activity = GetActivity();
        final View view = GetView();

        mErrorInfoView = (TextView)view->FindViewById(R.id.sync_settings_error_info);
        mErrorInfoView->SetVisibility(View.GONE);

        mAuthorities = activity->GetIntent()->GetStringArrayExtra(AUTHORITIES_FILTER_KEY);

        Bundle args = GetArguments();
        if (args != NULL && args->ContainsKey(KEY_ACCOUNT_LABEL)) {
            GetActivity()->SetTitle(args->GetString(KEY_ACCOUNT_LABEL));
        }
    }

    //@Override
    CARAPI OnStop() {
        super->OnStop();
        final Activity activity = GetActivity();
        mAuthenticatorHelper->StopListeningToAccountUpdates();
        activity->GetActionBar()->SetDisplayOptions(0, ActionBar.DISPLAY_SHOW_CUSTOM);
        activity->GetActionBar()->SetCustomView(NULL);
    }

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen preferences, Preference preference) {
        if (preference instanceof AccountPreference) {
            StartAccountSettings((AccountPreference) preference);
        } else {
            return FALSE;
        }
        return TRUE;
    }

    private void StartAccountSettings(AccountPreference acctPref) {
        Bundle args = new Bundle();
        args->PutParcelable(AccountSyncSettings.ACCOUNT_KEY, acctPref->GetAccount());
        args->PutParcelable(EXTRA_USER, mUserHandle);
        ((SettingsActivity) GetActivity()).StartPreferencePanel(
                AccountSyncSettings.class->GetCanonicalName(), args,
                R::string::account_sync_settings_title, acctPref->GetAccount().name,
                this, REQUEST_SHOW_SYNC_SETTINGS);
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        MenuItem syncNow = menu->Add(0, MENU_SYNC_NOW_ID, 0,
                GetString(R::string::sync_menu_sync_now))
                .SetIcon(R.drawable.ic_menu_refresh_holo_dark);
        MenuItem syncCancel = menu->Add(0, MENU_SYNC_CANCEL_ID, 0,
                GetString(R::string::sync_menu_sync_cancel))
                .SetIcon(R.drawable.ic_menu_close_clear_cancel);
        super->OnCreateOptionsMenu(menu, inflater);
    }

    //@Override
    CARAPI OnPrepareOptionsMenu(Menu menu) {
        super->OnPrepareOptionsMenu(menu);
        Boolean syncActive = ContentResolver->GetCurrentSyncsAsUser(
                mUserHandle->GetIdentifier()).IsEmpty();
        menu->FindItem(MENU_SYNC_NOW_ID).SetVisible(!syncActive && mFirstAccount != NULL);
        menu->FindItem(MENU_SYNC_CANCEL_ID).SetVisible(syncActive && mFirstAccount != NULL);
    }

    //@Override
    public Boolean OnOptionsItemSelected(MenuItem item) {
        switch (item->GetItemId()) {
        case MENU_SYNC_NOW_ID:
            RequestOrCancelSyncForAccounts(TRUE);
            return TRUE;
        case MENU_SYNC_CANCEL_ID:
            RequestOrCancelSyncForAccounts(FALSE);
            return TRUE;
        }
        return super->OnOptionsItemSelected(item);
    }

    private void RequestOrCancelSyncForAccounts(Boolean sync) {
        final Int32 userId = mUserHandle->GetIdentifier();
        SyncAdapterType[] syncAdapters = ContentResolver->GetSyncAdapterTypesAsUser(userId);
        Bundle extras = new Bundle();
        extras->PutBoolean(ContentResolver.SYNC_EXTRAS_MANUAL, TRUE);
        Int32 count = GetPreferenceScreen()->GetPreferenceCount();
        // For each account
        for (Int32 i = 0; i < count; i++) {
            Preference pref = GetPreferenceScreen()->GetPreference(i);
            if (pref instanceof AccountPreference) {
                Account account = ((AccountPreference) pref).GetAccount();
                // For all available sync authorities, sync those that are enabled for the account
                for (Int32 j = 0; j < syncAdapters.length; j++) {
                    SyncAdapterType sa = syncAdapters[j];
                    if (syncAdapters[j].accountType->Equals(mAccountType)
                            && ContentResolver->GetSyncAutomaticallyAsUser(account, sa.authority,
                                    userId)) {
                        if (sync) {
                            ContentResolver->RequestSyncAsUser(account, sa.authority, userId,
                                    extras);
                        } else {
                            ContentResolver->CancelSyncAsUser(account, sa.authority, userId);
                        }
                    }
                }
            }
        }
    }

    //@Override
    protected void OnSyncStateUpdated() {
        ShowSyncState();
    }

    private void ShowSyncState() {
        // Catch any delayed delivery of update messages
        if (GetActivity() == NULL) return;

        final Int32 userId = mUserHandle->GetIdentifier();

        // iterate over all the preferences, setting the state properly for each
        List<SyncInfo> currentSyncs = ContentResolver->GetCurrentSyncsAsUser(userId);

        Boolean anySyncFailed = FALSE; // TRUE if sync on any account failed
        Date date = new Date();

        // only track userfacing sync adapters when deciding if account is synced or not
        final SyncAdapterType[] syncAdapters = ContentResolver->GetSyncAdapterTypesAsUser(userId);
        HashSet<String> userFacing = new HashSet<String>();
        for (Int32 k = 0, n = syncAdapters.length; k < n; k++) {
            final SyncAdapterType sa = syncAdapters[k];
            if (sa->IsUserVisible()) {
                userFacing->Add(sa.authority);
            }
        }
        for (Int32 i = 0, count = GetPreferenceScreen()->GetPreferenceCount(); i < count; i++) {
            Preference pref = GetPreferenceScreen()->GetPreference(i);
            if (! (pref instanceof AccountPreference)) {
                continue;
            }

            AccountPreference accountPref = (AccountPreference) pref;
            Account account = accountPref->GetAccount();
            Int32 syncCount = 0;
            Int64 lastSuccessTime = 0;
            Boolean syncIsFailing = FALSE;
            final ArrayList<String> authorities = accountPref->GetAuthorities();
            Boolean syncingNow = FALSE;
            if (authorities != NULL) {
                for (String authority : authorities) {
                    SyncStatusInfo status = ContentResolver->GetSyncStatusAsUser(account, authority,
                            userId);
                    Boolean syncEnabled = IsSyncEnabled(userId, account, authority);
                    Boolean authorityIsPending = ContentResolver->IsSyncPending(account, authority);
                    Boolean activelySyncing = IsSyncing(currentSyncs, account, authority);
                    Boolean lastSyncFailed = status != NULL
                            && syncEnabled
                            && status.lastFailureTime != 0
                            && status->GetLastFailureMesgAsInt(0)
                               != ContentResolver.SYNC_ERROR_SYNC_ALREADY_IN_PROGRESS;
                    if (lastSyncFailed && !activelySyncing && !authorityIsPending) {
                        syncIsFailing = TRUE;
                        anySyncFailed = TRUE;
                    }
                    syncingNow |= activelySyncing;
                    if (status != NULL && lastSuccessTime < status.lastSuccessTime) {
                        lastSuccessTime = status.lastSuccessTime;
                    }
                    syncCount += syncEnabled && userFacing->Contains(authority) ? 1 : 0;
                }
            } else {
                if (Log->IsLoggable(TAG, Log.VERBOSE)) {
                    Logger::V(TAG, "no syncadapters found for " + account);
                }
            }
            if (syncIsFailing) {
                accountPref->SetSyncStatus(AccountPreference.SYNC_ERROR, TRUE);
            } else if (syncCount == 0) {
                accountPref->SetSyncStatus(AccountPreference.SYNC_DISABLED, TRUE);
            } else if (syncCount > 0) {
                if (syncingNow) {
                    accountPref->SetSyncStatus(AccountPreference.SYNC_IN_PROGRESS, TRUE);
                } else {
                    accountPref->SetSyncStatus(AccountPreference.SYNC_ENABLED, TRUE);
                    if (lastSuccessTime > 0) {
                        accountPref->SetSyncStatus(AccountPreference.SYNC_ENABLED, FALSE);
                        date->SetTime(lastSuccessTime);
                        final String timeString = FormatSyncDate(date);
                        accountPref->SetSummary(GetResources()->GetString(
                                R::string::last_synced, timeString));
                    }
                }
            } else {
                accountPref->SetSyncStatus(AccountPreference.SYNC_DISABLED, TRUE);
            }
        }

        mErrorInfoView->SetVisibility(anySyncFailed ? View.VISIBLE : View.GONE);
    }


    private Boolean IsSyncing(List<SyncInfo> currentSyncs, Account account, String authority) {
        final Int32 count = currentSyncs->Size();
        for (Int32 i = 0; i < count;  i++) {
            SyncInfo syncInfo = currentSyncs->Get(i);
            if (syncInfo.account->Equals(account) && syncInfo.authority->Equals(authority)) {
                return TRUE;
            }
        }
        return FALSE;
    }

    private Boolean IsSyncEnabled(Int32 userId, Account account, String authority) {
        return ContentResolver->GetSyncAutomaticallyAsUser(account, authority, userId)
                && ContentResolver->GetMasterSyncAutomaticallyAsUser(userId)
                && (ContentResolver->GetIsSyncableAsUser(account, authority, userId) > 0);
    }

    //@Override
    CARAPI OnAccountsUpdate(UserHandle userHandle) {
        ShowAccountsIfNeeded();
        OnSyncStateUpdated();
    }

    private void ShowAccountsIfNeeded() {
        if (GetActivity() == NULL) return;
        Account[] accounts = AccountManager->Get(GetActivity()).GetAccountsAsUser(
                mUserHandle->GetIdentifier());
        GetPreferenceScreen()->RemoveAll();
        mFirstAccount = NULL;
        AddPreferencesFromResource(R.xml.manage_accounts_settings);
        for (Int32 i = 0, n = accounts.length; i < n; i++) {
            final Account account = accounts[i];
            // If an account type is specified for this screen, skip other types
            if (mAccountType != NULL && !account.type->Equals(mAccountType)) continue;
            final ArrayList<String> auths = GetAuthoritiesForAccountType(account.type);

            Boolean showAccount = TRUE;
            if (mAuthorities != NULL && auths != NULL) {
                showAccount = FALSE;
                for (String requestedAuthority : mAuthorities) {
                    if (auths->Contains(requestedAuthority)) {
                        showAccount = TRUE;
                        break;
                    }
                }
            }

            if (showAccount) {
                final Drawable icon = GetDrawableForType(account.type);
                final AccountPreference preference =
                        new AccountPreference(GetActivity(), account, icon, auths, FALSE);
                GetPreferenceScreen()->AddPreference(preference);
                if (mFirstAccount == NULL) {
                    mFirstAccount = account;
                    GetActivity()->InvalidateOptionsMenu();
                }
            }
        }
        if (mAccountType != NULL && mFirstAccount != NULL) {
            AddAuthenticatorSettings();
        } else {
            // There's no account, reset to top-level of settings
            Intent settingsTop = new Intent(android.provider.Settings.ACTION_SETTINGS);
            settingsTop->SetFlags(IIntent::FLAG_ACTIVITY_CLEAR_TOP);
            GetActivity()->StartActivity(settingsTop);
        }
    }

    private void AddAuthenticatorSettings() {
        PreferenceScreen prefs = AddPreferencesForType(mAccountType, GetPreferenceScreen());
        if (prefs != NULL) {
            UpdatePreferenceIntents(prefs);
        }
    }

    /** Listens to a preference click event and starts a fragment */
    private class FragmentStarter
            implements Preference.OnPreferenceClickListener {
        private final String mClass;
        private final Int32 mTitleRes;

        /**
         * @param className the class name of the fragment to be started.
         * @param title the title resource id of the started preference panel.
         */
        public FragmentStarter(String className, Int32 title) {
            mClass = className;
            mTitleRes = title;
        }

        //@Override
        public Boolean OnPreferenceClick(Preference preference) {
            ((SettingsActivity) GetActivity()).StartPreferencePanel(
                    mClass, NULL, mTitleRes, NULL, NULL, 0);
            // Hack: announce that the Google account preferences page is launching the location
            // settings
            if (mClass->Equals(LocationSettings.class->GetName())) {
                Intent intent = new Intent(LAUNCHING_LOCATION_SETTINGS);
                GetActivity()->SendBroadcast(
                        intent, Manifest::permission::WRITE_SECURE_SETTINGS);
            }
            return TRUE;
        }
    }

    /**
     * Filters through the preference list provided by GoogleLoginService.
     *
     * This method removes all the invalid intent from the list, adds account name as extra into the
     * intent, and hack the location settings to start it as a fragment.
     */
    private void UpdatePreferenceIntents(PreferenceScreen prefs) {
        final PackageManager pm = GetActivity()->GetPackageManager();
        for (Int32 i = 0; i < prefs->GetPreferenceCount();) {
            Preference pref = prefs->GetPreference(i);
            Intent intent = pref->GetIntent();
            if (intent != NULL) {
                // Hack. Launch "Location" as fragment instead of as activity.
                //
                // When "Location" is launched as activity via Intent, there's no "Up" button at the
                // top left, and if there's another running instance of "Location" activity, the
                // back stack would usually point to some other place so the user won't be able to
                // go back to the previous page by "back" key. Using fragment is a much easier
                // solution to those problems.
                //
                // If we set Intent to NULL and assign a fragment to the PreferenceScreen item here,
                // in order to make it work as expected, we still need to modify the container
                // PreferenceActivity, override OnPreferenceStartFragment() and call
                // StartPreferencePanel() there. In order to inject the title string there, more
                // dirty further hack is still needed. It's much easier and cleaner to listen to
                // preference click event here directly.
                if (intent->GetAction()->Equals(
                        android.provider.Settings.ACTION_LOCATION_SOURCE_SETTINGS)) {
                    // The OnPreferenceClickListener overrides the click event completely. No intent
                    // will get fired.
                    pref->SetOnPreferenceClickListener(new FragmentStarter(
                            LocationSettings.class->GetName(),
                            R::string::location_settings_title));
                } else {
                    ResolveInfo ri = pm->ResolveActivityAsUser(intent,
                            PackageManager.MATCH_DEFAULT_ONLY, mUserHandle->GetIdentifier());
                    if (ri == NULL) {
                        prefs->RemovePreference(pref);
                        continue;
                    } else {
                        intent->PutExtra(ACCOUNT_KEY, mFirstAccount);
                        intent->SetFlags(intent->GetFlags() | IIntent::FLAG_ACTIVITY_NEW_TASK);
                        pref->SetOnPreferenceClickListener(new OnPreferenceClickListener() {
                            //@Override
                            public Boolean OnPreferenceClick(Preference preference) {
                                Intent prefIntent = preference->GetIntent();
                                /*
                                 * Check the intent to see if it resolves to a exported=FALSE
                                 * activity that doesn't share a uid with the authenticator.
                                 *
                                 * Otherwise the intent is considered unsafe in that it will be
                                 * exploiting the fact that settings has system privileges.
                                 */
                                if (IsSafeIntent(pm, prefIntent)) {
                                    GetActivity()->StartActivityAsUser(prefIntent, mUserHandle);
                                } else {
                                    Logger::E(TAG,
                                            "Refusing to launch authenticator intent because"
                                            + "it exploits Settings permissions: "
                                            + prefIntent);
                                }
                                return TRUE;
                            }
                        });
                    }
                }
            }
            i++;
        }
    }

    /**
     * Determines if the supplied Intent is safe. A safe intent is one that is
     * will launch a exported=TRUE activity or owned by the same uid as the
     * authenticator supplying the intent.
     */
    private Boolean IsSafeIntent(PackageManager pm, Intent intent) {
        AuthenticatorDescription authDesc =
                mAuthenticatorHelper->GetAccountTypeDescription(mAccountType);
        ResolveInfo resolveInfo = pm->ResolveActivity(intent, 0);
        if (resolveInfo == NULL) {
            return FALSE;
        }
        ActivityInfo resolvedActivityInfo = resolveInfo.activityInfo;
        ApplicationInfo resolvedAppInfo = resolvedActivityInfo.applicationInfo;
        try {
            ApplicationInfo authenticatorAppInf = pm->GetApplicationInfo(authDesc.packageName, 0);
            return resolvedActivityInfo.exported
                    || resolvedAppInfo.uid == authenticatorAppInf.uid;
        } catch (NameNotFoundException e) {
            Logger::E(TAG,
                    "Intent considered unsafe due to exception.",
                    e);
            return FALSE;
        }
    }

    //@Override
    protected void OnAuthDescriptionsUpdated() {
        // Update account icons for all account preference items
        for (Int32 i = 0; i < GetPreferenceScreen()->GetPreferenceCount(); i++) {
            Preference pref = GetPreferenceScreen()->GetPreference(i);
            if (pref instanceof AccountPreference) {
                AccountPreference accPref = (AccountPreference) pref;
                accPref->SetSummary(GetLabelForType(accPref->GetAccount().type));
            }
        }
    }
}
