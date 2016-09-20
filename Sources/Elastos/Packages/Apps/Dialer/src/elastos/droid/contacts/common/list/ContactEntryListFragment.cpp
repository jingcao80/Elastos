
#include "elastos/droid/contacts/common/list/ContactEntryListFragment.h"
#include "elastos/droid/contacts/common/list/DirectoryPartition.h"
#include "elastos/droid/contacts/common/list/DirectoryListLoader.h"
#include "elastos/droid/contacts/common/util/ContactListViewUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/widget/ListView.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>
#include "Elastos.Droid.Provider.h"

using Elastos::Droid::App::EIID_ILoaderManagerLoaderCallbacks;
using Elastos::Droid::Contacts::Common::List::EIID_IContactEntryListFragment;
using Elastos::Droid::Contacts::Common::Util::ContactListViewUtils;
using Elastos::Droid::Contacts::Common::Preference::EIID_IContactsPreferencesChangeListener;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::CCursorLoader;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Provider::IContactsContractDirectory;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::EIID_IViewOnFocusChangeListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::EIID_IAbsListViewOnScrollListener;
using Elastos::Droid::Widget::ListView;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

//=================================================================
// ContactEntryListFragment::SIMStateReceiver
//=================================================================

ContactEntryListFragment::SIMStateReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->mForceLoad = mHost->LoadPreferences();
    mHost->ReloadData();
    return NOERROR;
}


//=================================================================
// ContactEntryListFragment::DelayedDirectorySearchHandler
//=================================================================

ContactEntryListFragment::DelayedDirectorySearchHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    if (what == DIRECTORY_SEARCH_MESSAGE) {
        Int32 arg1;
        msg->GetArg1(&arg1);
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        AutoPtr<IDirectoryPartition> partition = IDirectoryPartition::Probe(obj);
        mHost->LoadDirectoryPartition(arg1, partition);
    }
    return NOERROR;
}


// =================================================================
// ContactEntryListFragment::PreferencesChangeListener
// =================================================================

CAR_INTERFACE_IMPL(ContactEntryListFragment::PreferencesChangeListener, Object, IContactsPreferencesChangeListener)

ECode ContactEntryListFragment::PreferencesChangeListener::OnChange()
{
    if(mHost->LoadPreferences()) {
        mHost->ReloadData();
    }
    return NOERROR;
}


//=================================================================
// ContactEntryListFragment
//=================================================================

CAR_INTERFACE_IMPL_4(ContactEntryListFragment::InnerListener, Object
    , IAdapterViewOnItemClickListener
    , IAbsListViewOnScrollListener
    , IViewOnFocusChangeListener
    , IViewOnTouchListener)

ContactEntryListFragment::InnerListener::InnerListener(
    /* [in] */ ContactEntryListFragment* host)
{}

ECode ContactEntryListFragment::InnerListener::OnScroll(
    /* [in] */ IAbsListView* view,
    /* [in] */ Int32 firstVisibleItem,
    /* [in] */ Int32 visibleItemCount,
    /* [in] */ Int32 totalItemCount)
{
    return mHost->OnScroll(view, firstVisibleItem, visibleItemCount, totalItemCount);
}

ECode ContactEntryListFragment::InnerListener::OnScrollStateChanged(
    /* [in] */ IAbsListView* view,
    /* [in] */ Int32 scrollState)
{
    return mHost->OnScrollStateChanged(view, scrollState);
}

ECode ContactEntryListFragment::InnerListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return mHost->OnItemClick(parent, view, position, id);
}

ECode ContactEntryListFragment::InnerListener::OnFocusChange(
    /* [in] */ IView* v,
    /* [in] */ Boolean hasFocus)
{
    return mHost->OnFocusChange(v, hasFocus);
}

ECode ContactEntryListFragment::InnerListener::OnTouch(
    /* [in] */ IView* view,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    return mHost->OnTouch(view, event, result);
}

//=================================================================
// ContactEntryListFragment
//=================================================================

const String ContactEntryListFragment::KEY_LIST_STATE("liststate");
const String ContactEntryListFragment::KEY_SECTION_HEADER_DISPLAY_ENABLED("sectionHeaderDisplayEnabled");
const String ContactEntryListFragment::KEY_PHOTO_LOADER_ENABLED("photoLoaderEnabled");
const String ContactEntryListFragment::KEY_QUICK_CONTACT_ENABLED("quickContactEnabled");
const String ContactEntryListFragment::KEY_ADJUST_SELECTION_BOUNDS_ENABLED("adjustSelectionBoundsEnabled");
const String ContactEntryListFragment::KEY_QUICK_CALL_BUTTON_ENABLED("quickCallButtonEnabled");
const String ContactEntryListFragment::KEY_INCLUDE_PROFILE("includeProfile");
const String ContactEntryListFragment::KEY_SEARCH_MODE("searchMode");
const String ContactEntryListFragment::KEY_VISIBLE_SCROLLBAR_ENABLED("visibleScrollbarEnabled");
const String ContactEntryListFragment::KEY_SCROLLBAR_POSITION("scrollbarPosition");
const String ContactEntryListFragment::KEY_QUERY_STRING("queryString");
const String ContactEntryListFragment::KEY_DIRECTORY_SEARCH_MODE("directorySearchMode");
const String ContactEntryListFragment::KEY_SELECTION_VISIBLE("selectionVisible");
const String ContactEntryListFragment::KEY_REQUEST("request");
const String ContactEntryListFragment::KEY_DARK_THEME("darkTheme");
const String ContactEntryListFragment::KEY_LEGACY_COMPATIBILITY("legacyCompatibility");
const String ContactEntryListFragment::KEY_DIRECTORY_RESULT_LIMIT("directoryResultLimit");
const String ContactEntryListFragment::DIRECTORY_ID_ARG_KEY("directoryId");
const Int32 ContactEntryListFragment::DIRECTORY_LOADER_ID;
const Int32 ContactEntryListFragment::DIRECTORY_SEARCH_DELAY_MILLIS;
const Int32 ContactEntryListFragment::DIRECTORY_SEARCH_MESSAGE;
const Int32 ContactEntryListFragment::DEFAULT_DIRECTORY_RESULT_LIMIT;
const Int32 ContactEntryListFragment::STATUS_NOT_LOADED;
const Int32 ContactEntryListFragment::STATUS_LOADING;
const Int32 ContactEntryListFragment::STATUS_LOADED;


CAR_INTERFACE_IMPL_2(ContactEntryListFragment, AnalyticsFragment,
    IContactEntryListFragment, ILoaderManagerLoaderCallbacks)

ContactEntryListFragment::ContactEntryListFragment()
    : mUserProfileExists(FALSE)
    , mSectionHeaderDisplayEnabled(FALSE)
    , mPhotoLoaderEnabled(FALSE)
    , mQuickContactEnabled(TRUE)
    , mAdjustSelectionBoundsEnabled(TRUE)
    , mQuickCallButtonEnabled(FALSE)
    , mIncludeProfile(FALSE)
    , mSearchMode(FALSE)
    , mVisibleScrollbarEnabled(FALSE)
    , mShowEmptyListForEmptyQuery(FALSE)
    , mDirectorySearchMode(DirectoryListLoader::SEARCH_MODE_NONE)
    , mSelectionVisible(FALSE)
    , mLegacyCompatibility(FALSE)
    , mEnabled(TRUE)
    , mDisplayOrder(0)
    , mSortOrder(0)
    , mDirectoryResultLimit(DEFAULT_DIRECTORY_RESULT_LIMIT)
    , mForceLoad(FALSE)
    , mDarkTheme(FALSE)
    , mDirectoryListStatus(STATUS_NOT_LOADED)
    , mLoadPriorityDirectoriesOnly(FALSE)
    , mDefaultVerticalScrollbarPosition(0)
{
    mVerticalScrollbarPosition = GetDefaultVerticalScrollbarPosition();
    mSIMStateReceiver = new SIMStateReceiver(this);
    mDelayedDirectorySearchHandler = new DelayedDirectorySearchHandler(this);
    // TODO: no PreferencesChangeListener
    // mPreferencesChangeListener  = new PreferencesChangeListener(this);
}

ECode ContactEntryListFragment::OnAttach(
    /* [in] */ IActivity* activity)
{
    AnalyticsFragment::OnAttach(activity);
    SetContext(IContext::Probe(activity));
    AutoPtr<ILoaderManager> manager;
    AnalyticsFragment::GetLoaderManager((ILoaderManager**)&manager);
    SetLoaderManager(manager);
    return NOERROR;
}

ECode ContactEntryListFragment::SetContext(
    /* [in] */ IContext* context)
{
    mContext = context;
    ConfigurePhotoLoader();
    return NOERROR;
}

ECode ContactEntryListFragment::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context)
    *context = mContext;
    REFCOUNT_ADD(*context)
    return NOERROR;
}

ECode ContactEntryListFragment::SetEnabled(
    /* [in] */ Boolean enabled)
{
    if (mEnabled != enabled) {
        mEnabled = enabled;
        if (mAdapter != NULL) {
            if (mEnabled) {
                ReloadData();
            }
            else {
                assert(0);
                // mAdapter->ClearPartitions();
            }
        }
    }
    return NOERROR;
}

ECode ContactEntryListFragment::SetLoaderManager(
    /* [in] */ ILoaderManager* loaderManager)
{
    mLoaderManager = loaderManager;
    return NOERROR;
}

ECode ContactEntryListFragment::GetAdapter(
    /* [out] */ IInterface** adapter)
{
    VALIDATE_NOT_NULL(adapter)
    *adapter = IInterface::Probe(mAdapter);
    REFCOUNT_ADD(*adapter)
    return NOERROR;
}

ECode ContactEntryListFragment::GetView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    *view = mView;
    REFCOUNT_ADD(*view)
    return NOERROR;
}

ECode ContactEntryListFragment::GetListView(
    /* [out] */ IListView** listView)
{
    VALIDATE_NOT_NULL(listView)
    *listView = mListView;
    REFCOUNT_ADD(*listView)
    return NOERROR;
}

ECode ContactEntryListFragment::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    AnalyticsFragment::OnSaveInstanceState(outState);
    outState->PutBoolean(KEY_SECTION_HEADER_DISPLAY_ENABLED, mSectionHeaderDisplayEnabled);
    outState->PutBoolean(KEY_PHOTO_LOADER_ENABLED, mPhotoLoaderEnabled);
    outState->PutBoolean(KEY_QUICK_CONTACT_ENABLED, mQuickContactEnabled);
    outState->PutBoolean(KEY_ADJUST_SELECTION_BOUNDS_ENABLED, mAdjustSelectionBoundsEnabled);
    outState->PutBoolean(KEY_QUICK_CALL_BUTTON_ENABLED, mQuickCallButtonEnabled);
    outState->PutBoolean(KEY_INCLUDE_PROFILE, mIncludeProfile);
    outState->PutBoolean(KEY_SEARCH_MODE, mSearchMode);
    outState->PutBoolean(KEY_VISIBLE_SCROLLBAR_ENABLED, mVisibleScrollbarEnabled);
    outState->PutInt32(KEY_SCROLLBAR_POSITION, mVerticalScrollbarPosition);
    outState->PutInt32(KEY_DIRECTORY_SEARCH_MODE, mDirectorySearchMode);
    outState->PutBoolean(KEY_SELECTION_VISIBLE, mSelectionVisible);
    outState->PutBoolean(KEY_LEGACY_COMPATIBILITY, mLegacyCompatibility);
    outState->PutString(KEY_QUERY_STRING, mQueryString);
    outState->PutInt32(KEY_DIRECTORY_RESULT_LIMIT, mDirectoryResultLimit);
    outState->PutBoolean(KEY_DARK_THEME, mDarkTheme);

    if (mListView != NULL) {
        outState->PutParcelable(KEY_LIST_STATE, ((ListView*)mListView.Get())->OnSaveInstanceState());
    }
    return NOERROR;
}

ECode ContactEntryListFragment::OnCreate(
    /* [in] */ IBundle* savedState)
{
    AnalyticsFragment::OnCreate(savedState);
    RestoreSavedState(savedState);
    mAdapter = CreateListAdapter();
    // TODO: ContactsPreferences not implemented
    // mContactsPrefs = new ContactsPreferences(mContext);
    RestoreSavedState(savedState);

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_AIRPLANE_MODE_CHANGED);
    filter->AddAction(ITelephonyIntents::ACTION_SIM_STATE_CHANGED);
    if (mContext != NULL) {
        AutoPtr<IIntent> intent;
        mContext->RegisterReceiver(mSIMStateReceiver, filter, (IIntent**)&intent);
    }
    return NOERROR;
}

ECode ContactEntryListFragment::RestoreSavedState(
    /* [in] */ IBundle* savedState)
{
    if (savedState == NULL) {
        return NOERROR;
    }

    savedState->GetBoolean(KEY_SECTION_HEADER_DISPLAY_ENABLED, &mSectionHeaderDisplayEnabled);
    savedState->GetBoolean(KEY_PHOTO_LOADER_ENABLED, &mPhotoLoaderEnabled);
    savedState->GetBoolean(KEY_QUICK_CONTACT_ENABLED, &mQuickContactEnabled);
    savedState->GetBoolean(KEY_ADJUST_SELECTION_BOUNDS_ENABLED, &mAdjustSelectionBoundsEnabled);
    savedState->GetBoolean(KEY_QUICK_CALL_BUTTON_ENABLED, &mQuickCallButtonEnabled);
    savedState->GetBoolean(KEY_INCLUDE_PROFILE, &mIncludeProfile);
    savedState->GetBoolean(KEY_SEARCH_MODE, &mSearchMode);
    savedState->GetBoolean(KEY_VISIBLE_SCROLLBAR_ENABLED, &mVisibleScrollbarEnabled);
    savedState->GetInt32(KEY_SCROLLBAR_POSITION, &mVerticalScrollbarPosition);
    savedState->GetInt32(KEY_DIRECTORY_SEARCH_MODE, &mDirectorySearchMode);
    savedState->GetBoolean(KEY_SELECTION_VISIBLE, &mSelectionVisible);
    savedState->GetBoolean(KEY_LEGACY_COMPATIBILITY, &mLegacyCompatibility);
    savedState->GetString(KEY_QUERY_STRING, &mQueryString);
    savedState->GetInt32(KEY_DIRECTORY_RESULT_LIMIT, &mDirectoryResultLimit);
    savedState->GetBoolean(KEY_DARK_THEME, &mDarkTheme);

    // Retrieve list state. This will be applied in onLoadFinished
    mListState = NULL;
    savedState->GetParcelable(KEY_LIST_STATE, (IParcelable**)&mListState);
    return NOERROR;
}

ECode ContactEntryListFragment::OnStart()
{
    AnalyticsFragment::OnStart();

    // TODO: no ContactsPreferences
    // mContactsPrefs->RegisterChangeListener(mPreferencesChangeListener);

    mForceLoad = LoadPreferences();

    mDirectoryListStatus = STATUS_NOT_LOADED;
    mLoadPriorityDirectoriesOnly = TRUE;

    StartLoading();
    return NOERROR;
}

void ContactEntryListFragment::StartLoading()
{
    if (mAdapter == NULL) {
        // The method was called before the fragment was started
        return;
    }

    ConfigureAdapter();
    assert(0);
    Int32 partitionCount;
    // mAdapter->GetPartitionCount(&partitionCount);
    for (Int32 i = 0; i < partitionCount; i++) {
        // TODO: no com.android.common.widget.CompositeCursorAdapter;
        // Partition partition = mAdapter.getPartition(i);
        // if (partition instanceof DirectoryPartition) {
        //     DirectoryPartition directoryPartition = (DirectoryPartition)partition;
        //     if (directoryPartition.getStatus() == DirectoryPartition.STATUS_NOT_LOADED) {
        //         if (directoryPartition.isPriorityDirectory() || !mLoadPriorityDirectoriesOnly) {
        //             startLoadingDirectoryPartition(i);
        //         }
        //     }
        // } else {
        AutoPtr<ILoaderManager> lm;
        GetLoaderManager((ILoaderManager**)&lm);
        AutoPtr<ILoader> l;
        lm->InitLoader(i, NULL, ILoaderManagerLoaderCallbacks::Probe(this), (ILoader**)&l);
        // }
    }

    // Next time this method is called, we should start loading non-priority directories
    mLoadPriorityDirectoriesOnly = FALSE;
    // TODO: no com.android.common.widget.CompositeCursorAdapter;
    // mAdapter->NotifyDataSetChanged();
}

ECode ContactEntryListFragment::OnCreateLoader(
    /* [in] */ Int32 id,
    /* [in] */ IBundle* args,
    /* [out] */ ILoader** _loader)
{
    VALIDATE_NOT_NULL(_loader)
    if (id == DIRECTORY_LOADER_ID) {
        AutoPtr<DirectoryListLoader> loader = new DirectoryListLoader(mContext);
        Int32 mode;
        mAdapter->GetDirectorySearchMode(&mode);
        loader->SetDirectorySearchMode(mode);
        loader->SetLocalInvisibleDirectoryEnabled(
                IContactEntryListAdapter::LOCAL_INVISIBLE_DIRECTORY_ENABLED);
        *_loader = ILoader::Probe(loader);
        REFCOUNT_ADD(*_loader)
    }
    else {
        AutoPtr<ICursorLoader> loader;
        CreateCursorLoader(mContext, (ICursorLoader**)&loader);
        Int64 directoryId = IContactsContractDirectory::DEFAULT;
        Boolean contains;
        if (args != NULL && (args->ContainsKey(DIRECTORY_ID_ARG_KEY, &contains), contains)) {
            args->GetInt64(DIRECTORY_ID_ARG_KEY, IContactsContractDirectory::DEFAULT, &directoryId);
        }
        mAdapter->ConfigureLoader(loader, directoryId);
        *_loader = ILoader::Probe(loader);
        REFCOUNT_ADD(*_loader)
    }
    return NOERROR;
}

ECode ContactEntryListFragment::CreateCursorLoader(
    /* [in] */ IContext* context,
    /* [out] */ ICursorLoader** loader)
{
    VALIDATE_NOT_NULL(loader)
    return CCursorLoader::New(context, NULL, NULL, String(NULL), NULL, String(NULL), loader);
}

void ContactEntryListFragment::StartLoadingDirectoryPartition(
    /* [in] */ Int32 partitionIndex)
{
    // TODO: no ICompositeCursorAdapter;
    assert(0);
    // AutoPtr<IPartition> temp;
    // ICompositeCursorAdapter::Probe(mAdapter)->GetPartition(partitionIndex, (IPartition**)&temp);
    // AutoPtr<IDirectoryPartition> partition = IDirectoryPartition::Probe(temp);
    // partition->SetStatus(IDirectoryPartition::STATUS_LOADING);
    // Int64 directoryId;
    // partition->GetDirectoryId(&directoryId);
    // if (mForceLoad) {
    //     if (directoryId == IContactsContractDirectory::DEFAULT) {
    //         LoadDirectoryPartition(partitionIndex, partition);
    //     }
    //     else {
    //         LoadDirectoryPartitionDelayed(partitionIndex, partition);
    //     }
    // }
    // else {
    //     AutoPtr<IBundle> args;
    //     CBundle::New((IBundle**)&args);
    //     args->PutInt64(DIRECTORY_ID_ARG_KEY, directoryId);
    //     AutoPtr<ILoaderManager> lm;
    //     GetLoaderManager((ILoaderManager**)&lm);
    //     AutoPtr<ILoader> l;
    //     lm->InitLoader(partitionIndex, args, ILoaderManagerLoaderCallbacks::Probe(this), (ILoader**)&l);
    // }
}

void ContactEntryListFragment::LoadDirectoryPartitionDelayed(
    /* [in] */ Int32 partitionIndex,
    /* [in] */ IDirectoryPartition* partition)
{
    mDelayedDirectorySearchHandler->RemoveMessages(DIRECTORY_SEARCH_MESSAGE, IInterface::Probe(partition));
    AutoPtr<IMessage> msg;
    mDelayedDirectorySearchHandler->ObtainMessage(
            DIRECTORY_SEARCH_MESSAGE, partitionIndex, 0, IInterface::Probe(partition), (IMessage**)&msg);
    Boolean result;
    mDelayedDirectorySearchHandler->SendMessageDelayed(msg, DIRECTORY_SEARCH_DELAY_MILLIS, &result);
}

void ContactEntryListFragment::LoadDirectoryPartition(
    /* [in] */ Int32 partitionIndex,
    /* [in] */ IDirectoryPartition* partition)
{
    AutoPtr<IBundle> args;
    CBundle::New((IBundle**)&args);
    Int64 directoryId = ((DirectoryPartition*)partition)->GetDirectoryId();
    args->PutInt64(DIRECTORY_ID_ARG_KEY, directoryId);
    AutoPtr<ILoaderManager> lm;
    GetLoaderManager((ILoaderManager**)&lm);
    AutoPtr<ILoader> l;
    if (lm->GetLoader(partitionIndex, (ILoader**)&l), l != NULL) {
        lm->DestroyLoader(partitionIndex);
    }
    AutoPtr<ILoader> loader;
    lm->RestartLoader(partitionIndex, args, ILoaderManagerLoaderCallbacks::Probe(this), (ILoader**)&loader);
}

void ContactEntryListFragment::RemovePendingDirectorySearchRequests()
{
    mDelayedDirectorySearchHandler->RemoveMessages(DIRECTORY_SEARCH_MESSAGE);
}

ECode ContactEntryListFragment::OnLoadFinished(
    /* [in] */ ILoader* loader,
    /* [in] */ IInterface* data)
{
    if (!mEnabled) {
        return NOERROR;
    }

    Int32 loaderId;
    loader->GetId(&loaderId);
    if (loaderId == DIRECTORY_LOADER_ID) {
        mDirectoryListStatus = STATUS_LOADED;
        mAdapter->ChangeDirectories(ICursor::Probe(data));
        StartLoading();
    }
    else {
        OnPartitionLoaded(loaderId, ICursor::Probe(data));
        Boolean isSearchMode;
        if (IsSearchMode(&isSearchMode), isSearchMode) {
            Int32 directorySearchMode;
            GetDirectorySearchMode(&directorySearchMode);
            if (directorySearchMode != DirectoryListLoader::SEARCH_MODE_NONE) {
                if (mDirectoryListStatus == STATUS_NOT_LOADED) {
                    mDirectoryListStatus = STATUS_LOADING;
                    AutoPtr<ILoaderManager> lm;
                    GetLoaderManager((ILoaderManager**)&lm);
                    AutoPtr<ILoader> l;
                    lm->InitLoader(DIRECTORY_LOADER_ID, NULL,
                            ILoaderManagerLoaderCallbacks::Probe(this), (ILoader**)&l);
                }
                else {
                    StartLoading();
                }
            }
        }
        else {
            mDirectoryListStatus = STATUS_NOT_LOADED;
            AutoPtr<ILoaderManager> lm;
            GetLoaderManager((ILoaderManager**)&lm);
            lm->DestroyLoader(DIRECTORY_LOADER_ID);
        }
    }
    return NOERROR;
}

ECode ContactEntryListFragment::OnLoaderReset(
    /* [in] */ ILoader* loader)
{
    Int32 id;
    if (loader->GetId(&id), id >= 0) {
        // TODO: no CompositeCursorAdapter and then no function ChangeCursor
        assert(0);
        // mAdapter->ChangeCursor(id, NULL);
    }
    else {
        mAdapter->ChangeCursor(NULL);
    }
    return NOERROR;
}

void ContactEntryListFragment::OnPartitionLoaded(
    /* [in] */ Int32 partitionIndex,
    /* [in] */ ICursor* data)
{
    // Int32 count;
    assert(0);
    // TODO: no CompositeCursorAdapter and then no function GetPartitionCount
    // if (mAdapter->GetPartitionCount(&count), partitionIndex >= count) {
    //     // When we get unsolicited data, ignore it.  This could happen
    //     // when we are switching from search mode to the default mode.
    //     return;
    // }

    // TODO: no CompositeCursorAdapter and then no function ChangeCursor
    // mAdapter->ChangeCursor(partitionIndex, data);
    SetProfileHeader();

    Boolean isLoading;
    if (IsLoading(&isLoading), !isLoading) {
        CompleteRestoreInstanceState();
    }
}

ECode ContactEntryListFragment::IsLoading(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean isLoading;
    if (mAdapter != NULL && (mAdapter->IsLoading(&isLoading), isLoading)) {
        *result = TRUE;
        return NOERROR;
    }

    Boolean isLoadingList;
    if (IsLoadingDirectoryList(&isLoadingList) ,isLoadingList) {
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

ECode ContactEntryListFragment::IsLoadingDirectoryList(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean isSearchMode;
    Int32 mode;
    *result = (IsSearchMode(&isSearchMode), isSearchMode)
            && (GetDirectorySearchMode(&mode), mode != DirectoryListLoader::SEARCH_MODE_NONE)
            && (mDirectoryListStatus == STATUS_NOT_LOADED || mDirectoryListStatus == STATUS_LOADING);
    return NOERROR;
}

ECode ContactEntryListFragment::OnStop()
{
    AnalyticsFragment::OnStop();
    // TODO: no ContactsPreferences
    // mContactsPrefs->UnregisterChangeListener();
    // mAdapter->ClearPartitions();
    return NOERROR;
}

ECode ContactEntryListFragment::OnDestroy()
{
    AnalyticsFragment::OnDestroy();
    if (mContext != NULL) {
        mContext->UnregisterReceiver(mSIMStateReceiver);
    }
    return NOERROR;
}

void ContactEntryListFragment::ReloadData()
{
    RemovePendingDirectorySearchRequests();
    mAdapter->OnDataReload();
    mLoadPriorityDirectoriesOnly = TRUE;
    mForceLoad = TRUE;
    StartLoading();
}

void ContactEntryListFragment::SetProfileHeader()
{
    mUserProfileExists = FALSE;
}

void ContactEntryListFragment::Finish()
{}

ECode ContactEntryListFragment::SetSectionHeaderDisplayEnabled(
    /* [in] */ Boolean flag)
{
    if (mSectionHeaderDisplayEnabled != flag) {
        mSectionHeaderDisplayEnabled = flag;
        if (mAdapter != NULL) {
            assert(0);
            // mAdapter->SetSectionHeaderDisplayEnabled(flag);
        }
        ConfigureVerticalScrollbar();
    }
    return NOERROR;
}

ECode ContactEntryListFragment::IsSectionHeaderDisplayEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSectionHeaderDisplayEnabled;
    return NOERROR;
}

ECode ContactEntryListFragment::SetVisibleScrollbarEnabled(
    /* [in] */ Boolean flag)
{
    if (mVisibleScrollbarEnabled != flag) {
        mVisibleScrollbarEnabled = flag;
        ConfigureVerticalScrollbar();
    }
    return NOERROR;
}

ECode ContactEntryListFragment::IsVisibleScrollbarEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mVisibleScrollbarEnabled;
    return NOERROR;
}

ECode ContactEntryListFragment::SetVerticalScrollbarPosition(
    /* [in] */ Int32 position)
{
    if (mVerticalScrollbarPosition != position) {
        mVerticalScrollbarPosition = position;
        ConfigureVerticalScrollbar();
    }
    return NOERROR;
}

void ContactEntryListFragment::ConfigureVerticalScrollbar()
{
    Boolean isVisibleEnabled, isSectionEnabled;
    Boolean hasScrollbar = (IsVisibleScrollbarEnabled(&isVisibleEnabled), isVisibleEnabled)
            && (IsSectionHeaderDisplayEnabled(&isSectionEnabled), isSectionEnabled);

    if (mListView != NULL) {
        AutoPtr<IAbsListView> absListView  = IAbsListView::Probe(mListView);
        absListView->SetFastScrollEnabled(hasScrollbar);
        absListView->SetFastScrollAlwaysVisible(hasScrollbar);
        AutoPtr<IView> view = IView::Probe(mListView);
        view->SetVerticalScrollbarPosition(mVerticalScrollbarPosition);
        view->SetScrollBarStyle(IView::SCROLLBARS_OUTSIDE_OVERLAY);
    }
}

ECode ContactEntryListFragment::SetPhotoLoaderEnabled(
    /* [in] */ Boolean flag)
{
    mPhotoLoaderEnabled = flag;
    ConfigurePhotoLoader();
    return NOERROR;
}

ECode ContactEntryListFragment::IsPhotoLoaderEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPhotoLoaderEnabled;
    return NOERROR;
}

ECode ContactEntryListFragment::IsSelectionVisible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSelectionVisible;
    return NOERROR;
}

ECode ContactEntryListFragment::SetSelectionVisible(
    /* [in] */ Boolean flag)
{
    mSelectionVisible = flag;
    return NOERROR;
}

ECode ContactEntryListFragment::SetQuickContactEnabled(
    /* [in] */ Boolean flag)
{
    mQuickContactEnabled = flag;
    return NOERROR;
}

ECode ContactEntryListFragment::SetAdjustSelectionBoundsEnabled(
    /* [in] */ Boolean flag)
{
    mAdjustSelectionBoundsEnabled = flag;
    return NOERROR;
}

ECode ContactEntryListFragment::SetQuickCallButtonEnabled(
    /* [in] */ Boolean flag)
{
    mQuickCallButtonEnabled = flag;
    return NOERROR;
}

ECode ContactEntryListFragment::SetIncludeProfile(
    /* [in] */ Boolean flag)
{
    mIncludeProfile = flag;
    if(mAdapter != NULL) {
        mAdapter->SetIncludeProfile(flag);
    }
    return NOERROR;
}

void ContactEntryListFragment::SetSearchMode(
    /* [in] */ Boolean flag)
{
    if (mSearchMode != flag) {
        mSearchMode = flag;
        SetSectionHeaderDisplayEnabled(!mSearchMode);

        if (!flag) {
            mDirectoryListStatus = STATUS_NOT_LOADED;
            AutoPtr<ILoaderManager> lm;
            GetLoaderManager((ILoaderManager**)&lm);
            lm->DestroyLoader(DIRECTORY_LOADER_ID);
        }

        if (mAdapter != NULL) {
            mAdapter->SetSearchMode(flag);

            // TODO: ContactEntryListAdapter
            // mAdapter->ClearPartitions();
            if (!flag) {
                // If we are switching from search to regular display, remove all directory
                // partitions after default one, assuming they are remote directories which
                // should be cleaned up on exiting the search mode.
                mAdapter->RemoveDirectoriesAfterDefault();
            }
            mAdapter->ConfigureDefaultPartition(FALSE, flag);
        }

        if (mListView != NULL) {
            IAbsListView::Probe(mListView)->SetFastScrollEnabled(!flag);
        }
    }
}

ECode ContactEntryListFragment::IsSearchMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSearchMode;
    return NOERROR;
}

ECode ContactEntryListFragment::GetQueryString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mQueryString;
    return NOERROR;
}

ECode ContactEntryListFragment::SetQueryString(
    /* [in] */ const String& queryString,
    /* [in] */ Boolean delaySelection)
{
    if (!TextUtils::Equals(mQueryString, queryString)) {
        if (mShowEmptyListForEmptyQuery && mAdapter != NULL && mListView != NULL) {
            if (TextUtils::IsEmpty(mQueryString)) {
                // Restore the adapter if the query used to be empty.
                IAdapterView::Probe(mListView)->SetAdapter(IAdapter::Probe(mAdapter));
            }
            else if (TextUtils::IsEmpty(queryString)) {
                // Instantly clear the list view if the new query is empty.
                IAdapterView::Probe(mListView)->SetAdapter(NULL);
            }
        }

        mQueryString = queryString;
        SetSearchMode(!TextUtils::IsEmpty(mQueryString) || mShowEmptyListForEmptyQuery);

        if (mAdapter != NULL) {
            mAdapter->SetQueryString(queryString);
            ReloadData();
        }
    }
    return NOERROR;
}

ECode ContactEntryListFragment::SetShowEmptyListForNullQuery(
    /* [in] */ Boolean show)
{
    mShowEmptyListForEmptyQuery = show;
    return NOERROR;
}

ECode ContactEntryListFragment::GetDirectoryLoaderId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = DIRECTORY_LOADER_ID;
    return NOERROR;
}

ECode ContactEntryListFragment::GetDirectorySearchMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode)
    *mode = mDirectorySearchMode;
    return NOERROR;
}

ECode ContactEntryListFragment::SetDirectorySearchMode(
    /* [in] */ Int32 mode)
{
    mDirectorySearchMode = mode;
    return NOERROR;
}

ECode ContactEntryListFragment::IsLegacyCompatibilityMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mLegacyCompatibility;
    return NOERROR;
}

ECode ContactEntryListFragment::SetLegacyCompatibilityMode(
    /* [in] */ Boolean flag)
{
    mLegacyCompatibility = flag;
    return NOERROR;
}

Int32 ContactEntryListFragment::GetContactNameDisplayOrder()
{
    return mDisplayOrder;
}

void ContactEntryListFragment::SetContactNameDisplayOrder(
    /* [in] */ Int32 displayOrder)
{
    mDisplayOrder = displayOrder;
    if (mAdapter != NULL) {
        mAdapter->SetContactNameDisplayOrder(displayOrder);
    }
}

ECode ContactEntryListFragment::GetSortOrder(
    /* [out] */ Int32* sortOrder)
{
    VALIDATE_NOT_NULL(sortOrder)
    *sortOrder = mSortOrder;
    return NOERROR;
}

ECode ContactEntryListFragment::SetSortOrder(
    /* [in] */ Int32 sortOrder)
{
    mSortOrder = sortOrder;
    if (mAdapter != NULL) {
        mAdapter->SetSortOrder(sortOrder);
    }
    return NOERROR;
}

ECode ContactEntryListFragment::SetDirectoryResultLimit(
    /* [in] */ Int32 limit)
{
    mDirectoryResultLimit = limit;
    return NOERROR;
}

Boolean ContactEntryListFragment::LoadPreferences()
{
    Boolean changed = FALSE;
    // TODO: no ContactsPreferences
    // if (GetContactNameDisplayOrder() != mContactsPrefs->GetDisplayOrder()) {
    //     SetContactNameDisplayOrder(mContactsPrefs->GetDisplayOrder());
    //     changed = TRUE;
    // }

    // Int32 sort;
    // if (GetSortOrder(&sort), sort != mContactsPrefs->GetSortOrder()) {
    //     SetSortOrder(mContactsPrefs->GetSortOrder());
    //     changed = TRUE;
    // }

    return changed;
}

ECode ContactEntryListFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    FAIL_RETURN(OnCreateView(inflater, container))

    Boolean searchMode;
    IsSearchMode(&searchMode);
    mAdapter->SetSearchMode(searchMode);
    mAdapter->ConfigureDefaultPartition(FALSE, searchMode);
    // TODO: no contactphotomanager
    // mAdapter->SetPhotoLoader(mPhotoManager);
    IAdapterView::Probe(mListView)->SetAdapter(IAdapter::Probe(mAdapter));

    if (IsSearchMode(&searchMode), !searchMode) {
        AutoPtr<IView> v = IView::Probe(mListView);
        v->SetFocusableInTouchMode(TRUE);
        Boolean result;
        v->RequestFocus(&result);
    }

    *view = mView;
    REFCOUNT_ADD(*view)
    return NOERROR;
}

ECode ContactEntryListFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container)
{
    mView = InflateView(inflater, container);

    AutoPtr<IView> temp;
    mView->FindViewById(Elastos::Droid::R::id::list, (IView**)&temp);
    mListView = IListView::Probe(temp);
    if (mListView == NULL) {
        Logger::E("ContactEntryListFragment", "Your content must have a ListView whose id attribute is 'android.R.id.list'");
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<IView> emptyView;
    mView->FindViewById(Elastos::Droid::R::id::empty, (IView**)&emptyView);
    AutoPtr<IAdapterView> adapterV = IAdapterView::Probe(mListView);
    if (emptyView != NULL) {
        adapterV->SetEmptyView(emptyView);
    }

    AutoPtr<InnerListener> listener = new InnerListener(this);
    adapterV->SetOnItemClickListener(listener);
    AutoPtr<IView> v = IView::Probe(mListView);
    v->SetOnFocusChangeListener(listener);
    v->SetOnTouchListener(listener);
    Boolean isSearchMode;
    IsSearchMode(&isSearchMode);
    IAbsListView::Probe(mListView)->SetFastScrollEnabled(!isSearchMode);

    // Tell list view to not show dividers. We'll do it ourself so that we can *not* show
    // them when an A-Z headers is visible.
    mListView->SetDividerHeight(0);

    // We manually save/restore the listview state
    v->SetSaveEnabled(FALSE);

    ConfigureVerticalScrollbar();
    ConfigurePhotoLoader();

    AutoPtr<IInterface> adapter;
    GetAdapter((IInterface**)&adapter);
    AutoPtr<IView> view;
    GetView((IView**)&view);
    IContactEntryListAdapter::Probe(adapter)->SetFragmentRootView(view);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    return ContactListViewUtils::ApplyCardPaddingToView(res, mListView, mView);
}

ECode ContactEntryListFragment::OnHiddenChanged(
    /* [in] */ Boolean hidden)
{
    AnalyticsFragment::OnHiddenChanged(hidden);
    AutoPtr<IActivity> a;
    AutoPtr<IView> v;
    if ((GetActivity((IActivity**)&a), a != NULL) && (GetView((IView**)&v), v != NULL) && !hidden) {
        // If the padding was last applied when in a hidden state, it may have been applied
        // incorrectly. Therefore we need to reapply it.
        AutoPtr<IResources> res;
        GetResources((IResources**)&res);
        return ContactListViewUtils::ApplyCardPaddingToView(res, mListView, v);
    }
    return NOERROR;
}

void ContactEntryListFragment::ConfigurePhotoLoader()
{
    Boolean isEnabled;
    if ((IsPhotoLoaderEnabled(&isEnabled), isEnabled) && mContext != NULL) {
        // TODO: no ContactPhotoManager
        // if (mPhotoManager == NULL) {
        //     mPhotoManager = ContactPhotoManager::GetInstance(mContext);
        // }
        if (mListView != NULL) {
            AutoPtr<InnerListener> listener = new InnerListener(this);
            IAbsListView::Probe(mListView)->SetOnScrollListener(listener);
        }
        if (mAdapter != NULL) {
            // TODO: no ContactPhotoManager
            // mAdapter->SetPhotoLoader(mPhotoManager);
        }
    }
}

void ContactEntryListFragment::ConfigureAdapter()
{
    if (mAdapter == NULL) {
        return;
    }

    mAdapter->SetQuickContactEnabled(mQuickContactEnabled);
    mAdapter->SetAdjustSelectionBoundsEnabled(mAdjustSelectionBoundsEnabled);
    mAdapter->SetIncludeProfile(mIncludeProfile);
    mAdapter->SetQueryString(mQueryString);
    mAdapter->SetDirectorySearchMode(mDirectorySearchMode);
    IPinnedHeaderListAdapter::Probe(mAdapter)->SetPinnedPartitionHeadersEnabled(FALSE);
    mAdapter->SetContactNameDisplayOrder(mDisplayOrder);
    mAdapter->SetSortOrder(mSortOrder);
    IIndexerListAdapter::Probe(mAdapter)->SetSectionHeaderDisplayEnabled(mSectionHeaderDisplayEnabled);
    mAdapter->SetSelectionVisible(mSelectionVisible);
    mAdapter->SetDirectoryResultLimit(mDirectoryResultLimit);
    mAdapter->SetDarkTheme(mDarkTheme);
}

ECode ContactEntryListFragment::OnScroll(
    /* [in] */ IAbsListView* view,
    /* [in] */ Int32 firstVisibleItem,
    /* [in] */ Int32 visibleItemCount,
    /* [in] */ Int32 totalItemCount)
{
    return NOERROR;
}

ECode ContactEntryListFragment::OnScrollStateChanged(
    /* [in] */ IAbsListView* view,
    /* [in] */ Int32 scrollState)
{
    Boolean isEnabled;
    if (scrollState == IAbsListViewOnScrollListener::SCROLL_STATE_FLING) {
        assert(0);
        // mPhotoManager->Pause();
    }
    else if (IsPhotoLoaderEnabled(&isEnabled), isEnabled) {
        assert(0);
        // mPhotoManager->Resume();
    }
    return NOERROR;
}

ECode ContactEntryListFragment::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    HideSoftKeyboard();

    Int32 count = ((ListView*)mListView.Get())->GetHeaderViewsCount();
    Int32 adjPosition = position - count;
    if (adjPosition >= 0) {
        OnItemClick(adjPosition, id);
    }
    return NOERROR;
}

void ContactEntryListFragment::HideSoftKeyboard()
{
    // Hide soft keyboard, if visible
    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&service);
    AutoPtr<IInputMethodManager> inputMethodManager = IInputMethodManager::Probe(service);
    AutoPtr<IBinder> token;
    IView::Probe(mListView)->GetWindowToken((IBinder**)&token);
    Boolean result;
    inputMethodManager->HideSoftInputFromWindow(token, 0, &result);
}

ECode ContactEntryListFragment::OnFocusChange(
    /* [in] */ IView* view,
    /* [in] */ Boolean hasFocus)
{
    if (view == IView::Probe(mListView) && hasFocus) {
        HideSoftKeyboard();
    }
    return NOERROR;
}

ECode ContactEntryListFragment::OnTouch(
    /* [in] */ IView* view,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (view == IView::Probe(mListView)) {
        HideSoftKeyboard();
    }
    *result = FALSE;
    return NOERROR;
}

ECode ContactEntryListFragment::OnPause()
{
    AnalyticsFragment::OnPause();
    RemovePendingDirectorySearchRequests();
    return NOERROR;
}

void ContactEntryListFragment::CompleteRestoreInstanceState()
{
    if (mListState != NULL) {
        ((ListView*)mListView.Get())->OnRestoreInstanceState(mListState);
        mListState = NULL;
    }
}

ECode ContactEntryListFragment::SetDarkTheme(
    /* [in] */ Boolean value)
{
    mDarkTheme = value;
    if (mAdapter != NULL) mAdapter->SetDarkTheme(value);
    return NOERROR;
}

ECode ContactEntryListFragment::OnPickerResult(
    /* [in] */ IIntent* data)
{
    Logger::E("ContactEntryListFragment", "Picker result handler is not implemented.");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

Int32 ContactEntryListFragment::GetDefaultVerticalScrollbarPosition()
{
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    Int32 layoutDirection = TextUtils::GetLayoutDirectionFromLocale(locale);
    switch (layoutDirection) {
        case IView::LAYOUT_DIRECTION_RTL:
            return IView::SCROLLBAR_POSITION_LEFT;
        case IView::LAYOUT_DIRECTION_LTR:
        default:
            return IView::SCROLLBAR_POSITION_RIGHT;
    }
}

} // List
} // Common
} // Contacts
} // Droid
} // Elastos
