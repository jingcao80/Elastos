
#ifndef __ELASTOS_DROID_WIDGET_HORIZONTALSCROLLVIEW_H__
#define __ELASTOS_DROID_WIDGET_HORIZONTALSCROLLVIEW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/widget/FrameLayout.h"
#include "elastos/droid/view/VelocityTracker.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::VelocityTracker;

namespace Elastos {
namespace Droid {
namespace Widget {

class HorizontalScrollView
    : public FrameLayout
    , public IHorizontalScrollView
{
public:
    class HorizontalScrollViewSavedState
        : public View::BaseSavedState
        , public IHorizontalScrollViewSavedState
    {
    public:
        CAR_INTERFACE_DECL()

        HorizontalScrollViewSavedState();

        ~HorizontalScrollViewSavedState();

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
        Boolean mIsLayoutRtl;
    };

public:
    CAR_INTERFACE_DECL()

    HorizontalScrollView();

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

    virtual ~HorizontalScrollView();

    virtual CARAPI GetMaxScrollAmount(
        /* [out] */ Int32* maxScrollAmount);

    using FrameLayout::AddView;

    CARAPI AddView(
        /* [in] */ IView* child);

    CARAPI AddView(
        /* [in] */ IView* child,
        /* [in] */ Int32 index);

    CARAPI AddView(
        /* [in] */ IView* child,
        /* [in] */ IViewGroupLayoutParams* params);

    CARAPI AddView(
        /* [in] */ IView* child,
        /* [in] */ Int32 index,
        /* [in] */ IViewGroupLayoutParams* params);

    virtual CARAPI IsFillViewport(
        /* [out] */ Boolean* isFilled);

    virtual CARAPI SetFillViewport(
        /* [in] */ Boolean fillViewport);

    virtual CARAPI IsSmoothScrollingEnabled(
        /* [out] */ Boolean* enabled);

    virtual CARAPI SetSmoothScrollingEnabled(
        /* [in] */ Boolean smoothScrollingEnabled);

    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    virtual CARAPI ExecuteKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    virtual CARAPI ArrowScroll(
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* result);

    virtual CARAPI FullScroll(
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* result);

    virtual CARAPI PageScroll(
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* result);

    virtual CARAPI SmoothScrollBy(
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy);

    virtual CARAPI SmoothScrollTo(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI ComputeScroll();

    CARAPI RequestChildFocus(
        /* [in] */ IView* child,
        /* [in] */ IView* focused);

    CARAPI RequestChildRectangleOnScreen(
        /* [in] */ IView* child,
        /* [in] */ IRect* rectangle,
        /* [in] */ Boolean immediate,
        /* [out] */ Boolean* result);

    CARAPI RequestLayout();

    virtual CARAPI Fling(
        /* [in] */ Int32 velocityX);

    CARAPI ScrollTo(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI SetOverScrollMode(
        /* [in] */ Int32 overScrollMode);

    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    virtual CARAPI RequestDisallowInterceptTouchEvent(
        /* [in] */ Boolean disallowIntercept);

    virtual CARAPI OnGenericMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    virtual CARAPI ShouldDelayChildPressedState(
        /* [out] */ Boolean* compatibility);

    virtual CARAPI PerformAccessibilityAction(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments,
        /* [out] */ Boolean* result);

    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

protected:
    CARAPI_(Float) GetLeftFadingEdgeStrength();

    CARAPI_(Float) GetRightFadingEdgeStrength();

    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI_(void) OnOverScrolled(
        /* [in] */ Int32 scrollX,
        /* [in] */ Int32 scrollY,
        /* [in] */ Boolean clampedX,
        /* [in] */ Boolean clampedY);

    CARAPI_(Int32) ComputeHorizontalScrollRange();

    CARAPI_(Int32) ComputeHorizontalScrollOffset();

    CARAPI_(void) MeasureChild(
        /* [in] */ IView* child,
        /* [in] */ Int32 parentWidthMeasureSpec,
        /* [in] */ Int32 parentHeightMeasureSpec);

    CARAPI_(void) MeasureChildWithMargins(
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
    CARAPI_(Boolean) OnRequestFocusInDescendants(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);

    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI_(void) OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    // @Override
    CARAPI_(void) OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

    // @Override
    CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

private:
    CARAPI_(void) InitScrollView();

    /**
     * @return Returns true this HorizontalScrollView can be scrolled
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
     * Finds the next focusable component that fits in this View's bounds
     * (excluding fading edges) pretending that this View's left is located at
     * the parameter left.
     * </p>
     *
     * @param leftFocus          look for a candidate is the one at the left of the bounds
     *                           if leftFocus is true, or at the right of the bounds if leftFocus
     *                           is false
     * @param left               the left offset of the bounds in which a focusable must be
     *                           found (the fading edge is assumed to start at this position)
     * @param preferredFocusable the View that has highest priority and will be
     *                           returned if it is within my bounds (null is valid)
     * @return the next focusable component in the bounds or null if none can be found
     */
    AutoPtr<IView> FindFocusableViewInMyBounds(
        /* [in] */ Boolean leftFocus,
        /* [in] */ Int32 left,
        /* [in] */ IView* preferredFocusable);

    /**
     * <p>
     * Finds the next focusable component that fits in the specified bounds.
     * </p>
     *
     * @param leftFocus look for a candidate is the one at the left of the bounds
     *                  if leftFocus is true, or at the right of the bounds if
     *                  leftFocus is false
     * @param left      the left offset of the bounds in which a focusable must be
     *                  found
     * @param right     the right offset of the bounds in which a focusable must
     *                  be found
     * @return the next focusable component in the bounds or null if none can
     *         be found
     */
    CARAPI_(AutoPtr<IView>) FindFocusableViewInBounds(
        /* [in] */ Boolean leftFocus,
        /* [in] */ Int32 left,
        /* [in] */ Int32 right);

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
        /* [in] */ Int32 left,
        /* [in] */ Int32 right);

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
        /* [in] */ Int32 delta);

    /**
     * Smooth scroll by a Y delta
     *
     * @param delta the number of pixels to scroll by on the Y axis
     */
    CARAPI_(void) DoScrollX(
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

    CARAPI_(Int32) Clamp(
        /* [in] */ Int32 n,
        /* [in] */ Int32 my,
        /* [in] */ Int32 child);

    CARAPI_(void) InitOrResetVelocityTracker();

    CARAPI_(void) InitVelocityTrackerIfNotExists();

    CARAPI_(void) RecycleVelocityTracker();

private:
    const static String HORIZONTALSCROLLVIEW_NAME;
    static const Int32 ANIMATED_SCROLL_GAP;
    static const Float MAX_SCROLL_FACTOR;
    static const Int32 INVALID_POINTER;

    Int64 mLastScroll;

    AutoPtr<IRect> mTempRect;
    AutoPtr<IOverScroller> mScroller;
    AutoPtr<IEdgeEffect> mEdgeGlowLeft;
    AutoPtr<IEdgeEffect> mEdgeGlowRight;

    /**
     * Position of the last motion event.
     */
    Int32 mLastMotionX;

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
     * True if the user is currently dragging this HorizontalScrollView around. This is
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
    AutoPtr<HorizontalScrollViewSavedState> mSavedState;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_HORIZONTALSCROLLVIEW_H__
