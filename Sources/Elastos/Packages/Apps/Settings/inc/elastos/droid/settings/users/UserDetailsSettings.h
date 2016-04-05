

package com.android.settings.users;

using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::ISwitchPreference;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::IUtils;

using Elastos::Utility::IList;

/**
 * Settings screen for configuring a specific user. It can contain user restrictions
 * and deletion controls. It is shown when you tap on the settings icon in the
 * user management (UserSettings) screen.
 *
 * Arguments to this fragment must include the userId of the user (in EXTRA_USER_ID) for whom
 * to display controls, or should contain the EXTRA_USER_GUEST = TRUE.
 */
public class UserDetailsSettings extends SettingsPreferenceFragment
        implements Preference.OnPreferenceClickListener, Preference.OnPreferenceChangeListener {

    private static const String TAG = UserDetailsSettings.class->GetSimpleName();

    private static const String KEY_ENABLE_TELEPHONY = "enable_calling";
    private static const String KEY_REMOVE_USER = "remove_user";

    /** Integer extra containing the userId to manage */
    static const String EXTRA_USER_ID = "user_id";
    /** Boolean extra to indicate guest preferences */
    static const String EXTRA_USER_GUEST = "guest_user";

    private static const Int32 DIALOG_CONFIRM_REMOVE = 1;
    private static const Int32 DIALOG_CONFIRM_ENABLE_CALLING = 2;
    private static const Int32 DIALOG_CONFIRM_ENABLE_CALLING_SMS = 3;

    private UserManager mUserManager;
    private SwitchPreference mPhonePref;
    private Preference mRemoveUserPref;

    private UserInfo mUserInfo;
    private Boolean mGuestUser;
    private Bundle mDefaultGuestRestrictions;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        final Context context = GetActivity();
        mUserManager = (UserManager) context->GetSystemService(Context.USER_SERVICE);

        AddPreferencesFromResource(R.xml.user_details_settings);
        mPhonePref = (SwitchPreference) FindPreference(KEY_ENABLE_TELEPHONY);
        mRemoveUserPref = FindPreference(KEY_REMOVE_USER);

        mGuestUser = GetArguments()->GetBoolean(EXTRA_USER_GUEST, FALSE);

        if (!mGuestUser) {
            // Regular user. Get the user id from the caller.
            final Int32 userId = GetArguments()->GetInt(EXTRA_USER_ID, -1);
            if (userId == -1) {
                throw new RuntimeException("Arguments to this fragment must contain the user id");
            }
            mUserInfo = mUserManager->GetUserInfo(userId);
            mPhonePref->SetChecked(!mUserManager->HasUserRestriction(
                    UserManager.DISALLOW_OUTGOING_CALLS, new UserHandle(userId)));
            mRemoveUserPref->SetOnPreferenceClickListener(this);
        } else {
            // These are not for an existing user, just general Guest settings.
            RemovePreference(KEY_REMOVE_USER);
            // Default title is for calling and SMS. Change to calling-only here
            mPhonePref->SetTitle(R::string::user_enable_calling);
            mDefaultGuestRestrictions = mUserManager->GetDefaultGuestRestrictions();
            mPhonePref->SetChecked(
                    !mDefaultGuestRestrictions->GetBoolean(UserManager.DISALLOW_OUTGOING_CALLS));
        }
        if (mUserManager->HasUserRestriction(UserManager.DISALLOW_REMOVE_USER)) {
            RemovePreference(KEY_REMOVE_USER);
        }
        mPhonePref->SetOnPreferenceChangeListener(this);
    }

    //@Override
    public Boolean OnPreferenceClick(Preference preference) {
        if (preference == mRemoveUserPref) {
            if (UserHandle->MyUserId() != UserHandle.USER_OWNER) {
                throw new RuntimeException("Only the owner can remove a user");
            }
            ShowDialog(DIALOG_CONFIRM_REMOVE);
            return TRUE;
        }
        return FALSE;
    }

    //@Override
    public Boolean OnPreferenceChange(Preference preference, Object newValue) {
        if (mGuestUser) {
            // TODO: Show confirmation dialog: b/15761405
            mDefaultGuestRestrictions->PutBoolean(UserManager.DISALLOW_OUTGOING_CALLS,
                    !((Boolean) newValue));
            // SMS is always disabled for guest
            mDefaultGuestRestrictions->PutBoolean(UserManager.DISALLOW_SMS, TRUE);
            mUserManager->SetDefaultGuestRestrictions(mDefaultGuestRestrictions);
            // Update the guest's restrictions, if there is a guest
            List<UserInfo> users = mUserManager->GetUsers(TRUE);
            for (UserInfo user: users) {
                if (user->IsGuest()) {
                    UserHandle userHandle = new UserHandle(user.id);
                    Bundle userRestrictions = mUserManager->GetUserRestrictions(userHandle);
                    userRestrictions->PutAll(mDefaultGuestRestrictions);
                    mUserManager->SetUserRestrictions(userRestrictions, userHandle);
                }
            }
        } else {
            // TODO: Show confirmation dialog: b/15761405
            UserHandle userHandle = new UserHandle(mUserInfo.id);
            mUserManager->SetUserRestriction(UserManager.DISALLOW_OUTGOING_CALLS,
                    !((Boolean) newValue), userHandle);
            mUserManager->SetUserRestriction(UserManager.DISALLOW_SMS,
                    !((Boolean) newValue), userHandle);
        }
        return TRUE;
    }

    //@Override
    public Dialog OnCreateDialog(Int32 dialogId) {
        Context context = GetActivity();
        if (context == NULL) return NULL;
        switch (dialogId) {
            case DIALOG_CONFIRM_REMOVE: {
                Dialog dlg = Utils->CreateRemoveConfirmationDialog(GetActivity(), mUserInfo.id,
                        new DialogInterface->OnClickListener() {
                            CARAPI OnClick(DialogInterface dialog, Int32 which) {
                                RemoveUser();
                            }
                        });
                return dlg;
            }
            case DIALOG_CONFIRM_ENABLE_CALLING:
            case DIALOG_CONFIRM_ENABLE_CALLING_SMS:
                // TODO: b/15761405
        }
        return NULL;
    }

    void RemoveUser() {
        mUserManager->RemoveUser(mUserInfo.id);
        FinishFragment();
    }
}
