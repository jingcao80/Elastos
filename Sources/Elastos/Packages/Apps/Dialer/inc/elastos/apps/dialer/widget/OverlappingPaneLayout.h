#ifndef __ELASTOS_APPS_DIALER_WIDGET_OVERLAPPINGPAELAYOUT_H__
#define __ELASTOS_APPS_DIALER_WIDGET_OVERLAPPINGPAELAYOUT_H__

namespace Elastos{
namespace Apps{
namespace Dialer{
namespace Widget{

/**
 * A custom layout that aligns its child views vertically as two panes, and allows for the bottom
 * pane to be dragged upwards to overlap and hide the top pane. This layout is adapted from
 * {@link android.support.v4.widget.SlidingPaneLayout}.
 */
class OverlappingPaneLayout
    : public ViewGroup
    , public IOverlappingPaneLayout
{
public:
    class LayoutParams
        : public ViewGroup::MarginLayoutParams
        : public IOverlappingPaneLayoutLayoutParams
    {
    public:
        CAR_INTERFACE_DECL()

        LayoutParams(
            /* [in] */ OverlappingPaneLayout* host);

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI constructor(
            /* [in] */ IViewGroupLayoutParams* source);

        CARAPI constructor(
            /* [in] */ IViewGroupMarginLayoutParams* source);

        CARAPI constructor(
            /* [in] */ IOverlappingPaneLayoutLayoutParams* source);

        CARAPI constructor(
            /* [in] */ IContext* c,
            /* [in] */ IAttributeSet* attrs);

    public:
        /**
         * The weighted proportion of how much of the leftover space
         * this child should consume after measurement.
         */
        Float mWeight; // = 0;

        /**
         * True if this pane is the slideable pane in the layout.
         */
        Boolean mSlideable;

    private:
        OverlappingPaneLayout* mHost;
        static const Int32 ATTRS;
    };

    class SavedState
        : public BaseSavedState
    {
    public:
        SavedState(
            /* [in] */ OverlappingPaneLayout* host);

        CARAPI constructor(
        /* [in] */ IParcelable* superState);

        // @Override
        CARAPI WriteToParcel(
            /* [in] */ IParcel* out);

        // @Override
        CARAPI ReadFromParcel(
            /* [in] */ IParcel* in);

    public:
        Boolean mIsOpen;

    private:
        OverlappingPaneLayout* mHost;
    };

    // class AccessibilityDelegate
    //     : public AccessibilityDelegateCompat
    // {
    // public:
    //     AccessibilityDelegate(
    //         /* [in] */ OverlappingPaneLayout* host);

    //     // @Override
    //     CARAPI OnInitializeAccessibilityNodeInfo(
    //         /* [in] */ IView* host,
    //         /* [in] */ IAccessibilityNodeInfoCompat* info);

    //     // @Override
    //     CARAPI OnInitializeAccessibilityEvent(
    //         /* [in] */ IView* host,
    //         /* [in] */ IAccessibilityEvent* event);

    // private:
    //     /**
    //      * This should really be in AccessibilityNodeInfoCompat, but there unfortunately
    //      * seem to be a few elements that are not easily cloneable using the underlying API.
    //      * Leave it here as it's not general-purpose useful.
    //      */
    //     CARAPI_(void) CopyNodeInfoNoChildren(
    //         /* [in] */ IAccessibilityNodeInfoCompat* dest,
    //         /* [in] */ IAccessibilityNodeInfoCompat* src);

    // private:
    //     OverlappingPaneLayout* mHost;
    //     AutoPtr<IRect> mTmpRect;
    // };

private:
    class DragHelperCallback
        : public Object
        , public IViewDragHelperCallback
    {
    public:
        CAR_INTERFACE_DECL()

        DragHelperCallback(
            /* [in] */ OverlappingPaneLayout* host);

        // @Override
        CARAPI TryCaptureView(
            /* [in] */ IView* child,
            /* [in] */ Int32 pointerId,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnViewDragStateChanged(
            /* [in] */ Int32 state);

        // @Override
        CARAPI OnViewCaptured(
            /* [in] */ IView* capturedChild,
            /* [in] */ Int32 activePointerId);

        // @Override
        CARAPI OnViewPositionChanged(
            /* [in] */ IView* changedView,
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 dx,
            /* [in] */ Int32 dy);

        // @Override
        CARAPI OnViewReleased(
            /* [in] */ IView* releasedChild,
            /* [in] */ Float xvel,
            /* [in] */ Float yvel);

        // @Override
        CARAPI GetViewVerticalDragRange(
            /* [in] */ IView* child,
            /* [out] */ Int32* range);

        // @Override
        CARAPI ClampViewPositionHorizontal(
            /* [in] */ IView* child,
            /* [in] */ Int32 left,
            /* [in] */ Int32 dx,
            /* [out] */ Int32* result);

        // @Override
        CARAPI ClampViewPositionVertical(
            /* [in] */ IView* child,
            /* [in] */ Int32 top,
            /* [in] */ Int32 dy,
            /* [out] */ Int32* result);

        // @Override
        CARAPI OnEdgeDragStarted(
            /* [in] */ Int32 edgeFlags,
            /* [in] */ Int32 pointerId);

    private:
        OverlappingPaneLayout* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    /**
     * Set an offset, somewhere in between the panel's fully closed state and fully opened state,
     * where the panel can be temporarily pinned or opened up to.
     *
     * @param offset Offset in pixels
     */
    CARAPI SetIntermediatePinnedOffset(
        /* [in] */ Int32 offset);

    /**
     * Set the view that can be used to start dragging the sliding pane.
     */
    CARAPI SetCapturableView(
        /* [in] */ IView* capturableView);

    CARAPI SetPanelSlideCallbacks(
        /* [in] */ IOverlappingPaneLayoutPanelSlideCallbacks* listener);

    CARAPI_(void) DispatchOnPanelSlide(
        /* [in] */ IView* panel);

    CARAPI_(void) DispatchOnPanelOpened(
        /* [in] */ IView* panel);

    CARAPI_(void) DispatchOnPanelClosed(
        /* [in] */ IView* panel);

    CARAPI_(void) UpdateObscuredViewsVisibility(
        /* [in] */ IView* panel);

    CARAPI_(void) SetAllChildrenVisible();

    // @Override
    CARAPI RequestChildFocus(
        /* [in] */ IView* child,
        /* [in] */ IView* focused);

    // @Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* res);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    /**
     * Open the sliding pane if it is currently slideable. If first layout
     * has already completed this will animate.
     *
     * @return true if the pane was slideable and is now open/in the process of opening
     */
    CARAPI OpenPane(
        /* [out] */ Boolean* result);

    /**
     * Close the sliding pane if it is currently slideable. If first layout
     * has already completed this will animate.
     *
     * @return true if the pane was slideable and is now closed/in the process of closing
     */
    CARAPI ClosePane(
        /* [out] */ Boolean* result);

    /**
     * Check if the layout is open. It can be open either because the slider
     * itself is open revealing the left pane, or if all content fits without sliding.
     *
     * @return true if sliding panels are open
     */
    CARAPI IsOpen(
        /* [out] */ Boolean* result);

    /**
     * Check if the content in this layout cannot fully fit side by side and therefore
     * the content pane can be slid back and forth.
     *
     * @return true if content in this layout can be slid open and closed
     */
    CARAPI IsSlideable(
        /* [out] */ Boolean* result);

    /**
     * Smoothly animate mDraggingPane to the target X position within its range.
     *
     * @param slideOffset position to animate to
     * @param velocity initial velocity in case of fling, or 0.
     */
    CARAPI_(Boolean) SmoothSlideTo(
        /* [in] */ Float slideOffset,
        /* [in] */ Int32 velocity);

    // @Override
    CARAPI ComputeScroll();

    // @Override
    CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params);

    // @Override
    CARAPI OnStartNestedScroll(
        /* [in] */ IView* child,
        /* [in] */ IView* target,
        /* [in] */ Int32 nestedScrollAxes,
        /* [out] */ Boolean* res);

    // @Override
    CARAPI OnNestedPreScroll(
        /* [in] */ IView* target,
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy,
        /* [in] */ ArrayOf<Int32>* consumed);

    // @Override
    CARAPI OnNestedPreFling(
        /* [in] */ IView* target,
        /* [in] */ Float velocityX,
        /* [in] */ Float velocityY,
        /* [out] */ Boolean* res);

    // @Override
    CARAPI OnNestedScroll(
        /* [in] */ IView* target,
        /* [in] */ Int32 dxConsumed,
        /* [in] */ Int32 dyConsumed,
        /* [in] */ Int32 dxUnconsumed,
        /* [in] */ Int32 dyUnconsumed);

    // @Override
    CARAPI OnStopNestedScroll(
        /* [in] */ IView* child);

protected:
    // @Override
    CARAPI OnAttachedToWindow();

    // @Override
    CARAPI OnDetachedFromWindow();

    // @Override
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    // @Override
    CARAPI_(void) OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    // @Override
    CARAPI_(Boolean) DrawChild(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IView* child,
        /* [in] */ Int64 drawingTime);

    // @Override
    CARAPI GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** params);

    // @Override
    CARAPI_(AutoPtr<IViewGroupLayoutParams>) GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    // @Override
    CARAPI_(Boolean) CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    // @Override
    CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

    // @Override
    CARAPI_(void) OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

private:
    static CARAPI_(Boolean) ViewIsOpaque(
        /* [in] */ IView* v);

    CARAPI_(Boolean) ClosePane(
        /* [in] */ IView* pane,
        /* [in] */ Int32 initialVelocity);

    CARAPI_(Boolean) OpenPane(
        /* [in] */ IView* pane,
        /* [in] */ Int32 initialVelocity);

    CARAPI_(void) UpdateSlideOffset(
        /* [in] */ Int32 offsetPx);

    CARAPI_(void) OnPanelDragged(
        /* [in] */ Int32 newTop);

    CARAPI_(Boolean) IsCapturableViewUnder(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

private:
    static const String TAG; // = "SlidingPaneLayout";
    static const Boolean DEBUG; // = false;

    /**
     * Default size of the overhang for a pane in the open state.
     * At least this much of a sliding pane will remain visible.
     * This indicates that there is more content available and provides
     * a "physical" edge to grab to pull it closed.
     */
    static const Int32 DEFAULT_OVERHANG_SIZE; // = 32; // dp;

    /**
     * If no fade color is given by default it will fade to 80% gray.
     */
    static const Int32 DEFAULT_FADE_COLOR; // = 0xcccccccc;

    /**
     * Minimum velocity that will be detected as a fling
     */
    static const Int32 MIN_FLING_VELOCITY; // = 400; // dips per second

    /**
     * The size of the overhang in pixels.
     * This is the minimum section of the sliding panel that will
     * be visible in the open state to allow for a closing drag.
     */
    Int32 mOverhangSize;

    /**
     * True if a panel can slide with the current measurements
     */
    Boolean mCanSlide;

    /**
     * The child view that can slide, if any.
     */
    AutoPtr<IView> mSlideableView;

    /**
     * The view that can be used to start the drag with.
     */
    AutoPtr<IView> mCapturableView;

    /**
     * How far the panel is offset from its closed position.
     * range [0, 1] where 0 = closed, 1 = open.
     */
    Float mSlideOffset;

    /**
     * How far the panel is offset from its closed position, in pixels.
     * range [0, {@link #mSlideRange}] where 0 is completely closed.
     */
    Int32 mSlideOffsetPx;

    /**
     * How far in pixels the slideable panel may move.
     */
    Int32 mSlideRange;

    /**
     * A panel view is locked into internal scrolling or another condition that
     * is preventing a drag.
     */
    Boolean mIsUnableToDrag;

    /**
     * Tracks whether or not a child view is in the process of a nested scroll.
     */
    Boolean mIsInNestedScroll;

    /**
     * Indicates that the layout is currently in the process of a nested pre-scroll operation where
     * the child scrolling view is being dragged downwards.
     */
    Boolean mInNestedPreScrollDownwards;

    /**
     * Indicates that the layout is currently in the process of a nested pre-scroll operation where
     * the child scrolling view is being dragged upwards.
     */
    Boolean mInNestedPreScrollUpwards;

    /**
     * Indicates that the layout is currently in the process of a fling initiated by a pre-fling
     * from the child scrolling view.
     */
    Boolean mIsInNestedFling;

    /**
     * Indicates the direction of the pre fling. We need to store this information since
     * OverScoller doesn't expose the direction of its velocity.
     */
    Boolean mInUpwardsPreFling;

    /**
     * Stores an offset used to represent a point somewhere in between the panel's fully closed
     * state and fully opened state where the panel can be temporarily pinned or opened up to
     * during scrolling.
     */
    Int32 mIntermediateOffset = 0;

    Float mInitialMotionX;
    Float mInitialMotionY;

    AutoPtr<IOverlappingPaneLayoutPanelSlideCallbacks> mPanelSlideCallbacks;

    AutoPtr<IViewDragHelper> mDragHelper;

    /**
     * Stores whether or not the pane was open the last time it was slideable.
     * If open/close operations are invoked this state is modified. Used by
     * instance state save/restore.
     */
    Boolean mPreservedOpenState;
    Boolean mFirstLayout = true;

    AutoPtr<IRect> mTmpRect; // = new Rect();

    /**
     * How many dips we need to scroll past a position before we can snap to the next position
     * on release. Using this prevents accidentally snapping to positions.
     *
     * This is needed since vertical nested scrolling can be passed to this class even if the
     * vertical scroll is less than the the nested list's touch slop.
     */
    Int32 mReleaseScrollSlop;
};

} // Widget
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_WIDGET_OVERLAPPINGPAELAYOUT_H__

