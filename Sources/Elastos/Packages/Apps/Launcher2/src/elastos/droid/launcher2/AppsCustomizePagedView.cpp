
#include "elastos/droid/launcher2/PackageChangedReceiver.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(AsyncTaskPageData, Object, IAppsCustomizePagedViewAsyncTaskPageData);

AsyncTaskPageData::AsyncTaskPageData(
    /* [in] */ Int32 p,
    /* [in] */ IArrayList* l,
    /* [in] */ Int32 cw,
    /* [in] */ Int32 ch,
    /* [in] */ IAppsCustomizePagedViewAsyncTaskCallback* bgR,
    /* [in] */ IAppsCustomizePagedViewAsyncTaskCallback* postR,
    /* [in] */ IWidgetPreviewLoader* w)
    : mPage(p)
    , mItems(l)
    , mMaxImageWidth(cw)
    , mMaxImageHeight(ch)
    , mDoInBackgroundCallback(bgR)
    , mPostExecuteCallback(postR)
    , mWidgetPreviewLoader(w)
{
    CArrayList::New((IArrayList**)&mGeneratedImages);
}

ECode AsyncTaskPageData::Cleanup(
    /* [in] */ Boolean cancelled)
{
    // Clean up any references to source/generated bitmaps
    if (mGeneratedImages != NULL) {
        if (cancelled) {
            Int32 size;
            generatedImages->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj1;
                mItems->Get(i, (IInterface**)&obj1);
                AutoPtr<IInterface> obj2;
                mGeneratedImages->Get(i, (IInterface**)&obj2);
                AutoPtr<IBitmap> map = IBitmap::Probe(obj2);
                mWidgetPreviewLoader->RecycleBitmap(obj1, obj2);
            }
        }
        mGeneratedImages->Clear();
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(AppsCustomizeAsyncTask, AsyncTask,
        IAppsCustomizePagedViewAppsCustomizeAsyncTask);

AppsCustomizeAsyncTask::AppsCustomizeAsyncTask(
    /* [in] */ Int32 p,
    /* [in] */ AsyncTaskPageDataType ty)
    : mPage(p)
    , mThreadPriority(IProcess::THREAD_PRIORITY_DEFAULT);
    , mDataType(ty)
{
}

ECode AppsCustomizeAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    if (params->GetLength() != 1) {
        *obj = NULL;
        return NOERROR;
    }
    // Load each of the widget previews in the background
    AsyncTaskPageData* date =
            (AsyncTaskPageData**)IAppsCustomizePagedViewAsyncTaskPageData::Probe((*params)[0]);
    date->mDoInBackgroundCallback->Run(IAppsCustomizePagedViewAppsCustomizeAsyncTask::Probe(this),
            IAppsCustomizePagedViewAsyncTaskPageData::Probe((*params)[0]));
    *obj = (*params)[0];
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode AppsCustomizeAsyncTask::OnPostExecute(
    /* [in] */ IInterface* result)
{
    // All the widget previews are loaded, so we can just callback to inflate the page
    AsyncTaskPageData* date =
            (AsyncTaskPageData**)IAppsCustomizePagedViewAsyncTaskPageData::Probe(result);
    return date->mPostExecuteCallback->Run(IAppsCustomizePagedViewAppsCustomizeAsyncTask::Probe(this),
            IAppsCustomizePagedViewAsyncTaskPageData::Probe(result));
}

ECode AppsCustomizeAsyncTask::SetThreadPriority(
    /* [in] */ Int32 p)
{
    mThreadPriority = p;
    return NOERROR;
}

ECode AppsCustomizeAsyncTask::SyncThreadPriority()
{
    return Process::SetThreadPriority(mThreadPriority);
}

AppsCustomizePagedView::MyRunnable::MyRunnable(
    /* [in] */ AppsCustomizePagedView* host)
    : mHost(host)
{
}

ECode AppsCustomizePagedView::MyRunnable::Run()
{
    return mHost->ShowAllAppsCling();
}

AppsCustomizePagedView::MyRunnable2::MyRunnable(
    /* [in] */ AppsCustomizePagedView* host,
    /* [in] */ PendingAddWidgetInfo* info)
    : mHost(host)
    , mInfo(info)
{
}

ECode AppsCustomizePagedView::MyRunnable2::Run()
{
    mHost->mWidgetLoadingId;
    AutoPtr<ILauncherAppWidgetHost> launcherAppWidgetHost;
    mHost->mLauncher->GetAppWidgetHost((ILauncherAppWidgetHost**)&launcherAppWidgetHost);
    launcherAppWidgetHost->AllocateAppWidgetId();

    AutoPtr<IAppWidgetManager> manager =  AppWidgetManager::GetInstance(mLauncher);
    AutoPtr<IUserHandle> handle;
    mInfo->mInfo->GetProfile((IUserHandle**)&handle);
    Boolean res;
    manager->BindAppWidgetIdIfAllowed(
            mHost->mWidgetLoadingId, handle,
            mInfo->mComponentName, options, &res);
    if (res) {
        mHost->mWidgetCleanupState = WIDGET_BOUND;
    }
    return NOERROR;
}

AppsCustomizePagedView::MyRunnable3::MyRunnable(
    /* [in] */ AppsCustomizePagedView* host,
    /* [in] */ PendingAddWidgetInfo* info)
    : mHost(host)
    , mInfo(info)
{
}

ECode AppsCustomizePagedView::MyRunnable3::Run()
{
    if (mHost->mWidgetCleanupState != WIDGET_BOUND) {
        return NOERROR;
    }
    AutoPtr<ILauncherAppWidgetHost> launcherAppWidgetHost;

    mHost->mLauncher->GetAppWidgetHost((ILauncherAppWidgetHost**)&launcherAppWidgetHost);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IAppWidgetHostView> hostView;
    launcherAppWidgetHost->CreateView(context,
            mHost->mWidgetLoadingId, pInfo, (IAppWidgetHostView**)&hostView);
    mInfo->mBoundWidget = hostView;
    mHost->mWidgetCleanupState = WIDGET_INFLATED;
    hostView->SetVisibility(INVISIBLE);

    AutoPtr<IWorkspace> workspace;
    mHost->mLauncher->GetWorkspace((IWorkspace**)&workspace);
    AutoPtr<ArrayOf<Int32> > unScaledSize;
    workspace->EstimateItemSize(mInfo->mSpanX,
            mInfo->mSpanY, IPendingAddWidgetInfo::Probe(mInfo), FALSE,
            (ArrayOf<Int32>**)&unScaledSize);

    // We want the first widget layout to be the correct size. This will be important
    // for width size reporting to the AppWidgetManager.
    AutoPtr<IDragLayerLayoutParams> lp = new DragLayer::LayoutParams((*unScaledSize)[0],
            (*unScaledSize)[1]);
    lp->SetX(0);
    lp->SetY(0);
    lp->SetCustomPosition(TRUE);
    hostView->SetLayoutParams(lp);
    AutoPtr<IDragLayer> dragLayer;
    mHost->mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
    return dragLayer->AddView(hostView);
}

AppsCustomizePagedView::MyRunnable4::MyRunnable(
    /* [in] */ AppsCustomizePagedView* host)
    : mHost(host)
{
}

ECode AppsCustomizePagedView::MyRunnable4::Run()
{
    // We don't enter spring-loaded mode if the drag has been cancelled
    AutoPtr<IDragController> dragController;
    mHost->mLauncher->GetDragController((IDragController**)&dragController);
    Boolean res;
    dragController->IsDragging(&res);
    if (res) {
        // Dismiss the cling
        mHost->mLauncher->DismissAllAppsCling(NULL);

        // Reset the alpha on the dragged icon before we drag
        mHost->ResetDrawableState();

        // Go into spring loaded mode (must happen before we startDrag())
        mHost->mLauncher->EnterSpringLoadedDragMode();
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(AppsCustomizePagedView::MyAsyncTaskCallback, Object,
        IAppsCustomizePagedViewAsyncTaskCallback);

AppsCustomizePagedView::MyAsyncTaskCallback::MyAsyncTaskCallback(
    /* [in] */ AppsCustomizePagedView* host,
    /* [in] */ Int32 sleepMs)
    : mHost(host)
    , mSleepMs(sleepMs)
{
}

ECode AppsCustomizePagedView::MyAsyncTaskCallback::Run(
    /* [in] */ IAppsCustomizePagedViewAppsCustomizeAsyncTask* task,
    /* [in] */ IAppsCustomizePagedViewAsyncTaskPageData* data)
{
    //try {
    //try {
    Thread::Sleep(mSleepMs);
    //} catch (Exception e) {}
    mHost->loadWidgetPreviewsInBackground(task, data);
    //} finally {
    Boolean res;
    task->IsCancelled(&res);
    if (res) {
        data->Cleanup(TRUE);
    }
    //}
    return NOERROR;
}

CAR_INTERFACE_IMPL(AppsCustomizePagedView::MyAsyncTaskCallback2, Object,
        IAppsCustomizePagedViewAsyncTaskCallback);

AppsCustomizePagedView::MyAsyncTaskCallback2::MyAsyncTaskCallback2(
    /* [in] */ AppsCustomizePagedView* host)
    : mHost(host)
{
}

ECode AppsCustomizePagedView::MyAsyncTaskCallback2::Run(
    /* [in] */ IAppsCustomizePagedViewAppsCustomizeAsyncTask* task,
    /* [in] */ IAppsCustomizePagedViewAsyncTaskPageData* data)
{
    mHost->mRunningTasks->Remove(task);
    Boolean res;
    task->IsCancelled(&res);
    if (res) return NOERROR;
    // do cleanup inside onSyncWidgetPageItems
    return mHost->OnSyncWidgetPageItems(data);
}

AppsCustomizePagedView::MyRunnable5::MyRunnable5(
    /* [in] */ AppsCustomizePagedView* host,
    /* [in] */ Int32 cellWidth,
    /* [in] */ Int32 cellHeight,
    /* [in] */ IPagedViewGridLayout* layout,
    /* [in] */ Boolean immediate)
    : mHost(host)
    , mCellWidth(cellWidth)
    , mCellHeight(cellHeight)
    , mLayout(layout)
    . mImmediate(immediate)
{
}

ECode AppsCustomizePagedView::MyRunnable5::Run()
{
    // Load the widget previews
    Int32 maxPreviewWidth = mCellWidth;
    Int32 maxPreviewHeight = mCellHeight;
    Int32 count;
    mHost->mLayout->GetChildCount(&count);
    if (count > 0) {
        AutoPtr<IView> view;
        layout->GetChildAt(0, (IView**)&view);
        AutoPtr<IPagedViewWidget> w = IPagedViewWidget::Probe(view);
        AutoPtr<ArrayOf<Int32> > maxSize;
        w->GetPreviewSize((ArrayOf<Int32>**)&maxSize);
        maxPreviewWidth = (*maxSize)[0];
        maxPreviewHeight = (*maxSize)[1];
    }

    mHost->mWidgetPreviewLoader->SetPreviewSize(
            maxPreviewWidth, maxPreviewHeight, mHost->mWidgetSpacingLayout);
    if (mImmediate) {
        AutoPtr<IAppsCustomizePagedViewAsyncTaskPageData> data =
                new AsyncTaskPageData(page, items, maxPreviewWidth,
                maxPreviewHeight, NULL, NULL, mHost->mWidgetPreviewLoader);
        mHost->LoadWidgetPreviewsInBackground(NULL, data);
        mHost->OnSyncWidgetPageItems(data);
    }
    else {
        if (mInTransition) {
            mHost->mDeferredPrepareLoadWidgetPreviewsTasks->Add(this);
        }
        else {
            mHost->PrepareLoadWidgetPreviewsTask(page, items,
                    maxPreviewWidth, maxPreviewHeight, mHost->mWidgetCountX);
        }
    }
    return layout->SetOnLayoutListener(NULL);
}

const String AppsCustomizePagedView::TAG("AppsCustomizePagedView");

Float AppsCustomizePagedView::CAMERA_DISTANCE = 6500;
Float AppsCustomizePagedView::TRANSITION_SCALE_FACTOR = 0.74f;
Float AppsCustomizePagedView::TRANSITION_PIVOT = 0.65f;
Float AppsCustomizePagedView::TRANSITION_MAX_ROTATION = 22;
Boolean AppsCustomizePagedView::PERFORM_OVERSCROLL_ROTATION = TRUE;

const Int32 AppsCustomizePagedView::sPageSleepDelay = 200;

const Int32 AppsCustomizePagedView::WIDGET_NO_CLEANUP_REQUIRED = -1;
const Int32 AppsCustomizePagedView::WIDGET_PRELOAD_PENDING = 0;
const Int32 AppsCustomizePagedView::WIDGET_BOUND = 1;
const Int32 AppsCustomizePagedView::WIDGET_INFLATED = 2;

const Int32 AppsCustomizePagedView::sLookBehindPageCount = 2;
const Int32 AppsCustomizePagedView::sLookAheadPageCount = 2;

CAR_INTERFACE_IMPL_7(AppsCustomizePagedView, PagedViewWithDraggableItems,
        IAppsCustomizePagedView, IViewOnClickListener, IViewOnKeyListener,
        IDragSource, IPagedViewIconPressedCallback,
        IPagedViewWidgetShortPressListener, ILauncherTransitionable);

AppsCustomizePagedView::AppsCustomizePagedView()
    : mSaveInstanceStateItemIndex(-1)
    , mHasShownAllAppsCling(FALSE)
    , mClingFocusedX(0)
    , mClingFocusedY(0)
    , mContentWidth(0)
    , mMaxAppCellCountX(0)
    , mMaxAppCellCountY(0)
    , mWidgetCountX(0)
    , mWidgetCountY(0)
    , mWidgetWidthGap(0)
    , mWidgetHeightGap(0)
    , mNumAppsPages(0)
    , mNumWidgetPages(0)
    , mWidgetCleanupState(WIDGET_NO_CLEANUP_REQUIRED)
    , mWidgetLoadingId(-1)
    , mDraggingWidget(FALSE)
    , mInTransition(FALSE)
    , mInBulkBind(FALSE)
    , mNeedToUpdatePageCountsAndInvalidateData(FALSE)
{
    mZInterpolator = new Workspace::ZInterpolator(0.5f);

    CAccelerateInterpolator::New(0.9f, (IAccelerateInterpolator**)&mAlphaInterpolator);
    CDecelerateInterpolator::New(4, (IDecelerateInterpolator**)&mLeftScreenAlphaInterpolator);

    CArrayList::New((IArrayList**)&mDeferredSyncWidgetPageItems);
    CArrayList::New((IArrayList**)&mDeferredPrepareLoadWidgetPreviewsTasks);

    CRect::New((IRect**)&mTmpRect);

    CBitmapCache::New((IBitmapCache**)&mCachedShortcutPreviewBitmap);
    CPaintCache::New((IPaintCache**)&mCachedShortcutPreviewPaint);
    CCanvasCache::New((ICanvasCache**)&mCachedShortcutPreviewCanvas);

    CCanvasCache::New((ICanvasCache**)&mCachedAppWidgetPreviewCanvas);
    CRectCache::New((IRectCache**)&mCachedAppWidgetPreviewSrcRect);
    CRectCache::New((IRectCache**)&mCachedAppWidgetPreviewDestRect);
    CPaintCache::New((IPaintCache**)&mCachedAppWidgetPreviewPaint);
}

ECode AppsCustomizePagedView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    PagedViewWithDraggableItems::constructor(context, attrs);

    LayoutInflater::From(context, (ILayoutInflater**)&mLayoutInflater);
    context->GetPackageManager((IPackageManager**)&mPackageManager);
    CArrayList::New((IArrayList**)&mApps);
    CArrayList::New((IArrayList**)&mWidgets);

    AutoPtr<IContext> ctx;
    GetApplicationContext((IContext**)&ctx);
    ILauncherApplication::Probe(ctx)->GetIconCache((IIconCache**)&mIconCache);
    CCanvas::New((ICanvas**)&mCanvas);
    CArrayList::New((IArrayList**)&mRunningTasks);

    // Save the default widget preview background
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs,
            Elastos::Droid::Launcher2::R::styleable::AppsCustomizePagedView,
            0, 0, (ITypedArray**)&a);
    a->GetInt32(
            Elastos::Droid::Launcher2::R::styleable::AppsCustomizePagedView_maxAppCellCountX,
            -1, &mMaxAppCellCountX);
    a->GetInt32(
            Elastos::Droid::Launcher2::R::styleable::AppsCustomizePagedView_maxAppCellCountY,
            -1, &mMaxAppCellCountY);
    a->GetDimensionPixelSize(
            Elastos::Droid::Launcher2::R::styleable::AppsCustomizePagedView_widgetCellWidthGap,
            0, &mWidgetWidthGap);
    a->GetDimensionPixelSize(
            Elastos::Droid::Launcher2::R::styleable::AppsCustomizePagedView_widgetCellHeightGap,
            0, &mWidgetHeightGap);
    a->GetInt32(
            Elastos::Droid::Launcher2::R::styleable::AppsCustomizePagedView_widgetCountX,
            2, &mWidgetCountX);
    a->GetInt32(
            Elastos::Droid::Launcher2::R::styleable::AppsCustomizePagedView_widgetCountY,
            2, &mWidgetCountY);
    a->GetInt32(
            Elastos::Droid::Launcher2::R::styleable::AppsCustomizePagedView_clingFocusedX,
            0, &mClingFocusedX);
    a->GetInt32(
            Elastos::Droid::Launcher2::R::styleable::AppsCustomizePagedView_clingFocusedY,
            0, &mClingFocusedY);
    a->Recycle();
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    CPagedViewCellLayout::New(context, (IPagedViewCellLayout**)&mWidgetSpacingLayout);

    // The padding on the non-matched dimension for the default widget preview icons
    // (top + bottom)
    mFadeInAdjacentScreens = FALSE;

    // Unless otherwise specified this view is important for accessibility.
    Int32 accessibility;
    GetImportantForAccessibility(&accessibility);
    if (accessibility == IView::IMPORTANT_FOR_ACCESSIBILITY_AUTO) {
        SetImportantForAccessibility(IView::IMPORTANT_FOR_ACCESSIBILITY_YES);
    }
    return NOERROR;
}

ECode AppsCustomizePagedView::Init()
{
    PagedViewWithDraggableItems::Init();
    mCenterPagesVertically = FALSE;

    AutoPtr<IContext> context;
    GetContext((IContext*8)&context);
    AutoPtr<IResources> r;
    context->GetResources((IResources**)&r);
    Int32 num;
    r->GetInteger(
            Elastos::Droid::Launcher2::R::integer::config_appsCustomizeDragSlopeThreshold,
            &num);
    return SetDragSlopeThreshold(num/100f);
}

Int32 AppsCustomizePagedView::GetMiddleComponentIndexOnCurrentPage()
{
    Int32 i = -1;
    Int32 count;
    GetPageCount(&count);
    if (count > 0) {
        Int32 currentPage;
        GetCurrentPage(&currentPage);
        if (currentPage < mNumAppsPages) {
            AutoPtr<IView> view;
            GetPageAt(currentPage, (IView**)&view);
            AutoPtr<IPagedViewCellLayout> layout = IPagedViewCellLayout::Probe(view);
            AutoPtr<IPagedViewCellLayoutChildren> childrenLayout;
            layout->GetChildrenLayout((IPagedViewCellLayoutChildren**)&childrenLayout);
            Int32 numItemsPerPage = mCellCountX * mCellCountY;
            Int32 childCount;
            childrenLayout->GetChildCount(&childCount);
            if (childCount > 0) {
                i = (currentPage * numItemsPerPage) + (childCount / 2);
            }
        }
        else {
            Int32 numApps;
            mApps->GetSize(&numApps);
            AutoPtr<IView> view;
            GetPageAt(currentPage, (IView**)&view);
            AutoPtr<IPagedViewGridLayout> layout = IPagedViewGridLayout::Probe(view);
            Int32 numItemsPerPage = mWidgetCountX * mWidgetCountY;
            Int32 childCount;
            layout->GetChildCount(&childCount);
            if (childCount > 0) {
                i = numApps +
                    ((currentPage - mNumAppsPages) * numItemsPerPage) + (childCount / 2);
            }
        }
    }
    return i;
}

ECode AppsCustomizePagedView::GetSaveInstanceStateIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);

    if (mSaveInstanceStateItemIndex == -1) {
        GetMiddleComponentIndexOnCurrentPage(&mSaveInstanceStateItemIndex);
    }
    *index = mSaveInstanceStateItemIndex;
    return NOERROR;
}

ECode AppsCustomizePagedView::GetPageForComponent(
    /* [in] */ Int32 index,
    /* [out] */ Int32* page)
{
    VALIDATE_NOT_NULL(page);

    if (index < 0) {
        *page = 0;
        return NOERROR;
    }

    Int32 size;
    mApps->GetSize(&size);
    if (index < size) {
        Int32 numItemsPerPage = mCellCountX * mCellCountY;
        *page = (index / numItemsPerPage);
        return NOERROR;
    }
    else {
        Int32 numItemsPerPage = mWidgetCountX * mWidgetCountY;
        *page = mNumAppsPages + ((index - size) / numItemsPerPage);
        return NOERROR;
    }
    return NOERROR;
}

ECode AppsCustomizePagedView::RestorePageForIndex(
    /* [in] */ Int32 index)
{
    if (index < 0) return NOERROR;
    mSaveInstanceStateItemIndex = index;
    return NOERROR;
}

void AppsCustomizePagedView::UpdatePageCounts()
{
    Int32 size;
    mWidgets->GetSize(&size);
    mNumWidgetPages = (Int32)Math::Ceil(size /
            (Float)(mWidgetCountX * mWidgetCountY));
    mApps->GetSize(&size);
    mNumAppsPages = (Int32)Math::Ceil((Float)size / (mCellCountX * mCellCountY));
    return;
}

ECode AppsCustomizePagedView::OnDataReady(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (mWidgetPreviewLoader == NULL) {
        CWidgetPreviewLoader::New(mLauncher, (IWidgetPreviewLoader**)&mWidgetPreviewLoader);
    }

    // Note that we transpose the counts in portrait so that we get a similar layout
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IConfiguration> configuration;
    resources->GetConfiguration((IConfiguration**)&configuration);
    Int32 orientation;
    configuration->GetOrientation(&orientation);

    Boolean isLandscape = orientation == IConfiguration::ORIENTATION_LANDSCAPE;
    Int32 maxCellCountX = Integer.MAX_VALUE;
    Int32 maxCellCountY = Integer.MAX_VALUE;

    Boolean res;
    LauncherApplication::IsScreenLarge(&res);
    if (res) {
        Int32 x;
        LauncherModel::GetCellCountX(&x);
        Int32 y;
        LauncherModel::GetCellCountY(&y);
        maxCellCountX = (isLandscape ? x : y);
        maxCellCountY = (isLandscape ? y : x);
    }
    if (mMaxAppCellCountX > -1) {
        maxCellCountX = Math::Min(maxCellCountX, mMaxAppCellCountX);
    }
    // Temp hack for now: only use the max cell count Y for widget layout
    Int32 maxWidgetCellCountY = maxCellCountY;
    if (mMaxAppCellCountY > -1) {
        maxWidgetCellCountY = Math::Min(maxWidgetCellCountY, mMaxAppCellCountY);
    }

    // Now that the data is ready, we can calculate the content width, the number of cells to
    // use for each page
    mWidgetSpacingLayout->SetGap(mPageLayoutWidthGap, mPageLayoutHeightGap);
    mWidgetSpacingLayout->SetPadding(mPageLayoutPaddingLeft, mPageLayoutPaddingTop,
            mPageLayoutPaddingRight, mPageLayoutPaddingBottom);
    mWidgetSpacingLayout->CalculateCellCount(width, height, maxCellCountX, maxCellCountY);
    mWidgetSpacingLayout->GetCellCountX(&mCellCountX);
    mWidgetSpacingLayout->GetCellCountY(&mCellCountY);
    UpdatePageCounts();

    // Force a measure to update recalculate the gaps
    Int32 width;
    GetMeasuredWidth(&width);
    Int32 widthSpec;
    MeasureSpec::MakeMeasureSpec(width, IMeasureSpec::AT_MOST, &widthSpec);
    Int32 height;
    GetMeasuredHeight(&height);
    Int32 heightSpec;
    MeasureSpec::MakeMeasureSpec(height, IMeasureSpec::AT_MOST, &heightSpec);
    mWidgetSpacingLayout->CalculateCellCount(width, height, maxCellCountX,
            maxWidgetCellCountY);
    mWidgetSpacingLayout->Measure(widthSpec, heightSpec);
    mWidgetSpacingLayout->GetContentWidth(&mContentWidth);

    AutoPtr<IAppsCustomizeTabHost> host = GetTabHost();
    Boolean hostIsTransitioning;
    host->IsTransitioning(&hostIsTransitioning);

    // Restore the page
    Int32 page;
    GetPageForComponent(mSaveInstanceStateItemIndex, &page);
    InvalidatePageData(Math::Max(0, page), hostIsTransitioning);

    // Show All Apps cling if we are finished transitioning, otherwise, we will try again when
    // the transition completes in AppsCustomizeTabHost (otherwise the wrong offsets will be
    // returned while animating)
    if (!hostIsTransitioning) {
        AutoPtr<IRunnable> run = new MyRunnable(this);
        Boolean res;
        Post(run, &res);
    }
    return NOERROR;
}

ECode AppsCustomizePagedView::ShowAllAppsCling()
{
    Boolean res;
    IsDataReady(&res);
    if (!mHasShownAllAppsCling && res) {
        mHasShownAllAppsCling = TRUE;
        // Calculate the position for the cling punch through
        AutoPtr<ArrayOf<Int32> > offset = ArrayOf<Int32>::Alloc(2);
        AutoPtr<ArrayOf<Int32> > pos;
        mWidgetSpacingLayout->EstimateCellPosition(mClingFocusedX, mClingFocusedY,
                (ArrayOf<Int32>**)&pos);
        AUtoPtr<IDragLaye> dragLayer;
        mLauncher->GetDragLayer(&dragLayer);
        dragLayer->GetLocationInDragLayer(this, offset);
        // PagedViews are centered horizontally but top aligned
        // Note we have to shift the items up now that Launcher sits under the status bar
        Int32 width;
        GetMeasuredWidth(&width);
        Int32 width2;
        mWidgetSpacingLayout->GetMeasuredWidth(&width2);
        (*pos)[0] += (width - width2) / 2 +
                (*offset)[0];
        Int32 top;
        dragLayer->GetPaddingTop(&top);
        (*pos)[1] += (*offset)[1] - top;
        return mLauncher->ShowFirstRunAllAppsCling(pos);
    }
    return NOERROR;
}

ECode AppsCustomizePagedView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 width;
    MeasureSpec->GetSize(widthMeasureSpec, &width);
    Int32 height;
    MeasureSpec->GetSize(heightMeasureSpec, &height);
    Boolean res;
    IsDataReady(&res);
    if (!res) {
        Int32 size;
        mApps->GetSize(&size);
        Boolean tmp;
        mWidgets->IsEmpty(&tmp);
        if (size > 0 && !tmp) {
            SetDataIsReady();
            SetMeasuredDimension(width, height);
            OnDataReady(width, height);
        }
    }

    return PagedViewWithDraggableItems::OnMeasure(widthMeasureSpec, heightMeasureSpec);
}

ECode AppsCustomizePagedView::OnPackagesUpdated(
    /* [in] */ IArrayList* widgetsAndShortcuts)
{
    // Get the list of widgets and shortcuts
    mWidgets->Clear();
    Int32 size;
    widgetsAndShortcuts->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> o;
        widgetsAndShortcuts->Get(i, (IInterface**)&o);

        if (IAppWidgetProviderInfo::Probe(o) != NULL) {
            AutoPtr<IAppWidgetProviderInfo> widget = IAppWidgetProviderInfo::Probe(o);
            Int32 minWidth;
            widget->GetMinWidth(&minWidth);
            Int32 minHeight;
            widget->GetMinHeight(&minHeight);
            if (minWidth > 0 && minHeight > 0) {
                // Ensure that all widgets we show can be added on a workspace of this size
                AutoPtr<ArrayOf<Int32> > spanXY;
                Launcher::GetSpanForWidget(mLauncher, widget, (ArrayOf<Int32>**)&spanXY);
                AutoPtr<ArrayOf<Int32> > minSpanXY;
                Launcher::GetMinSpanForWidget(mLauncher, widget, (ArrayOf<Int32>**)&minSpanXY);
                Int32 minSpanX = Math::Min((*spanXY)[0], (*minSpanXY)[0]);
                Int32 minSpanY = Math::Min((*spanXY)[1], (*minSpanXY)[1]);
                Int32 x;
                LauncherModel::GetCellCountX(&x);
                Int32 y;
                LauncherModel::GetCellCountY(&y);
                if (minSpanX <= x && minSpanY <= y)) {
                    mWidgets->Add(widget);
                }
                else {
                    Slogger::E(TAG, "Widget " + widget.provider + " can not fit on this device (" +
                          widget.minWidth + ", " + widget.minHeight + ")");
                }
            }
            else {
                Slogger::E(TAG, "Widget " + widget.provider + " has invalid dimensions (" +
                      widget.minWidth + ", " + widget.minHeight + ")");
            }
        }
        else {
            // just add shortcuts
            mWidgets->Add(o);
        }
    }

    return UpdatePageCountsAndInvalidateData();
}

ECode AppsCustomizePagedView::SetBulkBind(
    /* [in] */ Boolean bulkBind)
{
    if (bulkBind) {
        mInBulkBind = TRUE;
    }
    else {
        mInBulkBind = FALSE;
        if (mNeedToUpdatePageCountsAndInvalidateData) {
            UpdatePageCountsAndInvalidateData();
        }
    }
    return NOERROR;
}

void AppsCustomizePagedView::UpdatePageCountsAndInvalidateData()
{
    if (mInBulkBind) {
        mNeedToUpdatePageCountsAndInvalidateData = TRUE;
    }
    else {
        UpdatePageCounts();
        InvalidateOnDataChange();
        mNeedToUpdatePageCountsAndInvalidateData = FALSE;
    }
}

ECode AppsCustomizePagedView::OnClick(
    /* [in] */ IView* v)
{
    // When we have exited all apps or are in transition, disregard clicks
    Boolean res;
    mLauncher->IsAllAppsVisible(res);
    if (!res) return NOERROR;

    AutoPtr<IWorkspace> workspace;
    mLauncher->GetWorkspace((IWorkspace**)&workspace);
    workspace->IsSwitchingState(&res);
    if (res) return NOERROR;

    if (IPagedViewIcon::Probe(v) != NULL) {
        // Animate some feedback to the click
        AutoPtr<IInterface> obj;
        v->GetTag((IInterface**)&obj);
        AutoPtr<IApplicationInfo> appInfo = IApplicationInfo::Probe(obj);

        // Lock the drawable state to pressed until we return to Launcher
        if (mPressedIcon != NULL) {
            mPressedIcon->LockDrawableState();
        }

        // NOTE: We want all transitions from launcher to act as if the wallpaper were enabled
        // to be consistent.  So re-enable the flag here, and we will re-disable it as necessary
        // when Launcher resumes and we are still in AllApps.
        mLauncher->UpdateWallpaperVisibility(true);
        AutoPtr<IIntent> intent;
        appInfo->GetIntent((IIntent**)&intent);
        mLauncher->StartActivitySafely(v, intent, appInfo);

    }
    else if (IPagedViewWidget::Probe(v) != NULL) {
        // Let the user know that they have to long press to add a widget
        if (mWidgetInstructionToast != NULL) {
            mWidgetInstructionToast->Cancel();
        }
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        Toast::MakeText(context,
                Elastos::Droid::Launcher2::R::string::long_press_widget_to_add,
                IToast::LENGTH_SHORT, (IToast**)&mWidgetInstructionToast);
        mWidgetInstructionToast->Show();

        // Create a little animation to show that the widget can move
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        Float offsetY;
        resources->GetDimensionPixelSize(
                Elastos::Droid::Launcher2::R::dimen::dragViewOffsetY,
                &offsetY);
        AutoPtr<IView> view;
        v->FindViewById(
                Elastos::Droid::Launcher2::R::id::widget_preview,
                (IView**)&view);
        AutoPtr<IImageView> p = IImageView::Probe(view);
        AutoPtr<IAnimatorSet> bounce;
        LauncherAnimUtils::CreateAnimatorSet((IAnimatorSet**)&bounce);
        AutoPtr<IValueAnimator> tyuAnim;
        LauncherAnimUtils->>OfFloat(p, String("translationY"),
                offsetY, (IValueAnimator**)&tyuAnim);
        tyuAnim->SetDuration(125);
        AutoPtr<IValueAnimator> tydAnim;
        LauncherAnimUtils::OfFloat(p, String("translationY"),
                0f, (IValueAnimator**)&tydAnim);
        tydAnim->SetDuration(100);
        AutoPtr<IAnimatorSetBuilder> builder;
        bounce->Play(IAnimator::Probe(tyuAnim), (IAnimatorSetBuilder**)&builder);
        builder->Before(tydAnim);
        AutoPtr<IAccelerateInterpolator> tmp;
        CAccelerateInterpolator::New((IAccelerateInterpolator**)&tmp);
        bounce->SetInterpolator(tmp);
        bounce->Start();
    }
    return NOERROR;
}

ECode AppsCustomizePagedView::OnKey(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return FocusHelper::HandleAppsCustomizeKeyEvent(v,  keyCode, event, result);
}

ECode AppsCustomizePagedView::DetermineDraggingStart(
    /* [in] */ IMotionEvent* ev)
{
    // Disable dragging by pulling an app down for now.
    return NOERROR;
}

void AppsCustomizePagedView::BeginDraggingApplication(
    /* [in] */ IView* v)
{
    AutoPtr<IWorkspace> workspace;
    workspace->OnDragStartedWithItem(v);
    workspace->BeginDragShared(v, this);
}

ECode AppsCustomizePagedView::GetDefaultOptionsForWidget(
    /* [in] */ ILauncher* launcher,
    /* [in] */ IPendingAddWidgetInfo* info,
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle);

    AutoPtr<IBundle> options;
    if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN_MR1) {
        Int32 spanX;
        PendingAddWidgetInfo* _info = (PendingAddWidgetInfo*)info;
        AppWidgetResizeFrame::GetWidgetSizeRanges(mLauncher,
                info->mSpanX, info->mSpanY, mTmpRect);
        AutoPtr<IRect> padding;
        AppWidgetHostView::GetDefaultPaddingForWidget(mLauncher,
                info->mComponentName, NULL, (IRect**)&padding);

        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        AutoPtr<IDisplayMetrics> metrics;
        resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
        Float density;
        metrics->GetDensity(&density);
        Int32 left;
        padding->GetLeft(&left);
        Int32 right;
        padding->GetRight(&right);
        Int32 xPaddingDips = (Int32)((left + right) / density);
        Int32 top;
        padding->GetTop(&top);
        Int32 bottom;
        padding->GetBottom(bottom);
        Int32 yPaddingDips = (Int32)((top + bottom) / density);

        CBundle::New((IBundle**)&options);
        mTmpRect->GetLeft(&left);
        options->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_MIN_WIDTH,
                left - xPaddingDips);
        mTmpRect->GetTop(&top);
        options->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_MIN_HEIGHT,
                top - yPaddingDips);
        mTmpRect->GetRight(&right);
        options->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_MAX_WIDTH,
                right - xPaddingDips);
        mTmpRect->GetBottom(bottom);
        options->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_MAX_HEIGHT,
                bottom - yPaddingDips);
    }
    *bundle = options;
    REFCOUNT_ADD(*bundle);
    return NOERROR;
}

void AppsCustomizePagedView::PreloadWidget(
    /* [in] */ IPendingAddWidgetInfo* info)
{
    PendingAddWidgetInfo* _info = (PendingAddWidgetInfo*)info;
    AutoPtr<IAppWidgetProviderInfo> pInfo = info->mInfo;
    AutoPtr<IBundle> options;
    GetDefaultOptionsForWidget(mLauncher, info, (IBundle**)&options);

    AutoPtr<IComponentName> name;
    pInfo->GetConfigure((IComponentName**)&name);
    if (name != NULL) {
        _info->mBindOptions = options;
        return;
    }

    mWidgetCleanupState = WIDGET_PRELOAD_PENDING;
    mBindWidgetRunnable = new MyRunnable2(this, _info);
    Boolean res;
    Post(mBindWidgetRunnable, &res);

    mInflateWidgetRunnable = new MyRunnable3(this, _info);
    Post(mInflateWidgetRunnable, &res);
}

ECode AppsCustomizePagedView::OnShortPress(
    /* [in] */ IView* v)
{
    // We are anticipating a long press, and we use this time to load bind and instantiate
    // the widget. This will need to be cleaned up if it turns out no long press occurs.
    if (mCreateWidgetInfo != NULL) {
        // Just in case the cleanup process wasn't properly executed. This shouldn't happen.
        CleanupWidgetPreloading(FALSE);
    }
    AutoPtr<IInterface> obj;
    v->GetTag((IInterface**)&obj);
    mCreateWidgetInfo = new PendingAddWidgetInfo(IPendingAddWidgetInfo::Probe(obj));
    return PreloadWidget(mCreateWidgetInfo);
}

void AppsCustomizePagedView::CleanupWidgetPreloading(
    /* [in] */ Boolean widgetWasAdded)
{
    if (!widgetWasAdded) {
        // If the widget was not added, we may need to do further cleanup.
        AutoPtr<IPendingAddWidgetInfo> info = mCreateWidgetInfo;
        mCreateWidgetInfo = NULL;

        if (mWidgetCleanupState == WIDGET_PRELOAD_PENDING) {
            // We never did any preloading, so just remove pending callbacks to do so
            RemoveCallbacks(mBindWidgetRunnable);
            RemoveCallbacks(mInflateWidgetRunnable);
        }
        else if (mWidgetCleanupState == WIDGET_BOUND) {
             // Delete the widget id which was allocated
            if (mWidgetLoadingId != -1) {
                AutoPtr<ILauncherAppWidgetHost> appWidgetHost;
                mLauncher->GetAppWidgetHost((ILauncherAppWidgetHost**)&appWidgetHost);
                appWidgetHost->DeleteAppWidgetId(mWidgetLoadingId);
            }

            // We never got around to inflating the widget, so remove the callback to do so.
            RemoveCallbacks(mInflateWidgetRunnable);
        }
        else if (mWidgetCleanupState == WIDGET_INFLATED) {
            // Delete the widget id which was allocated
            if (mWidgetLoadingId != -1) {
                AutoPtr<ILauncherAppWidgetHost> appWidgetHost;
                mLauncher->GetAppWidgetHost((ILauncherAppWidgetHost**)&appWidgetHost);
                appWidgetHost->DeleteAppWidgetId(mWidgetLoadingId);
            }

            // The widget was inflated and added to the DragLayer -- remove it.
            AutoPtr<IAppWidgetHostView> widget = info.boundWidget;
            AutoPtr<IDragLayer> dragLayer;
            dragLayer->RemoveView(widget);
        }
    }
    mWidgetCleanupState = WIDGET_NO_CLEANUP_REQUIRED;
    mWidgetLoadingId = -1;
    mCreateWidgetInfo = NULL;
    return PagedViewWidget::ResetShortPressTarget();
}

ECode AppsCustomizePagedView::CleanUpShortPress(
    /* [in] */ IView* v)
{
    if (!mDraggingWidget) {
        CleanupWidgetPreloading(false);
    }
    return NOERROR;
}

Boolean AppsCustomizePagedView::BeginDraggingWidget(
        /* [in] */ IView* v)
{
    mDraggingWidget = true;
    // Get the widget preview as the drag representation
    AutoPtr<IView> view;
    v->FindViewById(
            Elastos::Droid::Launcher2::::R::id::widget_preview,
            (IView**)&view);
    AutoPtr<IImageView> image = IImageView::Probe(view);
    AutoPtr<IInterface> obj;
    v->GetTag((IInterface**)&obj);
    AutoPtr<IPendingAddItemInfo> createItemInfo = IPendingAddItemInfo::Probe(obj);

    // If the ImageView doesn't have a drawable yet, the widget preview hasn't been loaded and
    // we abort the drag.
    AutoPtr<IDrawable> drawable;
    image->GetDrawable((IDrawable**)&drawable);
    if (drawable == NULL) {
        mDraggingWidget = FALSE;
        return FALSE;
    }

    // Compose the drag image
    AutoPtr<IBitmap> preview;
    AutoPtr<IBitmap> outline;
    Float scale = 1f;
    AutoPtr<IPoint> previewPadding;

    if (IPendingAddWidgetInfo::Probe(createItemInfo) != NULL) {
        // This can happen in some weird cases involving multi-touch. We can't start dragging
        // the widget if this is null, so we break out.
        if (mCreateWidgetInfo == NULL) {
            return FALSE;
        }

        AutoPtr<IPendingAddWidgetInfo> createWidgetInfo = mCreateWidgetInfo;
        createItemInfo = createWidgetInfo;
        Int32 spanX = createItemInfo.spanX;
        Int32 spanY = createItemInfo.spanY;
        AutoPtr<IWorkspace> workspace;
        mLauncher->GetWorkspace((IWorkspace**)&workspace);
        AutoPtr<ArrayOf<Int32> > size;
        workspace->EstimateItemSize(spanX, spanY,
                createWidgetInfo, TRUE, (ArrayOf<Int32>**)&size);

        AutoPtr<IDrawable> drawable;
        image->GetDrawable((IDrawable**)&drawable);
        AutoPtr<IFastBitmapDrawable> previewDrawable = IFastBitmapDrawable::Probe(drawable);
        Float minScale = 1.25f;
        Int32 maxWidth, maxHeight;
        Int32 width;
        previewDrawable->GetIntrinsicWidth(&width);
        maxWidth = Math::Min((Int32)(width * minScale), (*size)[0]);
        Int32 height;
        previewDrawable->GetIntrinsicHeight(&height);
        maxHeight = Math::Min((Int32)(height * minScale), (*size)[1]);

        AutoPtr<ArrayOf<Int32> > previewSizeBeforeScale = ArrayOf<Int32>::Alloc(1);

        mWidgetPreviewLoader->GenerateWidgetPreview(createWidgetInfo.info, spanX,
                spanY, maxWidth, maxHeight, NULL,
                previewSizeBeforeScale, (IBitmap**)&preview);

        // Compare the size of the drag preview to the preview in the AppsCustomize tray
        Int32 previewWidth;
        mWidgetPreviewLoader->MaxWidthForWidgetPreview(spanX, &previewWidth)
        Int32 previewWidthInAppsCustomize = Math::Min((*previewSizeBeforeScale)[0],
                previewWidth);
        Int32 width2;
        preview->GetWidth(&width2);
        scale = previewWidthInAppsCustomize / (Float)width2;

        // The bitmap in the AppsCustomize tray is always the the same size, so there
        // might be extra pixels around the preview itself - this accounts for that
        Int32 intrinsicWidth;
        previewDrawable->GetIntrinsicWidth(&intrinsicWidth);
        if (previewWidthInAppsCustomize < intrinsicWidth) {
            Int32 padding = (intrinsicWidth - previewWidthInAppsCustomize) / 2;
            CPoint::New(padding, 0, (IPoint**)&previewPadding);
        }
    }
    else {
        AutoPtr<IInterface> obj;
        v->GetTag((IInterface**)&obj);
        AutoPtr<IPendingAddShortcutInfo> createShortcutInfo =
                IPendingAddShortcutInfo::Probe(obj);
        // Widgets are only supported for current user, not for other profiles.
        // Hence use myUserHandle().
        AutoPtr<IUserHandle> user;
        Process::MyUserHandle((IUserHandle**)&user);
        AutoPtr<IDrawable> icon;
        mIconCache->GetFullResIcon(createShortcutInfo.shortcutActivityInfo,
                user, (IDrawable**)&icon);
        Int32 intrinsicWidth;
        icon->GetIntrinsicWidth(&intrinsicWidth);
        Int32 intrinsicHeight;
        icon->GetIntrinsicHeight(&intrinsicHeight);
        Bitmap::CreateBitmap(intrinsicWidth, intrinsicHeight,
                Bitmap::Config::ARGB_8888, (IBitmap**)&preview);

        mCanvas->SetBitmap(preview);
        mCanvas->Save();
        WidgetPreviewLoader::RenderDrawableToBitmap(icon, preview, 0, 0,
                intrinsicWidth, intrinsicHeight);
        mCanvas->Restore();
        mCanvas->SetBitmap(NULL);
        createItemInfo.spanX = createItemInfo.spanY = 1;
    }

    // Don't clip alpha values for the drag outline if we're using the default widget preview
    Boolean clipAlpha;
    if (IPendingAddWidgetInfo::Probe(createItemInfo) != NULL) {
        if(IPendingAddWidgetInfo::Probe(createItemInfo)->info->previewImage == 0) {
            clipAlpha = FALSE;
        }
        else{
            clipAlpha = TRUE;
        }
    }
    else{
        clipAlpha = TRUE;
    }

    // Save the preview for the outline generation, then dim the preview
    Int32 width;
    preview->GetWidth(&width);
    Int32 height;
    preview->GetHeight(&height);
    Bitmap::CreateScaledBitmap(preview, width, height,
            FALSE, (IBitmap**)&outline);

    // Start the drag
    mLauncher->LockScreenOrientation();
    AutoPtr<IWorkspace> workspace;
    mLauncher->GetWorkspace((IWorkspace**)&workspace);
    workspace->OnDragStartedWithItem(createItemInfo, outline, clipAlpha);
    mDragController->StartDrag(image, preview, this, createItemInfo,
            IDragController::DRAG_ACTION_COPY, previewPadding, scale);
    outline->Recycle();
    preview->Recycle();
    return TRUE;
}

ECode AppsCustomizePagedView::BeginDragging(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean res;
    PagedViewWithDraggableItems::BeginDragging(v, &res);
    if (!res) {
        *result = FALSE;
        return NOERROR;
    }

    if (IPagedViewIcon::Probe(v) != NULL) {
        BeginDraggingApplication(v);
    }
    else if (IPagedViewWidget::Probe(v) != NULL) {
        if (!BeginDraggingWidget(v)) {
            *result = FALSE;
            return NOERROR;
        }
    }

    // We delay entering spring-loaded mode slightly to make sure the UI
    // thready is free of any work.
    AutoPtr<IRunnable> run = new MyRunnable4(this);
    PostDelayed(run, 150, &res);

    *result = TRUE;
    return NOERROR;
}

void AppsCustomizePagedView::EndDragging(
    /* [in] */ IView* target,
    /* [in] */ Boolean isFlingToDelete,
    /* [in] */ Boolean success)
{
    AutoPtr<IWorkspace> workspace;
    mLauncher->GetWorkspace((IWorkspace**)&workspace);
    if (isFlingToDelete || !success || (TO_IINTERFACE(target) !=
            TO_IINTERFACE(workspace) && IDeleteDropTarget::Probe(target) != NULL)) {
        // Exit spring loaded mode if we have not successfully dropped or have not handled the
        // drop in Workspace
        mLauncher->ExitSpringLoadedDragMode();
    }
    mLauncher->UnlockScreenOrientation(FALSE);
}

ECode AppsCustomizePagedView::GetContent(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    *view = NULL;
    return NOERROR;
}

ECode AppsCustomizePagedView::OnLauncherTransitionPrepare(
    /* [in] */ ILauncher* l,
    /* [in] */ Boolean animated,
    /* [in] */ Boolean toWorkspace)
{
    mInTransition = TRUE;
    if (toWorkspace) {
        CancelAllTasks();
    }
    return NOERROR;
}

ECode AppsCustomizePagedView::OnLauncherTransitionStart(
    /* [in] */ ILauncher* l,
    /* [in] */ Boolean animated,
    /* [in] */ Boolean toWorkspace)
{
    return NOERROR;
}

ECode AppsCustomizePagedView::OnLauncherTransitionStep(
    /* [in] */ ILauncher* l,
    /* [in] */ float t)
{
    return NOERROR;
}

ECode AppsCustomizePagedView::OnLauncherTransitionEnd(
    /* [in] */ ILauncher* l,
    /* [in] */ Boolean animated,
    /* [in] */ Boolean toWorkspace)
{
    mInTransition = FALSE;
    Int32 size;
    mDeferredSyncWidgetPageItems->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mDeferredSyncWidgetPageItems->Get(i, (IInterface**)&obj);
        AutoPtr<IAsyncTaskPageData> d = IAsyncTaskPageData::Probe(obj);

        OnSyncWidgetPageItems(d);
    }
    mDeferredSyncWidgetPageItems->Clear();

    Int32 size2;
    mDeferredPrepareLoadWidgetPreviewsTasks->GetSize(&size2);
    for (Int32 i = 0; i < size2; i++) {
        AutoPtr<IInterface> obj;
        mDeferredPrepareLoadWidgetPreviewsTasks->Get(i, (IInterface**)&obj);
        AutoPtr<IRunnable> r = IRunnable::Probe(obj);
        r->Run();
    }
    mDeferredPrepareLoadWidgetPreviewsTasks->Clear();
    mForceDrawAllChildrenNextFrame = !toWorkspace;
    return NOERROR;
}

ECode AppsCustomizePagedView::OnDropCompleted(
    /* [in] */ IView* target,
    /* [in] */ DragObject* d,
    /* [in] */ Boolean isFlingToDelete,
    /* [in] */ Boolean success)
{
    // Return early and wait for onFlingToDeleteCompleted if this was the result of a fling
    if (isFlingToDelete) return NOERROR;

    EndDragging(target, FALSE, success);

    // Display an error message if the drag failed due to there not being enough space on the
    // target layout we were dropping on.
    if (!success) {
        Boolean showOutOfSpaceMessage = FALSE;
        if (IWorkspace::Probe(target) == NULL) {
            Int32 currentScreen;
            mLauncher->GetCurrentWorkspaceScreen(&currentScreen);
            AutoPtr<IWorkspace> workspace = IWorkspace::Probe(target);
            AutoPtr<IView> view;
            workspace->GetChildAt(currentScreen, (IView**)&view);
            AutoPtr<ICellLayout> layout = ICellLayout::Probe(view);
            AutoPtr<IItemInfo> itemInfo = IItemInfo::Probe(d->mDragInfo);
            if (layout != NULL) {
                layout->CalculateSpans(itemInfo);
                layout->FindCellForSpan(NULL, itemInfo.spanX, itemInfo.spanY, &showOutOfSpaceMessage);
                showOutOfSpaceMessage = !showOutOfSpaceMessage;
            }
        }
        if (showOutOfSpaceMessage) {
            mLauncher->ShowOutOfSpaceMessage(FALSE);
        }

        d->mDeferDragViewCleanupPostAnimation = FALSE;
    }
    CleanupWidgetPreloading(success);
    mDraggingWidget = FALSE;
    return NOERROR;
}

ECode AppsCustomizePagedView::OnFlingToDeleteCompleted()
{
    // We just dismiss the drag when we fling, so cleanup here
    EndDragging(NULL, TRUE, TRUE);
    CleanupWidgetPreloading(FALSE);
    mDraggingWidget = FALSE;
    return NOERROR;
}

ECode AppsCustomizePagedView::SupportsFlingToDelete(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = TRUE;
    return NOERROR;
}

ECode AppsCustomizePagedView::OnDetachedFromWindow()
{
    PagedViewWithDraggableItems::OnDetachedFromWindow();
    return CancelAllTasks();
}

ECode AppsCustomizePagedView::ClearAllWidgetPages()
{
    CancelAllTasks();
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> v;
        GetPageAt(i, (IView**)&v);
        if (IPagedViewGridLayout::Probe(v) != NULL) {
            IPagedViewGridLayout::Probe(v)->RemoveAllViewsOnPage();
            mDirtyPageContent->Set(i, TRUE);
        }
    }
    return NOERROR;
}

void AppsCustomizePagedView::CancelAllTasks()
{
    // Clean up all the async tasks
    AutoPtr<IIterator> iter;
    mRunningTasks->GetIterator((IIterator**)&iter);
    Boolean res;
    while (iter->HasNext(&res), res) {
        AutoPtr<IInterface> obj;
        iter->Next((IInterface**)&obj);
        AutoPtr<IAppsCustomizeAsyncTask> task = IAppsCustomizeAsyncTask::Probe(obj);
        task->Cancel(FALSE);
        iter->Remove();
        mDirtyPageContent->Set(task.page, TRUE);

        // We've already preallocated the views for the data to load into, so clear them as well
        AutoPtr<IView> v;
        GetPageAt(task.page, (IView**)&v);
        if (IPagedViewGridLayout::Probe(v) != NULL) {
            IPagedViewGridLayout::Probe(v)->RemoveAllViewsOnPage();
        }
    }
    mDeferredSyncWidgetPageItems->Clear();
    mDeferredPrepareLoadWidgetPreviewsTasks->Clear();
}

ECode AppsCustomizePagedView::SetContentType(
    /* [in] */ IContentType* type)
{
    if (type == IContentType::Widgets) {
        InvalidatePageData(mNumAppsPages, TRUE);
    }
    else if (type == IContentType::Applications) {
        InvalidatePageData(0, TRUE);
    }
    return NOERROR;
}

ECode AppsCustomizePagedView::SnapToPage(
    /* [in] */ Int32 whichPage,
    /* [in] */ Int32 delta,
    /* [in] */ Int32 duration)
{
    PagedViewWithDraggableItems::SnapToPage(whichPage, delta, duration);
    UpdateCurrentTab(whichPage);

    // Update the thread priorities given the direction lookahead
    AutoPtr<IIterator> iter;
    mRunningTasks->GetIterator((IIterator**)&iter);
    Boolean res;
    while (iter->HasNext(&res), res) {
        AutoPtr<IInterface> obj;
        iter->Next((IInterface**)&obj);
        AutoPtr<IAppsCustomizeAsyncTask> task = IAppsCustomizeAsyncTask::Probe(obj);
        Int32 pageIndex = task.page;
        if ((mNextPage > mCurrentPage && pageIndex >= mCurrentPage) ||
            (mNextPage < mCurrentPage && pageIndex <= mCurrentPage)) {
            Int32 prio;
            GetThreadPriorityForPage(pageIndex, &prio);
            task->SetThreadPriority(prio);
        }
        else {
            task->SetThreadPriority(IProcess::THREAD_PRIORITY_LOWEST);
        }
    }
    return NOERROR;
}

void AppsCustomizePagedView::UpdateCurrentTab(
        /* [in] */ Int32 currentPage)
{
    AutoPtr<IAppsCustomizeTabHost> tabHost;
    GetTabHost((IAppsCustomizeTabHost**)&tabHost);
    if (tabHost != NULL) {
        String tag;
        tabHost->GetCurrentTabTag(&tag);
        if (!tag.IsNull()) {
            String type;
            tabHost->GetTabTagForContentType(IContentType::Widgets, &type);
            Boolean res;
            if (currentPage >= mNumAppsPages && tag.Equals(type, &res), !res)) {
                tabHost->SetCurrentTabFromContent(IContentType::Widgets);
            } else if (currentPage < mNumAppsPages) {
                tabHost->GetTabTagForContentType(IContentType::Applications, &type);
                if (tag.Equals(type, &res), !res)) {
                    tabHost.setCurrentTabFromContent(ContentType.Applications);
                }
            }
        }
    }
}

void AppsCustomizePagedView::SetVisibilityOnChildren(
    /* [in] */ IViewGroup* layout,
    /* [in] */ Int32 visibility)
{
    Int32 childCount;
    layout->GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; ++i) {
        AutoPtr<IView> view;
        layout->GetChildAt(i, (IView**)&view);
        view->SetVisibility(visibility);
    }
}

void AppsCustomizePagedView::SetupPage(
    /* [in] */ IPagedViewCellLayout* layout)
{
    layout->SetCellCount(mCellCountX, mCellCountY);
    layout->SetGap(mPageLayoutWidthGap, mPageLayoutHeightGap);
    layout->SetPadding(mPageLayoutPaddingLeft, mPageLayoutPaddingTop,
            mPageLayoutPaddingRight, mPageLayoutPaddingBottom);

    // Note: We force a measure here to get around the fact that when we do layout calculations
    // immediately after syncing, we don't have a proper width.  That said, we already know the
    // expected page width, so we can actually optimize by hiding all the TextView-based
    // children that are expensive to measure, and let that happen naturally later.
    SetVisibilityOnChildren(layout, IView::GONE);
    Int32 measuredWidth;
    GetMeasuredWidth(&measuredWidth);
    Int32 measuredHeight;
    GetMeasuredHeight(&measuredHeight);
    Int32 widthSpec;
    MeasureSpec::MakeMeasureSpec(measuredWidth, IMeasureSpec::AT_MOST, &widthSpec);
    Int32 heightSpec;
    MeasureSpec::MakeMeasureSpec(measuredHeight, IMeasureSpec::AT_MOST, &heightSpec);
    Int32 pageContentWidth;
    GetPageContentWidth(&pageContentWidth);
    layout->SetMinimumWidth(pageContentWidth);
    layout->Measure(widthSpec, heightSpec);
    SetVisibilityOnChildren(layout, IView::VISIBLE);
}

ECode AppsCustomizePagedView::SyncAppsPageItems(
    /* [in] */ Int page,
    /* [in] */ Boolean immediate)
{
    // ensure that we have the right number of items on the pages
    Boolean isRtl;
    IsLayoutRtl(&isRtl);
    Int32 numCells = mCellCountX * mCellCountY;
    Int32 startIndex = page * numCells;
    Int32 size;
    mApps->GetSize(&size);
    Int32 endIndex = Math::Min(startIndex + numCells, size);
    AutoPtr<IView> view;
    GetPageAt(page, (IView**)&view);
    AutoPtr<IPagedViewCellLayout> layout = IPagedViewCellLayout::Probe(view);

    layout->RemoveAllViewsOnPage();
    AutoPtr<IArrayList> items;
    CArrayList::New((IArrayList**)&items);
    AutoPtr<IArrayList> images;
    CArrayList::New((IArrayList**)&images);
    for (Int32 i = startIndex; i < endIndex; ++i) {
        AutoPtr<IInterface> obj;
        mApps->Get(i, (IInterface**)&obj);
        AutoPtr<IApplicationInfo> info = IApplicationInfo::Probe(obj);
        AutoPtr<IView> view;
        mLayoutInflater->Inflate(
                Elastos::Droid::Launcher2::R::layout::apps_customize_application,
                layout, FALSE, (IView**)&view);
        AutoPtr<IPagedViewIcon> icon = IPagedViewIcon::Probe(view);
        icon->ApplyFromApplicationInfo(info, TRUE, this);
        icon->SetOnClickListener(this);
        icon->SetOnLongClickListener(this);
        icon->SetOnTouchListener(this);
        icon->SetOnKeyListener(this);

        Int32 index = i - startIndex;
        Int32 x = index % mCellCountX;
        Int32 y = index / mCellCountX;
        if (isRtl) {
            x = mCellCountX - x - 1;
        }
        AutoPtr<PagedViewCellLayout::LayoutParams> params =
                new PagedViewCellLayout::LayoutParams(x,y, 1,1);
        layout->AddViewToCellLayout(icon, -1, i, params);

        items->Add(info);
        images->Add(info.iconBitmap);
    }

    return EnableHwLayersOnVisiblePages();
}

Int32 AppsCustomizePagedView::GetWidgetPageLoadPriority(
    /* [in] */ Int32 page)
{
    // If we are snapping to another page, use that index as the target page index
    Int32 toPage = mCurrentPage;
    if (mNextPage > -1) {
        toPage = mNextPage;
    }

    // We use the distance from the target page as an initial guess of priority, but if there
    // are no pages of higher priority than the page specified, then bump up the priority of
    // the specified page.
    Int32 minPageDiff = Integer.MAX_VALUE;
    AutoPtr<IIterator> iter;
    mRunningTasks->GetIterator((IIterator**)&iter);
    Boolean res;
    while (iter->HasNext(&res), res) {
        AutoPtr<IInterface> obj;
        iter->Next((IInterface**)&obj);
        AutoPtr<IAppsCustomizeAsyncTask> task = IAppsCustomizeAsyncTask::Probe(obj);
        minPageDiff = Math::Abs(task.page - toPage);
    }

    Int32 rawPageDiff = Math::Abs(page - toPage);
    return rawPageDiff - Math::Min(rawPageDiff, minPageDiff);
}

Int32 AppsCustomizePagedView::GetThreadPriorityForPage(
    /* [in] */ Int32 page)
{
    // TODO-APPS_CUSTOMIZE: detect number of cores and set thread priorities accordingly below
    Int32 pageDiff;
    GetWidgetPageLoadPriority(page, &pageDiff);
    if (pageDiff <= 0) {
        return IProcess::THREAD_PRIORITY_LESS_FAVORABLE;
    }
    else if (pageDiff <= 1) {
        return IProcess::THREAD_PRIORITY_LOWEST;
    }
    else {
        return IProcess::THREAD_PRIORITY_LOWEST;
    }
}

Int32 AppsCustomizePagedView::GetSleepForPage(
    /* [in] */ Int32 page)
{
    Int32 pageDiff;
    GetWidgetPageLoadPriority(page, &pageDiff);
    return Math::Max(0, pageDiff * sPageSleepDelay);
}

void AppsCustomizePagedView::PrepareLoadWidgetPreviewsTask(
    /* [in] */ Int32 page,
    /* [in] */ IArrayList* widgets,
    /* [in] */ Int32 cellWidth,
    /* [in] */ Int32 cellHeight,
    /* [in] */ Int32 cellCountX)
{
    // Prune all tasks that are no longer needed
    Int32 minPageDiff = Integer.MAX_VALUE;
    AutoPtr<IIterator> iter;
    mRunningTasks->GetIterator((IIterator**)&iter);
    Boolean res;
    while (iter->HasNext(&res), res) {
        AutoPtr<IInterface> obj;
        iter->Next((IInterface**)&obj);
        AutoPtr<IAppsCustomizeAsyncTask> task = IAppsCustomizeAsyncTask::Probe(obj);
        Int32 taskPage = task.page;
        Int32 lowerPageBound;
        GetAssociatedLowerPageBound(mCurrentPage, &lowerPageBound);
        Int32 upperPageBound;
        GetAssociatedUpperPageBound(mCurrentPage, &upperPageBound);
        if (taskPage < lowerPageBound || taskPage > upperPageBound) {
            task->Cancel(FALSE);
            iter->Remove();
        }
        else {
            Int32 priorityForPage;
            GetThreadPriorityForPage(taskPage, &priorityForPage);
            task->SetThreadPriority(priorityForPage);
        }
    }

    // We introduce a slight delay to order the loading of side pages so that we don't thrash
    Int32 sleepMs;
    GetSleepForPage(page, &sleepMs);
    AutoPtr<IAppsCustomizePagedViewAsyncTaskCallback> Callback1 =
            new MyAsyncTaskCallback(this, sleepMs);
    AutoPtr<IAppsCustomizePagedViewAsyncTaskCallback> Callback2 =
            new MyAsyncTaskCallback2(this);
    AutoPtr<IAppsCustomizePagedViewAsyncTaskPageData> pageData =
            new AsyncTaskPageData(page, widgets, cellWidth, cellHeight,
            Callback1, Callback2, mWidgetPreviewLoader);

    // Ensure that the task is appropriately prioritized and runs in parallel
    AutoPtr<IAppsCustomizePagedViewAppsCustomizeAsyncTask> t =
            new AppsCustomizeAsyncTask(page, AsyncTaskPageDataType::LoadWidgetPreviewData);
    Int32 priorityForPage;
    GetThreadPriorityForPage(page, &priorityForPage);
    t->SetThreadPriority(priorityForPage);
    t->ExecuteOnExecutor(IAsyncTask::THREAD_POOL_EXECUTOR, pageData);
    mRunningTasks->Add(t);
}

void AppsCustomizePagedView::SetupPage(
        /* [in] */ IPagedViewGridLayout* layout)
{
    layout->SetPadding(mPageLayoutPaddingLeft, mPageLayoutPaddingTop,
            mPageLayoutPaddingRight, mPageLayoutPaddingBottom);

    // Note: We force a measure here to get around the fact that when we do layout calculations
    // immediately after syncing, we don't have a proper width.
    Int32 measuredWidth;
    GetMeasuredWidth(&measuredWidth);
    Int32 widthSpec;
    MeasureSpec::MakeMeasureSpec(measuredWidth, IMeasureSpec::AT_MOST, &widthSpec);
    Int32 measuredHeight;
    GetMeasuredHeight(&measuredHeight);
    Int32 heightSpec;
    MeasureSpec::MakeMeasureSpec(measuredHeight, IMeasureSpec::AT_MOST, &heightSpec);
    Int32 width;
    GetPageContentWidth(&width);
    layout->SetMinimumWidth(width);
    layout->Measure(widthSpec, heightSpec);
}

ECode AppsCustomizePagedView::SyncWidgetPageItems(
    /* [in] */ Int32 page,
    /* [in] */ Boolean immediate)
{
    Int32 numItemsPerPage = mWidgetCountX * mWidgetCountY;

    // Calculate the dimensions of each cell we are giving to each widget
    AutoPtr<IArrayList> items;
    CArrayList::New((IArrayList**)&items);
    Int32 contentWidth;
    mWidgetSpacingLayout->GetContentWidth(&contentWidth);
    Int32 cellWidth = ((contentWidth - mPageLayoutPaddingLeft - mPageLayoutPaddingRight
            - ((mWidgetCountX - 1) * mWidgetWidthGap)) / mWidgetCountX);
    Int32 contentHeight;
    mWidgetSpacingLayout->GetContentHeight(&contentHeight);
    Int32 cellHeight = ((contentHeight - mPageLayoutPaddingTop - mPageLayoutPaddingBottom
            - ((mWidgetCountY - 1) * mWidgetHeightGap)) / mWidgetCountY);

    // Prepare the set of widgets to load previews for in the background
    Int32 offset = (page - mNumAppsPages) * numItemsPerPage;
    Int32 size;
    mWidgets->GetSize(&size);
    for (Int32 i = offset; i < Math::Min(offset + numItemsPerPage, size); ++i) {
        AutoPtr<IInterface> obj;
        mWidgets->Get(i, (IInterface**)&obj);
        items->Add(obj);
    }

    // Prepopulate the pages with the other widget info, and fill in the previews later
    AutoPtr<IView> view;
    GetPageAt(page, (IView**)&view);
    AutoPtr<IPagedViewGridLayout> layout = IPagedViewGridLayout::Probe(view);
    Int32 x;
    layout->GetCellCountX(&x);
    layout->SetColumnCount(x);
    Int32 size2;
    items->GetSize(&size2);
    for (Int32 i = 0; i < size2; ++i) {
        AutoPtr<IInterface> rawInfo;
        items->Get(i, (IInterface**)&rawInfo);
        AutoPtr<IPendingAddItemInfo> createItemInfo;
        AutoPtr<IView> _view;
        mLayoutInflater->Inflate(
                Elastos::Droid::Launcher2::R::layout::apps_customize_widget,
                layout, FALSE, (IView**)&_view);
        AutoPtr<IPagedViewWidget> widget = IPagedViewWidget::Probe(_view);
        if (IAppWidgetProviderInfo::Probe(rawInfo) != NULL) {
            // Fill in the widget information
            AutoPtr<IAppWidgetProviderInfo> info = IAppWidgetProviderInfo::Probe(rawInfo);
            createItemInfo = new PendingAddWidgetInfo(info, NULL, NULL);

            // Determine the widget spans and min resize spans.
            AutoPtr<ArrayOf<Int32> > spanXY;
            Launcher::GetSpanForWidget(mLauncher, info, (ArrayOf<Int32>**)&spanXY);
            createItemInfo.spanX = (*spanXY)[0];
            createItemInfo.spanY = (*spanXY)[1];
            AutoPtr<ArrayOf<Int32> > minSpanXY;
            Launcher::GetMinSpanForWidget(mLauncher, info, (ArrayOf<Int32>**)&minSpanXY);
            createItemInfo.minSpanX = (*minSpanXY)[0];
            createItemInfo.minSpanY = (*minSpanXY)[1];

            widget->ApplyFromAppWidgetProviderInfo(info, -1, spanXY, mWidgetPreviewLoader);
            widget->SetTag(createItemInfo);
            widget->SetShortPressListener(this);
        }
        else if (IResolveInfo::Probe(rawInfo) != NULL) {
            // Fill in the shortcuts information
            AutoPtr<IResolveInfo> info = IResolveInfo::Probe(rawInfo);
            createItemInfo = new PendingAddShortcutInfo(info.activityInfo);
            createItemInfo.itemType = LauncherSettings::Favorites::ITEM_TYPE_SHORTCUT;
            createItemInfo.componentName = new ComponentName(info.activityInfo.packageName,
                    info.activityInfo.name);
            widget->ApplyFromResolveInfo(mPackageManager, info, mWidgetPreviewLoader);
            widget->SetTag(createItemInfo);
        }
        widget->SetOnClickListener(this);
        widget->SetOnLongClickListener(this);
        widget->SetOnTouchListener(this);
        widget->SetOnKeyListener(this);

        // Layout each widget
        Int32 ix = i % mWidgetCountX;
        Int32 iy = i / mWidgetCountX;
        AutoPtr<IGridLayoutSpec> specx;
        GridLayout::Spec(iy, IGridLayout::START, (IGridLayoutSpec**)&specx);
        AutoPtr<IGridLayoutSpec> specy;
        GridLayout::Spec(ix, IGridLayout::TOP, (IGridLayoutSpec**)&specy);
        AutoPtr<IGridLayoutLayoutParams> lp;
        CGridLayoutLayoutParams::New(specx, specy, (IGridLayoutLayoutParams**)&lp);
        lp.width = cellWidth;
        lp.height = cellHeight;
        lp->SetGravity(IGravity::TOP | IGravity::START);
        if (ix > 0) lp.leftMargin = mWidgetWidthGap;
        if (iy > 0) lp.topMargin = mWidgetHeightGap;
        layout->AddView(widget, lp);
    }

    // wait until a call on onLayout to start loading, because
    // PagedViewWidget.getPreviewSize() will return 0 if it hasn't been laid out
    // TODO: can we do a measure/layout immediately?
    AutoPtr<IRunnable> run = new MyRunnable5(this, cellWidth,
            cellHeight, layout, immediate);
    return layout->SetOnLayoutListener(run);
}

void AppsCustomizePagedView::LoadWidgetPreviewsInBackground(
    /* [in] */ IAppsCustomizePagedViewAppsCustomizeAsyncTask* task,
    /* [in] */ IAppsCustomizePagedViewAsyncTaskPageData* data)
{
    // loadWidgetPreviewsInBackground can be called without a task to load a set of widget
    // previews synchronously
    if (task != NULL) {
        // Ensure that this task starts running at the correct priority
        task->SyncThreadPriority();
    }

    // Load each of the widget/shortcut previews
    AutoPtr<IArrayList> items = data.items;
    AutoPtr<IArrayList> images = data.generatedImages;
    Int32 count;
    items->SetSize(&count);
    for (Int32 i = 0; i < count; ++i) {
        if (task != NULL) {
            // Ensure we haven't been cancelled yet
            Boolean res;
            task->IsCancelled(&res);
            if (res) break;
            // Before work on each item, ensure that this task is running at the correct
            // priority
            task->SyncThreadPriority();
        }

        AutoPtr<IInterface> obj;
        items->Get(i, (IInterface**)&obj);
        AutoPtr<IBitmap> bitmap;
        mWidgetPreviewLoader->GetPreview((IBitmap**)&bitmap);
        images->Add(TO_IINTERFACE(bitmap));
    }
}

void AppsCustomizePagedView::OnSyncWidgetPageItems(
    /* [in] */ IAppsCustomizePagedViewAsyncTaskPageData* data)
{
    if (mInTransition) {
        mDeferredSyncWidgetPageItems->Add(data);
        return;
    }
    //try {
    Int32 page = data.page;
    AutoPtr<IView> view;
    FAIL_GOTO(GetPageAt(page, (IView**)&view), FINALLY);
    AutoPtr<IPagedViewGridLayout> layout = IPagedViewGridLayout::Probe(view);

    AutoPtr<IArrayList> items = data.items;
    Int32 count;
    FAIL_GOTO(items->GetSize(&count), FINALLY);
    for (Int32 i = 0; i < count; ++i) {
        AutoPtr<IView> view;
        FAIL_GOTO(layout->GetChildAt(i, (IView**)&view), FINALLY);
        AutoPtr<IPagedViewWidget> widget = IPagedViewWidget::Probe(view);
        if (widget != NULL) {
            AutoPtr<IInterface> obj;
            data.generatedImages->Get(i, (IInterface**)&obj);
            AutoPtr<IBitmap> preview = IBitmap::Probe(obj);
            AutoPtr<IFastBitmapDrawable> drawable;
            CFastBitmapDrawable::New(preview, (IFastBitmapDrawable**)&drawable);
            FAIL_GOTO(widget->ApplyPreview(drawable, i), FINALLY);
        }
    }

    FAIL_GOTO(EnableHwLayersOnVisiblePages(), FINALLY);

    // Update all thread priorities
    AutoPtr<IIterator> iter;
    mRunningTasks->GetIterator((IIterator**)&iter);
    Boolean res;
    while (iter->HasNext(&res), res) {
        AutoPtr<IInterface> obj;
        iter->Next((IInterface**)&obj);
        AutoPtr<IAppsCustomizeAsyncTask> task = IAppsCustomizeAsyncTask::Probe(obj);
        Int32 pageIndex = task.page;
        Int32 priorityForPage;
        FAIL_GOTO(GetThreadPriorityForPage(pageIndex, &priorityForPage), FINALLY);
        FAIL_GOTO(task->SetThreadPriority(priorityForPage), FINALLY);
    }
    //} finally {
FINALLY:
    data->Cleanup(FALSE);
    //}
}

ECode AppsCustomizePagedView::SyncPages()
{
    RemoveAllViews();
    CancelAllTasks();

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    for (Int32 j = 0; j < mNumWidgetPages; ++j) {
        AutoPtr<PagedViewGridLayout> layout = new PagedViewGridLayout(
                context, mWidgetCountX, mWidgetCountY);
        SetupPage(layout);
        AutoPtr<PagedView::LayoutParams> params = new
                PagedView::LayoutParams(ILayoutParams::MATCH_PARENT,
                ILayoutParams::MATCH_PARENT)
        AddView(layout, params);
    }

    for (Int32 i = 0; i < mNumAppsPages; ++i) {
        AutoPtr<IPagedViewCellLayout> layout = new PagedViewCellLayout(context);
        SetupPage(layout);
        AddView(layout);
    }
    return NOERROR;
}

CARAPI AppsCustomizePagedView::SyncPageItems(
    /* [in] */ Int32 page,
    /* [in] */ Boolean immediate)
{
    if (page < mNumAppsPages) {
        SyncAppsPageItems(page, immediate);
    }
    else {
        SyncWidgetPageItems(page, immediate);
    }
    return NOERROR;
}

CARAPI AppsCustomizePagedView::GetPageAt(
    /* [in] */ Int32 index,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    Int32 _index;
    IndexToPage(index, &_index);
    return GetChildAt(_index, view);
}

CARAPI AppsCustomizePagedView::ScreenScrolled(
    /* [in] */ Int32 screenCenter)
{
    Boolean isRtl;
    IsLayoutRtl(&isRtl);
    PagedViewWithDraggableItems::ScreenScrolled(screenCenter);

    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> v;
        GetPageAt(i, (IView**)&v);
        if (v != NULL) {
            Float scrollProgress;
            GetScrollProgress(screenCenter, v, i. &scrollProgress);

            Float interpolatedProgress;
            Float translationX;
            Float maxScrollProgress = Math::Max(0, scrollProgress);
            Float minScrollProgress = Math::Min(0, scrollProgress);

            if (isRtl) {
                Int32 width;
                v->GetMeasuredWidth(&width);
                translationX = maxScrollProgress * width;
                mZInterpolator->GetInterpolation(Math::Abs(maxScrollProgress),
                        &interpolatedProgress);
            }
            else {
                Int32 width;
                v->GetMeasuredWidth(&width);
                translationX = minScrollProgress * width;
                mZInterpolator->GgetInterpolation(Math::Abs(minScrollProgress),
                        &interpolatedProgress);
            }
            Float scale = (1 - interpolatedProgress) +
                    interpolatedProgress * TRANSITION_SCALE_FACTOR;

            Float alpha;
            if (isRtl && (scrollProgress > 0)) {
                mAlphaInterpolator->GetInterpolation(1 - Math::Abs(maxScrollProgress),
                        &alpha);
            }
            else if (!isRtl && (scrollProgress < 0)) {
                mAlphaInterpolator->GetInterpolation(1 - Math::Abs(scrollProgress),
                        &alpha);
            }
            else {
                //  On large screens we need to fade the page as it nears its leftmost position
                mLeftScreenAlphaInterpolator->GetInterpolation(1 - scrollProgress,
                        &alpha);
            }

            v->SetCameraDistance(mDensity * CAMERA_DISTANCE);
            Int32 pageWidth;
            v->GetMeasuredWidth(&pageWidth);
            Int32 pageHeight;
            v->GetMeasuredHeight(&pageHeight);

            if (PERFORM_OVERSCROLL_ROTATION) {
                Float xPivot = isRtl ? 1f - TRANSITION_PIVOT : TRANSITION_PIVOT;
                Boolean isOverscrollingFirstPage = isRtl ?
                        scrollProgress > 0 : scrollProgress < 0;
                Boolean isOverscrollingLastPage = isRtl ?
                        scrollProgress < 0 : scrollProgress > 0;

                Int32 count;
                GetChildCount(&count);
                if (i == 0 && isOverscrollingFirstPage) {
                    // Overscroll to the left
                    v->SetPivotX(xPivot * pageWidth);
                    v->SetRotationY(-TRANSITION_MAX_ROTATION * scrollProgress);
                    scale = 1.0f;
                    alpha = 1.0f;
                    // On the first page, we don't want the page to have any lateral motion
                    translationX = 0;
                }
                else if (i == count - 1 && isOverscrollingLastPage) {
                    // Overscroll to the right
                    v->SetPivotX((1 - xPivot) * pageWidth);
                    v->SetRotationY(-TRANSITION_MAX_ROTATION * scrollProgress);
                    scale = 1.0f;
                    alpha = 1.0f;
                    // On the last page, we don't want the page to have any lateral motion.
                    translationX = 0;
                }
                else {
                    v->SetPivotY(pageHeight / 2.0f);
                    v->SetPivotX(pageWidth / 2.0f);
                    v->SetRotationY(0f);
                }
            }

            v->SetTranslationX(translationX);
            v->SetScaleX(scale);
            v->SetScaleY(scale);
            v->SetAlpha(alpha);

            // If the view has 0 alpha, we set it to be invisible so as to prevent
            // it from accepting touches
            Int32 visibility;
            v->GetVisibility(&visibility);
            if (alpha == 0) {
                v->SetVisibility(INVISIBLE);
            }
            else if (visibility != VISIBLE) {
                v->SetVisibility(VISIBLE);
            }
        }
    }

    return EnableHwLayersOnVisiblePages();
}

void AppsCustomizePagedView::EnableHwLayersOnVisiblePages()
{
    Int32 screenCount;
    GetChildCount(&screenCount);

    GetVisiblePages(mTempVisiblePagesRange);
    Int32 leftScreen = (*mTempVisiblePagesRange)[0];
    Int32 rightScreen = (*mTempVisiblePagesRange)[1];
    Int32 forceDrawScreen = -1;
    if (leftScreen == rightScreen) {
        // make sure we're caching at least two pages always
        if (rightScreen < screenCount - 1) {
            rightScreen++;
            forceDrawScreen = rightScreen;
        }
        else if (leftScreen > 0) {
            leftScreen--;
            forceDrawScreen = leftScreen;
        }
    }
    else {
        forceDrawScreen = leftScreen + 1;
    }

    for (Int32 i = 0; i < screenCount; i++) {
        AutoPtr<IView> layout;
        GetPageAt(i, (IView**)&layout);
        Boolean res;
        if (!(leftScreen <= i && i <= rightScreen &&
                (i == forceDrawScreen || (ShouldDrawChild(layout, &res), res)))) {
            layout->SetLayerType(LAYER_TYPE_NONE, NULL);
        }
    }

    for (Int32 i = 0; i < screenCount; i++) {
        AutoPtr<IView> layout;
        GetPageAt(i, (IView**)&layout);
        Boolean res;
        if (leftScreen <= i && i <= rightScreen &&
                (i == forceDrawScreen || (ShouldDrawChild(layout, &res), res))) {
            Int32 type;
            layout->GetLayerType(&type);
            if (type != LAYER_TYPE_HARDWARE) {
                layout->SetLayerType(LAYER_TYPE_HARDWARE, NULL);
            }
        }
    }
}

ECode AppsCustomizePagedView::OverScroll(
    /* [in] */ Float amount)
{
    return AcceleratedOverScroll(amount);
}

ECode AppsCustomizePagedView::GetPageContentWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);

    *width = mContentWidth;
    return NOERROR;
}

ECode AppsCustomizePagedView::OnPageEndMoving()
{
    PagedViewWithDraggableItems::OnPageEndMoving();
    mForceDrawAllChildrenNextFrame = TRUE;
    // We reset the save index when we change pages so that it will be recalculated on next
    // rotation
    mSaveInstanceStateItemIndex = -1;
    return NOERROR;
}

ECode AppsCustomizePagedView::Setup(
    /* [in] */ ILauncher* launcher,
    /* [in] */ IDragController* dragController)
{
    mLauncher = launcher;
    mDragController = dragController;
    return NOERROR;
}

void AppsCustomizePagedView::InvalidateOnDataChange()
{
    Boolean res;
    IsDataReady(&res);
    if (!res) {
        // The next layout pass will trigger data-ready if both widgets and apps are set, so
        // request a layout to trigger the page data when ready.
        RequestLayout();
    }
    else {
        CancelAllTasks();
        InvalidatePageData();
    }
}

ECode AppsCustomizePagedView::SetApps(
    /* [in] */ IArrayList* list)
{
    mApps = list;
    AutoPtr<IComparato> comparator;
    LauncherModel::GetAppNameComparator((IComparato**)&comparator);
    Collections::Sort(mApps, comparator);
    return UpdatePageCountsAndInvalidateData();
}

void AppsCustomizePagedView::AddAppsWithoutInvalidate(
        /* [in] */ IArrayList* list)
{
    // We add it in place, in alphabetical order
    Int32 count;
    list->GetSize(&count);
    for (Int32 i = 0; i < count; ++i) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        AutoPtr<IApplicationInfo> info = IApplicationInfo::Probe(obj);
        AutoPtr<IComparato> comparator;
        LauncherModel::GetAppNameComparator((IComparato**)&comparator);
        Int32 index;
        Collections::BinarySearch(mApps, info, comparator, &index);
        if (index < 0) {
            mApps->Add(-(index + 1), info);
        }
    }
}

ECode AppsCustomizePagedView::AddApps(
    /* [in] */ IArrayList* list)
{
    AddAppsWithoutInvalidate(list);
    return UpdatePageCountsAndInvalidateData();
}

Int32 AppsCustomizePagedView::FindAppByComponent(
    /* [in] */ IList* list,
    /* [in] */ IApplicationInfo* item)
{
    AutoPtr<IComponentName> removeComponent;
    item->mIntent->GetComponent((IComponentName**)&removeComponent);
    Int32 length;
    list->GetSize(&length);
    for (Int32 i = 0; i < length; ++i) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        AutoPtr<IApplicationInfo> info = IApplicationInfo::Probe(obj);
        if (info.user.Equals(item.user)
                && info.intent.getComponent().equals(removeComponent)) {
            return i;
        }
    }
    return -1;
}

void AppsCustomizePagedView::RemoveAppsWithoutInvalidate(
    /* [in] */ IArrayList* list)
{
    // loop through all the apps and remove apps that have the same component
    Int32 length;
    list->GetSize(&length);
    for (Int32 i = 0; i < length; ++i) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        AutoPtr<IApplicationInfo> info = IApplicationInfo::Probe(obj);
        Int32 removeIndex;
        FindAppByComponent(mApps, info, &removeIndex);
        if (removeIndex > -1) {
            mApps->Remove(removeIndex);
        }
    }
}

ECode AppsCustomizePagedView::RemoveApps(
    /* [in] */ IArrayList* appInfos)
{
    RemoveAppsWithoutInvalidate(appInfos);
    return UpdatePageCountsAndInvalidateData();
}

ECode AppsCustomizePagedView::UpdateApps(
    /* [in] */ IArrayList* list)
{
    // We remove and re-add the updated applications list because it's properties may have
    // changed (ie. the title), and this will ensure that the items will be in their proper
    // place in the list.
    RemoveAppsWithoutInvalidate(list);
    AddAppsWithoutInvalidate(list);
    return UpdatePageCountsAndInvalidateData();
}

ECode AppsCustomizePagedView::Reset()
{
    // If we have reset, then we should not continue to restore the previous state
    mSaveInstanceStateItemIndex = -1;

    AutoPtr<IAppsCustomizeTabHost> tabHost;
    GetTabHost((IAppsCustomizeTabHost**)&tabHost);
    String tag;
    tabHost->GetCurrentTabTag(&tag);
    if (tag != NULL) {
        String type;
        tabHost->GetTabTagForContentType(IContentType::Applications, &type);
        Boolean res;
        tag.Equals(type, &res);
        if (!res) {
            tabHost->SetCurrentTabFromContent(IContentType::Applications);
        }
    }

    if (mCurrentPage != 0) {
        InvalidatePageData(0);
    }
    return NOERROR;
}

AutoPtr<IAppsCustomizeTabHost> AppsCustomizePagedView::GetTabHost()
{
    AutoPtr<IView> view;
    mLauncher->FindViewById(
            Elastos::Droid::Launcher2::R::id::apps_customize_pane,
            (IView**)&view);
    AutoPtr<IAppsCustomizeTabHost> tmp = IAppsCustomizeTabHost::Probe(view);
    return tmp;
}

ECode AppsCustomizePagedView::DumpState()
{
    // TODO: Dump information related to current list of Applications, Widgets, etc.
    ApplicationInfo::DumpApplicationInfoList(TAG, String("mApps"), mApps);
    return DumpAppWidgetProviderInfoList(TAG, String("mWidgets"), mWidgets);
}

void AppsCustomizePagedView::DumpAppWidgetProviderInfoList(
    /* [in] */ const String& tag,
    /* [in] */ const String& label,
    /* [in] */ IArrayList* list)
{
    Slogger::D(tag, label + " size=" + list.size());
    Int32 size;
    list->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);

        if (IAppWidgetProviderInfo::Probe(obj) != NULL) {
            AUtoPtr<IAppWidgetProviderInfo> info = IAppWidgetProviderInfo::Probe(obj);
            Log.d(tag, "   label=\"" + info.label + "\" previewImage=" + info.previewImage
                    + " resizeMode=" + info.resizeMode + " configure=" + info.configure
                    + " initialLayout=" + info.initialLayout
                    + " minWidth=" + info.minWidth + " minHeight=" + info.minHeight);
        }
        else if (IResolveInfo::Probe(obj) != NULL) {
            AutoPtr<IResolveInfo> info = IResolveInfo::Probe(obj);
            Log.d(tag, "   label=\"" + info.loadLabel(mPackageManager) + "\" icon="
                    + info.icon);
        }
    }
}

ECode AppsCustomizePagedView::Surrender()
{
    // TODO: If we are in the middle of any process (ie. for holographic outlines, etc) we
    // should stop this now.

    // Stop all background tasks
    return CancelAllTasks();
}

ECode AppsCustomizePagedView::IconPressed(
    /* [in] */ IPagedViewIcon* icon)
{
    // Reset the previously pressed icon and store a reference to the pressed icon so that
    // we can reset it on return to Launcher (in Launcher.onResume())
    if (mPressedIcon != NULL) {
        mPressedIcon->ResetDrawableState();
    }
    mPressedIcon = icon;
    return NOERROR;
}

ECode AppsCustomizePagedView::ResetDrawableState()
{
    if (mPressedIcon != NULL) {
        mPressedIcon->ResetDrawableState();
        mPressedIcon = NULL;
    }
    return NOERROR;
}

ECode AppsCustomizePagedView::GetAssociatedLowerPageBound(
    /* [in] */ Int32 page,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 count;
    GetChildCount(&count);
    Int32 windowSize = Math::Min(count, sLookBehindPageCount + sLookAheadPageCount + 1);
    Int32 windowMinIndex = Math::Max(Math::Min(page - sLookBehindPageCount, count - windowSize), 0);
    *result = windowMinIndex;
    return NOERROR;
}

ECode AppsCustomizePagedView::GetAssociatedUpperPageBound(
    /* [in] */ Int32 page,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 count;
    GetChildCount(&count);
    Int32 windowSize = Math::Min(count, sLookBehindPageCount + sLookAheadPageCount + 1);
    Int32 windowMaxIndex = Math::Min(Math::Max(page + sLookAheadPageCount, windowSize - 1),
            count - 1);
    *result = windowMaxIndex;
    return NOERROR;
}

ECode AppsCustomizePagedView::GetCurrentPageDescription(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 page = (mNextPage != INVALID_PAGE) ? mNextPage : mCurrentPage;
    Int32 stringId = Elastos::Droid::Launcher2::R::string::default_scroll_format;
    Int32 count = 0;

    if (page < mNumAppsPages) {
        stringId = Elastos::Droid::Launcher2::R::string::apps_customize_apps_scroll_format;
        count = mNumAppsPages;
    }
    else {
        page -= mNumAppsPages;
        stringId = Elastos::Droid::Launcher2::R::string::apps_customize_widgets_scroll_format;
        count = mNumWidgetPages;
    }

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String str;
    context->GetString(stringId, &str);
    *result = String::Format(str, page + 1, count);
    return NOERROR;
}


} // namespace Launcher2
} // namespace Droid
} // namespace Elastos