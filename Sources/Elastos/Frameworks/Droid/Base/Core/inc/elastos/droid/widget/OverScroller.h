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

#ifndef __ELASTOS_DROID_WIDGET_OVERSCROLLER_H__
#define __ELASTOS_DROID_WIDGET_OVERSCROLLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Widget {

class SplineOverScroller;

/**
 * This class encapsulates scrolling with the ability to overshoot the bounds
 * of a scrolling operation. This class is a drop-in replacement for
 * {@link android.widget.Scroller} in most cases.
 */
class OverScroller
    : public Object
    , public IOverScroller
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates an OverScroller with a viscous fluid scroll interpolator and flywheel.
     * @param context
     */
    OverScroller();

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Creates an OverScroller with flywheel enabled.
     * @param context The context of this application.
     * @param interpolator The scroll interpolator. If null, a default (viscous) interpolator will
     * be used.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Elastos::Droid::View::Animation::IInterpolator* interpolator);

    /**
     * Creates an OverScroller.
     * @param context The context of this application.
     * @param interpolator The scroll interpolator. If null, a default (viscous) interpolator will
     * be used.
     * @param flywheel If TRUE, successive Fling motions will keep on increasing scroll speed.
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Elastos::Droid::View::Animation::IInterpolator* interpolator,
        /* [in] */ Boolean flywheel);

    /**
     * Creates an OverScroller with flywheel enabled.
     * @param context The context of this application.
     * @param interpolator The scroll interpolator. If null, a default (viscous) interpolator will
     * be used.
     * @param bounceCoefficientX A value between 0 and 1 that will determine the proportion of the
     * velocity which is preserved in the bounce when the horizontal edge is reached. A null value
     * means no bounce. This behavior is no longer supported and this coefficient has no effect.
     * @param bounceCoefficientY Same as bounceCoefficientX but for the vertical direction. This
     * behavior is no longer supported and this coefficient has no effect.
     * !deprecated Use {!link #OverScroller(Context, Interpolator, Boolean)} instead.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Elastos::Droid::View::Animation::IInterpolator* interpolator,
        /* [in] */ Float bounceCoefficientX,
        /* [in] */ Float bounceCoefficientY);

    /**
     * Creates an OverScroller.
     * @param context The context of this application.
     * @param interpolator The scroll interpolator. If null, a default (viscous) interpolator will
     * be used.
     * @param bounceCoefficientX A value between 0 and 1 that will determine the proportion of the
     * velocity which is preserved in the bounce when the horizontal edge is reached. A null value
     * means no bounce. This behavior is no longer supported and this coefficient has no effect.
     * @param bounceCoefficientY Same as bounceCoefficientX but for the vertical direction. This
     * behavior is no longer supported and this coefficient has no effect.
     * @param flywheel If TRUE, successive Fling motions will keep on increasing scroll speed.
     * !deprecated Use {!link OverScroller(Context, Interpolator, Boolean)} instead.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Elastos::Droid::View::Animation::IInterpolator* interpolator,
        /* [in] */ Float bounceCoefficientX,
        /* [in] */ Float bounceCoefficientY,
        /* [in] */ Boolean flywheel);

    CARAPI SetInterpolator(
        /* [in] */ Elastos::Droid::View::Animation::IInterpolator* interpolator);

    /**
     * The amount of friction applied to flings. The default value
     * is {@link ViewConfiguration#getScrollFriction}.
     *
     * @param friction A scalar dimension-less value representing the coefficient of
     *         friction.
     */
    CARAPI SetFriction(
        /* [in] */ Float friction);

    /**
     *
     * Returns whether the scroller has finished scrolling.
     *
     * @return True if the scroller has finished scrolling, false otherwise.
     */
    CARAPI IsFinished(
        /* [out] */ Boolean* finished);

    /**
     * Force the finished field to a particular value. Contrary to
     * {@link #abortAnimation()}, forcing the animation to finished
     * does NOT cause the scroller to move to the final x and y
     * position.
     *
     * @param finished The new finished value.
     */
    CARAPI ForceFinished(
        /* [in] */ Boolean finished);

    /**
     * Returns the current X offset in the scroll.
     *
     * @return The new X offset as an absolute distance from the origin.
     */
    CARAPI GetCurrX(
        /* [out] */ Int32* currX);

    /**
     * Returns the current Y offset in the scroll.
     *
     * @return The new Y offset as an absolute distance from the origin.
     */
    CARAPI GetCurrY(
        /* [out] */ Int32* currY);

    /**
     * Returns the absolute value of the current velocity.
     *
     * @return The original velocity less the deceleration, norm of the X and Y velocity vector.
     */
    CARAPI GetCurrVelocity(
        /* [out] */ Float* currVelocity);

    /**
     * Returns the start X offset in the scroll.
     *
     * @return The start X offset as an absolute distance from the origin.
     */
    CARAPI GetStartX(
        /* [out] */ Int32* startX);

    /**
     * Returns the start Y offset in the scroll.
     *
     * @return The start Y offset as an absolute distance from the origin.
     */
    CARAPI GetStartY(
        /* [out] */ Int32* startY);

    /**
     * Returns where the scroll will end. Valid only for "fling" scrolls.
     *
     * @return The final X offset as an absolute distance from the origin.
     */
    CARAPI GetFinalX(
        /* [out] */ Int32* finalX);

    /**
     * Returns where the scroll will end. Valid only for "fling" scrolls.
     *
     * @return The final Y offset as an absolute distance from the origin.
     */
    CARAPI GetFinalY(
        /* [out] */ Int32* finalY);

    /**
     * Returns how long the scroll event will take, in milliseconds.
     *
     * @return The duration of the scroll in milliseconds.
     *
     * @hide Pending removal once nothing depends on it
     * @deprecated OverScrollers don't necessarily have a fixed duration.
     *             This function will lie to the best of its ability.
     */
    CARAPI GetDuration(
        /* [out] */ Int32* duration);

    /**
     * Extend the scroll animation. This allows a running animation to scroll
     * further and longer, when used with {@link #setFinalX(int)} or {@link #setFinalY(int)}.
     *
     * @param extend Additional time to scroll in milliseconds.
     * @see #setFinalX(int)
     * @see #setFinalY(int)
     *
     * @hide Pending removal once nothing depends on it
     * @deprecated OverScrollers don't necessarily have a fixed duration.
     *             Instead of setting a new final position and extending
     *             the duration of an existing scroll, use startScroll
     *             to begin a new animation.
     */
    CARAPI ExtendDuration(
        /* [in] */ Int32 extend);

    CARAPI ExtendDuration(
        /* [in] */ Int32 extend,
        /* [out */ Int32* result);

    /**
     * Sets the final position (X) for this scroller.
     *
     * @param newX The new X offset as an absolute distance from the origin.
     * @see #extendDuration(int)
     * @see #setFinalY(int)
     *
     * @hide Pending removal once nothing depends on it
     * @deprecated OverScroller's final position may change during an animation.
     *             Instead of setting a new final position and extending
     *             the duration of an existing scroll, use startScroll
     *             to begin a new animation.
     */
    CARAPI SetFinalX(
        /* [in] */ Int32 newX);

    /**
     * Sets the final position (Y) for this scroller.
     *
     * @param newY The new Y offset as an absolute distance from the origin.
     * @see #extendDuration(int)
     * @see #setFinalX(int)
     *
     * @hide Pending removal once nothing depends on it
     * @deprecated OverScroller's final position may change during an animation.
     *             Instead of setting a new final position and extending
     *             the duration of an existing scroll, use startScroll
     *             to begin a new animation.
     */
    CARAPI SetFinalY(
        /* [in] */ Int32 newY);

    /**
     * Call this when you want to know the new location. If it returns true, the
     * animation is not yet finished.
     */
    CARAPI ComputeScrollOffset(
        /* [out] */ Boolean* scrollOffset);

    /**
     * Start scrolling by providing a starting point and the distance to travel.
     * The scroll will use the default value of 250 milliseconds for the
     * duration.
     *
     * @param startX Starting horizontal scroll offset in pixels. Positive
     *        numbers will scroll the content to the left.
     * @param startY Starting vertical scroll offset in pixels. Positive numbers
     *        will scroll the content up.
     * @param dx Horizontal distance to travel. Positive numbers will scroll the
     *        content to the left.
     * @param dy Vertical distance to travel. Positive numbers will scroll the
     *        content up.
     */
    CARAPI StartScroll(
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy);

    /**
     * Start scrolling by providing a starting point and the distance to travel.
     *
     * @param startX Starting horizontal scroll offset in pixels. Positive
     *        numbers will scroll the content to the left.
     * @param startY Starting vertical scroll offset in pixels. Positive numbers
     *        will scroll the content up.
     * @param dx Horizontal distance to travel. Positive numbers will scroll the
     *        content to the left.
     * @param dy Vertical distance to travel. Positive numbers will scroll the
     *        content up.
     * @param duration Duration of the scroll in milliseconds.
     */
    CARAPI StartScroll(
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy,
        /* [in] */ Int32 duration);

    /**
     * Call this when you want to 'spring back' into a valid coordinate range.
     *
     * @param startX Starting X coordinate
     * @param startY Starting Y coordinate
     * @param minX Minimum valid X value
     * @param maxX Maximum valid X value
     * @param minY Minimum valid Y value
     * @param maxY Minimum valid Y value
     * @return true if a springback was initiated, false if startX and startY were
     *          already within the valid range.
     */
    CARAPI SpringBack(
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ Int32 minX,
        /* [in] */ Int32 maxX,
        /* [in] */ Int32 minY,
        /* [in] */ Int32 maxY,
        /* [in] */ Boolean* result);

    /**
     * Start scrolling based on a fling gesture. The distance traveled will
     * depend on the initial velocity of the fling.
     *
     * @param startX Starting point of the scroll (X)
     * @param startY Starting point of the scroll (Y)
     * @param velocityX Initial velocity of the fling (X) measured in pixels per
     *            second.
     * @param velocityY Initial velocity of the fling (Y) measured in pixels per
     *            second
     * @param minX Minimum X value. The scroller will not scroll past this point
     *            unless overX > 0. If overfling is allowed, it will use minX as
     *            a springback boundary.
     * @param maxX Maximum X value. The scroller will not scroll past this point
     *            unless overX > 0. If overfling is allowed, it will use maxX as
     *            a springback boundary.
     * @param minY Minimum Y value. The scroller will not scroll past this point
     *            unless overY > 0. If overfling is allowed, it will use minY as
     *            a springback boundary.
     * @param maxY Maximum Y value. The scroller will not scroll past this point
     *            unless overY > 0. If overfling is allowed, it will use maxY as
     *            a springback boundary.
     * @param overX Overfling range. If > 0, horizontal overfling in either
     *            direction will be possible.
     * @param overY Overfling range. If > 0, vertical overfling in either
     *            direction will be possible.
     */

    CARAPI Fling(
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ Int32 velocityX,
        /* [in] */ Int32 velocityY,
        /* [in] */ Int32 minX,
        /* [in] */ Int32 maxX,
        /* [in] */ Int32 minY,
        /* [in] */ Int32 maxY);

    CARAPI Fling(
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ Int32 velocityX,
        /* [in] */ Int32 velocityY,
        /* [in] */ Int32 minX,
        /* [in] */ Int32 maxX,
        /* [in] */ Int32 minY,
        /* [in] */ Int32 maxY,
        /* [in] */ Int32 overX,
        /* [in] */ Int32 overY);

    /**
     * Notify the scroller that we've reached a horizontal boundary.
     * Normally the information to handle this will already be known
     * when the animation is started, such as in a call to one of the
     * fling functions. However there are cases where this cannot be known
     * in advance. This function will transition the current motion and
     * animate from startX to finalX as appropriate.
     *
     * @param startX Starting/current X position
     * @param finalX Desired final X position
     * @param overX Magnitude of overscroll allowed. This should be the maximum
     *              desired distance from finalX. Absolute value - must be positive.
     */
    CARAPI NotifyHorizontalEdgeReached(
        /* [in] */ Int32 startX,
        /* [in] */ Int32 finalX,
        /* [in] */ Int32 overX);

    /**
     * Notify the scroller that we've reached a vertical boundary.
     * Normally the information to handle this will already be known
     * when the animation is started, such as in a call to one of the
     * fling functions. However there are cases where this cannot be known
     * in advance. This function will animate a parabolic motion from
     * startY to finalY.
     *
     * @param startY Starting/current Y position
     * @param finalY Desired final Y position
     * @param overY Magnitude of overscroll allowed. This should be the maximum
     *              desired distance from finalY. Absolute value - must be positive.
     */
    CARAPI NotifyVerticalEdgeReached(
        /* [in] */ Int32 startY,
        /* [in] */ Int32 finalY,
        /* [in] */ Int32 overY);

    /**
     * Returns whether the current Scroller is currently returning to a valid position.
     * Valid bounds were provided by the
     * {@link #fling(int, int, int, int, int, int, int, int, int, int)} method.
     *
     * One should check this value before calling
     * {@link #startScroll(int, int, int, int)} as the interpolation currently in progress
     * to restore a valid position will then be stopped. The caller has to take into account
     * the fact that the started scroll will start from an overscrolled position.
     *
     * @return true when the current position is overscrolled and in the process of
     *         interpolating back to a valid value.
     */
    CARAPI IsOverScrolled(
        /* [in] */ Boolean* isOverScrolled);

    /**
     * Stops the animation. Contrary to {@link #forceFinished(boolean)},
     * aborting the animating causes the scroller to move to the final x and y
     * positions.
     *
     * @see #forceFinished(boolean)
     */
    CARAPI AbortAnimation();

    /**
     * Returns the time elapsed since the beginning of the scrolling.
     *
     * @return The elapsed time in milliseconds.
     *
     * @hide
     */
    CARAPI TimePassed(
        /* [out] */ Int32* timePassed);

    /**
     * @hide
     */
    CARAPI IsScrollingInDirection(
        /* [in] */ Float xvel,
        /* [in] */ Float yvel,
        /* [out] */ Boolean* isScrollingInDirection);

private:
    static const Int32 DEFAULT_DURATION = 250;
    static const Int32 SCROLL_MODE = 0;
    static const Int32 FLING_MODE = 1;

private:
    Int32 mMode;

    AutoPtr<SplineOverScroller> mScrollerX;
    AutoPtr<SplineOverScroller> mScrollerY;

    AutoPtr<Elastos::Droid::View::Animation::IInterpolator> mInterpolator;

    Boolean mFlywheel;
};

//==============================================================================
//          SplineOverScroller
//===============================================================================

class SplineOverScroller
    : public Object
{
public:
    SplineOverScroller(
        /* [in] */ IContext* context);

    CARAPI_(void) SetFriction(
        /* [in] */ Float friction);

    CARAPI_(void) UpdateScroll(
        /* [in] */ Float q);

    CARAPI_(void) StartScroll(
            /* [in] */ Int32 start,
            /* [in] */ Int32 distance,
            /* [in] */ Int32 duration);

    CARAPI_(void) Finish();

    CARAPI_(void) SetFinalPosition(
        /* [in] */ Int32 position);

    CARAPI_(void) ExtendDuration(
        /* [in] */ Int32 extend);

    CARAPI_(Boolean) Springback(
        /* [in] */ Int32 start,
        /* [in] */ Int32 min,
        /* [in] */ Int32 max);

    CARAPI_(void) Fling(
        /* [in] */ Int32 start,
        /* [in] */ Int32 velocity,
        /* [in] */ Int32 min,
        /* [in] */ Int32 max,
        /* [in] */ Int32 over);

    CARAPI_(Double) GetSplineDeceleration(
        /* [in] */ Int32 velocity);

    /* Returns the duration, expressed in milliseconds */
    CARAPI_(Int32) GetSplineFlingDuration(
        /* [in] */ Int32 velocity);

    CARAPI_(void) StartBounceAfterEdge(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 velocity);

    CARAPI_(void) NotifyEdgeReached(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 over);

    CARAPI_(Boolean) ContinueWhenFinished();

    /*
     * Update the current position and velocity for current time. Returns
     * TRUE if Update has been done and FALSE if animation duration has been
     * reached.
     */
    CARAPI_(Boolean) Update();

private:
    /*
     * Get a signed deceleration that will reduce the velocity.
     */
    static CARAPI_(Float) GetDeceleration(
        /* [in] */ Int32 velocity);

    /*
     * Modifies mDuration to the duration it takes to get from start to newFinal using the
     * spline interpolation. The previous duration was needed to get to oldFinal.
     */
    CARAPI_(void) AdjustDuration(
        /* [in] */ Int32 start,
        /* [in] */ Int32 oldFinal,
        /* [in] */ Int32 newFinal);

    CARAPI_(void) StartSpringback(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 velocity);

    CARAPI_(Double) GetSplineFlingDistance(
        /* [in] */ Int32 velocity);

    CARAPI_(void) FitOnBounceCurve(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 velocity);

    CARAPI_(void) StartAfterEdge(
        /* [in] */ Int32 start,
        /* [in] */ Int32 min,
        /* [in] */ Int32 max,
        /* [in] */ Int32 velocity);

    CARAPI_(void) OnEdgeReached();

public:
    // Initial position
    Int32 mStart;

    // Current position
    Int32 mCurrentPosition;

    // Final position
    Int32 mFinal;

    // Initial velocity
    Int32 mVelocity;

    // Current velocity
    Float mCurrVelocity;

    // Constant current deceleration
    Float mDeceleration;

    // Animation starting time, in system milliseconds
    Int64 mStartTime;

    // Animation duration, in milliseconds
    Int32 mDuration;

    // Duration to complete spline component of animation
    Int32 mSplineDuration;

    // Distance to travel along spline animation
    Int32 mSplineDistance;

    // Whether the animation is currently in progress
    Boolean mFinished;

    // The allowed overshot distance before boundary is reached.
    Int32 mOver;

    // Fling friction
    Float mFlingFriction;

    // Current state of the animation.
    Int32 mState;

    // Constant gravity value, used in the deceleration phase.
    static constexpr Float GRAVITY = 2000.0f;

    // A context-specific coefficient adjusted to physical values.
    Float mPhysicalCoeff;

    AutoPtr<IPowerManager> mPm;

    static Float DECELERATION_RATE;
    static constexpr Float INFLEXION = 0.35f; // Tension lines cross at (INFLEXION, 1)
    static constexpr Float START_TENSION = 0.5f;
    static constexpr Float END_TENSION = 1.0f;
    static const Float P1;
    static const Float P2;

    static const Int32 NB_SAMPLES = 100;
    static AutoPtr<ArrayOf<Float> > SPLINE_POSITION;
    static AutoPtr<ArrayOf<Float> > SPLINE_TIME;

    static const Int32 SPLINE = 0;
    static const Int32 CUBIC = 1;
    static const Int32 BALLISTIC = 2;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_OVERSCROLLER_H__
