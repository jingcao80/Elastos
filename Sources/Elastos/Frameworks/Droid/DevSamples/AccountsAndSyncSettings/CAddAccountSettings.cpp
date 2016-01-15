
#include "CAddAccountSettings.h"
#include "R.h"

#include <elastos/utility/logging/Logger.h>
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Accounts::EIID_IAccountManagerCallback;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Accounts::CAccountManager;
using Elastos::Droid::Accounts::IAccountManagerFuture;
using Elastos::Droid::Accounts::IAccountManagerHelper;
using Elastos::Droid::Accounts::CAccountManagerHelper;
using Elastos::Droid::Accounts::IAccountManager;
using Elastos::Utility::Logging::Logger;


namespace AccountsAndSyncSettings {

    const String CAddAccountSettings::KEY_ADD_CALLED("AddAccountCalled");
    const String CAddAccountSettings::KEY_CALLER_IDENTITY("pendingIntent");
    const String CAddAccountSettings::EXTRA_SELECTED_ACCOUNT("selected_account");
    const String CAddAccountSettings::TAG("CAddAccountSettings");
    const String CAddAccountSettings::EXTRA_HAS_MULTIPLE_USERS("hasMultipleUsers");

    const Int32 CAddAccountSettings::CHOOSE_ACCOUNT_REQUEST = 1;
    const Int32 CAddAccountSettings::ADD_ACCOUNT_REQUEST = 2;
    Boolean mAddAccountCalled = FALSE;

    //=======================================================================
    //              CAddAccountSettings
    //=======================================================================

    CAR_INTERFACE_IMPL(MyCAccountManagerCallback, IAccountManagerCallback);
    MyCAccountManagerCallback::MyCAccountManagerCallback(CAddAccountSettings* owner)
    :mOwner(owner)
    {}

    ECode MyCAccountManagerCallback::Run(IAccountManagerFuture* future)
    {
        Boolean done = TRUE;
        //try {
        AutoPtr<IBundle> bundle;
        future->GetResult((IInterface**)&bundle);

        //bundle.keySet();
        AutoPtr<IIntent> intent;
        (IIntent*) bundle->Get(IAccountManager::KEY_INTENT, (IInterface**)&intent);
        if (intent != NULL) {
            done = FALSE;
            AutoPtr<IBundle> addAccountOptions;
            ASSERT_SUCCEEDED(CBundle::New((IBundle**)&addAccountOptions));

            addAccountOptions->PutParcelable(CAddAccountSettings::KEY_CALLER_IDENTITY, IParcelable::Probe(mOwner->mPendingIntent));
            addAccountOptions->PutBoolean(CAddAccountSettings::EXTRA_HAS_MULTIPLE_USERS,
                    /*Utils.hasMultipleUsers(AddAccountSettings.this)*/FALSE);
            intent->PutExtras(addAccountOptions);
            mOwner->StartActivityForResult(intent, CAddAccountSettings::ADD_ACCOUNT_REQUEST);
        } else {
            mOwner->SetResult(IActivity::RESULT_OK);
            if (mOwner->mPendingIntent != NULL) {
                mOwner->mPendingIntent->Cancel();
                mOwner->mPendingIntent = NULL;
            }
        }

        /*if (Log.isLoggable(TAG, Log.VERBOSE)) Log.v(TAG, "account added: " + bundle);
        } catch (OperationCanceledException e) {
            if (Log.isLoggable(TAG, Log.VERBOSE)) Log.v(TAG, "addAccount was canceled");
        } catch (IOException e) {
            if (Log.isLoggable(TAG, Log.VERBOSE)) Log.v(TAG, "addAccount failed: " + e);
        } catch (AuthenticatorException e) {
            if (Log.isLoggable(TAG, Log.VERBOSE)) Log.v(TAG, "addAccount failed: " + e);
        } finally {
            if (done) {
                finish();
            }
        }*/

        return NOERROR;
    }

    //=======================================================================
    //              CAddAccountSettings
    //=======================================================================
    ECode CAddAccountSettings::OnCreate(
        /* [in] */ IBundle* savedInstanceState)
    {
        Logger::D(TAG, "OnCreate---");
        Activity::OnCreate(savedInstanceState);

        if (savedInstanceState != NULL) {
            savedInstanceState->GetBoolean(KEY_ADD_CALLED, &mAddAccountCalled);
            Logger::D(TAG, "restored");
        }

        if (mAddAccountCalled) {
            // We already called add account - maybe the callback was lost.
            Finish();
            return NOERROR;
        }

        AutoPtr<ArrayOf<String> > authorities, accountTypes;
        AutoPtr<IIntent> intent;
        GetIntent((IIntent**)&intent);
        intent->GetStringArrayExtra(String("authorities")/*AccountPreferenceBase.AUTHORITIES_FILTER_KEY*/, (ArrayOf<String>**)&authorities);
        intent->GetStringArrayExtra(String("account_types")/*AccountPreferenceBase.ACCOUNT_TYPES_FILTER_KEY*/, (ArrayOf<String>**)&accountTypes);

        AutoPtr<IIntent> intent2;
        FAIL_RETURN(CIntent::New((IIntent**)&intent2));
        intent2->SetClassName(String("elastos"),
                String("elastos.accounts.ChooseAccountActivity"));

        if (authorities != NULL) {
            intent2->PutExtra(String("authorities"), authorities);
        }
        if (accountTypes != NULL) {
            intent2->PutExtra(String("account_types"), accountTypes);
        }

        StartActivityForResult(intent2, CHOOSE_ACCOUNT_REQUEST);
        return NOERROR;
    }

    ECode CAddAccountSettings::AddAccount(
        const String& accountType)
    {
        AutoPtr<IBundle> addAccountOptions;
        ASSERT_SUCCEEDED(CIntent::New(String(NULL), (IIntent**)&addAccountOptions));
        AutoPtr<IPendingIntentHelper> pendingIntentHelper;
        CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pendingIntentHelper);
        AutoPtr<IIntent> intent;
        ASSERT_SUCCEEDED(CIntent::New(String(NULL), (IIntent**)&intent));
        AutoPtr<IParcelable> parcleable = IParcelable::Probe(mPendingIntent);
        pendingIntentHelper->GetBroadcast(this, 0, intent, 0, (IPendingIntent**)&mPendingIntent);
        addAccountOptions->PutParcelable(KEY_CALLER_IDENTITY, parcleable);
        addAccountOptions->PutBoolean(EXTRA_HAS_MULTIPLE_USERS, FALSE);

        AutoPtr<IAccountManager> accountMgr;
        AutoPtr<IAccountManagerFuture> accmgrFuture;
        AutoPtr<IAccountManagerHelper> helper;
        ASSERT_SUCCEEDED(CAccountManagerHelper::AcquireSingleton((IAccountManagerHelper**)&helper));
        helper->Get(this, (IAccountManager**)&accountMgr);
        AutoPtr< ArrayOf<String> > requiredFeatures;
        accountMgr->AddAccount(
                accountType,
                String(NULL), /* authTokenType */
                *requiredFeatures, /* requiredFeatures */
                addAccountOptions,
                NULL,
                NULL, /*mCallback,*/
                NULL /* handler */,
                (IAccountManagerFuture**)&accmgrFuture);

        mAddAccountCalled  = TRUE;
    }
} //AccountsAndSyncSettings
