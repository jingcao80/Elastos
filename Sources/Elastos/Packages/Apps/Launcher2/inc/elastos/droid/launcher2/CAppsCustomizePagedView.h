//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef  __ELASTOS_DROID_LAUNCHER2_CAPPSCUSTOMIZEPAGEDVIEW_H__
#define  __ELASTOS_DROID_LAUNCHER2_CAPPSCUSTOMIZEPAGEDVIEW_H__

#include "_Elastos_Droid_Launcher2_CAppsCustomizePagedView.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/PagedViewWithDraggableItems.h"
#include "elastos/droid/launcher2/PendingAddItemInfo.h"
#include "elastos/droid/launcher2/Workspace.h"
#include "elastos/droid/launcher2/PagedViewCellLayout.h"
#include "elastos/droid/os/AsyncTask.h"
#include <elastos/core/Runnable.h>
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::AppWidget::IAppWidgetProviderInfo;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::Animation::IAccelerateInterpolator;
using Elastos::Droid::View::Animation::IDecelerateInterpolator;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IBitmapCache;
using Elastos::Core::Object;
using Elastos::Core::IRunnable;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * The data needed to perform either of the custom AsyncTasks.
 */
class AsyncTaskPageData
    : public Object
    , public IAppsCustomizePagedViewAsyncTaskPageData
{
public:
    CAR_INTERFACE_DECL();

    AsyncTaskPageData(
        /* [in] */ Int32 p,
        /* [in] */ IArrayList* l,
        /* [in] */ Int32 cw,
        /* [in] */ Int32 ch,
        /* [in] */ IAppsCustomizePagedViewAsyncTaskCallback* bgR,
        /* [in] */ IAppsCustomizePagedViewAsyncTaskCallback* postR,
        /* [in] */ IWidgetPreviewLoader* w);

    CARAPI Cleanup(
        /* [in] */ Boolean cancelled);

public:
    Int32 mPage;
    AutoPtr<IArrayList> mItems;
    AutoPtr<IArrayList> mSourceImages;
    AutoPtr<IArrayList> mGeneratedImages;
    Int32 mMaxImageWidth;
    Int32 mMaxImageHeight;
    AutoPtr<IAppsCustomizePagedViewAsyncTaskCallback> mDoInBackgroundCallback;
    AutoPtr<IAppsCustomizePagedViewAsyncTaskCallback> mPostExecuteCallback;
    AutoPtr<IWidgetPreviewLoader> mWidgetPreviewLoader;
};

/**
 * A generic template for an async task used in AppsCustomize.
 */
class AppsCustomizeAsyncTask
    : public AsyncTask
    , public IAppsCustomizePagedViewAppsCustomizeAsyncTask
{
public:
    CAR_INTERFACE_DECL();

    AppsCustomizeAsyncTask(
        /* [in] */ Int32 p,
        /* [in] */ AsyncTaskPageDataType ty);

    CARAPI SetThreadPriority(
        /* [in] */ Int32 p);

    CARAPI SyncThreadPriority();

protected:
    //@Override
    CARAPI DoInBackground(
        /* [in] */ ArrayOf<IInterface*>* params,
        /* [out] */ IInterface** obj);

    //@Override
    CARAPI OnPostExecute(
            /* [in] */ IInterface* result);

public:
    // The page that this async task is associated with
    AsyncTaskPageDataType mDataType;
    Int32 mPage;
    Int32 mThreadPriority;
};

/**
 * The Apps/Customize page that displays all the applications, widgets, and shortcuts.
 */
CarClass(CAppsCustomizePagedView)
    , public PagedViewWithDraggableItems
    , public IAppsCustomizePagedView
    , public IDragSource
    , public ILauncherTransitionable
{
private:
    class InnerListener
        : public Object
        , public IPagedViewIconPressedCallback
        , public IPagedViewWidgetShortPressListener
        , public IViewOnClickListener
        , public IViewOnKeyListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CAppsCustomizePagedView* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

        CARAPI OnKey(
            /* [in] */ IView* v,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

        CARAPI CleanUpShortPress(
            /* [in] */ IView* v);

        CARAPI OnShortPress(
            /* [in] */ IView* v);

        CARAPI IconPressed(
            /* [in] */ IPagedViewIcon* icon);

    private:
        CAppsCustomizePagedView* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ CAppsCustomizePagedView* host);

        CARAPI Run();

    private:
        CAppsCustomizePagedView* mHost;
    };

    class MyRunnable2
        : public Runnable
    {
    public:
        MyRunnable2(
            /* [in] */ CAppsCustomizePagedView* host,
            /* [in] */ PendingAddWidgetInfo* info,
            /* [in] */ IBundle* options);

        CARAPI Run();

    private:
        CAppsCustomizePagedView* mHost;
        AutoPtr<PendingAddWidgetInfo> mInfo;
        AutoPtr<IBundle> mOptions;
    };

    class MyRunnable3
        : public Runnable
    {
    public:
        MyRunnable3(
            /* [in] */ CAppsCustomizePagedView* host,
            /* [in] */ PendingAddWidgetInfo* info,
            /* [in] */ IAppWidgetProviderInfo* pInfo);

        CARAPI Run();

    private:
        CAppsCustomizePagedView* mHost;
        AutoPtr<PendingAddWidgetInfo> mInfo;
        AutoPtr<IAppWidgetProviderInfo> mPInfo;
    };

    class MyRunnable4
        : public Runnable
    {
    public:
        MyRunnable4(
            /* [in] */ CAppsCustomizePagedView* host);

        CARAPI Run();

    private:
        CAppsCustomizePagedView* mHost;
    };

    class MyAsyncTaskCallback
        : public Object
        , public IAppsCustomizePagedViewAsyncTaskCallback
    {
    public:
        CAR_INTERFACE_DECL();

        MyAsyncTaskCallback(
            /* [in] */ CAppsCustomizePagedView* host,
            /* [in] */ Int32 sleepMs);

        //@Override
        CARAPI Run(
            /* [in] */ IAppsCustomizePagedViewAppsCustomizeAsyncTask* task,
            /* [in] */ IAppsCustomizePagedViewAsyncTaskPageData* data);

    private:
        CAppsCustomizePagedView* mHost;
        Int32 mSleepMs;
    };

    class MyAsyncTaskCallback2
        : public Object
        , public IAppsCustomizePagedViewAsyncTaskCallback
    {
    public:
        CAR_INTERFACE_DECL();

        MyAsyncTaskCallback2(
            /* [in] */ CAppsCustomizePagedView* host);

        //@Override
        CARAPI Run(
            /* [in] */ IAppsCustomizePagedViewAppsCustomizeAsyncTask* task,
            /* [in] */ IAppsCustomizePagedViewAsyncTaskPageData* data);

    private:
        CAppsCustomizePagedView* mHost;
    };

    class MyRunnable5
        : public Runnable
    {
    public:
        MyRunnable5(
            /* [in] */ CAppsCustomizePagedView* host,
            /* [in] */ Int32 cellWidth,
            /* [in] */ Int32 cellHeight,
            /* [in] */ IPagedViewGridLayout* layout,
            /* [in] */ Boolean immediate,
            /* [in] */ Int32 page,
            /* [in] */ IArrayList* items);

        CARAPI Run();

    private:
        CAppsCustomizePagedView* mHost;
        Int32 mCellWidth;
        Int32 mCellHeight;
        AutoPtr<IPagedViewGridLayout> mLayout;
        Boolean mImmediate;
        Int32 mPage;
        AutoPtr<IArrayList> mItems;
    };

public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CAppsCustomizePagedView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /** Get the index of the item to restore to if we need to restore the current page. */
    CARAPI GetSaveInstanceStateIndex(
        /* [out] */ Int32* index);

    /** Returns the page in the current orientation which is expected to contain the specified
     *  item index. */
    CARAPI GetPageForComponent(
        /* [in] */ Int32 index,
        /* [out] */ Int32* page);

    /** Restores the page for an item at the specified index */
    CARAPI RestorePageForIndex(
        /* [in] */ Int32 index);

    CARAPI ShowAllAppsCling();

    CARAPI OnPackagesUpdated(
        /* [in] */ IArrayList* widgetsAndShortcuts);

    CARAPI SetBulkBind(
        /* [in] */ Boolean bulkBind);

    //@Override
    virtual CARAPI OnClick(
        /* [in] */ IView* v);

    virtual CARAPI OnKey(
        /* [in] */ IView* v,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI GetDefaultOptionsForWidget(
        /* [in] */ ILauncher* launcher,
        /* [in] */ IPendingAddWidgetInfo* info,
        /* [out] */ IBundle** bundle);

    //@Override
    virtual CARAPI OnShortPress(
        /* [in] */ IView* v);

    //@Override
    virtual CARAPI CleanUpShortPress(
        /* [in] */ IView* v);

    //@Override
    virtual CARAPI GetContent(
        /* [out] */ IView** view);

    CARAPI OnLauncherTransitionPrepare(
        /* [in] */ ILauncher* l,
        /* [in] */ Boolean animated,
        /* [in] */ Boolean toWorkspace);

    CARAPI OnLauncherTransitionStart(
        /* [in] */ ILauncher* l,
        /* [in] */ Boolean animated,
        /* [in] */ Boolean toWorkspace);

    CARAPI OnLauncherTransitionStep(
        /* [in] */ ILauncher* l,
        /* [in] */ Float t);

    CARAPI OnLauncherTransitionEnd(
        /* [in] */ ILauncher* l,
        /* [in] */ Boolean animated,
        /* [in] */ Boolean toWorkspace);

    //@Override
    virtual CARAPI OnDropCompleted(
        /* [in] */ IView* target,
        /* [in] */ IDropTargetDragObject* d,
        /* [in] */ Boolean isFlingToDelete,
        /* [in] */ Boolean success);

    //@Override
    virtual CARAPI OnFlingToDeleteCompleted();

    //@Override
    virtual CARAPI SupportsFlingToDelete(
        /* [out] */ Boolean* result);

    CARAPI ClearAllWidgetPages();

    CARAPI SetContentType(
        /* [in] */ AppsCustomizePagedViewContentType type);

    CARAPI SyncAppsPageItems(
        /* [in] */ Int32 page,
        /* [in] */ Boolean immediate);

    CARAPI SyncWidgetPageItems(
        /* [in] */ Int32 page,
        /* [in] */ Boolean immediate);

    //@Override
    virtual CARAPI SyncPages();

    //@Override
    virtual CARAPI SyncPageItems(
        /* [in] */ Int32 page,
        /* [in] */ Boolean immediate);

    // We want our pages to be z-ordered such that the further a page is to the left, the higher
    // it is in the z-order. This is important to insure touch events are handled correctly.
    CARAPI GetPageAt(
        /* [in] */ Int32 index,
        /* [out] */ IView** view);

    //@Override
    virtual CARAPI IndexToPage(
        /* [in] */ Int32 index,
        /* [out] */ Int32* page);

    /**
     * Used by the parent to get the content width to set the tab bar to
     * @return
     */
    CARAPI GetPageContentWidth(
        /* [out] */ Int32* width);

    /*
     * AllAppsView implementation
     */
    CARAPI Setup(
        /* [in] */ ILauncher* launcher,
        /* [in] */ IDragController* dragController);

    CARAPI SetApps(
        /* [in] */ IArrayList* list);

    CARAPI AddApps(
        /* [in] */ IArrayList* list);

    CARAPI RemoveApps(
        /* [in] */ IArrayList* appInfos);

    CARAPI UpdateApps(
        /* [in] */ IArrayList* list);

    CARAPI Reset();

    CARAPI DumpState();

    CARAPI Surrender();

    //@Override
    virtual CARAPI IconPressed(
        /* [in] */ IPagedViewIcon* icon);

    CARAPI ResetDrawableState();

protected:
    //@Override
    CARAPI Init();

    CARAPI OnDataReady(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    //@Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    /*
     * PagedViewWithDraggableItems implementation
     */
    //@Override
    CARAPI_(void) DetermineDraggingStart(
        /* [in] */ IMotionEvent* ev);

    //@Override
    CARAPI_(Boolean) BeginDragging(
        /* [in] */ IView* v);

    //@Override
    CARAPI OnDetachedFromWindow();

    CARAPI SnapToPage(
        /* [in] */ Int32 whichPage,
        /* [in] */ Int32 delta,
        /* [in] */ Int32 duration);

    // In apps customize, we have a scrolling effect which emulates pulling cards off of a stack.
    //@Override
    CARAPI ScreenScrolled(
        /* [in] */ Int32 screenCenter);

    CARAPI OverScroll(
        /* [in] */ Float amount);

    //@Override
    CARAPI OnPageEndMoving();

    CARAPI_(Int32) GetAssociatedLowerPageBound(
        /* [in] */ Int32 page);

    CARAPI_(Int32) GetAssociatedUpperPageBound(
        /* [in] */ Int32 page);

    //@Override
    CARAPI_(String) GetCurrentPageDescription();

private:
    /** Returns the item index of the center item on this page so that we can restore to this
     *  item index when we rotate. */
    CARAPI_(Int32) GetMiddleComponentIndexOnCurrentPage();

    CARAPI_(void) UpdatePageCounts();

    CARAPI_(void) UpdatePageCountsAndInvalidateData();

    CARAPI_(void) BeginDraggingApplication(
        /* [in] */ IView* v);

    CARAPI_(void) PreloadWidget(
        /* [in] */ IPendingAddWidgetInfo* info);

    CARAPI_(void) CleanupWidgetPreloading(
        /* [in] */ Boolean widgetWasAdded);


    CARAPI_(Boolean) BeginDraggingWidget(
        /* [in] */ IView* v);

    /**
     * Clean up after dragging.
     *
     * @param target where the item was dragged to (can be null if the item was flung)
     */
    CARAPI_(void) EndDragging(
        /* [in] */ IView* target,
        /* [in] */ Boolean isFlingToDelete,
        /* [in] */ Boolean success);

    CARAPI_(void) CancelAllTasks();

    CARAPI_(void) UpdateCurrentTab(
        /* [in] */ Int32 currentPage);

    /*
     * Apps PagedView implementation
     */
    CARAPI_(void) SetVisibilityOnChildren(
        /* [in] */ IViewGroup* layout,
        /* [in] */ Int32 visibility);

    CARAPI_(void) SetupPage(
        /* [in] */ IPagedViewCellLayout* layout);

    /**
     * A helper to return the priority for loading of the specified widget page.
     */
    CARAPI_(Int32) GetWidgetPageLoadPriority(
        /* [in] */ Int32 page);

    /**
     * Return the appropriate thread priority for loading for a given page (we give the current
     * page much higher priority)
     */
    CARAPI_(Int32) GetThreadPriorityForPage(
        /* [in] */ Int32 page);

    CARAPI_(Int32) GetSleepForPage(
        /* [in] */ Int32 page);
    /**
     * Creates and executes a new AsyncTask to load a page of widget previews.
     */
    CARAPI_(void) PrepareLoadWidgetPreviewsTask(
        /* [in] */ Int32 page,
        /* [in] */ IArrayList* widgets,
        /* [in] */ Int32 cellWidth,
        /* [in] */ Int32 cellHeight,
        /* [in] */ Int32 cellCountX);

    /*
     * Widgets PagedView implementation
     */
    CARAPI_(void) SetupPage(
        /* [in] */ IPagedViewGridLayout* layout);

    CARAPI_(void) LoadWidgetPreviewsInBackground(
        /* [in] */ IAppsCustomizePagedViewAppsCustomizeAsyncTask* task,
        /* [in] */ IAppsCustomizePagedViewAsyncTaskPageData* data);

    CARAPI_(void) OnSyncWidgetPageItems(
        /* [in] */ IAppsCustomizePagedViewAsyncTaskPageData* data);

    CARAPI_(void) EnableHwLayersOnVisiblePages();

    /**
     * We should call thise method whenever the core data changes (mApps, mWidgets) so that we can
     * appropriately determine when to invalidate the PagedView page data.  In cases where the data
     * has yet to be set, we can requestLayout() and wait for onDataReady() to be called in the
     * next onMeasure() pass, which will trigger an invalidatePageData() itself.
     */
    CARAPI_(void) InvalidateOnDataChange();

    CARAPI_(void) AddAppsWithoutInvalidate(
        /* [in] */ IArrayList* list);

    CARAPI_(Int32) FindAppByComponent(
        /* [in] */ IList* list,
        /* [in] */ IApplicationInfo* item);

    CARAPI_(void) RemoveAppsWithoutInvalidate(
        /* [in] */ IArrayList* list);

    CARAPI_(AutoPtr<IAppsCustomizeTabHost>) GetTabHost();

    CARAPI_(void) DumpAppWidgetProviderInfoList(
        /* [in] */ const String& tag,
        /* [in] */ const String& label,
        /* [in] */ IArrayList* list);

private:
    static const String TAG;

    // Refs
    ILauncher* mLauncher;
    AutoPtr<IDragController> mDragController;
    AutoPtr<ILayoutInflater> mLayoutInflater;
    AutoPtr<IPackageManager> mPackageManager;

    // Save and Restore
    Int32 mSaveInstanceStateItemIndex;
    AutoPtr<IPagedViewIcon> mPressedIcon;

    // Content
    AutoPtr<IArrayList> mApps;
    AutoPtr<IArrayList> mWidgets;

    // Cling
    Boolean mHasShownAllAppsCling;
    Int32 mClingFocusedX;
    Int32 mClingFocusedY;

    // Caching
    AutoPtr<ICanvas> mCanvas;
    AutoPtr<IIconCache> mIconCache;

    // Dimens
    Int32 mContentWidth;
    Int32 mMaxAppCellCountX, mMaxAppCellCountY;
    Int32 mWidgetCountX, mWidgetCountY;
    Int32 mWidgetWidthGap, mWidgetHeightGap;
    AutoPtr<PagedViewCellLayout> mWidgetSpacingLayout;
    Int32 mNumAppsPages;
    Int32 mNumWidgetPages;

    // Relating to the scroll and overscroll effects
    AutoPtr<Workspace::ZInterpolator> mZInterpolator;
    static Float CAMERA_DISTANCE;
    static Float TRANSITION_SCALE_FACTOR;
    static Float TRANSITION_PIVOT;
    static Float TRANSITION_MAX_ROTATION;
    static Boolean PERFORM_OVERSCROLL_ROTATION;
    AutoPtr<IAccelerateInterpolator> mAlphaInterpolator;
    AutoPtr<IDecelerateInterpolator> mLeftScreenAlphaInterpolator;

    // Previews & outlines
    AutoPtr<IArrayList> mRunningTasks;
    static const Int32 sPageSleepDelay;

    AutoPtr<IRunnable> mInflateWidgetRunnable;
    AutoPtr<IRunnable> mBindWidgetRunnable;
    static const Int32 WIDGET_NO_CLEANUP_REQUIRED;
    static const Int32 WIDGET_PRELOAD_PENDING;
    static const Int32 WIDGET_BOUND;
    static const Int32 WIDGET_INFLATED;
    Int32 mWidgetCleanupState;
    Int32 mWidgetLoadingId;
    AutoPtr<IPendingAddWidgetInfo> mCreateWidgetInfo;
    Boolean mDraggingWidget;

    AutoPtr<IToast> mWidgetInstructionToast;

    // Deferral of loading widget previews during launcher transitions
    Boolean mInTransition;
    AutoPtr<IArrayList> mDeferredSyncWidgetPageItems;
    AutoPtr<IArrayList> mDeferredPrepareLoadWidgetPreviewsTasks;

    AutoPtr<IRect> mTmpRect;

    // Used for drawing shortcut previews
    //AutoPtr<IBitmapCache> mCachedShortcutPreviewBitmap;
    //AutoPtr<IPaintCache> mCachedShortcutPreviewPaint;
    //AutoPtr<ICanvasCache> mCachedShortcutPreviewCanvas;

    // Used for drawing widget previews
    //AutoPtr<ICanvasCache> mCachedAppWidgetPreviewCanvas;
    //AutoPtr<IRectCache> mCachedAppWidgetPreviewSrcRect;
    //AutoPtr<IRectCache> mCachedAppWidgetPreviewDestRect;
    //AutoPtr<IPaintCache> mCachedAppWidgetPreviewPaint;

    AutoPtr<IWidgetPreviewLoader> mWidgetPreviewLoader;

    Boolean mInBulkBind;
    Boolean mNeedToUpdatePageCountsAndInvalidateData;

    /*
     * We load an extra page on each side to prevent flashes from scrolling and loading of the
     * widget previews in the background with the AsyncTasks.
     */
    static const Int32 sLookBehindPageCount;
    static const Int32 sLookAheadPageCount;
    AutoPtr<InnerListener> mInnerListener;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CAPPSCUSTOMIZEPAGEDVIEW_H__
