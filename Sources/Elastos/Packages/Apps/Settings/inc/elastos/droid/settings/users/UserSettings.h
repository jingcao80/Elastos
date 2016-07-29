/*
 * Copyright (C) 2012 The Android Open Source Project
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

package com.android.settings.users;

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Accounts::IAccountManager;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManagerNative;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IAsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::Preference::IOnPreferenceClickListener;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::Settings::ISecure;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::Widget::ISimpleAdapter;

using Elastos::Droid::Internal::Utility::IUserIcons;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Settings::IChooseLockGeneric;
using Elastos::Droid::Settings::IOwnerInfoSettings;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISelectableEditTextPreference;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::IUtils;
using Elastos::Droid::Settings::Drawable::ICircleFramedDrawable;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;

/**
 * Screen that manages the list of users on the device.
 * Guest user is an always visible entry, even if the guest is not currently
 * active/created. It is meant for controlling properties of a guest user.
 *
 * The first one is always the current user.
 * Owner is the primary user.
 */
public class UserSettings extends SettingsPreferenceFragment
        implements OnPreferenceClickListener, OnClickListener, DialogInterface.OnDismissListener,
        Preference.OnPreferenceChangeListener,
        EditUserInfoController.OnContentChangedCallback {

    private static const String TAG = "UserSettings";

    /** UserId of the user being removed */
    private static const String SAVE_REMOVING_USER = "removing_user";
    /** UserId of the user that was just added */
    private static const String SAVE_ADDING_USER = "adding_user";

    private static const String KEY_USER_LIST = "user_list";
    private static const String KEY_USER_ME = "user_me";
    private static const String KEY_ADD_USER = "user_add";

    private static const Int32 MENU_REMOVE_USER = Menu.FIRST;
    private static const Int32 MENU_ADD_ON_LOCKSCREEN = Menu.FIRST + 1;

    private static const Int32 DIALOG_CONFIRM_REMOVE = 1;
    private static const Int32 DIALOG_ADD_USER = 2;
    private static const Int32 DIALOG_SETUP_USER = 3;
    private static const Int32 DIALOG_SETUP_PROFILE = 4;
    private static const Int32 DIALOG_USER_CANNOT_MANAGE = 5;
    private static const Int32 DIALOG_CHOOSE_USER_TYPE = 6;
    private static const Int32 DIALOG_NEED_LOCKSCREEN = 7;
    private static const Int32 DIALOG_CONFIRM_EXIT_GUEST = 8;
    private static const Int32 DIALOG_USER_PROFILE_EDITOR = 9;

    private static const Int32 MESSAGE_UPDATE_LIST = 1;
    private static const Int32 MESSAGE_SETUP_USER = 2;
    private static const Int32 MESSAGE_CONFIG_USER = 3;

    private static const Int32 USER_TYPE_USER = 1;
    private static const Int32 USER_TYPE_RESTRICTED_PROFILE = 2;

    private static const Int32 REQUEST_CHOOSE_LOCK = 10;

    private static const String KEY_ADD_USER_LONG_MESSAGE_DISPLAYED =
            "key_add_user_long_message_displayed";

    private static const String KEY_TITLE = "title";
    private static const String KEY_SUMMARY = "summary";

    private PreferenceGroup mUserListCategory;
    private Preference mMePreference;
    private SelectableEditTextPreference mNicknamePreference;
    private Preference mAddUser;
    private Int32 mRemovingUserId = -1;
    private Int32 mAddedUserId = 0;
    private Boolean mAddingUser;
    private Boolean mEnabled = TRUE;
    private Boolean mCanAddRestrictedProfile = TRUE;

    private final Object mUserLock = new Object();
    private UserManager mUserManager;
    private SparseArray<Bitmap> mUserIcons = new SparseArray<Bitmap>();
    private Boolean mIsOwner = UserHandle->MyUserId() == UserHandle.USER_OWNER;
    private Boolean mIsGuest;

    private EditUserInfoController mEditUserInfoController =
            new EditUserInfoController();

    // A place to cache the generated default avatar
    private Drawable mDefaultIconDrawable;

    private Handler mHandler = new Handler() {
        //@Override
        CARAPI HandleMessage(Message msg) {
            switch (msg.what) {
            case MESSAGE_UPDATE_LIST:
                UpdateUserList();
                break;
            case MESSAGE_SETUP_USER:
                OnUserCreated(msg.arg1);
                break;
            case MESSAGE_CONFIG_USER:
                OnManageUserClicked(msg.arg1, TRUE);
                break;
            }
        }
    };

    private BroadcastReceiver mUserChangeReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            if (intent->GetAction()->Equals(IIntent::ACTION_USER_REMOVED)) {
                mRemovingUserId = -1;
            } else if (intent->GetAction()->Equals(IIntent::ACTION_USER_INFO_CHANGED)) {
                Int32 userHandle = intent->GetIntExtra(Intent.EXTRA_USER_HANDLE, -1);
                if (userHandle != -1) {
                    mUserIcons->Remove(userHandle);
                }
            }
            mHandler->SendEmptyMessage(MESSAGE_UPDATE_LIST);
        }
    };

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        if (icicle != NULL) {
            if (icicle->ContainsKey(SAVE_ADDING_USER)) {
                mAddedUserId = icicle->GetInt(SAVE_ADDING_USER);
            }
            if (icicle->ContainsKey(SAVE_REMOVING_USER)) {
                mRemovingUserId = icicle->GetInt(SAVE_REMOVING_USER);
            }
            mEditUserInfoController->OnRestoreInstanceState(icicle);
        }
        final Context context = GetActivity();
        mUserManager = (UserManager) context->GetSystemService(Context.USER_SERVICE);
        Boolean hasMultipleUsers = mUserManager->GetUserCount() > 1;
        if ((!UserManager->SupportsMultipleUsers() && !hasMultipleUsers)
                || Utils->IsMonkeyRunning()) {
            mEnabled = FALSE;
            return;
        }

        final Int32 myUserId = UserHandle->MyUserId();
        mIsGuest = mUserManager->GetUserInfo(myUserId).IsGuest();

        AddPreferencesFromResource(R.xml.user_settings);
        mUserListCategory = (PreferenceGroup) FindPreference(KEY_USER_LIST);
        mMePreference = new UserPreference(context, NULL /* attrs */, myUserId,
                NULL /* settings icon handler */,
                NULL /* delete icon handler */);
        mMePreference->SetKey(KEY_USER_ME);
        mMePreference->SetOnPreferenceClickListener(this);
        if (mIsOwner) {
            mMePreference->SetSummary(R::string::user_owner);
        }
        mAddUser = FindPreference(KEY_ADD_USER);
        if (!mIsOwner || UserManager->GetMaxSupportedUsers() < 2
                || !UserManager->SupportsMultipleUsers()
                || mUserManager->HasUserRestriction(UserManager.DISALLOW_ADD_USER)) {
            RemovePreference(KEY_ADD_USER);
        } else {
            mAddUser->SetOnPreferenceClickListener(this);
            DevicePolicyManager dpm = (DevicePolicyManager) context->GetSystemService(
                    Context.DEVICE_POLICY_SERVICE);
            // No restricted profiles for tablets with a device owner, or phones.
            if (dpm->GetDeviceOwner() != NULL || Utils->IsVoiceCapable(context)) {
                mCanAddRestrictedProfile = FALSE;
                mAddUser->SetTitle(R::string::user_add_user_menu);
            }
        }
        LoadProfile();
        SetHasOptionsMenu(TRUE);
        IntentFilter filter = new IntentFilter(IIntent::ACTION_USER_REMOVED);
        filter->AddAction(IIntent::ACTION_USER_INFO_CHANGED);
        context->RegisterReceiverAsUser(mUserChangeReceiver, UserHandle.ALL, filter, NULL,
                mHandler);
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();

        if (!mEnabled) return;

        LoadProfile();
        UpdateUserList();
    }

    //@Override
    CARAPI OnDestroy() {
        super->OnDestroy();

        if (!mEnabled) return;

        GetActivity()->UnregisterReceiver(mUserChangeReceiver);
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle outState) {
        super->OnSaveInstanceState(outState);
        mEditUserInfoController->OnSaveInstanceState(outState);
        outState->PutInt(SAVE_ADDING_USER, mAddedUserId);
        outState->PutInt(SAVE_REMOVING_USER, mRemovingUserId);
    }

    //@Override
    CARAPI StartActivityForResult(Intent intent, Int32 requestCode) {
        mEditUserInfoController->StartingActivityForResult();
        super->StartActivityForResult(intent, requestCode);
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        Int32 pos = 0;
        UserManager um = (UserManager) GetActivity()->GetSystemService(Context.USER_SERVICE);
        if (!mIsOwner && !um->HasUserRestriction(UserManager.DISALLOW_REMOVE_USER)) {
            String nickname = mUserManager->GetUserName();
            MenuItem removeThisUser = menu->Add(0, MENU_REMOVE_USER, pos++,
                    GetResources()->GetString(R::string::user_remove_user_menu, nickname));
            removeThisUser->SetShowAsAction(MenuItem.SHOW_AS_ACTION_NEVER);
        }
        if (mIsOwner && !um->HasUserRestriction(UserManager.DISALLOW_ADD_USER)) {
            MenuItem allowAddOnLockscreen = menu->Add(0, MENU_ADD_ON_LOCKSCREEN, pos++,
                    R::string::user_add_on_lockscreen_menu);
            allowAddOnLockscreen->SetCheckable(TRUE);
            allowAddOnLockscreen->SetChecked(Settings::Global::>GetInt(GetContentResolver(),
                    Settings::Global::ADD_USERS_WHEN_LOCKED, 0) == 1);
        }
        super->OnCreateOptionsMenu(menu, inflater);
    }

    //@Override
    public Boolean OnOptionsItemSelected(MenuItem item) {
        final Int32 itemId = item->GetItemId();
        if (itemId == MENU_REMOVE_USER) {
            OnRemoveUserClicked(UserHandle->MyUserId());
            return TRUE;
        } else if (itemId == MENU_ADD_ON_LOCKSCREEN) {
            final Boolean isChecked = item->IsChecked();
            Settings::Global::>PutInt(GetContentResolver(), Settings::Global::ADD_USERS_WHEN_LOCKED,
                    isChecked ? 0 : 1);
            item->SetChecked(!isChecked);
            return TRUE;
        } else {
            return super->OnOptionsItemSelected(item);
        }
    }

    /**
     * Loads profile information for the current user.
     */
    private void LoadProfile() {
        if (mIsGuest) {
            // No need to load profile information
            mMePreference->SetIcon(GetEncircledDefaultIcon());
            mMePreference->SetTitle(R::string::user_exit_guest_title);
            return;
        }

        new AsyncTask<Void, Void, String>() {
            //@Override
            protected void OnPostExecute(String result) {
                FinishLoadProfile(result);
            }

            //@Override
            protected String DoInBackground(Void... values) {
                UserInfo user = mUserManager->GetUserInfo(UserHandle->MyUserId());
                if (user.iconPath == NULL || user.iconPath->Equals("")) {
                    AssignProfilePhoto(user);
                }
                return user.name;
            }
        }.Execute();
    }

    private void FinishLoadProfile(String profileName) {
        if (GetActivity() == NULL) return;
        mMePreference->SetTitle(GetString(R::string::user_you, profileName));
        Int32 myUserId = UserHandle->MyUserId();
        Bitmap b = mUserManager->GetUserIcon(myUserId);
        if (b != NULL) {
            mMePreference->SetIcon(Encircle(b));
            mUserIcons->Put(myUserId, b);
        }
    }

    private Boolean HasLockscreenSecurity() {
        LockPatternUtils lpu = new LockPatternUtils(GetActivity());
        return lpu->IsLockPasswordEnabled() || lpu->IsLockPatternEnabled();
    }

    private void LaunchChooseLockscreen() {
        Intent chooseLockIntent = new Intent(DevicePolicyManager.ACTION_SET_NEW_PASSWORD);
        chooseLockIntent->PutExtra(CChooseLockGenericFragment::MINIMUM_QUALITY_KEY,
                DevicePolicyManager.PASSWORD_QUALITY_SOMETHING);
        StartActivityForResult(chooseLockIntent, REQUEST_CHOOSE_LOCK);
    }

    //@Override
    CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        super->OnActivityResult(requestCode, resultCode, data);

        if (requestCode == REQUEST_CHOOSE_LOCK) {
            if (resultCode != Activity.RESULT_CANCELED && HasLockscreenSecurity()) {
                AddUserNow(USER_TYPE_RESTRICTED_PROFILE);
            }
        } else {
            mEditUserInfoController->OnActivityResult(requestCode, resultCode, data);
        }
    }

    private void OnAddUserClicked(Int32 userType) {
        {    AutoLock syncLock(mUserLock);
            if (mRemovingUserId == -1 && !mAddingUser) {
                switch (userType) {
                case USER_TYPE_USER:
                    ShowDialog(DIALOG_ADD_USER);
                    break;
                case USER_TYPE_RESTRICTED_PROFILE:
                    if (HasLockscreenSecurity()) {
                        AddUserNow(USER_TYPE_RESTRICTED_PROFILE);
                    } else {
                        ShowDialog(DIALOG_NEED_LOCKSCREEN);
                    }
                    break;
                }
            }
        }
    }

    private void OnRemoveUserClicked(Int32 userId) {
        {    AutoLock syncLock(mUserLock);
            if (mRemovingUserId == -1 && !mAddingUser) {
                mRemovingUserId = userId;
                ShowDialog(DIALOG_CONFIRM_REMOVE);
            }
        }
    }

    private UserInfo CreateLimitedUser() {
        UserInfo newUserInfo = mUserManager->CreateSecondaryUser(
                GetResources()->GetString(R::string::user_new_profile_name),
                UserInfo.FLAG_RESTRICTED);
        Int32 userId = newUserInfo.id;
        UserHandle user = new UserHandle(userId);
        mUserManager->SetUserRestriction(UserManager.DISALLOW_MODIFY_ACCOUNTS, TRUE, user);
        // Change the setting before applying the DISALLOW_SHARE_LOCATION restriction, otherwise
        // the PutIntForUser() will fail.
        Secure->PutIntForUser(GetContentResolver(),
                Secure.LOCATION_MODE, Secure.LOCATION_MODE_OFF, userId);
        mUserManager->SetUserRestriction(UserManager.DISALLOW_SHARE_LOCATION, TRUE, user);
        AssignDefaultPhoto(newUserInfo);
        // Add shared accounts
        AccountManager am = AccountManager->Get(GetActivity());
        Account [] accounts = am->GetAccounts();
        if (accounts != NULL) {
            for (Account account : accounts) {
                am->AddSharedAccount(account, user);
            }
        }
        return newUserInfo;
    }

    private UserInfo CreateTrustedUser() {
        UserInfo newUserInfo = mUserManager->CreateSecondaryUser(
                GetResources()->GetString(R::string::user_new_user_name), 0);
        if (newUserInfo != NULL) {
            AssignDefaultPhoto(newUserInfo);
        }
        return newUserInfo;
    }

    private void OnManageUserClicked(Int32 userId, Boolean newUser) {
        if (userId == UserPreference.USERID_GUEST_DEFAULTS) {
            Bundle extras = new Bundle();
            extras->PutBoolean(UserDetailsSettings.EXTRA_USER_GUEST, TRUE);
            ((SettingsActivity) GetActivity()).StartPreferencePanel(
                    UserDetailsSettings.class->GetName(),
                    extras, R::string::user_guest, NULL, NULL, 0);
            return;
        }
        UserInfo info = mUserManager->GetUserInfo(userId);
        if (info->IsRestricted() && mIsOwner) {
            Bundle extras = new Bundle();
            extras->PutInt(RestrictedProfileSettings.EXTRA_USER_ID, userId);
            extras->PutBoolean(RestrictedProfileSettings.EXTRA_NEW_USER, newUser);
            ((SettingsActivity) GetActivity()).StartPreferencePanel(
                    RestrictedProfileSettings.class->GetName(),
                    extras, R::string::user_restrictions_title, NULL,
                    NULL, 0);
        } else if (info.id == UserHandle->MyUserId()) {
            // Jump to owner info panel
            Bundle extras = new Bundle();
            if (!info->IsRestricted()) {
                extras->PutBoolean(OwnerInfoSettings.EXTRA_SHOW_NICKNAME, TRUE);
            }
            Int32 titleResId = info.id == UserHandle.USER_OWNER ? R::string::owner_info_settings_title
                    : (info->IsRestricted() ? R::string::profile_info_settings_title
                            : R::string::user_info_settings_title);
            ((SettingsActivity) GetActivity()).StartPreferencePanel(
                    OwnerInfoSettings.class->GetName(),
                    extras, titleResId, NULL, NULL, 0);
        } else if (mIsOwner) {
            Bundle extras = new Bundle();
            extras->PutInt(UserDetailsSettings.EXTRA_USER_ID, userId);
            ((SettingsActivity) GetActivity()).StartPreferencePanel(
                    UserDetailsSettings.class->GetName(),
                    extras,
                    -1, /* No title res id */
                    info.name, /* title */
                    NULL, /* resultTo */
                    0 /* resultRequestCode */);
        }
    }

    private void OnUserCreated(Int32 userId) {
        mAddedUserId = userId;
        if (mUserManager->GetUserInfo(userId).IsRestricted()) {
            ShowDialog(DIALOG_SETUP_PROFILE);
        } else {
            ShowDialog(DIALOG_SETUP_USER);
        }
    }

    //@Override
    CARAPI OnDialogShowing() {
        super->OnDialogShowing();

        SetOnDismissListener(this);
    }

    //@Override
    public Dialog OnCreateDialog(Int32 dialogId) {
        Context context = GetActivity();
        if (context == NULL) return NULL;
        switch (dialogId) {
            case DIALOG_CONFIRM_REMOVE: {
                Dialog dlg =
                        Utils->CreateRemoveConfirmationDialog(GetActivity(), mRemovingUserId,
                                new DialogInterface->OnClickListener() {
                                    CARAPI OnClick(DialogInterface dialog, Int32 which) {
                                        RemoveUserNow();
                                    }
                                }
                        );
                return dlg;
            }
            case DIALOG_USER_CANNOT_MANAGE:
                return new AlertDialog->Builder(context)
                    .SetMessage(R::string::user_cannot_manage_message)
                    .SetPositiveButton(android.R::string::ok, NULL)
                    .Create();
            case DIALOG_ADD_USER: {
                final SharedPreferences preferences = GetActivity()->GetPreferences(
                        Context.MODE_PRIVATE);
                final Boolean longMessageDisplayed = preferences->GetBoolean(
                        KEY_ADD_USER_LONG_MESSAGE_DISPLAYED, FALSE);
                final Int32 messageResId = longMessageDisplayed
                        ? R::string::user_add_user_message_short
                        : R::string::user_add_user_message_long;
                final Int32 userType = dialogId == DIALOG_ADD_USER
                        ? USER_TYPE_USER : USER_TYPE_RESTRICTED_PROFILE;
                Dialog dlg = new AlertDialog->Builder(context)
                    .SetTitle(R::string::user_add_user_title)
                    .SetMessage(messageResId)
                    .SetPositiveButton(android.R::string::ok,
                        new DialogInterface->OnClickListener() {
                            CARAPI OnClick(DialogInterface dialog, Int32 which) {
                                AddUserNow(userType);
                                if (!longMessageDisplayed) {
                                    preferences->Edit()->PutBoolean(
                                            KEY_ADD_USER_LONG_MESSAGE_DISPLAYED, TRUE).Apply();
                                }
                            }
                    })
                    .SetNegativeButton(android.R::string::cancel, NULL)
                    .Create();
                return dlg;
            }
            case DIALOG_SETUP_USER: {
                Dialog dlg = new AlertDialog->Builder(context)
                    .SetTitle(R::string::user_setup_dialog_title)
                    .SetMessage(R::string::user_setup_dialog_message)
                    .SetPositiveButton(R::string::user_setup_button_setup_now,
                        new DialogInterface->OnClickListener() {
                            CARAPI OnClick(DialogInterface dialog, Int32 which) {
                                SwitchUserNow(mAddedUserId);
                            }
                    })
                    .SetNegativeButton(R::string::user_setup_button_setup_later, NULL)
                    .Create();
                return dlg;
            }
            case DIALOG_SETUP_PROFILE: {
                Dialog dlg = new AlertDialog->Builder(context)
                    .SetMessage(R::string::user_setup_profile_dialog_message)
                    .SetPositiveButton(android.R::string::ok,
                        new DialogInterface->OnClickListener() {
                            CARAPI OnClick(DialogInterface dialog, Int32 which) {
                                SwitchUserNow(mAddedUserId);
                            }
                    })
                    .SetNegativeButton(android.R::string::cancel, NULL)
                    .Create();
                return dlg;
            }
            case DIALOG_CHOOSE_USER_TYPE: {
                List<HashMap<String, String>> data = new ArrayList<HashMap<String,String>>();
                HashMap<String,String> addUserItem = new HashMap<String,String>();
                addUserItem->Put(KEY_TITLE, GetString(R::string::user_add_user_item_title));
                addUserItem->Put(KEY_SUMMARY, GetString(R::string::user_add_user_item_summary));
                HashMap<String,String> addProfileItem = new HashMap<String,String>();
                addProfileItem->Put(KEY_TITLE, GetString(R::string::user_add_profile_item_title));
                addProfileItem->Put(KEY_SUMMARY, GetString(R::string::user_add_profile_item_summary));
                data->Add(addUserItem);
                data->Add(addProfileItem);
                Dialog dlg = new AlertDialog->Builder(context)
                        .SetTitle(R::string::user_add_user_type_title)
                        .SetAdapter(new SimpleAdapter(context, data, R.layout.two_line_list_item,
                                new String[] {KEY_TITLE, KEY_SUMMARY},
                                new Int32[] {R.id.title, R.id.summary}),
                                new DialogInterface->OnClickListener() {
                                    CARAPI OnClick(DialogInterface dialog, Int32 which) {
                                        OnAddUserClicked(which == 0
                                                ? USER_TYPE_USER
                                                : USER_TYPE_RESTRICTED_PROFILE);
                                    }
                                })
                        .Create();
                return dlg;
            }
            case DIALOG_NEED_LOCKSCREEN: {
                Dialog dlg = new AlertDialog->Builder(context)
                        .SetMessage(R::string::user_need_lock_message)
                        .SetPositiveButton(R::string::user_set_lock_button,
                                new DialogInterface->OnClickListener() {
                                    //@Override
                                    CARAPI OnClick(DialogInterface dialog, Int32 which) {
                                        LaunchChooseLockscreen();
                                    }
                                })
                        .SetNegativeButton(android.R::string::cancel, NULL)
                        .Create();
                return dlg;
            }
            case DIALOG_CONFIRM_EXIT_GUEST: {
                Dialog dlg = new AlertDialog->Builder(context)
                        .SetTitle(R::string::user_exit_guest_confirm_title)
                        .SetMessage(R::string::user_exit_guest_confirm_message)
                        .SetPositiveButton(R::string::user_exit_guest_dialog_remove,
                                new DialogInterface->OnClickListener() {
                                    //@Override
                                    CARAPI OnClick(DialogInterface dialog, Int32 which) {
                                        ExitGuest();
                                    }
                                })
                        .SetNegativeButton(android.R::string::cancel, NULL)
                        .Create();
                return dlg;
            }
            case DIALOG_USER_PROFILE_EDITOR: {
                Dialog dlg = mEditUserInfoController->CreateDialog(
                        (Fragment) this,
                        mMePreference->GetIcon(),
                        mMePreference->GetTitle(),
                        R::string::profile_info_settings_title,
                        this /* callback */,
                        android.os.Process->MyUserHandle());
                return dlg;
            }
            default:
                return NULL;
        }
    }

    private void RemoveUserNow() {
        if (mRemovingUserId == UserHandle->MyUserId()) {
            RemoveThisUser();
        } else {
            new Thread() {
                CARAPI Run() {
                    {    AutoLock syncLock(mUserLock);
                        mUserManager->RemoveUser(mRemovingUserId);
                        mHandler->SendEmptyMessage(MESSAGE_UPDATE_LIST);
                    }
                }
            }.Start();
        }
    }

    private void RemoveThisUser() {
        try {
            ActivityManagerNative->GetDefault()->SwitchUser(UserHandle.USER_OWNER);
            ((UserManager) GetActivity()->GetSystemService(Context.USER_SERVICE))
                    .RemoveUser(UserHandle->MyUserId());
        } catch (RemoteException re) {
            Logger::E(TAG, "Unable to remove self user");
        }
    }

    private void AddUserNow(final Int32 userType) {
        {    AutoLock syncLock(mUserLock);
            mAddingUser = TRUE;
            //UpdateUserList();
            new Thread() {
                CARAPI Run() {
                    UserInfo user = NULL;
                    // Could take a few seconds
                    if (userType == USER_TYPE_USER) {
                        user = CreateTrustedUser();
                    } else {
                        user = CreateLimitedUser();
                    }
                    {    AutoLock syncLock(mUserLock);
                        mAddingUser = FALSE;
                        if (userType == USER_TYPE_USER) {
                            mHandler->SendEmptyMessage(MESSAGE_UPDATE_LIST);
                            mHandler->SendMessage(mHandler->ObtainMessage(
                                    MESSAGE_SETUP_USER, user.id, user.serialNumber));
                        } else {
                            mHandler->SendMessage(mHandler->ObtainMessage(
                                    MESSAGE_CONFIG_USER, user.id, user.serialNumber));
                        }
                    }
                }
            }.Start();
        }
    }

    private void SwitchUserNow(Int32 userId) {
        try {
            ActivityManagerNative->GetDefault()->SwitchUser(userId);
        } catch (RemoteException re) {
            // Nothing to do
        }
    }

    /**
     * Erase the current user (guest) and switch to another user.
     */
    private void ExitGuest() {
        // Just to be safe
        if (!mIsGuest) {
            return;
        }
        RemoveThisUser();
    }

    private void UpdateUserList() {
        if (GetActivity() == NULL) return;
        List<UserInfo> users = mUserManager->GetUsers(TRUE);
        final Context context = GetActivity();

        mUserListCategory->RemoveAll();
        mUserListCategory->SetOrderingAsAdded(FALSE);
        mUserListCategory->AddPreference(mMePreference);

        final Boolean voiceCapable = Utils->IsVoiceCapable(context);
        final ArrayList<Integer> missingIcons = new ArrayList<Integer>();
        for (UserInfo user : users) {
            if (user->IsManagedProfile()) {
                // Managed profiles appear under Accounts Settings instead
                continue;
            }
            Preference pref;
            if (user.id == UserHandle->MyUserId()) {
                pref = mMePreference;
            } else if (user->IsGuest()) {
                // Skip over Guest. We add generic Guest settings after this loop
                continue;
            } else {
                // With Telephony:
                //   Secondary user: Settings
                //   Guest: Settings
                //   Restricted Profile: There is no Restricted Profile
                // Without Telephony:
                //   Secondary user: Delete
                //   Guest: Nothing
                //   Restricted Profile: Settings
                final Boolean showSettings = mIsOwner && (voiceCapable || user->IsRestricted());
                final Boolean showDelete = mIsOwner
                        && (!voiceCapable && !user->IsRestricted() && !user->IsGuest());
                pref = new UserPreference(context, NULL, user.id,
                        showSettings ? this : NULL,
                        showDelete ? this : NULL);
                pref->SetOnPreferenceClickListener(this);
                pref->SetKey("id=" + user.id);
                mUserListCategory->AddPreference(pref);
                if (user.id == UserHandle.USER_OWNER) {
                    pref->SetSummary(R::string::user_owner);
                }
                pref->SetTitle(user.name);
            }
            if (!IsInitialized(user)) {
                if (user->IsRestricted()) {
                    pref->SetSummary(R::string::user_summary_restricted_not_set_up);
                } else {
                    pref->SetSummary(R::string::user_summary_not_set_up);
                }
            } else if (user->IsRestricted()) {
                pref->SetSummary(R::string::user_summary_restricted_profile);
            }
            if (user.iconPath != NULL) {
                if (mUserIcons->Get(user.id) == NULL) {
                    // Icon not loaded yet, print a placeholder
                    missingIcons->Add(user.id);
                    pref->SetIcon(GetEncircledDefaultIcon());
                } else {
                    SetPhotoId(pref, user);
                }
            } else {
                // Icon not available yet, print a placeholder
                pref->SetIcon(GetEncircledDefaultIcon());
            }
        }

        // Add a temporary entry for the user being created
        if (mAddingUser) {
            Preference pref = new UserPreference(GetActivity(), NULL, UserPreference.USERID_UNKNOWN,
                    NULL, NULL);
            pref->SetEnabled(FALSE);
            pref->SetTitle(R::string::user_new_user_name);
            pref->SetIcon(GetEncircledDefaultIcon());
            mUserListCategory->AddPreference(pref);
        }

        Boolean showGuestPreference = !mIsGuest;
        // If user has DISALLOW_ADD_USER don't allow creating a guest either.
        if (showGuestPreference && mUserManager->HasUserRestriction(UserManager.DISALLOW_ADD_USER)) {
            showGuestPreference = FALSE;
            // If guest already exists, no user creation needed.
            for (UserInfo user : users) {
                if (user->IsGuest()) {
                    showGuestPreference = TRUE;
                    break;
                }
            }
        }
        if (showGuestPreference) {
            // Add a virtual Guest user for guest defaults
            Preference pref = new UserPreference(GetActivity(), NULL,
                    UserPreference.USERID_GUEST_DEFAULTS,
                    mIsOwner && voiceCapable? this : NULL /* settings icon handler */,
                    NULL /* delete icon handler */);
            pref->SetTitle(R::string::user_guest);
            pref->SetIcon(GetEncircledDefaultIcon());
            pref->SetOnPreferenceClickListener(this);
            mUserListCategory->AddPreference(pref);
        }

        GetActivity()->InvalidateOptionsMenu();

        // Load the icons
        if (missingIcons->Size() > 0) {
            LoadIconsAsync(missingIcons);
        }
        Boolean moreUsers = mUserManager->CanAddMoreUsers();
        mAddUser->SetEnabled(moreUsers);
    }

    private void LoadIconsAsync(List<Integer> missingIcons) {
        final Resources resources = GetResources();
        new AsyncTask<List<Integer>, Void, Void>() {
            //@Override
            protected void OnPostExecute(Void result) {
                UpdateUserList();
            }

            //@Override
            protected Void DoInBackground(List<Integer>... values) {
                for (Int32 userId : values[0]) {
                    Bitmap bitmap = mUserManager->GetUserIcon(userId);
                    if (bitmap == NULL) {
                        bitmap = UserIcons->ConvertToBitmap(UserIcons->GetDefaultUserIcon(userId,
                                /* light= */ FALSE));
                    }
                    mUserIcons->Append(userId, bitmap);
                }
                return NULL;
            }
        }.Execute(missingIcons);
    }

    private void AssignProfilePhoto(final UserInfo user) {
        if (!Utils->CopyMeProfilePhoto(GetActivity(), user)) {
            AssignDefaultPhoto(user);
        }
    }

    private void AssignDefaultPhoto(UserInfo user) {
        Bitmap bitmap = UserIcons->ConvertToBitmap(UserIcons->GetDefaultUserIcon(user.id,
                /* light= */ FALSE));
        mUserManager->SetUserIcon(user.id, bitmap);
    }

    private Drawable GetEncircledDefaultIcon() {
        if (mDefaultIconDrawable == NULL) {
            mDefaultIconDrawable = Encircle(UserIcons->ConvertToBitmap(
                    UserIcons->GetDefaultUserIcon(UserHandle.USER_NULL, /* light= */ FALSE)));
        }
        return mDefaultIconDrawable;
    }

    private void SetPhotoId(Preference pref, UserInfo user) {
        Bitmap bitmap = mUserIcons->Get(user.id);
        if (bitmap != NULL) {
            pref->SetIcon(Encircle(bitmap));
        }
    }

    private void SetUserName(String name) {
        mUserManager->SetUserName(UserHandle->MyUserId(), name);
        mNicknamePreference->SetSummary(name);
        GetActivity()->InvalidateOptionsMenu();
    }

    //@Override
    public Boolean OnPreferenceClick(Preference pref) {
        if (pref == mMePreference) {
            if (mIsGuest) {
                ShowDialog(DIALOG_CONFIRM_EXIT_GUEST);
                return TRUE;
            }
            // If this is a limited user, launch the user info settings instead of profile editor
            if (mUserManager->IsLinkedUser()) {
                OnManageUserClicked(UserHandle->MyUserId(), FALSE);
            } else {
                ShowDialog(DIALOG_USER_PROFILE_EDITOR);
            }
        } else if (pref instanceof UserPreference) {
            Int32 userId = ((UserPreference) pref).GetUserId();
            if (userId == UserPreference.USERID_GUEST_DEFAULTS) {
                CreateAndSwitchToGuestUser();
            } else {
                // Get the latest status of the user
                UserInfo user = mUserManager->GetUserInfo(userId);
                if (!IsInitialized(user)) {
                    mHandler->SendMessage(mHandler->ObtainMessage(
                            MESSAGE_SETUP_USER, user.id, user.serialNumber));
                } else {
                    SwitchUserNow(userId);
                }
            }
        } else if (pref == mAddUser) {
            // If we allow both types, show a picker, otherwise directly go to
            // flow for full user.
            if (mCanAddRestrictedProfile) {
                ShowDialog(DIALOG_CHOOSE_USER_TYPE);
            } else {
                OnAddUserClicked(USER_TYPE_USER);
            }
        }
        return FALSE;
    }

    private void CreateAndSwitchToGuestUser() {
        List<UserInfo> users = mUserManager->GetUsers();
        for (UserInfo user : users) {
            if (user->IsGuest()) {
                SwitchUserNow(user.id);
                return;
            }
        }
        // No guest user. Create one, if there's no restriction.
        // If it is not the primary user, then adding users from lockscreen must be enabled
        if (mUserManager->HasUserRestriction(UserManager.DISALLOW_ADD_USER)
                || (!mIsOwner && Settings::Global::>GetInt(GetContentResolver(),
                        Settings::Global::ADD_USERS_WHEN_LOCKED, 0) != 1)) {
            Logger::I(TAG, "Blocking guest creation because it is restricted");
            return;
        }
        UserInfo guestUser = mUserManager->CreateGuest(GetActivity(),
                    GetResources()->GetString(R::string::user_guest));
        if (guestUser != NULL) {
            SwitchUserNow(guestUser.id);
        }
    }

    private Boolean IsInitialized(UserInfo user) {
        return (user.flags & UserInfo.FLAG_INITIALIZED) != 0;
    }

    private Drawable Encircle(Bitmap icon) {
        Drawable circled = CircleFramedDrawable->GetInstance(GetActivity(), icon);
        return circled;
    }

    //@Override
    CARAPI OnClick(View v) {
        if (v->GetTag() instanceof UserPreference) {
            Int32 userId = ((UserPreference) v->GetTag()).GetUserId();
            switch (v->GetId()) {
            case UserPreference.DELETE_ID:
                OnRemoveUserClicked(userId);
                break;
            case UserPreference.SETTINGS_ID:
                OnManageUserClicked(userId, FALSE);
                break;
            }
        }
    }

    //@Override
    CARAPI OnDismiss(DialogInterface dialog) {
        {    AutoLock syncLock(mUserLock);
            mAddingUser = FALSE;
            mRemovingUserId = -1;
            UpdateUserList();
        }
    }

    //@Override
    public Boolean OnPreferenceChange(Preference preference, Object newValue) {
        if (preference == mNicknamePreference) {
            String value = (String) newValue;
            if (preference == mNicknamePreference && value != NULL
                    && value->Length() > 0) {
                SetUserName(value);
            }
            return TRUE;
        }
        return FALSE;
    }

    //@Override
    public Int32 GetHelpResource() {
        return R::string::help_url_users;
    }

    //@Override
    CARAPI OnPhotoChanged(Drawable photo) {
        mMePreference->SetIcon(photo);
    }

    //@Override
    CARAPI OnLabelChanged(CharSequence label) {
        mMePreference->SetTitle(label);
    }
}
