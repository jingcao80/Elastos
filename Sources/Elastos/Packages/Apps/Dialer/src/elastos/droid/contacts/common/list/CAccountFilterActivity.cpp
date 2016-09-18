
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/contacts/common/list/CAccountFilterActivity.h"
#include "elastos/droid/contacts/common/list/CContactListFilter.h"
#include "elastos/droid/contacts/common/model/AccountTypeManager.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/R.h"
#include "R.h"

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::App::EIID_ILoaderManagerLoaderCallbacks;
using Elastos::Droid::App::ILoaderManager;
using Elastos::Droid::App::IActionBar;
using Elastos::Droid::Contacts::Common::List::EIID_IAccountFilterActivity;
using Elastos::Droid::Contacts::Common::List::ECLSID_CCustomContactListFilterActivity;
using Elastos::Droid::Contacts::Common::Model::AccountTypeManager;
using Elastos::Droid::Contacts::Common::Model::Account::IAccountWithDataSet;
using Elastos::Droid::Contacts::Common::Model::Account::IAccountType;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

//=================================================================
// CAccountFilterActivity::FilterLoader
//=================================================================
CAccountFilterActivity::FilterLoader::FilterLoader(
    /* [in] */ IContext* context)
    : mContext(context)
{
    AsyncTaskLoader::constructor(context);
}

ECode CAccountFilterActivity::FilterLoader::LoadInBackground(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IList> filters = CAccountFilterActivity::LoadAccountFilters(mContext);
    *result = IInterface::Probe(filters);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CAccountFilterActivity::FilterLoader::OnStartLoading()
{
    return ForceLoad();
}

ECode CAccountFilterActivity::FilterLoader::OnStopLoading()
{
    Boolean result;
    return CancelLoad(&result);
}

ECode CAccountFilterActivity::FilterLoader::OnReset()
{
    return OnStopLoading();
}


//=================================================================
// CAccountFilterActivity::MyLoaderCallbacks
//=================================================================
CAR_INTERFACE_IMPL(CAccountFilterActivity::MyLoaderCallbacks, Object, ILoaderManagerLoaderCallbacks)

ECode CAccountFilterActivity::MyLoaderCallbacks::OnCreateLoader(
    /* [in] */ Int32 id,
    /* [in] */ IBundle* args,
    /* [out] */ ILoader** loader)
{
    VALIDATE_NOT_NULL(loader)
    AutoPtr<FilterLoader> filterLoader = new FilterLoader(IContext::Probe(mHost));
    *loader = ILoader::Probe(filterLoader);
    REFCOUNT_ADD(*loader)
    return NOERROR;
}

ECode CAccountFilterActivity::MyLoaderCallbacks::OnLoadFinished(
    /* [in] */ ILoader* loader,
    /* [in] */ IInterface* data)
{
    if (data == NULL) { // Just in case...
        Logger::E(TAG, "Failed to load filters");
        return NOERROR;
    }
    AutoPtr<FilterListAdapter> listAdapter = new FilterListAdapter(IContext::Probe(mHost),
            IList::Probe(data), mHost->mCurrentFilter);
    (IAdapterView::Probe(mHost->mListView))->SetAdapter(IAdapter::Probe(listAdapter));
    return NOERROR;
}

ECode CAccountFilterActivity::MyLoaderCallbacks::OnLoaderReset(
    /* [in] */ ILoader* loader)
{
    return NOERROR;
}


//=================================================================
// CAccountFilterActivity::FilterListAdapter
//=================================================================
CAccountFilterActivity::FilterListAdapter::FilterListAdapter(
    /* [in] */ IContext* context,
    /* [in] */ IList* filters,
    /* [in] */ IContactListFilter* current)
{
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&service);
    mLayoutInflater = ILayoutInflater::Probe(service);
    mFilters = filters;
    mCurrentFilter = current;
    mAccountTypes = AccountTypeManager::GetInstance(context);
}

ECode CAccountFilterActivity::FilterListAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    return mFilters->GetSize(count);
}

ECode CAccountFilterActivity::FilterListAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* itemId)
{
    VALIDATE_NOT_NULL(itemId)
    *itemId = position;
    return NOERROR;
}

ECode CAccountFilterActivity::FilterListAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item)
    return mFilters->Get(position, item);
}

ECode CAccountFilterActivity::FilterListAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** _view)
{
    VALIDATE_NOT_NULL(_view)
    AutoPtr<IContactListFilterView> view;
    if (convertView != NULL) {
        view = IContactListFilterView::Probe(convertView);
    }
    else {
        AutoPtr<IView> temp;
        mLayoutInflater->Inflate(Elastos::Droid::Dialer::R::layout::contact_list_filter_item, parent, FALSE,
                (IView**)&temp);
        view = IContactListFilterView::Probe(temp);
    }

    Int32 size;
    mFilters->GetSize(&size);
    view->SetSingleAccount(size == 1);
    AutoPtr<IInterface> value;
    mFilters->Get(position, (IInterface**)&value);
    AutoPtr<IContactListFilter> filter = IContactListFilter::Probe(value);
    view->SetContactListFilter(filter);
    view->BindView(mAccountTypes);
    AutoPtr<IView> v = IView::Probe(view);
    v->SetTag(filter);
    Boolean equals;
    IObject::Probe(filter)->Equals(mCurrentFilter, &equals);
    v->SetActivated(equals);
    *_view = v;
    REFCOUNT_ADD(*_view)
    return NOERROR;
}


//=================================================================
// CAccountFilterActivity
//=================================================================
const String CAccountFilterActivity::TAG;
const Int32 CAccountFilterActivity::SUBACTIVITY_CUSTOMIZE_FILTER;
const Int32 CAccountFilterActivity::FILTER_LOADER_ID;

CAR_INTERFACE_IMPL_2(CAccountFilterActivity, Activity, IAccountFilterActivity, IAdapterViewOnItemClickListener)

CAR_OBJECT_IMPL(CAccountFilterActivity)

ECode CAccountFilterActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    FAIL_RETURN(Activity::OnCreate(icicle))
    SetContentView(Elastos::Droid::Dialer::R::layout::contact_list_filter);

    AutoPtr<IView> temp = FindViewById(Elastos::Droid::R::id::list);
    mListView = IListView::Probe(temp);
    IAdapterView::Probe(mListView)->SetOnItemClickListener(this);

    AutoPtr<IActionBar> actionBar;
    GetActionBar((IActionBar**)&actionBar);
    if (actionBar != NULL) {
        actionBar->SetDisplayHomeAsUpEnabled(TRUE);
    }

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    AutoPtr<IParcelable> parcelable;
    intent->GetParcelableExtra(KEY_EXTRA_CURRENT_FILTER, (IParcelable**)&parcelable);
    mCurrentFilter = IContactListFilter::Probe(parcelable);
    AutoPtr<ILoaderManager> lm;
    GetLoaderManager((ILoaderManager**)&lm);
    AutoPtr<ILoaderManagerLoaderCallbacks> cb = (ILoaderManagerLoaderCallbacks*)new MyLoaderCallbacks(this);
    AutoPtr<ILoader> l;
    lm->InitLoader(FILTER_LOADER_ID, NULL, cb, (ILoader**)&l);
    return NOERROR;
}

AutoPtr<IList> CAccountFilterActivity::LoadAccountFilters(
    /* [in] */ IContext* context)
{
    AutoPtr<IList> result;
    CArrayList::New((IList**)&result);
    AutoPtr<IList> accountFilters;
    CArrayList::New((IList**)&accountFilters);
    AutoPtr<IAccountTypeManager> accountTypes = AccountTypeManager::GetInstance(context);
    AutoPtr<IList> accounts;
    accountTypes->GetAccounts(FALSE, (IList**)&accounts);
    AutoPtr<IIterator> it;
    accounts->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        AutoPtr<IAccountWithDataSet> account = IAccountWithDataSet::Probe(next);
        String type, dataSet;
        IAccount::Probe(account)->GetType(&type);
        account->GetDataSet(&dataSet);
        AutoPtr<IAccountType> accountType;
        accountTypes->GetAccountType(type, dataSet, (IAccountType**)&accountType);
        Boolean isExtension;
        Boolean hasData;
        if ((accountType->IsExtension(&isExtension), isExtension) &&
                (account->HasData(context, &hasData), !hasData)) {
            // Hide extensions with no raw_contacts.
            continue;
        }
        AutoPtr<IDrawable> icon;
        if (accountType != NULL) {
            accountType->GetDisplayIcon(context, (IDrawable**)&icon);
        }
        String name;
        IAccount::Probe(account)->GetName(&name);
        AutoPtr<IContactListFilter> filter = CContactListFilter::CreateAccountFilter(
                type, name, dataSet, icon);
        accountFilters->Add(filter);
    }

    // Always show "All", even when there's no accounts.  (We may have local contacts)
    AutoPtr<IContactListFilter> filter = CContactListFilter::CreateFilterWithType(
            IContactListFilter::FILTER_TYPE_ALL_ACCOUNTS);
    result->Add(filter);

    Int32 count;
    accountFilters->GetSize(&count);
    if (count >= 1) {
        // If we only have one account, don't show it as "account", instead show it as "all"
        if (count > 1) {
            result->AddAll(ICollection::Probe(accountFilters));
        }
        AutoPtr<IContactListFilter> filter = CContactListFilter::CreateFilterWithType(
                IContactListFilter::FILTER_TYPE_CUSTOM);
        result->Add(filter);
    }
    return result;
}

ECode CAccountFilterActivity::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IInterface> tag;
    view->GetTag((IInterface**)&tag);
    AutoPtr<IContactListFilter> filter = IContactListFilter::Probe(tag);
    if (filter == NULL) {
        return NOERROR; // Just in case
    }
    Int32 filterType;
    if (filter->GetFilterType(&filterType), filterType == IContactListFilter::FILTER_TYPE_CUSTOM) {
        AutoPtr<IIntent> intent;
        CIntent::New(IContext::Probe(this), ECLSID_CCustomContactListFilterActivity, (IIntent**)&intent);
        StartActivityForResult(intent, SUBACTIVITY_CUSTOMIZE_FILTER);
    }
    else {
        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        intent->PutExtra(KEY_EXTRA_CONTACT_LIST_FILTER, IParcelable::Probe(filter));
        SetResult(IActivity::RESULT_OK, intent);
        Finish();
    }
    return NOERROR;
}

ECode CAccountFilterActivity::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    if (resultCode != IActivity::RESULT_OK) {
        return NOERROR;
    }

    switch (requestCode) {
        case SUBACTIVITY_CUSTOMIZE_FILTER: {
            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            AutoPtr<IContactListFilter> filter = CContactListFilter::CreateFilterWithType(
                    IContactListFilter::FILTER_TYPE_CUSTOM);
            intent->PutExtra(KEY_EXTRA_CONTACT_LIST_FILTER, IParcelable::Probe(filter));
            SetResult(IActivity::RESULT_OK, intent);
            Finish();
            break;
        }
    }
    return NOERROR;
}

ECode CAccountFilterActivity::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Int32 id;
    item->GetItemId(&id);
    switch (id) {
        case Elastos::Droid::R::id::home:
            // We have two logical "up" Activities: People and Phone.
            // Instead of having one static "up" direction, behave like back as an
            // exceptional case.
            OnBackPressed();
            *res = TRUE;
            return NOERROR;
        default:
            break;
    }
    return Activity::OnOptionsItemSelected(item, res);
}

} // List
} // Common
} // Contacts
} // Droid
} // Elastos
