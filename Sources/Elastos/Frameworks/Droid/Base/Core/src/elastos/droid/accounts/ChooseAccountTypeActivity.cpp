
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/accounts/ChooseAccountTypeActivity.h"
#include "elastos/droid/accounts/CAccountManager.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IAbsListView;

using Elastos::Core::CString;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Accounts {

//===============================================================
// ChooseAccountTypeActivity::AuthInfo
//===============================================================
ChooseAccountTypeActivity::AuthInfo::AuthInfo(
    /* [in] */ IAuthenticatorDescription* desc,
    /* [in] */ const String& name,
    /* [in] */ IDrawable* drawable)
    : mDesc(desc)
    , mName(name)
    , mDrawable(drawable)
{}

//===============================================================
// ChooseAccountTypeActivity::AccountArrayAdapter
//===============================================================
ChooseAccountTypeActivity::AccountArrayAdapter::AccountArrayAdapter(
    /* [in] */ IContext* context,
    /* [in] */ Int32 textViewResourceId,
    /* [in] */ IArrayList* infos)
    : mInfos(infos)
{
    ArrayAdapter::constructor(context, textViewResourceId, IList::Probe(infos));

    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&service);
    mLayoutInflater = ILayoutInflater::Probe(service);
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
        convertView->SetTag((IObject*)holder.Get());
    }
    else {
        AutoPtr<IInterface> obj;
        convertView->GetTag((IInterface**)&obj);
        holder = (ViewHolder*)(IObject*)obj.Get();
    }

    AutoPtr<IInterface> p;
    mInfos->Get(position, (IInterface**)&p);
    AutoPtr<AuthInfo> ai = (AuthInfo*)(IObject*)p.Get();
    AutoPtr<ICharSequence> csq;
    CString::New(ai->mName, (ICharSequence**)&csq);
    holder->mText->SetText(csq);
    holder->mIcon->SetImageDrawable(ai->mDrawable);

    *view = convertView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

//===============================================================
// ChooseAccountTypeActivity::AdapterViewOnItemClickListener
//===============================================================
const String ChooseAccountTypeActivity::TAG("AccountChooser");

CAR_INTERFACE_IMPL(ChooseAccountTypeActivity::AdapterViewOnItemClickListener, Object,
        IAdapterViewOnItemClickListener);

ChooseAccountTypeActivity::AdapterViewOnItemClickListener::AdapterViewOnItemClickListener(
    /* [in] */ ChooseAccountTypeActivity* host)
    : mHost(host)
{}

ECode ChooseAccountTypeActivity::AdapterViewOnItemClickListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IInterface> p;
    mHost->mAuthenticatorInfosToDisplay->Get(position, (IInterface**)&p);
    AutoPtr<AuthInfo> ai = (AuthInfo*)(IObject*)p.Get();
    String type;
    ai->mDesc->GetType(&type);
    mHost->SetResultAndFinish(type);
    return NOERROR;
}

//===============================================================
// ChooseAccountTypeActivity::
//===============================================================
ChooseAccountTypeActivity::ChooseAccountTypeActivity()
{
    CHashMap::New((IHashMap**)&mTypeToAuthenticatorInfo);
}

ECode ChooseAccountTypeActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(Activity::OnCreate(savedInstanceState));

    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG, "ChooseAccountTypeActivity.onCreate(savedInstanceState=%p)",
                savedInstanceState);
    }

    // Read the validAccountTypes, if present, and add them to the setOfAllowableAccountTypes
    AutoPtr<ISet> setOfAllowableAccountTypes;
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    AutoPtr<ArrayOf<String> > validAccountTypes;
    intent->GetStringArrayExtra(
            IChooseTypeAndAccountActivity::EXTRA_ALLOWABLE_ACCOUNT_TYPES_STRING_ARRAY,
            (ArrayOf<String>**)&validAccountTypes);
    if (validAccountTypes != NULL) {
        Int32 len = validAccountTypes->GetLength();
        CHashSet::New(len, (ISet**)&setOfAllowableAccountTypes);
        for (Int32 i = 0; i < len; ++i) {
            String type = (*validAccountTypes)[i];
            AutoPtr<ICharSequence> pType;
            CString::New(type, (ICharSequence**)&pType);
            setOfAllowableAccountTypes->Add(pType);
        }
    }

    // create a map of account authenticators
    BuildTypeToAuthDescriptionMap();

    // Create a list of authenticators that are allowable. Filter out those that
    // don't match the allowable account types, if provided.
    Int32 size = 0;
    mTypeToAuthenticatorInfo->GetSize(&size);
    CArrayList::New(size, (IArrayList**)&mAuthenticatorInfosToDisplay);
    AutoPtr<ISet> st;
    mTypeToAuthenticatorInfo->GetEntrySet((ISet**)&st);
    AutoPtr<IIterator> it;
    st->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(p);
        AutoPtr<IInterface> type;
        entry->GetKey((IInterface**)&type);
        AutoPtr<IInterface> info;
        entry->GetValue((IInterface**)&info);
        Boolean bCt = FALSE;
        if (setOfAllowableAccountTypes != NULL
                && !(setOfAllowableAccountTypes->Contains(type, &bCt), bCt)) {
            continue;
        }
        mAuthenticatorInfosToDisplay->Add(info);
    }

    Boolean bEmp = FALSE;
    if ((mAuthenticatorInfosToDisplay->IsEmpty(&bEmp), bEmp)) {
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

    Int32 disSize = 0;
    mAuthenticatorInfosToDisplay->GetSize(&disSize);
    if (disSize == 1) {
        AutoPtr<IInterface> p;
        mAuthenticatorInfosToDisplay->Get(0, (IInterface**)&p);
        AutoPtr<AuthInfo> ai = (AuthInfo*)(IObject*)p.Get();
        String type;
        ai->mDesc->GetType(&type);
        SetResultAndFinish(type);
        return NOERROR;
    }

    SetContentView(R::layout::choose_account_type);
    // Setup the list
    AutoPtr<IView> v;
    FAIL_RETURN(FindViewById(R::id::list, (IView**)&v));
    AutoPtr<IListView> list = IListView::Probe(v);
    // Use an existing ListAdapter that will map an array of strings to TextViews
    AutoPtr<AccountArrayAdapter> aaa = new AccountArrayAdapter(this,
            R::layout::simple_list_item_1, mAuthenticatorInfosToDisplay);
    IAdapterView::Probe(list)->SetAdapter(IAdapter::Probe(aaa));
    IAbsListView::Probe(list)->SetChoiceMode(IAbsListView::CHOICE_MODE_NONE);
    IAbsListView::Probe(list)->SetTextFilterEnabled(FALSE);
    AutoPtr<IAdapterViewOnItemClickListener> listener = new AdapterViewOnItemClickListener(this);
    IAdapterView::Probe(list)->SetOnItemClickListener(listener);
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
    if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
        Logger::V(TAG,
            "ChooseAccountTypeActivity.setResultAndFinish: selected account type %s",
            (const char*)type);
    }
    Finish();
}

void ChooseAccountTypeActivity::BuildTypeToAuthDescriptionMap()
{
    AutoPtr<IAccountManager> accountManager;
    ASSERT_SUCCEEDED(CAccountManager::Get(this, (IAccountManager**)&accountManager));
    AutoPtr<ArrayOf<IAuthenticatorDescription*> > types;
    accountManager->GetAuthenticatorTypes((ArrayOf<IAuthenticatorDescription*>**)&types);
    for (Int32 i = 0; i < types->GetLength(); ++i) {
        AutoPtr<IAuthenticatorDescription> desc = (*types)[i];
        String name(NULL);
        AutoPtr<IDrawable> icon;

        String pakName;
        desc->GetPackageName(&pakName);
        AutoPtr<IContext> authContext;
        if (FAILED(CreatePackageContext(pakName, 0,
                (IContext**)&authContext))) {
            // Nothing we can do much here, just log
            String type;
            desc->GetType(&type);
            if (Logger::IsLoggable(TAG, Logger::WARN)) {
                Logger::W(TAG, "No icon name for account type %s", (const char*)type);
            }
            AutoPtr<AuthInfo> authInfo = new AuthInfo(desc, name, icon);
            AutoPtr<ICharSequence> pType;
            CString::New(type, (ICharSequence**)&pType);
            mTypeToAuthenticatorInfo->Put(pType, (IObject*)authInfo.Get());
            continue;
        }
        Int32 iconId = 0;
        desc->GetIconId(&iconId);
        authContext->GetDrawable(iconId, (IDrawable**)&icon);
        AutoPtr<IResources> res;
        if (FAILED(authContext->GetResources((IResources**)&res))) {
            // Nothing we can do much here, just log
            String type;
            desc->GetType(&type);
            if (Logger::IsLoggable(TAG, Logger::WARN)) {
                Logger::W(TAG, "No icon resource for account type %s", (const char*)type);
            }
            AutoPtr<AuthInfo> authInfo = new AuthInfo(desc, name, icon);
            AutoPtr<ICharSequence> pType;
            CString::New(type, (ICharSequence**)&pType);
            mTypeToAuthenticatorInfo->Put(pType, (IObject*)authInfo.Get());
            continue;
        }
        Int32 labelId = 0;
        desc->GetLabelId(&labelId);
        AutoPtr<ICharSequence> sequence;
        res->GetText(labelId, (ICharSequence**)&sequence);
        assert(sequence != NULL);
        sequence->ToString(&name);
        String type;
        desc->GetType(&type);
        AutoPtr<AuthInfo> authInfo = new AuthInfo(desc, name, icon);
        AutoPtr<ICharSequence> pType;
        CString::New(type, (ICharSequence**)&pType);
        mTypeToAuthenticatorInfo->Put(pType, (IObject*)authInfo.Get());
    }
}

}  //namespace Accounts
}  //namespace Droid
}  //namespace Elastos
