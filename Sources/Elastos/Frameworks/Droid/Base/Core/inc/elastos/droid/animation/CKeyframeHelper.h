
#ifndef __ELASTOS_DROID_ANIMATION_CKEYFRAMEHELPER_H__
#define __ELASTOS_DROID_ANIMATION_CKEYFRAMEHELPER_H__

#include "_Elastos_Droid_Animation_CKeyframeHelper.h"
#include "elastos/droid/animation/Keyframe.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Animation {

CarClass(CKeyframeHelper)
    , public Singleton
    , public IKeyframeHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Constructs a Keyframe object with the given time and value. The time defines the
     * time, as a proportion of an overall animation's duration, at which the value will hold true
     * for the animation. The value for the animation between keyframes will be calculated as
     * an interpolation between the values at those keyframes.
     *
     * @param fraction The time, expressed as a value between 0 and 1, representing the fraction
     * of time elapsed of the overall animation duration.
     * @param value The value that the object will animate to as the animation time approaches
     * the time in this keyframe, and the the value animated from as the time passes the time in
     * this keyframe.
     */
    CARAPI OfInt32(
        /* [in] */ Float fraction,
        /* [in] */ Int32 value,
        /* [out] */ IKeyframe** obj);

    /**
     * Constructs a Keyframe object with the given time. The value at this time will be derived
     * from the target object when the animation first starts (note that this implies that keyframes
     * with no initial value must be used as part of an ObjectAnimator).
     * The time defines the
     * time, as a proportion of an overall animation's duration, at which the value will hold true
     * for the animation. The value for the animation between keyframes will be calculated as
     * an interpolation between the values at those keyframes.
     *
     * @sa ObjectAnimator
     *
     * @param fraction The time, expressed as a value between 0 and 1, representing the fraction
     * of time elapsed of the overall animation duration.
     */
    CARAPI OfInt32(
        /* [in] */ Float fraction,
        /* [out] */ IKeyframe** obj);

    /**
     * Constructs a Keyframe object with the given time and value. The time defines the
     * time, as a proportion of an overall animation's duration, at which the value will hold true
     * for the animation. The value for the animation between keyframes will be calculated as
     * an interpolation between the values at those keyframes.
     *
     * @param fraction The time, expressed as a value between 0 and 1, representing the fraction
     * of time elapsed of the overall animation duration.
     * @param value The value that the object will animate to as the animation time approaches
     * the time in this keyframe, and the the value animated from as the time passes the time in
     * this keyframe.
     */
    CARAPI OfFloat(
        /* [in] */ Float fraction,
        /* [in] */ Float value,
        /* [out] */ IKeyframe** obj);

    /**
     * Constructs a Keyframe object with the given time. The value at this time will be derived
     * from the target object when the animation first starts (note that this implies that keyframes
     * with no initial value must be used as part of an ObjectAnimator).
     * The time defines the
     * time, as a proportion of an overall animation's duration, at which the value will hold true
     * for the animation. The value for the animation between keyframes will be calculated as
     * an interpolation between the values at those keyframes.
     *
     * @sa ObjectAnimator
     *
     * @param fraction The time, expressed as a value between 0 and 1, representing the fraction
     * of time elapsed of the overall animation duration.
     */
    CARAPI OfFloat(
        /* [in] */ Float fraction,
        /* [out] */ IKeyframe** obj);

    /**
     * Constructs a Keyframe object with the given time and value. The time defines the
     * time, as a proportion of an overall animation's duration, at which the value will hold true
     * for the animation. The value for the animation between keyframes will be calculated as
     * an interpolation between the values at those keyframes.
     *
     * @param fraction The time, expressed as a value between 0 and 1, representing the fraction
     * of time elapsed of the overall animation duration.
     * @param value The value that the object will animate to as the animation time approaches
     * the time in this keyframe, and the the value animated from as the time passes the time in
     * this keyframe.
     */
    CARAPI OfObject(
        /* [in] */ Float fraction,
        /* [in] */ IInterface* value,
        /* [out] */ IKeyframe** obj);

    /**
     * Constructs a Keyframe object with the given time. The value at this time will be derived
     * from the target object when the animation first starts (note that this implies that keyframes
     * with no initial value must be used as part of an ObjectAnimator).
     * The time defines the
     * time, as a proportion of an overall animation's duration, at which the value will hold true
     * for the animation. The value for the animation between keyframes will be calculated as
     * an interpolation between the values at those keyframes.
     *
     * @sa ObjectAnimator
     *
     * @param fraction The time, expressed as a value between 0 and 1, representing the fraction
     * of time elapsed of the overall animation duration.
     */
    CARAPI OfObject(
        /* [in] */ Float fraction,
        /* [out] */ IKeyframe** obj);

};


}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos

#endif  // __ELASTOS_DROID_ANIMATION_CKEYFRAMEHELPER_H__
