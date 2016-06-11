#ifndef __ELASTOS_APPS_DIALER_WIDGET_VIEWDRAGHELPER_H__
#define __ELASTOS_APPS_DIALER_WIDGET_VIEWDRAGHELPER_H__

namespace Elastos{
namespace Apps{
namespace Dialer{
namespace Widget{

/**
 * ViewDragHelper is a utility class for writing custom ViewGroups. It offers a number
 * of useful operations and state tracking for allowing a user to drag and reposition
 * views within their parent ViewGroup.
 */
class ViewDragHelper
    : public Object
    , public IViewDragHelper
{
public:
    /**
     * A Callback is used as a communication channel with the ViewDragHelper back to the
     * parent view using it. <code>on*</code>methods are invoked on siginficant events and several
     * accessor methods are expected to provide the ViewDragHelper with more information
     * about the state of the parent view upon request. The callback also makes decisions
     * governing the range and draggability of child views.
     */
    class Callback
        : public Object
        , public IViewDragHelperCallback
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * Called when the drag state changes. See the <code>STATE_*</code> constants
         * for more information.
         *
         * @param state The new drag state
         *
         * @see #STATE_IDLE
         * @see #STATE_DRAGGING
         * @see #STATE_SETTLING
         */
        CARAPI OnViewDragStateChanged(
            /* [in] */ Int32 state);

        /**
         * Called when the captured view's position changes as the result of a drag or settle.
         *
         * @param changedView View whose position changed
         * @param left New X coordinate of the left edge of the view
         * @param top New Y coordinate of the top edge of the view
         * @param dx Change in X position from the last call
         * @param dy Change in Y position from the last call
         */
        CARAPI OnViewPositionChanged(
            /* [in] */ IView* changedView,
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 dx,
            /* [in] */ Int32 dy);

        /**
         * Called when a child view is captured for dragging or settling. The ID of the pointer
         * currently dragging the captured view is supplied. If activePointerId is
         * identified as {@link #INVALID_POINTER} the capture is programmatic instead of
         * pointer-initiated.
         *
         * @param capturedChild Child view that was captured
         * @param activePointerId Pointer id tracking the child capture
         */
        CARAPI OnViewCaptured(
            /* [in] */ IView* capturedChild,
            /* [in] */ Int32 activePointerId);

        /**
         * Called when the child view is no longer being actively dragged.
         * The fling velocity is also supplied, if relevant. The velocity values may
         * be clamped to system minimums or maximums.
         *
         * <p>Calling code may decide to fling or otherwise release the view to let it
         * settle into place. It should do so using {@link #settleCapturedViewAt(Int32, Int32)}
         * or {@link #flingCapturedView(Int32, Int32, Int32, Int32)}. If the Callback invokes
         * one of these methods, the ViewDragHelper will enter {@link #STATE_SETTLING}
         * and the view capture will not fully end until it comes to a complete stop.
         * If neither of these methods is invoked before <code>onViewReleased</code> returns,
         * the view will stop in place and the ViewDragHelper will return to
         * {@link #STATE_IDLE}.</p>
         *
         * @param releasedChild The captured child view now being released
         * @param xvel X velocity of the pointer as it left the screen in pixels per second.
         * @param yvel Y velocity of the pointer as it left the screen in pixels per second.
         */
        CARAPI OnViewReleased(
            /* [in] */ IView* releasedChild,
            /* [in] */ Float xvel,
            /* [in] */ Float yvel);

        /**
         * Called when the child view has been released with a fling.
         *
         * <p>Calling code may decide to fling or otherwise release the view to let it
         * settle into place.</p>
         *
         * @param releasedChild The captured child view now being released
         * @param xvel X velocity of the fling.
         * @param yvel Y velocity of the fling.
         */
        CARAPI OnViewFling(
            /* [in] */ IView* releasedChild,
            /* [in] */ Float xvel,
            /* [in] */ Float yvel);

        /**
         * Called when one of the subscribed edges in the parent view has been touched
         * by the user while no child view is currently captured.
         *
         * @param edgeFlags A combination of edge flags describing the edge(s) currently touched
         * @param pointerId ID of the pointer touching the described edge(s)
         * @see #EDGE_LEFT
         * @see #EDGE_TOP
         * @see #EDGE_RIGHT
         * @see #EDGE_BOTTOM
         */
        CARAPI OnEdgeTouched(
            /* [in] */ Int32 edgeFlags,
            /* [in] */ Int32 pointerId);

        /**
         * Called when the given edge may become locked. This can happen if an edge drag
         * was preliminarily rejected before beginning, but after {@link #onEdgeTouched(Int32, Int32)}
         * was called. This method should return true to lock this edge or false to leave it
         * unlocked. The default behavior is to leave edges unlocked.
         *
         * @param edgeFlags A combination of edge flags describing the edge(s) locked
         * @return true to lock the edge, false to leave it unlocked
         */
        CARAPI OnEdgeLock(
            /* [in] */ Int32 edgeFlags,
            /* [out] */ Boolean* result);

        /**
         * Called when the user has started a deliberate drag away from one
         * of the subscribed edges in the parent view while no child view is currently captured.
         *
         * @param edgeFlags A combination of edge flags describing the edge(s) dragged
         * @param pointerId ID of the pointer touching the described edge(s)
         * @see #EDGE_LEFT
         * @see #EDGE_TOP
         * @see #EDGE_RIGHT
         * @see #EDGE_BOTTOM
         */
        CARAPI OnEdgeDragStarted(
            /* [in] */ Int32 edgeFlags,
            /* [in] */ Int32 pointerId);

        /**
         * Called to determine the Z-order of child views.
         *
         * @param index the ordered position to query for
         * @return index of the view that should be ordered at position <code>index</code>
         */
        CARAPI GetOrderedChildIndex(
            /* [in] */ Int32 index,
            /* [out] */ Int32* childIndex);

        /**
         * Return the magnitude of a draggable child view's horizontal range of motion in pixels.
         * This method should return 0 for views that cannot move horizontally.
         *
         * @param child Child view to check
         * @return range of horizontal motion in pixels
         */
        CARAPI GetViewHorizontalDragRange(
            /* [in] */ IView* child,
            /* [out] */ Int32* result);

        /**
         * Return the magnitude of a draggable child view's vertical range of motion in pixels.
         * This method should return 0 for views that cannot move vertically.
         *
         * @param child Child view to check
         * @return range of vertical motion in pixels
         */
        CARAPI GetViewVerticalDragRange(
            /* [in] */ IView* child,
            /* [out] */ Int32* result);

        /**
         * Restrict the motion of the dragged child view along the horizontal axis.
         * The default implementation does not allow horizontal motion; the extending
         * class must override this method and provide the desired clamping.
         *
         *
         * @param child Child view being dragged
         * @param left Attempted motion along the X axis
         * @param dx Proposed change in position for left
         * @return The new clamped position for left
         */
        CARAPI ClampViewPositionHorizontal(
            /* [in] */ IView* child,
            /* [in] */ Int32 left,
            /* [in] */ Int32 dx,
            /* [out] */ Int32* position);

        /**
         * Restrict the motion of the dragged child view along the vertical axis.
         * The default implementation does not allow vertical motion; the extending
         * class must override this method and provide the desired clamping.
         *
         *
         * @param child Child view being dragged
         * @param top Attempted motion along the Y axis
         * @param dy Proposed change in position for top
         * @return The new clamped position for top
         */
        CARAPI ClampViewPositionVertical(
            /* [in] */ IView* child,
            /* [in] */ Int32 top,
            /* [in] */ Int32 dy,
            /* [out] */ Int32* position);
    };

private:
    class SetIdleRunnable
        : public Runnable
    {
    public:
        SetIdleRunnable(
            /* [in] */ ViewDragHelper* host);

        CARAPI Run();

    private:
        ViewDragHelper* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Factory method to create a new ViewDragHelper.
     *
     * @param forParent Parent view to monitor
     * @param cb Callback to provide information and receive events
     * @return a new ViewDragHelper instance
     */
    static CARAPI_(AutoPtr<IViewDragHelper>) Create(
        /* [in] */ IViewGroup* forParent,
        /* [in] */ IViewDragHelperCallback* cb);

    /**
     * Factory method to create a new ViewDragHelper.
     *
     * @param forParent Parent view to monitor
     * @param sensitivity Multiplier for how sensitive the helper should be about detecting
     *                    the start of a drag. Larger values are more sensitive. 1.0f is normal.
     * @param cb Callback to provide information and receive events
     * @return a new ViewDragHelper instance
     */
    static CARAPI_(AutoPtr<IViewDragHelper>) Create(
        /* [in] */ IViewGroup* forParent,
        /* [in] */ Float sensitivity,
        /* [in] */ IViewDragHelperCallback* cb);

    /**
     * Set the minimum velocity that will be detected as having a magnitude greater than zero
     * in pixels per second. Callback methods accepting a velocity will be clamped appropriately.
     *
     * @param minVel Minimum velocity to detect
     */
    CARAPI SetMinVelocity(
        /* [in] */ Float minVel);

    /**
     * Return the currently configured minimum velocity. Any flings with a magnitude less
     * than this value in pixels per second. Callback methods accepting a velocity will receive
     * zero as a velocity value if the real detected velocity was below this threshold.
     *
     * @return the minimum velocity that will be detected
     */
    CARAPI GetMinVelocity(
        /* [out] */ Float* minVel);

    /**
     * Retrieve the current drag state of this helper. This will return one of
     * {@link #STATE_IDLE}, {@link #STATE_DRAGGING} or {@link #STATE_SETTLING}.
     * @return The current drag state
     */
    CARAPI GetViewDragState(
        /* [out] */ Int32* state);

    /**
     * Enable edge tracking for the selected edges of the parent view.
     * The callback's {@link Callback#onEdgeTouched(Int32, Int32)} and
     * {@link Callback#onEdgeDragStarted(Int32, Int32)} methods will only be invoked
     * for edges for which edge tracking has been enabled.
     *
     * @param edgeFlags Combination of edge flags describing the edges to watch
     * @see #EDGE_LEFT
     * @see #EDGE_TOP
     * @see #EDGE_RIGHT
     * @see #EDGE_BOTTOM
     */
    CARAPI SetEdgeTrackingEnabled(
        /* [in] */ Int32 edgeFlags);

    /**
     * Return the size of an edge. This is the range in pixels along the edges of this view
     * that will actively detect edge touches or drags if edge tracking is enabled.
     *
     * @return The size of an edge in pixels
     * @see #setEdgeTrackingEnabled(Int32)
     */
    CARAPI GetEdgeSize(
        /* [out] */ Int32* size);

    /**
     * Capture a specific child view for dragging within the parent. The callback will be notified
     * but {@link Callback#tryCaptureView(android.view.View, Int32)} will not be asked permission to
     * capture this view.
     *
     * @param childView Child view to capture
     * @param activePointerId ID of the pointer that is dragging the captured child view
     */
    CARAPI CaptureChildView(
        /* [in] */ IView* childView,
        /* [in] */ Int32 activePointerId);

    /**
     * @return The currently captured view, or null if no view has been captured.
     */
    CARAPI GetCapturedView(
        /* [out] */ IView** view);

    /**
     * @return The ID of the pointer currently dragging the captured view,
     *         or {@link #INVALID_POINTER}.
     */
    CARAPI GetActivePointerId(
        /* [out] */ Int32* id);

    /**
     * @return The minimum distance in pixels that the user must travel to initiate a drag
     */
    CARAPI GetTouchSlop(
        /* [out] */ Int32* touchSlop);

    /**
     * The result of a call to this method is equivalent to
     * {@link #processTouchEvent(android.view.MotionEvent)} receiving an ACTION_CANCEL event.
     */
    CARAPI Cancel();

    /**
     * {@link #cancel()}, but also abort all motion in progress and snap to the end of any
     * animation.
     */
    CARAPI Abort();

    /**
     * Animate the view <code>child</code> to the given (left, top) position.
     * If this method returns true, the caller should invoke {@link #continueSettling(boolean)}
     * on each subsequent frame to continue the motion until it returns false. If this method
     * returns false there is no further work to do to complete the movement.
     *
     * <p>This operation does not count as a capture event, though {@link #getCapturedView()}
     * will still report the sliding view while the slide is in progress.</p>
     *
     * @param child Child view to capture and animate
     * @param constLeft const left position of child
     * @param constTop const top position of child
     * @return true if animation should continue through {@link #continueSettling(boolean)} calls
     */
    CARAPI SmoothSlideViewTo(
        /* [in] */ IView* child,
        /* [in] */ Int32 constLeft,
        /* [in] */ Int32 constTop,
        /* [out] */ Boolean* result);

    /**
     * Settle the captured view at the given (left, top) position.
     * The appropriate velocity from prior motion will be taken into account.
     * If this method returns true, the caller should invoke {@link #continueSettling(boolean)}
     * on each subsequent frame to continue the motion until it returns false. If this method
     * returns false there is no further work to do to complete the movement.
     *
     * @param constLeft Settled left edge position for the captured view
     * @param constTop Settled top edge position for the captured view
     * @return true if animation should continue through {@link #continueSettling(boolean)} calls
     */
    CARAPI SettleCapturedViewAt(
        /* [in] */ Int32 constLeft,
        /* [in] */ Int32 constTop,
        /* [out] */ Boolean* result);

    /**
     * Settle the captured view based on standard free-moving fling behavior.
     * The caller should invoke {@link #continueSettling(boolean)} on each subsequent frame
     * to continue the motion until it returns false.
     *
     * @param minLeft Minimum X position for the view's left edge
     * @param minTop Minimum Y position for the view's top edge
     * @param maxLeft Maximum X position for the view's left edge
     * @param maxTop Maximum Y position for the view's top edge
     */
    CARAPI FlingCapturedView(
        /* [in] */ Int32 minLeft,
        /* [in] */ Int32 minTop,
        /* [in] */ Int32 maxLeft,
        /* [in] */ Int32 maxTop);

    /**
     * Settle the captured view based on standard free-moving fling behavior.
     * The caller should invoke {@link #continueSettling(boolean)} on each subsequent frame
     * to continue the motion until it returns false.
     *
     * @param minLeft Minimum X position for the view's left edge
     * @param minTop Minimum Y position for the view's top edge
     * @param maxLeft Maximum X position for the view's left edge
     * @param maxTop Maximum Y position for the view's top edge
     * @param yvel the Y velocity to fling with
     */
    CARAPI FlingCapturedView(
        /* [in] */ Int32 minLeft,
        /* [in] */ Int32 minTop,
        /* [in] */ Int32 maxLeft,
        /* [in] */ Int32 maxTop,
        /* [in] */ Int32 yvel);

    /**
     * Predict how far a fling with {@param yvel} will cause the view to travel from stand still.
     * @return predicted y offset
     */
    CARAPI PredictFlingYOffset(
        /* [in] */ Int32 yvel,
        /* [out] */ Int32* yOffset);

    /**
     * Move the captured settling view by the appropriate amount for the current time.
     * If <code>continueSettling</code> returns true, the caller should call it again
     * on the next frame to continue.
     *
     * @param deferCallbacks true if state callbacks should be deferred via posted message.
     *                       Set this to true if you are calling this method from
     *                       {@link android.view.View#computeScroll()} or similar methods
     *                       invoked as part of layout or drawing.
     * @return true if settle is still in progress
     */
    CARAPI ContinueSettling(
        /* [in] */ Boolean deferCallbacks,
        /* [out] */ Boolean* result);

    CARAPI ProcessNestedFling(
        /* [in] */ IView* target,
        /* [in] */ Int32 yvel);

    CARAPI GetVelocityMagnitude(
        /* [out] */ Int32* magnitude);

    /**
     * Check if the given pointer ID represents a pointer that is currently down (to the best
     * of the ViewDragHelper's knowledge).
     *
     * <p>The state used to report this information is populated by the methods
     * {@link #shouldInterceptTouchEvent(android.view.MotionEvent)} or
     * {@link #processTouchEvent(android.view.MotionEvent)}. If one of these methods has not
     * been called for all relevant MotionEvents to track, the information reported
     * by this method may be stale or incorrect.</p>
     *
     * @param pointerId pointer ID to check; corresponds to IDs provided by MotionEvent
     * @return true if the pointer with the given ID is still down
     */
    CARAPI IsPointerDown(
        /* [in] */ Int32 pointerId,
        /* [out] */ Boolean* result);

    CARAPI_(void) SetDragState(
        /* [in] */ Int32 state);

    /**
     * Attempt to capture the view with the given pointer ID. The callback will be involved.
     * This will put us into the "dragging" state. If we've already captured this view with
     * this pointer this method will immediately return true without consulting the callback.
     *
     * @param toCapture View to capture
     * @param pointerId Pointer to capture with
     * @return true if capture was successful
     */
    CARAPI_(Boolean) TryCaptureViewForDrag(
        /* [in] */ IView* toCapture,
        /* [in] */ Int32 pointerId);

    /**
     * Check if this event as provided to the parent view's onInterceptTouchEvent should
     * cause the parent to intercept the touch event stream.
     *
     * @param ev MotionEvent provided to onInterceptTouchEvent
     * @return true if the parent view should return true from onInterceptTouchEvent
     */
    CARAPI ShouldInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    /**
     * Process a touch event received by the parent view. This method will dispatch callback events
     * as needed before returning. The parent view's onTouchEvent implementation should call this.
     *
     * @param ev The touch event received by the parent view
     */
    CARAPI ProcessTouchEvent(
        /* [in] */ IMotionEvent* ev);

    /**
     * Check if any pointer tracked in the current gesture has crossed
     * the required slop threshold.
     *
     * <p>This depends on internal state populated by
     * {@link #shouldInterceptTouchEvent(android.view.MotionEvent)} or
     * {@link #processTouchEvent(android.view.MotionEvent)}. You should only rely on
     * the results of this method after all currently available touch data
     * has been provided to one of these two methods.</p>
     *
     * @param directions Combination of direction flags, see {@link #DIRECTION_HORIZONTAL},
     *                   {@link #DIRECTION_VERTICAL}, {@link #DIRECTION_ALL}
     * @return true if the slop threshold has been crossed, false otherwise
     */
    CARAPI CheckTouchSlop(
        /* [in] */ Int32 directions,
        /* [out] */ Boolean* result);

    /**
     * Check if the specified pointer tracked in the current gesture has crossed
     * the required slop threshold.
     *
     * <p>This depends on internal state populated by
     * {@link #shouldInterceptTouchEvent(android.view.MotionEvent)} or
     * {@link #processTouchEvent(android.view.MotionEvent)}. You should only rely on
     * the results of this method after all currently available touch data
     * has been provided to one of these two methods.</p>
     *
     * @param directions Combination of direction flags, see {@link #DIRECTION_HORIZONTAL},
     *                   {@link #DIRECTION_VERTICAL}, {@link #DIRECTION_ALL}
     * @param pointerId ID of the pointer to slop check as specified by MotionEvent
     * @return true if the slop threshold has been crossed, false otherwise
     */
    CARAPI CheckTouchSlop(
        /* [in] */ Int32 directions,
        /* [in] */ Int32 pointerId,
        /* [out] */ Boolean* result);

    /**
     * Check if any of the edges specified were initially touched in the currently active gesture.
     * If there is no currently active gesture this method will return false.
     *
     * @param edges Edges to check for an initial edge touch. See {@link #EDGE_LEFT},
     *              {@link #EDGE_TOP}, {@link #EDGE_RIGHT}, {@link #EDGE_BOTTOM} and
     *              {@link #EDGE_ALL}
     * @return true if any of the edges specified were initially touched in the current gesture
     */
    CARAPI IsEdgeTouched(
        /* [in] */ Int32 edges,
        /* [out] */ Boolean* result);

    /**
     * Check if any of the edges specified were initially touched by the pointer with
     * the specified ID. If there is no currently active gesture or if there is no pointer with
     * the given ID currently down this method will return false.
     *
     * @param edges Edges to check for an initial edge touch. See {@link #EDGE_LEFT},
     *              {@link #EDGE_TOP}, {@link #EDGE_RIGHT}, {@link #EDGE_BOTTOM} and
     *              {@link #EDGE_ALL}
     * @return true if any of the edges specified were initially touched in the current gesture
     */
    CARAPI IsEdgeTouched(
        /* [in] */ Int32 edges,
        /* [in] */ Int32 pointerId,
        /* [out] */ Boolean* result);

    /**
     * Determine if the currently captured view is under the given point in the
     * parent view's coordinate system. If there is no captured view this method
     * will return false.
     *
     * @param x X position to test in the parent's coordinate system
     * @param y Y position to test in the parent's coordinate system
     * @return true if the captured view is under the given point, false otherwise
     */
    CARAPI IsCapturedViewUnder(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out] */ Boolean* result);

    /**
     * Determine if the supplied view is under the given point in the
     * parent view's coordinate system.
     *
     * @param view Child view of the parent to hit test
     * @param x X position to test in the parent's coordinate system
     * @param y Y position to test in the parent's coordinate system
     * @return true if the supplied view is under the given point, false otherwise
     */
    CARAPI IsViewUnder(
        /* [in] */ IView* view,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out] */ Boolean* result);

    /**
     * Find the topmost child under the given point within the parent view's coordinate system.
     * The child order is determined using {@link Callback#getOrderedChildIndex(Int32)}.
     *
     * @param x X position to test in the parent's coordinate system
     * @param y Y position to test in the parent's coordinate system
     * @return The topmost child view under (x, y) or null if none found.
     */
    CARAPI FindTopChildUnder(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y
        /* [out] */ IView** view);

    /**
     * Prepares the {@link ViewDragHelper} for the beginning of a nested scroll.
     *
     * @param target The child view that is dispatching the nested scroll.
     */
    CARAPI StartNestedScroll(
        /* [in] */ IView* target);

    /**
     * Informs the {@link ViewDragHelper} that a nested scroll has ended.
     *
     * @param target The child view that is dispatching the nested scroll.
     */
    CARAPI StopNestedScroll(
        /* [in] */ IView* target);

    /**
     * Update the {@link ViewDragHelper} with a new nested scrolling event.
     *
     * @param target The child view that is dispatching the nested scroll.
     * @param dx The x distance scrolled on the child, in pixels.
     * @param dy The y distance scroll on the child, in pixels.
     * @param consumed An Int32 array for the {@link ViewDragHelper} to report back the scroll
     *         deltas that it consumed.
     */
    CARAPI ProcessNestedScroll(
        /* [in] */ IView* target,
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy,
        /* [in] */ ArrayOf<Int32>* consumed);

protected:
    /**
     * Tests scrollability within child views of v given a delta of dx.
     *
     * @param v View to test for horizontal scrollability
     * @param checkV Whether the view v passed should itself be checked for scrollability (true),
     *               or just its children (false).
     * @param dx Delta scrolled in pixels along the X axis
     * @param dy Delta scrolled in pixels along the Y axis
     * @param x X coordinate of the active touch point
     * @param y Y coordinate of the active touch point
     * @return true if child views of v can be scrolled by delta of dx.
     */
     CARAPI_(Boolean) CanScroll(
        /* [in] */ IView* v,
        /* [in] */ Boolean checkV,
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

private:
    /**
     * Apps should use ViewDragHelper.create() to get a new instance.
     * This will allow VDH to use internal compatibility implementations for different
     * platform versions.
     *
     * @param context Context to initialize config-dependent params from
     * @param forParent Parent view to monitor
     */
    ViewDragHelper(
        /* [in] */ IContext* context,
        /* [in] */ IViewGroup* forParent,
        /* [in] */ IViewDragHelperCallback* cb);

    /**
     * Settle the captured view at the given (left, top) position.
     *
     * @param constLeft Target left position for the captured view
     * @param constTop Target top position for the captured view
     * @param xvel Horizontal velocity
     * @param yvel Vertical velocity
     * @return true if animation should continue through {@link #continueSettling(boolean)} calls
     */
    CARAPI_(Boolean) ForceSettleCapturedViewAt(
        /* [in] */ Int32 constLeft,
        /* [in] */ Int32 constTop,
        /* [in] */ Int32 xvel,
        /* [in] */ Int32 yvel);

    CARAPI_(Int32) ComputeSettleDuration(
        /* [in] */ IView* child,
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy,
        /* [in] */ Int32 xvel,
        /* [in] */ Int32 yvel);

    CARAPI_(Int32) ComputeAxisDuration(
        /* [in] */ Int32 delta,
        /* [in] */ Int32 velocity,
        /* [in] */ Int32 motionRange);

    /**
     * Clamp the magnitude of value for absMin and absMax.
     * If the value is below the minimum, it will be clamped to zero.
     * If the value is above the maximum, it will be clamped to the maximum.
     *
     * @param value Value to clamp
     * @param absMin Absolute value of the minimum significant value to return
     * @param absMax Absolute value of the maximum value to return
     * @return The clamped value with the same sign as <code>value</code>
     */
    CARAPI_(Int32) ClampMag(
        /* [in] */ Int32 value,
        /* [in] */ Int32 absMin,
        /* [in] */ Int32 absMax);

    /**
     * Clamp the magnitude of value for absMin and absMax.
     * If the value is below the minimum, it will be clamped to zero.
     * If the value is above the maximum, it will be clamped to the maximum.
     *
     * @param value Value to clamp
     * @param absMin Absolute value of the minimum significant value to return
     * @param absMax Absolute value of the maximum value to return
     * @return The clamped value with the same sign as <code>value</code>
     */
    CARAPI_(Float) ClampMag(
        /* [in] */ Float value,
        /* [in] */ Float absMin,
        /* [in] */ Float absMax);

    CARAPI_(Float) DistanceInfluenceForSnapDuration(
        /* [in] */ Float f);

    CARAPI_(void) DispatchViewFling(
        /* [in] */ Float xvel,
        /* [in] */ Float yvel);

    /**
     * Like all callback events this must happen on the UI thread, but release
     * involves some extra semantics. During a release (mReleaseInProgress)
     * is the only time it is valid to call {@link #settleCapturedViewAt(Int32, Int32)}
     * or {@link #flingCapturedView(Int32, Int32, Int32, Int32)}.
     */
    CARAPI_(void) DispatchViewReleased(
        /* [in] */ Float xvel,
        /* [in] */ Float yvel);

    CARAPI_(void) ClearMotionHistory();

    CARAPI_(void) ClearMotionHistory(
        /* [in] */ Int32 pointerId);

    CARAPI_(void) EnsureMotionHistorySizeForId(
        /* [in] */ Int32 pointerId);

    CARAPI_(void) SaveInitialMotion(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Int32 pointerId);

    CARAPI_(void) SaveLastMotion(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(void) ReportNewEdgeDrags(
        /* [in] */ Float dx,
        /* [in] */ Float dy,
        /* [in] */ Int32 pointerId);

    CARAPI_(Boolean) CheckNewEdgeDrag(
        /* [in] */ Float delta,
        /* [in] */ Float odelta,
        /* [in] */ Int32 pointerId,
        /* [in] */ Int32 edge);

    /**
     * Check if we've crossed a reasonable touch slop for the given child view.
     * If the child cannot be dragged along the horizontal or vertical axis, motion
     * along that axis will not count toward the slop check.
     *
     * @param child Child to check
     * @param dx Motion since initial position along X axis
     * @param dy Motion since initial position along Y axis
     * @return true if the touch slop has been crossed
     */
    CARAPI_(Boolean) CheckTouchSlop(
        /* [in] */ IView* child,
        /* [in] */ Float dx,
        /* [in] */ Float dy);

    CARAPI_(void) ReleaseViewForPointerUp();

    CARAPI_(void) DragTo(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy);

    CARAPI_(Int32) GetEdgesTouched(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

private:
    static const String TAG; // = "ViewDragHelper";

    static const Int32 EDGE_SIZE; // = 20; // dp

    static const Int32 BASE_SETTLE_DURATION; // = 256; // ms
    static const Int32 MAX_SETTLE_DURATION; // = 600; // ms

    // Current drag state; idle, dragging or settling
    Int32 mDragState;

    // Distance to travel before a drag may begin
    Int32 mTouchSlop;

    // Last known position/pointer tracking
    Int32 mActivePointerId; // = INVALID_POINTER;
    AutoPtr<ArrayOf<Float> > mInitialMotionX;
    AutoPtr<ArrayOf<Float> > mInitialMotionY;
    AutoPtr<ArrayOf<Float> > mLastMotionX;
    AutoPtr<ArrayOf<Float> > mLastMotionY;
    AutoPtr<ArrayOf<Int32> > mInitialEdgesTouched;
    AutoPtr<ArrayOf<Int32> > mEdgeDragsInProgress;
    AutoPtr<ArrayOf<Int32> > mEdgeDragsLocked;
    Int32 mPointersDown;

    AutoPtr<IVelocityTracker> mVelocityTracker;
    Float mMaxVelocity;
    Float mMinVelocity;

    Int32 mEdgeSize;
    Int32 mTrackingEdges;

    // We need to use a Scroller instead of an OverScroller (b/17700698) and as a result, we need
    // to keep track of the const scroll position ourselves in mconstScrollY (b/17704016) whenever
    // we programmatically scroll or fling mScroller.
    AutoPtr<IScroller> mScroller;
    Int32 mconstScrollY;

    AutoPtr<IViewDragHelperCallback> mCallback;

    AutoPtr<IView> mCapturedView;
    Boolean mReleaseInProgress;

    AutoPtr<IViewGroup> mParentView;

    AutoPtr<IRunnable> mSetIdleRunnable;
};

} // Widget
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_WIDGET_VIEWDRAGHELPER_H__
