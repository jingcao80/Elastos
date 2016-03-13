
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/accounts/ChooseAccountActivity.h"
#include "elastos/droid/accounts/CAccountManager.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Accounts {

//===============================================================
// ChooseAccountActivity::AccountArrayAdapter
//===============================================================

ChooseAccountActivity::AccountArrayAdapter::AccountArrayAdapter(
    /* [in] */ IContext* context,
    /* [in] */ Int32 textViewResourceId,
    /* [in] */ ArrayOf<AccountInfo*>* infos)
    : mInfos(infos)
{
    ArrayAdapter::constructor(context, textViewResourceId, (ArrayOf<IInterface*>*)infos);
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&service);
    mLayoutInflater = ILayoutInflater::Probe(service);
}

ChooseAccountActivity::AccountArrayAdapter::~AccountArrayAdapter()
{
}

ECode ChooseAccountActivity::AccountArrayAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* cv,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    AutoPtr<ViewHolder> holder;
    AutoPtr<IView> convertView = cv;
    if (convertView == NULL) {
        FAIL_RETURN(mLayoutInflater->Inflate(R::layout::choose_account_row, NULL, (IView**)&convertView));
        holder = new ViewHolder();
        AutoPtr<IView> tempView;
        FAIL_RETURN(convertView->FindViewById(R::id::account_row_text, (IView**)&tempView));
        holder->mText = ITextView::Probe(tempView);
        tempView = NULL;
        FAIL_RETURN(convertView->FindViewById(R::id::account_row_icon, (IView**)&tempView));
        holder->mIcon = IImageView::Probe(tempView);
        convertView->SetTag((IObject*)holder.Get());
    }
    else {
        AutoPtr<IInterface> obj;
        convertView->GetTag((IInterface**)&obj);
        holder = (ViewHolder*)(IObject*)obj.Get();
    }

    AutoPtr<ICharSequence> csq;
    CString::New((*mInfos)[position]->mName, (ICharSequence**)&csq);
    holder->mText->SetText(csq);
    holder->mIcon->SetImageDrawable((*mInfos)[position]->mDrawable);

    *view = convertView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

//===============================================================
// ChooseAccountActivity::AdapterViewOnItemClickListener
//===============================================================
CAR_INTERFACE_IMPL(ChooseAccountActivity::AdapterViewOnItemClickListener, Object,
        IAdapterViewOnItemClickListener);

ChooseAccountActivity::AdapterViewOnItemClickListener::AdapterViewOnItemClickListener(
    /* [in] */ ChooseAccountActivity* host)
    : mHost(host)
{}

ECode ChooseAccountActivity::AdapterViewOnItemClickListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IListView> parentView = IListView::Probe(parent);
    return mHost->OnListItemClick(parentView, view, position, id);
}

//===============================================================
// ChooseAccountActivity::
//===============================================================
const String ChooseAccountActivity::TAG("AccountManager");

CAR_INTERFACE_IMPL(ChooseAccountActivity, Activity, IChooseAccountActivity)

ECode ChooseAccountActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(Activity::OnCreate(savedInstanceState));

    AutoPtr<IIntent> intent;
    FAIL_RETURN(GetIntent((IIntent**)&intent));
    FAIL_RETURN(intent->GetParcelableArrayExtra(IAccountManager::KEY_ACCOUNTS,
            (ArrayOf<IParcelable*>**)&mAccounts));
    AutoPtr<IParcelable> parcelable;
    FAIL_RETURN(intent->GetParcelableExtra(IAccountManager::KEY_ACCOUNT_MANAGER_RESPONSE,
            (IParcelable**)&parcelable));
    mAccountManagerResponse = IAccountManagerResponse::Probe(parcelable);

    // KEY_ACCOUNTS is a required parameter
    if (mAccounts == NULL) {
        SetResult(RESULT_CANCELED);
        return Finish();
    }

    GetAuthDescriptions();

    Int32 len = mAccounts->GetLength();
    AutoPtr<ArrayOf<AccountInfo*> > accountInfos = ArrayOf<AccountInfo*>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IAccount> account = IAccount::Probe((*mAccounts)[i]);
        String name, type;
        account->GetName(&name);
        account->GetType(&type);
        (*accountInfos)[i] = new AccountInfo(name, GetDrawableForType(type));
    }

    FAIL_RETURN(SetContentView(R::layout::choose_account));

    // Setup the list
    AutoPtr<IView> v;
    FAIL_RETURN(FindViewById(R::id::list, (IView**)&v));
    AutoPtr<IListView> list = IListView::Probe(v);
    // Use an existing ListAdapter that will map an array of strings to TextViews
    AutoPtr<AccountArrayAdapter> aaa = new AccountArrayAdapter(this, R::layout::simple_list_item_1, accountInfos);
    IAdapterView::Probe(list)->SetAdapter(IAdapter::Probe(aaa));
    IAbsListView::Probe(list)->SetChoiceMode(IAbsListView::CHOICE_MODE_SINGLE);
    IAbsListView::Probe(list)->SetTextFilterEnabled(TRUE);
    AutoPtr<IAdapterViewOnItemClickListener> listener = new AdapterViewOnItemClickListener(this);
    IAdapterView::Probe(list)->SetOnItemClickListener(listener);
    return NOERROR;
}

void ChooseAccountActivity::GetAuthDescriptions()
{
    AutoPtr<IAccountManager> accountManager;
    ASSERT_SUCCEEDED(CAccountManager::Get(this, (IAccountManager**)&accountManager));
    AutoPtr<ArrayOf<IAuthenticatorDescription*> > descs;
    ASSERT_SUCCEEDED(accountManager->GetAuthenticatorTypes(
            (ArrayOf<IAuthenticatorDescription*>**)&descs));
    for (Int32 i = 0; i < descs->GetLength(); ++i) {
        AutoPtr<IAuthenticatorDescription> desc = (*descs)[i];
        String type;
        desc->GetType(&type);
        AutoPtr<ICharSequence> cs;
        CString::New(type, (ICharSequence**)&cs);
        mTypeToAuthDescription->Put(cs, desc);
    }
}

AutoPtr<IDrawable> ChooseAccountActivity::GetDrawableForType(
    /* [in] */ const String& accountType)
{
    AutoPtr<ICharSequence> pAccType;
    CString::New(accountType, (ICharSequence**)&pAccType);
    AutoPtr<IDrawable> icon;
    Boolean bCK = FALSE;
    if ((mTypeToAuthDescription->ContainsKey(pAccType, &bCK), bCK)) {
        AutoPtr<IInterface> pVal;
        mTypeToAuthDescription->Get(pAccType, (IInterface**)&pVal);
        AutoPtr<IAuthenticatorDescription> desc = IAuthenticatorDescription::Probe(pVal);
        String pakName;
        desc->GetPackageName(&pakName);
        AutoPtr<IContext> authContext;
        ASSERT_SUCCEEDED(CreatePackageContext(pakName, 0,
                (IContext**)&authContext));
        Int32 iconId = 0;
        desc->GetIconId(&iconId);
        authContext->GetDrawable(iconId, (IDrawable**)&icon);
    }
    return icon;
}

ECode ChooseAccountActivity::OnListItemClick(
    /* [in] */ IListView* l,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IAccount> account = IAccount::Probe((*mAccounts)[position]);
    Logger::D(TAG, "selected account %p", account.Get());
    AutoPtr<IBundle> bundle;
    ASSERT_SUCCEEDED(CBundle::New((IBundle**)&bundle));
    String name, type;
    account->GetName(&name);
    account->GetType(&type);
    bundle->PutString(IAccountManager::KEY_ACCOUNT_NAME, name);
    bundle->PutString(IAccountManager::KEY_ACCOUNT_TYPE, type);
    mResult = bundle;
    return Finish();
}

ECode ChooseAccountActivity::Finish()
{
    if (mAccountManagerResponse != NULL) {
        if (mResult != NULL) {
            return mAccountManagerResponse->OnResult(mResult);
        }
        else {
            return mAccountManagerResponse->OnError(
                    IAccountManager::ERROR_CODE_CANCELED, String("canceled"));
        }
    }
    return Activity::Finish();
}

}  //namespace Accounts
}  //namespace Droid
}  //namespace Elastos
