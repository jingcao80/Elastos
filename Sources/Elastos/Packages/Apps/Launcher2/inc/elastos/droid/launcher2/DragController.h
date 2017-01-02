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

#ifndef  __ELASTOS_DROID_LAUNCHER2_DRAGCONTROLLER_H__
#define  __ELASTOS_DROID_LAUNCHER2_DRAGCONTROLLER_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/DropTarget.h"
#include <elastos/core/Runnable.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Launcher2::DragObject;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IPointF;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::View::IVelocityTracker;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * Class for initiating a drag within a view or across multiple views.
 */
class DragController
    : public Object
    , public IDragController
{
private:
    class ScrollRunnable
        : public Runnable
    {
    public:
        ScrollRunnable(
            /* [in] */ DragController* host);

        CARAPI Run();

        CARAPI SetDirection(
            /* [in] */ Int32 direction);

    private:
        Int32 mDirection;
        DragController* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    DragController();

    /**
     * Used to create a new DragLayer from XML.
     *
     * @param context The application's context.
     */
    CARAPI constructor(
        /* [in] */ ILauncher* launcher);

    CARAPI Dragging(
        /* [out] */ Boolean* result);

    /**
     * Starts a drag.
     *
     * @param v The view that is being dragged
     * @param bmp The bitmap that represents the view being dragged
     * @param source An object representing where the drag originated
     * @param dragInfo The data associated with the object that is being dragged
     * @param dragAction The drag action: either {@link #DRAG_ACTION_MOVE} or
     *        {@link #DRAG_ACTION_COPY}
     * @param dragRegion Coordinates within the bitmap b for the position of item being dragged.
     *          Makes dragging feel more precise, e.g. you can clip out a transparent border
     */
    CARAPI StartDrag(
        /* [in] */ IView* v,
        /* [in] */ IBitmap* bmp,
        /* [in] */ IDragSource* source,
        /* [in] */ IInterface* dragInfo,
        /* [in] */ Int32 dragAction,
        /* [in] */ IPoint* extraPadding,
        /* [in] */ Float initialDragViewScale);

    /**
     * Starts a drag.
     *
     * @param b The bitmap to display as the drag image.  It will be re-scaled to the
     *          enlarged size.
     * @param dragLayerX The x position in the DragLayer of the left-top of the bitmap.
     * @param dragLayerY The y position in the DragLayer of the left-top of the bitmap.
     * @param source An object representing where the drag originated
     * @param dragInfo The data associated with the object that is being dragged
     * @param dragAction The drag action: either {@link #DRAG_ACTION_MOVE} or
     *        {@link #DRAG_ACTION_COPY}
     * @param dragRegion Coordinates within the bitmap b for the position of item being dragged.
     *          Makes dragging feel more precise, e.g. you can clip out a transparent border
     */
    CARAPI StartDrag(
        /* [in] */ IBitmap* b,
        /* [in] */ Int32 dragLayerX,
        /* [in] */ Int32 dragLayerY,
        /* [in] */ IDragSource* source,
        /* [in] */ IInterface* dragInfo,
        /* [in] */ Int32 dragAction,
        /* [in] */ IPoint* dragOffset,
        /* [in] */ IRect* dragRegion,
        /* [in] */ Float initialDragViewScale);

    /**
     * Draw the view into a bitmap.
     */
    CARAPI GetViewBitmap(
        /* [in] */ IView* v,
        /* [out] */ IBitmap** map);

    /**
     * Call this from a drag source view like this:
     *
     * <pre>
     *  @Override
     *  public boolean dispatchKeyEvent(KeyEvent event) {
     *      return mDragController.dispatchKeyEvent(this, event)
     *              || super.dispatchKeyEvent(event);
     * </pre>
     */
    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI IsDragging(
        /* [out] */ Boolean* result);

    /**
     * Stop dragging without dropping.
     */
    CARAPI CancelDrag();

    CARAPI OnAppsRemoved(
        /* [in] */ IArrayList* appInfos,
        /* [in] */ IContext* context);

    /**
     * This only gets called as a result of drag view cleanup being deferred in endDrag();
     */
    CARAPI OnDeferredEndDrag(
        /* [in] */ IDragView* dragView);

    CARAPI OnDeferredEndFling(
        /* [in] */ IDropTargetDragObject* d);

    CARAPI GetLastGestureUpTime(
        /* [out] */ Int64* time);

    CARAPI ResetLastGestureUpTime();

    /**
     * Call this from a drag source view.
     */
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    /**
     * Sets the view that should handle move events.
     */
    CARAPI SetMoveTarget(
        /* [in] */ IView* view);

    CARAPI DispatchUnhandledMove(
        /* [in] */ IView* focused,
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* result);

    CARAPI ForceTouchMove();

    /**
     * Call this from a drag source view.
     */
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    CARAPI SetDragScoller(
        /* [in] */ IDragScroller* scroller);

    CARAPI SetWindowToken(
        /* [in] */ IBinder* token);

    /**
     * Sets the drag listner which will be notified when a drag starts or ends.
     */
    CARAPI AddDragListener(
        /* [in] */ IDragControllerDragListener* l);

    /**
     * Remove a previously installed drag listener.
     */
    CARAPI RemoveDragListener(
        /* [in] */ IDragControllerDragListener* l);

    /**
     * Add a DropTarget to the list of potential places to receive drop events.
     */
    CARAPI AddDropTarget(
        /* [in] */ IDropTarget* target);

    /**
     * Don't send drop events to <em>target</em> any more.
     */
    CARAPI RemoveDropTarget(
        /* [in] */ IDropTarget* target);

    /**
     * Sets the current fling-to-delete drop target.
     */
    CARAPI SetFlingToDeleteDropTarget(
        /* [in] */ IDropTarget* target);

    /**
     * Set which view scrolls for touch events near the edge of the screen.
     */
    CARAPI SetScrollView(
        /* [in] */ IView* v);

    CARAPI GetDragView(
        /* [out] */ IDragView** view);

private:
    CARAPI_(void) EndDrag();

    /**
     * Clamps the position to the drag layer bounds.
     */
    CARAPI_(AutoPtr<ArrayOf<Int32> >) GetClampedDragLayerPos(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(void) ClearScrollRunnable();

    CARAPI_(void) HandleMoveEvent(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI_(void) CheckTouchMove(
        /* [in] */ IDropTarget* dropTarget);

    CARAPI_(void) CheckScrollState(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    /**
     * Determines whether the user flung the current item to delete it.
     *
     * @return the vector at which the item was flung, or null if no fling was detected.
     */
    CARAPI_(AutoPtr<IPointF>) IsFlingingToDelete(
        /* [in] */ IDragSource* source);

    CARAPI_(void) DropOnFlingToDeleteTarget(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ IPointF* vel);

    CARAPI_(void) Drop(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(AutoPtr<IDropTarget>) FindDropTarget(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ ArrayOf<Int32>* dropCoordinates);

    CARAPI_(void) AcquireVelocityTrackerAndAddMovement(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(void) ReleaseVelocityTracker();

protected:
    Int32 mFlingToDeleteThresholdVelocity;

private:
    static const String TAG;

    static const Int32 SCROLL_DELAY;
    static const Int32 RESCROLL_DELAY;
    static const Int32 VIBRATE_DURATION;

    static const Boolean PROFILE_DRAWING_DURING_DRAG;

    static const Int32 SCROLL_OUTSIDE_ZONE;
    static const Int32 SCROLL_WAITING_IN_ZONE;

    static const Float MAX_FLING_DEGREES;

    ILauncher* mLauncher;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IVibrator> mVibrator;

    // temporaries to avoid gc thrash
    AutoPtr<IRect> mRectTemp;
    AutoPtr<ArrayOf<Int32> > mCoordinatesTemp;

    /** Whether or not we're dragging. */
    Boolean mDragging;

    /** X coordinate of the down event. */
    Int32 mMotionDownX;

    /** Y coordinate of the down event. */
    Int32 mMotionDownY;

    /** the area at the edge of the screen that makes the workspace go left
     *   or right while you're dragging.
     */
    Int32 mScrollZone;

    AutoPtr<DragObject> mDragObject;

    /** Who can receive drop events */
    AutoPtr<IArrayList> mDropTargets;
    AutoPtr<IArrayList> mListeners;
    AutoPtr<IDropTarget> mFlingToDeleteDropTarget;

    /** The window token used as the parent for the DragView. */
    AutoPtr<IBinder> mWindowToken;

    /** The view that will be scrolled when dragging to the left and right edges of the screen. */
    AutoPtr<IView> mScrollView;

    AutoPtr<IView> mMoveTarget;

    AutoPtr<IDragScroller> mDragScroller;
    Int32 mScrollState;
    AutoPtr<ScrollRunnable> mScrollRunnable;

    AutoPtr<IDropTarget> mLastDropTarget;

    AutoPtr<IInputMethodManager> mInputMethodManager;

    AutoPtr<ArrayOf<Int32> > mLastTouch;
    Int64 mLastTouchUpTime;
    Int32 mDistanceSinceScroll;

    AutoPtr<ArrayOf<Int32> > mTmpPoint;
    AutoPtr<IRect> mDragLayerRect;


    AutoPtr<IVelocityTracker> mVelocityTracker;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_DRAGCONTROLLER_H__