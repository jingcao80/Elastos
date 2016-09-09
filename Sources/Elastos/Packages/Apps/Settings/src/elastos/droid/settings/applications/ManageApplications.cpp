
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/settings/applications/ManageApplications.h"
#include "elastos/droid/settings/applications/AppViewHolder.h"
#include "elastos/droid/settings/deviceinfo/StorageMeasurement.h"
#include "elastos/droid/settings/CSettingsActivity.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/view/animation/AnimationUtils.h"
#include "elastos/droid/R.h"
#include "../R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Settings::Deviceinfo::StorageMeasurement;
using Elastos::Droid::Support::V4::View::EIID_IViewPagerOnPageChangeListener;
using Elastos::Droid::Support::V4::View::IPagerTabStrip;
using Elastos::Droid::Support::V4::View::IPagerAdapter;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreferenceFrameLayout;
using Elastos::Droid::Preference::IPreferenceFrameLayoutParams;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Net::CNetworkPolicyManagerHelper;
using Elastos::Droid::Net::INetworkPolicyManagerHelper;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::View::Animation::IAnimation;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::Widget::EIID_IFilterable;
using Elastos::Droid::Widget::EIID_IRecyclerListener;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::ISpinner;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

const String ManageApplications::APP_CHG("chg");

const Int32 ManageApplications::SIZE_TOTAL = 0;
const Int32 ManageApplications::SIZE_INTERNAL;
const Int32 ManageApplications::SIZE_EXTERNAL;

const Int32 ManageApplications::FILTER_APPS_ALL = MENU_OPTIONS_BASE + 0;
const Int32 ManageApplications::FILTER_APPS_THIRD_PARTY;
const Int32 ManageApplications::FILTER_APPS_SDCARD;
const Int32 ManageApplications::FILTER_APPS_DISABLED;

const Int32 ManageApplications::SORT_ORDER_ALPHA = MENU_OPTIONS_BASE + 4;
const Int32 ManageApplications::SORT_ORDER_SIZE;
const Int32 ManageApplications::SHOW_RUNNING_SERVICES = MENU_OPTIONS_BASE + 6;
const Int32 ManageApplications::SHOW_BACKGROUND_PROCESSES = MENU_OPTIONS_BASE + 7;
const Int32 ManageApplications::RESET_APP_PREFERENCES = MENU_OPTIONS_BASE + 8;

const String ManageApplications::TAG("ManageApplications");
const Boolean ManageApplications::DEBUG = FALSE;

const Int32 ManageApplications::LIST_TYPE_DOWNLOADED = 0;
const Int32 ManageApplications::LIST_TYPE_RUNNING = 1;
const Int32 ManageApplications::LIST_TYPE_SDCARD = 2;
const Int32 ManageApplications::LIST_TYPE_ALL = 3;
const Int32 ManageApplications::LIST_TYPE_DISABLED = 4;

const String ManageApplications::EXTRA_LIST_TYPE("currentListType");
const String ManageApplications::EXTRA_SORT_ORDER("sortOrder");
const String ManageApplications::EXTRA_SHOW_BACKGROUND("showBackground");
const String ManageApplications::EXTRA_DEFAULT_LIST_TYPE("defaultListType");
const String ManageApplications::EXTRA_RESET_DIALOG("resetDialog");

const Int32 ManageApplications::INSTALLED_APP_DETAILS = 1;

const Int32 ManageApplications::MENU_OPTIONS_BASE;

const Int32 ManageApplications::LIST_TYPE_MISSING = -1;

//===============================================================================
//                  ManageApplications::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL(ManageApplications::InnerListener, Object, IDialogInterfaceOnDismissListener)

ManageApplications::InnerListener::InnerListener(
    /* [in] */ ManageApplications* host)
    : mHost(host)
{}

ECode ManageApplications::InnerListener::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    return mHost->OnDismiss(dialog);
}

//===============================================================================
//                  ManageApplications::TabInfo
//===============================================================================

CAR_INTERFACE_IMPL_2(ManageApplications::TabInfo, Object, IManageApplicationsTabInfo, IAdapterViewOnItemClickListener)

ManageApplications::TabInfo::TabInfo(
    /* [in] */ ManageApplications* owner,
    /* [in] */ ApplicationsState* apps,
    /* [in] */ ICharSequence* label,
    /* [in] */ Int32 listType,
    /* [in] */ IManageApplicationsAppClickListener* clickListener,
    /* [in] */ IBundle* savedInstanceState)
    : mListType(0)
    , mFilter(0)
    , mFreeStorage(0)
    , mAppStorage(0)
    , mTotalStorage(0)
    , mLastUsedStorage(0)
    , mLastAppStorage(0)
    , mLastFreeStorage(0)
{
    mRunningProcessesAvail = new TabInfoRunnable(this);
    mOwner = owner;
    mApplicationsState = apps;
    mLabel = label;
    mListType = listType;
    switch (listType) {
        case LIST_TYPE_DOWNLOADED: mFilter = FILTER_APPS_THIRD_PARTY; break;
        case LIST_TYPE_SDCARD: mFilter = FILTER_APPS_SDCARD; break;
        case LIST_TYPE_DISABLED: mFilter = FILTER_APPS_DISABLED; break;
        default: mFilter = FILTER_APPS_ALL; break;
    }
    mClickListener = clickListener;

    AutoPtr<IActivity> activity;
    owner->GetActivity((IActivity**)&activity);
    IContext* _activity = IContext::Probe(activity);
    _activity->GetText(R::string::invalid_size_value, (ICharSequence**)&mInvalidSizeStr);
    _activity->GetText(R::string::computing_size, (ICharSequence**)&mComputingSizeStr);
    mSavedInstanceState = savedInstanceState;
}

ECode ManageApplications::TabInfo::SetContainerService(
    /* [in] */ IIMediaContainerService* containerService)
{
    mContainerService = containerService;
    UpdateStorageUsage();
    return NOERROR;
}

AutoPtr<IView> ManageApplications::TabInfo::Build(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* contentParent,
    /* [in] */ IView* contentChild)
{
    if (mRootView != NULL) {
        return mRootView;
    }

    mInflater = inflater;
    mRootView = NULL;
    inflater->Inflate(mListType == LIST_TYPE_RUNNING
            ? R::layout::manage_applications_running
            : R::layout::manage_applications_apps, NULL, (IView**)&mRootView);
    AutoPtr<IView> tmp;
    mRootView->FindViewById(R::id::pinned_header, (IView**)&tmp);
    mPinnedHeader = IViewGroup::Probe(tmp);
    if (mOwner->mProfileSpinnerAdapter != NULL) {
        AutoPtr<IView> spinnerTmp;
        inflater->Inflate(R::layout::spinner_view, NULL, (IView**)&spinnerTmp);
        IAdapterView* spinner = IAdapterView::Probe(spinnerTmp);
        spinner->SetAdapter(mOwner->mProfileSpinnerAdapter);
        spinner->SetOnItemSelectedListener(mOwner);
        mPinnedHeader->AddView(spinnerTmp);
        IView::Probe(mPinnedHeader)->SetVisibility(IView::VISIBLE);
    }
    mRootView->FindViewById(R::id::loading_container, (IView**)&mLoadingContainer);
    mLoadingContainer->SetVisibility(IView::VISIBLE);
    mRootView->FindViewById(R::id::list_container, (IView**)&mListContainer);
    if (mListContainer != NULL) {
        // Create adapter and list view here
        AutoPtr<IView> emptyView;
        mListContainer->FindViewById(Elastos::Droid::R::id::empty, (IView**)&emptyView);
        tmp = NULL;
        mListContainer->FindViewById(Elastos::Droid::R::id::list, (IView**)&tmp);
        AutoPtr<IListView> lv = IListView::Probe(tmp);
        IAdapterView* _lv = IAdapterView::Probe(lv);
        if (emptyView != NULL) {
            _lv->SetEmptyView(emptyView);
        }
        _lv->SetOnItemClickListener(this);
        IView::Probe(lv)->SetSaveEnabled(TRUE);
        lv->SetItemsCanFocus(TRUE);
        IAbsListView::Probe(lv)->SetTextFilterEnabled(TRUE);
        mListView = lv;
        mApplications = new ApplicationsAdapter(mApplicationsState, this, mFilter);
        IAdapterView::Probe(mListView)->SetAdapter((IAdapter*)mApplications);
        IAbsListView::Probe(mListView)->SetRecyclerListener((IRecyclerListener*)mApplications);
        //mColorBar = (LinearColorBar)mListContainer->FindViewById(R.id.storage_color_bar);
        //mStorageChartLabel = (TextView)mListContainer->FindViewById(R.id.storageChartLabel);
        //mUsedStorageText = (TextView)mListContainer->FindViewById(R.id.usedStorageText);
        //mFreeStorageText = (TextView)mListContainer->FindViewById(R.id.freeStorageText);
        Utils::PrepareCustomPreferencesList(contentParent, contentChild, IView::Probe(mListView), FALSE);
        if (mFilter == FILTER_APPS_SDCARD) {
            //mStorageChartLabel->SetText(mOwner->GetActivity()->GetText(
            //        R::string::sd_card_storage));
        }
        else {
            //mStorageChartLabel->SetText(mOwner->GetActivity()->GetText(
            //        R::string::internal_storage));
        }
        ApplyCurrentStorage();
    }
    tmp = NULL;
    mRootView->FindViewById(
            R::id::running_processes, (IView**)&tmp);
    mRunningProcessesView = (CRunningProcessesView*)tmp.Get();
    if (mRunningProcessesView != NULL) {
        mRunningProcessesView->DoCreate(mSavedInstanceState);
    }

    return mRootView;
}

ECode ManageApplications::TabInfo::DetachView()
{
    if (mRootView != NULL) {
        AutoPtr<IViewParent> tmp;
        mRootView->GetParent((IViewParent**)&tmp);
        IViewGroup* group = IViewGroup::Probe(tmp);
        if (group != NULL) {
            group->RemoveView(mRootView);
        }
    }
    return NOERROR;
}

ECode ManageApplications::TabInfo::Resume(
    /* [in] */ Int32 sortOrder)
{
    if (mApplications != NULL) {
        mApplications->Resume(sortOrder);
    }
    if (mRunningProcessesView != NULL) {
        Boolean haveData = mRunningProcessesView->DoResume(mOwner, mRunningProcessesAvail);
        if (haveData) {
            mRunningProcessesView->SetVisibility(IView::VISIBLE);
            mLoadingContainer->SetVisibility(IView::INVISIBLE);
        }
        else {
            mLoadingContainer->SetVisibility(IView::VISIBLE);
        }
    }
    return NOERROR;
}

ECode ManageApplications::TabInfo::Pause()
{
    if (mApplications != NULL) {
        mApplications->Pause();
    }
    if (mRunningProcessesView != NULL) {
        mRunningProcessesView->DoPause();
    }
    return NOERROR;
}

ECode ManageApplications::TabInfo::ReleaseItem()
{
    if (mApplications != NULL) {
        mApplications->ReleaseItem();
    }
    return NOERROR;
}

void ManageApplications::TabInfo::UpdateStorageUsage()
{
    // Make sure a callback didn't come at an inopportune time.
    AutoPtr<IActivity> activity;
    mOwner->GetActivity((IActivity**)&activity);
    if (activity == NULL) return;
    // Doesn't make sense for stuff that is not an app list.
    if (mApplications == NULL) return;

    mFreeStorage = 0;
    mAppStorage = 0;
    mTotalStorage = 0;

    ECode ec = NOERROR;

    AutoPtr<IEnvironment> environment;
    CEnvironment::AcquireSingleton((IEnvironment**)&environment);
    if (mFilter == FILTER_APPS_SDCARD) {
        if (mContainerService != NULL) {
            // try {
                AutoPtr<IFile> esd;
                environment->GetExternalStorageDirectory((IFile**)&esd);
                String path;
                esd->GetPath(&path);
                AutoPtr< ArrayOf<Int64> > stats;
                ec = mContainerService->GetFileSystemStats(path, (ArrayOf<Int64>**)&stats);
                if (ec == (ECode)E_REMOTE_EXCEPTION) {
                    Logger::W(TAG, "Problem in container service 0x%08x", ec);
                }
                mTotalStorage = (*stats)[0];
                mFreeStorage = (*stats)[1];
            // } catch (RemoteException e) {
            //     Logger::W(TAG, "Problem in container service", e);
            // }
        }

        if (mApplications != NULL) {
            Int32 N;
            mApplications->GetCount(&N);
            for (Int32 i = 0; i < N; i++) {
                AutoPtr<ApplicationsState::AppEntry> ae = mApplications->GetAppEntry(i);
                mAppStorage += ae->mExternalCodeSize + ae->mExternalDataSize
                        + ae->mExternalCacheSize;
            }
        }
    }
    else {
        if (mContainerService != NULL) {
            // try {
                AutoPtr<IFile> dir;
                environment->GetDataDirectory((IFile**)&dir);
                String path;
                dir->GetPath(&path);
                AutoPtr< ArrayOf<Int64> > stats;
                ec = mContainerService->GetFileSystemStats(path, (ArrayOf<Int64>**)&stats);
                if (ec == (ECode)E_REMOTE_EXCEPTION) {
                    Logger::W(TAG, "Problem in container service 0x%08x", ec);
                }
                mTotalStorage = (*stats)[0];
                mFreeStorage = (*stats)[1];
            // } catch (RemoteException e) {
            //     Logger::W(TAG, "Problem in container service", e);
            // }
        }

        Boolean emulatedStorage;
        environment->IsExternalStorageEmulated(&emulatedStorage);
        if (mApplications != NULL) {
            Int32 N;
            mApplications->GetCount(&N);
            for (Int32 i = 0; i < N; i++) {
                AutoPtr<ApplicationsState::AppEntry> ae = mApplications->GetAppEntry(i);
                mAppStorage += ae->mCodeSize + ae->mDataSize;
                if (emulatedStorage) {
                    mAppStorage += ae->mExternalCodeSize + ae->mExternalDataSize;
                }
            }
        }
        mFreeStorage += mApplicationsState->SumCacheSizes();
    }

    ApplyCurrentStorage();
}

void ManageApplications::TabInfo::ApplyCurrentStorage()
{
    // If view hierarchy is not yet created, no views to update.
    if (mRootView == NULL) {
        return;
    }
    /*
    if (mTotalStorage > 0) {
        BidiFormatter bidiFormatter = BidiFormatter->GetInstance();
        mColorBar->SetRatios((mTotalStorage-mFreeStorage-mAppStorage)/(Float)mTotalStorage,
                mAppStorage/(Float)mTotalStorage, mFreeStorage/(Float)mTotalStorage);
        Int64 usedStorage = mTotalStorage - mFreeStorage;
        if (mLastUsedStorage != usedStorage) {
            mLastUsedStorage = usedStorage;
            String sizeStr = bidiFormatter->UnicodeWrap(
                    Formatter->FormatShortFileSize(mOwner->GetActivity(), usedStorage));
            mUsedStorageText->SetText(mOwner->GetActivity()->GetResources().GetString(
                    R::string::service_foreground_processes, sizeStr));
        }
        if (mLastFreeStorage != mFreeStorage) {
            mLastFreeStorage = mFreeStorage;
            String sizeStr = bidiFormatter->UnicodeWrap(
                    Formatter->FormatShortFileSize(mOwner->GetActivity(), mFreeStorage));
            mFreeStorageText->SetText(mOwner->GetActivity()->GetResources().GetString(
                    R::string::service_background_processes, sizeStr));
        }
    }
    else {
        mColorBar->SetRatios(0, 0, 0);
        if (mLastUsedStorage != -1) {
            mLastUsedStorage = -1;
            mUsedStorageText->SetText("");
        }
        if (mLastFreeStorage != -1) {
            mLastFreeStorage = -1;
            mFreeStorageText->SetText("");
        }
    }
    */
}

ECode ManageApplications::TabInfo::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return mClickListener->OnItemClick(this, parent, view, position, id);
}

void ManageApplications::TabInfo::HandleRunningProcessesAvail()
{
    AutoPtr<IActivity> _activity;
    mOwner->GetActivity((IActivity**)&_activity);
    IContext* activity = IContext::Probe(_activity);
    AutoPtr<IAnimation> animation;
    AnimationUtils::LoadAnimation(
            activity, Elastos::Droid::R::anim::fade_out, (IAnimation**)&animation);
    mLoadingContainer->StartAnimation(animation);
    animation = NULL;
    AnimationUtils::LoadAnimation(
            activity, Elastos::Droid::R::anim::fade_in, (IAnimation**)&animation);
    mRunningProcessesView->StartAnimation(animation);
    mRunningProcessesView->SetVisibility(IView::VISIBLE);
    mLoadingContainer->SetVisibility(IView::GONE);
}

//===============================================================================
//                  ManageApplications::ApplicationsAdapter
//===============================================================================

CAR_INTERFACE_IMPL_3(ManageApplications::ApplicationsAdapter, BaseAdapter, IFilterable, IApplicationsStateCallbacks, IRecyclerListener)

ManageApplications::ApplicationsAdapter::ApplicationsAdapter(
    /* [in] */ ApplicationsState* state,
    /* [in] */ TabInfo* tab,
    /* [in] */ Int32 filterMode)
    : mFilterMode(0)
    , mResumed(FALSE)
    , mLastSortMode(-1)
    , mWaitingForData(FALSE)
    , mWhichSize(SIZE_TOTAL)
{
    CArrayList::New((IArrayList**)&mActive);
    mFilter = new ApplicationsAdapterFilter(this);

    mState = state;
    mSession = state->NewSession(this);
    mTab = tab;
    AutoPtr<IActivity> activity;
    tab->mOwner->GetActivity((IActivity**)&activity);
    mContext = IContext::Probe(activity);
    mFilterMode = filterMode;
}

ECode ManageApplications::ApplicationsAdapter::Resume(
    /* [in] */ Int32 sort)
{
    if (DEBUG) Logger::I(TAG, "Resume!  mResumed=%d", mResumed);
    if (!mResumed) {
        mResumed = TRUE;
        mSession->Resume();
        mLastSortMode = sort;
        Rebuild(TRUE);
    }
    else {
        Rebuild(sort);
    }
    return NOERROR;
}

ECode ManageApplications::ApplicationsAdapter::Pause()
{
    if (mResumed) {
        mResumed = FALSE;
        mSession->Pause();
    }
    return NOERROR;
}

ECode ManageApplications::ApplicationsAdapter::ReleaseItem()
{
    return mSession->ReleaseItem();
}

ECode ManageApplications::ApplicationsAdapter::Rebuild(
    /* [in] */ Int32 sort)
{
    if (sort == mLastSortMode) {
        return NOERROR;
    }
    mLastSortMode = sort;
    return Rebuild(TRUE);
}

ECode ManageApplications::ApplicationsAdapter::Rebuild(
    /* [in] */ Boolean eraseold)
{
    if (DEBUG) Logger::I(TAG, "Rebuilding app list...");
    AutoPtr<IApplicationsStateAppFilter> filterObj;
    AutoPtr<IComparator> comparatorObj; //Comparator<AppEntry>
    AutoPtr<IEnvironment> environment;
    CEnvironment::AcquireSingleton((IEnvironment**)&environment);
    Boolean emulated;
    environment->IsExternalStorageEmulated(&emulated);
    if (emulated) {
        mWhichSize = SIZE_TOTAL;
    }
    else {
        mWhichSize = SIZE_INTERNAL;
    }
    switch (mFilterMode) {
        case FILTER_APPS_THIRD_PARTY:
            filterObj = ApplicationsState::THIRD_PARTY_FILTER;
            break;
        case FILTER_APPS_SDCARD:
            filterObj = ApplicationsState::ON_SD_CARD_FILTER;
            if (!emulated) {
                mWhichSize = SIZE_EXTERNAL;
            }
            break;
        case FILTER_APPS_DISABLED:
            filterObj = ApplicationsState::DISABLED_FILTER;
            break;
        default:
            filterObj = ApplicationsState::ALL_ENABLED_FILTER;
            break;
    }
    switch (mLastSortMode) {
        case SORT_ORDER_SIZE:
            switch (mWhichSize) {
                case SIZE_INTERNAL:
                    comparatorObj = ApplicationsState::INTERNAL_SIZE_COMPARATOR;
                    break;
                case SIZE_EXTERNAL:
                    comparatorObj = ApplicationsState::EXTERNAL_SIZE_COMPARATOR;
                    break;
                default:
                    comparatorObj = ApplicationsState::SIZE_COMPARATOR;
                    break;
            }
            break;
        default:
            comparatorObj = ApplicationsState::ALPHA_COMPARATOR;
            break;
    }
    AutoPtr<IArrayList>/*ArrayList<ApplicationsState::AppEntry>*/ entries
            = mSession->Rebuild(filterObj, comparatorObj);
    if (entries == NULL && !eraseold) {
        // Don't have new list yet, but can continue using the old one.
        return NOERROR;
    }
    mBaseEntries = entries;
    if (mBaseEntries != NULL) {
        mEntries = ApplyPrefixFilter(mCurFilterPrefix, mBaseEntries);
    }
    else {
        mEntries = NULL;
    }
    NotifyDataSetChanged();
    mTab->UpdateStorageUsage();

    if (entries == NULL) {
        mWaitingForData = TRUE;
        mTab->mListContainer->SetVisibility(IView::INVISIBLE);
        mTab->mLoadingContainer->SetVisibility(IView::VISIBLE);
    }
    else {
        mTab->mListContainer->SetVisibility(IView::VISIBLE);
        mTab->mLoadingContainer->SetVisibility(IView::GONE);
    }
    return NOERROR;
}

AutoPtr<IArrayList> ManageApplications::ApplicationsAdapter::ApplyPrefixFilter(
    /* [in] */ ICharSequence* prefix,
    /* [in] */ IArrayList* origEntries)
{
    AutoPtr<IArrayList> result = origEntries;
    Int32 length;
    if (prefix == NULL || (prefix->GetLength(&length), length == 0)) {
        return result;
    }
    else {
        String str;
        prefix->ToString(&str);
        String prefixStr = ApplicationsState::Normalize(str);
        String spacePrefixStr = String(" ") + prefixStr;
        AutoPtr<IArrayList> newEntries;
        CArrayList::New((IArrayList**)&newEntries);
        Int32 size;
        origEntries->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            origEntries->Get(i, (IInterface**)&obj);
            IApplicationsStateAppEntry* entry = IApplicationsStateAppEntry::Probe(obj);
            String nlabel = ((ApplicationsState::AppEntry*)entry)->GetNormalizedLabel();
            if (nlabel.StartWith(prefixStr) || nlabel.IndexOf(spacePrefixStr) != -1) {
                newEntries->Add(entry);
            }
        }
        return newEntries;
    }
}

ECode ManageApplications::ApplicationsAdapter::OnRunningStateChanged(
    /* [in] */ Boolean running)
{
    AutoPtr<IActivity> activity;
    mTab->mOwner->GetActivity((IActivity**)&activity);
    activity->SetProgressBarIndeterminateVisibility(running);
    return NOERROR;
}

ECode ManageApplications::ApplicationsAdapter::OnRebuildComplete(
    /* [in] */ IArrayList* apps)
{
    Int32 visibility;
    mTab->mLoadingContainer->GetVisibility(&visibility);
    if (visibility == IView::VISIBLE) {
        AutoPtr<IAnimation> animation;
        AnimationUtils::LoadAnimation(
                mContext, Elastos::Droid::R::anim::fade_out, (IAnimation**)&animation);
        mTab->mLoadingContainer->StartAnimation(animation);
        animation = NULL;
        AnimationUtils::LoadAnimation(
                mContext, Elastos::Droid::R::anim::fade_in, (IAnimation**)&animation);
        mTab->mListContainer->StartAnimation(animation);
    }
    mTab->mListContainer->SetVisibility(IView::VISIBLE);
    mTab->mLoadingContainer->SetVisibility(IView::GONE);
    mWaitingForData = FALSE;
    mBaseEntries = apps;
    mEntries = ApplyPrefixFilter(mCurFilterPrefix, mBaseEntries);
    NotifyDataSetChanged();
    mTab->UpdateStorageUsage();
    return NOERROR;
}

ECode ManageApplications::ApplicationsAdapter::OnPackageListChanged()
{
    return Rebuild(FALSE);
}

ECode ManageApplications::ApplicationsAdapter::OnPackageIconChanged()
{
    // We ensure icons are loaded when their item is displayed, so
    // don't care about icons loaded in the background.
    return NOERROR;
}

ECode ManageApplications::ApplicationsAdapter::OnPackageSizeChanged(
    /* [in] */ const String& packageName)
{
    Int32 size;
    mActive->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mActive->Get(i, (IInterface**)&obj);
        IView* view = IView::Probe(obj);
        AutoPtr<IInterface> tmp;
        view->GetTag((IInterface**)&tmp);
        AppViewHolder* holder = (AppViewHolder*) IObject::Probe(tmp);
        String name;
        IPackageItemInfo::Probe(holder->mEntry->mInfo)->GetPackageName(&name);
        if (name.Equals(packageName)) {
            {
                AutoLock syncLock(holder->mEntry);
                holder->UpdateSizeText(mTab->mInvalidSizeStr, mWhichSize);
            }
            if (name.Equals(mTab->mOwner->mCurrentPkgName)
                    && mLastSortMode == SORT_ORDER_SIZE) {
                // We got the size information for the last app the
                // user viewed, and are sorting by size...  they may
                // have cleared data, so we immediately want to resort
                // the list with the new size to reflect it to the user.
                Rebuild(FALSE);
            }
            mTab->UpdateStorageUsage();
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode ManageApplications::ApplicationsAdapter::OnAllSizesComputed()
{
    if (mLastSortMode == SORT_ORDER_SIZE) {
        Rebuild(FALSE);
    }
    mTab->UpdateStorageUsage();
    return NOERROR;
}

ECode ManageApplications::ApplicationsAdapter::GetCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (mEntries != NULL) {
        mEntries->GetSize(result);
    }
    return NOERROR;
}

ECode ManageApplications::ApplicationsAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    return mEntries->Get(position, result);
}

AutoPtr<ApplicationsState::AppEntry> ManageApplications::ApplicationsAdapter::GetAppEntry(
    /* [in] */ Int32 position)
{
    AutoPtr<IInterface> obj;
    mEntries->Get(position, (IInterface**)&obj);
    AutoPtr<ApplicationsState::AppEntry> entry = (ApplicationsState::AppEntry*)IApplicationsStateAppEntry::Probe(obj);
    return entry;
}

ECode ManageApplications::ApplicationsAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    mEntries->Get(position, (IInterface**)&obj);

    *result = ((ApplicationsState::AppEntry*)IApplicationsStateAppEntry::Probe(obj))->mId;
    return NOERROR;
}

ECode ManageApplications::ApplicationsAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* _convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IView> convertView = _convertView;
    // A ViewHolder keeps references to children views to avoid unnecessary calls
    // to FindViewById() on each row.
    AutoPtr<AppViewHolder> holder = AppViewHolder::CreateOrRecycle(mTab->mInflater, convertView);
    convertView = holder->mRootView;

    // Bind the data efficiently with the holder
    AutoPtr<IInterface> obj;
    mEntries->Get(position, (IInterface**)&obj);
    AutoPtr<ApplicationsState::AppEntry> entry = (ApplicationsState::AppEntry*)IApplicationsStateAppEntry::Probe(obj);
    {
        AutoLock syncLock(entry);
        holder->mEntry = entry;
        if (!entry->mLabel.IsNull()) {
            holder->mAppName->SetText(CoreUtils::Convert(entry->mLabel));
        }
        mState->EnsureIcon(entry);
        if (entry->mIcon != NULL) {
            holder->mAppIcon->SetImageDrawable(entry->mIcon);
        }
        holder->UpdateSizeText(mTab->mInvalidSizeStr, mWhichSize);
        Int32 flags;
        entry->mInfo->GetFlags(&flags);
        Boolean enabled;
        if ((flags & IApplicationInfo::FLAG_INSTALLED) == 0) {
            IView::Probe(holder->mDisabled)->SetVisibility(IView::VISIBLE);
            holder->mDisabled->SetText(R::string::not_installed);
        }
        else if (entry->mInfo->GetEnabled(&enabled), !enabled) {
            IView::Probe(holder->mDisabled)->SetVisibility(IView::VISIBLE);
            holder->mDisabled->SetText(R::string::disabled);
        }
        else {
            IView::Probe(holder->mDisabled)->SetVisibility(IView::GONE);
        }
        if (mFilterMode == FILTER_APPS_SDCARD) {
            IView::Probe(holder->mCheckBox)->SetVisibility(IView::VISIBLE);
            ICheckable::Probe(holder->mCheckBox)->SetChecked((flags
                    & IApplicationInfo::FLAG_EXTERNAL_STORAGE) != 0);
        }
        else {
            IView::Probe(holder->mCheckBox)->SetVisibility(IView::GONE);
        }
    }
    mActive->Remove(convertView);
    mActive->Add(convertView);
    *result = convertView;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ManageApplications::ApplicationsAdapter::GetFilter(
    /* [out] */ IFilter** filter)
{
    VALIDATE_NOT_NULL(filter)
    *filter = mFilter;
    REFCOUNT_ADD(*filter)
    return NOERROR;
}

ECode ManageApplications::ApplicationsAdapter::OnMovedToScrapHeap(
    /* [in] */ IView* view)
{
    return mActive->Remove(view);
}

//===============================================================================
//                  ManageApplications::TabInfoRunnable
//===============================================================================

ManageApplications::TabInfoRunnable::TabInfoRunnable(
    /* [in] */ TabInfo* host)
    : mHost(host)
{}

ManageApplications::TabInfoRunnable::~TabInfoRunnable()
{}

ECode ManageApplications::TabInfoRunnable::Run()
{
    mHost->HandleRunningProcessesAvail();
    return NOERROR;
}

// ===============================================================================
//                  ManageApplications::MyPagerAdapter
// ===============================================================================

CAR_INTERFACE_IMPL(ManageApplications::MyPagerAdapter, PagerAdapter, IViewPagerOnPageChangeListener)

ManageApplications::MyPagerAdapter::MyPagerAdapter(
    /* [in] */ ManageApplications* host)
    : mCurPos(0)
    , mHost(host)
{}

ManageApplications::MyPagerAdapter::~MyPagerAdapter()
{}

ECode ManageApplications::MyPagerAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = mHost->mNumTabs;
    return NOERROR;
}

ECode ManageApplications::MyPagerAdapter::InstantiateItem(
    /* [in] */ IViewGroup* container,
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item)

    AutoPtr<IInterface> obj;
    mHost->mTabs->Get(position, (IInterface**)&obj);
    IManageApplicationsTabInfo* tab = IManageApplicationsTabInfo::Probe(obj);
    AutoPtr<IView> root = ((TabInfo*)tab)->Build(mHost->mInflater, mHost->mContentContainer, mHost->mRootView);
    container->AddView(root);
    root->SetTag(R::id::name, tab);
    *item = root;
    REFCOUNT_ADD(*item)
    return NOERROR;
}

ECode ManageApplications::MyPagerAdapter::DestroyItem(
    /* [in] */ IViewGroup* container,
    /* [in] */ Int32 position,
    /* [in] */ IInterface* object)
{
    return container->RemoveView(IView::Probe(object));
}

ECode ManageApplications::MyPagerAdapter::IsViewFromObject(
    /* [in] */ IView* view,
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = view == IView::Probe(object);
    return NOERROR;
}

ECode ManageApplications::MyPagerAdapter::GetItemPosition(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* pos)
{
    VALIDATE_NOT_NULL(pos)
    //return ((TabInfo)((View)object).GetTag(R.id.name)).mListType;
    return PagerAdapter::GetItemPosition(object, pos);
}

ECode ManageApplications::MyPagerAdapter::GetPageTitle(
    /* [in] */ Int32 position,
    /* [out] */ ICharSequence** title)
{
    VALIDATE_NOT_NULL(title)
    AutoPtr<IInterface> obj;
    mHost->mTabs->Get(position, (IInterface**)&obj);
    *title = ((TabInfo*)IManageApplicationsTabInfo::Probe(obj))->mLabel;
    REFCOUNT_ADD(*title)
    return NOERROR;
}

ECode ManageApplications::MyPagerAdapter::OnPageScrolled(
    /* [in] */ Int32 position,
    /* [in] */ Float positionOffset,
    /* [in] */ Int32 positionOffsetPixels)
{
    return NOERROR;
}

ECode ManageApplications::MyPagerAdapter::OnPageSelected(
    /* [in] */ Int32 position)
{
    mCurPos = position;
    return NOERROR;
}

ECode ManageApplications::MyPagerAdapter::OnPageScrollStateChanged(
    /* [in] */ Int32 state)
{
    if (state == IViewPager::SCROLL_STATE_IDLE) {
        mHost->UpdateCurrentTab(mCurPos);
    }
    return NOERROR;
}

//===============================================================================
//                  ManageApplications::ApplicationsAdapterFilter
//===============================================================================

ManageApplications::ApplicationsAdapterFilter::ApplicationsAdapterFilter(
    /* [in] */ ApplicationsAdapter* host)
    : mHost(host)
{}

ManageApplications::ApplicationsAdapterFilter::~ApplicationsAdapterFilter()
{}

ECode ManageApplications::ApplicationsAdapterFilter::PerformFiltering(
    /* [in] */ ICharSequence* constraint,
    /* [out] */ IFilterResults** filterResults)
{
    VALIDATE_NOT_NULL(filterResults)

    AutoPtr<IArrayList> entries = mHost->ApplyPrefixFilter(constraint, mHost->mBaseEntries);
    AutoPtr<IFilterResults> fr = new Filter::FilterResults();
    fr->SetValues(entries);
    Int32 size;
    entries->GetSize(&size);
    fr->SetCount(size);
    *filterResults = fr;
    REFCOUNT_ADD(*filterResults)
    return NOERROR;
}

ECode ManageApplications::ApplicationsAdapterFilter::PublishResults(
    /* [in] */ ICharSequence* constraint,
    /* [in] */ IFilterResults* results)
{
    mHost->mCurFilterPrefix = constraint;
    AutoPtr<IInterface> obj;
    results->GetValues((IInterface**)&obj);
    mHost->mEntries = IArrayList::Probe(obj);
    mHost->NotifyDataSetChanged();
    mHost->mTab->UpdateStorageUsage();
    return NOERROR;
}

//===============================================================================
//                  ManageApplications::OnClickAsyncTask
//===============================================================================

ManageApplications::OnClickAsyncTask::OnClickAsyncTask(
    /* [in] */ ManageApplications* host,
    /* [in] */ IPackageManager* pm,
    /* [in] */ IINotificationManager* nm,
    /* [in] */ IIPackageManager* ipm,
    /* [in] */ IAppOpsManager* aom,
    /* [in] */ INetworkPolicyManager* npm,
    /* [in] */ IHandler* handler)
    : mHost(host)
    , mPm(pm)
    , mNm(nm)
    , mIPm(ipm)
    , mAom(aom)
    , mNpm(npm)
    , mHandler(handler)
{}

ManageApplications::OnClickAsyncTask::~OnClickAsyncTask()
{}

ECode ManageApplications::OnClickAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IList> apps;
    mPm->GetInstalledApplications(
            IPackageManager::GET_DISABLED_COMPONENTS, (IList**)&apps);
    Int32 size;
    apps->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        apps->Get(i, (IInterface**)&obj);
        IApplicationInfo* app = IApplicationInfo::Probe(obj);
        // try {
        String packageName;
        IPackageItemInfo::Probe(app)->GetPackageName(&packageName);
        if (DEBUG) Logger::V(ManageApplications::TAG, "Enabling notifications: %s", packageName.string());
        Int32 uid;
        app->GetUid(&uid);
        mNm->SetNotificationsEnabledForPackage(packageName, uid, TRUE);
        // } catch (android.os.RemoteException ex) {
        // }
        Boolean enabled;
        if (app->GetEnabled(&enabled), !enabled) {
            if (DEBUG) Logger::V(ManageApplications::TAG, "Enabling app: %s", packageName.string());
            Int32 value;
            mPm->GetApplicationEnabledSetting(packageName, &value);
            if (value == IPackageManager::COMPONENT_ENABLED_STATE_DISABLED_USER) {
                mPm->SetApplicationEnabledSetting(packageName,
                        IPackageManager::COMPONENT_ENABLED_STATE_DEFAULT,
                        IPackageManager::DONT_KILL_APP);
            }
        }
    }
    // try {
    mIPm->ResetPreferredActivities(UserHandle::GetMyUserId());
    // } catch (RemoteException e) {
    // }
    mAom->ResetAllModes();
    AutoPtr< ArrayOf<Int32> > restrictedUids;
    mNpm->GetUidsWithPolicy(
            INetworkPolicyManager::POLICY_REJECT_METERED_BACKGROUND, (ArrayOf<Int32>**)&restrictedUids);
    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Int32 currentUserId;
    helper->GetCurrentUser(&currentUserId);
    for (Int32 i = 0; i < restrictedUids->GetLength(); ++i) {
        Int32 uid = (*restrictedUids)[i];
        // Only reset for current user
        if (UserHandle::GetUserId(uid) == currentUserId) {
            if (DEBUG) Logger::V(ManageApplications::TAG, "Clearing data policy: %d", uid);
            mNpm->SetUidPolicy(uid, INetworkPolicyManager::POLICY_NONE);
        }
    }

    AutoPtr<Runnable> runnable = new OnClickRunnable(mHost);
    Boolean res;
    mHandler->Post(runnable, &res);

    *result = NULL;
    return NOERROR;
}

//===============================================================================
//                  ManageApplications::OnClickRunnable
//===============================================================================

ManageApplications::OnClickRunnable::OnClickRunnable(
    /* [in] */ ManageApplications* host)
    : mHost(host)
{}

ManageApplications::OnClickRunnable::~OnClickRunnable()
{}

ECode ManageApplications::OnClickRunnable::Run()
{
    if (DEBUG) Logger::V(TAG, "Done clearing");
    AutoPtr<IActivity> activity;
    mHost->GetActivity((IActivity**)&activity);
    if (activity != NULL && mHost->mActivityResumed) {
        if (DEBUG) Logger::V(TAG, "Updating UI!");
        Int32 size;
        mHost->mTabs->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mHost->mTabs->Get(i, (IInterface**)&obj);
            AutoPtr<TabInfo> tab = (TabInfo*)IManageApplicationsTabInfo::Probe(obj);
            if (tab->mApplications != NULL) {
                tab->mApplications->Pause();
            }
        }
        if (mHost->mCurTab != NULL) {
            mHost->mCurTab->Resume(mHost->mSortOrder);
        }
    }
    return NOERROR;
}

//===============================================================================
//                  ManageApplications::InitServiceConnection
//===============================================================================

CAR_INTERFACE_IMPL(ManageApplications::InitServiceConnection, Object, IServiceConnection)

ManageApplications::InitServiceConnection::InitServiceConnection(
    /* [in] */ ManageApplications* host)
    : mHost(host)
{}

ManageApplications::InitServiceConnection::~InitServiceConnection()
{}

ECode ManageApplications::InitServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    mHost->mContainerService = IIMediaContainerService::Probe(service);
    Int32 size;
    mHost->mTabs->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mHost->mTabs->Get(i, (IInterface**)&obj);
        ((TabInfo*)IManageApplicationsTabInfo::Probe(obj))->SetContainerService(mHost->mContainerService);
    }
    return NOERROR;
}

ECode ManageApplications::InitServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    mHost->mContainerService = NULL;
    return NOERROR;
}

//===============================================================================
//                  ManageApplications
//===============================================================================

CAR_INTERFACE_IMPL_3(ManageApplications, Fragment, IManageApplicationsAppClickListener,
        IDialogInterfaceOnClickListener, IAdapterViewOnItemSelectedListener)

ManageApplications::ManageApplications()
    : mSortOrder(SORT_ORDER_ALPHA)
    , mNumTabs(0)
    , mActivityResumed(FALSE)
    , mShowBackground(FALSE)
    , mDefaultListType(-1)
{
    CArrayList::New((IArrayList**)&mTabs);
    mContainerConnection = new InitServiceConnection(this);
}

ManageApplications::~ManageApplications()
{}

ECode ManageApplications::constructor()
{
    return Fragment::constructor();
}

ECode ManageApplications::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::I(TAG, " >> enter ManageApplications::OnCreate");
    Fragment::OnCreate(savedInstanceState);

    SetHasOptionsMenu(TRUE);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);

    mContext = IContext::Probe(activity);
    AutoPtr<IApplication> application;
    activity->GetApplication((IApplication**)&application);
    mApplicationsState = ApplicationsState::GetInstance(application);
    AutoPtr<IIntent> intent;
    activity->GetIntent((IIntent**)&intent);
    String action;
    intent->GetAction(&action);
    Int32 defaultListType = LIST_TYPE_DOWNLOADED;
    AutoPtr<IBundle> bundle;
    GetArguments((IBundle**)&bundle);
    String className;
    if (bundle != NULL) {
        bundle->GetString(String("classname"), &className);
    }
    if (className.IsNull()) {
        AutoPtr<IComponentName> comp;
        intent->GetComponent((IComponentName**)&comp);
        comp->GetClassName(&className);
    }
    if (className.Equals("Elastos.Droid.Settings.CSettings.RunningServicesActivity")
            || className.EndWith(".RunningServices")) {
        defaultListType = LIST_TYPE_RUNNING;
    }
    else if (className.Equals("Elastos.Droid.Settings.CSettings.StorageUseActivity")
            || IIntent::ACTION_MANAGE_PACKAGE_STORAGE.Equals(action)
            || className.EndWith(".StorageUse")) {
        mSortOrder = SORT_ORDER_SIZE;
        defaultListType = LIST_TYPE_ALL;
    }
    else if (ISettings::ACTION_MANAGE_ALL_APPLICATIONS_SETTINGS.Equals(action)) {
        // Select the all-apps list, with the default sorting
        defaultListType = LIST_TYPE_ALL;
    }

    if (savedInstanceState != NULL) {
        savedInstanceState->GetInt32(EXTRA_SORT_ORDER, mSortOrder, &mSortOrder);
        Int32 tmp;
        savedInstanceState->GetInt32(EXTRA_DEFAULT_LIST_TYPE, -1, &tmp);
        if (tmp != -1) defaultListType = tmp;
        savedInstanceState->GetBoolean(EXTRA_SHOW_BACKGROUND, FALSE, &mShowBackground);
    }

    mDefaultListType = defaultListType;

    AutoPtr<IIntent> containerIntent;
    CIntent::New((IIntent**)&containerIntent);
    containerIntent->SetComponent(StorageMeasurement::DEFAULT_CONTAINER_COMPONENT);

    IContext* _activity = IContext::Probe(activity);
    Boolean res;
    _activity->BindService(containerIntent, mContainerConnection, IContext::BIND_AUTO_CREATE, &res);

    _activity->GetText(R::string::invalid_size_value, (ICharSequence**)&mInvalidSizeStr);
    _activity->GetText(R::string::computing_size, (ICharSequence**)&mComputingSizeStr);

    String str;
    _activity->GetString(R::string::filter_apps_third_party, &str);
    AutoPtr<TabInfo> tab = new TabInfo(this, mApplicationsState,
            CoreUtils::Convert(str), LIST_TYPE_DOWNLOADED, this, savedInstanceState);
    mTabs->Add((IManageApplicationsTabInfo*)tab);

    AutoPtr<IEnvironment> environment;
    CEnvironment::AcquireSingleton((IEnvironment**)&environment);
    if (environment->IsExternalStorageEmulated(&res), !res) {
        _activity->GetString(R::string::filter_apps_onsdcard, &str);
        tab = new TabInfo(this, mApplicationsState,
                CoreUtils::Convert(str), LIST_TYPE_SDCARD, this, savedInstanceState);
        mTabs->Add((IManageApplicationsTabInfo*)tab);
    }

    _activity->GetString(R::string::filter_apps_running, &str);
    tab = new TabInfo(this, mApplicationsState,
            CoreUtils::Convert(str), LIST_TYPE_RUNNING, this, savedInstanceState);
    mTabs->Add((IManageApplicationsTabInfo*)tab);

    _activity->GetString(R::string::filter_apps_all, &str);
    tab = new TabInfo(this, mApplicationsState,
            CoreUtils::Convert(str), LIST_TYPE_ALL, this, savedInstanceState);
    mTabs->Add((IManageApplicationsTabInfo*)tab);

    _activity->GetString(R::string::filter_apps_disabled, &str);
    tab = new TabInfo(this, mApplicationsState,
            CoreUtils::Convert(str), LIST_TYPE_DISABLED, this, savedInstanceState);
    mTabs->Add((IManageApplicationsTabInfo*)tab);

    mTabs->GetSize(&mNumTabs);

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    IUserManager* um = IUserManager::Probe(obj);
    mProfileSpinnerAdapter = Utils::CreateUserSpinnerAdapter(um, mContext);
    Logger::I(TAG, " << leave ManageApplications::OnCreate");
    return NOERROR;
}

ECode ManageApplications::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    // initialize the inflater
    mInflater = inflater;

    AutoPtr<IView> rootView;
    mInflater->Inflate(R::layout::manage_applications_content,
            container, FALSE, (IView**)&rootView);
    mContentContainer = container;
    mRootView = rootView;

    AutoPtr<IView> tmp;
    rootView->FindViewById(R::id::pager, (IView**)&tmp);
    mViewPager = IViewPager::Probe(tmp);
    AutoPtr<MyPagerAdapter> adapter = new MyPagerAdapter(this);
    mViewPager->SetAdapter(adapter);
    mViewPager->SetOnPageChangeListener(adapter);
    tmp = NULL;
    rootView->FindViewById(R::id::tabs, (IView**)&tmp);
    AutoPtr<IPagerTabStrip> tabs = IPagerTabStrip::Probe(tmp);
    tabs->SetTabIndicatorColorResource(R::color::theme_accent);

    // We have to do this now because PreferenceFrameLayout looks at it
    // only when the view is added.
    if (IPreferenceFrameLayout::Probe(container) != NULL) {
        AutoPtr<IViewGroupLayoutParams> params;
        rootView->GetLayoutParams((IViewGroupLayoutParams**)&params);
        IPreferenceFrameLayoutParams::Probe(params)->SetRemoveBorders(TRUE);
    }

    Boolean res;
    if (savedInstanceState != NULL && (savedInstanceState->GetBoolean(EXTRA_RESET_DIALOG, &res), res)) {
        BuildResetDialog();
    }

    if (savedInstanceState == NULL) {
        // First time init: make sure view pager is showing the correct tab.
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr<IIntent> intent;
        activity->GetIntent((IIntent**)&intent);
        Int32 extraCurrentListType;
        intent->GetInt32Extra(EXTRA_LIST_TYPE,
                LIST_TYPE_MISSING, &extraCurrentListType);
        Int32 currentListType = (extraCurrentListType != LIST_TYPE_MISSING)
                ? extraCurrentListType : mDefaultListType;
        for (Int32 i = 0; i < mNumTabs; i++) {
            AutoPtr<IInterface> obj;
            mTabs->Get(i, (IInterface**)&obj);
            IManageApplicationsTabInfo* tab = IManageApplicationsTabInfo::Probe(obj);
            if (((TabInfo*)tab)->mListType == currentListType) {
                mViewPager->SetCurrentItem(i);
                break;
            }
        }
    }

    *result = rootView;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ManageApplications::OnStart()
{
    return Fragment::OnStart();
}

ECode ManageApplications::OnResume()
{
    Fragment::OnResume();
    mActivityResumed = TRUE;
    Int32 item;
    mViewPager->GetCurrentItem(&item);
    UpdateCurrentTab(item);
    UpdateNumTabs();
    UpdateOptionsMenu();
    return NOERROR;
}

ECode ManageApplications::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    Fragment::OnSaveInstanceState(outState);
    outState->PutInt32(EXTRA_SORT_ORDER, mSortOrder);
    if (mDefaultListType != -1) {
        outState->PutInt32(EXTRA_DEFAULT_LIST_TYPE, mDefaultListType);
    }
    outState->PutBoolean(EXTRA_SHOW_BACKGROUND, mShowBackground);
    if (mResetDialog != NULL) {
        outState->PutBoolean(EXTRA_RESET_DIALOG, TRUE);
    }
    return NOERROR;
}

ECode ManageApplications::OnPause()
{
    Fragment::OnPause();
    mActivityResumed = FALSE;
    Int32 size;
    mTabs->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mTabs->Get(i, (IInterface**)&obj);
        ((TabInfo*)IManageApplicationsTabInfo::Probe(obj))->Pause();
    }
    return NOERROR;
}

ECode ManageApplications::OnStop()
{
    Fragment::OnStop();
    if (mResetDialog != NULL) {
        IDialogInterface::Probe(mResetDialog)->Dismiss();
        mResetDialog = NULL;
    }
    return NOERROR;
}

ECode ManageApplications::OnDestroyView()
{
    Fragment::OnDestroyView();

    // We are going to keep the tab data structures around, but they
    // are no longer attached to their view hierarchy.
    Int32 size;
    mTabs->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mTabs->Get(i, (IInterface**)&obj);
        TabInfo* tabinfo = (TabInfo*)IManageApplicationsTabInfo::Probe(obj);
        tabinfo->DetachView();
        tabinfo->ReleaseItem();
    }
    return NOERROR;
}

ECode ManageApplications::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    if (requestCode == INSTALLED_APP_DETAILS && mCurrentPkgName != NULL) {
        mApplicationsState->RequestSize(mCurrentPkgName);
    }
    return NOERROR;
}

ECode ManageApplications::OnItemSelected(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IUserHandle> selectedUser = mProfileSpinnerAdapter->GetUserHandle(position);
    Int32 identifier;
    selectedUser->GetIdentifier(&identifier);
    if (identifier != UserHandle::GetMyUserId()) {
        AutoPtr<IIntent> intent;
        CIntent::New(ISettings::ACTION_APPLICATION_SETTINGS, (IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        intent->AddFlags(IIntent::FLAG_ACTIVITY_CLEAR_TASK);
        Int32 currentTab;
        mViewPager->GetCurrentItem(&currentTab);
        AutoPtr<IInterface> obj;
        mTabs->Get(currentTab, (IInterface**)&obj);
        intent->PutExtra(EXTRA_LIST_TYPE, ((TabInfo*)IManageApplicationsTabInfo::Probe(obj))->mListType);
        mContext->StartActivityAsUser(intent, selectedUser);
    }
    return NOERROR;
}

ECode ManageApplications::OnNothingSelected(
    /* [in] */ IAdapterView* parent)
{
    // Nothing to do
    return NOERROR;
}

void ManageApplications::UpdateNumTabs()
{
    Int32 size;
    mTabs->GetSize(&size);
    Int32 newNum = size;
    if (!mApplicationsState->HaveDisabledApps()) {
        newNum = size -1;
    }

    if (newNum != mNumTabs) {
        mNumTabs = newNum;
        if (mViewPager != NULL) {
            AutoPtr<IPagerAdapter> adapter;
            mViewPager->GetAdapter((IPagerAdapter**)&adapter);
            adapter->NotifyDataSetChanged();
        }
    }
}

AutoPtr<ManageApplications::TabInfo> ManageApplications::TabForType(
    /* [in] */ Int32 type)
{
    Int32 size;
    mTabs->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mTabs->Get(i, (IInterface**)&obj);
        AutoPtr<TabInfo> tab = (TabInfo*)IManageApplicationsTabInfo::Probe(obj);
        if (tab->mListType == type) {
            return tab;
        }
    }
    return NULL;
}

void ManageApplications::StartApplicationDetailsActivity()
{
    // start new fragment to display extended information
    AutoPtr<IBundle> args;
    CBundle::New((IBundle**)&args);
    args->PutString(String("package")/*InstalledAppDetails::ARG_PACKAGE_NAME*/, mCurrentPkgName);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    CSettingsActivity* sa = (CSettingsActivity*)ISettingsActivity::Probe(activity);
    sa->StartPreferencePanel(String("Elastos.Droid.Settings.Applications.CInstalledAppDetails"), args,
            R::string::application_info_label, NULL, this, INSTALLED_APP_DETAILS);
}

ECode ManageApplications::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [in] */ IMenuInflater* inflater)
{
    mOptionsMenu = menu;
    // note: icons removed for now because the cause the new action
    // bar UI to be very confusing.
    AutoPtr<IMenuItem> item;
    menu->Add(0, SORT_ORDER_ALPHA, 1, R::string::sort_order_alpha, (IMenuItem**)&item);
    //item.SetIcon(android.R.drawable.ic_menu_sort_alphabetically)
    item->SetShowAsAction(IMenuItem::SHOW_AS_ACTION_NEVER);
    item = NULL;
    menu->Add(0, SORT_ORDER_SIZE, 2, R::string::sort_order_size, (IMenuItem**)&item);
    //item.SetIcon(android.R.drawable.ic_menu_sort_by_size)
    item->SetShowAsAction(IMenuItem::SHOW_AS_ACTION_NEVER);
    item = NULL;
    menu->Add(0, SHOW_RUNNING_SERVICES, 3, R::string::show_running_services, (IMenuItem**)&item);
    item->SetShowAsAction(IMenuItem::SHOW_AS_ACTION_IF_ROOM);
    item = NULL;
    menu->Add(0, SHOW_BACKGROUND_PROCESSES, 3, R::string::show_background_processes, (IMenuItem**)&item);
    item->SetShowAsAction(IMenuItem::SHOW_AS_ACTION_IF_ROOM);
    item = NULL;
    menu->Add(0, RESET_APP_PREFERENCES, 4, R::string::reset_app_preferences, (IMenuItem**)&item);
    item->SetShowAsAction(IMenuItem::SHOW_AS_ACTION_NEVER);
    UpdateOptionsMenu();
    return NOERROR;
}

ECode ManageApplications::OnPrepareOptionsMenu(
    /* [in] */ IMenu* menu)
{
    UpdateOptionsMenu();
    return NOERROR;
}

ECode ManageApplications::OnDestroyOptionsMenu()
{
    mOptionsMenu = NULL;
    return NOERROR;
}

ECode ManageApplications::OnDestroy()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext::Probe(activity)->UnbindService(mContainerConnection);
    return Fragment::OnDestroy();
}

void ManageApplications::UpdateOptionsMenu()
{
    if (mOptionsMenu == NULL) {
        return;
    }

    /*
     * The running processes screen doesn't use the mApplicationsAdapter
     * so bringing up this menu in that case doesn't make any sense.
     */
    if (mCurTab != NULL && mCurTab->mListType == LIST_TYPE_RUNNING) {
        AutoPtr<TabInfo> tab = TabForType(LIST_TYPE_RUNNING);
        Boolean showingBackground = FALSE;
        if (tab != NULL && tab->mRunningProcessesView != NULL) {
            showingBackground = tab->mRunningProcessesView->mAdapter->GetShowBackground();
        }
        AutoPtr<IMenuItem> item;
        mOptionsMenu->FindItem(SORT_ORDER_ALPHA, (IMenuItem**)&item);
        item->SetVisible(FALSE);
        item = NULL;
        mOptionsMenu->FindItem(SORT_ORDER_SIZE, (IMenuItem**)&item);
        item->SetVisible(FALSE);
        item = NULL;
        mOptionsMenu->FindItem(SHOW_RUNNING_SERVICES, (IMenuItem**)&item);
        item->SetVisible(showingBackground);
        item = NULL;
        mOptionsMenu->FindItem(SHOW_BACKGROUND_PROCESSES, (IMenuItem**)&item);
        item->SetVisible(!showingBackground);
        item = NULL;
        mOptionsMenu->FindItem(RESET_APP_PREFERENCES, (IMenuItem**)&item);
        item->SetVisible(FALSE);
        mShowBackground = showingBackground;
    }
    else {
        AutoPtr<IMenuItem> item;
        mOptionsMenu->FindItem(SORT_ORDER_ALPHA, (IMenuItem**)&item);
        item->SetVisible(mSortOrder != SORT_ORDER_ALPHA);
        item = NULL;
        mOptionsMenu->FindItem(SORT_ORDER_SIZE, (IMenuItem**)&item);
        item->SetVisible(mSortOrder != SORT_ORDER_SIZE);
        item = NULL;
        mOptionsMenu->FindItem(SHOW_RUNNING_SERVICES, (IMenuItem**)&item);
        item->SetVisible(FALSE);
        item = NULL;
        mOptionsMenu->FindItem(SHOW_BACKGROUND_PROCESSES, (IMenuItem**)&item);
        item->SetVisible(FALSE);
        item = NULL;
        mOptionsMenu->FindItem(RESET_APP_PREFERENCES, (IMenuItem**)&item);
        item->SetVisible(TRUE);
    }
}

void ManageApplications::BuildResetDialog()
{
    if (mResetDialog == NULL) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(IContext::Probe(activity), (IAlertDialogBuilder**)&builder);
        builder->SetTitle(R::string::reset_app_preferences_title);
        builder->SetMessage(R::string::reset_app_preferences_desc);
        builder->SetPositiveButton(R::string::reset_app_preferences_button, this);
        builder->SetNegativeButton(R::string::cancel, NULL);
        mResetDialog = NULL;
        builder->Show((IAlertDialog**)&mResetDialog);
        AutoPtr<InnerListener> listener = new InnerListener(this);
        IDialog::Probe(mResetDialog)->SetOnDismissListener(listener);
    }
}

ECode ManageApplications::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    if (IDialogInterface::Probe(mResetDialog) == dialog) {
        mResetDialog = NULL;
    }
    return NOERROR;
}

ECode ManageApplications::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    if (IDialogInterface::Probe(mResetDialog) == dialog) {
        AutoPtr<IActivity> _activity;
        GetActivity((IActivity**)&_activity);
        IContext* activity = IContext::Probe(_activity);
        AutoPtr<IPackageManager> pm;
        activity->GetPackageManager((IPackageManager**)&pm);
        AutoPtr<IIPackageManager> mIPm = IIPackageManager::Probe(ServiceManager::GetService(String("package")));
        AutoPtr<IINotificationManager> nm = IINotificationManager::Probe(
                ServiceManager::GetService(IContext::NOTIFICATION_SERVICE));
        AutoPtr<INetworkPolicyManagerHelper> helper;
        CNetworkPolicyManagerHelper::AcquireSingleton((INetworkPolicyManagerHelper**)&helper);
        AutoPtr<INetworkPolicyManager> npm;
        helper->From(activity, (INetworkPolicyManager**)&npm);
        AutoPtr<IInterface> obj;
        activity->GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&obj);
        AutoPtr<IAppOpsManager> aom = IAppOpsManager::Probe(obj);
        AutoPtr<ILooper> looper;
        activity->GetMainLooper((ILooper**)&looper);
        AutoPtr<IHandler> handler;
        CHandler::New(looper, (IHandler**)&handler);

        AutoPtr<OnClickAsyncTask> task = new OnClickAsyncTask(this, pm, nm, mIPm, aom, npm, handler);
        AutoPtr< ArrayOf<IInterface*> > args;
        task->Execute(args);
    }
    return NOERROR;
}

ECode ManageApplications::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 menuId;
    item->GetItemId(&menuId);
    if ((menuId == SORT_ORDER_ALPHA) || (menuId == SORT_ORDER_SIZE)) {
        mSortOrder = menuId;
        if (mCurTab != NULL && mCurTab->mApplications != NULL) {
            mCurTab->mApplications->Rebuild(mSortOrder);
        }
    }
    else if (menuId == SHOW_RUNNING_SERVICES) {
        mShowBackground = FALSE;
        if (mCurTab != NULL && mCurTab->mRunningProcessesView != NULL) {
            mCurTab->mRunningProcessesView->mAdapter->SetShowBackground(FALSE);
        }
    }
    else if (menuId == SHOW_BACKGROUND_PROCESSES) {
        mShowBackground = TRUE;
        if (mCurTab != NULL && mCurTab->mRunningProcessesView != NULL) {
            mCurTab->mRunningProcessesView->mAdapter->SetShowBackground(TRUE);
        }
    }
    else if (menuId == RESET_APP_PREFERENCES) {
        BuildResetDialog();
    }
    else {
        // Handle the home button
        *result = FALSE;
        return NOERROR;
    }
    UpdateOptionsMenu();
    *result = TRUE;
    return NOERROR;
}

ECode ManageApplications::OnItemClick(
    /* [in] */ IManageApplicationsTabInfo* _tab,
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    TabInfo* tab = (TabInfo*)_tab;
    Int32 count;
    if (tab->mApplications != NULL && (tab->mApplications->GetCount(&count), count) > position) {
        AutoPtr<ApplicationsState::AppEntry> entry = tab->mApplications->GetAppEntry(position);
        IPackageItemInfo::Probe(entry->mInfo)->GetPackageName(&mCurrentPkgName);
        StartApplicationDetailsActivity();
    }
    return NOERROR;
}

ECode ManageApplications::UpdateCurrentTab(
    /* [in] */ Int32 position)
{
    AutoPtr<IInterface> obj;
    mTabs->Get(position, (IInterface**)&obj);
    AutoPtr<TabInfo> tab = (TabInfo*) IManageApplicationsTabInfo::Probe(obj);
    mCurTab = tab;

    // Put things in the correct paused/resumed state.
    if (mActivityResumed) {
        mCurTab->Build(mInflater, mContentContainer, mRootView);
        mCurTab->Resume(mSortOrder);
    }
    else {
        mCurTab->Pause();
    }
    Int32 size;
    mTabs->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mTabs->Get(i, (IInterface**)&obj);
        TabInfo* t = (TabInfo*) IManageApplicationsTabInfo::Probe(obj);
        if (t != mCurTab.Get()) {
            t->Pause();
        }
    }

    mCurTab->UpdateStorageUsage();
    UpdateOptionsMenu();
    AutoPtr<IActivity> host;
    GetActivity((IActivity**)&host);
    if (host != NULL) {
        host->InvalidateOptionsMenu();
    }
    return NOERROR;
}

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos