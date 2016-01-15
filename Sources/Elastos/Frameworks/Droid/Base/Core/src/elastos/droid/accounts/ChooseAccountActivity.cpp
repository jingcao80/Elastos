
#include "elastos/droid/accounts/ChooseAccountActivity.h"
#include "elastos/droid/accounts/CAccount.h"
#include "elastos/droid/accounts/CAccountManager.h"
#include "elastos/droid/accounts/CAuthenticatorDescription.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>


using Elastos::Utility::Logging::Slogger;
using Elastos::Core::CString;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::EIID_IAdapter;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;

namespace Elastos {
namespace Droid {
namespace Accounts {

PInterface ChooseAccountActivity::AccountInfo::Probe(
    /* [in]  */ REIID riid)
{
    if ( riid == EIID_IInterface) {
        return (IInterface*)this;
    }
    return NULL;
}

UInt32 ChooseAccountActivity::AccountInfo::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 ChooseAccountActivity::AccountInfo::Release()
{
    return ElRefBase::Release();
}

ECode ChooseAccountActivity::AccountInfo::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return NOERROR;
}


PInterface ChooseAccountActivity::ViewHolder::Probe(
    /* [in]  */ REIID riid)
{
    if ( riid == EIID_IInterface) {
        return (IInterface*)this;
    }
    return NULL;
}

UInt32 ChooseAccountActivity::ViewHolder::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 ChooseAccountActivity::ViewHolder::Release()
{
    return ElRefBase::Release();
}

ECode ChooseAccountActivity::ViewHolder::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return NOERROR;
}


ChooseAccountActivity::AccountArrayAdapter::AccountArrayAdapter(
    /* [in] */ IContext* context,
    /* [in] */ Int32 textViewResourceId,
    /* [in] */ List<AutoPtr<AccountInfo> >* infos)
    : mInfos(infos)
{
    AutoPtr<IObjectContainer> container;
    CObjectContainer::New((IObjectContainer**)&container);
    List<AutoPtr<AccountInfo> >::Iterator it = infos->Begin();
    for (; it != infos->End(); ++it) {
        container->Add((IInterface*)(*it));
    }
    ArrayAdapter(context, textViewResourceId, container);
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE,
            (IInterface**)&mLayoutInflater);
}

ChooseAccountActivity::AccountArrayAdapter::~AccountArrayAdapter()
{
}

PInterface ChooseAccountActivity::AccountArrayAdapter::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IArrayAdapter*)this;
    }
    else if (riid == Elastos::Droid::Widget::EIID_IArrayAdapter) {
        return (IArrayAdapter*)this;
    }
    else if (riid == Elastos::Droid::Widget::EIID_IBaseAdapter) {
        return (IBaseAdapter*)this;
    }
    else if (riid == Elastos::Droid::Widget::EIID_IListAdapter) {
        return (IListAdapter*)this;
    }
    else if (riid == Elastos::Droid::Widget::EIID_ISpinnerAdapter) {
        return (ISpinnerAdapter*)this;
    }
    else if (riid == Elastos::Droid::Widget::EIID_IAdapter) {
        return (IAdapter*)(ISpinnerAdapter*)this;
    }

    return NULL;
}

UInt32 ChooseAccountActivity::AccountArrayAdapter::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 ChooseAccountActivity::AccountArrayAdapter::Release()
{
    return ElRefBase::Release();
}

ECode ChooseAccountActivity::AccountArrayAdapter::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    VALIDATE_NOT_NULL(pIID);

    if (pObject == (IInterface*)(IArrayAdapter*)this) {
        *pIID = Elastos::Droid::Widget::EIID_IArrayAdapter;
    }
    else if (pObject == (IInterface*)(IBaseAdapter*)this) {
        *pIID = Elastos::Droid::Widget::EIID_IBaseAdapter;
    }
    // else if (pObject == (IInterface*)(IListAdapter*)this) {
    //     *pIID = Elastos::Droid::Widget::EIID_IListAdapter;
    // }
    else if (pObject == (IInterface*)(ISpinnerAdapter*)this) {
        *pIID = Elastos::Droid::Widget::EIID_ISpinnerAdapter;
    }
    else if (pObject == (IInterface*)(IAdapter*)(ISpinnerAdapter*)this) {
        *pIID = Elastos::Droid::Widget::EIID_IAdapter;
    }
    else {
        return E_INVALID_ARGUMENT;
    }

    return  NOERROR;
}

ECode ChooseAccountActivity::AccountArrayAdapter::Add(
    /* [in] */ IInterface* object)
{
    return ArrayAdapter::Add(object);
}

ECode ChooseAccountActivity::AccountArrayAdapter::AddAll(
    /* [in] */ IObjectContainer* collection)
{
    return ArrayAdapter::AddAll(collection);
}

ECode ChooseAccountActivity::AccountArrayAdapter::AddAll(
    /* [in] */ ArrayOf<IInterface* >* items)
{
    return ArrayAdapter::AddAll(items);
}

ECode ChooseAccountActivity::AccountArrayAdapter::Insert(
    /* [in] */ IInterface* object,
    /* [in] */ Int32 index)
{
    return ArrayAdapter::Insert(object, index);
}

ECode ChooseAccountActivity::AccountArrayAdapter::Remove(
    /* [in] */ IInterface* object)
{
    return ArrayAdapter::Remove(object);
}

ECode ChooseAccountActivity::AccountArrayAdapter::Clear()
{
    return ArrayAdapter::Clear();
}

ECode ChooseAccountActivity::AccountArrayAdapter::Sort(
    /* [in] */ IComparator* comparator)
{
    return ArrayAdapter::Sort(comparator);
}

ECode ChooseAccountActivity::AccountArrayAdapter::SetNotifyOnChange(
    /* [in] */ Boolean notifyOnChange)
{
    return ArrayAdapter::SetNotifyOnChange(notifyOnChange);
}

ECode ChooseAccountActivity::AccountArrayAdapter::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context);
    *context = ArrayAdapter::GetContext();
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode ChooseAccountActivity::AccountArrayAdapter::GetPosition(
    /* [in] */ IInterface* item,
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position);
    *position = ArrayAdapter::GetPosition(item);
    return NOERROR;
}

ECode ChooseAccountActivity::AccountArrayAdapter::SetDropDownViewResource(
    /* [in] */ Int32 resource)
{
    return ArrayAdapter::SetDropDownViewResource(resource);
}

ECode ChooseAccountActivity::AccountArrayAdapter::NotifyDataSetChanged()
{
    return ArrayAdapter::NotifyDataSetChanged();
}

ECode ChooseAccountActivity::AccountArrayAdapter::NotifyDataSetInvalidated()
{
    return ArrayAdapter::NotifyDataSetInvalidated();
}

ECode ChooseAccountActivity::AccountArrayAdapter::AreAllItemsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = ArrayAdapter::AreAllItemsEnabled();
    return NOERROR;
}

ECode ChooseAccountActivity::AccountArrayAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = ArrayAdapter::IsEnabled(position);
    return NOERROR;
}

ECode ChooseAccountActivity::AccountArrayAdapter::GetDropDownView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> tdView = ArrayAdapter::GetDropDownView(position, convertView, parent);
    *view = tdView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode ChooseAccountActivity::AccountArrayAdapter::RegisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return ArrayAdapter::RegisterDataSetObserver(observer);
}

ECode ChooseAccountActivity::AccountArrayAdapter::UnregisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return ArrayAdapter::UnregisterDataSetObserver(observer);
}

ECode ChooseAccountActivity::AccountArrayAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = ArrayAdapter::GetCount();
    return NOERROR;
}

ECode ChooseAccountActivity::AccountArrayAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);
    AutoPtr<IInterface> i = ArrayAdapter::GetItem(position);
    *item = i;
    REFCOUNT_ADD(*item);
    return NOERROR;
}

ECode ChooseAccountActivity::AccountArrayAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* itemId)
{
    VALIDATE_NOT_NULL(itemId);
    *itemId = ArrayAdapter::GetItemId(position);
    return NOERROR;
}

ECode ChooseAccountActivity::AccountArrayAdapter::HasStableIds(
    /* [out] */ Boolean* hasStableIds)
{
    VALIDATE_NOT_NULL(hasStableIds);
    *hasStableIds = ArrayAdapter::HasStableIds();
    return NOERROR;
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
        convertView->SetTag(holder);
    }
    else {
        AutoPtr<IInterface> obj;
        convertView->GetTag((IInterface**)&obj);
        holder = (ViewHolder*)obj.Get();
    }

    AutoPtr<ICharSequence> csq;
    CString::New((*mInfos)[position]->mName, (ICharSequence**)&csq);
    holder->mText->SetText(csq);
    holder->mIcon->SetImageDrawable((*mInfos)[position]->mDrawable);

    *view = convertView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode ChooseAccountActivity::AccountArrayAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* viewType)
{
    VALIDATE_NOT_NULL(viewType);
    *viewType = ArrayAdapter::GetItemViewType(position);
    return NOERROR;
}

ECode ChooseAccountActivity::AccountArrayAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = ArrayAdapter::GetViewTypeCount();
    return NOERROR;
}

ECode ChooseAccountActivity::AccountArrayAdapter::IsEmpty(
     /* [out] */ Boolean* isEmpty)
{
    VALIDATE_NOT_NULL(isEmpty);
    *isEmpty = ArrayAdapter::IsEmpty();
    return NOERROR;
}


ChooseAccountActivity::AdapterViewOnItemClickListener::AdapterViewOnItemClickListener(
    /* [in] */ ChooseAccountActivity* host)
    : mHost(host)
{}

CAR_INTERFACE_IMPL(ChooseAccountActivity::AdapterViewOnItemClickListener,
        IAdapterViewOnItemClickListener);

ECode ChooseAccountActivity::AdapterViewOnItemClickListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IListView> parentView = (IListView*)parent->Probe(
            Elastos::Droid::Widget::EIID_IListView);
    return mHost->OnListItemClick(parentView, view, position, id);
}


const String ChooseAccountActivity::TAG("AccountManager");

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
    mAccountManagerResponse = (IAccountManagerResponse*)
            parcelable->Probe(EIID_IAccountManagerResponse);

    // KEY_ACCOUNTS is a required parameter
    if (mAccounts == NULL) {
        SetResult(RESULT_CANCELED);
        return Finish();
    }

    GetAuthDescriptions();

    Int32 len = mAccounts->GetLength();
    AutoPtr< List< AutoPtr<AccountInfo> > > accountInfos = new List< AutoPtr<AccountInfo> >(len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<CAccount> account = (CAccount*)(IAccount*)(*mAccounts)[i]->Probe(
                EIID_IAccount);
        accountInfos->PushBack(new AccountInfo(account->mName,
                GetDrawableForType(account->mType)));
    }

    FAIL_RETURN(SetContentView(R::layout::choose_account));

    // Setup the list
    AutoPtr<IListView> list;
    FAIL_RETURN(FindViewById(R::id::list, (IView**)(IListView**)&list));
    // Use an existing ListAdapter that will map an array of strings to TextViews
    AutoPtr<IContext> ctx = THIS_PROBE(IContext);
    AutoPtr<AccountArrayAdapter> aaa = new AccountArrayAdapter(ctx, R::layout::simple_list_item_1, accountInfos);
    list->SetAdapter((IAdapter*)(aaa->Probe(EIID_IAdapter)));
    list->SetChoiceMode(IListView::CHOICE_MODE_SINGLE);
    list->SetTextFilterEnabled(TRUE);
    AutoPtr<IAdapterViewOnItemClickListener> listener = new AdapterViewOnItemClickListener(this);
    list->SetOnItemClickListener(listener);
    return NOERROR;
}

void ChooseAccountActivity::GetAuthDescriptions()
{
    AutoPtr<IContext> ctx = THIS_PROBE(IContext);
    AutoPtr<IAccountManager> accountManager;
    ASSERT_SUCCEEDED(CAccountManager::Get(ctx, (IAccountManager**)&accountManager));
    AutoPtr< ArrayOf<IAuthenticatorDescription*> > descs;
    ASSERT_SUCCEEDED(accountManager->GetAuthenticatorTypes(
            (ArrayOf<IAuthenticatorDescription*>**)&descs));
    for (Int32 i = 0; i < descs->GetLength(); ++i) {
        mTypeToAuthDescription[((CAuthenticatorDescription*)(*descs)[i])->mType] =
                (*descs)[i];
    }
}

AutoPtr<IDrawable> ChooseAccountActivity::GetDrawableForType(
    /* [in] */ const String& accountType)
{
    AutoPtr<IDrawable> icon;
    HashMap<String, AutoPtr<IAuthenticatorDescription> >::Iterator it
            = mTypeToAuthDescription.Begin();
    for (; it != mTypeToAuthDescription.End(); ++it) {
        if (it->mFirst == accountType) {
            // try {
            AutoPtr<CAuthenticatorDescription> desc = (CAuthenticatorDescription*)it->mSecond.Get();
            AutoPtr<IContext> authContext;
            ASSERT_SUCCEEDED(CreatePackageContext(desc->mPackageName, 0,
                    (IContext**)&authContext));
            AutoPtr<IResources> res;
            ASSERT_SUCCEEDED(authContext->GetResources((IResources**)&res));
            res->GetDrawable(desc->mIconId, (IDrawable**)&icon);
            // } catch (PackageManager.NameNotFoundException e) {
            //     // Nothing we can do much here, just log
            //     if (Log.isLoggable(TAG, Log.WARN)) {
            //         Log.w(TAG, "No icon name for account type " + accountType);
            //     }
            // } catch (Resources.NotFoundException e) {
            //     // Nothing we can do much here, just log
            //     if (Log.isLoggable(TAG, Log.WARN)) {
            //         Log.w(TAG, "No icon resource for account type " + accountType);
            //     }
            // }
            break;
        }
    }
    return icon;
}

ECode ChooseAccountActivity::OnListItemClick(
    /* [in] */ IListView* l,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<CAccount> account = (CAccount*)(IAccount*)(*mAccounts)[position]->Probe(
            EIID_IAccount);
    Slogger::D(TAG, "selected account %p", account.Get());
    AutoPtr<IBundle> bundle;
    ASSERT_SUCCEEDED(CBundle::New((IBundle**)&bundle));
    bundle->PutString(IAccountManager::KEY_ACCOUNT_NAME, account->mName);
    bundle->PutString(IAccountManager::KEY_ACCOUNT_TYPE, account->mType);
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
