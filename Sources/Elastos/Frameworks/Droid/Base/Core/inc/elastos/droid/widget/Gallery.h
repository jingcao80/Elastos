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

#ifndef __ELASTOS_DROID_WIDGET_GALLERY_H__
#define __ELASTOS_DROID_WIDGET_GALLERY_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/view/ViewGroup.h"
#include "elastos/droid/widget/AbsSpinner.h"
#include "elastos/droid/widget/Scroller.h"

using Elastos::Core::Runnable;
using Elastos::Droid::View::IGestureDetector;
using Elastos::Droid::View::IGestureDetectorOnGestureListener;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::Widget::IScroller;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * A view that shows items in a center-locked, horizontally scrolling list.
 * <p>
 * The default values for the Gallery assume you will be using
 * {@link android.R.styleable#Theme_galleryItemBackground} as the background for
 * each View given to the Gallery from the Adapter. If you are not doing this,
 * you may need to adjust some Gallery properties, such as the spacing.
 * <p>
 * Views given to the Gallery should use {@link Gallery.LayoutParams} as their
 * layout parameters type.
 *
 * <p>See the <a href="{@docRoot}resources/tutorials/views/hello-gallery.html">Gallery
 * tutorial</a>.</p>
 *
 * @attr ref android.R.styleable#Gallery_animationDuration
 * @attr ref android.R.styleable#Gallery_spacing
 * @attr ref android.R.styleable#Gallery_gravity
 */
class Gallery
    : public AbsSpinner
    , public IGallery
    , public IGestureDetectorOnGestureListener
{
public:
    /**
     * Gallery extends LayoutParams to provide a place to hold current
     * Transformation information along with previous position/transformation
     * info.
     *
     */
    class GalleryLayoutParams
        : public ViewGroup::LayoutParams
        , public IGalleryLayoutParams
    {
    public:
        CAR_INTERFACE_DECL()

        GalleryLayoutParams();

        ~GalleryLayoutParams();

        CARAPI constructor(
            /* [in] */ IContext* c,
            /* [in] */ IAttributeSet* attrs);

        CARAPI constructor(
            /* [in] */ Int32 w,
            /* [in] */ Int32 h);

        CARAPI constructor(
            /* [in] */ IViewGroupLayoutParams* source);
    };

private:
    /**
     * Responsible for fling behavior. Use {@link #startUsingVelocity(Int32)} to
     * initiate a fling. Each frame of the fling is handled in {@link #run()}.
     * A FlingRunnable will keep re-posting itself until the fling is done.
     *
     */
    class FlingRunnable : public Runnable
    {
        friend class Gallery;

    public:
        FlingRunnable(
            /* [in] */ Gallery* host);

        CARAPI_(void) StartUsingVelocity(
            /* [in] */ Int32 initialVelocity);

        CARAPI_(void) StartUsingDistance(
            /* [in] */ Int32 distance);

        CARAPI_(void) Stop(
            /* [in] */ Boolean scrollIntoSlots);

        CARAPI_(void) StartCommon();

        CARAPI_(void) EndFling(
            /* [in] */ Boolean scrollIntoSlots);

        virtual CARAPI Run();

    private:
        Gallery* mHost;

        /**
         * Tracks the decay of a fling scroll
         */
        AutoPtr<IScroller> mScroller;

        /**
         * X value reported by mScroller on the previous fling
         */
        Int32 mLastFlingX;
    };

    class DisableSuppressSelectionChangedRunnable : public Runnable
    {
    public:
        DisableSuppressSelectionChangedRunnable(
            /* [in] */ Gallery* host);

        virtual CARAPI Run();

    private:
        Gallery* mHost;
    };

    class KeyUpRunnable : public Runnable
    {
    public:
        KeyUpRunnable(
            /* [in] */ Gallery* host);

        virtual CARAPI Run();

    private:
        Gallery* mHost;
    };

    class DetectorOnGestureListener
        : public Object
        , public IGestureDetectorOnGestureListener
    {
    public:
        CAR_INTERFACE_DECL()

        DetectorOnGestureListener(
            /* [in] */ Gallery* host);

        virtual CARAPI OnDown(
            /* [in] */ IMotionEvent* e,
            /* [out] */ Boolean* res);

        virtual CARAPI OnShowPress(
            /* [in] */ IMotionEvent* e);

        virtual CARAPI OnSingleTapUp(
            /* [in] */ IMotionEvent* e,
            /* [out] */ Boolean* res);

        virtual CARAPI OnScroll(
            /* [in] */ IMotionEvent* e1,
            /* [in] */ IMotionEvent* e2,
            /* [in] */ Float distanceX,
            /* [in] */ Float distanceY,
            /* [out] */ Boolean* res);

        virtual CARAPI OnLongPress(
            /* [in] */ IMotionEvent* e);

        virtual CARAPI OnFling(
            /* [in] */ IMotionEvent* e1,
            /* [in] */ IMotionEvent* e2,
            /* [in] */ Float velocityX,
            /* [in] */ Float velocityY,
            /* [out] */ Boolean* res);

    private:
        Gallery* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    Gallery();

    virtual ~Gallery();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    /**
     * Whether or not to callback on any {@link #getOnItemSelectedListener()}
     * while the items are being flinged. If FALSE, only the final selected item
     * will cause the callback. If TRUE, all items between the first and the
     * final will cause callbacks.
     *
     * @param shouldCallback Whether or not to callback on the listener while
     *            the items are being flinged.
     */
    virtual CARAPI SetCallbackDuringFling(
        /* [in] */ Boolean shouldCallback);

    /**
     * Whether or not to callback when an item that is not selected is clicked.
     * If FALSE, the item will become selected (and re-centered). If TRUE, the
     * {@link #getOnItemClickListener()} will get the callback.
     *
     * @param shouldCallback Whether or not to callback on the listener when a
     *            item that is not selected is clicked.
     * @hide
     */
    virtual CARAPI SetCallbackOnUnselectedItemClick(
        /* [in] */ Boolean shouldCallback);

    /**
     * Sets how long the transition animation should run when a child view
     * changes position. Only relevant if animation is turned on.
     *
     * @param animationDurationMillis The duration of the transition, in
     *        milliseconds.
     *
     * @attr ref android.R.styleable#Gallery_animationDuration
     */
    virtual CARAPI SetAnimationDuration(
        /* [in] */ Int32 animationDurationMillis);

    /**
     * Sets the spacing between items in a Gallery
     *
     * @param spacing The spacing in pixels between items in the Gallery
     *
     * @attr ref android.R.styleable#Gallery_spacing
     */
    virtual CARAPI SetSpacing(
        /* [in] */ Int32 spacing);

    /**
     * Sets the alpha of items that are not selected in the Gallery.
     *
     * @param unselectedAlpha the alpha for the items that are not selected.
     *
     * @attr ref android.R.styleable#Gallery_unselectedAlpha
     */
    virtual CARAPI SetUnselectedAlpha(
        /* [in] */ Float unselectedAlpha);

    CARAPI_(Int32) GetChildHeight(
        /* [in] */ IView* child);

    /**
     * Tracks a motion scroll. In reality, this is used to do just about any
     * movement to items (touch scroll, arrow-key scroll, set an item as selected).
     *
     * @param deltaX Change in X from the previous event.
     */
    virtual CARAPI_(void) TrackMotionScroll(
        /* [in] */ Int32 deltaX);

    virtual CARAPI_(Int32) GetLimitedMotionScrollAmount(
        /* [in] */ Boolean motionToLeft,
        /* [in] */ Int32 deltaX);

    //@Override
    CARAPI_(void) SelectionChanged();

    using AdapterView::Layout;

    /**
     * Creates and positions all views for this Gallery.
     * <p>
     * We layout rarely, most of the time {@link #trackMotionScroll(Int32)} takes
     * care of repositioning, adding, and removing children.
     *
     * @param delta Change in the selected position. +1 means the selection is
     *            moving to the right, so views are scrolling to the left. -1
     *            means the selection is moving to the left.
     */
    //@Override
    CARAPI_(void) Layout(
        /* [in] */ Int32 delta,
        /* [in] */ Boolean animate);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    /**
     * {@inheritDoc}
     */
    virtual CARAPI OnSingleTapUp(
        /* [in] */ IMotionEvent* e,
        /* [out] */ Boolean* res);

    /**
     * {@inheritDoc}
     */
    virtual CARAPI OnFling(
        /* [in] */ IMotionEvent* e1,
        /* [in] */ IMotionEvent* e2,
        /* [in] */ Float velocityX,
        /* [in] */ Float velocityY,
        /* [out] */ Boolean* res);

    /**
     * {@inheritDoc}
     */
    virtual CARAPI OnScroll(
        /* [in] */ IMotionEvent* e1,
        /* [in] */ IMotionEvent* e2,
        /* [in] */ Float distanceX,
        /* [in] */ Float distanceY,
        /* [out] */ Boolean* res);

    /**
     * {@inheritDoc}
     */
    virtual CARAPI OnDown(
        /* [in] */ IMotionEvent* e,
        /* [out] */ Boolean* res);

    /**
     * Called when a touch event's action is MotionEvent.ACTION_UP.
     */
    virtual CARAPI OnUp();

    /**
     * Called when a touch event's action is MotionEvent.ACTION_CANCEL.
     */
    virtual CARAPI OnCancel();

    /**
     * {@inheritDoc}
     */
    virtual CARAPI OnLongPress(
        /* [in] */ IMotionEvent* e);

    // Unused methods from GestureDetector.OnGestureListener below

    /**
     * {@inheritDoc}
     */
    virtual CARAPI OnShowPress(
        /* [in] */ IMotionEvent* e);

    //@Override
    CARAPI DispatchSetSelected(
        /* [in] */ Boolean selected);

    //@Override
    CARAPI ShowContextMenuForChild(
        /* [in] */ IView* originalView,
        /* [out] */ Boolean* res);

    //@Override
    CARAPI ShowContextMenu(
        /* [out] */ Boolean* res);

    //@Override
    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    /**
     * Handles left, right, and clicking
     * @see android.view.View#onKeyDown
     */
    //@Override
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    //@Override
    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI_(Boolean) MovePrevious();

    virtual CARAPI_(Boolean) MoveNext();

    //@Override
    CARAPI_(void) SetSelectedPositionInt(
        /* [in] */ Int32 position);

    /**
     * Describes how the child views are aligned.
     * @param gravity
     *
     * @attr ref android.R.styleable#Gallery_gravity
     */
    virtual CARAPI SetGravity(
        /* [in] */ Int32 gravity);

    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    virtual CARAPI PerformAccessibilityAction(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments,
        /* [out] */ Boolean* res);

protected:
    //@Override
    CARAPI_(Boolean) GetChildStaticTransformation(
        /* [in] */ IView* child,
        /* [in] */ ITransformation* t);

    //@Override
    CARAPI_(Int32) ComputeHorizontalScrollExtent();

    //@Override
    CARAPI_(Int32) ComputeHorizontalScrollOffset();

    //@Override
    CARAPI_(Int32) ComputeHorizontalScrollRange();

    //@Override
    CARAPI_(Boolean) CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    //@Override
    CARAPI_(AutoPtr<IViewGroupLayoutParams>) GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    //@Override
    CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params);

    //@Override
    CARAPI GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** params);

    //@Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    //@Override
    CARAPI_(void) DispatchSetPressed(
        /* [in] */ Boolean pressed);

    //@Override
    CARAPI_(AutoPtr<IContextMenuInfo>) GetContextMenuInfo();

    //@Override
    CARAPI_(Int32) GetChildDrawingOrder(
        /* [in] */ Int32 childCount,
        /* [in] */ Int32 i);

    //@Override
    CARAPI_(void) OnFocusChanged(
        /* [in] */ Boolean gainFocus,
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);


private:
    /**
     * Offset the horizontal location of all children of this view by the
     * specified number of pixels.
     *
     * @param offset the number of pixels to offset
     */

    CARAPI_(void) OffsetChildrenLeftAndRight(
        /* [in] */ Int32 offset);

    /**
     * @return The center of this Gallery.
     */
    CARAPI_(Int32) GetCenterOfGallery();

    /**
     * @return The center of the given view.
     */
    static CARAPI_(Int32) GetCenterOfView(
        /* [in] */ IView* view);

    /**
     * Detaches children that are off the screen (i.e.: Gallery bounds).
     *
     * @param toLeft Whether to detach children to the left of the Gallery, or
     *            to the right.
     */
    CARAPI_(void) DetachOffScreenChildren(
        /* [in] */ Boolean toLeft);

    /**
     * Scrolls the items so that the selected item is in its 'slot' (its center
     * is the gallery's center).
     */
    CARAPI_(void) ScrollIntoSlots();

    CARAPI_(void) OnFinishedMovement();

    /**
     * Looks for the child that is closest to the center and sets it as the
     * selected child.
     */
    CARAPI_(void) SetSelectionToCenterChild();

    CARAPI_(void) FillToGalleryLeft();

    CARAPI_(void) FillToGalleryLeftRtl();

    CARAPI_(void) FillToGalleryLeftLtr();

    CARAPI_(void) FillToGalleryRight();

    CARAPI_(void) FillToGalleryRightRtl();

    CARAPI_(void) FillToGalleryRightLtr();

    /**
     * Obtain a view, either by pulling an existing view from the recycler or by
     * getting a new one from the adapter. If we are animating, make sure there
     * is enough information in the view's layout parameters to animate from the
     * old to new positions.
     *
     * @param position Position in the gallery for the view to obtain
     * @param offset Offset from the selected position
     * @param x X-coordintate indicating where this view should be placed. This
     *        will either be the left or right edge of the view, depending on
     *        the fromLeft paramter
     * @param fromLeft Are we posiitoning views based on the left edge? (i.e.,
     *        building from left to right)?
     * @return A view that has been added to the gallery
     */
    CARAPI_(AutoPtr<IView>) MakeAndAddView(
        /* [in] */ Int32 position,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 x,
        /* [in] */ Boolean fromLeft);

    /**
     * Helper for makeAndAddView to set the position of a view and fill out its
     * layout paramters.
     *
     * @param child The view to position
     * @param offset Offset from the selected position
     * @param x X-coordintate indicating where this view should be placed. This
     *        will either be the left or right edge of the view, depending on
     *        the fromLeft paramter
     * @param fromLeft Are we posiitoning views based on the left edge? (i.e.,
     *        building from left to right)?
     */
    CARAPI_(void) SetUpChild(
        /* [in] */ IView* child,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 x,
        /* [in] */ Boolean fromLeft);

    /**
     * Figure out vertical placement based on mGravity
     *
     * @param child Child to place
     * @return Where the top of the child should be
     */
    CARAPI_(Int32) CalculateTop(
        /* [in] */ IView* child,
        /* [in] */ Boolean duringLayout);

    CARAPI_(void) DispatchPress(
        /* [in] */ IView* child);

    CARAPI_(void) DispatchUnpress();

    CARAPI_(Boolean) DispatchLongPress(
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    CARAPI_(Boolean) ScrollToChild(
        /* [in] */ Int32 childPosition);

    CARAPI_(void) UpdateSelectedItemMetadata();

    CARAPI InitFromAttributes(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

private:
    static const Boolean localLOGV;

    /**
     * Duration in milliseconds from the start of a scroll during which we're
     * unsure whether the user is scrolling or flinging.
     */
    static const Int32 SCROLL_TO_FLING_UNCERTAINTY_TIMEOUT;

    static const String GALLERY_NAME;

    /**
     * Horizontal spacing between items.
     */
    Int32 mSpacing;

    /**
     * How long the transition animation should run when a child view changes
     * position, measured in milliseconds.
     */
    Int32 mAnimationDuration;

    /**
     * The alpha of items that are not selected.
     */
    Float mUnselectedAlpha;

    /**
     * Left most edge of a child seen so far during layout.
     */
    Int32 mLeftMost;

    /**
     * Right most edge of a child seen so far during layout.
     */
    Int32 mRightMost;

    Int32 mGravity;

    /**
     * Helper for detecting touch gestures.
     */
    AutoPtr<IGestureDetector> mGestureDetector;

    /**
     * The position of the item that received the user's down touch.
     */
    Int32 mDownTouchPosition;

    /**
     * The view of the item that received the user's down touch.
     */
    AutoPtr<IView> mDownTouchView;

    /**
     * Executes the delta scrolls from a fling or scroll movement.
     */
    AutoPtr<FlingRunnable> mFlingRunnable;

    /**
     * Sets mSuppressSelectionChanged = FALSE. This is used to set it to FALSE
     * in the future. It will also trigger a selection changed.
     */
    AutoPtr<IRunnable> mDisableSuppressSelectionChangedRunnable;

    AutoPtr<KeyUpRunnable> mKeyUpRunnable;

    /**
     * When fling runnable runs, it resets this to FALSE. Any method along the
     * path until the end of its run() can set this to TRUE to abort any
     * remaining fling. For example, if we've reached either the leftmost or
     * rightmost item, we will set this to TRUE.
     */
    Boolean mShouldStopFling;

    /**
     * The currently selected item's child.
     */
    AutoPtr<IView> mSelectedChild;

    /**
     * Whether to continuously callback on the item selected listener during a
     * fling.
     */
    Boolean mShouldCallbackDuringFling;

    /**
     * Whether to callback when an item that is not selected is clicked.
     */
    Boolean mShouldCallbackOnUnselectedItemClick;

    /**
     * If TRUE, do not callback to item selected listener.
     */
    Boolean mSuppressSelectionChanged;

    /**
     * If TRUE, we have received the "invoke" (center or enter buttons) key
     * down. This is checked before we action on the "invoke" key up, and is
     * subsequently cleared.
     */
    Boolean mReceivedInvokeKeyDown;

    AutoPtr<AdapterContextMenuInfo> mContextMenuInfo;

    /**
     * If TRUE, this onScroll is the first for this user's drag (remember, a
     * drag sends many onScrolls).
     */
    Boolean mIsFirstScroll;

    /**
     * If true, mFirstPosition is the position of the rightmost child, and
     * the children are ordered right to left.
     */
    Boolean mIsRtl;

    /**
     * Offset between the center of the selected child view and the center of the Gallery.
     * Used to reset position correctly during layout.
     */
    Int32 mSelectedCenterOffset;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_GALLERY_H__
