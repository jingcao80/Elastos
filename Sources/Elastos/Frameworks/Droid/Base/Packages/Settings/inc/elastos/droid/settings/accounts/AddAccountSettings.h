

package com.android.settings.accounts;

using Elastos::Droid::Accounts::IAccountManager;
using Elastos::Droid::Accounts::IAccountManagerCallback;
using Elastos::Droid::Accounts::IAccountManagerFuture;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Widget::IToast;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::IUtils;


using static android::Content::Intent::IEXTRA_USER;

/**
 * Entry point Activity for account setup. Works as follows
 *
 * 1) When the other Activities launch this Activity, it launches {@link ChooseAccountActivity}
 *    without showing anything.
 * 2) After receiving an account type from ChooseAccountActivity, this Activity launches the
 *    account setup specified by AccountManager.
 * 3) After the account setup, this Activity finishes without showing anything.
 *
 * Note:
 * Previously this Activity did what {@link ChooseAccountActivity} does right now, but we
 * currently delegate the work to the other Activity. When we let this Activity do that work, users
 * would see the list of account types when leaving this Activity, since the UI is already ready
 * when returning from each account setup, which doesn't look good.
 *
 * An extra {@link UserHandle} can be specified in the intent as {@link EXTRA_USER}, if the user for
 * which the action needs to be performed is different to the one the Settings App will run in.
 */
public class AddAccountSettings extends Activity {
    /**
     *
     */
    private static const String KEY_ADD_CALLED = "AddAccountCalled";

    /**
     * Extra parameter to identify the caller. Applications may display a
     * different UI if the calls is made from Settings or from a specific
     * application.
     */
    private static const String KEY_CALLER_IDENTITY = "pendingIntent";
    private static const String SHOULD_NOT_RESOLVE = "SHOULDN'T RESOLVE!";

    private static const String TAG = "AccountSettings";

    /* package */ static const String EXTRA_SELECTED_ACCOUNT = "selected_account";

    // show additional info regarding the use of a device with multiple users
    static const String EXTRA_HAS_MULTIPLE_USERS = "hasMultipleUsers";

    private static const Int32 CHOOSE_ACCOUNT_REQUEST = 1;
    private static const Int32 ADD_ACCOUNT_REQUEST = 2;

    private PendingIntent mPendingIntent;

    private final AccountManagerCallback<Bundle> mCallback = new AccountManagerCallback<Bundle>() {
        //@Override
        CARAPI Run(AccountManagerFuture<Bundle> future) {
            Boolean done = TRUE;
            try {
                Bundle bundle = future->GetResult();
                //bundle->KeySet();
                Intent intent = (Intent) bundle->Get(AccountManager.KEY_INTENT);
                if (intent != NULL) {
                    done = FALSE;
                    Bundle addAccountOptions = new Bundle();
                    addAccountOptions->PutParcelable(KEY_CALLER_IDENTITY, mPendingIntent);
                    addAccountOptions->PutBoolean(EXTRA_HAS_MULTIPLE_USERS,
                            Utils->HasMultipleUsers(AddAccountSettings.this));
                    addAccountOptions->PutParcelable(EXTRA_USER, mUserHandle);
                    intent->PutExtras(addAccountOptions);
                    StartActivityForResultAsUser(intent, ADD_ACCOUNT_REQUEST, mUserHandle);
                } else {
                    SetResult(RESULT_OK);
                    if (mPendingIntent != NULL) {
                        mPendingIntent->Cancel();
                        mPendingIntent = NULL;
                    }
                }

                if (Log->IsLoggable(TAG, Log.VERBOSE)) Logger::V(TAG, "account added: " + bundle);
            } catch (OperationCanceledException e) {
                if (Log->IsLoggable(TAG, Log.VERBOSE)) Logger::V(TAG, "addAccount was canceled");
            } catch (IOException e) {
                if (Log->IsLoggable(TAG, Log.VERBOSE)) Logger::V(TAG, "addAccount failed: " + e);
            } catch (AuthenticatorException e) {
                if (Log->IsLoggable(TAG, Log.VERBOSE)) Logger::V(TAG, "addAccount failed: " + e);
            } finally {
                if (done) {
                    Finish();
                }
            }
        }
    };

    private Boolean mAddAccountCalled = FALSE;
    private UserHandle mUserHandle;

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        if (savedInstanceState != NULL) {
            mAddAccountCalled = savedInstanceState->GetBoolean(KEY_ADD_CALLED);
            if (Log->IsLoggable(TAG, Log.VERBOSE)) Logger::V(TAG, "restored");
        }

        final UserManager um = (UserManager) GetSystemService(Context.USER_SERVICE);
        mUserHandle = Utils->GetSecureTargetUser(GetActivityToken(), um, NULL /* arguments */,
                GetIntent()->GetExtras());
        if (um->HasUserRestriction(UserManager.DISALLOW_MODIFY_ACCOUNTS, mUserHandle)) {
            // We aren't allowed to add an account.
            Toast->MakeText(this, R::string::user_cannot_add_accounts_message, Toast.LENGTH_LONG)
                    .Show();
            Finish();
            return;
        }
        if (mAddAccountCalled) {
            // We already called add account - maybe the callback was lost.
            Finish();
            return;
        }
        final String[] authorities =
                GetIntent()->GetStringArrayExtra(AccountPreferenceBase.AUTHORITIES_FILTER_KEY);
        final String[] accountTypes =
                GetIntent()->GetStringArrayExtra(AccountPreferenceBase.ACCOUNT_TYPES_FILTER_KEY);
        final Intent intent = new Intent(this, ChooseAccountActivity.class);
        if (authorities != NULL) {
            intent->PutExtra(AccountPreferenceBase.AUTHORITIES_FILTER_KEY, authorities);
        }
        if (accountTypes != NULL) {
            intent->PutExtra(AccountPreferenceBase.ACCOUNT_TYPES_FILTER_KEY, accountTypes);
        }
        intent->PutExtra(EXTRA_USER, mUserHandle);
        StartActivityForResult(intent, CHOOSE_ACCOUNT_REQUEST);
    }

    //@Override
    CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        switch (requestCode) {
        case CHOOSE_ACCOUNT_REQUEST:
            if (resultCode == RESULT_CANCELED) {
                SetResult(resultCode);
                Finish();
                return;
            }
            // Go to account setup screen. Finish() is called inside mCallback.
            AddAccount(data->GetStringExtra(EXTRA_SELECTED_ACCOUNT));
            break;
        case ADD_ACCOUNT_REQUEST:
            SetResult(resultCode);
            if (mPendingIntent != NULL) {
                mPendingIntent->Cancel();
                mPendingIntent = NULL;
            }
            Finish();
            break;
        }
    }

    //@Override
    protected void OnSaveInstanceState(Bundle outState) {
        super->OnSaveInstanceState(outState);
        outState->PutBoolean(KEY_ADD_CALLED, mAddAccountCalled);
        if (Log->IsLoggable(TAG, Log.VERBOSE)) Logger::V(TAG, "saved");
    }

    private void AddAccount(String accountType) {
        Bundle addAccountOptions = new Bundle();
        /*
         * The identityIntent is for the purposes of establishing the identity
         * of the caller and isn't intended for launching activities, services
         * or broadcasts.
         *
         * Unfortunately for legacy reasons we still need to support this. But
         * we can cripple the intent so that 3rd party authenticators can't
         * fill in addressing information and launch arbitrary actions.
         */
        Intent identityIntent = new Intent();
        identityIntent->SetComponent(new ComponentName(SHOULD_NOT_RESOLVE, SHOULD_NOT_RESOLVE));
        identityIntent->SetAction(SHOULD_NOT_RESOLVE);
        identityIntent->AddCategory(SHOULD_NOT_RESOLVE);

        mPendingIntent = PendingIntent->GetBroadcast(this, 0, identityIntent, 0);
        addAccountOptions->PutParcelable(KEY_CALLER_IDENTITY, mPendingIntent);
        addAccountOptions->PutBoolean(EXTRA_HAS_MULTIPLE_USERS, Utils->HasMultipleUsers(this));
        AccountManager->Get(this).AddAccountAsUser(
                accountType,
                NULL, /* authTokenType */
                NULL, /* requiredFeatures */
                addAccountOptions,
                NULL,
                mCallback,
                NULL /* handler */,
                mUserHandle);
        mAddAccountCalled  = TRUE;
    }
}
