
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/accounts/ChooseTypeAndAccountActivity.h"
#include "elastos/droid/accounts/CAccountManager.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/accounts/CAccount.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/CUserManagerHelper.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/widget/CArrayAdapter.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::IUserManagerHelper;
using Elastos::Droid::Os::CUserManagerHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Accounts {

const String ChooseTypeAndAccountActivity::TAG("AccountChooser");
const String ChooseTypeAndAccountActivity::KEY_INSTANCE_STATE_PENDING_REQUEST(
        "pendingRequest");
const String ChooseTypeAndAccountActivity::KEY_INSTANCE_STATE_EXISTING_ACCOUNTS(
        "existingAccounts");
const String ChooseTypeAndAccountActivity::KEY_INSTANCE_STATE_SELECTED_ACCOUNT_NAME(
        "selectedAccountName");
const String ChooseTypeAndAccountActivity::KEY_INSTANCE_STATE_SELECTED_ADD_ACCOUNT(
        "selectedAddAccount");
const String ChooseTypeAndAccountActivity::KEY_INSTANCE_STATE_ACCOUNT_LIST("accountList");
const Int32 ChooseTypeAndAccountActivity::SELECTED_ITEM_NONE = -1;

//===============================================================
// ChooseTypeAndAccountActivity::AdapterViewOnItemClickListener
//===============================================================
CAR_INTERFACE_IMPL(ChooseTypeAndAccountActivity::AdapterViewOnItemClickListener, Object,
        IAdapterViewOnItemClickListener);

ChooseTypeAndAccountActivity::AdapterViewOnItemClickListener::AdapterViewOnItemClickListener(
    /* [in] */ ChooseTypeAndAccountActivity* host)
    : mHost(host)
{}

ECode ChooseTypeAndAccountActivity::AdapterViewOnItemClickListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    mHost->mSelectedItemIndex = position;
    return IView::Probe(mHost->mOkButton)->SetEnabled(TRUE);
}

//===============================================================
// ChooseTypeAndAccountActivity::
//===============================================================
CAR_INTERFACE_IMPL_2(ChooseTypeAndAccountActivity, Activity, IChooseTypeAndAccountActivity, IAccountManagerCallback)

ChooseTypeAndAccountActivity::ChooseTypeAndAccountActivity()
    : mSelectedAddNewAccount(FALSE)
    , mAlwaysPromptForAccount(FALSE)
    , mPendingRequest(REQUEST_NULL)
    , mSelectedItemIndex(0)
{}

ChooseTypeAndAccountActivity::~ChooseTypeAndAccountActivity()
{
}

ECode ChooseTypeAndAccountActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(Activity::OnCreate(savedInstanceState));
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "ChooseTypeAndAccountActivity.onCreate(savedInstanceState=%p)",
                    savedInstanceState);
    }

    String message(NULL);

    AutoPtr<IBinder> activityToken;
    GetActivityToken((IBinder**)&activityToken);
    ActivityManagerNative::GetDefault()->GetLaunchedFromUid(activityToken, &mCallingUid);
    ActivityManagerNative::GetDefault()->GetLaunchedFromPackage(
            activityToken, &mCallingPackage);
    if (mCallingUid != 0 && !mCallingPackage.IsNull()) {
        AutoPtr<IUserHandle> uh;
        CUserHandle::New(UserHandle::GetUserId(mCallingUid), (IUserHandle**)&uh);
        AutoPtr<IUserManagerHelper> hlp;
        CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&hlp);
        AutoPtr<IUserManager> um;
        hlp->Get(this, (IUserManager**)&um);
        AutoPtr<IBundle> restrictions;
        um->GetUserRestrictions(uh, (IBundle**)&restrictions);
        restrictions->GetBoolean(IUserManager::DISALLOW_MODIFY_ACCOUNTS, FALSE, &mDisallowAddAccounts);
    }

    // save some items we use frequently
    AutoPtr<IIntent> intent;
    FAIL_RETURN(GetIntent((IIntent**)&intent));

    if (savedInstanceState != NULL) {
        savedInstanceState->GetInt32(KEY_INSTANCE_STATE_PENDING_REQUEST, &mPendingRequest);
        savedInstanceState->GetParcelableArray(KEY_INSTANCE_STATE_EXISTING_ACCOUNTS,
                (ArrayOf<IParcelable*>**)&mExistingAccounts);

        // Makes sure that any user selection is preserved across orientation changes.
        savedInstanceState->GetString(KEY_INSTANCE_STATE_SELECTED_ACCOUNT_NAME,
                &mSelectedAccountName);

        savedInstanceState->GetBoolean(KEY_INSTANCE_STATE_SELECTED_ADD_ACCOUNT,
                FALSE, &mSelectedAddNewAccount);

        savedInstanceState->GetParcelableArrayList(KEY_INSTANCE_STATE_ACCOUNT_LIST, (IArrayList**)&mAccounts);
    }
    else {
        mPendingRequest = REQUEST_NULL;
        mExistingAccounts = NULL;
        // If the selected account as specified in the intent matches one in the list we will
        // show is as pre-selected.
        AutoPtr<IParcelable> parcel;
        intent->GetParcelableExtra(EXTRA_SELECTED_ACCOUNT, (IParcelable**)&parcel);
        AutoPtr<IAccount> selectedAccount = IAccount::Probe(parcel);
        if (selectedAccount != NULL) {
            mSelectedAccountName = ((CAccount*)selectedAccount.Get())->mName;
        }
    }

    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "selected account name is %s", (const char*)mSelectedAccountName);
    }

    mSetOfAllowableAccounts = GetAllowableAccountSet(intent);
    mSetOfRelevantAccountTypes = GetReleventAccountTypes(intent);
    intent->GetBooleanExtra(EXTRA_ALWAYS_PROMPT_FOR_ACCOUNT,
            FALSE, &mAlwaysPromptForAccount);
    intent->GetStringExtra(EXTRA_DESCRIPTION_TEXT_OVERRIDE, &mDescriptionOverride);

    // Need to do this once here to request the window feature. Can't do it in onResume
    AutoPtr<IAccountManager> accountManager;
    FAIL_RETURN(CAccountManager::Get(this, (IAccountManager**)&accountManager));
    mAccounts = GetAcceptableAccountChoices(accountManager);
    Boolean bEmp = FALSE;
    if ((mAccounts->IsEmpty(&bEmp), bEmp)
            && mDisallowAddAccounts) {
        Boolean b = FALSE;
        RequestWindowFeature(IWindow::FEATURE_NO_TITLE, &b);
        SetContentView(R::layout::app_not_authorized);
        mDontShowPicker = TRUE;
    }

    return NOERROR;
}

ECode ChooseTypeAndAccountActivity::OnResume()
{
    FAIL_RETURN(Activity::OnResume());

    if (mDontShowPicker) return NOERROR;

    AutoPtr<IAccountManager> accountManager;
    FAIL_RETURN(CAccountManager::Get(this, (IAccountManager**)&accountManager));

    mAccounts = GetAcceptableAccountChoices(accountManager);

    // In cases where the activity does not need to show an account picker, cut the chase
    // and return the result directly. Eg:
    // Single account -> select it directly
    // No account -> launch add account activity directly
    if (mPendingRequest == REQUEST_NULL) {
        // If there are no relevant accounts and only one relevant account type go directly to
        // add account. Otherwise let the user choose.
        Boolean bEmp = FALSE;
        if ((mAccounts->IsEmpty(&bEmp), bEmp)) {
            Int32 size = 0;
            mSetOfRelevantAccountTypes->GetSize(&size);
            if (size == 1) {
                AutoPtr<IIterator> it;
                ICollection::Probe(mSetOfRelevantAccountTypes)->GetIterator((IIterator**)&it);
                AutoPtr<IInterface> nxt;
                it->GetNext((IInterface**)&nxt);
                AutoPtr<ICharSequence> pCS = ICharSequence::Probe(nxt);
                String str;
                pCS->ToString(&str);
                RunAddAccountForAuthenticator(str);
            }
            else {
                StartChooseAccountTypeActivity();
            }
            return NOERROR;
        }

        // if there is only one allowable account return it
        Int32 accSize = 0;
        mAccounts->GetSize(&accSize);
        if (!mAlwaysPromptForAccount && accSize == 1) {
            AutoPtr<IInterface> p;
            mAccounts->Get(0, (IInterface**)&p);
            AutoPtr<IAccount> account = IAccount::Probe(p);
            String name, type;
            account->GetName(&name);
            account->GetType(&type);
            SetResultAndFinish(name, type);
            return NOERROR;
        }
    }

    AutoPtr<ArrayOf<String> > listItems = GetListOfDisplayableOptions(mAccounts);
    mSelectedItemIndex = GetItemIndexToSelect(
        mAccounts, mSelectedAccountName, mSelectedAddNewAccount);

    // Cannot set content view until we know that mPendingRequest is not null, otherwise
    // would cause screen flicker.
    FAIL_RETURN(SetContentView(R::layout::choose_type_and_account));
    OverrideDescriptionIfSupplied(mDescriptionOverride);
    PopulateUIAccountList(listItems);

    // Only enable "OK" button if something has been selected.
    AutoPtr<IView> view;
    FAIL_RETURN(FindViewById(R::id::button2, (IView**)&view));
    mOkButton = IButton::Probe(view);
    IView::Probe(mOkButton)->SetEnabled(mSelectedItemIndex != SELECTED_ITEM_NONE);
    return NOERROR;
}

ECode ChooseTypeAndAccountActivity::OnDestroy()
{
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "ChooseTypeAndAccountActivity.onDestroy()");
    }
    return Activity::OnDestroy();
}

ECode ChooseTypeAndAccountActivity::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    FAIL_RETURN(Activity::OnSaveInstanceState(outState));
    outState->PutInt32(KEY_INSTANCE_STATE_PENDING_REQUEST, mPendingRequest);
    if (mPendingRequest == REQUEST_ADD_ACCOUNT) {
        outState->PutParcelableArray(KEY_INSTANCE_STATE_EXISTING_ACCOUNTS, mExistingAccounts);
    }
    if (mSelectedItemIndex != SELECTED_ITEM_NONE) {
        Int32 size = 0;
        mAccounts->GetSize(&size);
        if (mSelectedItemIndex == size) {
            outState->PutBoolean(KEY_INSTANCE_STATE_SELECTED_ADD_ACCOUNT, TRUE);
        }
        else {
            outState->PutBoolean(KEY_INSTANCE_STATE_SELECTED_ADD_ACCOUNT, FALSE);
            AutoPtr<IInterface> p;
            mAccounts->Get(mSelectedItemIndex, (IInterface**)&p);
            AutoPtr<IAccount> pAcc = IAccount::Probe(p);
            String name;
            pAcc->GetName(&name);
            outState->PutString(KEY_INSTANCE_STATE_SELECTED_ACCOUNT_NAME, name);
        }
    }
    outState->PutParcelableArrayList(KEY_INSTANCE_STATE_ACCOUNT_LIST, mAccounts);
    return NOERROR;
}

ECode ChooseTypeAndAccountActivity::OnCancelButtonClicked(
    /* [in] */ IView* view)
{
    return OnBackPressed();
}

ECode ChooseTypeAndAccountActivity::OnOkButtonClicked(
    /* [in] */ IView* view)
{
    Int32 size = 0;
    mAccounts->GetSize(&size);
    if (mSelectedItemIndex == size) {
        // Selected "Add New Account" option
        StartChooseAccountTypeActivity();
    }
    else if (mSelectedItemIndex != SELECTED_ITEM_NONE) {
        AutoPtr<IInterface> p;
        mAccounts->Get(mSelectedItemIndex, (IInterface**)&p);
        AutoPtr<IAccount> pAcc = IAccount::Probe(p);
        OnAccountSelected(pAcc);
    }
    return NOERROR;
}

ECode ChooseTypeAndAccountActivity::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        AutoPtr<IBundle> bd;
        if (data != NULL && (data->GetExtras((IBundle**)&bd), bd) != NULL) {
            AutoPtr<ISet> s;
            bd->GetKeySet((ISet**)&s);
        }
        AutoPtr<IBundle> extras;
        if (data != NULL) {
            data->GetExtras((IBundle**)&bd);
        }
        Logger::V(TAG,
            "ChooseTypeAndAccountActivity.onActivityResult(reqCode=%d, resCode=%d, extras=%p)",
            requestCode, resultCode, extras.Get());
    }

    // we got our result, so clear the fact that we had a pending request
    mPendingRequest = REQUEST_NULL;

    if (resultCode == RESULT_CANCELED) {
        // if canceling out of addAccount and the original state caused us to skip this,
        // finish this activity
        Boolean bEmp = FALSE;
        if ((mAccounts->IsEmpty(&bEmp), bEmp)) {
            SetResult(IActivity::RESULT_CANCELED);
            Finish();
        }
        return NOERROR;
    }

    if (resultCode == RESULT_OK) {
        if (requestCode == REQUEST_CHOOSE_TYPE) {
            if (data != NULL) {
                String accountType;
                data->GetStringExtra(IAccountManager::KEY_ACCOUNT_TYPE, &accountType);
                if (!accountType.IsNull()) {
                    RunAddAccountForAuthenticator(accountType);
                    return NOERROR;
                }
            }
            Logger::D(TAG, "ChooseTypeAndAccountActivity.onActivityResult: unable to find account type, pretending the request was canceled");
        }
        else if (requestCode == REQUEST_ADD_ACCOUNT) {
            String accountName(NULL);
            String accountType(NULL);

            if (data != NULL) {
                data->GetStringExtra(IAccountManager::KEY_ACCOUNT_NAME, &accountName);
                data->GetStringExtra(IAccountManager::KEY_ACCOUNT_TYPE, &accountType);
            }

            if (accountName.IsNull() || accountType.IsNull()) {
                AutoPtr< ArrayOf<IAccount*> > currentAccounts;
                AutoPtr<IAccountManager> accountManager;
                FAIL_RETURN(CAccountManager::Get(IContext::Probe(this), (IAccountManager**)&accountManager));
                accountManager->GetAccountsForPackage(mCallingPackage, mCallingUid, (ArrayOf<IAccount*>**)&currentAccounts);
                AutoPtr<ISet> preExistingAccounts;
                CHashSet::New((ISet**)&preExistingAccounts);
                for (Int32 i = 0; i < mExistingAccounts->GetLength(); ++i) {
                    preExistingAccounts->Add(IAccount::Probe((*mExistingAccounts)[i]));
                }
                for(Int32 i = 0; i < currentAccounts->GetLength(); ++i) {
                    AutoPtr<IAccount> account = (*currentAccounts)[i];
                    Boolean bContains = FALSE;
                    preExistingAccounts->Contains(account, &bContains);
                    if (!bContains) {
                        account->GetName(&accountName);
                        account->GetType(&accountType);
                        break;
                    }
                }
            }

            if (!accountName.IsNull() || !accountType.IsNull()) {
                SetResultAndFinish(accountName, accountType);
                return NOERROR;
            }
        }
        Logger::D(TAG, "ChooseTypeAndAccountActivity.onActivityResult: unable to find added account, pretending the request was canceled");
    }
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "ChooseTypeAndAccountActivity.onActivityResult: canceled");
    }
    SetResult(IActivity::RESULT_CANCELED);
    return Finish();
}

void ChooseTypeAndAccountActivity::RunAddAccountForAuthenticator(
    /* [in] */ const String& type)
{
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "runAddAccountForAuthenticator: %s", (const char*)type);
    }
    AutoPtr<IIntent> intent;
    ASSERT_SUCCEEDED(GetIntent((IIntent**)&intent));
    AutoPtr<IBundle> options;
    intent->GetBundleExtra(EXTRA_ADD_ACCOUNT_OPTIONS_BUNDLE, (IBundle**)&options);
    AutoPtr< ArrayOf<String> > requiredFeatures;
    intent->GetStringArrayExtra(EXTRA_ADD_ACCOUNT_REQUIRED_FEATURES_STRING_ARRAY,
            (ArrayOf<String>**)&requiredFeatures);
    String authTokenType;
    intent->GetStringExtra(EXTRA_ADD_ACCOUNT_AUTH_TOKEN_TYPE_STRING, &authTokenType);
    AutoPtr<IContext> ctx = (IContext*)Probe(Elastos::Droid::Content::EIID_IContext);
    AutoPtr<IAccountManager> accountManager;
    ASSERT_SUCCEEDED(CAccountManager::Get(ctx, (IAccountManager**)&accountManager));
    AutoPtr<IAccountManagerFuture> future;
    accountManager->AddAccount(type, authTokenType, requiredFeatures,
            options, NULL /* activity */, this /* callback */,
            NULL /* Handler */, (IAccountManagerFuture**)&future);
}

ECode ChooseTypeAndAccountActivity::Run(
    /* [in] */ IAccountManagerFuture* accountManagerFuture)
{
    AutoPtr<IBundle> accountManagerResult;
    FAIL_RETURN(accountManagerFuture->GetResult((IInterface**)&accountManagerResult));
    AutoPtr<IParcelable> parcel;
    accountManagerResult->GetParcelable(IAccountManager::KEY_INTENT, (IParcelable**)&parcel);
    AutoPtr<IIntent> intent = (IIntent*)parcel->Probe(Elastos::Droid::Content::EIID_IIntent);
    if (intent != NULL) {
        mPendingRequest = REQUEST_ADD_ACCOUNT;
        AutoPtr<IContext> ctx = (IContext*)Probe(Elastos::Droid::Content::EIID_IContext);
        AutoPtr<IAccountManager> accountManager;
        ASSERT_SUCCEEDED(CAccountManager::Get(ctx, (IAccountManager**)&accountManager));
        mExistingAccounts = NULL;
        accountManager->GetAccountsForPackage(mCallingPackage,
                        mCallingUid, (ArrayOf<IAccount*>**)&mExistingAccounts);
        Int32 flags;
        intent->GetFlags(&flags);
        intent->SetFlags(flags & ~IIntent::FLAG_ACTIVITY_NEW_TASK);
        ECode ec = StartActivityForResult(intent, REQUEST_ADD_ACCOUNT);
        if (ec == (ECode)E_OPERATION_CANCELED_EXCEPTION) {
            SetResult(IActivity::RESULT_CANCELED);
            Finish();
            return ec;
        }
        return NOERROR;
    }
    AutoPtr<IBundle> bundle;
    FAIL_RETURN(CBundle::New((IBundle**)&bundle));
    bundle->PutString(IAccountManager::KEY_ERROR_MESSAGE,
            String("error communicating with server"));
    FAIL_RETURN(CIntent::New((IIntent**)&intent));
    intent->PutExtras(bundle);
    SetResult(IActivity::RESULT_OK, intent);
    return Finish();
}

void ChooseTypeAndAccountActivity::OnAccountSelected(
    /* [in] */ IAccount* account)
{
    Logger::D(TAG, "selected account %p", account);
    AutoPtr<CAccount> cls = (CAccount*)account;
    SetResultAndFinish(cls->mName, cls->mType);
}

void ChooseTypeAndAccountActivity::SetResultAndFinish(
    /* [in] */ const String& accountName,
    /* [in] */ const String& accountType)
{
    AutoPtr<IBundle> bundle;
    ASSERT_SUCCEEDED(CBundle::New((IBundle**)&bundle));
    bundle->PutString(IAccountManager::KEY_ACCOUNT_NAME, accountName);
    bundle->PutString(IAccountManager::KEY_ACCOUNT_TYPE, accountType);
    AutoPtr<IIntent> intent;
    ASSERT_SUCCEEDED(CIntent::New((IIntent**)&intent));
    intent->PutExtras(bundle);
    SetResult(IActivity::RESULT_OK, intent);
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "ChooseTypeAndAccountActivity.setResultAndFinish: selected account %s, %s",
            (const char*)accountName, (const char*)accountType);
    }
    Finish();
}

void ChooseTypeAndAccountActivity::StartChooseAccountTypeActivity()
{
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "ChooseAccountTypeActivity.startChooseAccountTypeActivity()");
    }
    AutoPtr<IIntent> intent;
    assert(0);
    // ASSERT_SUCCEEDED(CIntent::New(this, ChooseAccountTypeActivity.class));
    intent->SetFlags(IIntent::FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET);
    AutoPtr<IIntent> i;
    ASSERT_SUCCEEDED(GetIntent((IIntent**)&i));
    AutoPtr< ArrayOf<String> > stringArray;
    i->GetStringArrayExtra(EXTRA_ALLOWABLE_ACCOUNT_TYPES_STRING_ARRAY,
            (ArrayOf<String>**)&stringArray);
    intent->PutExtra(EXTRA_ALLOWABLE_ACCOUNT_TYPES_STRING_ARRAY,
            stringArray);
    AutoPtr<IBundle> bundle;
    i->GetBundleExtra(EXTRA_ADD_ACCOUNT_OPTIONS_BUNDLE, (IBundle**)&bundle);
    intent->PutExtra(EXTRA_ADD_ACCOUNT_OPTIONS_BUNDLE, bundle);
    stringArray = NULL;
    i->GetStringArrayExtra(EXTRA_ADD_ACCOUNT_REQUIRED_FEATURES_STRING_ARRAY,
            (ArrayOf<String>**)&stringArray);
    intent->PutExtra(EXTRA_ADD_ACCOUNT_REQUIRED_FEATURES_STRING_ARRAY,
            stringArray);
    String str;
    i->GetStringExtra(EXTRA_ADD_ACCOUNT_AUTH_TOKEN_TYPE_STRING, &str);
    intent->PutExtra(EXTRA_ADD_ACCOUNT_AUTH_TOKEN_TYPE_STRING, str);
    StartActivityForResult(intent, REQUEST_CHOOSE_TYPE);
    mPendingRequest = REQUEST_CHOOSE_TYPE;
}

Int32 ChooseTypeAndAccountActivity::GetItemIndexToSelect(
    /* [in] */ IArrayList* accounts,
    /* [in] */ const String& selectedAccountName,
    /* [in] */ Boolean selectedAddNewAccount)
{
    // If "Add account" option was previously selected by user, preserve it across
    // orientation changes.
    Int32 size = 0;
    accounts->GetSize(&size);
    if (selectedAddNewAccount) {
        return size;
    }
    // search for the selected account name if present
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> p;
        accounts->Get(i, (IInterface**)&p);
        AutoPtr<IAccount> pAcc = IAccount::Probe(p);
        String name;
        pAcc->GetName(&name);
        if (name.Equals(selectedAccountName)) {
          return i;
        }
    }
    // no account selected.
    return SELECTED_ITEM_NONE;
}

AutoPtr< ArrayOf<String> > ChooseTypeAndAccountActivity::GetListOfDisplayableOptions(
    /* [in] */ IArrayList* accounts)
{
    // List of options includes all accounts found together with "Add new account" as the
    // last item in the list.
    Int32 size = 0;
    accounts->GetSize(&size);
    AutoPtr<ArrayOf<String> > listItems = ArrayOf<String>::Alloc(size + (mDisallowAddAccounts ? 0 : 1));
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> p;
        accounts->Get(i, (IInterface**)&p);
        AutoPtr<IAccount> acc = IAccount::Probe(p);
        String name;
        acc->GetName(&name);
        (*listItems)[i] = name;
    }
    if (!mDisallowAddAccounts) {
        AutoPtr<IResources> r;
        ASSERT_SUCCEEDED(GetResources((IResources**)&r));
        r->GetString(R::string::add_account_button_label, &(*listItems)[size]);
    }
    return listItems;
}

AutoPtr<IArrayList> ChooseTypeAndAccountActivity::GetAcceptableAccountChoices(
    /* [in] */ IAccountManager* accountManager)
{
    AutoPtr<ArrayOf<IAccount*> > accounts;
    ASSERT_SUCCEEDED(accountManager->GetAccountsForPackage(mCallingPackage, mCallingUid, (ArrayOf<IAccount*>**)&accounts));
    AutoPtr<IArrayList> accountsToPopulate;
    CArrayList::New(accounts->GetLength(), (IArrayList**)&accountsToPopulate);
    for (Int32 i = 0; i < accounts->GetLength(); ++i) {
        AutoPtr<IAccount> account = (*accounts)[i];
        Boolean bContains = FALSE;
        if (mSetOfAllowableAccounts != NULL
                && (mSetOfAllowableAccounts->Contains(account, &bContains), bContains)) {
            continue;
        }
        String type;
        account->GetType(&type);
        AutoPtr<ICharSequence> pType;
        CString::New(type, (ICharSequence**)&pType);
        if (mSetOfRelevantAccountTypes != NULL
                && !(mSetOfRelevantAccountTypes->Contains(pType, &bContains), bContains)) {
            continue;
        }
        accountsToPopulate->Add(account);
    }
    return accountsToPopulate;
}

AutoPtr<ISet> ChooseTypeAndAccountActivity::GetReleventAccountTypes(
    /* [in] */ IIntent* intent)
{
    // An account type is relevant iff it is allowed by the caller and supported by the account
    // manager.
    AutoPtr<ISet> setOfRelevantAccountTypes;
    AutoPtr< ArrayOf<String> > allowedAccountTypes;
    ASSERT_SUCCEEDED(intent->GetStringArrayExtra(EXTRA_ALLOWABLE_ACCOUNT_TYPES_STRING_ARRAY,
            (ArrayOf<String>**)&allowedAccountTypes));
    if (allowedAccountTypes != NULL) {
        CHashSet::New((ISet**)&setOfRelevantAccountTypes);
        for (Int32 i = 0; i < allowedAccountTypes->GetLength(); ++i) {
            AutoPtr<ICharSequence> pCS;
            CString::New((*allowedAccountTypes)[i], (ICharSequence**)&pCS);
        }
        AutoPtr< ArrayOf<IAuthenticatorDescription*> > descs;
        AutoPtr<IAccountManager> accountManager;
        ASSERT_SUCCEEDED(CAccountManager::Get(this, (IAccountManager**)&accountManager));
        ASSERT_SUCCEEDED(accountManager->GetAuthenticatorTypes(
                (ArrayOf<IAuthenticatorDescription*>**)&descs));
        Int32 len = descs->GetLength();
        AutoPtr<ISet> supportedAccountTypes;
        CHashSet::New(len, (ISet**)&supportedAccountTypes);
        for (Int32 i = 0; i < len; ++i) {
            AutoPtr<IAuthenticatorDescription> description = (*descs)[i];
            String type;
            description->GetType(&type);
            AutoPtr<ICharSequence> cs;
            CString::New(type, (ICharSequence**)&cs);
            supportedAccountTypes->Add(cs);
        }
        setOfRelevantAccountTypes->RetainAll(ICollection::Probe(supportedAccountTypes));
    }
    return setOfRelevantAccountTypes;
}

AutoPtr<ISet> ChooseTypeAndAccountActivity::GetAllowableAccountSet(
    /* [in] */ IIntent* intent)
{
    AutoPtr<ISet> setOfAllowableAccounts;
    AutoPtr<IArrayList> validAccounts;
    intent->GetParcelableArrayListExtra(EXTRA_ALLOWABLE_ACCOUNTS_ARRAYLIST, (IArrayList**)&validAccounts);
    if (validAccounts != NULL) {
        Int32 size = 0;
        validAccounts->GetSize(&size);
        CHashSet::New(size, (ISet**)&setOfAllowableAccounts);
        AutoPtr<IIterator> it;
        ICollection::Probe(validAccounts)->GetIterator((IIterator**)&it);
        Boolean bHasNext = FALSE;
        while ((it->HasNext(&bHasNext), bHasNext)) {
            AutoPtr<IInterface> ct;
            it->GetNext((IInterface**)&ct);
            AutoPtr<IParcelable> parcelable = IParcelable::Probe(ct);
            setOfAllowableAccounts->Add(IAccount::Probe(parcelable));
        }
    }
    return setOfAllowableAccounts;
}

void ChooseTypeAndAccountActivity::OverrideDescriptionIfSupplied(
    /* [in] */ const String& descriptionOverride)
{
    AutoPtr<IView> view;
    ASSERT_SUCCEEDED(FindViewById(R::id::description, (IView**)&view));
    AutoPtr<ITextView> descriptionView = ITextView::Probe(view);
    AutoPtr<ICharSequence> csq;
    CString::New(descriptionOverride, (ICharSequence**)&csq);
    if (!TextUtils::IsEmpty(csq)) {
        descriptionView->SetText(csq);
    }
    else {
        IView::Probe(descriptionView)->SetVisibility(IView::GONE);
    }
}

void ChooseTypeAndAccountActivity::PopulateUIAccountList(
    /* [in] */ ArrayOf<String>* listItems)
{
    AutoPtr<IView> view;
    ASSERT_SUCCEEDED(FindViewById(R::id::list, (IView**)&view));
    AutoPtr<IListView> list = IListView::Probe(view);

    AutoPtr<IArrayList> items;
    CArrayList::New((IArrayList**)&items);
    for (Int32 i = 0; i < listItems->GetLength(); ++i) {
        AutoPtr<ICharSequence> csq;
        CString::New((*listItems)[i], (ICharSequence**)&csq);
        items->Add(csq);
    }
    AutoPtr<IArrayAdapter> adapter;
    ASSERT_SUCCEEDED(CArrayAdapter::New(this,
            R::layout::simple_list_item_single_choice, IList::Probe(items),
            (IArrayAdapter**)&adapter));

    IAdapterView::Probe(list)->SetAdapter(IAdapter::Probe(adapter));
    IAbsListView::Probe(list)->SetChoiceMode(IAbsListView::CHOICE_MODE_SINGLE);
    list->SetItemsCanFocus(FALSE);
    IAdapterView::Probe(list)->SetOnItemClickListener(
            (IAdapterViewOnItemClickListener*)new AdapterViewOnItemClickListener(this));
    if (mSelectedItemIndex != SELECTED_ITEM_NONE) {
        IAbsListView::Probe(list)->SetItemChecked(mSelectedItemIndex, TRUE);
        if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
            Logger::V(TAG, "List item %d should be selected", mSelectedItemIndex);
        }
    }
}

}  //namespace Accounts
}  //namespace Droid
}  //namespace Elastos
