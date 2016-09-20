
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Preference.h"
#include "Elastos.Droid.Provider.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/contacts/common/list/CCustomContactListFilterActivity.h"
#include "elastos/droid/contacts/common/list/CGroupDelta.h"
#include "elastos/droid/contacts/common/model/AccountTypeManager.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/R.h"
#include "R.h"

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::App::IProgressDialogHelper;
using Elastos::Droid::App::CProgressDialogHelper;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IActionBar;
using Elastos::Droid::App::ILoaderManager;
using Elastos::Droid::App::EIID_ILoaderManagerLoaderCallbacks;
using Elastos::Droid::Contacts::Common::List::EIID_ICustomContactListFilterActivity;
using Elastos::Droid::Contacts::Common::Model::IValuesDelta;
using Elastos::Droid::Contacts::Common::Model::AccountTypeManager;
using Elastos::Droid::Contacts::Common::Model::Account::IAccountWithDataSet;
using Elastos::Droid::Contacts::Common::Model::Account::IAccountType;
using Elastos::Droid::Contacts::Common::Util::ECLSID_CEmptyService;
using Elastos::Droid::Content::IEntityIterator;
using Elastos::Droid::Content::IEntity;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentProviderOperation;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentProviderResult;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Preference::IPreferenceManagerHelper;
using Elastos::Droid::Preference::CPreferenceManagerHelper;
using Elastos::Droid::Provider::IContactsContractGroups;
using Elastos::Droid::Provider::CContactsContractGroups;
using Elastos::Droid::Provider::IContactsContractSyncColumns;
using Elastos::Droid::Provider::IContactsContractGroupsColumns;
using Elastos::Droid::Provider::IContactsContract;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::EIID_IOnMenuItemClickListener;
using Elastos::Droid::View::EIID_IViewOnCreateContextMenuListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IExpandableListContextMenuInfo;
using Elastos::Droid::Widget::EIID_IExpandableListViewOnChildClickListener;
using Elastos::Droid::Widget::IExpandableListViewHelper;
using Elastos::Droid::Widget::CExpandableListViewHelper;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::IInteger64;
using Elastos::Core::EIID_IComparator;
using Elastos::Utility::IList;
using Elastos::Utility::IIterator;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

//=================================================================
// CCustomContactListFilterActivity::CustomFilterConfigurationLoader
//=================================================================
CCustomContactListFilterActivity::CustomFilterConfigurationLoader::CustomFilterConfigurationLoader(
    /* [in] */ IContext* context)
{
    AsyncTaskLoader::constructor(context);
}

ECode CCustomContactListFilterActivity::CustomFilterConfigurationLoader::LoadInBackground(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IAccountTypeManager> accountTypes = AccountTypeManager::GetInstance(context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<AccountSet> accounts = new AccountSet();
    AutoPtr<IList> as;
    accountTypes->GetAccounts(FALSE, (IList**)&as);
    AutoPtr<IIterator> it;
    as->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<IAccountWithDataSet> account = IAccountWithDataSet::Probe(value);
        AutoPtr<IAccountType> accountType;
        accountTypes->GetAccountTypeForAccount(account, (IAccountType**)&accountType);
        Boolean isExtension, hasData;
        if ((accountType->IsExtension(&isExtension), isExtension)
                && (account->HasData(context, &hasData), !hasData)) {
            // Extension with no data -- skip.
            continue;
        }

        AutoPtr<IAccount> a = IAccount::Probe(account);
        String name;
        a->GetName(&name);
        String type;
        a->GetType(&type);
        String dataSet;
        account->GetDataSet(&dataSet);
        AutoPtr<AccountDisplay> accountDisplay =
                new AccountDisplay(resolver, name, type, dataSet);

        AutoPtr<IContactsContractGroups> groups;
        CContactsContractGroups::AcquireSingleton((IContactsContractGroups**)&groups);
        AutoPtr<IUri> uri;
        groups->GetCONTENT_URI((IUri**)&uri);
        AutoPtr<IUriBuilder> groupsUri;
        uri->BuildUpon((IUriBuilder**)&groupsUri);
        groupsUri->AppendQueryParameter(IContactsContractSyncColumns::ACCOUNT_NAME, name);
        groupsUri->AppendQueryParameter(IContactsContractSyncColumns::ACCOUNT_TYPE, type);
        if (!dataSet.IsNull()) {
            groupsUri->AppendQueryParameter(IContactsContractGroupsColumns::DATA_SET, dataSet);
            AutoPtr<IUri> out;
            groupsUri->Build((IUri**)&out);
        }
        AutoPtr<IUri> out;
        groupsUri->Build((IUri**)&out);
        AutoPtr<ICursor> cursor;
        resolver->Query(out, NULL, String(NULL), NULL, String(NULL), (ICursor**)&cursor);
        AutoPtr<IEntityIterator> iterator;
        groups->NewEntityIterator(cursor, (IEntityIterator**)&iterator);
        // try {
        Boolean hasGroups = FALSE;

        // Create entries for each known group
        Boolean next;
        while (iterator->HasNext(&next), next) {
            AutoPtr<IEntity> entry;
            iterator->GetNext((IEntity**)&entry);
            AutoPtr<IContentValues> values;
            entry->GetEntityValues((IContentValues**)&values);
            AutoPtr<IGroupDelta> group = CGroupDelta::FromBefore(values);
            accountDisplay->AddGroup(group);
            hasGroups = TRUE;
        }
        // Create single entry handling ungrouped status
        accountDisplay->mUngrouped =
                CGroupDelta::FromSettings(resolver, name, type, dataSet, hasGroups);
        accountDisplay->AddGroup(accountDisplay->mUngrouped);
        // } finally {
        //     iterator.close();
        // }
        iterator->Close();

        accounts->Add((IObject*)accountDisplay.Get());
    }

    *result = accounts->Probe(EIID_IInterface);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CCustomContactListFilterActivity::CustomFilterConfigurationLoader::DeliverResult(
    /* [in] */ IInterface* cursor)
{
    Boolean isReset;
    if (IsReset(&isReset), isReset) {
        return NOERROR;
    }

    mAccountSet = (AccountSet*)IArrayList::Probe(cursor);

    Boolean isStarted;
    if (IsStarted(&isStarted), isStarted) {
        return AsyncTaskLoader::DeliverResult(cursor);
    }
    return NOERROR;
}

ECode CCustomContactListFilterActivity::CustomFilterConfigurationLoader::OnStartLoading()
{
    if (mAccountSet != NULL) {
        DeliverResult(mAccountSet->Probe(EIID_IInterface));
    }
    Boolean result;
    if ((TakeContentChanged(&result), result) || mAccountSet == NULL) {
        ForceLoad();
    }
    return NOERROR;
}

ECode CCustomContactListFilterActivity::CustomFilterConfigurationLoader::OnStopLoading()
{
    Boolean result;
    return CancelLoad(&result);
}

ECode CCustomContactListFilterActivity::CustomFilterConfigurationLoader::OnReset()
{
    FAIL_RETURN(AsyncTaskLoader::OnReset())
    OnStopLoading();
    mAccountSet = NULL;
    return NOERROR;
}


//=================================================================
// CCustomContactListFilterActivity::AccountSet
//=================================================================
AutoPtr<IArrayList> CCustomContactListFilterActivity::AccountSet::BuildDiff()
{
    AutoPtr<IArrayList> diff;
    CArrayList::New((IArrayList**)&diff);
    AutoPtr<IIterator> it;
    GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        AutoPtr<AccountDisplay> account = (AccountDisplay*)(IObject*)next.Get();
        account->BuildDiff(diff);
    }
    return diff;
}


//=================================================================
// CCustomContactListFilterActivity::AccountDisplay
//=================================================================
CCustomContactListFilterActivity::AccountDisplay::AccountDisplay(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& accountName,
    /* [in] */ const String& accountType,
    /* [in] */ const String& dataSet)
    : mName(accountName)
    , mType(accountType)
    , mDataSet(dataSet)
{
    CArrayList::New((IArrayList**)&mUnsyncedGroups);
    CArrayList::New((IArrayList**)&mSyncedGroups);
}

void CCustomContactListFilterActivity::AccountDisplay::AddGroup(
    /* [in] */ IGroupDelta* group)
{
    Boolean result;
    if (group->GetShouldSync(&result), result) {
        mSyncedGroups->Add(group);
    }
    else {
        mUnsyncedGroups->Add(group);
    }
}

void CCustomContactListFilterActivity::AccountDisplay::SetShouldSync(
    /* [in] */ Boolean shouldSync)
{
    AutoPtr<IIterator> oppositeChildren;
    if (shouldSync) {
        mUnsyncedGroups->GetIterator((IIterator**)&oppositeChildren);
    }
    else {
        mSyncedGroups->GetIterator((IIterator**)&oppositeChildren);
    }
    Boolean hasNext;
    while (oppositeChildren->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        oppositeChildren->GetNext((IInterface**)&next);
        AutoPtr<IGroupDelta> child = IGroupDelta::Probe(next);
        SetShouldSync(child, shouldSync, FALSE);
        oppositeChildren->Remove();
    }
}

void CCustomContactListFilterActivity::AccountDisplay::SetShouldSync(
    /* [in] */ IGroupDelta* child,
    /* [in] */ Boolean shouldSync)
{
    SetShouldSync(child, shouldSync, TRUE);
}

void CCustomContactListFilterActivity::AccountDisplay::SetShouldSync(
    /* [in] */ IGroupDelta* child,
    /* [in] */ Boolean shouldSync,
    /* [in] */ Boolean attemptRemove)
{
    child->PutShouldSync(shouldSync);
    if (shouldSync) {
        if (attemptRemove) {
            mUnsyncedGroups->Remove(child);
        }
        mSyncedGroups->Add(child);
        AutoPtr<ICollections> collections;
        CCollections::AcquireSingleton((ICollections**)&collections);
        collections->Sort(IList::Probe(mSyncedGroups), CCustomContactListFilterActivity::sIdComparator);
    }
    else {
        if (attemptRemove) {
            mSyncedGroups->Remove(child);
        }
        mUnsyncedGroups->Add(child);
    }
}

void CCustomContactListFilterActivity::AccountDisplay::BuildDiff(
    /* [in] */ IArrayList* diff)
{
    AutoPtr<IIterator> syncedIt;
    mSyncedGroups->GetIterator((IIterator**)&syncedIt);
    Boolean hasNext;
    while (syncedIt->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        syncedIt->GetNext((IInterface**)&next);
        AutoPtr<IGroupDelta> group = IGroupDelta::Probe(next);
        AutoPtr<IContentProviderOperation> oper;
        group->BuildDiff((IContentProviderOperation**)&oper);
        if (oper != NULL) diff->Add(oper);
    }
    AutoPtr<IIterator> unsyncedIt;
    mUnsyncedGroups->GetIterator((IIterator**)&unsyncedIt);
    while (unsyncedIt->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        unsyncedIt->GetNext((IInterface**)&next);
        AutoPtr<IGroupDelta> group = IGroupDelta::Probe(next);
        AutoPtr<IContentProviderOperation> oper;
        group->BuildDiff((IContentProviderOperation**)&oper);
        if (oper != NULL) diff->Add(oper);
    }
}


//=================================================================
// CCustomContactListFilterActivity::DisplayAdapter
//=================================================================
CCustomContactListFilterActivity::DisplayAdapter::DisplayAdapter(
    /* [in] */ IContext* context)
    : mChildWithPhones(FALSE)
{
    mContext = context;
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&service);
    mInflater = ILayoutInflater::Probe(service);
    mAccountTypes = AccountTypeManager::GetInstance(context);
}

void CCustomContactListFilterActivity::DisplayAdapter::SetAccounts(
    /* [in] */ AccountSet* accounts)
{
    mAccounts = accounts;
    NotifyDataSetChanged();
}

void CCustomContactListFilterActivity::DisplayAdapter::SetChildDescripWithPhones(
    /* [in] */ Boolean withPhones)
{
    mChildWithPhones = withPhones;
}

ECode CCustomContactListFilterActivity::DisplayAdapter::GetGroupView(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Boolean isExpanded,
    /* [in] */ IView* _convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    AutoPtr<IView> convertView = _convertView;
    if (convertView == NULL) {
        mInflater->Inflate(Elastos::Droid::Dialer::R::layout::custom_contact_list_filter_account,
                parent, FALSE, (IView**)&convertView);
    }

    AutoPtr<IView> temp1;
    convertView->FindViewById(Elastos::Droid::R::id::text1, (IView**)&temp1);
    AutoPtr<ITextView> text1 = ITextView::Probe(temp1);
    AutoPtr<IView> temp2;
    convertView->FindViewById(Elastos::Droid::R::id::text2, (IView**)&temp2);
    AutoPtr<ITextView> text2 = ITextView::Probe(temp2);

    AutoPtr<IInterface> group;
    GetGroup(groupPosition, (IInterface**)&group);
    AutoPtr<AccountDisplay> account = (AccountDisplay*)(IObject*)group.Get();

    AutoPtr<IAccountType> accountType;
    mAccountTypes->GetAccountType(account->mType, account->mDataSet, (IAccountType**)&accountType);

    AutoPtr<ICharSequence> nameCS;
    CString::New(account->mName, (ICharSequence**)&nameCS);
    text1->SetText(nameCS);
    IView::Probe(text1)->SetVisibility(account->mName.IsNull() ? IView::GONE : IView::VISIBLE);
    AutoPtr<ICharSequence> label;
    accountType->GetDisplayLabel(mContext, (ICharSequence**)&label);
    text2->SetText(label);

    *view = convertView;
    REFCOUNT_ADD(*view)
    return NOERROR;
}

ECode CCustomContactListFilterActivity::DisplayAdapter::GetChildView(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition,
    /* [in] */ Boolean isLastChild,
    /* [in] */ IView* _convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    AutoPtr<IView> convertView = _convertView;
    if (convertView == NULL) {
        mInflater->Inflate(Elastos::Droid::Dialer::R::layout::custom_contact_list_filter_group,
                parent, FALSE, (IView**)&convertView);
    }

    AutoPtr<IView> temp1;
    convertView->FindViewById(Elastos::Droid::R::id::text1, (IView**)&temp1);
    AutoPtr<ITextView> text1 = ITextView::Probe(temp1);
    AutoPtr<IView> temp2;
    convertView->FindViewById(Elastos::Droid::R::id::text2, (IView**)&temp2);
    AutoPtr<ITextView> text2 = ITextView::Probe(temp2);
    AutoPtr<IView> temp3;
    convertView->FindViewById(Elastos::Droid::R::id::checkbox, (IView**)&temp3);
    AutoPtr<ICheckBox> checkbox = ICheckBox::Probe(temp3);

    AutoPtr<IInterface> group;
    GetGroup(groupPosition, (IInterface**)&group);
    AutoPtr<AccountDisplay> account = (AccountDisplay*)(IObject*)group.Get();

    AutoPtr<IInterface> childTemp;
    GetChild(groupPosition, childPosition, (IInterface**)&childTemp);
    AutoPtr<IGroupDelta> child = IGroupDelta::Probe(childTemp);
    if (child != NULL) {
        // Handle normal group, with title and checkbox
        Boolean groupVisible;
        child->GetVisible(&groupVisible);
        IView::Probe(checkbox)->SetVisibility(IView::VISIBLE);
        ICheckable::Probe(checkbox)->SetChecked(groupVisible);

        AutoPtr<ICharSequence> groupTitle;
        child->GetTitle(mContext, (ICharSequence**)&groupTitle);
        text1->SetText(groupTitle);
        IView::Probe(text2)->SetVisibility(IView::GONE);
    }
    else {
        // When unknown child, this is "more" footer view
        IView::Probe(checkbox)->SetVisibility(IView::GONE);
        text1->SetText(Elastos::Droid::Dialer::R::string::display_more_groups);
        IView::Probe(text2)->SetVisibility(IView::GONE);
    }

    *view = convertView;
    REFCOUNT_ADD(*view)
    return NOERROR;
}

ECode CCustomContactListFilterActivity::DisplayAdapter::GetChild(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition,
    /* [out] */ IInterface** child)
{
    VALIDATE_NOT_NULL(child)
    *child = NULL;
    AutoPtr<IInterface> value;
    mAccounts->Get(groupPosition, (IInterface**)&value);
    AutoPtr<AccountDisplay> account = (AccountDisplay*)(IObject*)value.Get();
    Int32 size;
    Boolean validChild = childPosition >= 0
            && (account->mSyncedGroups->GetSize(&size), childPosition < size);
    if (validChild) {
        return account->mSyncedGroups->Get(childPosition, child);
    }
    return NOERROR;
}

ECode CCustomContactListFilterActivity::DisplayAdapter::GetChildId(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition,
    /* [out] */ Int64* _id)
{
    VALIDATE_NOT_NULL(_id)
    AutoPtr<IInterface> value;
    GetChild(groupPosition, childPosition, (IInterface**)&value);
    AutoPtr<IGroupDelta> child = IGroupDelta::Probe(value);
    if (child != NULL) {
        AutoPtr<IInteger64> childId;
        IValuesDelta::Probe(child)->GetId((IInteger64**)&childId);
        Int64 id;
        *_id = childId != NULL ? (childId->GetValue(&id), id) : Elastos::Core::Math::INT64_MIN_VALUE;
    }
    else {
        *_id = Elastos::Core::Math::INT64_MIN_VALUE;
    }
    return NOERROR;
}

ECode CCustomContactListFilterActivity::DisplayAdapter::GetChildrenCount(
    /* [in] */ Int32 groupPosition,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    // Count is any synced groups, plus possible footer
    AutoPtr<IInterface> value;
    mAccounts->Get(groupPosition, (IInterface**)&value);
    AutoPtr<AccountDisplay> account = (AccountDisplay*)(IObject*)value.Get();
    Int32 size;
    account->mUnsyncedGroups->GetSize(&size);
    Boolean anyHidden = size > 0;
    account->mSyncedGroups->GetSize(&size);
    *count = size + (anyHidden ? 1 : 0);
    return NOERROR;
}

ECode CCustomContactListFilterActivity::DisplayAdapter::GetGroup(
    /* [in] */ Int32 groupPosition,
    /* [out] */ IInterface** child)
{
    VALIDATE_NOT_NULL(child)
    return mAccounts->Get(groupPosition, child);
}

ECode CCustomContactListFilterActivity::DisplayAdapter::GetGroupCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    if (mAccounts == NULL) {
        *count = 0;
        return NOERROR;
    }
    return mAccounts->GetSize(count);
}

ECode CCustomContactListFilterActivity::DisplayAdapter::GetGroupId(
    /* [in] */ Int32 groupPosition,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id)
    *id = groupPosition;
    return NOERROR;
}

ECode CCustomContactListFilterActivity::DisplayAdapter::HasStableIds(
    /* [out] */ Boolean* id)
{
    VALIDATE_NOT_NULL(id)
    *id = TRUE;
    return NOERROR;
}

ECode CCustomContactListFilterActivity::DisplayAdapter::IsChildSelectable(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition,
    /* [out] */ Boolean* selectable)
{
    VALIDATE_NOT_NULL(selectable)
    *selectable = TRUE;
    return NOERROR;
}


//=================================================================
// CCustomContactListFilterActivity::UpdateTask
//=================================================================
CCustomContactListFilterActivity::UpdateTask::UpdateTask(
    /* [in] */ IActivity* target)
    : WeakAsyncTask(IInterface::Probe(target))
{}

ECode CCustomContactListFilterActivity::UpdateTask::OnPreExecute(
    /* [in] */ IInterface* target)
{
    AutoPtr<IContext> context = IContext::Probe(target);

    AutoPtr<IProgressDialogHelper> helper;
    CProgressDialogHelper::AcquireSingleton((IProgressDialogHelper**)&helper);
    AutoPtr<ICharSequence> text;
    context->GetText(Elastos::Droid::Dialer::R::string::savingDisplayGroups, (ICharSequence**)&text);
    helper->Show(context, NULL, text, (IProgressDialog**)&mProgress);

    // Before starting this task, start an empty service to protect our
    // process from being reclaimed by the system.
    AutoPtr<IIntent> intent;
    CIntent::New(context, ECLSID_CEmptyService, (IIntent**)&intent);
    AutoPtr<IComponentName> cn;
    return context->StartService(intent, (IComponentName**)&cn);
}

ECode CCustomContactListFilterActivity::UpdateTask::DoInBackground(
    /* [in] */ IInterface* target,
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IContext> context = IContext::Probe(target);
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);

    // try {
    AutoPtr<IArrayList> diff = IArrayList::Probe((*params)[0]);
    AutoPtr<ArrayOf<IContentProviderResult*> > results;
    ECode ec = resolver->ApplyBatch(IContactsContract::AUTHORITY, diff, (ArrayOf<IContentProviderResult*>**)&results);
    if (FAILED(ec)) {
        Logger::E(CCustomContactListFilterActivity::TAG, "Problem saving display groups 0x%08x", ec);
    }
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Problem saving display groups", e);
    // } catch (OperationApplicationException e) {
    //     Log.e(TAG, "Problem saving display groups", e);
    // }

    *result = NULL;
    return NOERROR;
}

ECode CCustomContactListFilterActivity::UpdateTask::OnPostExecute(
    /* [in] */ IInterface* target,
    /* [in] */ IInterface* result)
{
    AutoPtr<IContext> context = IContext::Probe(target);

    // try {
    ECode ec = IDialogInterface::Probe(mProgress)->Dismiss();
    if (FAILED(ec)) {
        Logger::E(CCustomContactListFilterActivity::TAG, "Error dismissing progress dialog 0x%08x", ec);
    }
    // } catch (Exception e) {
    //     Log.e(TAG, "Error dismissing progress dialog", e);
    // }

    IActivity::Probe(target)->Finish();

    // Stop the service that was protecting us
    AutoPtr<IIntent> intent;
    CIntent::New(context, ECLSID_CEmptyService, (IIntent**)&intent);
    Boolean v;
    return context->StopService(intent, &v);
}


//=================================================================
// CCustomContactListFilterActivity::IdComparator
//=================================================================
CAR_INTERFACE_IMPL(CCustomContactListFilterActivity::IdComparator, Object, IComparator)

ECode CCustomContactListFilterActivity::IdComparator::Compare(
    /* [in] */  IInterface* a,
    /* [in] */  IInterface* b,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IValuesDelta> object1 = IValuesDelta::Probe(a);
    AutoPtr<IValuesDelta> object2 = IValuesDelta::Probe(b);
    AutoPtr<IInteger64> id1;
    object1->GetId((IInteger64**)&id1);
    AutoPtr<IInteger64> id2;
    object2->GetId((IInteger64**)&id2);
    if (id1 == NULL && id2 == NULL) {
        *result = 0;
        return NOERROR;
    }
    else if (id1 == NULL) {
        *result = -1;
        return NOERROR;
    }
    else if (id2 == NULL) {
        *result = 1;
        return NOERROR;
    }
    else if (id1 < id2) {
        *result = -1;
        return NOERROR;
    }
    else if (id1 > id2) {
        *result = 1;
        return NOERROR;
    }
    else {
        *result = 0;
        return NOERROR;
    }
}


//=================================================================
// CCustomContactListFilterActivity::SyncRemoveClickListener
//=================================================================
CAR_INTERFACE_IMPL(CCustomContactListFilterActivity::SyncRemoveClickListener, Object, IOnMenuItemClickListener)

ECode CCustomContactListFilterActivity::SyncRemoveClickListener::OnMenuItemClick(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* isConsumed)
{
    VALIDATE_NOT_NULL(isConsumed)
    mHost->HandleRemoveSync(mAccount, mChild, mSyncMode, mTitle);
    *isConsumed = TRUE;
    return NOERROR;
}


//=================================================================
// CCustomContactListFilterActivity::OkClickListener
//=================================================================
CAR_INTERFACE_IMPL(CCustomContactListFilterActivity::OkClickListener, Object, IDialogInterfaceOnClickListener)

ECode CCustomContactListFilterActivity::OkClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    // Mark both this group and ungrouped to stop syncing
    mAccount->SetShouldSync(mAccount->mUngrouped, FALSE);
    mAccount->SetShouldSync(mChild, FALSE);
    mHost->mAdapter->NotifyDataSetChanged();
    return NOERROR;
}


//=================================================================
// CCustomContactListFilterActivity::AddSyncClickListener
//=================================================================
CAR_INTERFACE_IMPL(CCustomContactListFilterActivity::AddSyncClickListener, Object, IOnMenuItemClickListener)

ECode CCustomContactListFilterActivity::AddSyncClickListener::OnMenuItemClick(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* isConsumed)
{
    VALIDATE_NOT_NULL(isConsumed)
    // Adding specific group for syncing
    Boolean ungrouped;
    if ((mChild->GetUngrouped(&ungrouped), ungrouped) && mSyncMode == SYNC_MODE_EVERYTHING) {
        mAccount->SetShouldSync(TRUE);
    }
    else {
        mAccount->SetShouldSync(mChild, TRUE);
    }
    mHost->mAdapter->NotifyDataSetChanged();
    *isConsumed = TRUE;
    return NOERROR;
}


CAR_INTERFACE_IMPL_3(CCustomContactListFilterActivity::InnerListener, Object,
    IViewOnClickListener, IExpandableListViewOnChildClickListener,
    IViewOnCreateContextMenuListener)

CCustomContactListFilterActivity::InnerListener::InnerListener(
    /* [in] */ CCustomContactListFilterActivity* host)
    : mHost(host)
{}


ECode CCustomContactListFilterActivity::InnerListener::OnClick(
    /* [in] */ IView* view)
{
    return mHost->OnClick(view);
}

ECode CCustomContactListFilterActivity::InnerListener::OnChildClick(
    /* [in] */ IExpandableListView* parent,
    /* [in] */ IView* v,
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition,
    /* [in] */ Int64 id,
    /* [out] */ Boolean* handle)
{
    return mHost->OnChildClick(parent, v, groupPosition, childPosition, id, handle);
}

ECode CCustomContactListFilterActivity::InnerListener::OnCreateContextMenu(
    /* [in] */ IContextMenu* menu,
    /* [in] */ IView* v,
    /* [in] */ IContextMenuInfo* menuInfo)
{
    return mHost->OnCreateContextMenu(menu, v, menuInfo);
}

//=================================================================
// CCustomContactListFilterActivity
//=================================================================
const String CCustomContactListFilterActivity::TAG("CCustomContactListFilterActivity");
const Int32 CCustomContactListFilterActivity::ACCOUNT_SET_LOADER_ID;
const Int32 CCustomContactListFilterActivity::DEFAULT_SHOULD_SYNC;
const Int32 CCustomContactListFilterActivity::DEFAULT_VISIBLE;
const Int32 CCustomContactListFilterActivity::SYNC_MODE_UNSUPPORTED;
const Int32 CCustomContactListFilterActivity::SYNC_MODE_UNGROUPED;
const Int32 CCustomContactListFilterActivity::SYNC_MODE_EVERYTHING;

AutoPtr<IComparator> CCustomContactListFilterActivity::sIdComparator = new IdComparator();

CAR_INTERFACE_IMPL_2(CCustomContactListFilterActivity, Activity,
    ICustomContactListFilterActivity,
    ILoaderManagerLoaderCallbacks)

CAR_OBJECT_IMPL(CCustomContactListFilterActivity)

ECode CCustomContactListFilterActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    FAIL_RETURN(Activity::OnCreate(icicle))
    SetContentView(Elastos::Droid::Dialer::R::layout::contact_list_filter_custom);

    AutoPtr<InnerListener> listener = new InnerListener(this);
    mList = IExpandableListView::Probe(FindViewById(Elastos::Droid::R::id::list));
    mList->SetOnChildClickListener(listener);
    IListView::Probe(mList)->SetHeaderDividersEnabled(TRUE);
    AutoPtr<IPreferenceManagerHelper> helper;
    CPreferenceManagerHelper::AcquireSingleton((IPreferenceManagerHelper**)&helper);
    AutoPtr<ISharedPreferences> temp;
    helper->GetDefaultSharedPreferences(this, (ISharedPreferences**)&temp);
    mPrefs = temp;
    mAdapter = new DisplayAdapter(this);

    AutoPtr<ILayoutInflater> inflater;
    GetLayoutInflater((ILayoutInflater**)&inflater);

    FindViewById(Elastos::Droid::Dialer::R::id::btn_done)->SetOnClickListener(listener);
    FindViewById(Elastos::Droid::Dialer::R::id::btn_discard)->SetOnClickListener(listener);

    IView::Probe(mList)->SetOnCreateContextMenuListener(listener);

    mList->SetAdapter(mAdapter);

    AutoPtr<IActionBar> actionBar;
    GetActionBar((IActionBar**)&actionBar);
    if (actionBar != NULL) {
        // android.Elastos::Droid::Dialer::R::id::home will be triggered in onOptionsItemSelected()
        actionBar->SetDisplayHomeAsUpEnabled(TRUE);
    }
    return NOERROR;
}

ECode CCustomContactListFilterActivity::OnStart()
{
    AutoPtr<ILoaderManager> manager;
    GetLoaderManager((ILoaderManager**)&manager);
    AutoPtr<ILoader> loader;
    manager->InitLoader(ACCOUNT_SET_LOADER_ID, NULL, ILoaderManagerLoaderCallbacks::Probe(this), (ILoader**)&loader);
    return Activity::OnStart();
}

ECode CCustomContactListFilterActivity::OnCreateLoader(
    /* [in] */ Int32 id,
    /* [in] */ IBundle* args,
    /* [out] */ ILoader** loader)
{
    VALIDATE_NOT_NULL(loader)
    AutoPtr<CustomFilterConfigurationLoader> customLoader = new CustomFilterConfigurationLoader(this);
    *loader = ILoader::Probe(customLoader);
    REFCOUNT_ADD(*loader)
    return NOERROR;
}

ECode CCustomContactListFilterActivity::OnLoadFinished(
    /* [in] */ ILoader* loader,
    /* [in] */ IInterface* data)
{
    mAdapter->SetAccounts((AccountSet*)IArrayList::Probe(data));
    return NOERROR;
}

ECode CCustomContactListFilterActivity::OnLoaderReset(
    /* [in] */ ILoader* loader)
{
    mAdapter->SetAccounts(NULL);
    return NOERROR;
}

AutoPtr<IUri> CCustomContactListFilterActivity::AddCallerIsSyncAdapterParameter(
    /* [in] */ IUri* uri)
{
    AutoPtr<IUriBuilder> result;
    uri->BuildUpon((IUriBuilder**)&result);
    result->AppendQueryParameter(IContactsContract::CALLER_IS_SYNCADAPTER, String("true"));
    AutoPtr<IUri> out;
    result->Build((IUri**)&out);
    return out;
}

ECode CCustomContactListFilterActivity::OnClick(
    /* [in] */ IView* view)
{
    Int32 id;
    view->GetId(&id);
    switch (id) {
        case Elastos::Droid::Dialer::R::id::btn_done: {
            DoSaveAction();
            break;
        }
        case Elastos::Droid::Dialer::R::id::btn_discard: {
            Finish();
            break;
        }
    }
    return NOERROR;
}

ECode CCustomContactListFilterActivity::OnChildClick(
    /* [in] */ IExpandableListView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition,
    /* [in] */ Int64 id,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IView> temp;
    view->FindViewById(Elastos::Droid::R::id::checkbox, (IView**)&temp);
    AutoPtr<ICheckBox> checkbox = ICheckBox::Probe(temp);

    AutoPtr<IInterface> group;
    mAdapter->GetGroup(groupPosition, (IInterface**)&group);
    AutoPtr<AccountDisplay> account = (AccountDisplay*)(IObject*)group.Get();
    AutoPtr<IInterface> childTemp;
    mAdapter->GetChild(groupPosition, childPosition, (IInterface**)&childTemp);
    AutoPtr<IGroupDelta> child = IGroupDelta::Probe(childTemp);
    if (child != NULL) {
        AutoPtr<ICheckable> c = ICheckable::Probe(checkbox);
        c->Toggle();
        Boolean isChecked;
        c->IsChecked(&isChecked);
        child->PutVisible(isChecked);
    }
    else {
        // Open context menu for bringing back unsynced
        OpenContextMenu(view);
    }
    *result = TRUE;
    return NOERROR;
}

Int32 CCustomContactListFilterActivity::GetSyncMode(
    /* [in] */ AccountDisplay* account)
{
    // TODO: read sync mode through <sync-adapter> definition
    // if (GoogleAccountType.ACCOUNT_TYPE.equals(account->mType) && account->mDataSet.IsNull()) {
    //     return SYNC_MODE_EVERYTHING;
    // } else {
        return SYNC_MODE_UNSUPPORTED;
    // }
}

ECode CCustomContactListFilterActivity::OnCreateContextMenu(
    /* [in] */ IContextMenu* menu,
    /* [in] */ IView* view,
    /* [in] */ IContextMenuInfo* menuInfo)
{
    FAIL_RETURN(Activity::OnCreateContextMenu(menu, view, menuInfo))

    // Bail if not working with expandable long-press, or if not child
    AutoPtr<IExpandableListContextMenuInfo> info = IExpandableListContextMenuInfo::Probe(menuInfo);
    if (info == NULL) return NOERROR;

    AutoPtr<IExpandableListViewHelper> helper;
    CExpandableListViewHelper::AcquireSingleton((IExpandableListViewHelper**)&helper);
    Int64 packedPosition;
    info->GetPackedPosition(&packedPosition);
    Int32 groupPosition;
    helper->GetPackedPositionGroup(packedPosition, &groupPosition);
    Int32 childPosition;
    helper->GetPackedPositionChild(packedPosition, &childPosition);

    // Skip long-press on expandable parents
    if (childPosition == -1) return NOERROR;

    AutoPtr<IInterface> group;
    mAdapter->GetGroup(groupPosition, (IInterface**)&group);
    AutoPtr<AccountDisplay> account = (AccountDisplay*)(IObject*)group.Get();
    AutoPtr<IInterface> childTemp;
    mAdapter->GetChild(groupPosition, childPosition, (IInterface**)&childTemp);
    AutoPtr<IGroupDelta> child = IGroupDelta::Probe(childTemp);

    // Ignore when selective syncing unsupported
    Int32 syncMode = GetSyncMode(account);
    if (syncMode == SYNC_MODE_UNSUPPORTED) return NOERROR;

    if (child != NULL) {
        ShowRemoveSync(menu, account, child, syncMode);
    }
    else {
        ShowAddSync(menu, account, syncMode);
    }
    return NOERROR;
}

void CCustomContactListFilterActivity::ShowRemoveSync(
    /* [in] */ IContextMenu* menu,
    /* [in] */ AccountDisplay* account,
    /* [in] */ IGroupDelta* child,
    /* [in] */ Int32 syncMode)
{
    AutoPtr<ICharSequence> title;
    child->GetTitle(this, (ICharSequence**)&title);

    menu->SetHeaderTitle(title);
    AutoPtr<IMenuItem> item;
    IMenu::Probe(menu)->Add(Elastos::Droid::Dialer::R::string::menu_sync_remove, (IMenuItem**)&item);
    AutoPtr<IOnMenuItemClickListener> listener = new SyncRemoveClickListener(account, child, syncMode, title, this);
    item->SetOnMenuItemClickListener(listener);
}

void CCustomContactListFilterActivity::HandleRemoveSync(
    /* [in] */ AccountDisplay* account,
    /* [in] */ IGroupDelta* child,
    /* [in] */ Int32 syncMode,
    /* [in] */ ICharSequence* title)
{
    Boolean shouldSyncUngrouped;
    account->mUngrouped->GetShouldSync(&shouldSyncUngrouped);
    Boolean equals;
    if (syncMode == SYNC_MODE_EVERYTHING && shouldSyncUngrouped
            && (IObject::Probe(child)->Equals(account->mUngrouped, &equals), !equals)) {
        // Warn before removing this group when it would cause ungrouped to stop syncing
        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);
        AutoPtr<ArrayOf<IInterface*> > attrs = ArrayOf<IInterface*>::Alloc(1);
        attrs->Set(0, title);
        String str;
        GetString(Elastos::Droid::Dialer::R::string::display_warn_remove_ungrouped, attrs, &str);
        AutoPtr<ICharSequence> removeMessage;
        CString::New(str, (ICharSequence**)&removeMessage);
        builder->SetTitle(Elastos::Droid::Dialer::R::string::menu_sync_remove);
        builder->SetMessage(removeMessage);
        builder->SetNegativeButton(Elastos::Droid::R::string::cancel, NULL);
        AutoPtr<IDialogInterfaceOnClickListener> listener = new OkClickListener(account, child, this);
        builder->SetPositiveButton(Elastos::Droid::R::string::ok, listener);
        AutoPtr<IAlertDialog> dialog;
        builder->Show((IAlertDialog**)&dialog);
    }
    else {
        // Mark this group to not sync
        account->SetShouldSync(child, FALSE);
        mAdapter->NotifyDataSetChanged();
    }
}

void CCustomContactListFilterActivity::ShowAddSync(
    /* [in] */ IContextMenu* menu,
    /* [in] */ AccountDisplay* account,
    /* [in] */ Int32 syncMode)
{
    menu->SetHeaderTitle(Elastos::Droid::Dialer::R::string::dialog_sync_add);

    // Create item for each available, unsynced group
    AutoPtr<IIterator> it;
    account->mUnsyncedGroups->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        AutoPtr<IGroupDelta> child = IGroupDelta::Probe(next);
        Boolean sync;
        if (child->GetShouldSync(&sync), !sync) {
            AutoPtr<ICharSequence> title;
            child->GetTitle(this, (ICharSequence**)&title);
            AutoPtr<IMenuItem> item;
            IMenu::Probe(menu)->Add(title, (IMenuItem**)&item);
            AutoPtr<IOnMenuItemClickListener> listener
                    = (IOnMenuItemClickListener*)new AddSyncClickListener(account, child, syncMode, this);
            item->SetOnMenuItemClickListener(listener);
        }
    }
}

void CCustomContactListFilterActivity::DoSaveAction()
{
    if (mAdapter == NULL || mAdapter->mAccounts == NULL) {
        Finish();
        return;
    }

    SetResult(RESULT_OK);

    AutoPtr<IArrayList> diff = mAdapter->mAccounts->BuildDiff();
    Boolean isEmpty;
    if (diff->IsEmpty(&isEmpty), isEmpty) {
        Finish();
        return;
    }

    AutoPtr<UpdateTask> task = new UpdateTask(IActivity::Probe(this));
    Int32 size;
    diff->GetSize(&size);
    AutoPtr<ArrayOf<IInterface*> > attrs = ArrayOf<IInterface*>::Alloc(size);
    AutoPtr<IIterator> it;
    diff->GetIterator((IIterator**)&it);
    Int32 i = 0;
    Boolean hasNext;
    while(it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        attrs->Set(i, next);
        ++i;
    }
    task->Execute(attrs);
}

ECode CCustomContactListFilterActivity::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Int32 id;
    item->GetItemId(&id);
    switch (id) {
        case Elastos::Droid::R::id::home:
            // Pretend cancel.
            SetResult(IActivity::RESULT_CANCELED);
            Finish();
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
