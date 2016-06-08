
#ifndef __ELASTOS_DROID_WIDGET_SCROLLVIEW_H__
#define __ELASTOS_DROID_WIDGET_SCROLLVIEW_H__

#include <elastos/droid/widget/FrameLayout.h>
#include <elastos/droid/view/VelocityTracker.h>

using Elastos::Droid::Os::IStrictModeSpan;
using Elastos::Droid::View::VelocityTracker;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;

namespace Elastos {
namespace Droid {
namespace Widget {

class OverScroller;

class ECO_PUBLIC ScrollView
    : public FrameLayout
    , public IScrollView
{
public:
    class ScrollViewSavedState
        : public BaseSavedState
        , public IScrollViewSavedState
    {
    public:
        CAR_INTERFACE_DECL()

        ScrollViewSavedState();

        ~ScrollViewSavedState();

        // @Override
        CARAPI WriteToParcel(
            /* [in] */ IParcel* dest);

        CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    public:
        Int32 mScrollPosition;
    };

public:
    CAR_INTERFACE_DECL()

    ScrollView();

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

    virtual ~ScrollView();

    virtual CARAPI GetMaxScrollAmount(
        /* [out] */ Int32* maxAmount);

    virtual CARAPI IsFillViewport(
        /* [out] */ Boolean* isFilled);

    virtual CARAPI SetFillViewport(
        /* [in] */ Boolean fillViewport);

    virtual CARAPI IsSmoothScrollingEnabled(
        /* [out] */ Boolean* enabled);

    virtual CARAPI SetSmoothScrollingEnabled(
        /* [in] */ Boolean smoothScrollingEnabled);

    virtual CARAPI ExecuteKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* handled);

    virtual CARAPI PageScroll(
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* result);

    virtual CARAPI FullScroll(
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* result);

    virtual CARAPI ArrowScroll(
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* result);

    virtual CARAPI SmoothScrollBy(
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy);

    virtual CARAPI SmoothScrollTo(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    virtual CARAPI Fling(
        /* [in] */ Int32 velocityY);

    virtual CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    virtual CARAPI ComputeScroll();

    virtual CARAPI RequestChildFocus(
        /* [in] */ IView* child,
        /* [in] */ IView* focused);

    virtual CARAPI RequestChildRectangleOnScreen(
        /* [in] */ IView* child,
        /* [in] */ IRect* rectangle,
        /* [in] */ Boolean immediate,
        /* [out] */ Boolean* result);

    virtual CARAPI RequestLayout();

    virtual CARAPI ScrollTo(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    virtual CARAPI SetOverScrollMode(
        /* [in] */ Int32 overScrollMode);

    // @Override
    CARAPI OnStartNestedScroll(
        /* [in] */ IView* child,
        /* [in] */ IView* target,
        /* [in] */ Int32 nestedScrollAxes,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnNestedScrollAccepted(
        /* [in] */ IView* child,
        /* [in] */ IView* target,
        /* [in] */ Int32 axes);

    /**
     * @inheritDoc
     */
    // @Override
    CARAPI OnStopNestedScroll(
        /* [in] */ IView* target);

    // @Override
    CARAPI OnNestedScroll(
        /* [in] */ IView* target,
        /* [in] */ Int32 dxConsumed,
        /* [in] */ Int32 dyConsumed,
        /* [in] */ Int32 dxUnconsumed,
        /* [in] */ Int32 dyUnconsumed);

    /**
     * @inheritDoc
     */
    // @Override
    CARAPI OnNestedFling(
        /* [in] */ IView* target,
        /* [in] */ Float velocityX,
        /* [in] */ Float velocityY,
        /* [in] */ Boolean consumed,
        /* [out] */ Boolean* result);

    virtual CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    virtual CARAPI AddView(
        /* [in] */ IView* child);

    virtual CARAPI AddView(
        /* [in] */ IView* child,
        /* [in] */ Int32 index);

    using FrameLayout::AddView;

    virtual CARAPI AddView(
        /* [in] */ IView* child,
        /* [in] */ IViewGroupLayoutParams* params);

    virtual CARAPI AddView(
        /* [in] */ IView* child,
        /* [in] */ Int32 index,
        /* [in] */ IViewGroupLayoutParams* params);

    virtual CARAPI ShouldDelayChildPressedState(
        /* [out] */ Boolean* compatibility);

    virtual CARAPI RequestDisallowInterceptTouchEvent(
        /* [in] */ Boolean disallowIntercept);

    virtual CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* value);

    virtual CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    virtual CARAPI OnGenericMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* handled);

    virtual CARAPI PerformAccessibilityAction(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments,
        /* [out] */ Boolean* performed);

    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

protected:
    virtual CARAPI_(Float) GetTopFadingEdgeStrength();

    virtual CARAPI_(Float) GetBottomFadingEdgeStrength();

    virtual CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    virtual CARAPI_(void) OnOverScrolled(
        /* [in] */ Int32 scrollX,
        /* [in] */ Int32 scrollY,
        /* [in] */ Boolean clampedX,
        /* [in] */ Boolean clampedY);

    virtual CARAPI_(Int32) ComputeVerticalScrollRange();

    virtual CARAPI_(Int32) ComputeVerticalScrollOffset();

    virtual CARAPI_(void) MeasureChild(
        /* [in] */ IView* child,
        /* [in] */ Int32 parentWidthMeasureSpec,
        /* [in] */ Int32 parentHeightMeasureSpec);

    virtual CARAPI_(void) MeasureChildWithMargins(
        /* [in] */ IView* child,
        /* [in] */ Int32 parentWidthMeasureSpec,
        /* [in] */ Int32 widthUsed,
        /* [in] */ Int32 parentHeightMeasureSpec,
        /* [in] */ Int32 heightUsed);

    /**
     * Compute the amount to scroll in the Y direction in order to get
     * a rectangle completely on the screen (or, if taller than the screen,
     * at least the first screen size chunk of it).
     *
     * @param rect The rect.
     * @return The scroll delta.
     */
    CARAPI_(Int32) ComputeScrollDeltaToGetChildRectOnScreen(
        /* [in] */ IRect* rect);

    /**
     * When looking for focus in children of a scroll view, need to be a little
     * more careful not to give focus to something that is scrolled off screen.
     *
     * This is more expensive than the default {@link android.view.ViewGroup}
     * implementation, otherwise this behavior might have been made the default.
     */
    virtual CARAPI_(Boolean) OnRequestFocusInDescendants(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);

    virtual CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    virtual CARAPI_(void) OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    virtual CARAPI OnDetachedFromWindow();


    // @Override
    CARAPI_(void) OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

    // @Override
    CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

private:
    CARAPI_(void) InitScrollView();

    /**
     * @return Returns true this ScrollView can be scrolled
     */
    CARAPI_(Boolean) CanScroll();

    CARAPI_(Boolean) InChild(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI_(void) OnSecondaryPointerUp(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(Int32) GetScrollRange();

    /**
     * <p>
     * Finds the next focusable component that fits in the specified bounds.
     * </p>
     *
     * @param topFocus look for a candidate is the one at the top of the bounds
     *                 if topFocus is true, or at the bottom of the bounds if topFocus is
     *                 false
     * @param top      the top offset of the bounds in which a focusable must be
     *                 found
     * @param bottom   the bottom offset of the bounds in which a focusable must
     *                 be found
     * @return the next focusable component in the bounds or null if none can
     *         be found
     */
    CARAPI_(AutoPtr<IView>) FindFocusableViewInBounds(
        /* [in] */ Boolean topFocus,
        /* [in] */ Int32 top,
        /* [in] */ Int32 bottom);

    /**
     * <p>Scrolls the view to make the area defined by <code>top</code> and
     * <code>bottom</code> visible. This method attempts to give the focus
     * to a component visible in this area. If no component can be focused in
     * the new visible area, the focus is reclaimed by this scrollview.</p>
     *
     * @param direction the scroll direction: {@link android.view.View#FOCUS_UP}
     *                  to go upward
     *                  {@link android.view.View#FOCUS_DOWN} to downward
     * @param top       the top offset of the new area to be made visible
     * @param bottom    the bottom offset of the new area to be made visible
     * @return true if the key event is consumed by this method, false otherwise
     */
    CARAPI_(Boolean) ScrollAndFocus(
        /* [in] */ Int32 direction,
        /* [in] */ Int32 top,
        /* [in] */ Int32 bottom);

    /**
     * @return whether the descendant of this scroll view is scrolled off
     *  screen.
     */
    CARAPI_(Boolean) IsOffScreen(
        /* [in] */ IView* descendant);

    /**
     * @return whether the descendant of this scroll view is within delta
     *  pixels of being on the screen.
     */
    CARAPI_(Boolean) IsWithinDeltaOfScreen(
        /* [in] */ IView* descendant,
        /* [in] */ Int32 delta,
        /* [in] */ Int32 height);

    /**
     * Smooth scroll by a Y delta
     *
     * @param delta the number of pixels to scroll by on the Y axis
     */
    CARAPI_(void) DoScrollY(
        /* [in] */ Int32 delta);

    /**
     * Scrolls the view to the given child.
     *
     * @param child the View to scroll to
     */
    CARAPI_(void) ScrollToChild(
        /* [in] */ IView* child);

    /**
     * If rect is off screen, scroll just enough to get it (or at least the
     * first screen size chunk of it) on screen.
     *
     * @param rect      The rectangle.
     * @param immediate True to scroll immediately without animation
     * @return true if scrolling was performed
     */
    CARAPI_(Boolean) ScrollToChildRect(
        /* [in] */ IRect* rect,
        /* [in] */ Boolean immediate);

    /**
     * Return true if child is an descendant of parent, (or equal to the parent).
     */
    CARAPI_(Boolean) IsViewDescendantOf(
        /* [in] */ IView* child,
        /* [in] */ IView* parent);

    ECO_LOCAL static CARAPI_(Int32) Clamp(
        /* [in] */ Int32 n,
        /* [in] */ Int32 my,
        /* [in] */ Int32 child);

    CARAPI_(void) InitOrResetVelocityTracker();

    CARAPI_(void) InitVelocityTrackerIfNotExists();

    CARAPI_(void) RecycleVelocityTracker();

    CARAPI_(void) EndDrag();

    CARAPI_(void) FlingWithNestedDispatch(
        /* [in] */ Int32 velocityY);

public:
    static const Int32 ANIMATED_SCROLL_GAP;
    static const Float MAX_SCROLL_FACTOR;
    static const String TAG;// = "ScrollView";

private:
     /**
     * Sentinel value for no current active pointer.
     * Used by {@link #mActivePointerId}.
     */
    ECO_LOCAL static const Int32 INVALID_POINTER;

private:
    Int64 mLastScroll;

    AutoPtr<IRect> mTempRect;
    AutoPtr<IOverScroller> mScroller;
    AutoPtr<IEdgeEffect> mEdgeGlowTop;
    AutoPtr<IEdgeEffect> mEdgeGlowBottom;

    /**
     * Position of the last motion event.
     */
    Int32 mLastMotionY;

    /**
     * True when the layout has changed but the traversal has not come through yet.
     * Ideally the view hierarchy would keep track of this for us.
     */
    Boolean mIsLayoutDirty;

    /**
     * The child to give focus to in the event that a child has requested focus while the
     * layout is dirty. This prevents the scroll from being wrong if the child has not been
     * laid out before requesting focus.
     */
    AutoPtr<IView> mChildToScrollTo;

    /**
     * True if the user is currently dragging this ScrollView around. This is
     * not the same as 'is being flinged', which can be checked by
     * mScroller.isFinished() (flinging begins when the user lifts his finger).
     */
    Boolean mIsBeingDragged;

    /**
     * Determines speed during touch scrolling
     */
    AutoPtr<VelocityTracker> mVelocityTracker;

    /**
     * When set to true, the scroll view measure its child to make it fill the currently
     * visible area.
     */
    Boolean mFillViewport;

    /**
     * Whether arrow scrolling is animated.
     */
    Boolean mSmoothScrollingEnabled;

    Int32 mTouchSlop;
    Int32 mMinimumVelocity;
    Int32 mMaximumVelocity;

    Int32 mOverscrollDistance;
    Int32 mOverflingDistance;

    /**
     * ID of the active pointer. This is used to retain consistency during
     * drags/flings if multiple pointers are used.
     */
    Int32 mActivePointerId;

    /**
     * Used during scrolling to retrieve the new offset within the window.
     */
    AutoPtr<ArrayOf<Int32> > mScrollOffset;
    AutoPtr<ArrayOf<Int32> > mScrollConsumed;
    Int32 mNestedYOffset;

    /**
     * The StrictMode "critical time span" objects to catch animation
     * stutters.  Non-null when a time-sensitive animation is
     * in-flight.  Must call finish() on them when done animating.
     * These are no-ops on user builds.
     */
     AutoPtr<IStrictModeSpan> mScrollStrictSpan;
     AutoPtr<IStrictModeSpan> mFlingStrictSpan;

     AutoPtr<ScrollViewSavedState> mSavedState;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_SCROLLVIEW_H__
