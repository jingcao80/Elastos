

package com.android.settings.accounts;


using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Accounts::IAccountManager;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::Preference::IOnPreferenceClickListener;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::IPreferenceScreen;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::IUtils;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IComparator;
using Elastos::Utility::IList;

using static android::Content::Intent::IEXTRA_USER;
using static android::Os::UserManager::IDISALLOW_MODIFY_ACCOUNTS;
using static android::Provider::Settings::IEXTRA_AUTHORITIES;

/**
 * Settings screen for the account types on the device.
 * This shows all account types available for personal and work profiles.
 *
 * An extra {@link UserHandle} can be specified in the intent as {@link EXTRA_USER}, if the user for
 * which the action needs to be performed is different to the one the Settings App will run in.
 */
public class AccountSettings extends SettingsPreferenceFragment
        implements AuthenticatorHelper.OnAccountsUpdateListener,
        OnPreferenceClickListener {
    public static const String TAG = "AccountSettings";

    private static const String KEY_ACCOUNT = "account";

    private static const String ADD_ACCOUNT_ACTION = "android.settings.ADD_ACCOUNT_SETTINGS";
    private static const String TAG_CONFIRM_AUTO_SYNC_CHANGE = "confirmAutoSyncChange";

    private static const Int32 ORDER_LAST = 1001;
    private static const Int32 ORDER_NEXT_TO_LAST = 1000;

    private UserManager mUm;
    private SparseArray<ProfileData> mProfiles = new SparseArray<ProfileData>();
    private ManagedProfileBroadcastReceiver mManagedProfileBroadcastReceiver
                = new ManagedProfileBroadcastReceiver();
    private Preference mProfileNotAvailablePreference;
    private String[] mAuthorities;
    private Int32 mAuthoritiesCount = 0;

    /**
     * Holds data related to the accounts belonging to one profile.
     */
    private static class ProfileData {
        /**
         * The preference that displays the accounts.
         */
        public PreferenceGroup preferenceGroup;
        /**
         * The preference that displays the add account button.
         */
        public Preference addAccountPreference;
        /**
         * The preference that displays the button to remove the managed profile
         */
        public Preference removeWorkProfilePreference;
        /**
         * The {@link AuthenticatorHelper} that holds accounts data for this profile.
         */
        public AuthenticatorHelper authenticatorHelper;
        /**
         * The {@link UserInfo} of the profile.
         */
        public UserInfo userInfo;
    }

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        mUm = (UserManager) GetSystemService(Context.USER_SERVICE);
        mProfileNotAvailablePreference = new Preference(GetActivity());
        mAuthorities = GetActivity()->GetIntent().GetStringArrayExtra(EXTRA_AUTHORITIES);
        if (mAuthorities != NULL) {
            mAuthoritiesCount = mAuthorities.length;
        }
        SetHasOptionsMenu(TRUE);
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        inflater->Inflate(R.menu.account_settings, menu);
        super->OnCreateOptionsMenu(menu, inflater);
    }

    //@Override
    CARAPI OnPrepareOptionsMenu(Menu menu) {
        final UserHandle currentProfile = Process->MyUserHandle();
        if (mProfiles->Size() == 1) {
            menu->FindItem(R.id.account_settings_menu_auto_sync)
                    .SetVisible(TRUE)
                    .SetOnMenuItemClickListener(new MasterSyncStateClickListener(currentProfile))
                    .SetChecked(ContentResolver->GetMasterSyncAutomaticallyAsUser(
                            currentProfile->GetIdentifier()));
            menu->FindItem(R.id.account_settings_menu_auto_sync_personal).SetVisible(FALSE);
            menu->FindItem(R.id.account_settings_menu_auto_sync_work).SetVisible(FALSE);
        } else if (mProfiles->Size() > 1) {
            // We assume there's only one managed profile, otherwise UI needs to change
            final UserHandle managedProfile = mProfiles->ValueAt(1).userInfo->GetUserHandle();

            menu->FindItem(R.id.account_settings_menu_auto_sync_personal)
                    .SetVisible(TRUE)
                    .SetOnMenuItemClickListener(new MasterSyncStateClickListener(currentProfile))
                    .SetChecked(ContentResolver->GetMasterSyncAutomaticallyAsUser(
                            currentProfile->GetIdentifier()));
            menu->FindItem(R.id.account_settings_menu_auto_sync_work)
                    .SetVisible(TRUE)
                    .SetOnMenuItemClickListener(new MasterSyncStateClickListener(managedProfile))
                    .SetChecked(ContentResolver->GetMasterSyncAutomaticallyAsUser(
                            managedProfile->GetIdentifier()));
            menu->FindItem(R.id.account_settings_menu_auto_sync).SetVisible(FALSE);
         } else {
             Logger::W(TAG, "Method onPrepareOptionsMenu called before mProfiles was initialized");
         }
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        UpdateUi();
        mManagedProfileBroadcastReceiver->Register(GetActivity());
        ListenToAccountUpdates();
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        StopListeningToAccountUpdates();
        mManagedProfileBroadcastReceiver->Unregister(GetActivity());
        CleanUpPreferences();
    }

    //@Override
    CARAPI OnAccountsUpdate(UserHandle userHandle) {
        final ProfileData profileData = mProfiles->Get(userHandle->GetIdentifier());
        if (profileData != NULL) {
            UpdateAccountTypes(profileData);
        } else {
            Logger::W(TAG, "Missing Settings screen for: " + userHandle->GetIdentifier());
        }
    }

    //@Override
    public Boolean OnPreferenceClick(Preference preference) {
        // Check the preference
        final Int32 count = mProfiles->Size();
        for (Int32 i = 0; i < count; i++) {
            ProfileData profileData = mProfiles->ValueAt(i);
            if (preference == profileData.addAccountPreference) {
                Intent intent = new Intent(ADD_ACCOUNT_ACTION);
                intent->PutExtra(EXTRA_USER, profileData.userInfo->GetUserHandle());
                intent->PutExtra(EXTRA_AUTHORITIES, mAuthorities);
                StartActivity(intent);
                return TRUE;
            }
            if (preference == profileData.removeWorkProfilePreference) {
                final Int32 userId = profileData.userInfo.id;
                Utils->CreateRemoveConfirmationDialog(GetActivity(), userId,
                        new DialogInterface->OnClickListener() {
                            //@Override
                            CARAPI OnClick(DialogInterface dialog, Int32 which) {
                                mUm->RemoveUser(userId);
                            }
                        }
                ).Show();
                return TRUE;
            }
        }
        return FALSE;
    }

    void UpdateUi() {
        // Load the preferences from an XML resource
        AddPreferencesFromResource(R.xml.account_settings);

        if (Utils->IsManagedProfile(mUm)) {
            // This should not happen
            Logger::E(TAG, "We should not be showing settings for a managed profile");
            Finish();
            return;
        }

        final PreferenceScreen preferenceScreen = (PreferenceScreen) FindPreference(KEY_ACCOUNT);
        If(mUm->IsLinkedUser()) {
            // Restricted user or similar
            UserInfo userInfo = mUm->GetUserInfo(UserHandle->MyUserId());
            UpdateProfileUi(userInfo, FALSE /* no category needed */, preferenceScreen);
        } else {
            List<UserInfo> profiles = mUm->GetProfiles(UserHandle->MyUserId());
            final Int32 profilesCount = profiles->Size();
            final Boolean addCategory = profilesCount > 1;
            for (Int32 i = 0; i < profilesCount; i++) {
                UpdateProfileUi(profiles->Get(i), addCategory, preferenceScreen);
            }
        }

        // Add all preferences, starting with one for the primary profile.
        // Note that we're relying on the ordering given by the SparseArray keys, and on the
        // value of UserHandle.USER_OWNER being smaller than all the rest.
        final Int32 profilesCount = mProfiles->Size();
        for (Int32 i = 0; i < profilesCount; i++) {
            ProfileData profileData = mProfiles->ValueAt(i);
            if (!profileData.preferenceGroup->Equals(preferenceScreen)) {
                preferenceScreen->AddPreference(profileData.preferenceGroup);
            }
            UpdateAccountTypes(profileData);
        }
    }

    private void UpdateProfileUi(final UserInfo userInfo, Boolean addCategory,
            PreferenceScreen parent) {
        final Context context = GetActivity();
        final ProfileData profileData = new ProfileData();
        profileData.userInfo = userInfo;
        if (addCategory) {
            profileData.preferenceGroup = new PreferenceCategory(context);
            profileData.preferenceGroup->SetTitle(userInfo->IsManagedProfile()
                    ? R::string::category_work : R::string::category_personal);
            parent->AddPreference(profileData.preferenceGroup);
        } else {
            profileData.preferenceGroup = parent;
        }
        if (userInfo->IsEnabled()) {
            profileData.authenticatorHelper = new AuthenticatorHelper(context,
                    userInfo->GetUserHandle(), mUm, this);
            if (!mUm->HasUserRestriction(DISALLOW_MODIFY_ACCOUNTS, userInfo->GetUserHandle())) {
                profileData.addAccountPreference = NewAddAccountPreference(context);
            }
        }
        if (userInfo->IsManagedProfile()) {
            profileData.removeWorkProfilePreference = NewRemoveWorkProfilePreference(context);
        }
        mProfiles->Put(userInfo.id, profileData);
    }

    private Preference NewAddAccountPreference(Context context) {
        Preference preference = new Preference(context);
        preference->SetTitle(R::string::add_account_label);
        preference->SetIcon(R.drawable.ic_menu_add_dark);
        preference->SetOnPreferenceClickListener(this);
        preference->SetOrder(ORDER_NEXT_TO_LAST);
        return preference;
    }

    private Preference NewRemoveWorkProfilePreference(Context context) {
        Preference preference = new Preference(context);
        preference->SetTitle(R::string::remove_managed_profile_label);
        preference->SetIcon(R.drawable.ic_menu_delete);
        preference->SetOnPreferenceClickListener(this);
        preference->SetOrder(ORDER_LAST);
        return preference;
    }

    private void CleanUpPreferences() {
        PreferenceScreen preferenceScreen = GetPreferenceScreen();
        if (preferenceScreen != NULL) {
            preferenceScreen->RemoveAll();
        }
        mProfiles->Clear();
    }

    private void ListenToAccountUpdates() {
        final Int32 count = mProfiles->Size();
        for (Int32 i = 0; i < count; i++) {
            AuthenticatorHelper authenticatorHelper = mProfiles->ValueAt(i).authenticatorHelper;
            if (authenticatorHelper != NULL) {
                authenticatorHelper->ListenToAccountUpdates();
            }
        }
    }

    private void StopListeningToAccountUpdates() {
        final Int32 count = mProfiles->Size();
        for (Int32 i = 0; i < count; i++) {
            AuthenticatorHelper authenticatorHelper = mProfiles->ValueAt(i).authenticatorHelper;
            if (authenticatorHelper != NULL) {
                authenticatorHelper->StopListeningToAccountUpdates();
            }
        }
    }

    private void UpdateAccountTypes(ProfileData profileData) {
        profileData.preferenceGroup->RemoveAll();
        if (profileData.userInfo->IsEnabled()) {
            final ArrayList<AccountPreference> preferences = GetAccountTypePreferences(
                    profileData.authenticatorHelper, profileData.userInfo->GetUserHandle());
            final Int32 count = preferences->Size();
            for (Int32 i = 0; i < count; i++) {
                profileData.preferenceGroup->AddPreference(preferences->Get(i));
            }
            if (profileData.addAccountPreference != NULL) {
                profileData.preferenceGroup->AddPreference(profileData.addAccountPreference);
            }
        } else {
            // Put a label instead of the accounts list
            mProfileNotAvailablePreference->SetEnabled(FALSE);
            mProfileNotAvailablePreference->SetIcon(R.drawable.empty_icon);
            mProfileNotAvailablePreference->SetTitle(NULL);
            mProfileNotAvailablePreference->SetSummary(
                    R::string::managed_profile_not_available_label);
            profileData.preferenceGroup->AddPreference(mProfileNotAvailablePreference);
        }
        if (profileData.removeWorkProfilePreference != NULL) {
            profileData.preferenceGroup->AddPreference(profileData.removeWorkProfilePreference);
        }
    }

    private ArrayList<AccountPreference> GetAccountTypePreferences(AuthenticatorHelper helper,
            UserHandle userHandle) {
        final String[] accountTypes = helper->GetEnabledAccountTypes();
        final ArrayList<AccountPreference> accountTypePreferences =
                new ArrayList<AccountPreference>(accountTypes.length);

        for (Int32 i = 0; i < accountTypes.length; i++) {
            final String accountType = accountTypes[i];
            // Skip showing any account that does not have any of the requested authorities
            if (!AccountTypeHasAnyRequestedAuthorities(helper, accountType)) {
                continue;
            }
            final CharSequence label = helper->GetLabelForType(GetActivity(), accountType);
            if (label == NULL) {
                continue;
            }

            final Account[] accounts = AccountManager->Get(GetActivity())
                    .GetAccountsByTypeAsUser(accountType, userHandle);
            final Boolean skipToAccount = accounts.length == 1
                    && !helper->HasAccountPreferences(accountType);

            if (skipToAccount) {
                final Bundle fragmentArguments = new Bundle();
                fragmentArguments->PutParcelable(AccountSyncSettings.ACCOUNT_KEY,
                        accounts[0]);
                fragmentArguments->PutParcelable(EXTRA_USER, userHandle);

                accountTypePreferences->Add(new AccountPreference(GetActivity(), label,
                        AccountSyncSettings.class->GetName(), fragmentArguments,
                        helper->GetDrawableForType(GetActivity(), accountType)));
            } else {
                final Bundle fragmentArguments = new Bundle();
                fragmentArguments->PutString(ManageAccountsSettings.KEY_ACCOUNT_TYPE, accountType);
                fragmentArguments->PutString(ManageAccountsSettings.KEY_ACCOUNT_LABEL,
                        label->ToString());
                fragmentArguments->PutParcelable(EXTRA_USER, userHandle);

                accountTypePreferences->Add(new AccountPreference(GetActivity(), label,
                        ManageAccountsSettings.class->GetName(), fragmentArguments,
                        helper->GetDrawableForType(GetActivity(), accountType)));
            }
            helper->PreloadDrawableForType(GetActivity(), accountType);
        }
        // Sort by label
        Collections->Sort(accountTypePreferences, new Comparator<AccountPreference>() {
            //@Override
            public Int32 Compare(AccountPreference t1, AccountPreference t2) {
                return t1.mTitle->ToString()->CompareTo(t2.mTitle->ToString());
            }
        });
        return accountTypePreferences;
    }

    private Boolean AccountTypeHasAnyRequestedAuthorities(AuthenticatorHelper helper,
            String accountType) {
        if (mAuthoritiesCount == 0) {
            // No authorities required
            return TRUE;
        }
        final ArrayList<String> authoritiesForType = helper->GetAuthoritiesForAccountType(
                accountType);
        if (authoritiesForType == NULL) {
            Logger::D(TAG, "No sync authorities for account type: " + accountType);
            return FALSE;
        }
        for (Int32 j = 0; j < mAuthoritiesCount; j++) {
            if (authoritiesForType->Contains(mAuthorities[j])) {
                return TRUE;
            }
        }
        return FALSE;
    }

    private class AccountPreference extends Preference implements OnPreferenceClickListener {
        /**
         * Title of the tile that is shown to the user.
         * @attr ref android.R.styleable#PreferenceHeader_title
         */
        private final CharSequence mTitle;

        /**
         * Full class name of the fragment to display when this tile is
         * selected.
         * @attr ref android.R.styleable#PreferenceHeader_fragment
         */
        private final String mFragment;

        /**
         * Optional arguments to supply to the fragment when it is
         * instantiated.
         */
        private final Bundle mFragmentArguments;

        public AccountPreference(Context context, CharSequence title, String fragment,
                Bundle fragmentArguments, Drawable icon) {
            Super(context);
            mTitle = title;
            mFragment = fragment;
            mFragmentArguments = fragmentArguments;
            SetWidgetLayoutResource(R.layout.account_type_preference);

            SetTitle(title);
            SetIcon(icon);

            SetOnPreferenceClickListener(this);
        }

        //@Override
        public Boolean OnPreferenceClick(Preference preference) {
            if (mFragment != NULL) {
                Utils->StartWithFragment(
                        GetContext(), mFragment, mFragmentArguments, NULL, 0, 0, mTitle);
                return TRUE;
            }
            return FALSE;
        }
    }

    private class ManagedProfileBroadcastReceiver extends BroadcastReceiver {
        private Boolean listeningToManagedProfileEvents;

        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            if (intent->GetAction()->Equals(IIntent::ACTION_MANAGED_PROFILE_REMOVED)
                    || intent->GetAction()->Equals(IIntent::ACTION_MANAGED_PROFILE_ADDED)) {
                Logger::V(TAG, "Received broadcast: " + intent->GetAction());
                // Clean old state
                StopListeningToAccountUpdates();
                CleanUpPreferences();
                // Build new state
                UpdateUi();
                ListenToAccountUpdates();
                // Force the menu to update. Note that #onPrepareOptionsMenu uses data built by
                // #updateUi so we must call this later
                GetActivity()->InvalidateOptionsMenu();
                return;
            }
            Logger::W(TAG, "Cannot handle received broadcast: " + intent->GetAction());
        }

        CARAPI Register(Context context) {
            if (!listeningToManagedProfileEvents) {
                IntentFilter intentFilter = new IntentFilter();
                intentFilter->AddAction(IIntent::ACTION_MANAGED_PROFILE_REMOVED);
                intentFilter->AddAction(IIntent::ACTION_MANAGED_PROFILE_ADDED);
                context->RegisterReceiver(this, intentFilter);
                listeningToManagedProfileEvents = TRUE;
            }
        }

        CARAPI Unregister(Context context) {
            if (listeningToManagedProfileEvents) {
                context->UnregisterReceiver(this);
                listeningToManagedProfileEvents = FALSE;
            }
        }
    }

    private class MasterSyncStateClickListener implements MenuItem.OnMenuItemClickListener {
        private final UserHandle mUserHandle;

        public MasterSyncStateClickListener(UserHandle userHandle) {
            mUserHandle = userHandle;
        }

        //@Override
        public Boolean OnMenuItemClick(MenuItem item) {
            if (ActivityManager->IsUserAMonkey()) {
                Logger::D(TAG, "ignoring monkey's attempt to flip sync state");
            } else {
                ConfirmAutoSyncChangeFragment->Show(AccountSettings.this, !item->IsChecked(),
                        mUserHandle);
            }
            return TRUE;
        }
    }

    /**
     * Dialog to inform user about changing auto-sync setting
     */
    public static class ConfirmAutoSyncChangeFragment extends DialogFragment {
        private static const String SAVE_ENABLING = "enabling";
        private Boolean mEnabling;
        private UserHandle mUserHandle;

        public static void Show(AccountSettings parent, Boolean enabling, UserHandle userHandle) {
            if (!parent->IsAdded()) return;

            final ConfirmAutoSyncChangeFragment dialog = new ConfirmAutoSyncChangeFragment();
            dialog.mEnabling = enabling;
            dialog.mUserHandle = userHandle;
            dialog->SetTargetFragment(parent, 0);
            dialog->Show(parent->GetFragmentManager(), TAG_CONFIRM_AUTO_SYNC_CHANGE);
        }

        //@Override
        public Dialog OnCreateDialog(Bundle savedInstanceState) {
            final Context context = GetActivity();
            if (savedInstanceState != NULL) {
                mEnabling = savedInstanceState->GetBoolean(SAVE_ENABLING);
            }

            final AlertDialog.Builder builder = new AlertDialog->Builder(context);
            if (!mEnabling) {
                builder->SetTitle(R::string::data_usage_auto_sync_off_dialog_title);
                builder->SetMessage(R::string::data_usage_auto_sync_off_dialog);
            } else {
                builder->SetTitle(R::string::data_usage_auto_sync_on_dialog_title);
                builder->SetMessage(R::string::data_usage_auto_sync_on_dialog);
            }

            builder->SetPositiveButton(android.R::string::ok, new DialogInterface->OnClickListener() {
                //@Override
                CARAPI OnClick(DialogInterface dialog, Int32 which) {
                    ContentResolver->SetMasterSyncAutomaticallyAsUser(mEnabling,
                            mUserHandle->GetIdentifier());
                }
            });
            builder->SetNegativeButton(android.R::string::cancel, NULL);

            return builder->Create();
        }

        //@Override
        CARAPI OnSaveInstanceState(Bundle outState) {
            super->OnSaveInstanceState(outState);
            outState->PutBoolean(SAVE_ENABLING, mEnabling);
        }
    }
    // TODO Implement a {@link SearchIndexProvider} to allow Indexing and Search of account types
    // See http://b/15403806
}
