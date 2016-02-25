
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CFLINGANIMATIONUTILS_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CFLINGANIMATIONUTILS_H__

#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

/**
 * Utility class to calculate general fling animation when the finger is released.
 */
class FlingAnimationUtils
    : public Object
{
private:
    /**
     * An interpolator which interpolates two interpolators with an interpolator.
     */
    class InterpolatorInterpolator
        : public Object
        , public IInterpolator
        , public ITimeInterpolator
    {
    public:
        CAR_INTERFACE_DECL();

        InterpolatorInterpolator(
            /* [in] */ IInterpolator* interpolator1,
            /* [in] */ IInterpolator* interpolator2,
            /* [in] */ IInterpolator* crossfader);

        // @Override
        CARAPI GetInterpolation(
            /* [in] */ Float input,
            /* [out] */ Float* result);

        CARAPI HasNativeInterpolator(
            /* [out] */ Boolean* res);

    private:
        AutoPtr<IInterpolator> mInterpolator1;
        AutoPtr<IInterpolator> mInterpolator2;
        AutoPtr<IInterpolator> mCrossfader;
    };

    /**
     * An interpolator which interpolates with a fixed velocity.
     */
    class VelocityInterpolator
        : public Object
        , public IInterpolator
        , public ITimeInterpolator
    {
    public:
        CAR_INTERFACE_DECL();

        VelocityInterpolator(
            /* [in] */ Float durationSeconds,
            /* [in] */ Float velocity,
            /* [in] */ Float diff);

        // @Override
        CARAPI GetInterpolation(
            /* [in] */ Float input,
            /* [out] */ Float* result);

        CARAPI HasNativeInterpolator(
            /* [out] */ Boolean* res);

    private:
        Float mDurationSeconds;
        Float mVelocity;
        Float mDiff;
    };

    class AnimatorProperties: public Object
    {
    public:
        AnimatorProperties();

    public:
        AutoPtr<IInterpolator> mInterpolator;
        Int64 mDuration;
    };

public:
    FlingAnimationUtils(
        /* [in] */ IContext* ctx,
        /* [in] */ Float maxLengthSeconds);

    /**
     * Applies the interpolator and length to the animator, such that the fling animation is
     * consistent with the finger motion.
     *
     * @param animator the animator to apply
     * @param currValue the current value
     * @param endValue the end value of the animator
     * @param velocity the current velocity of the motion
     */
    CARAPI_(void) Apply(
        /* [in] */ IAnimator* animator,
        /* [in] */ Float currValue,
        /* [in] */ Float endValue,
        /* [in] */ Float velocity);

    /**
     * Applies the interpolator and length to the animator, such that the fling animation is
     * consistent with the finger motion.
     *
     * @param animator the animator to apply
     * @param currValue the current value
     * @param endValue the end value of the animator
     * @param velocity the current velocity of the motion
     */
    CARAPI_(void) Apply(
        /* [in] */ IViewPropertyAnimator* animator,
        /* [in] */ Float currValue,
        /* [in] */ Float endValue,
        /* [in] */ Float velocity);

    /**
     * Applies the interpolator and length to the animator, such that the fling animation is
     * consistent with the finger motion.
     *
     * @param animator the animator to apply
     * @param currValue the current value
     * @param endValue the end value of the animator
     * @param velocity the current velocity of the motion
     * @param maxDistance the maximum distance for this interaction; the maximum animation length
     *                    gets multiplied by the ratio between the actual distance and this value
     */
    CARAPI_(void) Apply(
        /* [in] */ IAnimator* animator,
        /* [in] */ Float currValue,
        /* [in] */ Float endValue,
        /* [in] */ Float velocity,
        /* [in] */ Float maxDistance);

    /**
     * Applies the interpolator and length to the animator, such that the fling animation is
     * consistent with the finger motion.
     *
     * @param animator the animator to apply
     * @param currValue the current value
     * @param endValue the end value of the animator
     * @param velocity the current velocity of the motion
     * @param maxDistance the maximum distance for this interaction; the maximum animation length
     *                    gets multiplied by the ratio between the actual distance and this value
     */
    CARAPI_(void) Apply(
        /* [in] */ IViewPropertyAnimator* animator,
        /* [in] */ Float currValue,
        /* [in] */ Float endValue,
        /* [in] */ Float velocity,
        /* [in] */ Float maxDistance);

    /**
     * Applies the interpolator and length to the animator, such that the fling animation is
     * consistent with the finger motion for the case when the animation is making something
     * disappear.
     *
     * @param animator the animator to apply
     * @param currValue the current value
     * @param endValue the end value of the animator
     * @param velocity the current velocity of the motion
     * @param maxDistance the maximum distance for this interaction; the maximum animation length
     *                    gets multiplied by the ratio between the actual distance and this value
     */
    CARAPI_(void) ApplyDismissing(
        /* [in] */ IAnimator* animator,
        /* [in] */ Float currValue,
        /* [in] */ Float endValue,
        /* [in] */ Float velocity,
        /* [in] */ Float maxDistance);

    /**
     * Applies the interpolator and length to the animator, such that the fling animation is
     * consistent with the finger motion for the case when the animation is making something
     * disappear.
     *
     * @param animator the animator to apply
     * @param currValue the current value
     * @param endValue the end value of the animator
     * @param velocity the current velocity of the motion
     * @param maxDistance the maximum distance for this interaction; the maximum animation length
     *                    gets multiplied by the ratio between the actual distance and this value
     */
    CARAPI_(void) ApplyDismissing(
        /* [in] */ IViewPropertyAnimator* animator,
        /* [in] */ Float currValue,
        /* [in] */ Float endValue,
        /* [in] */ Float velocity,
        /* [in] */ Float maxDistance);

    /**
     * @return the minimum velocity a gesture needs to have to be considered a fling
     */
    CARAPI_(Float) GetMinVelocityPxPerSecond();

private:
    CARAPI_(AutoPtr<AnimatorProperties>) GetProperties(
        /* [in] */ Float currValue,
        /* [in] */ Float endValue,
        /* [in] */ Float velocity,
        /* [in] */ Float maxDistance);

    CARAPI_(AutoPtr<AnimatorProperties>) GetDismissingProperties(
        /* [in] */ Float currValue,
        /* [in] */ Float endValue,
        /* [in] */ Float velocity,
        /* [in] */ Float maxDistance);

    /**
     * Calculates the y2 control point for a linear-out-faster-in path interpolator depending on the
     * velocity. The faster the velocity, the more "linear" the interpolator gets.
     *
     * @param velocity the velocity of the gesture.
     * @return the y2 control point for a cubic bezier path interpolator
     */
    CARAPI_(Float) CalculateLinearOutFasterInY2(
        /* [in] */ Float velocity);

private:
    static const Float LINEAR_OUT_SLOW_IN_X2;
    static const Float LINEAR_OUT_FASTER_IN_X2;
    static const Float LINEAR_OUT_FASTER_IN_Y2_MIN;
    static const Float LINEAR_OUT_FASTER_IN_Y2_MAX;
    static const Float MIN_VELOCITY_DP_PER_SECOND;
    static const Float HIGH_VELOCITY_DP_PER_SECOND;

    /**
     * Crazy math. http://en.wikipedia.org/wiki/B%C3%A9zier_curve
     */
    static const Float LINEAR_OUT_SLOW_IN_START_GRADIENT;

    AutoPtr<IInterpolator> mLinearOutSlowIn;
    AutoPtr<IInterpolator> mFastOutSlowIn;
    AutoPtr<IInterpolator> mFastOutLinearIn;

    Float mMinVelocityPxPerSecond;
    Float mMaxLengthSeconds;
    Float mHighVelocityPxPerSecond;

    AutoPtr<AnimatorProperties> mAnimatorProperties;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CFLINGANIMATIONUTILS_H__
