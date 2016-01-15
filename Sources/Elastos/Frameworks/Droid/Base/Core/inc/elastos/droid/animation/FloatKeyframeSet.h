
#ifndef  __ELASTOS_DROID_ANIMATION_FLOATKEYFRAMESET_H__
#define  __ELASTOS_DROID_ANIMATION_FLOATKEYFRAMESET_H__

#include "elastos/droid/animation/KeyframeSet.h"

namespace Elastos {
namespace Droid {
namespace Animation {
/**
 * This class holds a collection of FloatKeyframe objects and is called by ValueAnimator to calculate
 * values between those keyframes for a given animation. The class internal to the animation
 * package because it is an implementation detail of how Keyframes are stored and used.
 *
 * <p>This type-specific subclass of KeyframeSet, along with the other type-specific subclass for
 * Int32, exists to speed up the getValue() method when there is no custom
 * TypeEvaluator set for the animation, so that values can be calculated without autoboxing to the
 * Object equivalents of these primitive types.</p>
 */
class FloatKeyframeSet
    : public KeyframeSet
    , public IFloatKeyframeSet
    , public IFloatKeyframes
{
public:
    CAR_INTERFACE_DECL();

    FloatKeyframeSet(
        /* [in] */ ArrayOf<IFloatKeyframe*>* keyframes);

    /**
     * Gets the animated value, given the elapsed fraction of the animation (interpolated by the
     * animation's interpolator) and the evaluator used to calculate in-between values. This
     * function maps the input fraction to the appropriate keyframe interval and a fraction
     * between them and returns the interpolated value. Note that the input fraction may fall
     * outside the [0-1] bounds, if the animation's interpolator made that happen (e.g., a
     * spring interpolation that might send the fraction past 1.0). We handle this situation by
     * just using the two keyframes at the appropriate end when the value is outside those bounds.
     *
     * @param fraction The elapsed fraction of the animation
     * @return The animated value.
     */
    CARAPI GetValue(
        /* [in] */ Float fraction,
        /* [out] */ IInterface** value);

    CARAPI Clone(
        /* [out] */ IInterface** obj);

    virtual CARAPI InvalidateCache();

    CARAPI GetFloatValue(
        /* [in] */ Float frc,
        /* [out] */ Float* value);

    CARAPI GetKeyframes(
        /* [out, callee] */ ArrayOf<IKeyframe*>** frames);

    // @Override
    virtual CARAPI GetType(
        /* [out] */ InterfaceID* type);

private:
    Float mFirstValue;
    Float mLastValue;
    Float mDeltaValue;
    Boolean mFirstTime;
};

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos

#endif // __ELASTOS_DROID_ANIMATION_FLOATKEYFRAMESET_H__
