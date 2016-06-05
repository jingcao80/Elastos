
#ifndef __ELASTOS_DROID_ANIMATION_CFLOATEVALUATOR_H__
#define __ELASTOS_DROID_ANIMATION_CFLOATEVALUATOR_H__

#include "_Elastos_Droid_Animation_CFloatEvaluator.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Animation {

/**
 * This evaluator can be used to perform type interpolation between <code>float</code> values.
 */
CarClass(CFloatEvaluator)
    , public Object
    , public IFloatEvaluator
    , public ITypeEvaluator
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CFloatEvaluator();

    ~CFloatEvaluator();

    CARAPI constructor();

    /**
     * This function returns the result of linearly interpolating the start and end values, with
     * <code>fraction</code> representing the proportion between the start and end values. The
     * calculation is a simple parametric calculation: <code>result = x0 + t * (v1 - v0)</code>,
     * where <code>x0</code> is <code>startValue</code>, <code>x1</code> is <code>endValue</code>,
     * and <code>t</code> is <code>fraction</code>.
     *
     * @param fraction   The fraction from the starting to the ending values
     * @param startValue The start value; should be of type <code>float</code> or
     *                   <code>Float</code>
     * @param endValue   The end value; should be of type <code>float</code> or <code>Float</code>
     * @return A linear interpolation between the start and end values, given the
     *         <code>fraction</code> parameter.
     */
    CARAPI Evaluate(
        /* [in] */ Float fraction,
        /* [in] */ IInterface* startValue,
        /* [in] */ IInterface* endValue,
        /* [out] */ IInterface** result);
};


}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos

#endif  // __ELASTOS_DROID_ANIMATION_CFLOATEVALUATOR_H__
