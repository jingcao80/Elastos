
#include "elastos/droid/settings/applications/CRunningProcessesView.h"
#include "elastos/droid/settings/CSettingsActivity.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/text/format/DateUtils.h"
#include "elastos/droid/text/format/Formatter.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::App::CActivityManagerMemoryInfo;
using Elastos::Droid::App::IActivityManagerMemoryInfo;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Internal::Utility::CMemInfoReader;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Text::IBidiFormatter;
using Elastos::Droid::Text::IBidiFormatterHelper;
using Elastos::Droid::Text::CBidiFormatterHelper;
using Elastos::Droid::Text::Format::DateUtils;
using Elastos::Droid::Text::Format::Formatter;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::EIID_IRecyclerListener;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ICollection;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

//===============================================================================
//                  CRunningProcessesView::ActiveItem
//===============================================================================

CRunningProcessesView::ActiveItem::ActiveItem()
    : mFirstRunTime(0)
    , mSetBackground(FALSE)
{}

CRunningProcessesView::ActiveItem::~ActiveItem()
{}

void CRunningProcessesView::ActiveItem::UpdateTime(
    /* [in] */ IContext* context,
    /* [in] */ const String& builder)
{
    AutoPtr<ITextView> uptimeView;

    AutoPtr<RunningState::BaseItem> _mItem = (RunningState::BaseItem*)mItem.Get();

    if (IRunningStateServiceItem::Probe(mItem) != NULL) {
        // If we are displaying a service, then the service
        // uptime goes at the top.
        uptimeView = mHolder->mSize;

    }
    else {
        String size = !_mItem->mSizeStr.IsNull() ? _mItem->mSizeStr : String("");
        if (!size.Equals(_mItem->mCurSizeStr)) {
            _mItem->mCurSizeStr = size;
            mHolder->mSize->SetText(CoreUtils::Convert(size));
        }

        if (_mItem->mBackground) {
            // This is a background process; no uptime.
            if (!mSetBackground) {
                mSetBackground = TRUE;
                mHolder->mUptime->SetText(CoreUtils::Convert(""));
            }
        }
        else if (IRunningStateMergedItem::Probe(mItem) != NULL) {
            // This item represents both services and processes,
            // so show the service uptime below.
            uptimeView = mHolder->mUptime;
        }
    }

    if (uptimeView != NULL) {
        mSetBackground = FALSE;
        if (mFirstRunTime >= 0) {
            //Logger::I("foo", "Time for " + mItem.mDisplayLabel
            //        + ": " + (SystemClock->UptimeMillis()-mFirstRunTime));
            uptimeView->SetText(CoreUtils::Convert(DateUtils::FormatElapsedTime(builder,
                    (SystemClock::GetElapsedRealtime()-mFirstRunTime)/1000)));
        }
        else {
            Boolean isService = FALSE;
            if (IRunningStateMergedItem::Probe(mItem) != NULL) {
                Int32 size;
                ((RunningState::MergedItem*)mItem.Get())->mServices->GetSize(&size);
                isService = size > 0;
            }
            if (isService) {
                AutoPtr<IResources> resources;
                context->GetResources((IResources**)&resources);
                AutoPtr<ICharSequence> cs;
                resources->GetText(
                        R::string::service_restarting, (ICharSequence**)&cs);
                uptimeView->SetText(cs);
            }
            else {
                uptimeView->SetText(CoreUtils::Convert(""));
            }
        }
    }
}

//===============================================================================
//                  CRunningProcessesView::ViewHolder
//===============================================================================

CRunningProcessesView::ViewHolder::ViewHolder(
    /* [in] */ IView* v)
{
    mRootView = v;
    AutoPtr<IView> tmp;
    v->FindViewById(R::id::icon, (IView**)&tmp);
    mIcon = IImageView::Probe(tmp);
    tmp = NULL;
    v->FindViewById(R::id::name, (IView**)&tmp);
    mName = ITextView::Probe(tmp);
    tmp = NULL;
    v->FindViewById(R::id::description, (IView**)&tmp);
    mDescription = ITextView::Probe(tmp);
    tmp = NULL;
    v->FindViewById(R::id::size, (IView**)&tmp);
    mSize = ITextView::Probe(tmp);
    tmp = NULL;
    v->FindViewById(R::id::uptime, (IView**)&tmp);
    mUptime = ITextView::Probe(tmp);
    v->SetTag((IObject*)this);
}

AutoPtr<CRunningProcessesView::ActiveItem> CRunningProcessesView::ViewHolder::Bind(
    /* [in] */ RunningState* state,
    /* [in] */ RunningState::BaseItem* item,
    /* [in] */ const String& builder)
{
    {
        AutoLock syncLock(state->mLock);
        AutoPtr<IContext> context;
        mRootView->GetContext((IContext**)&context);
        AutoPtr<IPackageManager> pm;
        context->GetPackageManager((IPackageManager**)&pm);
        if (item->mPackageInfo == NULL && IRunningStateMergedItem::Probe(item) != NULL) {
            // Items for background processes don't normally load
            // their labels for performance reasons.  Do it now.
            RunningState::MergedItem* mergedItem = (RunningState::MergedItem*)item;
            if (mergedItem->mProcess != NULL) {
                mergedItem->mProcess->EnsureLabel(pm);
                item->mPackageInfo = mergedItem->mProcess->mPackageInfo;
                item->mDisplayLabel = mergedItem->mProcess->mDisplayLabel;
            }
        }
        mName->SetText(item->mDisplayLabel);
        AutoPtr<ActiveItem> ai = new ActiveItem();
        ai->mRootView = mRootView;
        ai->mItem = item;
        ai->mHolder = this;
        ai->mFirstRunTime = item->mActiveSince;
        if (item->mBackground) {
            AutoPtr<ICharSequence> cs;
            context->GetText(R::string::cached, (ICharSequence**)&cs);
            mDescription->SetText(cs);
        }
        else {
            mDescription->SetText(CoreUtils::Convert(item->mDescription));
        }
        item->mCurSizeStr = String(NULL);
        AutoPtr<IDrawable> drawable;
        item->LoadIcon(context, state, (IDrawable**)&drawable);
        mIcon->SetImageDrawable(drawable);
        IView::Probe(mIcon)->SetVisibility(IView::VISIBLE);
        ai->UpdateTime(context, builder);
        return ai;
    }
}

//===============================================================================
//                  CRunningProcessesView::TimeTicker
//===============================================================================

CRunningProcessesView::TimeTicker::TimeTicker(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    TextView::constructor(context, attrs);
}

//===============================================================================
//                  CRunningProcessesView::ServiceListAdapter
//===============================================================================

CRunningProcessesView::ServiceListAdapter::ServiceListAdapter(
    /* [in] */ RunningState* state,
    /* [in] */ CRunningProcessesView* host)
    : mShowBackground(FALSE)
    , mHost(host)
{
    CArrayList::New((IArrayList**)&mItems);

    mState = state;
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    AutoPtr<IInterface> obj;
    context->GetSystemService(
            IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);
    RefreshItems();
}

void CRunningProcessesView::ServiceListAdapter::SetShowBackground(
    /* [in] */ Boolean showBackground)
{
    if (mShowBackground != showBackground) {
        mShowBackground = showBackground;
        mState->SetWatchingBackgroundItems(showBackground);
        RefreshItems();
        mHost->RefreshUi(TRUE);
    }
}

Boolean CRunningProcessesView::ServiceListAdapter::GetShowBackground()
{
    return mShowBackground;
}

void CRunningProcessesView::ServiceListAdapter::RefreshItems()
{
    AutoPtr<IArrayList> newItems;//ArrayList<RunningState.MergedItem>
    if (mShowBackground) {
        newItems = mState->GetCurrentBackgroundItems();
    }
    else {
        newItems = mState->GetCurrentMergedItems();
    }

    if (mOrigItems != newItems) {
        mOrigItems = newItems;
        if (newItems == NULL) {
            mItems->Clear();
        }
        else {
            mItems->Clear();
            mItems->AddAll(ICollection::Probe(newItems));
            if (mShowBackground) {
                AutoPtr<ICollections> coll;
                CCollections::AcquireSingleton((ICollections**)&coll);
                coll->Sort(IList::Probe(mItems), mState->mBackgroundComparator);
            }
        }
    }
}

ECode CRunningProcessesView::ServiceListAdapter::HasStableIds(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has)
    *has = TRUE;
    return NOERROR;
}

ECode CRunningProcessesView::ServiceListAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    return mItems->GetSize(count);
}

ECode CRunningProcessesView::ServiceListAdapter::IsEmpty(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Int32 size;
    *res = mState->HasData() && (mItems->GetSize(&size), size) == 0;
    return NOERROR;
}

ECode CRunningProcessesView::ServiceListAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    return mItems->Get(position, result);
}

ECode CRunningProcessesView::ServiceListAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> obj;
    mItems->Get(position, (IInterface**)&obj);
    Int32 data;
    IObject::Probe(obj)->GetHashCode(&data);
    *result = data;
    return NOERROR;
}

ECode CRunningProcessesView::ServiceListAdapter::AreAllItemsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled)
    *enabled = FALSE;
    return NOERROR;
}

ECode CRunningProcessesView::ServiceListAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled)
    AutoPtr<IInterface> obj;
    mItems->Get(position, (IInterface**)&obj);
    RunningState::MergedItem* item = (RunningState::MergedItem*)IRunningStateMergedItem::Probe(obj);
    *enabled = !item->mIsProcess;
    return NOERROR;
}

ECode CRunningProcessesView::ServiceListAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)

    AutoPtr<IView> v;
    if (convertView == NULL) {
        NewView(parent, (IView**)&v);
    }
    else {
        v = convertView;
    }
    BindView(v, position);
    *view = v;
    REFCOUNT_ADD(*view)
    return NOERROR;
}

ECode CRunningProcessesView::ServiceListAdapter::NewView(
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)

    AutoPtr<IView> v;
    mInflater->Inflate(R::layout::running_processes_item, parent, FALSE, (IView**)&v);
    AutoPtr<ViewHolder> holder = new ViewHolder(v);

    *view = v;
    REFCOUNT_ADD(*view)
    return NOERROR;
}

ECode CRunningProcessesView::ServiceListAdapter::BindView(
    /* [in] */ IView* view,
    /* [in] */ Int32 position)
{
    {
        AutoLock syncLock(mState->mLock);
        Int32 size;
        mItems->GetSize(&size);
        if (position >= size) {
            // List must have changed since we last reported its
            // size...  ignore here, we will be doing a data changed
            // to refresh the entire list.
            return NOERROR;
        }
        AutoPtr<IInterface> obj;
        view->GetTag((IInterface**)&obj);
        AutoPtr<ViewHolder> vh = (ViewHolder*)IObject::Probe(obj);
        obj = NULL;
        mItems->Get(position, (IInterface**)&obj);
        AutoPtr<RunningState::MergedItem> item = (RunningState::MergedItem*)IRunningStateMergedItem::Probe(obj);
        AutoPtr<ActiveItem> ai = vh->Bind(mState, item, mHost->mBuilder->ToString());
        mHost->mActiveItems->Put(view, (IObject*)ai);
    }
    return NOERROR;
}

//===============================================================================
//                  CRunningProcessesView
//===============================================================================

CAR_INTERFACE_IMPL_3(CRunningProcessesView, FrameLayout, IAdapterViewOnItemClickListener, IRecyclerListener, IRunningStateOnRefreshUiListener)

CAR_OBJECT_IMPL(CRunningProcessesView)

CRunningProcessesView::CRunningProcessesView()
    : mMyUserId(0)
    , SECONDARY_SERVER_MEM(0)
    , mCurTotalRam(-1)
    , mCurHighRam(-1)
    , mCurMedRam(-1)
    , mCurLowRam(-1)
    , mCurShowCached(FALSE)
{
    CHashMap::New((IHashMap**)&mActiveItems);
    mBuilder = new StringBuilder(128);
    CMemInfoReader::New((IMemInfoReader**)&mMemInfoReader);
}

ECode CRunningProcessesView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FrameLayout::constructor(context, attrs);
    mMyUserId = UserHandle::GetMyUserId();
    return NOERROR;
}

void CRunningProcessesView::RefreshUi(
    /* [in] */ Boolean dataChanged)
{
    if (dataChanged) {
        ServiceListAdapter* adapter = mAdapter;
        adapter->RefreshItems();
        adapter->NotifyDataSetChanged();
    }

    if (mDataAvail != NULL) {
        mDataAvail->Run();
        mDataAvail = NULL;
    }

    mMemInfoReader->ReadMemInfo();

    /*
    // This is the amount of available memory until we start killing
    // background services.
    Int64 availMem = mMemInfoReader->GetFreeSize() + mMemInfoReader->GetCachedSize()
            - SECONDARY_SERVER_MEM;
    if (availMem < 0) {
        availMem = 0;
    }
    */

    {
        AutoLock syncLock(mState->mLock);
        if (mCurShowCached != mAdapter->mShowBackground) {
            mCurShowCached = mAdapter->mShowBackground;
            AutoPtr<IResources> resources;
            GetResources((IResources**)&resources);
            AutoPtr<ICharSequence> cs;
            if (mCurShowCached) {
                resources->GetText(
                        R::string::running_processes_header_used_prefix, (ICharSequence**)&cs);
                mForegroundProcessPrefix->SetText(cs);
                cs = NULL;
                resources->GetText(
                        R::string::running_processes_header_cached_prefix, (ICharSequence**)&cs);
                mAppsProcessPrefix->SetText(cs);
            }
            else {
                resources->GetText(
                        R::string::running_processes_header_system_prefix, (ICharSequence**)&cs);
                mForegroundProcessPrefix->SetText(cs);
                cs = NULL;
                resources->GetText(
                        R::string::running_processes_header_apps_prefix, (ICharSequence**)&cs);
                mAppsProcessPrefix->SetText(cs);
            }
        }

        Int64 totalRam;
        mMemInfoReader->GetTotalSize(&totalRam);
        Int64 medRam;
        Int64 lowRam;
        Int64 freeSize, cachedSize;
        mMemInfoReader->GetFreeSize(&freeSize);
        mMemInfoReader->GetCachedSize(&cachedSize);

        if (mCurShowCached) {
            lowRam = freeSize + cachedSize;
            medRam = mState->mBackgroundProcessMemory;
        }
        else {
            lowRam = freeSize + cachedSize
                    + mState->mBackgroundProcessMemory;
            medRam = mState->mServiceProcessMemory;

        }
        Int64 highRam = totalRam - medRam - lowRam;

        if (mCurTotalRam != totalRam || mCurHighRam != highRam || mCurMedRam != medRam
                || mCurLowRam != lowRam) {
            mCurTotalRam = totalRam;
            mCurHighRam = highRam;
            mCurMedRam = medRam;
            mCurLowRam = lowRam;
            AutoPtr<IBidiFormatterHelper> helper;
            CBidiFormatterHelper::AcquireSingleton((IBidiFormatterHelper**)&helper);
            AutoPtr<IBidiFormatter> bidiFormatter;
            helper->GetInstance((IBidiFormatter**)&bidiFormatter);
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            String sizeStr;
            bidiFormatter->UnicodeWrap(
                    Formatter::FormatShortFileSize(context, lowRam), &sizeStr);

            AutoPtr<IResources> resources;
            GetResources((IResources**)&resources);

            AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, CoreUtils::Convert(sizeStr));
            String tmp;
            resources->GetString(
                    R::string::running_processes_header_ram, args, &tmp);
            mBackgroundProcessText->SetText(CoreUtils::Convert(tmp));
            bidiFormatter->UnicodeWrap(
                    Formatter::FormatShortFileSize(context, medRam), &sizeStr);
            resources->GetString(
                    R::string::running_processes_header_ram, args, &tmp);
            mAppsProcessText->SetText(CoreUtils::Convert(tmp));
            bidiFormatter->UnicodeWrap(
                    Formatter::FormatShortFileSize(context, highRam), &sizeStr);
            resources->GetString(
                    R::string::running_processes_header_ram, args, &tmp);
            mForegroundProcessText->SetText(CoreUtils::Convert(tmp));
            mColorBar->SetRatios(highRam/(Float)totalRam,
                    medRam/(Float)totalRam,
                    lowRam/(Float)totalRam);
        }
    }
}

ECode CRunningProcessesView::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IListView> l = IListView::Probe(parent);
    AutoPtr<IAdapter> adapter;
    parent->GetAdapter((IAdapter**)&adapter);
    AutoPtr<IInterface> obj;
    adapter->GetItem(position, (IInterface**)&obj);
    AutoPtr<RunningState::MergedItem> mi = (RunningState::MergedItem*)IRunningStateMergedItem::Probe(obj);
    mCurSelected = mi;
    StartServiceDetailsActivity(mi);
    return NOERROR;
}

void CRunningProcessesView::StartServiceDetailsActivity(
    /* [in] */ RunningState::MergedItem* mi)
{
    if (mOwner != NULL) {
        // start new fragment to display extended information
        AutoPtr<IBundle> args;
        CBundle::New((IBundle**)&args);
        if (mi->mProcess != NULL) {
            args->PutInt32(String("uid")/*RunningServiceDetails::KEY_UID*/, mi->mProcess->mUid);
            args->PutString(String("process")/*RunningServiceDetails::KEY_PROCESS*/, mi->mProcess->mProcessName);
        }
        args->PutInt32(String("user_id")/*RunningServiceDetails::KEY_USER_ID*/, mi->mUserId);
        args->PutBoolean(String("background")/*RunningServiceDetails::KEY_BACKGROUND*/, mAdapter->mShowBackground);

        AutoPtr<IActivity> activity;
        mOwner->GetActivity((IActivity**)&activity);
        CSettingsActivity* sa = (CSettingsActivity*)ISettingsActivity::Probe(activity);
        sa->StartPreferencePanel(String("Elastos.Droid.Settings.Applications.CRunningServiceDetails"), args,
                R::string::runningservicedetails_settings_title, NULL, NULL, 0);
    }
}

ECode CRunningProcessesView::OnMovedToScrapHeap(
    /* [in] */ IView* view)
{
    return mActiveItems->Remove(view);
}

ECode CRunningProcessesView::DoCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&obj);
    mAm = IActivityManager::Probe(obj);
    mState = RunningState::GetInstance(context);
    obj = NULL;
    context->GetSystemService(
            IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(obj);
    AutoPtr<IView> viewInflater;
    inflater->Inflate(R::layout::running_processes_view, this, (IView**)&viewInflater);
    AutoPtr<IView> tmp;
    FindViewById(Elastos::Droid::R::id::list, (IView**)&tmp);
    mListView = IListView::Probe(tmp);
    AutoPtr<IView> emptyView;
    FindViewById(Elastos::Droid::R::id::empty, (IView**)&emptyView);
    IAdapterView* _listView  = IAdapterView::Probe(mListView);
    if (emptyView != NULL) {
        _listView->SetEmptyView(emptyView);
    }
    _listView->SetOnItemClickListener(this);
    IAbsListView::Probe(mListView)->SetRecyclerListener(this);
    mAdapter = new ServiceListAdapter(mState, this);
    _listView->SetAdapter(mAdapter);
    mHeader = NULL;
    inflater->Inflate(R::layout::running_processes_header, NULL, (IView**)&mHeader);
    mListView->AddHeaderView(mHeader, NULL, FALSE /* set as not selectable */);
    tmp = NULL;
    mHeader->FindViewById(R::id::color_bar, (IView**)&tmp);
    mColorBar = (CLinearColorBar*)tmp.Get();
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    Int32 color1, color2, color3;
    res->GetColor(R::color::running_processes_system_ram, &color1);
    res->GetColor(R::color::running_processes_apps_ram, &color2);
    res->GetColor(R::color::running_processes_free_ram, &color3);
    mColorBar->SetColors(color1, color2, color3);
    tmp = NULL;
    mHeader->FindViewById(R::id::freeSizePrefix, (IView**)&tmp);
    mBackgroundProcessPrefix = ITextView::Probe(tmp);
    tmp = NULL;
    mHeader->FindViewById(R::id::appsSizePrefix, (IView**)&tmp);
    mAppsProcessPrefix = ITextView::Probe(tmp);
    tmp = NULL;
    mHeader->FindViewById(R::id::systemSizePrefix, (IView**)&tmp);
    mForegroundProcessPrefix = ITextView::Probe(tmp);
    tmp = NULL;
    mHeader->FindViewById(R::id::freeSize, (IView**)&tmp);
    mBackgroundProcessText = ITextView::Probe(tmp);
    tmp = NULL;
    mHeader->FindViewById(R::id::appsSize, (IView**)&tmp);
    mAppsProcessText = ITextView::Probe(tmp);
    tmp = NULL;
    mHeader->FindViewById(R::id::systemSize, (IView**)&tmp);
    mForegroundProcessText = ITextView::Probe(tmp);

    AutoPtr<IActivityManagerMemoryInfo> memInfo;
    CActivityManagerMemoryInfo::New((IActivityManagerMemoryInfo**)&memInfo);
    memInfo = NULL;
    mAm->GetMemoryInfo((IActivityManagerMemoryInfo**)&memInfo);
    memInfo->GetSecondaryServerThreshold(&SECONDARY_SERVER_MEM);
    return NOERROR;
}

ECode CRunningProcessesView::DoPause()
{
    mState->Pause();
    mDataAvail = NULL;
    mOwner = NULL;
    return NOERROR;
}

Boolean CRunningProcessesView::DoResume(
    /* [in] */ IFragment* owner,
    /* [in] */ IRunnable* dataAvail)
{
    mOwner = owner;
    mState->Resume(this);
    if (mState->HasData()) {
        // If the state already has its data, then let's populate our
        // list right now to avoid flicker.
        RefreshUi(TRUE);
        return TRUE;
    }
    mDataAvail = dataAvail;
    return FALSE;
}

void CRunningProcessesView::UpdateTimes()
{
    AutoPtr<ICollection> values;
    mActiveItems->GetValues((ICollection**)&values);
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        ActiveItem* ai = (ActiveItem*)IObject::Probe(obj);
        AutoPtr<IBinder> binder;
        ai->mRootView->GetWindowToken((IBinder**)&binder);
        if (binder == NULL) {
            // Clean out any dead views, just in case.
            it->Remove();
            continue;
        }
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        ai->UpdateTime(context, mBuilder->ToString());
    }
}

ECode CRunningProcessesView::OnRefreshUi(
    /* [in] */ Int32 what)
{
    switch (what) {
        case REFRESH_TIME:
            UpdateTimes();
            break;
        case REFRESH_DATA:
            RefreshUi(FALSE);
            UpdateTimes();
            break;
        case REFRESH_STRUCTURE:
            RefreshUi(TRUE);
            UpdateTimes();
            break;
    }
    return NOERROR;
}

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos