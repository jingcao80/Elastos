
#include "elastos/droid/launcher2/CAppsCustomizePagedView.h"
#include "elastos/droid/launcher2/DragLayer.h"
#include "elastos/droid/launcher2/WidgetPreviewLoader.h"
#include "elastos/droid/launcher2/LauncherApplication.h"
#include "elastos/droid/launcher2/LauncherAnimUtils.h"
#include "elastos/droid/launcher2/Launcher.h"
#include "elastos/droid/launcher2/FocusHelper.h"
#include "elastos/droid/launcher2/PagedViewWidget.h"
#include "elastos/droid/launcher2/LauncherSettings.h"
#include "elastos/droid/launcher2/FastBitmapDrawable.h"
#include "elastos/droid/launcher2/PagedViewGridLayout.h"
#include "elastos/droid/launcher2/PendingAddItemInfo.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/view/ViewGroup.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/widget/GridLayout.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/Build.h"
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Service.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include "R.h"

using Elastos::Droid::Animation::IAnimatorSetBuilder;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::AppWidget::IAppWidgetHost;
using Elastos::Droid::AppWidget::IAppWidgetManager;
using Elastos::Droid::AppWidget::CAppWidgetManagerHelper;
using Elastos::Droid::AppWidget::IAppWidgetManagerHelper;
using Elastos::Droid::AppWidget::CAppWidgetHostViewHelper;
using Elastos::Droid::AppWidget::IAppWidgetHostViewHelper;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::View::Animation::CAccelerateInterpolator;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::Widget::ITabHost;
using Elastos::Droid::Widget::IGridLayout;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::IGridLayoutSpec;
using Elastos::Droid::Widget::GridLayout;
using Elastos::Droid::Widget::IGridLayoutLayoutParams;
using Elastos::Droid::Widget::CGridLayoutLayoutParams;
using Elastos::Core::IBoolean;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::Logging::Slogger;

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
            mGeneratedImages->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj1;
                mItems->Get(i, (IInterface**)&obj1);
                AutoPtr<IInterface> obj2;
                mGeneratedImages->Get(i, (IInterface**)&obj2);
                AutoPtr<IBitmap> map = IBitmap::Probe(obj2);
                mWidgetPreviewLoader->RecycleBitmap(obj1, map);
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
    : mDataType(ty)
    , mPage(p)
    , mThreadPriority(IProcess::THREAD_PRIORITY_DEFAULT)
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
            (AsyncTaskPageData*)IAppsCustomizePagedViewAsyncTaskPageData::Probe((*params)[0]);
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
            (AsyncTaskPageData*)IAppsCustomizePagedViewAsyncTaskPageData::Probe(result);
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

CAppsCustomizePagedView::MyRunnable::MyRunnable(
    /* [in] */ CAppsCustomizePagedView* host)
    : mHost(host)
{
}

ECode CAppsCustomizePagedView::MyRunnable::Run()
{
    return mHost->ShowAllAppsCling();
}

CAppsCustomizePagedView::MyRunnable2::MyRunnable2(
    /* [in] */ CAppsCustomizePagedView* host,
    /* [in] */ PendingAddWidgetInfo* info,
    /* [in] */ IBundle* options)
    : mHost(host)
    , mInfo(info)
    , mOptions(options)
{
}

ECode CAppsCustomizePagedView::MyRunnable2::Run()
{
    AutoPtr<ILauncherAppWidgetHost> launcherAppWidgetHost;
    mHost->mLauncher->GetAppWidgetHost((ILauncherAppWidgetHost**)&launcherAppWidgetHost);
    IAppWidgetHost::Probe(launcherAppWidgetHost)->AllocateAppWidgetId(&(mHost->mWidgetLoadingId));

    AutoPtr<IAppWidgetManager> manager;
    AutoPtr<IAppWidgetManagerHelper> helper;
    CAppWidgetManagerHelper::AcquireSingleton((IAppWidgetManagerHelper**)&helper);
    helper->GetInstance(IContext::Probe(mHost->mLauncher), (IAppWidgetManager**)&manager);

    AutoPtr<IUserHandle> handle;
    mInfo->mInfo->GetProfile((IUserHandle**)&handle);
    Boolean res;
    manager->BindAppWidgetIdIfAllowed(
            mHost->mWidgetLoadingId, handle,
            mInfo->mComponentName, mOptions, &res);
    if (res) {
        mHost->mWidgetCleanupState = WIDGET_BOUND;
    }
    return NOERROR;
}

CAppsCustomizePagedView::MyRunnable3::MyRunnable3(
    /* [in] */ CAppsCustomizePagedView* host,
    /* [in] */ PendingAddWidgetInfo* info,
    /* [in] */ IAppWidgetProviderInfo* pInfo)
    : mHost(host)
    , mInfo(info)
    , mPInfo(pInfo)
{
}

ECode CAppsCustomizePagedView::MyRunnable3::Run()
{
    if (mHost->mWidgetCleanupState != WIDGET_BOUND) {
        return NOERROR;
    }
    AutoPtr<ILauncherAppWidgetHost> launcherAppWidgetHost;

    mHost->mLauncher->GetAppWidgetHost((ILauncherAppWidgetHost**)&launcherAppWidgetHost);
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    AutoPtr<IAppWidgetHostView> hostView;
    IAppWidgetHost::Probe(launcherAppWidgetHost)->CreateView(context,
            mHost->mWidgetLoadingId, mPInfo, (IAppWidgetHostView**)&hostView);
    mInfo->mBoundWidget = hostView;
    mHost->mWidgetCleanupState = WIDGET_INFLATED;
    IView::Probe(hostView)->SetVisibility(INVISIBLE);

    AutoPtr<IWorkspace> workspace;
    mHost->mLauncher->GetWorkspace((IWorkspace**)&workspace);
    AutoPtr<ArrayOf<Int32> > unScaledSize;
    workspace->EstimateItemSize(mInfo->mSpanX,
            mInfo->mSpanY, IItemInfo::Probe(mInfo), FALSE,
            (ArrayOf<Int32>**)&unScaledSize);

    // We want the first widget layout to be the correct size. This will be important
    // for width size reporting to the AppWidgetManager.
    AutoPtr<DragLayer::DragLayerLayoutParams> lp = new DragLayer::DragLayerLayoutParams();
    lp->constructor((*unScaledSize)[0], (*unScaledSize)[1]);
    lp->SetX(0);
    lp->SetY(0);
    lp->mCustomPosition = TRUE;
    IView::Probe(hostView)->SetLayoutParams(IViewGroupLayoutParams::Probe(lp));
    AutoPtr<IDragLayer> dragLayer;
    mHost->mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
    return IViewGroup::Probe(dragLayer)->AddView(IView::Probe(hostView));
}

CAppsCustomizePagedView::MyRunnable4::MyRunnable4(
    /* [in] */ CAppsCustomizePagedView* host)
    : mHost(host)
{
}

ECode CAppsCustomizePagedView::MyRunnable4::Run()
{
    // We don't enter spring-loaded mode if the drag has been cancelled
    AutoPtr<IDragController> dragController;
    mHost->mLauncher->GetDragController((IDragController**)&dragController);
    Boolean res;
    dragController->IsDragging(&res);
    if (res) {
        // Dismiss the cling
Slogger::E("CAppsCustomizePagedView", "===================MyRunnable4 call DismissAllAppsCling");
        mHost->mLauncher->DismissAllAppsCling(NULL);

        // Reset the alpha on the dragged icon before we drag
        mHost->ResetDrawableState();

        // Go into spring loaded mode (must happen before we startDrag())
        mHost->mLauncher->EnterSpringLoadedDragMode();
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CAppsCustomizePagedView::MyAsyncTaskCallback, Object,
        IAppsCustomizePagedViewAsyncTaskCallback);

CAppsCustomizePagedView::MyAsyncTaskCallback::MyAsyncTaskCallback(
    /* [in] */ CAppsCustomizePagedView* host,
    /* [in] */ Int32 sleepMs)
    : mHost(host)
    , mSleepMs(sleepMs)
{
}

ECode CAppsCustomizePagedView::MyAsyncTaskCallback::Run(
    /* [in] */ IAppsCustomizePagedViewAppsCustomizeAsyncTask* task,
    /* [in] */ IAppsCustomizePagedViewAsyncTaskPageData* data)
{
    //try {
    //try {
    Thread::Sleep(mSleepMs);
    //} catch (Exception e) {}
    mHost->LoadWidgetPreviewsInBackground(task, data);
    //} finally {
    if (((AppsCustomizeAsyncTask*)task)->IsCancelled()) {
        data->Cleanup(TRUE);
    }
    //}
    return NOERROR;
}

CAR_INTERFACE_IMPL(CAppsCustomizePagedView::MyAsyncTaskCallback2, Object,
        IAppsCustomizePagedViewAsyncTaskCallback);

CAppsCustomizePagedView::MyAsyncTaskCallback2::MyAsyncTaskCallback2(
    /* [in] */ CAppsCustomizePagedView* host)
    : mHost(host)
{
}

ECode CAppsCustomizePagedView::MyAsyncTaskCallback2::Run(
    /* [in] */ IAppsCustomizePagedViewAppsCustomizeAsyncTask* task,
    /* [in] */ IAppsCustomizePagedViewAsyncTaskPageData* data)
{
    mHost->mRunningTasks->Remove(task);
    if (((AppsCustomizeAsyncTask*)task)->IsCancelled()) return NOERROR;
    // do cleanup inside onSyncWidgetPageItems
    mHost->OnSyncWidgetPageItems(data);
    return NOERROR;
}

CAppsCustomizePagedView::MyRunnable5::MyRunnable5(
    /* [in] */ CAppsCustomizePagedView* host,
    /* [in] */ Int32 cellWidth,
    /* [in] */ Int32 cellHeight,
    /* [in] */ IPagedViewGridLayout* layout,
    /* [in] */ Boolean immediate,
    /* [in] */ Int32 page,
    /* [in] */ IArrayList* items)
    : mHost(host)
    , mCellWidth(cellWidth)
    , mCellHeight(cellHeight)
    , mLayout(layout)
    , mImmediate(immediate)
    , mPage(page)
    , mItems(items)
{
}

ECode CAppsCustomizePagedView::MyRunnable5::Run()
{
    // Load the widget previews
    Int32 maxPreviewWidth = mCellWidth;
    Int32 maxPreviewHeight = mCellHeight;
    Int32 count;
    IViewGroup::Probe(mLayout)->GetChildCount(&count);
    if (count > 0) {
        AutoPtr<IView> view;
        IViewGroup::Probe(mLayout)->GetChildAt(0, (IView**)&view);
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
                new AsyncTaskPageData(mPage, mItems, maxPreviewWidth,
                maxPreviewHeight, NULL, NULL, mHost->mWidgetPreviewLoader);
        mHost->LoadWidgetPreviewsInBackground(NULL, data);
        mHost->OnSyncWidgetPageItems(data);
    }
    else {
        if (mHost->mInTransition) {
            mHost->mDeferredPrepareLoadWidgetPreviewsTasks->Add(TO_IINTERFACE(this));
        }
        else {
            mHost->PrepareLoadWidgetPreviewsTask(mPage, mItems,
                    maxPreviewWidth, maxPreviewHeight, mHost->mWidgetCountX);
        }
    }
    return mLayout->SetOnLayoutListener(NULL);
}

const String CAppsCustomizePagedView::TAG("AppsCustomizePagedView");

Float CAppsCustomizePagedView::CAMERA_DISTANCE = 6500;
Float CAppsCustomizePagedView::TRANSITION_SCALE_FACTOR = 0.74f;
Float CAppsCustomizePagedView::TRANSITION_PIVOT = 0.65f;
Float CAppsCustomizePagedView::TRANSITION_MAX_ROTATION = 22;
Boolean CAppsCustomizePagedView::PERFORM_OVERSCROLL_ROTATION = TRUE;

const Int32 CAppsCustomizePagedView::sPageSleepDelay = 200;

const Int32 CAppsCustomizePagedView::WIDGET_NO_CLEANUP_REQUIRED = -1;
const Int32 CAppsCustomizePagedView::WIDGET_PRELOAD_PENDING = 0;
const Int32 CAppsCustomizePagedView::WIDGET_BOUND = 1;
const Int32 CAppsCustomizePagedView::WIDGET_INFLATED = 2;

const Int32 CAppsCustomizePagedView::sLookBehindPageCount = 2;
const Int32 CAppsCustomizePagedView::sLookAheadPageCount = 2;

CAR_INTERFACE_IMPL_7(CAppsCustomizePagedView, PagedViewWithDraggableItems,
        IAppsCustomizePagedView, IViewOnClickListener, IViewOnKeyListener,
        IDragSource, IPagedViewIconPressedCallback,
        IPagedViewWidgetShortPressListener, ILauncherTransitionable);

CAR_OBJECT_IMPL(CAppsCustomizePagedView);

CAppsCustomizePagedView::CAppsCustomizePagedView()
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

    //assert(0);
Slogger::E("CAppsCustomizePagedView", "====TODO===========CAppsCustomizePagedView::CAppsCustomizePagedView()");

    // CBitmapCache::New((IBitmapCache**)&mCachedShortcutPreviewBitmap);
    // CPaintCache::New((IPaintCache**)&mCachedShortcutPreviewPaint);
    // CCanvasCache::New((ICanvasCache**)&mCachedShortcutPreviewCanvas);

    // CCanvasCache::New((ICanvasCache**)&mCachedAppWidgetPreviewCanvas);
    // CRectCache::New((IRectCache**)&mCachedAppWidgetPreviewSrcRect);
    // CRectCache::New((IRectCache**)&mCachedAppWidgetPreviewDestRect);
    // CPaintCache::New((IPaintCache**)&mCachedAppWidgetPreviewPaint);
}

ECode CAppsCustomizePagedView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    PagedViewWithDraggableItems::constructor(context, attrs);

    LayoutInflater::From(context, (ILayoutInflater**)&mLayoutInflater);
    context->GetPackageManager((IPackageManager**)&mPackageManager);
    CArrayList::New((IArrayList**)&mApps);
    CArrayList::New((IArrayList**)&mWidgets);

    AutoPtr<IContext> ctx;
    context->GetApplicationContext((IContext**)&ctx);
    ILauncherApplication::Probe(ctx)->GetIconCache((IIconCache**)&mIconCache);
    CCanvas::New((ICanvas**)&mCanvas);
    CArrayList::New((IArrayList**)&mRunningTasks);

    // Save the default widget preview background
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(Elastos::Droid::Launcher2::R::styleable::AppsCustomizePagedView),
            ArraySize(Elastos::Droid::Launcher2::R::styleable::AppsCustomizePagedView));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, 0, 0, (ITypedArray**)&a);

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
    AutoPtr<IContext> _context;
    GetContext((IContext**)&_context);
    mWidgetSpacingLayout = new PagedViewCellLayout();
    mWidgetSpacingLayout->constructor(_context);

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

ECode CAppsCustomizePagedView::Init()
{
    PagedViewWithDraggableItems::Init();
    mCenterPagesVertically = FALSE;

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> r;
    context->GetResources((IResources**)&r);
    Int32 num;
    r->GetInteger(
            Elastos::Droid::Launcher2::R::integer::config_appsCustomizeDragSlopeThreshold,
            &num);
    return SetDragSlopeThreshold(num / 100.0f);
}

Int32 CAppsCustomizePagedView::GetMiddleComponentIndexOnCurrentPage()
{
    Int32 i = -1;
    Int32 count = GetPageCount();
    if (count > 0) {
        Int32 currentPage = GetCurrentPage();
        if (currentPage < mNumAppsPages) {
            AutoPtr<IView> view;
            GetPageAt(currentPage, (IView**)&view);
            AutoPtr<IPagedViewCellLayout> layout = IPagedViewCellLayout::Probe(view);
            AutoPtr<IPagedViewCellLayoutChildren> childrenLayout;
            layout->GetChildrenLayout((IPagedViewCellLayoutChildren**)&childrenLayout);
            Int32 numItemsPerPage = mCellCountX * mCellCountY;
            Int32 childCount;
            IViewGroup::Probe(childrenLayout)->GetChildCount(&childCount);
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
            IViewGroup::Probe(layout)->GetChildCount(&childCount);
            if (childCount > 0) {
                i = numApps +
                    ((currentPage - mNumAppsPages) * numItemsPerPage) + (childCount / 2);
            }
        }
    }
    return i;
}

ECode CAppsCustomizePagedView::GetSaveInstanceStateIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);

    if (mSaveInstanceStateItemIndex == -1) {
        mSaveInstanceStateItemIndex = GetMiddleComponentIndexOnCurrentPage();
    }
    *index = mSaveInstanceStateItemIndex;
    return NOERROR;
}

ECode CAppsCustomizePagedView::GetPageForComponent(
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

ECode CAppsCustomizePagedView::RestorePageForIndex(
    /* [in] */ Int32 index)
{
    if (index < 0) return NOERROR;
    mSaveInstanceStateItemIndex = index;
    return NOERROR;
}

void CAppsCustomizePagedView::UpdatePageCounts()
{
    Int32 size;
    mWidgets->GetSize(&size);
    mNumWidgetPages = (Int32)Elastos::Core::Math::Ceil(size /
            (Float)(mWidgetCountX * mWidgetCountY));
    mApps->GetSize(&size);
    mNumAppsPages = (Int32)Elastos::Core::Math::Ceil((Float)size / (mCellCountX * mCellCountY));
}

ECode CAppsCustomizePagedView::OnDataReady(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (mWidgetPreviewLoader == NULL) {
        mWidgetPreviewLoader = new WidgetPreviewLoader();
        ((WidgetPreviewLoader*)mWidgetPreviewLoader.Get())->constructor(mLauncher);
    }

    // Note that we transpose the counts in portrait so that we get a similar layout
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IConfiguration> configuration;
    resources->GetConfiguration((IConfiguration**)&configuration);
    Int32 orientation;
    configuration->GetOrientation(&orientation);

    Boolean isLandscape = orientation == IConfiguration::ORIENTATION_LANDSCAPE;
    Int32 maxCellCountX = Elastos::Core::Math::INT32_MAX_VALUE;
    Int32 maxCellCountY = Elastos::Core::Math::INT32_MAX_VALUE;

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
        maxCellCountX = Elastos::Core::Math::Min(maxCellCountX, mMaxAppCellCountX);
    }
    // Temp hack for now: only use the max cell count Y for widget layout
    Int32 maxWidgetCellCountY = maxCellCountY;
    if (mMaxAppCellCountY > -1) {
        maxWidgetCellCountY = Elastos::Core::Math::Min(maxWidgetCellCountY, mMaxAppCellCountY);
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
    Int32 mwidth;
    GetMeasuredWidth(&mwidth);
    Int32 widthSpec = View::MeasureSpec::MakeMeasureSpec(mwidth, View::MeasureSpec::AT_MOST);
    Int32 mheight;
    GetMeasuredHeight(&mheight);
    Int32 heightSpec = View::MeasureSpec::MakeMeasureSpec(mheight, View::MeasureSpec::AT_MOST);
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
    InvalidatePageData(Elastos::Core::Math::Max(0, page), hostIsTransitioning);

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

ECode CAppsCustomizePagedView::ShowAllAppsCling()
{
    if (!mHasShownAllAppsCling && IsDataReady()) {
        mHasShownAllAppsCling = TRUE;
        // Calculate the position for the cling punch through
        AutoPtr<ArrayOf<Int32> > offset = ArrayOf<Int32>::Alloc(2);
        AutoPtr<ArrayOf<Int32> > pos;
        mWidgetSpacingLayout->EstimateCellPosition(mClingFocusedX, mClingFocusedY,
                (ArrayOf<Int32>**)&pos);
        AutoPtr<IDragLayer> dragLayer;
        mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
        Float tmp;
        dragLayer->GetLocationInDragLayer(IView::Probe(this), offset, &tmp);
        // PagedViews are centered horizontally but top aligned
        // Note we have to shift the items up now that Launcher sits under the status bar
        Int32 width;
        GetMeasuredWidth(&width);
        Int32 width2;
        mWidgetSpacingLayout->GetMeasuredWidth(&width2);
        (*pos)[0] += (width - width2) / 2 +
                (*offset)[0];
        Int32 top;
        IView::Probe(dragLayer)->GetPaddingTop(&top);
        (*pos)[1] += (*offset)[1] - top;
        return mLauncher->ShowFirstRunAllAppsCling(pos);
    }
    return NOERROR;
}

void CAppsCustomizePagedView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 width = View::MeasureSpec::GetSize(widthMeasureSpec);
    Int32 height = View::MeasureSpec::GetSize(heightMeasureSpec);
    if (!IsDataReady()) {
        Int32 size;
        mApps->GetSize(&size);
        Boolean tmp;
        mWidgets->IsEmpty(&tmp);
Slogger::E("CAppsCustomizePagedView","==================TO DO /*&& !tmp*/=================???");
        if (size > 0 /*&& !tmp*/) {
            SetDataIsReady();
            SetMeasuredDimension(width, height);
            OnDataReady(width, height);
        }
    }

    PagedViewWithDraggableItems::OnMeasure(widthMeasureSpec, heightMeasureSpec);
}

ECode CAppsCustomizePagedView::OnPackagesUpdated(
    /* [in] */ IArrayList* widgetsAndShortcuts)
{
    // Get the list of widgets and shortcuts
    mWidgets->Clear();
    Int32 size;
    widgetsAndShortcuts->GetSize(&size);
Slogger::E("CAppsCustomizePagedView", "============OnPackagesUpdated size=%d",size);
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
                Launcher::GetSpanForWidget(IContext::Probe(mLauncher), widget, (ArrayOf<Int32>**)&spanXY);
                AutoPtr<ArrayOf<Int32> > minSpanXY;
                Launcher::GetMinSpanForWidget(IContext::Probe(mLauncher), widget, (ArrayOf<Int32>**)&minSpanXY);
                Int32 minSpanX = Elastos::Core::Math::Min((*spanXY)[0], (*minSpanXY)[0]);
                Int32 minSpanY = Elastos::Core::Math::Min((*spanXY)[1], (*minSpanXY)[1]);
                Int32 x;
                LauncherModel::GetCellCountX(&x);
                Int32 y;
                LauncherModel::GetCellCountY(&y);
                if (minSpanX <= x && minSpanY <= y) {
                    mWidgets->Add(widget);
                }
                else {
                    AutoPtr<IComponentName> name;
                    widget->GetProvider((IComponentName**)&name);
                    Int32 width;
                    widget->GetMinWidth(&width);
                    Int32 height;
                    widget->GetMinHeight(&height);
                    StringBuilder sb;
                    sb += "Widget ";
                    sb += TO_STR(name);
                    sb += " can not fit on this device (";
                    sb += width;
                    sb += ", ";
                    sb += height;
                    sb += ")";
                    Slogger::E(TAG, sb.ToString());
                }
            }
            else {
                AutoPtr<IComponentName> name;
                widget->GetProvider((IComponentName**)&name);
                Int32 width;
                widget->GetMinWidth(&width);
                Int32 height;
                widget->GetMinHeight(&height);
                StringBuilder sb;
                sb += "Widget ";
                sb += TO_STR(name);
                sb += " has invalid dimensions (";
                sb += width;
                sb += ", ";
                sb += height;
                sb += ")";
                Slogger::E(TAG, sb.ToString());
            }
        }
        else {
            // just add shortcuts
            mWidgets->Add(o);
        }
    }
Slogger::E("CAppsCustomizePagedView", "===========OnPackagesUpdated");
    UpdatePageCountsAndInvalidateData();
    return NOERROR;
}

ECode CAppsCustomizePagedView::SetBulkBind(
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

void CAppsCustomizePagedView::UpdatePageCountsAndInvalidateData()
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

ECode CAppsCustomizePagedView::OnClick(
    /* [in] */ IView* v)
{
    Slogger::E("CAppsCustomizePagedView", " >> OnClick %s", TO_CSTR(v));
    // When we have exited all apps or are in transition, disregard clicks
    Boolean res;
    mLauncher->IsAllAppsVisible(&res);
    if (!res) return NOERROR;

    AutoPtr<IWorkspace> workspace;
    mLauncher->GetWorkspace((IWorkspace**)&workspace);
    workspace->IsSwitchingState(&res);
    if (res) return NOERROR;

    if (IPagedViewIcon::Probe(v) != NULL) {
        // Animate some feedback to the click
        AutoPtr<IInterface> obj;
        v->GetTag((IInterface**)&obj);
        AutoPtr<ApplicationInfo> appInfo = (ApplicationInfo*)IApplicationInfo::Probe(obj);

        // Lock the drawable state to pressed until we return to Launcher
        if (mPressedIcon != NULL) {
            mPressedIcon->LockDrawableState();
        }

        // NOTE: We want all transitions from launcher to act as if the wallpaper were enabled
        // to be consistent.  So re-enable the flag here, and we will re-disable it as necessary
        // when Launcher resumes and we are still in AllApps.
        mLauncher->UpdateWallpaperVisibility(TRUE);
        Boolean tmp;
        mLauncher->StartActivitySafely(v, appInfo->mIntent, obj, &tmp);

    }
    else if (IPagedViewWidget::Probe(v) != NULL) {
        // Let the user know that they have to long press to add a widget
        if (mWidgetInstructionToast != NULL) {
            mWidgetInstructionToast->Cancel();
        }
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        helper->MakeText(context,
                Elastos::Droid::Launcher2::R::string::long_press_widget_to_add,
                IToast::LENGTH_SHORT, (IToast**)&mWidgetInstructionToast);

        // Create a little animation to show that the widget can move
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        Int32 offsetY;
        resources->GetDimensionPixelSize(
                Elastos::Droid::Launcher2::R::dimen::dragViewOffsetY,
                &offsetY);
        AutoPtr<IView> view;
        v->FindViewById(
                Elastos::Droid::Launcher2::R::id::widget_preview,
                (IView**)&view);
        AutoPtr<IImageView> p = IImageView::Probe(view);
        AutoPtr<IAnimatorSet> bounce = LauncherAnimUtils::CreateAnimatorSet();

        AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
        (*array)[0] = offsetY;
        AutoPtr<IObjectAnimator> _tyuAnim = LauncherAnimUtils::OfFloat(IView::Probe(p),
                String("translationY"), array);
        AutoPtr<IValueAnimator> tyuAnim = IValueAnimator::Probe(_tyuAnim);
        tyuAnim->SetDuration(125);

        AutoPtr<ArrayOf<Float> > array2 = ArrayOf<Float>::Alloc(1);
        (*array2)[0] = 0.0f;
        AutoPtr<IObjectAnimator> _tydAnim = LauncherAnimUtils::OfFloat(IView::Probe(p),
                String("translationY"), array2);
        AutoPtr<IValueAnimator> tydAnim = IValueAnimator::Probe(_tydAnim);
        tydAnim->SetDuration(100);
        AutoPtr<IAnimatorSetBuilder> builder;
        bounce->Play(IAnimator::Probe(tyuAnim), (IAnimatorSetBuilder**)&builder);
        builder->Before(IAnimator::Probe(tydAnim));
        AutoPtr<IAccelerateInterpolator> tmp;
        CAccelerateInterpolator::New((IAccelerateInterpolator**)&tmp);
        IAnimator::Probe(bounce)->SetInterpolator(ITimeInterpolator::Probe(tmp));
        IAnimator::Probe(bounce)->Start();
    }
    return NOERROR;
}

ECode CAppsCustomizePagedView::OnKey(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = FocusHelper::HandleAppsCustomizeKeyEvent(v,  keyCode, event);
    return NOERROR;
}

ECode CAppsCustomizePagedView::DetermineDraggingStart(
    /* [in] */ IMotionEvent* ev)
{
    // Disable dragging by pulling an app down for now.
    return NOERROR;
}

void CAppsCustomizePagedView::BeginDraggingApplication(
    /* [in] */ IView* v)
{
    AutoPtr<IWorkspace> workspace;
    workspace->OnDragStartedWithItem(v);
    workspace->BeginDragShared(v, this);
}

ECode CAppsCustomizePagedView::GetDefaultOptionsForWidget(
    /* [in] */ ILauncher* launcher,
    /* [in] */ IPendingAddWidgetInfo* info,
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle);

    AutoPtr<IBundle> options;
    if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN_MR1) {
        PendingAddWidgetInfo* _info = (PendingAddWidgetInfo*)info;
        AppWidgetResizeFrame::GetWidgetSizeRanges(mLauncher,
                _info->mSpanX, _info->mSpanY, mTmpRect);

        AutoPtr<IAppWidgetHostViewHelper> helper;
        CAppWidgetHostViewHelper::AcquireSingleton((IAppWidgetHostViewHelper**)&helper);
        AutoPtr<IRect> padding;
        helper->GetDefaultPaddingForWidget(IContext::Probe(mLauncher),
                _info->mComponentName, NULL, (IRect**)&padding);

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
        padding->GetBottom(&bottom);
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
        mTmpRect->GetBottom(&bottom);
        options->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_MAX_HEIGHT,
                bottom - yPaddingDips);
    }
    *bundle = options;
    REFCOUNT_ADD(*bundle);
    return NOERROR;
}

void CAppsCustomizePagedView::PreloadWidget(
    /* [in] */ IPendingAddWidgetInfo* info)
{
    PendingAddWidgetInfo* _info = (PendingAddWidgetInfo*)info;
    AutoPtr<IAppWidgetProviderInfo> pInfo = _info->mInfo;
    AutoPtr<IBundle> options;
    GetDefaultOptionsForWidget(mLauncher, info, (IBundle**)&options);

    AutoPtr<IComponentName> name;
    pInfo->GetConfigure((IComponentName**)&name);
    if (name != NULL) {
        _info->mBindOptions = options;
        return;
    }

    mWidgetCleanupState = WIDGET_PRELOAD_PENDING;
    mBindWidgetRunnable = new MyRunnable2(this, _info, options);
    Boolean res;
    Post(mBindWidgetRunnable, &res);

    mInflateWidgetRunnable = new MyRunnable3(this, _info, pInfo);
    Post(mInflateWidgetRunnable, &res);
}

ECode CAppsCustomizePagedView::OnShortPress(
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
    mCreateWidgetInfo = new PendingAddWidgetInfo();
    ((PendingAddWidgetInfo*)mCreateWidgetInfo.Get())->constructor(
            (PendingAddWidgetInfo*)IPendingAddWidgetInfo::Probe(obj));
    PreloadWidget(mCreateWidgetInfo);
    return NOERROR;
}

void CAppsCustomizePagedView::CleanupWidgetPreloading(
    /* [in] */ Boolean widgetWasAdded)
{
    if (!widgetWasAdded) {
        // If the widget was not added, we may need to do further cleanup.
        AutoPtr<IPendingAddWidgetInfo> info = mCreateWidgetInfo;
        mCreateWidgetInfo = NULL;

        if (mWidgetCleanupState == WIDGET_PRELOAD_PENDING) {
            // We never did any preloading, so just remove pending callbacks to do so
            Boolean res;
            RemoveCallbacks(mBindWidgetRunnable, &res);
            RemoveCallbacks(mInflateWidgetRunnable, &res);
        }
        else if (mWidgetCleanupState == WIDGET_BOUND) {
             // Delete the widget id which was allocated
            if (mWidgetLoadingId != -1) {
                AutoPtr<ILauncherAppWidgetHost> appWidgetHost;
                mLauncher->GetAppWidgetHost((ILauncherAppWidgetHost**)&appWidgetHost);
                IAppWidgetHost::Probe(appWidgetHost)->DeleteAppWidgetId(mWidgetLoadingId);
            }

            // We never got around to inflating the widget, so remove the callback to do so.
            Boolean res;
            RemoveCallbacks(mInflateWidgetRunnable, &res);
        }
        else if (mWidgetCleanupState == WIDGET_INFLATED) {
            // Delete the widget id which was allocated
            if (mWidgetLoadingId != -1) {
                AutoPtr<ILauncherAppWidgetHost> appWidgetHost;
                mLauncher->GetAppWidgetHost((ILauncherAppWidgetHost**)&appWidgetHost);
                IAppWidgetHost::Probe(appWidgetHost)->DeleteAppWidgetId(mWidgetLoadingId);
            }

            // The widget was inflated and added to the DragLayer -- remove it.
            AutoPtr<IAppWidgetHostView> widget =
                    ((PendingAddWidgetInfo*)info.Get())->mBoundWidget;
            AutoPtr<IDragLayer> dragLayer;
            IViewGroup::Probe(dragLayer)->RemoveView(IView::Probe(widget));
        }
    }
    mWidgetCleanupState = WIDGET_NO_CLEANUP_REQUIRED;
    mWidgetLoadingId = -1;
    mCreateWidgetInfo = NULL;
    PagedViewWidget::ResetShortPressTarget();
}

ECode CAppsCustomizePagedView::CleanUpShortPress(
    /* [in] */ IView* v)
{
    if (!mDraggingWidget) {
        CleanupWidgetPreloading(FALSE);
    }
    return NOERROR;
}

Boolean CAppsCustomizePagedView::BeginDraggingWidget(
        /* [in] */ IView* v)
{
    mDraggingWidget = TRUE;
    // Get the widget preview as the drag representation
    AutoPtr<IView> view;
    v->FindViewById(
            Elastos::Droid::Launcher2::R::id::widget_preview,
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
    Float scale = 1.0f;
    AutoPtr<IPoint> previewPadding;

    if (IPendingAddWidgetInfo::Probe(createItemInfo) != NULL) {
        // This can happen in some weird cases involving multi-touch. We can't start dragging
        // the widget if this is null, so we break out.
        if (mCreateWidgetInfo == NULL) {
            return FALSE;
        }

        AutoPtr<IPendingAddWidgetInfo> createWidgetInfo = mCreateWidgetInfo;
        createItemInfo = IPendingAddItemInfo::Probe(createWidgetInfo);
        Int32 spanX = ((PendingAddWidgetInfo*)createItemInfo.Get())->mSpanX;
        Int32 spanY = ((PendingAddWidgetInfo*)createItemInfo.Get())->mSpanY;
        AutoPtr<IWorkspace> workspace;
        mLauncher->GetWorkspace((IWorkspace**)&workspace);
        AutoPtr<ArrayOf<Int32> > size;
        workspace->EstimateItemSize(spanX, spanY,
                IItemInfo::Probe(createWidgetInfo), TRUE, (ArrayOf<Int32>**)&size);

        AutoPtr<IDrawable> drawable;
        image->GetDrawable((IDrawable**)&drawable);
        AutoPtr<IFastBitmapDrawable> previewDrawable = IFastBitmapDrawable::Probe(drawable);
        Float minScale = 1.25f;
        Int32 maxWidth, maxHeight;
        Int32 width;
        IDrawable::Probe(previewDrawable)->GetIntrinsicWidth(&width);
        maxWidth = Elastos::Core::Math::Min((Int32)(width * minScale), (*size)[0]);
        Int32 height;
        IDrawable::Probe(previewDrawable)->GetIntrinsicHeight(&height);
        maxHeight = Elastos::Core::Math::Min((Int32)(height * minScale), (*size)[1]);

        AutoPtr<ArrayOf<Int32> > previewSizeBeforeScale = ArrayOf<Int32>::Alloc(1);
        mWidgetPreviewLoader->GenerateWidgetPreview(
                ((PendingAddWidgetInfo*)createWidgetInfo.Get())->mInfo, spanX,
                spanY, maxWidth, maxHeight, NULL,
                previewSizeBeforeScale, (IBitmap**)&preview);

        // Compare the size of the drag preview to the preview in the AppsCustomize tray
        Int32 previewWidth;
        mWidgetPreviewLoader->MaxWidthForWidgetPreview(spanX, &previewWidth);
        Int32 previewWidthInAppsCustomize = Elastos::Core::Math::Min((*previewSizeBeforeScale)[0],
                previewWidth);
        Int32 width2;
        preview->GetWidth(&width2);
        scale = previewWidthInAppsCustomize / (Float)width2;

        // The bitmap in the AppsCustomize tray is always the the same size, so there
        // might be extra pixels around the preview itself - this accounts for that
        Int32 intrinsicWidth;
        IDrawable::Probe(previewDrawable)->GetIntrinsicWidth(&intrinsicWidth);
        if (previewWidthInAppsCustomize < intrinsicWidth) {
            Int32 padding = (intrinsicWidth - previewWidthInAppsCustomize) / 2;
            CPoint::New(padding, 0, (IPoint**)&previewPadding);
        }
    }
    else {
        AutoPtr<IInterface> obj;
        v->GetTag((IInterface**)&obj);
        AutoPtr<PendingAddShortcutInfo> createShortcutInfo =
                (PendingAddShortcutInfo*)IPendingAddShortcutInfo::Probe(obj);
        // Widgets are only supported for current user, not for other profiles.
        // Hence use myUserHandle().
        AutoPtr<IUserHandle> user;
        Process::MyUserHandle((IUserHandle**)&user);
        AutoPtr<IDrawable> icon = ((IconCache*)mIconCache.Get())->GetFullResIcon(
                createShortcutInfo->mShortcutActivityInfo, user);
        Int32 intrinsicWidth;
        icon->GetIntrinsicWidth(&intrinsicWidth);
        Int32 intrinsicHeight;
        icon->GetIntrinsicHeight(&intrinsicHeight);
        AutoPtr<IBitmapHelper> helper;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
        helper->CreateBitmap(intrinsicWidth, intrinsicHeight, BitmapConfig_ARGB_8888, (IBitmap**)&preview);

        mCanvas->SetBitmap(preview);
        Int32 tmp;
        mCanvas->Save(&tmp);
        WidgetPreviewLoader::RenderDrawableToBitmap(icon, preview, 0, 0,
                intrinsicWidth, intrinsicHeight);
        mCanvas->Restore();
        mCanvas->SetBitmap(NULL);
        ((PendingAddItemInfo*)createItemInfo.Get())->mSpanX = 1;
        ((PendingAddItemInfo*)createItemInfo.Get())->mSpanY = 1;
    }

    // Don't clip alpha values for the drag outline if we're using the default widget preview
    Boolean clipAlpha;
    if (IPendingAddWidgetInfo::Probe(createItemInfo) != NULL) {
        AutoPtr<IAppWidgetProviderInfo> _info =
                ((PendingAddWidgetInfo*)IPendingAddWidgetInfo::Probe(createItemInfo))->mInfo;
        Int32 image;
        _info->GetPreviewImage(&image);
        if(image == 0) {
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

    AutoPtr<IBitmapHelper> helper;
    CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
    helper->CreateScaledBitmap(preview, width, height,
            FALSE, (IBitmap**)&outline);

    // Start the drag
    mLauncher->LockScreenOrientation();
    AutoPtr<IWorkspace> workspace;
    mLauncher->GetWorkspace((IWorkspace**)&workspace);
    workspace->OnDragStartedWithItem(createItemInfo, outline, clipAlpha);
    mDragController->StartDrag(IView::Probe(image), preview, IDragSource::Probe(this), createItemInfo,
            IDragController::DRAG_ACTION_COPY, previewPadding, scale);
    outline->Recycle();
    preview->Recycle();
    return TRUE;
}

ECode CAppsCustomizePagedView::BeginDragging(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (!PagedViewWithDraggableItems::BeginDragging(v)) {
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
    Boolean res;
    PostDelayed(run, 150, &res);

    *result = TRUE;
    return NOERROR;
}

void CAppsCustomizePagedView::EndDragging(
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

ECode CAppsCustomizePagedView::GetContent(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    *view = NULL;
    return NOERROR;
}

ECode CAppsCustomizePagedView::OnLauncherTransitionPrepare(
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

ECode CAppsCustomizePagedView::OnLauncherTransitionStart(
    /* [in] */ ILauncher* l,
    /* [in] */ Boolean animated,
    /* [in] */ Boolean toWorkspace)
{
    return NOERROR;
}

ECode CAppsCustomizePagedView::OnLauncherTransitionStep(
    /* [in] */ ILauncher* l,
    /* [in] */ float t)
{
    return NOERROR;
}

ECode CAppsCustomizePagedView::OnLauncherTransitionEnd(
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
        AutoPtr<IAppsCustomizePagedViewAsyncTaskPageData> d =
                IAppsCustomizePagedViewAsyncTaskPageData::Probe(obj);

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

ECode CAppsCustomizePagedView::OnDropCompleted(
    /* [in] */ IView* target,
    /* [in] */ IDropTargetDragObject* d,
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
            IViewGroup::Probe(workspace)->GetChildAt(currentScreen, (IView**)&view);
            AutoPtr<ICellLayout> layout = ICellLayout::Probe(view);
            AutoPtr<ItemInfo> itemInfo = (ItemInfo*)IItemInfo::Probe(((DragObject*)d)->mDragInfo);
            if (layout != NULL) {
                layout->CalculateSpans(itemInfo);
                layout->FindCellForSpan(NULL, itemInfo->mSpanX, itemInfo->mSpanY, &showOutOfSpaceMessage);
                showOutOfSpaceMessage = !showOutOfSpaceMessage;
            }
        }
        if (showOutOfSpaceMessage) {
            mLauncher->ShowOutOfSpaceMessage(FALSE);
        }

        ((DragObject*)d)->mDeferDragViewCleanupPostAnimation = FALSE;
    }
    CleanupWidgetPreloading(success);
    mDraggingWidget = FALSE;
    return NOERROR;
}

ECode CAppsCustomizePagedView::OnFlingToDeleteCompleted()
{
    // We just dismiss the drag when we fling, so cleanup here
    EndDragging(NULL, TRUE, TRUE);
    CleanupWidgetPreloading(FALSE);
    mDraggingWidget = FALSE;
    return NOERROR;
}

ECode CAppsCustomizePagedView::SupportsFlingToDelete(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = TRUE;
    return NOERROR;
}

ECode CAppsCustomizePagedView::OnDetachedFromWindow()
{
    PagedViewWithDraggableItems::OnDetachedFromWindow();
    CancelAllTasks();
    return NOERROR;
}

ECode CAppsCustomizePagedView::ClearAllWidgetPages()
{
    CancelAllTasks();
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> v;
        GetPageAt(i, (IView**)&v);
        if (IPagedViewGridLayout::Probe(v) != NULL) {
            AutoPtr<IPagedViewGridLayout> tmp = IPagedViewGridLayout::Probe(v);
            IPage::Probe(tmp)->RemoveAllViewsOnPage();
            AutoPtr<IBoolean> value = CoreUtils::Convert(TRUE);
            mDirtyPageContent->Set(i, TO_IINTERFACE(value));
        }
    }
    return NOERROR;
}

void CAppsCustomizePagedView::CancelAllTasks()
{
    // Clean up all the async tasks
    AutoPtr<IIterator> iter;
    mRunningTasks->GetIterator((IIterator**)&iter);
    Boolean res;
    while (iter->HasNext(&res), res) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        AutoPtr<AppsCustomizeAsyncTask> task =
                (AppsCustomizeAsyncTask*)IAppsCustomizePagedViewAppsCustomizeAsyncTask::Probe(obj);
        task->Cancel(FALSE);
        iter->Remove();
        AutoPtr<IBoolean> value = CoreUtils::Convert(TRUE);
        mDirtyPageContent->Set(task->mPage, TO_IINTERFACE(value));

        // We've already preallocated the views for the data to load into, so clear them as well
        AutoPtr<IView> v;
        GetPageAt(task->mPage, (IView**)&v);
        if (IPagedViewGridLayout::Probe(v) != NULL) {
            AutoPtr<IPagedViewGridLayout> tmp = IPagedViewGridLayout::Probe(v);
            IPage::Probe(tmp)->RemoveAllViewsOnPage();
        }
    }
    mDeferredSyncWidgetPageItems->Clear();
    mDeferredPrepareLoadWidgetPreviewsTasks->Clear();
}

ECode CAppsCustomizePagedView::SetContentType(
    /* [in] */ AppsCustomizePagedViewContentType type)
{
    if (type == AppsCustomizePagedViewContentType_Widgets) {
        InvalidatePageData(mNumAppsPages, TRUE);
    }
    else if (type == AppsCustomizePagedViewContentType_Applications) {
        InvalidatePageData(0, TRUE);
    }
    return NOERROR;
}

ECode CAppsCustomizePagedView::SnapToPage(
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
        iter->GetNext((IInterface**)&obj);
        AutoPtr<AppsCustomizeAsyncTask> task =
                (AppsCustomizeAsyncTask*)IAppsCustomizePagedViewAppsCustomizeAsyncTask::Probe(obj);
        Int32 pageIndex = task->mPage;
        if ((mNextPage > mCurrentPage && pageIndex >= mCurrentPage) ||
            (mNextPage < mCurrentPage && pageIndex <= mCurrentPage)) {
            Int32 prio = GetThreadPriorityForPage(pageIndex);
            task->SetThreadPriority(prio);
        }
        else {
            task->SetThreadPriority(IProcess::THREAD_PRIORITY_LOWEST);
        }
    }
    return NOERROR;
}

void CAppsCustomizePagedView::UpdateCurrentTab(
        /* [in] */ Int32 currentPage)
{
    AutoPtr<IAppsCustomizeTabHost> tabHost = GetTabHost();
    if (tabHost != NULL) {
        String tag;
        ITabHost::Probe(tabHost)->GetCurrentTabTag(&tag);
        if (!tag.IsNull()) {
            String type;
            tabHost->GetTabTagForContentType(AppsCustomizePagedViewContentType_Widgets, &type);
            if (currentPage >= mNumAppsPages && !tag.Equals(type)) {
                tabHost->SetCurrentTabFromContent(AppsCustomizePagedViewContentType_Widgets);
            }
            else if (currentPage < mNumAppsPages) {
                tabHost->GetTabTagForContentType(AppsCustomizePagedViewContentType_Applications, &type);
                if (tag.Equals(type)) {
                    tabHost->SetCurrentTabFromContent(AppsCustomizePagedViewContentType_Applications);
                }
            }
        }
    }
}

void CAppsCustomizePagedView::SetVisibilityOnChildren(
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

void CAppsCustomizePagedView::SetupPage(
    /* [in] */ IPagedViewCellLayout* layout)
{
    layout->SetCellCount(mCellCountX, mCellCountY);
    layout->SetGap(mPageLayoutWidthGap, mPageLayoutHeightGap);
    IView::Probe(layout)->SetPadding(mPageLayoutPaddingLeft, mPageLayoutPaddingTop,
            mPageLayoutPaddingRight, mPageLayoutPaddingBottom);

    // Note: We force a measure here to get around the fact that when we do layout calculations
    // immediately after syncing, we don't have a proper width.  That said, we already know the
    // expected page width, so we can actually optimize by hiding all the TextView-based
    // children that are expensive to measure, and let that happen naturally later.
    SetVisibilityOnChildren(IViewGroup::Probe(layout), IView::GONE);
    Int32 measuredWidth;
    GetMeasuredWidth(&measuredWidth);
    Int32 measuredHeight;
    GetMeasuredHeight(&measuredHeight);
    Int32 widthSpec = View::MeasureSpec::MakeMeasureSpec(measuredWidth, View::MeasureSpec::AT_MOST);
    Int32 heightSpec = View::MeasureSpec::MakeMeasureSpec(measuredHeight, View::MeasureSpec::AT_MOST);
    Int32 pageContentWidth;
    GetPageContentWidth(&pageContentWidth);
    IView::Probe(layout)->SetMinimumWidth(pageContentWidth);
    IView::Probe(layout)->Measure(widthSpec, heightSpec);
    SetVisibilityOnChildren(IViewGroup::Probe(layout), IView::VISIBLE);
}

ECode CAppsCustomizePagedView::SyncAppsPageItems(
    /* [in] */ Int32 page,
    /* [in] */ Boolean immediate)
{
    // ensure that we have the right number of items on the pages
    Boolean isRtl;
    IsLayoutRtl(&isRtl);
    Int32 numCells = mCellCountX * mCellCountY;
    Int32 startIndex = page * numCells;
    Int32 size;
    mApps->GetSize(&size);
    Int32 endIndex = Elastos::Core::Math::Min(startIndex + numCells, size);
    AutoPtr<IView> view;
    GetPageAt(page, (IView**)&view);
    AutoPtr<IPagedViewCellLayout> layout = IPagedViewCellLayout::Probe(view);

    IPage::Probe(layout)->RemoveAllViewsOnPage();
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
                IViewGroup::Probe(layout), FALSE, (IView**)&view);
        AutoPtr<IPagedViewIcon> icon = IPagedViewIcon::Probe(view);
        icon->ApplyFromApplicationInfo(info, TRUE, this);
        view->SetOnClickListener(this);
        view->SetOnLongClickListener(this);
        view->SetOnTouchListener(this);
        view->SetOnKeyListener(this);

        Int32 index = i - startIndex;
        Int32 x = index % mCellCountX;
        Int32 y = index / mCellCountX;
        if (isRtl) {
            x = mCellCountX - x - 1;
        }
        AutoPtr<PagedViewCellLayout::PagedViewCellLayoutLayoutParams> params =
                new PagedViewCellLayout::PagedViewCellLayoutLayoutParams();
        params->constructor(x, y, 1, 1);
        Boolean tmp;
        layout->AddViewToCellLayout(view, -1, i,
                IPagedViewCellLayoutLayoutParams::Probe(params), &tmp);

        items->Add(info);
        images->Add(((ApplicationInfo*)info.Get())->mIconBitmap);
    }

    EnableHwLayersOnVisiblePages();
    return NOERROR;
}

Int32 CAppsCustomizePagedView::GetWidgetPageLoadPriority(
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
    Int32 minPageDiff = Elastos::Core::Math::INT32_MAX_VALUE;
    AutoPtr<IIterator> iter;
    mRunningTasks->GetIterator((IIterator**)&iter);
    Boolean res;
    while (iter->HasNext(&res), res) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        AutoPtr<AppsCustomizeAsyncTask> task =
                (AppsCustomizeAsyncTask*)IAppsCustomizePagedViewAppsCustomizeAsyncTask::Probe(obj);
        minPageDiff = Elastos::Core::Math::Abs(task->mPage - toPage);
    }

    Int32 rawPageDiff = Elastos::Core::Math::Abs(page - toPage);
    return rawPageDiff - Elastos::Core::Math::Min(rawPageDiff, minPageDiff);
}

Int32 CAppsCustomizePagedView::GetThreadPriorityForPage(
    /* [in] */ Int32 page)
{
    // TODO-APPS_CUSTOMIZE: detect number of cores and set thread priorities accordingly below
    Int32 pageDiff = GetWidgetPageLoadPriority(page);
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

Int32 CAppsCustomizePagedView::GetSleepForPage(
    /* [in] */ Int32 page)
{
    Int32 pageDiff = GetWidgetPageLoadPriority(page);
    return Elastos::Core::Math::Max(0, pageDiff * sPageSleepDelay);
}

void CAppsCustomizePagedView::PrepareLoadWidgetPreviewsTask(
    /* [in] */ Int32 page,
    /* [in] */ IArrayList* widgets,
    /* [in] */ Int32 cellWidth,
    /* [in] */ Int32 cellHeight,
    /* [in] */ Int32 cellCountX)
{
    // Prune all tasks that are no longer needed
    AutoPtr<IIterator> iter;
    mRunningTasks->GetIterator((IIterator**)&iter);
    Boolean res;
    while (iter->HasNext(&res), res) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        AutoPtr<AppsCustomizeAsyncTask> task =
                (AppsCustomizeAsyncTask*)IAppsCustomizePagedViewAppsCustomizeAsyncTask::Probe(obj);
        Int32 taskPage = task->mPage;
        Int32 lowerPageBound;
        GetAssociatedLowerPageBound(mCurrentPage, &lowerPageBound);
        Int32 upperPageBound;
        GetAssociatedUpperPageBound(mCurrentPage, &upperPageBound);
        if (taskPage < lowerPageBound || taskPage > upperPageBound) {
            task->Cancel(FALSE);
            iter->Remove();
        }
        else {
            Int32 priorityForPage = GetThreadPriorityForPage(taskPage);
            task->SetThreadPriority(priorityForPage);
        }
    }

    // We introduce a slight delay to order the loading of side pages so that we don't thrash
    Int32 sleepMs = GetSleepForPage(page);
    AutoPtr<IAppsCustomizePagedViewAsyncTaskCallback> Callback1 =
            new MyAsyncTaskCallback(this, sleepMs);
    AutoPtr<IAppsCustomizePagedViewAsyncTaskCallback> Callback2 =
            new MyAsyncTaskCallback2(this);
    AutoPtr<IAppsCustomizePagedViewAsyncTaskPageData> pageData =
            new AsyncTaskPageData(page, widgets, cellWidth, cellHeight,
            Callback1, Callback2, mWidgetPreviewLoader);

    // Ensure that the task is appropriately prioritized and runs in parallel
    AutoPtr<AppsCustomizeAsyncTask> t =
            new AppsCustomizeAsyncTask(page, AsyncTaskPageDataType_LoadWidgetPreviewData);
    Int32 priorityForPage = GetThreadPriorityForPage(page);
    t->SetThreadPriority(priorityForPage);

    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
    array->Set(0, TO_IINTERFACE(pageData));
    ((AsyncTask*)t)->ExecuteOnExecutor(AsyncTask::THREAD_POOL_EXECUTOR, array);
    mRunningTasks->Add(TO_IINTERFACE(t));
}

void CAppsCustomizePagedView::SetupPage(
        /* [in] */ IPagedViewGridLayout* layout)
{
    IView::Probe(layout)->SetPadding(mPageLayoutPaddingLeft, mPageLayoutPaddingTop,
            mPageLayoutPaddingRight, mPageLayoutPaddingBottom);

    // Note: We force a measure here to get around the fact that when we do layout calculations
    // immediately after syncing, we don't have a proper width.
    Int32 measuredWidth;
    GetMeasuredWidth(&measuredWidth);
    Int32 widthSpec = View::MeasureSpec::MakeMeasureSpec(measuredWidth, View::MeasureSpec::AT_MOST);
    Int32 measuredHeight;
    GetMeasuredHeight(&measuredHeight);
    Int32 heightSpec = View::MeasureSpec::MakeMeasureSpec(measuredHeight, View::MeasureSpec::AT_MOST);
    Int32 width;
    GetPageContentWidth(&width);
    IView::Probe(layout)->SetMinimumWidth(width);
    IView::Probe(layout)->Measure(widthSpec, heightSpec);
}

ECode CAppsCustomizePagedView::SyncWidgetPageItems(
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
    for (Int32 i = offset; i < Elastos::Core::Math::Min(offset + numItemsPerPage, size); ++i) {
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
    IGridLayout::Probe(layout)->SetColumnCount(x);
    Int32 size2;
    items->GetSize(&size2);
    for (Int32 i = 0; i < size2; ++i) {
        AutoPtr<IInterface> rawInfo;
        items->Get(i, (IInterface**)&rawInfo);
        AutoPtr<PendingAddItemInfo> createItemInfo;
        AutoPtr<IView> _view;
        mLayoutInflater->Inflate(
                Elastos::Droid::Launcher2::R::layout::apps_customize_widget,
                IViewGroup::Probe(layout), FALSE, (IView**)&_view);
        AutoPtr<IPagedViewWidget> widget = IPagedViewWidget::Probe(_view);
        if (IAppWidgetProviderInfo::Probe(rawInfo) != NULL) {
            // Fill in the widget information
            AutoPtr<IAppWidgetProviderInfo> info = IAppWidgetProviderInfo::Probe(rawInfo);
            AutoPtr<PendingAddWidgetInfo> tmp = new PendingAddWidgetInfo();
            tmp->constructor(info, String(NULL), NULL);
            createItemInfo = tmp;

            // Determine the widget spans and min resize spans.
            AutoPtr<ArrayOf<Int32> > spanXY;
            Launcher::GetSpanForWidget(IContext::Probe(mLauncher), info, (ArrayOf<Int32>**)&spanXY);
            createItemInfo->mSpanX = (*spanXY)[0];
            createItemInfo->mSpanY = (*spanXY)[1];
            AutoPtr<ArrayOf<Int32> > minSpanXY;
            Launcher::GetMinSpanForWidget(IContext::Probe(mLauncher), info, (ArrayOf<Int32>**)&minSpanXY);
            createItemInfo->mMinSpanX = (*minSpanXY)[0];
            createItemInfo->mMinSpanY = (*minSpanXY)[1];

            widget->ApplyFromAppWidgetProviderInfo(info, -1, spanXY, mWidgetPreviewLoader);
            IView::Probe(widget)->SetTag(TO_IINTERFACE(createItemInfo));
            widget->SetShortPressListener(this);
        }
        else if (IResolveInfo::Probe(rawInfo) != NULL) {
            // Fill in the shortcuts information
            AutoPtr<IResolveInfo> info = IResolveInfo::Probe(rawInfo);
            AutoPtr<IActivityInfo> activityInfo;
            info->GetActivityInfo((IActivityInfo**)&activityInfo);
            AutoPtr<PendingAddShortcutInfo> tmp = new PendingAddShortcutInfo();
            tmp->constructor(activityInfo);
            createItemInfo = tmp;
            createItemInfo->mItemType = LauncherSettings::Favorites::ITEM_TYPE_SHORTCUT;

            String pname;
            IPackageItemInfo::Probe(activityInfo)->GetPackageName(&pname);
            String name;
            IPackageItemInfo::Probe(activityInfo)->GetName(&name);
            CComponentName::New(pname, name, (IComponentName**)&(createItemInfo->mComponentName));
            widget->ApplyFromResolveInfo(mPackageManager, info, mWidgetPreviewLoader);
            IView::Probe(widget)->SetTag(TO_IINTERFACE(createItemInfo));
        }
        IView::Probe(widget)->SetOnClickListener(this);
        IView::Probe(widget)->SetOnLongClickListener(this);
        IView::Probe(widget)->SetOnTouchListener(this);
        IView::Probe(widget)->SetOnKeyListener(this);

        // Layout each widget
        Int32 ix = i % mWidgetCountX;
        Int32 iy = i / mWidgetCountX;
        AutoPtr<IGridLayoutSpec> specx = GridLayout::GetSpec(iy, GridLayout::START);
        AutoPtr<IGridLayoutSpec> specy = GridLayout::GetSpec(ix, GridLayout::TOP);
        AutoPtr<IGridLayoutLayoutParams> layoutParams;
        CGridLayoutLayoutParams::New(specx, specy, (IGridLayoutLayoutParams**)&layoutParams);
        IViewGroupLayoutParams* vglp = IViewGroupLayoutParams::Probe(layoutParams);
        vglp->SetWidth(cellWidth);
        vglp->SetHeight(cellHeight);
        layoutParams->SetGravity(IGravity::TOP | IGravity::START);

        IViewGroupMarginLayoutParams* vgmlp = IViewGroupMarginLayoutParams::Probe(layoutParams);
        if (ix > 0) vgmlp->SetLeftMargin(mWidgetWidthGap);
        if (iy > 0) vgmlp->SetTopMargin(mWidgetHeightGap);
        IViewGroup::Probe(layout)->AddView(IView::Probe(widget), vglp);
    }

    // wait until a call on onLayout to start loading, because
    // PagedViewWidget.getPreviewSize() will return 0 if it hasn't been laid out
    // TODO: can we do a measure/layout immediately?
    AutoPtr<IRunnable> run = new MyRunnable5(this, cellWidth,
            cellHeight, layout, immediate, page, items);
    return layout->SetOnLayoutListener(run);
}

void CAppsCustomizePagedView::LoadWidgetPreviewsInBackground(
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
    AutoPtr<IArrayList> items = ((AsyncTaskPageData*)data)->mItems;
    AutoPtr<IArrayList> images = ((AsyncTaskPageData*)data)->mGeneratedImages;
    Int32 count;
    items->GetSize(&count);
    for (Int32 i = 0; i < count; ++i) {
        if (task != NULL) {
            // Ensure we haven't been cancelled yet
            if (((AsyncTask*)task)->IsCancelled()) break;
            // Before work on each item, ensure that this task is running at the correct
            // priority
            task->SyncThreadPriority();
        }

        AutoPtr<IInterface> obj;
        items->Get(i, (IInterface**)&obj);
        AutoPtr<IBitmap> bitmap;
        mWidgetPreviewLoader->GetPreview(obj, (IBitmap**)&bitmap);
        images->Add(TO_IINTERFACE(bitmap));
    }
}

void CAppsCustomizePagedView::OnSyncWidgetPageItems(
    /* [in] */ IAppsCustomizePagedViewAsyncTaskPageData* data)
{
    if (mInTransition) {
        mDeferredSyncWidgetPageItems->Add(data);
        return;
    }
    //try
    {
        Int32 page = ((AsyncTaskPageData*)data)->mPage;
        AutoPtr<IView> view;
        FAIL_GOTO(GetPageAt(page, (IView**)&view), FINALLY);
        AutoPtr<IPagedViewGridLayout> layout = IPagedViewGridLayout::Probe(view);

        AutoPtr<IArrayList> items = ((AsyncTaskPageData*)data)->mItems;
        Int32 count;
        FAIL_GOTO(items->GetSize(&count), FINALLY);
        for (Int32 i = 0; i < count; ++i) {
            AutoPtr<IView> view;
            FAIL_GOTO(IViewGroup::Probe(layout)->GetChildAt(i, (IView**)&view), FINALLY);
            AutoPtr<IPagedViewWidget> widget = IPagedViewWidget::Probe(view);
            if (widget != NULL) {
                AutoPtr<IInterface> obj;
                ((AsyncTaskPageData*)data)->mGeneratedImages->Get(i, (IInterface**)&obj);
                AutoPtr<IBitmap> preview = IBitmap::Probe(obj);
                AutoPtr<IFastBitmapDrawable> drawable = new FastBitmapDrawable();
                ((FastBitmapDrawable*)drawable.Get())->constructor(preview);
                FAIL_GOTO(widget->ApplyPreview(drawable, i), FINALLY)
            }
        }

        EnableHwLayersOnVisiblePages();

        // Update all thread priorities
        AutoPtr<IIterator> iter;
        mRunningTasks->GetIterator((IIterator**)&iter);
        Boolean res;
        while (iter->HasNext(&res), res) {
            AutoPtr<IInterface> obj;
            iter->GetNext((IInterface**)&obj);
            AutoPtr<AppsCustomizeAsyncTask> task =
                    (AppsCustomizeAsyncTask*)IAppsCustomizePagedViewAppsCustomizeAsyncTask::Probe(obj);
            Int32 pageIndex = task->mPage;
            Int32 priorityForPage = GetThreadPriorityForPage(pageIndex);
            FAIL_GOTO(task->SetThreadPriority(priorityForPage), FINALLY);
        }
    }
    //} finally {
FINALLY:
    data->Cleanup(FALSE);
    //}
}

ECode CAppsCustomizePagedView::SyncPages()
{
    RemoveAllViews();
    CancelAllTasks();
Slogger::E("CAppsCustomizePagedView", "=================== SyncPages 1 mNumWidgetPages=%d,mNumAppsPages=%d",mNumWidgetPages,mNumAppsPages);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    for (Int32 j = 0; j < mNumWidgetPages; ++j) {
        AutoPtr<PagedViewGridLayout> layout = new PagedViewGridLayout(
                context, mWidgetCountX, mWidgetCountY);
        SetupPage(layout);
        AutoPtr<IViewGroupLayoutParams> params;
        CViewGroupLayoutParams::New(
            IViewGroupLayoutParams::MATCH_PARENT,
            IViewGroupLayoutParams::MATCH_PARENT,
            (IViewGroupLayoutParams**)&params);
        AddView(layout, params);
    }

    for (Int32 i = 0; i < mNumAppsPages; ++i) {
        AutoPtr<IPagedViewCellLayout> layout = new PagedViewCellLayout();
        ((PagedViewCellLayout*)layout.Get())->constructor(context);
Slogger::E("CAppsCustomizePagedView", "=================== SetupPage ");
        SetupPage(layout);
Slogger::E("CAppsCustomizePagedView", "=================== AddView ");
        AddView(IView::Probe(layout));
    }
    return NOERROR;
}

ECode CAppsCustomizePagedView::SyncPageItems(
    /* [in] */ Int32 page,
    /* [in] */ Boolean immediate)
{
    if (page < mNumAppsPages) {
Slogger::E("CAppsCustomizePagedView", "=================== SyncPageItems 1 page=%d,immediate=%d",page,immediate);
        SyncAppsPageItems(page, immediate);
    }
    else {
Slogger::E("CAppsCustomizePagedView", "=================== SyncPageItems 2");
        SyncWidgetPageItems(page, immediate);
    }
    return NOERROR;
}

ECode CAppsCustomizePagedView::GetPageAt(
    /* [in] */ Int32 index,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    Int32 _index;
    IndexToPage(index, &_index);
    return GetChildAt(_index, view);
}

ECode CAppsCustomizePagedView::ScreenScrolled(
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
            Float scrollProgress = GetScrollProgress(screenCenter, v, i);

            Float interpolatedProgress;
            Float translationX;
            Float maxScrollProgress = Elastos::Core::Math::Max((Float)0, scrollProgress);
            Float minScrollProgress = Elastos::Core::Math::Min((Float)0, scrollProgress);

            if (isRtl) {
                Int32 width;
                v->GetMeasuredWidth(&width);
                translationX = maxScrollProgress * width;
                mZInterpolator->GetInterpolation(Elastos::Core::Math::Abs(maxScrollProgress),
                        &interpolatedProgress);
            }
            else {
                Int32 width;
                v->GetMeasuredWidth(&width);
                translationX = minScrollProgress * width;
                mZInterpolator->GetInterpolation(Elastos::Core::Math::Abs(minScrollProgress),
                        &interpolatedProgress);
            }
            Float scale = (1 - interpolatedProgress) +
                    interpolatedProgress * TRANSITION_SCALE_FACTOR;

            Float alpha;
            if (isRtl && (scrollProgress > 0)) {
                mAlphaInterpolator->GetInterpolation(1 - Elastos::Core::Math::Abs(maxScrollProgress),
                        &alpha);
            }
            else if (!isRtl && (scrollProgress < 0)) {
                mAlphaInterpolator->GetInterpolation(1 - Elastos::Core::Math::Abs(scrollProgress),
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
                Float xPivot = isRtl ? 1.0f - TRANSITION_PIVOT : TRANSITION_PIVOT;
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
                    v->SetRotationY(0.0f);
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

    EnableHwLayersOnVisiblePages();
    return NOERROR;
}

void CAppsCustomizePagedView::EnableHwLayersOnVisiblePages()
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
        if (!(leftScreen <= i && i <= rightScreen &&
                (i == forceDrawScreen || (ShouldDrawChild(layout))))) {
            layout->SetLayerType(LAYER_TYPE_NONE, NULL);
        }
    }

    for (Int32 i = 0; i < screenCount; i++) {
        AutoPtr<IView> layout;
        GetPageAt(i, (IView**)&layout);
        if (leftScreen <= i && i <= rightScreen &&
                (i == forceDrawScreen || (ShouldDrawChild(layout)))) {
            Int32 type;
            layout->GetLayerType(&type);
            if (type != LAYER_TYPE_HARDWARE) {
                layout->SetLayerType(LAYER_TYPE_HARDWARE, NULL);
            }
        }
    }
}

ECode CAppsCustomizePagedView::OverScroll(
    /* [in] */ Float amount)
{
    return AcceleratedOverScroll(amount);
}

ECode CAppsCustomizePagedView::GetPageContentWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);

    *width = mContentWidth;
    return NOERROR;
}

ECode CAppsCustomizePagedView::OnPageEndMoving()
{
    PagedViewWithDraggableItems::OnPageEndMoving();
    mForceDrawAllChildrenNextFrame = TRUE;
    // We reset the save index when we change pages so that it will be recalculated on next
    // rotation
    mSaveInstanceStateItemIndex = -1;
    return NOERROR;
}

ECode CAppsCustomizePagedView::Setup(
    /* [in] */ ILauncher* launcher,
    /* [in] */ IDragController* dragController)
{
    mLauncher = launcher;
    mDragController = dragController;
    return NOERROR;
}

void CAppsCustomizePagedView::InvalidateOnDataChange()
{
    Boolean res = IsDataReady();
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

ECode CAppsCustomizePagedView::SetApps(
    /* [in] */ IArrayList* list)
{
    mApps = list;
    AutoPtr<IComparator> comparator;
    LauncherModel::GetAppNameComparator((IComparator**)&comparator);
    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    helper->Sort(IList::Probe(mApps), comparator);
    UpdatePageCountsAndInvalidateData();
    return NOERROR;
}

void CAppsCustomizePagedView::AddAppsWithoutInvalidate(
        /* [in] */ IArrayList* list)
{
    // We add it in place, in alphabetical order
    Int32 count;
    list->GetSize(&count);
    for (Int32 i = 0; i < count; ++i) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        AutoPtr<IApplicationInfo> info = IApplicationInfo::Probe(obj);
        AutoPtr<IComparator> comparator;
        LauncherModel::GetAppNameComparator((IComparator**)&comparator);
        Int32 index;
        AutoPtr<ICollections> helper;
        CCollections::AcquireSingleton((ICollections**)&helper);
        helper->BinarySearch(IList::Probe(mApps), TO_IINTERFACE(info), comparator, &index);
        if (index < 0) {
            mApps->Add(-(index + 1), info);
        }
    }
}

ECode CAppsCustomizePagedView::AddApps(
    /* [in] */ IArrayList* list)
{
    AddAppsWithoutInvalidate(list);
    UpdatePageCountsAndInvalidateData();
    return NOERROR;
}

Int32 CAppsCustomizePagedView::FindAppByComponent(
    /* [in] */ IList* list,
    /* [in] */ IApplicationInfo* item)
{
    AutoPtr<IComponentName> removeComponent;
    ApplicationInfo* _item = (ApplicationInfo*)item;
    _item->mIntent->GetComponent(
            (IComponentName**)&removeComponent);
    Int32 length;
    list->GetSize(&length);
    for (Int32 i = 0; i < length; ++i) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        AutoPtr<ApplicationInfo> info = (ApplicationInfo*)IApplicationInfo::Probe(obj);

        Boolean res;
        IObject::Probe(info->mUser)->Equals(_item->mUser, &res);
        if (res) {
            AutoPtr<IComponentName> name;
            info->mIntent->GetComponent((IComponentName**)&name);
            IObject::Probe(name)->Equals(removeComponent, &res);
            if (res) {
                return i;
            }
        }
    }
    return -1;
}

void CAppsCustomizePagedView::RemoveAppsWithoutInvalidate(
    /* [in] */ IArrayList* list)
{
    // loop through all the apps and remove apps that have the same component
    Int32 length;
    list->GetSize(&length);
    for (Int32 i = 0; i < length; ++i) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        AutoPtr<IApplicationInfo> info = IApplicationInfo::Probe(obj);
        Int32 removeIndex = FindAppByComponent(IList::Probe(mApps), info);
        if (removeIndex > -1) {
            mApps->Remove(removeIndex);
        }
    }
}

ECode CAppsCustomizePagedView::RemoveApps(
    /* [in] */ IArrayList* appInfos)
{
    RemoveAppsWithoutInvalidate(appInfos);
    UpdatePageCountsAndInvalidateData();
    return NOERROR;
}

ECode CAppsCustomizePagedView::UpdateApps(
    /* [in] */ IArrayList* list)
{
    // We remove and re-add the updated applications list because it's properties may have
    // changed (ie. the title), and this will ensure that the items will be in their proper
    // place in the list.
    RemoveAppsWithoutInvalidate(list);
    AddAppsWithoutInvalidate(list);
    UpdatePageCountsAndInvalidateData();
    return NOERROR;
}

ECode CAppsCustomizePagedView::Reset()
{
    // If we have reset, then we should not continue to restore the previous state
    mSaveInstanceStateItemIndex = -1;

    AutoPtr<IAppsCustomizeTabHost> tabHost = GetTabHost();
    String tag;
    ITabHost::Probe(tabHost)->GetCurrentTabTag(&tag);
    if (tag != NULL) {
        String type;
        tabHost->GetTabTagForContentType(AppsCustomizePagedViewContentType_Applications, &type);
        if (!tag.Equals(type)) {
            tabHost->SetCurrentTabFromContent(AppsCustomizePagedViewContentType_Applications);
        }
    }

    if (mCurrentPage != 0) {
        InvalidatePageData(0);
    }
    return NOERROR;
}

AutoPtr<IAppsCustomizeTabHost> CAppsCustomizePagedView::GetTabHost()
{
    AutoPtr<IView> view;
Logger::E("CAppsCustomizePagedView", "=================== GetTabHost IActivity::Probe(mLauncher) = %p", IActivity::Probe(mLauncher));
    IActivity::Probe(mLauncher)->FindViewById(
            Elastos::Droid::Launcher2::R::id::apps_customize_pane,
            (IView**)&view);
Logger::E("CAppsCustomizePagedView", "=================== GetTabHost view = %p", view.Get());

    AutoPtr<IAppsCustomizeTabHost> tmp = IAppsCustomizeTabHost::Probe(view);
    return tmp;
}

ECode CAppsCustomizePagedView::DumpState()
{
    // TODO: Dump information related to current list of Applications, Widgets, etc.
    ApplicationInfo::DumpApplicationInfoList(TAG, String("mApps"), mApps);
    DumpAppWidgetProviderInfoList(TAG, String("mWidgets"), mWidgets);
    return NOERROR;
}

void CAppsCustomizePagedView::DumpAppWidgetProviderInfoList(
    /* [in] */ const String& tag,
    /* [in] */ const String& label,
    /* [in] */ IArrayList* list)
{
    Int32 size;
    list->GetSize(&size);

    StringBuilder sb;
    sb += label;
    sb += " size=";
    sb += size;
    Slogger::D(tag, sb.ToString());

    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);

        if (IAppWidgetProviderInfo::Probe(obj) != NULL) {
            AutoPtr<IAppWidgetProviderInfo> info = IAppWidgetProviderInfo::Probe(obj);

            StringBuilder sb;
            sb += "   label=\"";
            String label;
            info->GetLabel(&label);
            sb += label;
            sb += "\" previewImage=";
            Int32 image;
            info->GetPreviewImage(&image);
            sb += image;
            sb += " resizeMode=";
            Int32 mode;
            info->GetResizeMode(&mode);
            sb += mode;
            sb += " configure=";
            AutoPtr<IComponentName> name;
            info->GetConfigure((IComponentName**)&name);
            sb += TO_STR(name);
            sb += " initialLayout=";
            Int32 layout;
            info->GetInitialLayout(&layout);
            sb += layout;
            sb += " minWidth=";
            Int32 width;
            info->GetMinWidth(&width);
            sb += width;
            sb += " minHeight=";
            Int32 height;
            info->GetMinHeight(&height);
            sb += height;
            Slogger::D(tag, sb.ToString());
        }
        else if (IResolveInfo::Probe(obj) != NULL) {
            AutoPtr<IResolveInfo> info = IResolveInfo::Probe(obj);
            StringBuilder sb;
            sb += "   label=\"";
            AutoPtr<ICharSequence> label;
            info->LoadLabel(mPackageManager, (ICharSequence**)&label);
            String str;
            label->ToString(&str);
            sb += str;
            sb += "\" icon=";
            Int32 icon;
            info->GetIcon(&icon);
            sb += icon;
            Slogger::D(tag, sb.ToString());
        }
    }
}

ECode CAppsCustomizePagedView::Surrender()
{
    // TODO: If we are in the middle of any process (ie. for holographic outlines, etc) we
    // should stop this now.

    // Stop all background tasks
    CancelAllTasks();
    return NOERROR;
}

ECode CAppsCustomizePagedView::IconPressed(
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

ECode CAppsCustomizePagedView::ResetDrawableState()
{
    if (mPressedIcon != NULL) {
        mPressedIcon->ResetDrawableState();
        mPressedIcon = NULL;
    }
    return NOERROR;
}

ECode CAppsCustomizePagedView::GetAssociatedLowerPageBound(
    /* [in] */ Int32 page,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 count;
    GetChildCount(&count);
    Int32 windowSize = Elastos::Core::Math::Min(count, sLookBehindPageCount + sLookAheadPageCount + 1);
    Int32 windowMinIndex = Elastos::Core::Math::Max(Elastos::Core::Math::Min(page - sLookBehindPageCount, count - windowSize), 0);
    *result = windowMinIndex;
    return NOERROR;
}

ECode CAppsCustomizePagedView::GetAssociatedUpperPageBound(
    /* [in] */ Int32 page,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 count;
    GetChildCount(&count);
    Int32 windowSize = Elastos::Core::Math::Min(count, sLookBehindPageCount + sLookAheadPageCount + 1);
    Int32 windowMaxIndex = Elastos::Core::Math::Min(Elastos::Core::Math::Max(page + sLookAheadPageCount, windowSize - 1),
            count - 1);
    *result = windowMaxIndex;
    return NOERROR;
}

ECode CAppsCustomizePagedView::GetCurrentPageDescription(
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
    return result->AppendFormat(str, page + 1, count);
}


} // namespace Launcher2
} // namespace Droid
} // namespace Elastos