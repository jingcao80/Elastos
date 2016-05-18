
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_AUTOSCROLLHELPER_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_AUTOSCROLLHELPER_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Runnable.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::Widget::IAbsListView;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

/**
 * Used by dialogs to change the font size and number of lines to try to fit
 * the text to the available space.
 */
class AutoScrollHelper
    : public Object
    , public IAutoScrollHelper
    , public IViewOnTouchListener
{
private:
    class ScrollAnimationRunnable
        : public Runnable
    {
    public:
        ScrollAnimationRunnable(
            /* [in] */ AutoScrollHelper* host);

        ~ScrollAnimationRunnable();

        // @Override
        virtual CARAPI Run();

    private:
        AutoScrollHelper* mHost;
    };

    /**
     * Scroller whose velocity follows the curve of an {@link Interpolator} and
     * is clamped to the interpolated 0f value before starting and the
     * interpolated 1f value after a specified duration.
     */
    class ClampedScroller
        : public Object
    {
        friend class AutoScrollHelper;
    public:
        ClampedScroller(
            /* [in] */ AutoScrollHelper* host);

        ~ClampedScroller();

        CARAPI_(void) SetRampUpDuration(
            /* [in] */ Int32 durationMillis);

        CARAPI_(void) SetRampDownDuration(
            /* [in] */ Int32 durationMillis);

        /**
         * Starts the scroller at the current animation time.
         */
        CARAPI_(void) Start();

        /**
         * Stops the scroller at the current animation time.
         */
        CARAPI_(void) RequestStop();

        CARAPI_(Boolean) IsFinished();

        /**
         * Computes the current scroll deltas. This usually only be called after
         * starting the scroller with {@link #start()}.
         *
         * @see #getDeltaX()
         * @see #getDeltaY()
         */
        CARAPI ComputeScrollDelta();

        /**
         * Sets the target velocity for this scroller.
         *
         * @param x The target X velocity in pixels per millisecond.
         * @param y The target Y velocity in pixels per millisecond.
         */
        CARAPI_(void) SetTargetVelocity(
            /* [in] */ Float x,
            /* [in] */ Float y);

        CARAPI_(Int32) GetHorizontalDirection();

        CARAPI_(Int32) GetVerticalDirection();

        /**
         * The distance traveled in the X-coordinate computed by the last call
         * to {@link #computeScrollDelta()}.
         */
        CARAPI_(Int32) GetDeltaX();

        /**
         * The distance traveled in the Y-coordinate computed by the last call
         * to {@link #computeScrollDelta()}.
         */
        CARAPI_(Int32) GetDeltaY();

    private:
        CARAPI_(Float) GetValueAt(
            /* [in] */ Int64 currentTime);

        /**
         * Interpolates the value along a parabolic curve corresponding to the equation
         * <code>y = -4x * (x-1)</code>.
         *
         * @param value The value to interpolate, between 0 and 1.
         * @return the interpolated value, between 0 and 1.
         */
        CARAPI_(Float) InterpolateValue(
            /* [in] */ Float value);

    private:
        AutoScrollHelper* mHost;
        Int32 mRampUpDuration;
        Int32 mRampDownDuration;
        Float mTargetVelocityX;
        Float mTargetVelocityY;

        Int64 mStartTime;

        Int64 mDeltaTime;
        Int32 mDeltaX;
        Int32 mDeltaY;

        Int64 mStopTime;
        Float mStopValue;
        Int32 mEffectiveRampDown;
    };

public:
    CAR_INTERFACE_DECL()

    AutoScrollHelper();

    ~AutoScrollHelper();

    /**
     * Creates a new helper for scrolling the specified target view.
     * <p>
     * The resulting helper may be configured by chaining setter calls and
     * should be set as a touch listener on the target view.
     * <p>
     * By default, the helper is disabled and will not respond to touch events
     * until it is enabled using {@link #setEnabled}.
     *
     * @param target The view to automatically scroll.
     */
    CARAPI constructor(
        /* [in] */ IView* target);

    /**
     * Sets whether the scroll helper is enabled and should respond to touch
     * events.
     *
     * @param enabled Whether the scroll helper is enabled.
     * @return The scroll helper, which may used to chain setter calls.
     */
    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    /**
     * @return True if this helper is enabled and responding to touch events.
     */
    CARAPI IsEnabled(
        /* [out] */ Boolean* enabled);

    /**
     * Enables or disables exclusive handling of touch events during scrolling.
     * By default, exclusive handling is disabled and the target view receives
     * all touch events.
     * <p>
     * When enabled, {@link #onTouch} will return true if the helper is
     * currently scrolling and false otherwise.
     *
     * @param exclusive True to exclusively handle touch events during scrolling,
     *            false to allow the target view to receive all touch events.
     * @return The scroll helper, which may used to chain setter calls.
     */
    CARAPI SetExclusive(
        /* [in] */ Boolean exclusive);

    /**
     * Indicates whether the scroll helper handles touch events exclusively
     * during scrolling.
     *
     * @return True if exclusive handling of touch events during scrolling is
     *         enabled, false otherwise.
     * @see #setExclusive(Boolean)
     */
    CARAPI IsExclusive(
        /* [out] */ Boolean* isExclusive);

    /**
     * Sets the absolute maximum scrolling velocity.
     * <p>
     * If relative velocity is not specified, scrolling will always reach the
     * same maximum velocity. If both relative and maximum velocities are
     * specified, the maximum velocity will be used to clamp the calculated
     * relative velocity.
     *
     * @param horizontalMax The maximum horizontal scrolling velocity, or
     *            {@link #NO_MAX} to leave the relative value unconstrained.
     * @param verticalMax The maximum vertical scrolling velocity, or
     *            {@link #NO_MAX} to leave the relative value unconstrained.
     * @return The scroll helper, which may used to chain setter calls.
     */
    CARAPI SetMaximumVelocity(
        /* [in] */ Float horizontalMax,
        /* [in] */ Float verticalMax);

    /**
     * Sets the absolute minimum scrolling velocity.
     * <p>
     * If both relative and minimum velocities are specified, the minimum
     * velocity will be used to clamp the calculated relative velocity.
     *
     * @param horizontalMin The minimum horizontal scrolling velocity, or
     *            {@link #NO_MIN} to leave the relative value unconstrained.
     * @param verticalMin The minimum vertical scrolling velocity, or
     *            {@link #NO_MIN} to leave the relative value unconstrained.
     * @return The scroll helper, which may used to chain setter calls.
     */
    CARAPI SetMinimumVelocity(
        /* [in] */ Float horizontalMin,
        /* [in] */ Float verticalMin);

    /**
     * Sets the target scrolling velocity relative to the host view's
     * dimensions.
     * <p>
     * If both relative and maximum velocities are specified, the maximum
     * velocity will be used to clamp the calculated relative velocity.
     *
     * @param horizontal The target horizontal velocity as a fraction of the
     *            host view width per second, or {@link #RELATIVE_UNSPECIFIED}
     *            to ignore.
     * @param vertical The target vertical velocity as a fraction of the host
     *            view height per second, or {@link #RELATIVE_UNSPECIFIED} to
     *            ignore.
     * @return The scroll helper, which may used to chain setter calls.
     */
    CARAPI SetRelativeVelocity(
        /* [in] */ Float horizontal,
        /* [in] */ Float vertical);

    /**
     * Sets the activation edge type, one of:
     * <ul>
     * <li>{@link #EDGE_TYPE_INSIDE} for edges that respond to touches inside
     * the bounds of the host view. If touch moves outside the bounds, scrolling
     * will stop.
     * <li>{@link #EDGE_TYPE_INSIDE_EXTEND} for inside edges that continued to
     * scroll when touch moves outside the bounds of the host view.
     * <li>{@link #EDGE_TYPE_OUTSIDE} for edges that only respond to touches
     * that move outside the bounds of the host view.
     * </ul>
     *
     * @param type The type of edge to use.
     * @return The scroll helper, which may used to chain setter calls.
     */
    CARAPI SetEdgeType(
        /* [in] */ Int32 type);

    /**
     * Sets the activation edge size relative to the host view's dimensions.
     * <p>
     * If both relative and maximum edges are specified, the maximum edge will
     * be used to constrain the calculated relative edge size.
     *
     * @param horizontal The horizontal edge size as a fraction of the host view
     *            width, or {@link #RELATIVE_UNSPECIFIED} to always use the
     *            maximum value.
     * @param vertical The vertical edge size as a fraction of the host view
     *            height, or {@link #RELATIVE_UNSPECIFIED} to always use the
     *            maximum value.
     * @return The scroll helper, which may used to chain setter calls.
     */
    CARAPI SetRelativeEdges(
        /* [in] */ Float horizontal,
        /* [in] */ Float vertical);

    /**
     * Sets the absolute maximum edge size.
     * <p>
     * If relative edge size is not specified, activation edges will always be
     * the maximum edge size. If both relative and maximum edges are specified,
     * the maximum edge will be used to constrain the calculated relative edge
     * size.
     *
     * @param horizontalMax The maximum horizontal edge size in pixels, or
     *            {@link #NO_MAX} to use the unconstrained calculated relative
     *            value.
     * @param verticalMax The maximum vertical edge size in pixels, or
     *            {@link #NO_MAX} to use the unconstrained calculated relative
     *            value.
     * @return The scroll helper, which may used to chain setter calls.
     */
    CARAPI SetMaximumEdges(
        /* [in] */ Float horizontalMax,
        /* [in] */ Float verticalMax);

    /**
     * Sets the delay after entering an activation edge before activation of
     * auto-scrolling. By default, the activation delay is set to
     * {@link ViewConfiguration#getTapTimeout()}.
     * <p>
     * Specifying a delay of zero will start auto-scrolling immediately after
     * the touch position enters an activation edge.
     *
     * @param delayMillis The activation delay in milliseconds.
     * @return The scroll helper, which may used to chain setter calls.
     */
    CARAPI SetActivationDelay(
        /* [in] */ Int32 delayMillis);

    /**
     * Sets the amount of time after activation of auto-scrolling that is takes
     * to reach target velocity for the current touch position.
     * <p>
     * Specifying a duration greater than zero prevents sudden jumps in
     * velocity.
     *
     * @param durationMillis The ramp-up duration in milliseconds.
     * @return The scroll helper, which may used to chain setter calls.
     */
    CARAPI SetRampUpDuration(
        /* [in] */ Int32 durationMillis);

    /**
     * Sets the amount of time after de-activation of auto-scrolling that is
     * takes to slow to a stop.
     * <p>
     * Specifying a duration greater than zero prevents sudden jumps in
     * velocity.
     *
     * @param durationMillis The ramp-down duration in milliseconds.
     * @return The scroll helper, which may used to chain setter calls.
     */
    CARAPI SetRampDownDuration(
        /* [in] */ Int32 durationMillis);


    /**
     * Handles touch events by activating automatic scrolling, adjusting scroll
     * velocity, or stopping.
     * <p>
     * If {@link #isExclusive()} is false, always returns false so that
     * the host view may handle touch events. Otherwise, returns true when
     * automatic scrolling is active and false otherwise.
     */
    // @Override
    CARAPI OnTouch(
        /* [in] */ IView* v,
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Override this method to scroll the target view by the specified number of
     * pixels.
     *
     * @param deltaX The number of pixels to scroll by horizontally.
     * @param deltaY The number of pixels to scroll by vertically.
     */
    CARAPI ScrollTargetBy(
        /* [in] */ Int32 deltaX,
        /* [in] */ Int32 deltaY) = 0;

    /**
     * Override this method to return whether the target view can be scrolled
     * horizontally in a certain direction.
     *
     * @param direction Negative to check scrolling left, positive to check
     *            scrolling right.
     * @return true if the target view is able to horizontally scroll in the
     *         specified direction.
     */
    CARAPI CanTargetScrollHorizontally(
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* res) = 0;

    /**
     * Override this method to return whether the target view can be scrolled
     * vertically in a certain direction.
     *
     * @param direction Negative to check scrolling up, positive to check
     *            scrolling down.
     * @return true if the target view is able to vertically scroll in the
     *         specified direction.
     */
    CARAPI CanTargetScrollVertically(
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* res) = 0;

private:
    /**
     * @return whether the target is able to scroll in the requested direction
     */
    CARAPI_(Boolean) ShouldAnimate();

    /**
     * Starts the scroll animation.
     */
    CARAPI_(void) StartAnimating();

    /**
     * Requests that the scroll animation slow to a stop. If there is an
     * activation delay, this may occur between posting the animation and
     * actually running it.
     */
    CARAPI_(void) RequestStop();

    CARAPI_(Float) ComputeTargetVelocity(
        /* [in] */ Int32 direction,
        /* [in] */ Float coordinate,
        /* [in] */ Float srcSize,
        /* [in] */ Float dstSize);

    /**
     * Returns the interpolated position of a touch point relative to an edge
     * defined by its relative inset, its maximum absolute inset, and the edge
     * interpolator.
     *
     * @param relativeValue The size of the inset relative to the total size.
     * @param size Total size.
     * @param maxValue The maximum size of the inset, used to clamp (relative *
     *            total).
     * @param current Touch position within within the total size.
     * @return Interpolated value of the touch position within the edge.
     */
    CARAPI_(Float) GetEdgeValue(
        /* [in] */ Float relativeValue,
        /* [in] */ Float size,
        /* [in] */ Float maxValue,
        /* [in] */ Float current);

    CARAPI_(Float) ConstrainEdgeValue(
        /* [in] */ Float current,
        /* [in] */ Float leading);

    static CARAPI_(Int32) Constrain(
        /* [in] */ Int32 value,
        /* [in] */ Int32 min,
        /* [in] */ Int32 max);

    static CARAPI_(Float) Constrain(
        /* [in] */ Float value,
        /* [in] */ Float min,
        /* [in] */ Float max);

    /**
     * Sends a {@link MotionEvent#ACTION_CANCEL} event to the target view,
     * canceling any ongoing touch events.
     */
    CARAPI_(void) CancelTargetTouch();

public:
    static const Float NO_MAX;

private:
    static const Int32 HORIZONTAL = 0;
    static const Int32 VERTICAL = 1;

    /** Scroller used to control acceleration toward maximum velocity. */
    AutoPtr<ClampedScroller> mScroller;

    /** Interpolator used to scale velocity with touch position. */
    AutoPtr<IInterpolator> mEdgeInterpolator;

    /** The view to auto-scroll. Might not be the source of touch events. */
    AutoPtr<IView> mTarget;

    /** Runnable used to animate scrolling. */
    AutoPtr<IRunnable> mRunnable;

    /** Edge insets used to activate auto-scrolling. */
    AutoPtr< ArrayOf<Float> > mRelativeEdges;

    /** Clamping values for edge insets used to activate auto-scrolling. */
    AutoPtr< ArrayOf<Float> > mMaximumEdges;

    /** The type of edge being used. */
    Int32 mEdgeType;

    /** Delay after entering an activation edge before auto-scrolling begins. */
    Int32 mActivationDelay;

    /** Relative scrolling velocity at maximum edge distance. */
    AutoPtr< ArrayOf<Float> > mRelativeVelocity;

    /** Clamping values used for scrolling velocity. */
    AutoPtr< ArrayOf<Float> > mMinimumVelocity;

    /** Clamping values used for scrolling velocity. */
    AutoPtr< ArrayOf<Float> > mMaximumVelocity;

    /** Whether to start activation immediately. */
    Boolean mAlreadyDelayed;

    /** Whether to reset the scroller start time on the next animation. */
    Boolean mNeedsReset;

    /** Whether to send a cancel motion event to the target view. */
    Boolean mNeedsCancel;

    /** Whether the auto-scroller is actively scrolling. */
    Boolean mAnimating;

    /** Whether the auto-scroller is enabled. */
    Boolean mEnabled;

    /** Whether the auto-scroller consumes events when scrolling. */
    Boolean mExclusive;

    // Default values.
    static const Int32 DEFAULT_EDGE_TYPE = EDGE_TYPE_INSIDE_EXTEND;
    static const Int32 DEFAULT_MINIMUM_VELOCITY_DIPS = 315;
    static const Int32 DEFAULT_MAXIMUM_VELOCITY_DIPS = 1575;
    static const Float DEFAULT_MAXIMUM_EDGE;
    static const Float DEFAULT_RELATIVE_EDGE = 0.2f;
    static const Float DEFAULT_RELATIVE_VELOCITY = 1.0f;
    static const Int32 DEFAULT_ACTIVATION_DELAY; //= ViewConfiguration.getTapTimeout();
    static const Int32 DEFAULT_RAMP_UP_DURATION = 500;
    static const Int32 DEFAULT_RAMP_DOWN_DURATION = 500;
};

class AbsListViewAutoScroller
    : public AutoScrollHelper
    , public IAbsListViewAutoScroller
{
public:
    CAR_INTERFACE_DECL()

    AbsListViewAutoScroller();

    ~AbsListViewAutoScroller();

    CARAPI constructor(
        /* [in] */ IAbsListView* target);

    // @Override
    CARAPI ScrollTargetBy(
        /* [in] */ Int32 deltaX,
        /* [in] */ Int32 deltaY);

    // @Override
    CARAPI CanTargetScrollHorizontally(
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* res);

    // @Override
    CARAPI CanTargetScrollVertically(
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* res);

private:
    AutoPtr<IAbsListView> mTarget;
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_WIDGET_AUTOSCROLLHELPER_H__
