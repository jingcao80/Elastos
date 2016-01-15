
#include "elastos/droid/accounts/ChooseTypeAndAccountActivity.h"
#include "elastos/droid/accounts/CAccountManager.h"
#include "elastos/droid/accounts/CAuthenticatorDescription.h"
#include "elastos/droid/accounts/CAccount.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/widget/CArrayAdapter.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;

namespace Elastos {
namespace Droid {
namespace Accounts {

const String ChooseTypeAndAccountActivity::EXTRA_ALLOWABLE_ACCOUNTS_ARRAYLIST(
        "allowableAccounts");
const String ChooseTypeAndAccountActivity::EXTRA_ALLOWABLE_ACCOUNT_TYPES_STRING_ARRAY(
        "allowableAccountTypes");
const String ChooseTypeAndAccountActivity::EXTRA_ADD_ACCOUNT_OPTIONS_BUNDLE(
        "addAccountOptions");
const String ChooseTypeAndAccountActivity::EXTRA_ADD_ACCOUNT_REQUIRED_FEATURES_STRING_ARRAY(
        "addAccountRequiredFeatures");
const String ChooseTypeAndAccountActivity::EXTRA_ADD_ACCOUNT_AUTH_TOKEN_TYPE_STRING(
        "authTokenType");
const String ChooseTypeAndAccountActivity::EXTRA_SELECTED_ACCOUNT(
        "selectedAccount");
const String ChooseTypeAndAccountActivity::EXTRA_ALWAYS_PROMPT_FOR_ACCOUNT(
        "alwaysPromptForAccount");
const String ChooseTypeAndAccountActivity::EXTRA_DESCRIPTION_TEXT_OVERRIDE(
        "descriptionTextOverride");
const Int32 ChooseTypeAndAccountActivity::REQUEST_NULL = 0;
const Int32 ChooseTypeAndAccountActivity::REQUEST_CHOOSE_TYPE = 1;
const Int32 ChooseTypeAndAccountActivity::REQUEST_ADD_ACCOUNT = 2;
const String ChooseTypeAndAccountActivity::TAG("AccountChooser");
const String ChooseTypeAndAccountActivity::KEY_INSTANCE_STATE_PENDING_REQUEST(
        "pendingRequest");
const String ChooseTypeAndAccountActivity::KEY_INSTANCE_STATE_EXISTING_ACCOUNTS(
        "existingAccounts");
const String ChooseTypeAndAccountActivity::KEY_INSTANCE_STATE_SELECTED_ACCOUNT_NAME(
        "selectedAccountName");
const String ChooseTypeAndAccountActivity::KEY_INSTANCE_STATE_SELECTED_ADD_ACCOUNT(
        "selectedAddAccount");
const Int32 ChooseTypeAndAccountActivity::SELECTED_ITEM_NONE = -1;

ChooseTypeAndAccountActivity::ChooseTypeAndAccountActivity()
    : mSelectedAddNewAccount(FALSE)
    , mAlwaysPromptForAccount(FALSE)
    , mPendingRequest(REQUEST_NULL)
    , mSelectedItemIndex(0)
{}

ChooseTypeAndAccountActivity::~ChooseTypeAndAccountActivity()
{
}

PInterface ChooseTypeAndAccountActivity::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IAccountManagerCallback) {
        return (IAccountManagerCallback*)this;
    }
    return Activity::Probe(riid);
}

UInt32 ChooseTypeAndAccountActivity::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 ChooseTypeAndAccountActivity::Release()
{
    return ElRefBase::Release();
}

ECode ChooseTypeAndAccountActivity::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pObject == (IInterface *)(IAccountManagerCallback *)this) {
        *pIID = EIID_IAccountManagerCallback;
        return NOERROR;
    }
    return Activity::GetInterfaceID(pObject, pIID);
}

ECode ChooseTypeAndAccountActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(Activity::OnCreate(savedInstanceState));
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "ChooseTypeAndAccountActivity.onCreate(savedInstanceState="
    //             + savedInstanceState + ")");
    // }

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
    }
    else {
        mPendingRequest = REQUEST_NULL;
        mExistingAccounts = NULL;
        // If the selected account as specified in the intent matches one in the list we will
        // show is as pre-selected.
        AutoPtr<IParcelable> parcel;
        intent->GetParcelableExtra(EXTRA_SELECTED_ACCOUNT, (IParcelable**)&parcel);
        AutoPtr<IAccount> selectedAccount = (IAccount*)parcel->Probe(EIID_IAccount);
        if (selectedAccount != NULL) {
            mSelectedAccountName = ((CAccount*)selectedAccount.Get())->mName;
        }
    }

    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "selected account name is " + mSelectedAccountName);
    // }

    mSetOfAllowableAccounts = GetAllowableAccountSet(intent);
    mSetOfRelevantAccountTypes = GetReleventAccountTypes(intent);
    intent->GetBooleanExtra(EXTRA_ALWAYS_PROMPT_FOR_ACCOUNT,
            FALSE, &mAlwaysPromptForAccount);
    intent->GetStringExtra(EXTRA_DESCRIPTION_TEXT_OVERRIDE, &mDescriptionOverride);

    return NOERROR;
}

ECode ChooseTypeAndAccountActivity::OnResume()
{
    FAIL_RETURN(Activity::OnResume());
    AutoPtr<IContext> ctx = (IContext*)Probe(Elastos::Droid::Content::EIID_IContext);
    AutoPtr<IAccountManager> accountManager;
    FAIL_RETURN(CAccountManager::Get(ctx, (IAccountManager**)&accountManager));

    mAccounts = GetAcceptableAccountChoices(accountManager);

    // In cases where the activity does not need to show an account picker, cut the chase
    // and return the result directly. Eg:
    // Single account -> select it directly
    // No account -> launch add account activity directly
    if (mPendingRequest == REQUEST_NULL) {
        // If there are no relevant accounts and only one relevant account type go directly to
        // add account. Otherwise let the user choose.
        if (mAccounts->IsEmpty()) {
            if (mSetOfRelevantAccountTypes->GetSize() == 1) {
                RunAddAccountForAuthenticator(*mSetOfRelevantAccountTypes->Begin());
            }
            else {
                StartChooseAccountTypeActivity();
            }
            return NOERROR;
        }

        // if there is only one allowable account return it
        if (!mAlwaysPromptForAccount && mAccounts->GetSize() == 1) {
            AutoPtr<CAccount> account = (CAccount*)(*mAccounts->Begin()).Get();
            SetResultAndFinish(account->mName, account->mType);
            return NOERROR;
        }
    }

    AutoPtr< ArrayOf<String> > listItems = GetListOfDisplayableOptions(mAccounts);
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
    mOkButton->SetEnabled(mSelectedItemIndex != SELECTED_ITEM_NONE);

    return NOERROR;
}

ECode ChooseTypeAndAccountActivity::OnDestroy()
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "ChooseTypeAndAccountActivity.onDestroy()");
    // }
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
        if ((UInt32)mSelectedItemIndex == mAccounts->GetSize()) {
            outState->PutBoolean(KEY_INSTANCE_STATE_SELECTED_ADD_ACCOUNT, TRUE);
        }
        else {
            outState->PutBoolean(KEY_INSTANCE_STATE_SELECTED_ADD_ACCOUNT, FALSE);
            outState->PutString(KEY_INSTANCE_STATE_SELECTED_ACCOUNT_NAME,
                    ((CAccount*)(*mAccounts)[mSelectedItemIndex].Get())->mName);
        }
    }
    return NOERROR;
}

void ChooseTypeAndAccountActivity::OnCancelButtonClicked(
    /* [in] */ IView* view)
{
    OnBackPressed();
}

void ChooseTypeAndAccountActivity::OnOkButtonClicked(
    /* [in] */ IView* view)
{
    if ((UInt32)mSelectedItemIndex == mAccounts->GetSize()) {
        // Selected "Add New Account" option
        StartChooseAccountTypeActivity();
    }
    else if (mSelectedItemIndex != SELECTED_ITEM_NONE) {
        OnAccountSelected((*mAccounts)[mSelectedItemIndex]);
    }
}

ECode ChooseTypeAndAccountActivity::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     if (data != null && data.getExtras() != null) data.getExtras().keySet();
    //     Bundle extras = data != null ? data.getExtras() : null;
    //     Log.v(TAG, "ChooseTypeAndAccountActivity.onActivityResult(reqCode=" + requestCode
    //             + ", resCode=" + resultCode + ", extras=" + extras + ")");
    // }

    // we got our result, so clear the fact that we had a pending request
    mPendingRequest = REQUEST_NULL;

    if (resultCode == RESULT_CANCELED) {
        // if canceling out of addAccount and the original state caused us to skip this,
        // finish this activity
        if (mAccounts->Begin() == mAccounts->End()) {
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
            Slogger::D(TAG, "ChooseTypeAndAccountActivity.onActivityResult: unable to find account type, pretending the request was canceled");
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
                AutoPtr<IContext> ctx = (IContext*)Probe(Elastos::Droid::Content::EIID_IContext);
                AutoPtr<IAccountManager> accountManager;
                FAIL_RETURN(CAccountManager::Get(ctx, (IAccountManager**)&accountManager));
                accountManager->GetAccounts((ArrayOf<IAccount*>**)&currentAccounts);
                AutoPtr<HashSet<AutoPtr<IAccount> > > preExistingAccounts =
                        new HashSet<AutoPtr<IAccount> >();
                for (Int32 i = 0; i < mExistingAccounts->GetLength(); ++i) {
                    preExistingAccounts->Insert(
                            (IAccount*)(*mExistingAccounts)[i]->Probe(EIID_IAccount));
                }
                for(Int32 i = 0; i < currentAccounts->GetLength(); ++i) {
                    if (preExistingAccounts->Find((*currentAccounts)[i])
                            == preExistingAccounts->End()) {
                        AutoPtr<CAccount> account = (CAccount*)(*currentAccounts)[i];
                        accountName = account->mName;
                        accountType = account->mType;
                        break;
                    }
                }
            }

            if (!accountName.IsNull() || !accountType.IsNull()) {
                SetResultAndFinish(accountName, accountType);
                return NOERROR;
            }
        }
        Slogger::D(TAG, "ChooseTypeAndAccountActivity.onActivityResult: unable to find added account, pretending the request was canceled");
    }
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "ChooseTypeAndAccountActivity.onActivityResult: canceled");
    // }
    SetResult(IActivity::RESULT_CANCELED);
    return Finish();
}

void ChooseTypeAndAccountActivity::RunAddAccountForAuthenticator(
    /* [in] */ const String& type)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "runAddAccountForAuthenticator: " + type);
    // }
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
    accountManager->AddAccount(type, authTokenType, *requiredFeatures,
            options, NULL /* activity */, (IAccountManagerCallback*)this /* callback */,
            NULL /* Handler */, (IAccountManagerFuture**)&future);
}

ECode ChooseTypeAndAccountActivity::Run(
    /* [in] */ IAccountManagerFuture* accountManagerFuture)
{
    // try {
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
        accountManager->GetAccounts((ArrayOf<IAccount*>**)&mExistingAccounts);
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
    // } catch (OperationCanceledException e) {
    //     setResult(Activity.RESULT_CANCELED);
    //     finish();
    //     return;
    // } catch (IOException e) {
    // } catch (AuthenticatorException e) {
    // }
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
    Slogger::D(TAG, "selected account %p", account);
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
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "ChooseTypeAndAccountActivity.setResultAndFinish: "
    //             + "selected account " + accountName + ", " + accountType);
    // }
    Finish();
}

void ChooseTypeAndAccountActivity::StartChooseAccountTypeActivity()
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "ChooseAccountTypeActivity.startChooseAccountTypeActivity()");
    // }
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
    /* [in] */ List<AutoPtr<IAccount> >* accounts,
    /* [in] */ const String& selectedAccountName,
    /* [in] */ Boolean selectedAddNewAccount)
{
    // If "Add account" option was previously selected by user, preserve it across
    // orientation changes.
    if (selectedAddNewAccount) {
        return accounts->GetSize();
    }
    // search for the selected account name if present
    List<AutoPtr<IAccount> >::Iterator it = accounts->Begin();
    Int32 i = 0;
    for (; it != accounts->End(); ++it, ++i) {
        if (((CAccount*)(*it).Get())->mName.Equals(selectedAccountName)) {
            return i;
        }
    }
    // no account selected.
    return SELECTED_ITEM_NONE;
}

AutoPtr< ArrayOf<String> > ChooseTypeAndAccountActivity::GetListOfDisplayableOptions(
    /* [in] */ List<AutoPtr<IAccount> >* accounts)
{
    // List of options includes all accounts found together with "Add new account" as the
    // last item in the list.
    Int32 size = accounts->GetSize();
    AutoPtr< ArrayOf<String> > listItems = ArrayOf<String>::Alloc(size + 1);
    List<AutoPtr<IAccount> >::Iterator it = accounts->Begin();
    for (Int32 i = 0; it != accounts->End(); ++it, ++i) {
        (*listItems)[i] = ((CAccount*)(*it).Get())->mName;
    }
    AutoPtr<IResources> r;
    ASSERT_SUCCEEDED(GetResources((IResources**)&r));
    r->GetString(R::string::add_account_button_label, &(*listItems)[size]);
    return listItems;
}

AutoPtr< List<AutoPtr<IAccount> > > ChooseTypeAndAccountActivity::GetAcceptableAccountChoices(
    /* [in] */ IAccountManager* accountManager)
{
    AutoPtr< ArrayOf<IAccount*> > accounts;
    ASSERT_SUCCEEDED(accountManager->GetAccounts((ArrayOf<IAccount*>**)&accounts));
    AutoPtr< List<AutoPtr<IAccount> > > accountsToPopulate = new List<AutoPtr<IAccount> >(accounts->GetLength());
    for (Int32 i = 0; i < accounts->GetLength(); ++i) {
        AutoPtr<IAccount> account = (*accounts)[i];
        if (mSetOfAllowableAccounts != NULL
                && mSetOfAllowableAccounts->Find(account) == mSetOfAllowableAccounts->End()) {
            continue;
        }
        if (mSetOfRelevantAccountTypes != NULL
                && mSetOfRelevantAccountTypes->Find(((CAccount*)account.Get())->mType)
                        == mSetOfRelevantAccountTypes->End()) {
            continue;
        }
        accountsToPopulate->PushBack(account);
    }
    return accountsToPopulate;
}

AutoPtr< HashSet<String> > ChooseTypeAndAccountActivity::GetReleventAccountTypes(
    /* [in] */ IIntent* intent)
{
    // An account type is relevant iff it is allowed by the caller and supported by the account
      // manager.
    AutoPtr< HashSet<String> > setOfRelevantAccountTypes;
    AutoPtr< ArrayOf<String> > allowedAccountTypes;
    ASSERT_SUCCEEDED(intent->GetStringArrayExtra(EXTRA_ALLOWABLE_ACCOUNT_TYPES_STRING_ARRAY,
            (ArrayOf<String>**)&allowedAccountTypes));
    if (allowedAccountTypes != NULL) {
        setOfRelevantAccountTypes = new HashSet<String>();
        for (Int32 i = 0; i < allowedAccountTypes->GetLength(); ++i) {
            setOfRelevantAccountTypes->Insert((*allowedAccountTypes)[i]);
        }
        AutoPtr< ArrayOf<IAuthenticatorDescription*> > descs;
        AutoPtr<IContext> ctx = (IContext*)Probe(Elastos::Droid::Content::EIID_IContext);
        AutoPtr<IAccountManager> accountManager;
        ASSERT_SUCCEEDED(CAccountManager::Get(ctx, (IAccountManager**)&accountManager));
        ASSERT_SUCCEEDED(accountManager->GetAuthenticatorTypes(
                (ArrayOf<IAuthenticatorDescription*>**)&descs));
        Int32 len = descs->GetLength();
        AutoPtr<HashSet<String> > supportedAccountTypes = new HashSet<String>(len);
        for (Int32 i = 0; i < len; ++i) {
            AutoPtr<CAuthenticatorDescription> description =
                    (CAuthenticatorDescription*)(*descs)[i];
            supportedAccountTypes->Insert(description->mType);
        }
        HashSet<String>::Iterator it = setOfRelevantAccountTypes->Begin();
        for (; it != setOfRelevantAccountTypes->End();) {
            if (supportedAccountTypes->Find(*it) == supportedAccountTypes->End()) {
                setOfRelevantAccountTypes->Erase(it++);
            }
            else {
                ++it;
            }
        }
    }
    return setOfRelevantAccountTypes;
}

AutoPtr< HashSet<AutoPtr<IAccount> > > ChooseTypeAndAccountActivity::GetAllowableAccountSet(
    /* [in] */ IIntent* intent)
{
    AutoPtr< HashSet<AutoPtr<IAccount> > > setOfAllowableAccounts;
    AutoPtr<IObjectContainer> validAccounts;
    intent->GetParcelableArrayListExtra(EXTRA_ALLOWABLE_ACCOUNTS_ARRAYLIST,
            (IObjectContainer**)&validAccounts);
    if (validAccounts != NULL) {
        Int32 count;
        validAccounts->GetObjectCount(&count);
        setOfAllowableAccounts = new HashSet<AutoPtr<IAccount> >(count);
        AutoPtr<IObjectEnumerator> enumerator;
        validAccounts->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
        Boolean hasNext = FALSE;
        while(enumerator->MoveNext(&hasNext), hasNext) {
            AutoPtr<IParcelable> parcelable;
            enumerator->Current((IInterface**)&parcelable);
            setOfAllowableAccounts->Insert((IAccount*)parcelable->Probe(EIID_IAccount));
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
        descriptionView->SetVisibility(IView::GONE);
    }
}

ChooseTypeAndAccountActivity::AdapterViewOnItemClickListener::AdapterViewOnItemClickListener(
    /* [in] */ ChooseTypeAndAccountActivity* host)
    : mHost(host)
{}

CAR_INTERFACE_IMPL(ChooseTypeAndAccountActivity::AdapterViewOnItemClickListener,
        IAdapterViewOnItemClickListener);

ECode ChooseTypeAndAccountActivity::AdapterViewOnItemClickListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    mHost->mSelectedItemIndex = position;
    return mHost->mOkButton->SetEnabled(TRUE);
}

void ChooseTypeAndAccountActivity::PopulateUIAccountList(
    /* [in] */ ArrayOf<String>* listItems)
{
    AutoPtr<IView> view;
    ASSERT_SUCCEEDED(FindViewById(R::id::list, (IView**)&view));
    AutoPtr<IListView> list = IListView::Probe(view);

    AutoPtr<IContext> ctx = (IContext*)Probe(Elastos::Droid::Content::EIID_IContext);
    AutoPtr<IObjectContainer> items;
    CObjectContainer::New((IObjectContainer**)&items);
    for (Int32 i = 0; i < listItems->GetLength(); ++i) {
        AutoPtr<ICharSequence> csq;
        CString::New((*listItems)[i], (ICharSequence**)&csq);
        items->Add((IInterface*)csq);
    }
    AutoPtr<IArrayAdapter> arrayAdapter;
    ASSERT_SUCCEEDED(CArrayAdapter::New(ctx,
            R::layout::simple_list_item_single_choice, items,
            (IArrayAdapter**)&arrayAdapter));
    AutoPtr<IAdapter> adapter = (IAdapter*)arrayAdapter->Probe(
            Elastos::Droid::Widget::EIID_IAdapter);
    list->SetAdapter(adapter);
    list->SetChoiceMode(IListView::CHOICE_MODE_SINGLE);
    list->SetItemsCanFocus(FALSE);
    list->SetOnItemClickListener(
            (IAdapterViewOnItemClickListener*)new AdapterViewOnItemClickListener(this));
    if (mSelectedItemIndex != SELECTED_ITEM_NONE) {
        list->SetItemChecked(mSelectedItemIndex, TRUE);
        // if (Log.isLoggable(TAG, Log.VERBOSE)) {
        //     Log.v(TAG, "List item " + mSelectedItemIndex + " should be selected");
        // }
    }
}

}  //namespace Accounts
}  //namespace Droid
}  //namespace Elastos
