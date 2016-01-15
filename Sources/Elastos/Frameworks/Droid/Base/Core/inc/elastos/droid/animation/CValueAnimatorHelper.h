#ifndef  __ELASTOS_DROID_ANIMATION_CVALUEANIMATORHELPER_H__
#define  __ELASTOS_DROID_ANIMATION_CVALUEANIMATORHELPER_H__

#include "_Elastos_Droid_Animation_CValueAnimatorHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Animation {

CarClass(CValueAnimatorHelper)
    , public Singleton
    , public IValueAnimatorHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * @hide
     */
    CARAPI SetDurationScale(
        /* [in] */ Float durationScale);

    /**
     * @hide
     */
    CARAPI GetDurationScale(
        /* [out] */ Float* durationScale);

    /**
     * Constructs and returns a ValueAnimator that animates between int values. A single
     * value implies that that value is the one being animated to. However, this is not typically
     * useful in a ValueAnimator object because there is no way for the object to determine the
     * starting value for the animation (unlike ObjectAnimator, which can derive that value
     * from the target object and property being animated). Therefore, there should typically
     * be two or more values.
     *
     * @param values A set of values that the animation will animate between over time.
     * @return A ValueAnimator object that is set up to animate between the given values.
     */
    CARAPI OfInt32(
        /* [in] */ ArrayOf<Int32>* values,
        /* [out] */ IValueAnimator** obj);

    /**
     * Constructs and returns a ValueAnimator that animates between float values. A single
     * value implies that that value is the one being animated to. However, this is not typically
     * useful in a ValueAnimator object because there is no way for the object to determine the
     * starting value for the animation (unlike ObjectAnimator, which can derive that value
     * from the target object and property being animated). Therefore, there should typically
     * be two or more values.
     *
     * @param values A set of values that the animation will animate between over time.
     * @return A ValueAnimator object that is set up to animate between the given values.
     */
    CARAPI OfFloat(
        /* [in] */ ArrayOf<Float>* values,
        /* [out] */ IValueAnimator** obj);

    /**
     * Constructs and returns a ValueAnimator that animates between the values
     * specified in the PropertyValuesHolder objects.
     *
     * @param values A set of PropertyValuesHolder objects whose values will be animated
     * between over time.
     * @return A ValueAnimator object that is set up to animate between the given values.
     */
    CARAPI OfPropertyValuesHolder(
        /* [in] */ ArrayOf<IPropertyValuesHolder*>* values,
        /* [out] */ IValueAnimator** obj);

    /**
     * Constructs and returns a ValueAnimator that animates between Object values. A single
     * value implies that that value is the one being animated to. However, this is not typically
     * useful in a ValueAnimator object because there is no way for the object to determine the
     * starting value for the animation (unlike ObjectAnimator, which can derive that value
     * from the target object and property being animated). Therefore, there should typically
     * be two or more values.
     *
     * <p>Since ValueAnimator does not know how to animate between arbitrary Objects, this
     * factory method also takes a TypeEvaluator object that the ValueAnimator will use
     * to perform that interpolation.
     *
     * @param evaluator A TypeEvaluator that will be called on each animation frame to
     * provide the ncessry interpolation between the Object values to derive the animated
     * value.
     * @param values A set of values that the animation will animate between over time.
     * @return A ValueAnimator object that is set up to animate between the given values.
     */
    CARAPI OfObject(
        /* [in] */ ITypeEvaluator* evaluator,
        /* [in] */ ArrayOf<IInterface*>* values,
        /* [out] */ IValueAnimator** obj);

    /**
     * The amount of time, in milliseconds, between each frame of the animation. This is a
     * requested time that the animation will attempt to honor, but the actual delay between
     * frames may be different, depending on system load and capabilities. This is a static
     * function because the same delay will be applied to all animations, since they are all
     * run off of a single timing loop.
     *
     * The frame delay may be ignored when the animation system uses an external timing
     * source, such as the display refresh rate (vsync), to govern animations.
     *
     * @return the requested time between frames, in milliseconds
     */
    CARAPI GetFrameDelay(
        /* [out] */ Int64* frameDelay);

    /**
     * The amount of time, in milliseconds, between each frame of the animation. This is a
     * requested time that the animation will attempt to honor, but the actual delay between
     * frames may be different, depending on system load and capabilities. This is a static
     * function because the same delay will be applied to all animations, since they are all
     * run off of a single timing loop.
     *
     * The frame delay may be ignored when the animation system uses an external timing
     * source, such as the display refresh rate (vsync), to govern animations.
     *
     * @param frameDelay the requested time between frames, in milliseconds
     */
    CARAPI SetFrameDelay(
        /* [in] */ Int64 frameDelay);

    /**
     * Return the number of animations currently running.
     *
     * Used by StrictMode internally to annotate violations.
     * May be called on arbitrary threads!
     *
     * @hide
     */
    CARAPI GetCurrentAnimationsCount(
        /* [out] */ Int32* animationsCount);

    /**
     * Clear all animations on this thread, without canceling or ending them.
     * This should be used with caution.
     *
     * @hide
     */
    CARAPI ClearAllAnimations();
};

} // namespace Animation
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_ANIMATION_CVALUEANIMATORHELPER_H__
