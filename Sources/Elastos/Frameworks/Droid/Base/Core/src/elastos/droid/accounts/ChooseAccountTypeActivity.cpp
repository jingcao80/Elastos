
#include "elastos/droid/accounts/ChooseAccountTypeActivity.h"
#include "elastos/droid/accounts/ChooseTypeAndAccountActivity.h"
#include "elastos/droid/accounts/CAuthenticatorDescription.h"
#include "elastos/droid/accounts/CAccountManager.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::CString;

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::EIID_IAdapter;

namespace Elastos {
namespace Droid {
namespace Accounts {

ChooseAccountTypeActivity::AuthInfo::AuthInfo(
    /* [in] */ IAuthenticatorDescription* desc,
    /* [in] */ const String& name,
    /* [in] */ IDrawable* drawable)
    : mDesc(desc)
    , mName(name)
    , mDrawable(drawable)
{}

PInterface ChooseAccountTypeActivity::AuthInfo::Probe(
    /* [in]  */ REIID riid)
{
    if ( riid == EIID_IInterface) {
        return (IInterface*)this;
    }
    return NULL;
}

UInt32 ChooseAccountTypeActivity::AuthInfo::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 ChooseAccountTypeActivity::AuthInfo::Release()
{
    return ElRefBase::Release();
}

ECode ChooseAccountTypeActivity::AuthInfo::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return NOERROR;
}


PInterface ChooseAccountTypeActivity::ViewHolder::Probe(
    /* [in]  */ REIID riid)
{
    if ( riid == EIID_IInterface) {
        return (IInterface*)this;
    }
    return NULL;
}

UInt32 ChooseAccountTypeActivity::ViewHolder::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 ChooseAccountTypeActivity::ViewHolder::Release()
{
    return ElRefBase::Release();
}

ECode ChooseAccountTypeActivity::ViewHolder::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return NOERROR;
}

ChooseAccountTypeActivity::AccountArrayAdapter::AccountArrayAdapter(
    /* [in] */ IContext* context,
    /* [in] */ Int32 textViewResourceId,
    /* [in] */ List<AutoPtr<AuthInfo> >* infos)
    : mInfos(infos)
{
    AutoPtr<IObjectContainer> container;
    CObjectContainer::New((IObjectContainer**)&container);
    List<AutoPtr<AuthInfo> >::Iterator it = infos->Begin();
    for (; it != infos->End(); ++it) {
        container->Add((IInterface*)(*it));
    }
    ArrayAdapter(context, textViewResourceId, container);
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE,
            (IInterface**)&mLayoutInflater);
}

PInterface ChooseAccountTypeActivity::AccountArrayAdapter::Probe(
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

UInt32 ChooseAccountTypeActivity::AccountArrayAdapter::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 ChooseAccountTypeActivity::AccountArrayAdapter::Release()
{
    return ElRefBase::Release();
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::GetInterfaceID(
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
    else if (pObject == (IInterface*)(IListAdapter*)this) {
        *pIID = Elastos::Droid::Widget::EIID_IListAdapter;
    }
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

ECode ChooseAccountTypeActivity::AccountArrayAdapter::Add(
    /* [in] */ IInterface* object)
{
    return ArrayAdapter::Add(object);
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::AddAll(
    /* [in] */ IObjectContainer* collection)
{
    return ArrayAdapter::AddAll(collection);
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::AddAll(
    /* [in] */ ArrayOf<IInterface* >* items)
{
    return ArrayAdapter::AddAll(items);
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::Insert(
    /* [in] */ IInterface* object,
    /* [in] */ Int32 index)
{
    return ArrayAdapter::Insert(object, index);
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::Remove(
    /* [in] */ IInterface* object)
{
    return ArrayAdapter::Remove(object);
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::Clear()
{
    return ArrayAdapter::Clear();
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::Sort(
    /* [in] */ IComparator* comparator)
{
    return ArrayAdapter::Sort(comparator);
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::SetNotifyOnChange(
    /* [in] */ Boolean notifyOnChange)
{
    return ArrayAdapter::SetNotifyOnChange(notifyOnChange);
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context);
    AutoPtr<IContext> ctx = ArrayAdapter::GetContext();
    *context = ctx;
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::GetPosition(
    /* [in] */ IInterface* item,
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position);
    *position = ArrayAdapter::GetPosition(item);
    return NOERROR;
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::SetDropDownViewResource(
    /* [in] */ Int32 resource)
{
    return ArrayAdapter::SetDropDownViewResource(resource);
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::NotifyDataSetChanged()
{
    return ArrayAdapter::NotifyDataSetChanged();
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::NotifyDataSetInvalidated()
{
    return ArrayAdapter::NotifyDataSetInvalidated();
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::AreAllItemsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = ArrayAdapter::AreAllItemsEnabled();
    return NOERROR;
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = ArrayAdapter::IsEnabled(position);
    return NOERROR;
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::GetDropDownView(
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

ECode ChooseAccountTypeActivity::AccountArrayAdapter::RegisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return ArrayAdapter::RegisterDataSetObserver(observer);
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::UnregisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return ArrayAdapter::UnregisterDataSetObserver(observer);
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = ArrayAdapter::GetCount();
    return NOERROR;
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);
    AutoPtr<IInterface> i = ArrayAdapter::GetItem(position);
    *item = i;
    REFCOUNT_ADD(*item);
    return NOERROR;
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* itemId)
{
    VALIDATE_NOT_NULL(itemId);
    *itemId = ArrayAdapter::GetItemId(position);
    return NOERROR;
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::HasStableIds(
    /* [out] */ Boolean* hasStableIds)
{
    VALIDATE_NOT_NULL(hasStableIds);
    *hasStableIds = ArrayAdapter::HasStableIds();
    return NOERROR;
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView_,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    AutoPtr<ViewHolder> holder;
    AutoPtr<IView> convertView = convertView_;
    if (convertView == NULL) {
        mLayoutInflater->Inflate(R::layout::choose_account_row, NULL, (IView**)&convertView);
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

ECode ChooseAccountTypeActivity::AccountArrayAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* viewType)
{
    VALIDATE_NOT_NULL(viewType);
    *viewType = ArrayAdapter::GetItemViewType(position);
    return NOERROR;
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = ArrayAdapter::GetViewTypeCount();
    return NOERROR;
}

ECode ChooseAccountTypeActivity::AccountArrayAdapter::IsEmpty(
     /* [out] */ Boolean* isEmpty)
{
    VALIDATE_NOT_NULL(isEmpty);
    *isEmpty = ArrayAdapter::IsEmpty();
    return NOERROR;
}


const String ChooseAccountTypeActivity::TAG("AccountChooser");

ChooseAccountTypeActivity::AdapterViewOnItemClickListener::AdapterViewOnItemClickListener(
    /* [in] */ ChooseAccountTypeActivity* host)
    : mHost(host)
{}

CAR_INTERFACE_IMPL(ChooseAccountTypeActivity::AdapterViewOnItemClickListener,
        IAdapterViewOnItemClickListener);

ECode ChooseAccountTypeActivity::AdapterViewOnItemClickListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<CAuthenticatorDescription> description = (CAuthenticatorDescription*)
            (*mHost->mAuthenticatorInfosToDisplay)[position]->mDesc.Get();
    mHost->SetResultAndFinish(description->mType);
    return NOERROR;
}

ChooseAccountTypeActivity::ChooseAccountTypeActivity()
{
    mAuthenticatorInfosToDisplay = new List<AutoPtr<AuthInfo> >();
}

ECode ChooseAccountTypeActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(Activity::OnCreate(savedInstanceState));

    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "ChooseAccountTypeActivity.onCreate(savedInstanceState="
    //             + savedInstanceState + ")");
    // }

    // Read the validAccountTypes, if present, and add them to the setOfAllowableAccountTypes
    HashSet<String> setOfAllowableAccountTypes;
    AutoPtr< ArrayOf<String> > validAccountTypes;
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    intent->GetStringArrayExtra(
            ChooseTypeAndAccountActivity::EXTRA_ALLOWABLE_ACCOUNT_TYPES_STRING_ARRAY,
            (ArrayOf<String>**)&validAccountTypes);
    if (validAccountTypes != NULL) {
        Int32 len = validAccountTypes->GetLength();
        setOfAllowableAccountTypes.Resize(len);
        for (Int32 i = 0; i < len; ++i) {
            setOfAllowableAccountTypes.Insert((*validAccountTypes)[i]);
        }
    }

    // create a map of account authenticators
    BuildTypeToAuthDescriptionMap();

    // Create a list of authenticators that are allowable. Filter out those that
    // don't match the allowable account types, if provided.
    mAuthenticatorInfosToDisplay->Resize(mTypeToAuthenticatorInfo.GetSize());
    HashMap<String, AutoPtr<AuthInfo> >::Iterator it = mTypeToAuthenticatorInfo.Begin();
    for (; it != mTypeToAuthenticatorInfo.End(); ++it) {
        String type = it->mFirst;
        AutoPtr<AuthInfo> info = it->mSecond;
        if (setOfAllowableAccountTypes.Find(type) == setOfAllowableAccountTypes.End()) {
            continue;
        }
        mAuthenticatorInfosToDisplay->PushBack(info);
    }

    if (mAuthenticatorInfosToDisplay->IsEmpty()) {
        AutoPtr<IBundle> bundle;
        ASSERT_SUCCEEDED(CBundle::New((IBundle**)&bundle));
        bundle->PutString(IAccountManager::KEY_ERROR_MESSAGE,
                String("no allowable account types"));
        AutoPtr<IIntent> intent;
        ASSERT_SUCCEEDED(CIntent::New((IIntent**)&intent));
        intent->PutExtras(bundle);
        SetResult(IActivity::RESULT_OK, intent);
        return Finish();
    }

    if (mAuthenticatorInfosToDisplay->GetSize() == 1) {
        AutoPtr<CAuthenticatorDescription> discription =
                (CAuthenticatorDescription*)(*mAuthenticatorInfosToDisplay->Begin())->mDesc.Get();
        SetResultAndFinish(discription->mType);
        return NOERROR;
    }

    SetContentView(R::layout::choose_account_type);
    // Setup the list
    AutoPtr<IListView> list;
    FAIL_RETURN(FindViewById(R::id::list, (IView**)(IListView**)&list));
    // Use an existing ListAdapter that will map an array of strings to TextViews
    AutoPtr<IContext> ctx = THIS_PROBE(IContext);
    AutoPtr<AccountArrayAdapter> aaa = new AccountArrayAdapter(ctx,
            R::layout::simple_list_item_1, mAuthenticatorInfosToDisplay);
    list->SetAdapter((IAdapter*)(aaa->Probe(EIID_IAdapter)));
    list->SetChoiceMode(IListView::CHOICE_MODE_NONE);
    list->SetTextFilterEnabled(FALSE);
    AutoPtr<IAdapterViewOnItemClickListener> listener = new AdapterViewOnItemClickListener(this);
    list->SetOnItemClickListener(listener);
    return NOERROR;
}

void ChooseAccountTypeActivity::SetResultAndFinish(
    /* [in] */ const String& type)
{
    AutoPtr<IBundle> bundle;
    ASSERT_SUCCEEDED(CBundle::New((IBundle**)&bundle));
    bundle->PutString(IAccountManager::KEY_ACCOUNT_TYPE, type);
    AutoPtr<IIntent> intent;
    ASSERT_SUCCEEDED(CIntent::New((IIntent**)&intent));
    intent->PutExtras(bundle);
    SetResult(IActivity::RESULT_OK, intent);
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Log.v(TAG, "ChooseAccountTypeActivity.setResultAndFinish: "
    //             + "selected account type " + type);
    // }
    Finish();
}

void ChooseAccountTypeActivity::BuildTypeToAuthDescriptionMap()
{
    AutoPtr<IContext> ctx = THIS_PROBE(IContext);
    AutoPtr<IAccountManager> accountManager;
    ASSERT_SUCCEEDED(CAccountManager::Get(ctx, (IAccountManager**)&accountManager));
    AutoPtr< ArrayOf<IAuthenticatorDescription*> > types;
    accountManager->GetAuthenticatorTypes((ArrayOf<IAuthenticatorDescription*>**)&types);
    for (Int32 i = 0; i < types->GetLength(); ++i) {
        AutoPtr<CAuthenticatorDescription> desc = (CAuthenticatorDescription*)(*types)[i];
        String name(NULL);
        AutoPtr<IDrawable> icon;
        // try {
        AutoPtr<IContext> authContext;
        if (FAILED(CreatePackageContext(desc->mPackageName, 0,
                (IContext**)&authContext))) {
            // Nothing we can do much here, just log
            // if (Log.isLoggable(TAG, Log.WARN)) {
            //     Log.w(TAG, "No icon name for account type " + desc.type);
            // }
            AutoPtr<AuthInfo> authInfo = new AuthInfo(desc, name, icon);
            mTypeToAuthenticatorInfo[desc->mType] = authInfo;
            continue;
        }
        AutoPtr<IResources> res;
        ASSERT_SUCCEEDED(authContext->GetResources((IResources**)&res));
        if (FAILED(res->GetDrawable(desc->mIconId, (IDrawable**)&icon))) {
            // Nothing we can do much here, just log
            // if (Log.isLoggable(TAG, Log.WARN)) {
            //     Log.w(TAG, "No icon resource for account type " + desc.type);
            // }
            AutoPtr<AuthInfo> authInfo = new AuthInfo(desc, name, icon);
            mTypeToAuthenticatorInfo[desc->mType] = authInfo;
            continue;
        }
        AutoPtr<ICharSequence> sequence;
        if (FAILED(res->GetText(desc->mLabelId, (ICharSequence**)&sequence))) {
            // Nothing we can do much here, just log
            // if (Log.isLoggable(TAG, Log.WARN)) {
            //     Log.w(TAG, "No icon resource for account type " + desc.type);
            // }
            AutoPtr<AuthInfo> authInfo = new AuthInfo(desc, name, icon);
            mTypeToAuthenticatorInfo[desc->mType] = authInfo;
            continue;
        }
        assert(sequence != NULL);
        sequence->ToString(&name);
        // } catch (PackageManager.NameNotFoundException e) {
        //     // Nothing we can do much here, just log
        //     if (Log.isLoggable(TAG, Log.WARN)) {
        //         Log.w(TAG, "No icon name for account type " + desc.type);
        //     }
        // } catch (Resources.NotFoundException e) {
        //     // Nothing we can do much here, just log
        //     if (Log.isLoggable(TAG, Log.WARN)) {
        //         Log.w(TAG, "No icon resource for account type " + desc.type);
        //     }
        // }
        AutoPtr<AuthInfo> authInfo = new AuthInfo(desc, name, icon);
        mTypeToAuthenticatorInfo[desc->mType] = authInfo;
    }
}

}  //namespace Accounts
}  //namespace Droid
}  //namespace Elastos
