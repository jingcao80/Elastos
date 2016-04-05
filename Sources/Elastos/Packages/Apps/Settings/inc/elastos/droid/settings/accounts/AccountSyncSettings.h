

package com.android.settings.accounts;

using com::Google::Android::Collect::ILists;

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Accounts::IAccountManager;
using Elastos::Droid::Accounts::IAccountManagerCallback;
using Elastos::Droid::Accounts::IAccountManagerFuture;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::ISyncAdapterType;
using Elastos::Droid::Content::ISyncInfo;
using Elastos::Droid::Content::ISyncStatusInfo;
using Elastos::Droid::Content::Pm::IProviderInfo;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::IUtils;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IDate;
using Elastos::Utility::IList;

public class AccountSyncSettings extends AccountPreferenceBase {

    public static const String ACCOUNT_KEY = "account";
    private static const Int32 MENU_SYNC_NOW_ID       = Menu.FIRST;
    private static const Int32 MENU_SYNC_CANCEL_ID    = Menu.FIRST + 1;
    private static const Int32 MENU_REMOVE_ACCOUNT_ID = Menu.FIRST + 2;
    private static const Int32 REALLY_REMOVE_DIALOG = 100;
    private static const Int32 FAILED_REMOVAL_DIALOG = 101;
    private static const Int32 CANT_DO_ONETIME_SYNC_DIALOG = 102;
    private TextView mUserId;
    private TextView mProviderId;
    private ImageView mProviderIcon;
    private TextView mErrorInfoView;
    private Account mAccount;
    private ArrayList<SyncStateCheckBoxPreference> mCheckBoxes =
                new ArrayList<SyncStateCheckBoxPreference>();
    private ArrayList<SyncAdapterType> mInvisibleAdapters = Lists->NewArrayList();

    //@Override
    public Dialog OnCreateDialog(final Int32 id) {
        Dialog dialog = NULL;
        if (id == REALLY_REMOVE_DIALOG) {
            dialog = new AlertDialog->Builder(GetActivity())
                .SetTitle(R::string::really_remove_account_title)
                .SetMessage(R::string::really_remove_account_message)
                .SetNegativeButton(android.R::string::cancel, NULL)
                .SetPositiveButton(R::string::remove_account_label,
                        new DialogInterface->OnClickListener() {
                    //@Override
                    CARAPI OnClick(DialogInterface dialog, Int32 which) {
                        // TODO: We need an API to remove an account from a different user.
                        // See: http://b/15466880
                        AccountManager->Get(AccountSyncSettings.this->GetActivity())
                                .RemoveAccountAsUser(mAccount,
                                new AccountManagerCallback<Boolean>() {
                            //@Override
                            CARAPI Run(AccountManagerFuture<Boolean> future) {
                                // If already out of this screen, don't proceed.
                                if (!AccountSyncSettings.this->IsResumed()) {
                                    return;
                                }
                                Boolean failed = TRUE;
                                try {
                                    if (future->GetResult() == TRUE) {
                                        failed = FALSE;
                                    }
                                } catch (OperationCanceledException e) {
                                    // handled below
                                } catch (IOException e) {
                                    // handled below
                                } catch (AuthenticatorException e) {
                                    // handled below
                                }
                                if (failed && GetActivity() != NULL &&
                                        !GetActivity()->IsFinishing()) {
                                    ShowDialog(FAILED_REMOVAL_DIALOG);
                                } else {
                                    Finish();
                                }
                            }
                        }, NULL, mUserHandle);
                    }
                })
                .Create();
        } else if (id == FAILED_REMOVAL_DIALOG) {
            dialog = new AlertDialog->Builder(GetActivity())
                .SetTitle(R::string::really_remove_account_title)
                .SetPositiveButton(android.R::string::ok, NULL)
                .SetMessage(R::string::remove_account_failed)
                .Create();
        } else if (id == CANT_DO_ONETIME_SYNC_DIALOG) {
            dialog = new AlertDialog->Builder(GetActivity())
                .SetTitle(R::string::cant_sync_dialog_title)
                .SetMessage(R::string::cant_sync_dialog_message)
                .SetPositiveButton(android.R::string::ok, NULL)
                .Create();
        }
        return dialog;
    }

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        SetHasOptionsMenu(TRUE);
    }

    //@Override
    public View OnCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        final View view = inflater->Inflate(R.layout.account_sync_screen, container, FALSE);

        final ListView list = (ListView) view->FindViewById(android.R.id.list);
        Utils->PrepareCustomPreferencesList(container, view, list, FALSE);

        InitializeUi(view);

        return view;
    }

    protected void InitializeUi(final View rootView) {
        AddPreferencesFromResource(R.xml.account_sync_settings);

        mErrorInfoView = (TextView) rootView->FindViewById(R.id.sync_settings_error_info);
        mErrorInfoView->SetVisibility(View.GONE);

        mUserId = (TextView) rootView->FindViewById(R.id.user_id);
        mProviderId = (TextView) rootView->FindViewById(R.id.provider_id);
        mProviderIcon = (ImageView) rootView->FindViewById(R.id.provider_icon);
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);

        Bundle arguments = GetArguments();
        if (arguments == NULL) {
            Logger::E(TAG, "No arguments provided when starting intent. ACCOUNT_KEY needed.");
            Finish();
            return;
        }
        mAccount = (Account) arguments->GetParcelable(ACCOUNT_KEY);
        if (!AccountExists(mAccount)) {
            Logger::E(TAG, "Account provided does not exist: " + mAccount);
            Finish();
            return;
        }
        if (Log->IsLoggable(TAG, Log.VERBOSE)) {
          Logger::V(TAG, "Got account: " + mAccount);
        }
        mUserId->SetText(mAccount.name);
        mProviderId->SetText(mAccount.type);
    }

    //@Override
    CARAPI OnResume() {
        mAuthenticatorHelper->ListenToAccountUpdates();
        UpdateAuthDescriptions();
        OnAccountsUpdate(UserHandle->GetCallingUserHandle());

        super->OnResume();
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        mAuthenticatorHelper->StopListeningToAccountUpdates();
    }

    private void AddSyncStateCheckBox(Account account, String authority) {
        SyncStateCheckBoxPreference item =
                new SyncStateCheckBoxPreference(GetActivity(), account, authority);
        item->SetPersistent(FALSE);
        final ProviderInfo providerInfo = GetPackageManager()->ResolveContentProviderAsUser(
                authority, 0, mUserHandle->GetIdentifier());
        if (providerInfo == NULL) {
            return;
        }
        CharSequence providerLabel = providerInfo->LoadLabel(GetPackageManager());
        if (TextUtils->IsEmpty(providerLabel)) {
            Logger::E(TAG, "Provider needs a label for authority '" + authority + "'");
            return;
        }
        String title = GetString(R::string::sync_item_title, providerLabel);
        item->SetTitle(title);
        item->SetKey(authority);
        mCheckBoxes->Add(item);
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {

        MenuItem syncNow = menu->Add(0, MENU_SYNC_NOW_ID, 0,
                GetString(R::string::sync_menu_sync_now))
                .SetIcon(R.drawable.ic_menu_refresh_holo_dark);
        MenuItem syncCancel = menu->Add(0, MENU_SYNC_CANCEL_ID, 0,
                GetString(R::string::sync_menu_sync_cancel))
                .SetIcon(R.drawable.ic_menu_close_clear_cancel);
        final UserManager um = (UserManager) GetSystemService(Context.USER_SERVICE);
        if (!um->HasUserRestriction(UserManager.DISALLOW_MODIFY_ACCOUNTS, mUserHandle)) {
            MenuItem removeAccount = menu->Add(0, MENU_REMOVE_ACCOUNT_ID, 0,
                    GetString(R::string::remove_account_label))
                    .SetIcon(R.drawable.ic_menu_delete);
            removeAccount->SetShowAsAction(MenuItem.SHOW_AS_ACTION_NEVER |
                    MenuItem.SHOW_AS_ACTION_WITH_TEXT);
        }
        syncNow->SetShowAsAction(MenuItem.SHOW_AS_ACTION_NEVER |
                MenuItem.SHOW_AS_ACTION_WITH_TEXT);
        syncCancel->SetShowAsAction(MenuItem.SHOW_AS_ACTION_NEVER |
                MenuItem.SHOW_AS_ACTION_WITH_TEXT);

        super->OnCreateOptionsMenu(menu, inflater);
    }

    //@Override
    CARAPI OnPrepareOptionsMenu(Menu menu) {
        super->OnPrepareOptionsMenu(menu);
        // Note that this also counts accounts that are not currently displayed
        Boolean syncActive = ContentResolver->GetCurrentSyncsAsUser(
                mUserHandle->GetIdentifier()).IsEmpty();
        menu->FindItem(MENU_SYNC_NOW_ID).SetVisible(!syncActive);
        menu->FindItem(MENU_SYNC_CANCEL_ID).SetVisible(syncActive);
    }

    //@Override
    public Boolean OnOptionsItemSelected(MenuItem item) {
        switch (item->GetItemId()) {
            case MENU_SYNC_NOW_ID:
                StartSyncForEnabledProviders();
                return TRUE;
            case MENU_SYNC_CANCEL_ID:
                CancelSyncForEnabledProviders();
                return TRUE;
            case MENU_REMOVE_ACCOUNT_ID:
                ShowDialog(REALLY_REMOVE_DIALOG);
                return TRUE;
        }
        return super->OnOptionsItemSelected(item);
    }

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen preferences, Preference preference) {
        if (preference instanceof SyncStateCheckBoxPreference) {
            SyncStateCheckBoxPreference syncPref = (SyncStateCheckBoxPreference) preference;
            String authority = syncPref->GetAuthority();
            Account account = syncPref->GetAccount();
            final Int32 userId = mUserHandle->GetIdentifier();
            Boolean syncAutomatically = ContentResolver->GetSyncAutomaticallyAsUser(account,
                    authority, userId);
            if (syncPref->IsOneTimeSyncMode()) {
                RequestOrCancelSync(account, authority, TRUE);
            } else {
                Boolean syncOn = syncPref->IsChecked();
                Boolean oldSyncState = syncAutomatically;
                if (syncOn != oldSyncState) {
                    // if we're enabling sync, this will request a sync as well
                    ContentResolver->SetSyncAutomaticallyAsUser(account, authority, syncOn, userId);
                    // if the master sync switch is off, the request above will
                    // get dropped.  when the user clicks on this toggle,
                    // we want to force the sync, however.
                    if (!ContentResolver->GetMasterSyncAutomaticallyAsUser(userId) || !syncOn) {
                        RequestOrCancelSync(account, authority, syncOn);
                    }
                }
            }
            return TRUE;
        } else {
            return super->OnPreferenceTreeClick(preferences, preference);
        }
    }

    private void StartSyncForEnabledProviders() {
        RequestOrCancelSyncForEnabledProviders(TRUE /* start them */);
        GetActivity()->InvalidateOptionsMenu();
    }

    private void CancelSyncForEnabledProviders() {
        RequestOrCancelSyncForEnabledProviders(FALSE /* cancel them */);
        GetActivity()->InvalidateOptionsMenu();
    }

    private void RequestOrCancelSyncForEnabledProviders(Boolean startSync) {
        // sync everything that the user has enabled
        Int32 count = GetPreferenceScreen()->GetPreferenceCount();
        for (Int32 i = 0; i < count; i++) {
            Preference pref = GetPreferenceScreen()->GetPreference(i);
            if (! (pref instanceof SyncStateCheckBoxPreference)) {
                continue;
            }
            SyncStateCheckBoxPreference syncPref = (SyncStateCheckBoxPreference) pref;
            if (!syncPref->IsChecked()) {
                continue;
            }
            RequestOrCancelSync(syncPref->GetAccount(), syncPref->GetAuthority(), startSync);
        }
        // plus whatever the system needs to sync, e.g., invisible sync adapters
        if (mAccount != NULL) {
            for (SyncAdapterType syncAdapter : mInvisibleAdapters) {
                  RequestOrCancelSync(mAccount, syncAdapter.authority, startSync);
            }
        }
    }

    private void RequestOrCancelSync(Account account, String authority, Boolean flag) {
        if (flag) {
            Bundle extras = new Bundle();
            extras->PutBoolean(ContentResolver.SYNC_EXTRAS_MANUAL, TRUE);
            ContentResolver->RequestSyncAsUser(account, authority, mUserHandle->GetIdentifier(),
                    extras);
        } else {
            ContentResolver->CancelSyncAsUser(account, authority, mUserHandle->GetIdentifier());
        }
    }

    private Boolean IsSyncing(List<SyncInfo> currentSyncs, Account account, String authority) {
        for (SyncInfo syncInfo : currentSyncs) {
            if (syncInfo.account->Equals(account) && syncInfo.authority->Equals(authority)) {
                return TRUE;
            }
        }
        return FALSE;
    }

    //@Override
    protected void OnSyncStateUpdated() {
        if (!IsResumed()) return;
        SetFeedsState();
    }

    private void SetFeedsState() {
        // iterate over all the preferences, setting the state properly for each
        Date date = new Date();
        final Int32 userId = mUserHandle->GetIdentifier();
        List<SyncInfo> currentSyncs = ContentResolver->GetCurrentSyncsAsUser(userId);
        Boolean syncIsFailing = FALSE;

        // Refresh the sync status checkboxes - some syncs may have become active.
        UpdateAccountCheckboxes();

        for (Int32 i = 0, count = GetPreferenceScreen()->GetPreferenceCount(); i < count; i++) {
            Preference pref = GetPreferenceScreen()->GetPreference(i);
            if (! (pref instanceof SyncStateCheckBoxPreference)) {
                continue;
            }
            SyncStateCheckBoxPreference syncPref = (SyncStateCheckBoxPreference) pref;

            String authority = syncPref->GetAuthority();
            Account account = syncPref->GetAccount();

            SyncStatusInfo status = ContentResolver->GetSyncStatusAsUser(account, authority, userId);
            Boolean syncEnabled = ContentResolver->GetSyncAutomaticallyAsUser(account, authority,
                    userId);
            Boolean authorityIsPending = status == NULL ? FALSE : status.pending;
            Boolean initialSync = status == NULL ? FALSE : status.initialize;

            Boolean activelySyncing = IsSyncing(currentSyncs, account, authority);
            Boolean lastSyncFailed = status != NULL
                    && status.lastFailureTime != 0
                    && status->GetLastFailureMesgAsInt(0)
                       != ContentResolver.SYNC_ERROR_SYNC_ALREADY_IN_PROGRESS;
            if (!syncEnabled) lastSyncFailed = FALSE;
            if (lastSyncFailed && !activelySyncing && !authorityIsPending) {
                syncIsFailing = TRUE;
            }
            if (Log->IsLoggable(TAG, Log.VERBOSE)) {
                Logger::D(TAG, "Update sync status: " + account + " " + authority +
                        " active = " + activelySyncing + " pend =" +  authorityIsPending);
            }

            final Int64 successEndTime = (status == NULL) ? 0 : status.lastSuccessTime;
            if (!syncEnabled) {
                syncPref->SetSummary(R::string::sync_disabled);
            } else if (activelySyncing) {
                syncPref->SetSummary(R::string::sync_in_progress);
            } else if (successEndTime != 0) {
                date->SetTime(successEndTime);
                final String timeString = FormatSyncDate(date);
                syncPref->SetSummary(GetResources()->GetString(R::string::last_synced, timeString));
            } else {
                syncPref->SetSummary("");
            }
            Int32 syncState = ContentResolver->GetIsSyncableAsUser(account, authority, userId);

            syncPref->SetActive(activelySyncing && (syncState >= 0) &&
                    !initialSync);
            syncPref->SetPending(authorityIsPending && (syncState >= 0) &&
                    !initialSync);

            syncPref->SetFailed(lastSyncFailed);
            ConnectivityManager connManager =
                (ConnectivityManager) GetSystemService(Context.CONNECTIVITY_SERVICE);
            final Boolean masterSyncAutomatically =
                    ContentResolver->GetMasterSyncAutomaticallyAsUser(userId);
            final Boolean backgroundDataEnabled = connManager->GetBackgroundDataSetting();
            final Boolean oneTimeSyncMode = !masterSyncAutomatically || !backgroundDataEnabled;
            syncPref->SetOneTimeSyncMode(oneTimeSyncMode);
            syncPref->SetChecked(oneTimeSyncMode || syncEnabled);
        }
        mErrorInfoView->SetVisibility(syncIsFailing ? View.VISIBLE : View.GONE);
        GetActivity()->InvalidateOptionsMenu();
    }

    //@Override
    CARAPI OnAccountsUpdate(final UserHandle userHandle) {
        super->OnAccountsUpdate(userHandle);
        if (!AccountExists(mAccount)) {
            // The account was deleted
            Finish();
            return;
        }
        UpdateAccountCheckboxes();
        OnSyncStateUpdated();
    }

    private Boolean AccountExists(Account account) {
        if (account == NULL) return FALSE;

        Account[] accounts = AccountManager->Get(GetActivity()).GetAccountsByTypeAsUser(
                account.type, mUserHandle);
        final Int32 count = accounts.length;
        for (Int32 i = 0; i < count; i++) {
            if (accounts[i].Equals(account)) {
                return TRUE;
            }
        }
        return FALSE;
    }

    private void UpdateAccountCheckboxes() {
        mInvisibleAdapters->Clear();

        SyncAdapterType[] syncAdapters = ContentResolver->GetSyncAdapterTypesAsUser(
                mUserHandle->GetIdentifier());
        ArrayList<String> authorities = new ArrayList<String>();
        for (Int32 i = 0, n = syncAdapters.length; i < n; i++) {
            final SyncAdapterType sa = syncAdapters[i];
            // Only keep track of sync adapters for this account
            if (!sa.accountType->Equals(mAccount.type)) continue;
            if (sa->IsUserVisible()) {
                if (Log->IsLoggable(TAG, Log.VERBOSE)) {
                    Logger::D(TAG, "updateAccountCheckboxes: added authority " + sa.authority
                            + " to accountType " + sa.accountType);
                }
                authorities->Add(sa.authority);
            } else {
                // keep track of invisible sync adapters, so sync now forces
                // them to sync as well.
                mInvisibleAdapters->Add(sa);
            }
        }

        for (Int32 i = 0, n = mCheckBoxes->Size(); i < n; i++) {
            GetPreferenceScreen()->RemovePreference(mCheckBoxes->Get(i));
        }
        mCheckBoxes->Clear();

        if (Log->IsLoggable(TAG, Log.VERBOSE)) {
            Logger::D(TAG, "looking for sync adapters that match account " + mAccount);
        }
        for (Int32 j = 0, m = authorities->Size(); j < m; j++) {
            final String authority = authorities->Get(j);
            // We could check services here....
            Int32 syncState = ContentResolver->GetIsSyncableAsUser(mAccount, authority,
                    mUserHandle->GetIdentifier());
            if (Log->IsLoggable(TAG, Log.VERBOSE)) {
                Logger::D(TAG, "  found authority " + authority + " " + syncState);
            }
            if (syncState > 0) {
                AddSyncStateCheckBox(mAccount, authority);
            }
        }

        Collections->Sort(mCheckBoxes);
        for (Int32 i = 0, n = mCheckBoxes->Size(); i < n; i++) {
            GetPreferenceScreen()->AddPreference(mCheckBoxes->Get(i));
        }
    }

    /**
     * Updates the titlebar with an icon for the provider type.
     */
    //@Override
    protected void OnAuthDescriptionsUpdated() {
        super->OnAuthDescriptionsUpdated();
        GetPreferenceScreen()->RemoveAll();
        if (mAccount != NULL) {
            mProviderIcon->SetImageDrawable(GetDrawableForType(mAccount.type));
            mProviderId->SetText(GetLabelForType(mAccount.type));
        }
        AddPreferencesFromResource(R.xml.account_sync_settings);
    }

    //@Override
    protected Int32 GetHelpResource() {
        return R::string::help_url_accounts;
    }
}
