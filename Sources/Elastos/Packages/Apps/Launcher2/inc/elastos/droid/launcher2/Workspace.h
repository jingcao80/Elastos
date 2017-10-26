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

#ifndef  __ELASTOS_DROID_LAUNCHER2_WORKSPACE_H__
#define  __ELASTOS_DROID_LAUNCHER2_WORKSPACE_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/SmoothPagedView.h"
#include "elastos/droid/launcher2/LauncherAnimatorUpdateListener.h"
#include "elastos/droid/launcher2/SpringLoadedDragController.h"
#include "elastos/droid/launcher2/PendingAddItemInfo.h"
#include "elastos/droid/launcher2/CellLayout.h"
#include "elastos/droid/launcher2/DropTarget.h"
#include "elastos/droid/launcher2/ItemInfo.h"
#include <elastos/core/Runnable.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>
#include <elastos/core/Thread.h>

using Elastos::Droid::App::IWallpaperManager;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPointF;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Core::Runnable;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::Animation::IDecelerateInterpolator;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IViewGroupOnHierarchyChangeListener;
using Elastos::Core::Object;
using Elastos::Core::Thread;
using Elastos::Core::IRunnable;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * The workspace is a wide area with a wallpaper and a finite number of pages.
 * Each page contains a number of icons, folders or widgets the user can
 * interact with. A workspace is meant to be used with a fixed width only.
 */
class Workspace
    : public SmoothPagedView
    , public IWorkspace
    , public IDropTarget
    , public IDragSource
    , public IDragScroller
    , public ILauncherTransitionable
{
public:
    class InnerListener
        : public Object
        , public IViewOnTouchListener
        , public IDragControllerDragListener
    {
    public:
        CAR_INTERFACE_DECL();

        InnerListener(
            /* [in] */ Workspace* host);

        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

        CARAPI OnDragStart(
            /* [in] */ IDragSource* source,
            /* [in] */ IInterface* info,
            /* [in] */ Int32 dragAction);

        CARAPI OnDragEnd();

    private:
        Workspace* mHost;
    };

    class WallpaperOffsetInterpolator
        : public Object
    {
    public:
        WallpaperOffsetInterpolator(
            /* [in] */ Workspace* host);

        CARAPI SetOverrideHorizontalCatchupConstant(
            /* [in] */ Boolean override);

        CARAPI SetHorizontalCatchupConstant(
            /* [in] */ Float f);

        CARAPI SetVerticalCatchupConstant(
            /* [in] */ Float f);

        CARAPI ComputeScrollOffset(
            /* [out] */ Boolean* result);

        CARAPI GetCurrX(
            /* [out] */ Float* x);

        CARAPI GetFinalX(
            /* [out] */ Float* x);

        CARAPI GetCurrY(
            /* [out] */ Float* y);

        CARAPI GetFinalY(
            /* [out] */ Float* y);

        CARAPI SetFinalX(
            /* [in] */ Float x);

        CARAPI SetFinalY(
            /* [in] */ Float y);

        CARAPI JumpToFinal();

    public:
        Workspace* mHost;
        Float mFinalHorizontalWallpaperOffset;
        Float mFinalVerticalWallpaperOffset;
        Float mHorizontalWallpaperOffset;
        Float mVerticalWallpaperOffset;
        Int64 mLastWallpaperOffsetUpdateTime;
        Boolean mIsMovingFast;
        Boolean mOverrideHorizontalCatchupConstant;
        Float mHorizontalCatchupConstant;
        Float mVerticalCatchupConstant;
    };

    /*
     * This interpolator emulates the rate at which the perceived scale of an object changes
     * as its distance from a camera increases. When this interpolator is applied to a scale
     * animation on a view, it evokes the sense that the object is shrinking due to moving away
     * from the camera.
     */
    class ZInterpolator
        : public Object
        , public ITimeInterpolator
    {
    public:
        CAR_INTERFACE_DECL();

        ZInterpolator(
            /* [in] */ Float foc);

        CARAPI GetInterpolation(
            /* [in] */ Float input,
            /* [out] */ Float* result);

        CARAPI HasNativeInterpolator(
            /* [out] */ Boolean* res);

    private:
        Float mFocalLength;
    };

    /*
     * The exact reverse of ZInterpolator.
     */
    class InverseZInterpolator
        : public Object
        , public ITimeInterpolator
    {
    public:
        CAR_INTERFACE_DECL();

        InverseZInterpolator(
            /* [in] */ Float foc);

        CARAPI GetInterpolation(
            /* [in] */ Float input,
            /* [out] */ Float* result);

        CARAPI HasNativeInterpolator(
            /* [out] */ Boolean* res);

    private:
        AutoPtr<ZInterpolator> mZInterpolator;
    };

    /*
     * ZInterpolator compounded with an ease-out.
     */
    class ZoomOutInterpolator
        : public Object
        , public ITimeInterpolator
    {
    public:
        CAR_INTERFACE_DECL();

        ZoomOutInterpolator();

        CARAPI GetInterpolation(
            /* [in] */ Float input,
            /* [out] */ Float* result);

        CARAPI HasNativeInterpolator(
            /* [out] */ Boolean* res);

    private:
        AutoPtr<IDecelerateInterpolator> mDecelerate;
        AutoPtr<ZInterpolator> mZInterpolator;
    };

    /*
     * InvereZInterpolator compounded with an ease-out.
     */
    class ZoomInInterpolator
        : public Object
        , public ITimeInterpolator
    {
    public:
        CAR_INTERFACE_DECL();

        ZoomInInterpolator();

        CARAPI GetInterpolation(
            /* [in] */ Float input,
            /* [out] */ Float* result);

        CARAPI HasNativeInterpolator(
            /* [out] */ Boolean* res);

    private:
        AutoPtr<InverseZInterpolator> mInverseZInterpolator;
        AutoPtr<IDecelerateInterpolator> mDecelerate;
    };

    class FolderCreationAlarmListener
        : public Object
        , public IAlarmOnAlarmListener
    {
    public:
        CAR_INTERFACE_DECL();

        FolderCreationAlarmListener(
            /* [in] */ ICellLayout* layout,
            /* [in] */ Int32 cellX,
            /* [in] */ Int32 cellY,
            /* [in] */ Workspace* host);

        CARAPI OnAlarm(
            /* [in] */ IAlarm* alarm);

    public:
        AutoPtr<ICellLayout> mLayout;
        Int32 mCellX;
        Int32 mCellY;

    private:
        Workspace* mHost;
    };

    class ReorderAlarmListener
        : public Object
        , public IAlarmOnAlarmListener
    {
    public:
        CAR_INTERFACE_DECL();

        ReorderAlarmListener(
            /* [in] */ ArrayOf<Float>* dragViewCenter,
            /* [in] */ Int32 minSpanX,
            /* [in] */ Int32 minSpanY,
            /* [in] */ Int32 spanX,
            /* [in] */ Int32 spanY,
            /* [in] */ IDragView* dragView,
            /* [in] */ IView* child,
            /* [in] */ Workspace* host);

        CARAPI OnAlarm(
            /* [in] */ IAlarm* alarm);

    public:
        AutoPtr<ArrayOf<Float> > mDragViewCenter;
        Int32 mMinSpanX;
        Int32 mMinSpanY;
        Int32 mSpanX;
        Int32 mSpanY;
        AutoPtr<IDragView> mDragView;
        AutoPtr<IView> mChild;

    private:
        Workspace* mHost;
    };

private:

    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ Workspace* host);

        CARAPI Run();

    private:
        Workspace* mHost;
    };

    class MyThread
        : public Thread
    {
    public:
        MyThread(
            /* [in] */ const String& name,
            /* [in] */ Workspace* host);

        CARAPI Run();

    private:
        Workspace* mHost;
    };

    class MyAnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyAnimatorUpdateListener(
            /* [in] */ Workspace* host);

        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        Workspace* mHost;
    };

    class MyLauncherAnimatorUpdateListener
        : public LauncherAnimatorUpdateListener
    {

    public:
        MyLauncherAnimatorUpdateListener(
            /* [in] */ ICellLayout* cl,
            /* [in] */ Float _old,
            /* [in] */ Float _new);

        CARAPI OnAnimationUpdate(
            /* [in] */ Float a,
            /* [in] */ Float b);

    private:
        AutoPtr<ICellLayout> mCl;
        Float mOld;
        Float mNew;
    };

    class MyRunnable2
        : public Runnable
    {
    public:
        MyRunnable2(
            /* [in] */ Workspace* host,
            /* [in] */ ItemInfo* info,
            /* [in] */ ILauncherAppWidgetHostView* hostView,
            /* [in] */ ICellLayout* cellLayout);

        CARAPI Run();

    private:
        Workspace* mHost;
        AutoPtr<ItemInfo> mInfo;
        AutoPtr<ILauncherAppWidgetHostView> mHostView;
        AutoPtr<ICellLayout> mCellLayout;
    };

    class MyRunnable3
        : public Runnable
    {
    public:
        MyRunnable3(
            /* [in] */ Workspace* host,
            /* [in] */ IRunnable* addResizeFrame);

        CARAPI Run();

    private:
        Workspace* mHost;
        AutoPtr<IRunnable> mAddResizeFrame;
    };

    class MyRunnable4
        : public Runnable
    {
    public:
        MyRunnable4(
            /* [in] */ Workspace* host,
            /* [in] */ IRunnable* finalResizeRunnable);

        CARAPI Run();

    private:
        Workspace* mHost;
        AutoPtr<IRunnable> mFinalResizeRunnable;
    };

    class MyRunnable5
        : public Runnable
    {
    public:
        MyRunnable5(
            /* [in] */ Workspace* host);

        CARAPI Run();

    private:
        Workspace* mHost;
    };

    class MyRunnabl6
        : public Runnable
    {
    public:
        MyRunnabl6(
            /* [in] */ Workspace* host,
            /* [in] */ PendingAddItemInfo* pendingInfo,
            /* [in] */ ItemInfo* item,
            /* [in] */ Int64 container,
            /* [in] */ Int32 screen);

        CARAPI Run();

    private:
        Workspace* mHost;
        AutoPtr<PendingAddItemInfo> mPendingInfo;
        AutoPtr<ItemInfo> mItem;
        Int64 mContainer;
        Int32 mScreen;
    };

    class MyRunnable7
        : public Runnable
    {
    public:
        MyRunnable7(
            /* [in] */ IView* finalView,
            /* [in] */ IRunnable* onCompleteRunnable);

        CARAPI Run();

    private:
        AutoPtr<IView> mFinalView;
        AutoPtr<IRunnable> mOnCompleteRunnable;
    };

    class MyRunnabl8
        : public Runnable
    {
    public:
        MyRunnabl8(
            /* [in] */ Workspace* host,
            /* [in] */ IViewGroup* layout,
            /* [in] */ IHashSet* componentNames,
            /* [in] */ IUserHandle* user,
            /* [in] */ ICellLayout* layoutParent);

        CARAPI Run();

    private:
        Workspace* mHost;
        AutoPtr<IViewGroup> mLayout;
        AutoPtr<IHashSet> mComponentNames;
        AutoPtr<IUserHandle> mUser;
        AutoPtr<ICellLayout> mLayoutParent;
    };

    class MyRunnabl9
        : public Runnable
    {
    public:
        MyRunnabl9(
            /* [in] */ IContext* context,
            /* [in] */ IHashSet* componentNames);

        CARAPI Run();

    private:
        AutoPtr<IContext> mContext;
        AutoPtr<IHashSet> mComponentNames;
    };

public:
    CAR_INTERFACE_DECL();

    Workspace();

    CARAPI constructor();

    /**
     * Used to inflate the Workspace from XML.
     *
     * @param context The application's context.
     * @param attrs The attributes set containing the Workspace's customization values.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Used to inflate the Workspace from XML.
     *
     * @param context The application's context.
     * @param attrs The attributes set containing the Workspace's customization values.
     * @param defStyle Unused.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    // estimate the size of a widget with spans hSpan, vSpan. return MAX_VALUE for each
    // dimension if unsuccessful
    CARAPI EstimateItemSize(
        /* [in] */ Int32 hSpan,
        /* [in] */ Int32 vSpan,
        /* [in] */ IItemInfo* itemInfo,
        /* [in] */ Boolean springLoaded,
        /* [out, callee] */ ArrayOf<Int32>** outarray);

    CARAPI EstimateItemPosition(
        /* [in] */ ICellLayout* cl,
        /* [in] */ IItemInfo* pendingInfo,
        /* [in] */ Int32 hCell,
        /* [in] */ Int32 vCell,
        /* [in] */ Int32 hSpan,
        /* [in] */ Int32 vSpan,
        /* [out] */ IRect** rect);

    virtual CARAPI OnDragStart(
        /* [in] */ IDragSource* source,
        /* [in] */ IInterface* info,
        /* [in] */ Int32 dragAction);

    virtual CARAPI OnDragEnd();

    //@Override
    virtual CARAPI OnChildViewAdded(
        /* [in] */ IView* parent,
        /* [in] */ IView* child);

    //@Override
    virtual CARAPI OnChildViewRemoved(
        /* [in] */ IView* parent,
        /* [in] */ IView* child);

    /**
     * @return The open folder on the current screen, or null if there is none
     */
    CARAPI GetOpenFolder(
        /* [out] */ IFolder** folder);

    CARAPI IsTouchActive(
        /* [out] */ Boolean* result);

    /**
     * Adds the specified child in the specified screen. The position and dimension of
     * the child are defined by x, y, spanX and spanY.
     *
     * @param child The child to add in one of the workspace's screens.
     * @param screen The screen in which to add the child.
     * @param x The X position of the child in the screen's grid.
     * @param y The Y position of the child in the screen's grid.
     * @param spanX The number of cells spanned horizontally by the child.
     * @param spanY The number of cells spanned vertically by the child.
     */
    CARAPI AddInScreen(
        /* [in] */ IView* child,
        /* [in] */ Int64 container,
        /* [in] */ Int32 screen,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY);

    /**
     * Adds the specified child in the specified screen. The position and dimension of
     * the child are defined by x, y, spanX and spanY.
     *
     * @param child The child to add in one of the workspace's screens.
     * @param screen The screen in which to add the child.
     * @param x The X position of the child in the screen's grid.
     * @param y The Y position of the child in the screen's grid.
     * @param spanX The number of cells spanned horizontally by the child.
     * @param spanY The number of cells spanned vertically by the child.
     * @param insert When true, the child is inserted at the beginning of the children list.
     */
    CARAPI AddInScreen(
        /* [in] */ IView* child,
        /* [in] */ Int64 container,
        /* [in] */ Int32 screen,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ Boolean insert);

    /**
     * Called directly from a CellLayout (not by the framework), after we've been added as a
     * listener via setOnInterceptTouchEventListener(). This allows us to tell the CellLayout
     * that it should intercept touch events, which is not something that is normally supported.
     */
    //@Override
    virtual CARAPI OnTouch(
        /* [in] */ IView* v,
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI IsSwitchingState(
        /* [out] */ Boolean* result);

    /** This differs from isSwitchingState in that we take into account how far the transition
     *  has completed. */
    CARAPI IsFinishedSwitchingState(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI DispatchUnhandledMove(
        /* [in] */ IView* focused,
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    CARAPI UpdateWallpaperOffsetImmediately();

    //@Override
    CARAPI ComputeScroll();

    CARAPI ShowOutlines();

    CARAPI HideOutlines();

    CARAPI ShowOutlinesTemporarily();

    CARAPI SetChildrenOutlineAlpha(
        /* [in] */ Float alpha);

    CARAPI GetChildrenOutlineAlpha(
        /* [out] */ Float* result);

    CARAPI DisableBackground();

    CARAPI EnableBackground();

    CARAPI SetBackgroundAlpha(
        /* [in] */ Float alpha);

    CARAPI GetBackgroundAlpha(
        /* [out] */ Float* result);

    CARAPI BackgroundAlphaInterpolator(
        /* [in] */ Float r,
        /* [out] */ Float* result);

    CARAPI IsDrawingBackgroundGradient(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetDescendantFocusability(
        /* [out] */ Int32* result);

    //@Override
    CARAPI AddFocusables(
        /* [in] */ IArrayList* views,
        /* [in] */ Int32 direction,
        /* [in] */ Int32 focusableMode);

    CARAPI IsSmall(
        /* [out] */ Boolean* result);

    CARAPI EnableChildrenCache(
        /* [in] */ Int32 fromPage,
        /* [in] */ Int32 toPage);

    CARAPI ClearChildrenCache();

    CARAPI BuildPageHardwareLayers();

    /*
    *
    * We call these methods (onDragStartedWithItemSpans/onDragStartedWithSize) whenever we
    * start a drag in Launcher, regardless of whether the drag has ever entered the Workspace
    *
    * These methods mark the appropriate pages as accepting drops (which alters their visual
    * appearance).
    *
    */
    CARAPI OnDragStartedWithItem(
        /* [in] */ IView* v);

    CARAPI OnDragStartedWithItem(
        /* [in] */ IPendingAddItemInfo* info,
        /* [in] */ IBitmap* b,
        /* [in] */ Boolean clipAlpha);

    CARAPI ExitWidgetResizeMode();

    CARAPI GetChangeStateAnimation(
        /* [in] */ State state,
        /* [in] */ Boolean animated,
        /* [out] */ IAnimator** animator);

    CARAPI GetChangeStateAnimation(
        /* [in] */ State state,
        /* [in] */ Boolean animated,
        /* [in] */ Int32 delay,
        /* [out] */ IAnimator** animator);

    //@Override
    CARAPI OnLauncherTransitionPrepare(
        /* [in] */ ILauncher* l,
        /* [in] */ Boolean animated,
        /* [in] */ Boolean toWorkspace);

    //@Override
    CARAPI OnLauncherTransitionStart(
        /* [in] */ ILauncher* l,
        /* [in] */ Boolean animated,
        /* [in] */ Boolean toWorkspace);

    //@Override
    CARAPI OnLauncherTransitionStep(
        /* [in] */ ILauncher* l,
        /* [in] */ Float t);

    //@Override
    CARAPI OnLauncherTransitionEnd(
        /* [in] */ ILauncher* l,
        /* [in] */ Boolean animated,
        /* [in] */ Boolean toWorkspace);

    //@Override
    CARAPI GetContent(
        /* [out] */ IView** view);

    /**
     * Returns a new bitmap to show when the given View is being dragged around.
     * Responsibility for the bitmap is transferred to the caller.
     */
    CARAPI CreateDragBitmap(
        /* [in] */ IView* v,
        /* [in] */ ICanvas* canvas,
        /* [in] */ Int32 padding,
        /* [out] */ IBitmap** map);

    using View::StartDrag;

    CARAPI StartDrag(
        /* [in] */ ICellLayoutCellInfo* cellInfo);

    CARAPI BeginDragShared(
        /* [in] */ IView* child,
        /* [in] */ IDragSource* source);

    CARAPI AddApplicationShortcut(
        /* [in] */ IShortcutInfo* info,
        /* [in] */ ICellLayout* target,
        /* [in] */ Int64 container,
        /* [in] */ Int32 screen,
        /* [in] */ Int32 cellX,
        /* [in] */ Int32 cellY,
        /* [in] */ Boolean insertAtFirst,
        /* [in] */ Int32 intersectX,
        /* [in] */ Int32 intersectY);

    CARAPI TransitionStateShouldAllowDrop(
        /* [out] */ Boolean* result);

    /**
     * {@inheritDoc}
     */
    CARAPI AcceptDrop(
        /* [in] */ IDropTargetDragObject* d,
        /* [out] */ Boolean* result);

    CARAPI WillCreateUserFolder(
        /* [in] */ IItemInfo* info,
        /* [in] */ ICellLayout* target,
        /* [in] */ ArrayOf<Int32>* targetCell,
        /* [in] */ Float distance,
        /* [in] */ Boolean considerTimeout,
        /* [out] */ Boolean* result);

    CARAPI WillAddToExistingUserFolder(
        /* [in] */ IInterface* dragInfo,
        /* [in] */ ICellLayout* target,
        /* [in] */ ArrayOf<Int32>* targetCell,
        /* [in] */ Float distance,
        /* [out] */ Boolean* result);

    CARAPI CreateUserFolderIfNecessary(
        /* [in] */ IView* newView,
        /* [in] */ Int64 container,
        /* [in] */ ICellLayout* target,
        /* [in] */ ArrayOf<Int32>* targetCell,
        /* [in] */ Float distance,
        /* [in] */ Boolean external,
        /* [in] */ IDragView* dragView,
        /* [in] */ IRunnable* postAnimationRunnable,
        /* [out] */ Boolean* result);

    CARAPI AddToExistingFolderIfNecessary(
        /* [in] */ IView* newView,
        /* [in] */ ICellLayout* target,
        /* [in] */ ArrayOf<Int32>* targetCell,
        /* [in] */ Float distance,
        /* [in] */ IDropTargetDragObject* d,
        /* [in] */ Boolean external,
        /* [out] */ Boolean* result);

    CARAPI OnDrop(
        /* [in] */ IDropTargetDragObject* d);

    CARAPI SetFinalScrollForPageChange(
        /* [in] */ Int32 screen);

    CARAPI ResetFinalScrollForPageChange(
        /* [in] */ Int32 screen);

    CARAPI GetViewLocationRelativeToSelf(
        /* [in] */ IView* v,
        /* [in] */ ArrayOf<Int32>* location);

    CARAPI OnDragEnter(
        /* [in] */ IDropTargetDragObject* d);

    static CARAPI GetCellLayoutMetrics(
        /* [in] */ ILauncher* launcher,
        /* [in] */ Int32 orientation,
        /* [out] */ IRect** rect);

    CARAPI OnDragExit(
        /* [in] */ IDropTargetDragObject* d);

    CARAPI SetCurrentDropLayout(
        /* [in] */ ICellLayout* layout);

    CARAPI SetCurrentDragOverlappingLayout(
        /* [in] */ ICellLayout* layout);

    CARAPI SetCurrentDropOverCell(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI SetDragMode(
        /* [in] */ Int32 dragMode);

    CARAPI GetDropTargetDelegate(
        /* [in] */ IDropTargetDragObject* d,
        /* [out] */ IDropTarget** target);

    /*
    *
    * Convert the 2D coordinate xy from the parent View's coordinate space to this CellLayout's
    * coordinate space. The argument xy is modified with the return result.
    *
    */
    CARAPI MapPointFromSelfToChild(
        /* [in] */ IView* v,
        /* [in] */ ArrayOf<Float>* xy);

    /*
    *
    * Convert the 2D coordinate xy from the parent View's coordinate space to this CellLayout's
    * coordinate space. The argument xy is modified with the return result.
    *
    * if cachedInverseMatrix is not null, this method will just use that matrix instead of
    * computing it itself; we use this to avoid redundant matrix inversions in
    * findMatchingPageForDragOver
    *
    */
    CARAPI MapPointFromSelfToChild(
        /* [in] */ IView* v,
        /* [in] */ ArrayOf<Float>* xy,
        /* [in] */ IMatrix* cachedInverseMatrix);


    CARAPI MapPointFromSelfToHotseatLayout(
        /* [in] */ IHotseat* hotseat,
        /* [in] */ ArrayOf<Float>* xy);

    /*
    *
    * Convert the 2D coordinate xy from this CellLayout's coordinate space to
    * the parent View's coordinate space. The argument xy is modified with the return result.
    *
    */
    CARAPI MapPointFromChildToSelf(
        /* [in] */ IView* v,
        /* [in] */ ArrayOf<Float>* xy);

    /*
     *
     * Returns true if the passed CellLayout cl overlaps with dragView
     *
     */
    CARAPI Overlaps(
        /* [in] */ ICellLayout* cl,
        /* [in] */ IDragView* dragView,
        /* [in] */ Int32 dragViewX,
        /* [in] */ Int32 dragViewY,
        /* [in] */ IMatrix* cachedInverseMatrix,
        /* [out] */ Boolean* result);

    CARAPI OnDragOver(
        /* [in] */ IDropTargetDragObject* d);

    //@Override
    CARAPI GetHitRect(
        /* [in] */ IRect* outRect);

    /**
     * Add the item specified by dragInfo to the given layout.
     * @return true if successful
     */
    CARAPI AddExternalItemToScreen(
        /* [in] */ IItemInfo* dragInfo,
        /* [in] */ ICellLayout* layout,
        /* [out] */ Boolean* result);

    CARAPI CreateWidgetBitmap(
        /* [in] */ IItemInfo* widgetInfo,
        /* [in] */ IView* layout,
        /* [out] */ IBitmap** map);

    CARAPI AnimateWidgetDrop(
        /* [in] */ IItemInfo* info,
        /* [in] */ ICellLayout* cellLayout,
        /* [in] */ IDragView* dragView,
        /* [in] */ IRunnable* onCompleteRunnable,
        /* [in] */ Int32 animationType,
        /* [in] */ IView* finalView,
        /* [in] */ Boolean external);

    CARAPI SetFinalTransitionTransform(
        /* [in] */ ICellLayout* layout);

    CARAPI ResetTransitionTransform(
        /* [in] */ ICellLayout* layout);

    /**
     * Return the current {@link CellLayout}, correctly picking the destination
     * screen while a scroll is in progress.
     */
    CARAPI GetCurrentDropLayout(
        /* [out] */ ICellLayout** layout);

    /**
     * Return the current CellInfo describing our current drag; this method exists
     * so that Launcher can sync this object with the correct info when the activity is created/
     * destroyed
     *
     */
    CARAPI GetDragInfo(
        /* [out] */ ICellLayoutCellInfo** info);

    CARAPI Setup(
        /* [in] */ IDragController* dragController);

    /**
     * Called at the end of a drag which originated on the workspace.
     */
    CARAPI OnDropCompleted(
        /* [in] */ IView* target,
        /* [in] */ IDropTargetDragObject* d,
        /* [in] */ Boolean isFlingToDelete,
        /* [in] */ Boolean success);

    CARAPI UpdateItemLocationsInDatabase(
        /* [in] */ ICellLayout* cl);

    //@Override
    CARAPI SupportsFlingToDelete(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnFlingToDelete(
        /* [in] */ IDropTargetDragObject* d,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ IPointF* vec);

    //@Override
    CARAPI OnFlingToDeleteCompleted();

    CARAPI IsDropEnabled(
        /* [out] */ Boolean* result);

    CARAPI RestoreInstanceStateForChild(
        /* [in] */ Int32 child);

    CARAPI RestoreInstanceStateForRemainingPages();

    // @Override
    CARAPI ScrollLeft();

    // @Override
    CARAPI ScrollRight();

    // @Override
    CARAPI OnEnterScrollArea(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnExitScrollArea(
        /* [out] */ Boolean* result);

    /**
     * Returns a specific CellLayout
     */
    CARAPI GetParentCellLayoutForView(
        /* [in] */ IView* v,
        /* [out] */ ICellLayout** cellLayout);

    /**
     * Returns a list of all the CellLayouts in the workspace.
     */
    CARAPI GetWorkspaceAndHotseatCellLayouts(
        /* [out] */ IArrayList** list);

    /**
     * We should only use this to search for specific children.  Do not use this method to modify
     * ShortcutsAndWidgetsContainer directly. Includes ShortcutAndWidgetContainers from
     * the hotseat and workspace pages
     */
    CARAPI GetAllShortcutAndWidgetContainers(
        /* [out] */ IArrayList** list);

    CARAPI GetFolderForTag(
        /* [in] */ IInterface* tag,
        /* [out] */ IFolder** folder);

    CARAPI GetViewForTag(
        /* [in] */ IInterface* tag,
        /* [out] */ IView** view);

    CARAPI ClearDropTargets();

    // Removes ALL items that match a given package name, this is usually called when a package
    // has been removed and we want to remove all components (widgets, shortcuts, apps) that
    // belong to that package.
    CARAPI RemoveItemsByPackageName(
        /* [in] */ IArrayList* packages,
        /* [in] */ IUserHandle* user);

    // Removes items that match the application info specified, when applications are removed
    // as a part of an update, this is called to ensure that other widgets and application
    // shortcuts are not removed.
    CARAPI RemoveItemsByApplicationInfo(
        /* [in] */ IArrayList* appInfos,
        /* [in] */ IUserHandle* user);

    CARAPI RemoveItemsByComponentName(
        /* [in] */ IHashSet* componentNames,
        /* [in] */ IUserHandle* user);

    CARAPI UpdateShortcuts(
        /* [in] */ IArrayList* apps);

    CARAPI MoveToDefaultScreen(
        /* [in] */ Boolean animate);

    //@Override
    CARAPI SyncPages();

    //@Override
    CARAPI SyncPageItems(
        /* [in] */ Int32 page,
        /* [in] */ Boolean immediate);

    CARAPI GetLocationInDragLayer(
        /* [in] */ ArrayOf<Int32>* loc);

    CARAPI SetFadeForOverScroll(
        /* [in] */ Float fade);

protected:
    friend class SpringLoadedDragController;
    /**
     * Initializes various states for this workspace.
     */
    CARAPI InitWorkspace();

    //@Override
    CARAPI GetScrollMode(
        /* [out] */ Int32* mode);

    CARAPI_(Boolean) ShouldDrawChild(
        /* [in] */ IView* child);

    //@Override
    CARAPI_(Boolean) HitsPreviousPage(
        /* [in] */ Float x,
        /* [in] */ Float y);

    //@Override
    CARAPI_(Boolean) HitsNextPage(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI OnWindowVisibilityChanged(
        /* [in] */ Int32 visibility);

    CARAPI ReinflateWidgetsIfNecessary();

    //@Override
    CARAPI DetermineScrollingStart(
        /* [in] */ IMotionEvent* ev);

    CARAPI OnPageBeginMoving();

    CARAPI OnPageEndMoving();

    //@Override
    CARAPI NotifyPageSwitchListener();

    CARAPI SetWallpaperDimension();

    //@Override
    CARAPI UpdateCurrentPageScroll();

    //@Override
    CARAPI SnapToPage(
        /* [in] */ Int32 whichPage);

    //@Override
    CARAPI SnapToPage(
        /* [in] */ Int32 whichPage,
        /* [in] */ Int32 duration);

    CARAPI SnapToPage(
        /* [in] */ Int32 whichPage,
        /* [in] */ IRunnable* r);

    //@Override
    CARAPI ScreenScrolled(
        /* [in] */ Int32 screenCenter);

    //@Override
    CARAPI OverScroll(
        /* [in] */ Float amount);

    CARAPI OnAttachedToWindow();

    CARAPI OnDetachedFromWindow();

    //@Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    //@Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    //@Override
    CARAPI OnRequestFocusInDescendants(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect,
        /* [out] */ Boolean* result);

    CARAPI OnWallpaperTap(
        /* [in] */ IMotionEvent* ev);

    //@Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

    //@Override
    CARAPI DispatchRestoreInstanceState(
        /* [in] */ ISparseArray* container);

    //@Override
    CARAPI_(String) GetCurrentPageDescription();

    TO_STRING_IMPL("Workspace")

private:
    /**
     * Check if the point (x, y) hits a given page.
     */
    CARAPI_(Boolean) HitsPage(
        /* [in] */ Int32 index,
        /* [in] */ Float x,
        /* [in] */ Float y);

    // As a ratio of screen height, the total distance we want the parallax effect to span
    // horizontally
    CARAPI_(Float) WallpaperTravelToScreenWidthRatio(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    // The range of scroll values for Workspace
    CARAPI_(Int32) GetScrollRange();

    CARAPI_(Float) WallpaperOffsetForCurrentScroll();

    CARAPI_(void) SyncWallpaperOffsetWithScroll();

    CARAPI_(void) UpdateWallpaperOffsets();

    CARAPI_(void) ComputeWallpaperScrollRatio(
        /* [in] */ Int32 page);

    CARAPI_(void) AnimateBackgroundGradient(
        /* [in] */ Float finalAlpha,
        /* [in] */ Boolean animated);

    CARAPI_(void) UpdatePageAlphaValues(
        /* [in] */ Int32 screenCenter);

    CARAPI_(void) SetChildrenBackgroundAlphaMultipliers(
        /* [in] */ Float a);

    CARAPI_(void) UpdateChildrenLayersEnabled(
        /* [in] */ Boolean force);

    CARAPI_(void) EnableHwLayersOnVisiblePages();

    CARAPI_(void) InitAnimationArrays();

    /**
     * Draw the View v into the given Canvas.
     *
     * @param v the view to draw
     * @param destCanvas the canvas to draw on
     * @param padding the horizontal and vertical padding to use when drawing
     */
    CARAPI_(void) DrawDragView(
        /* [in] */ IView* v,
        /* [in] */ ICanvas* destCanvas,
        /* [in] */ Int32 padding,
        /* [in] */ Boolean pruneToDrawable);

    /**
     * Returns a new bitmap to be used as the object outline, e.g. to visualize the drop location.
     * Responsibility for the bitmap is transferred to the caller.
     */
    CARAPI_(AutoPtr<IBitmap>) CreateDragOutline(
        /* [in] */ IView* v,
        /* [in] */ ICanvas* canvas,
        /* [in] */ Int32 padding);

    /**
     * Returns a new bitmap to be used as the object outline, e.g. to visualize the drop location.
     * Responsibility for the bitmap is transferred to the caller.
     */
    CARAPI_(AutoPtr<IBitmap>) CreateDragOutline(
        /* [in] */ IBitmap* orig,
        /* [in] */ ICanvas* canvas,
        /* [in] */ Int32 padding,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Boolean clipAlpha);

    CARAPI_(void) CleanupFolderCreation();

    CARAPI_(void) CleanupAddToFolder();

    CARAPI_(void) CleanupReorder(
        /* [in] */ Boolean cancelAlarm);

    static CARAPI_(Float) SquaredDistance(
        /* [in] */ ArrayOf<Float>* point1,
        /* [in] */ ArrayOf<Float>* point2);

    /*
     *
     * This method returns the CellLayout that is currently being dragged to. In order to drag
     * to a CellLayout, either the touch point must be directly over the CellLayout, or as a second
     * strategy, we see if the dragView is overlapping any CellLayout and choose the closest one
     *
     * Return null if no CellLayout is currently being dragged over
     *
     */
    CARAPI_(AutoPtr<ICellLayout>) FindMatchingPageForDragOver(
        /* [in] */ IDragView* dragView,
        /* [in] */ Float originX,
        /* [in] */ Float originY,
        /* [in] */ Boolean exact);

    // This is used to compute the visual center of the dragView. This point is then
    // used to visualize drop locations and determine where to drop an item. The idea is that
    // the visual center represents the user's interpretation of where the item is, and hence
    // is the appropriate point to use when determining drop location.
    CARAPI_(AutoPtr<ArrayOf<Float> >) GetDragViewVisualCenter(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 xOffset,
        /* [in] */ Int32 yOffset,
        /* [in] */ IDragView* dragView,
        /* [in] */ ArrayOf<Float>* recycle);

    CARAPI_(Boolean) IsDragWidget(
        /* [in] */ DragObject* d);

    CARAPI_(Boolean) IsExternalDragWidget(
        /* [in] */ DragObject* d);

    CARAPI_(void) ManageFolderFeedback(
        /* [in] */ ItemInfo* info,
        /* [in] */ ICellLayout* targetLayout,
        /* [in] */ ArrayOf<Int32>* targetCell,
        /* [in] */ Float distance,
        /* [in] */ IView* dragOverView);

    CARAPI_(void) OnDropExternal(
        /* [in] */ ArrayOf<Int32>* touchXY,
        /* [in] */ IInterface* dragInfo,
        /* [in] */ ICellLayout* cellLayout,
        /* [in] */ Boolean insertAtFirst);

    /**
     * Drop an item that didn't originate on one of the workspace screens.
     * It may have come from Launcher (e.g. from all apps or customize), or it may have
     * come from another app altogether.
     *
     * NOTE: This can also be called when we are outside of a drag event, when we want
     * to add an item to one of the workspace screens.
     */
    CARAPI OnDropExternal(
        /* [in] */ ArrayOf<Int32>* touchXY,
        /* [in] */ IInterface* dragInfo,
        /* [in] */ ICellLayout* cellLayout,
        /* [in] */ Boolean insertAtFirst,
        /* [in] */ DragObject* d);

    CARAPI_(void) GetFinalPositionForDropAnimation(
        /* [in] */ ArrayOf<Int32>* loc,
        /* [in] */ ArrayOf<Float>* scaleXY,
        /* [in] */ IDragView* dragView,
        /* [in] */ ICellLayout* layout,
        /* [in] */ ItemInfo* info,
        /* [in] */ ArrayOf<Int32>* targetCell,
        /* [in] */ Boolean external,
        /* [in] */ Boolean scale);

    /**
     * Calculate the nearest cell where the given object would be dropped.
     *
     * pixelX and pixelY should be in the coordinate system of layout
     */
    CARAPI FindNearestArea(
        /* [in] */ Int32 pixelX,
        /* [in] */ Int32 pixelY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ ICellLayout* layout,
        /* [in] */ ArrayOf<Int32>* recycle,
        /* [out] */ ArrayOf<Int32>** array);

    CARAPI_(void) OnResetScrollArea();

public:
    AutoPtr<InnerListener> mInnerListener;

private:
    static const String TAG;

    // Y rotation to apply to the workspace screens
    static const Float WORKSPACE_OVERSCROLL_ROTATION;

    static const Int32 CHILDREN_OUTLINE_FADE_OUT_DELAY;
    static const Int32 CHILDREN_OUTLINE_FADE_OUT_DURATION;
    static const Int32 CHILDREN_OUTLINE_FADE_IN_DURATION;

    static const Int32 BACKGROUND_FADE_OUT_DURATION;
    static const Int32 ADJACENT_SCREEN_DROP_DURATION;
    static const Int32 FLING_THRESHOLD_VELOCITY;

    // These animators are used to fade the children's outlines
    AutoPtr<IObjectAnimator> mChildrenOutlineFadeInAnimation;
    AutoPtr<IObjectAnimator> mChildrenOutlineFadeOutAnimation;
    Float mChildrenOutlineAlpha;

    // These properties refer to the background protection gradient used for AllApps and Customize
    AutoPtr<IValueAnimator> mBackgroundFadeInAnimation;
    AutoPtr<IValueAnimator> mBackgroundFadeOutAnimation;
    AutoPtr<IDrawable> mBackground;
    Boolean mDrawBackground;
    Float mBackgroundAlpha;

    Float mWallpaperScrollRatio;
    Int32 mOriginalPageSpacing;

    AutoPtr<IWallpaperManager> mWallpaperManager;
    AutoPtr<IBinder> mWindowToken;
    static const Float WALLPAPER_SCREENS_SPAN;

    Int32 mDefaultPage;

    /**
     * CellInfo for the cell that is currently being dragged
     */
    AutoPtr<CellLayout::CellInfo> mDragInfo;

    /**
     * Target drop area calculated during last acceptDrop call.
     */
    AutoPtr<ArrayOf<Int32> > mTargetCell;
    Int32 mDragOverX;
    Int32 mDragOverY;

    static AutoPtr<IRect> mLandscapeCellLayoutMetrics;
    static AutoPtr<IRect> mPortraitCellLayoutMetrics;

    /**
     * The CellLayout that is currently being dragged over
     */
    AutoPtr<ICellLayout> mDragTargetLayout;
    /**
     * The CellLayout that we will show as glowing
     */
    AutoPtr<ICellLayout> mDragOverlappingLayout;

    /**
     * The CellLayout which will be dropped to
     */
    AutoPtr<ICellLayout> mDropToLayout;

    ILauncher* mLauncher;
    AutoPtr<IIconCache> mIconCache;
    AutoPtr<IDragController> mDragController;

    // These are temporary variables to prevent having to allocate a new object just to
    // return an (x, y) value from helper functions. Do NOT use them to maintain other state.
    AutoPtr<ArrayOf<Int32> > mTempCell ;
    AutoPtr<ArrayOf<Int32> > mTempEstimate ;
    AutoPtr<ArrayOf<Float> > mDragViewVisualCenter;
    AutoPtr<ArrayOf<Float> > mTempDragCoordinates;
    AutoPtr<ArrayOf<Float> > mTempCellLayoutCenterCoordinates;
    AutoPtr<ArrayOf<Float> > mTempDragBottomRightCoordinates;
    AutoPtr<IMatrix> mTempInverseMatrix;

    AutoPtr<SpringLoadedDragController> mSpringLoadedDragController;
    Float mSpringLoadedShrinkFactor;

    static const Int32 DEFAULT_CELL_COUNT_X;
    static const Int32 DEFAULT_CELL_COUNT_Y;

    State mState;
    Boolean mIsSwitchingState;

    Boolean mAnimatingViewIntoPlace;
    Boolean mIsDragOccuring;
    Boolean mChildrenLayersEnabled;

    /** Is the user is dragging an item near the edge of a page? */
    Boolean mInScrollArea;

    AutoPtr<HolographicOutlineHelper> mOutlineHelper;
    AutoPtr<IBitmap> mDragOutline;
    AutoPtr<IRect> mTempRect;
    AutoPtr<ArrayOf<Int32> > mTempXY;
    AutoPtr<ArrayOf<Int32> > mTempVisiblePagesRange;
    Float mOverscrollFade;
    Boolean mOverscrollTransformsSet;
    Boolean mWorkspaceFadeInAdjacentScreens;

    Int32 mWallpaperWidth;
    Int32 mWallpaperHeight;
    AutoPtr<WallpaperOffsetInterpolator> mWallpaperOffset;
    Boolean mUpdateWallpaperOffsetImmediately;
    AutoPtr<IRunnable> mDelayedResizeRunnable;
    AutoPtr<IRunnable> mDelayedSnapToPageRunnable;
    AutoPtr<IPoint> mDisplaySize;
    Boolean mIsStaticWallpaper;
    Int32 mWallpaperTravelWidth;
    Int32 mSpringLoadedPageSpacing;
    Int32 mCameraDistance;

    // Variables relating to the creation of user folders by hovering shortcuts over shortcuts
    static const Int32 FOLDER_CREATION_TIMEOUT;
    static const Int32 REORDER_TIMEOUT;
    AutoPtr<IAlarm> mFolderCreationAlarm;
    AutoPtr<IAlarm> mReorderAlarm;
    AutoPtr<IFolderIconFolderRingAnimator> mDragFolderRingAnimator;
    AutoPtr<IFolderIcon> mDragOverFolderIcon;
    Boolean mCreateUserFolderOnDrop;
    Boolean mAddToExistingFolderOnDrop;
    AutoPtr<DragEnforcer> mDragEnforcer;
    Float mMaxDistanceForFolderCreation;

    // Variables relating to touch disambiguation (scrolling workspace vs. scrolling a widget)
    Float mXDown;
    Float mYDown;
    static Float START_DAMPING_TOUCH_SLOP_ANGLE;
    static Float MAX_SWIPE_ANGLE;
    static Float TOUCH_SLOP_DAMPING_FACTOR;

    // Related to dragging, folder creation and reordering
    static const Int32 DRAG_MODE_NONE;
    static const Int32 DRAG_MODE_CREATE_FOLDER;
    static const Int32 DRAG_MODE_ADD_TO_FOLDER;
    static const Int32 DRAG_MODE_REORDER;
    Int32 mDragMode;
    Int32 mLastReorderX;
    Int32 mLastReorderY;

    AutoPtr<ISparseArray> mSavedStates;
    AutoPtr<IArrayList> mRestoredPages;

    // These variables are used for storing the initial and final values during workspace animations
    Int32 mSavedScrollX;
    Float mSavedRotationY;
    Float mSavedTranslationX;
    Float mCurrentScaleX;
    Float mCurrentScaleY;
    Float mCurrentRotationY;
    Float mCurrentTranslationX;
    Float mCurrentTranslationY;
    AutoPtr<ArrayOf<Float> > mOldTranslationXs;
    AutoPtr<ArrayOf<Float> > mOldTranslationYs;
    AutoPtr<ArrayOf<Float> > mOldScaleXs;
    AutoPtr<ArrayOf<Float> > mOldScaleYs;
    AutoPtr<ArrayOf<Float> > mOldBackgroundAlphas;
    AutoPtr<ArrayOf<Float> > mOldAlphas;
    AutoPtr<ArrayOf<Float> > mNewTranslationXs;
    AutoPtr<ArrayOf<Float> > mNewTranslationYs;
    AutoPtr<ArrayOf<Float> > mNewScaleXs;
    AutoPtr<ArrayOf<Float> > mNewScaleYs;
    AutoPtr<ArrayOf<Float> > mNewBackgroundAlphas;
    AutoPtr<ArrayOf<Float> > mNewAlphas;
    AutoPtr<ArrayOf<Float> > mNewRotationYs;
    Float mTransitionProgress;

    AutoPtr<IRunnable> mBindPages;

    AutoPtr<ZoomInInterpolator> mZoomInInterpolator;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_PACKAGECHANGEDRECEIVER_H__