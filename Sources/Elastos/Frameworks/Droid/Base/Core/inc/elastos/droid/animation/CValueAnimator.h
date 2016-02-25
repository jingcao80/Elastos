
#ifndef __ELASTOS_DROID_ANIMATION_CVALUEANIMATOR_H__
#define __ELASTOS_DROID_ANIMATION_CVALUEANIMATOR_H__

#include "_Elastos_Droid_Animation_CValueAnimator.h"
#include "elastos/droid/animation/ValueAnimator.h"

namespace Elastos {
namespace Droid {
namespace Animation {

CarClass(CValueAnimator) , public ValueAnimator
{
public:
    CAR_OBJECT_DECL();

    /**
     * Constructs and returns a ValueAnimator that animates between Int32 values. A single
     * value implies that that value is the one being animated to. However, this is not typically
     * useful in a ValueAnimator object because there is no way for the object to determine the
     * starting value for the animation (unlike ObjectAnimator, which can derive that value
     * from the target object and property being animated). Therefore, there should typically
     * be two or more values.
     *
     * @param values A set of values that the animation will animate between over time.
     * @return A ValueAnimator object that is set up to animate between the given values.
     */
    static CARAPI_(AutoPtr<IValueAnimator>) OfInt32(
        /* [in] */ ArrayOf<Int32>* values);

    /**
     * Constructs and returns a ValueAnimator that animates between Float values. A single
     * value implies that that value is the one being animated to. However, this is not typically
     * useful in a ValueAnimator object because there is no way for the object to determine the
     * starting value for the animation (unlike ObjectAnimator, which can derive that value
     * from the target object and property being animated). Therefore, there should typically
     * be two or more values.
     *
     * @param values A set of values that the animation will animate between over time.
     * @return A ValueAnimator object that is set up to animate between the given values.
     */
    static CARAPI_(AutoPtr<IValueAnimator>) OfFloat(
        /* [in] */ ArrayOf<Float>* values);

    /**
     * Constructs and returns a ValueAnimator that animates between the values
     * specified in the PropertyValuesHolder objects.
     *
     * @param values A set of PropertyValuesHolder objects whose values will be animated
     * between over time.
     * @return A ValueAnimator object that is set up to animate between the given values.
     */
    static CARAPI_(AutoPtr<IValueAnimator>) OfPropertyValuesHolder(
        /* [in] */ ArrayOf<IPropertyValuesHolder*>* values);

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
    static CARAPI_(AutoPtr<IValueAnimator>) OfObject(
        /* [in] */ ITypeEvaluator* evaluator,
        /* [in] */ ArrayOf<IInterface*>* values);
};

} // namespace Animation
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_ANIMATION_CVALUEANIMATOR_H__
