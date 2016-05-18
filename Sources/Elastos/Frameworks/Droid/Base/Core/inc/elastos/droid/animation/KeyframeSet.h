
#ifndef __ELASTOS_DROID_ANIMATION_KEYFRAMESET_H__
#define __ELASTOS_DROID_ANIMATION_KEYFRAMESET_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Animation.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IPath;
using Elastos::Core::ICloneable;

namespace Elastos {
namespace Droid {
namespace Animation {

/**
 * This class holds a collection of Keyframe objects and is called by ValueAnimator to calculate
 * values between those keyframes for a given animation. The class internal to the animation
 * package because it is an implementation detail of how Keyframes are stored and used.
 */
class KeyframeSet
    : public Object
    , public IKeyframeSet
    , public IKeyframes
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    KeyframeSet();

    CARAPI constructor(
        /* [in] */ ArrayOf<IKeyframe*>* keyframes);

    virtual ~KeyframeSet();

    static CARAPI_(AutoPtr<IKeyframeSet>) OfInt32(
        /* [in] */ ArrayOf<Int32>* values);

    static CARAPI_(AutoPtr<IKeyframeSet>) OfFloat(
        /* [in] */ ArrayOf<Float>* values);

    static CARAPI_(AutoPtr<IKeyframeSet>) OfKeyframe(
        /* [in] */ ArrayOf<IKeyframe*>* keyframes);

    static CARAPI_(AutoPtr<IKeyframeSet>) OfObject(
        /* [in] */ ArrayOf<IInterface*>* values);

    static CARAPI_(AutoPtr<IPathKeyframes>) OfPath(
        /* [in] */ IPath* path);

    static CARAPI_(AutoPtr<IPathKeyframes>) OfPath(
        /* [in] */ IPath* path,
        /* [in] */ Float error);

    /**
     * Sets the TypeEvaluator to be used when calculating animated values. This object
     * is required only for KeyframeSets that are not either IntKeyframeSet or FloatKeyframeSet,
     * both of which assume their own evaluator to speed up calculations with those primitive
     * types.
     *
     * @param evaluator The TypeEvaluator to be used to calculate animated values.
     */
    virtual CARAPI SetEvaluator(
        /* [in] */ ITypeEvaluator* evaluator);

    virtual CARAPI GetType(
        /* [out] */ InterfaceID* type);

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
    virtual CARAPI GetValue(
        /* [in] */ Float fraction,
        /* [out] */ IInterface** value);

    virtual CARAPI Clone(
        /* [out] */ IInterface** obj);

    /**
     * If subclass has variables that it calculates based on the Keyframes, it should reset them
     * when this method is called because Keyframe contents might have changed.
     */
    // @Override
    virtual CARAPI InvalidateCache();

    virtual CARAPI GetKeyframes(
        /* [out, callee] */ ArrayOf<IKeyframe*>** frames);

    CARAPI ToString(
        /* [out] */ String* str);

protected:
    friend class CLayoutTransition;

    Int32 mNumKeyframes;

    AutoPtr<IKeyframe> mFirstKeyframe;
    AutoPtr<IKeyframe> mLastKeyframe;
    AutoPtr<ITimeInterpolator> mInterpolator; // only used in the 2-keyframe case
    AutoPtr<ArrayOf<IKeyframe*> > mKeyframes; // only used when there are not 2 keyframes
    AutoPtr<ITypeEvaluator> mEvaluator;
};

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos

#endif  // __ELASTOS_DROID_ANIMATION_KEYFRAMESET_H__
