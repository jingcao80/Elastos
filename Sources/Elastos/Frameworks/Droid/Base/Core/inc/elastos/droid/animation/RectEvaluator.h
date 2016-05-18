
#ifndef __ELASTOS_DROID_ANIMATION_RECTEVALUATOR_H__
#define __ELASTOS_DROID_ANIMATION_RECTEVALUATOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Animation.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IRect;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Animation {

/**
 * This evaluator can be used to perform type interpolation between <code>Rect</code> values.
 */
class RectEvaluator
    : public Object
    , public ITypeEvaluator
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Construct a RectEvaluator that returns a new Rect on every evaluate call.
     * To avoid creating an object for each evaluate call,
     * {@link RectEvaluator#RectEvaluator(android.graphics.Rect)} should be used
     * whenever possible.
     */
    RectEvaluator();

    /**
     * Constructs a RectEvaluator that modifies and returns <code>reuseRect</code>
     * in {@link #evaluate(float, android.graphics.Rect, android.graphics.Rect)} calls.
     * The value returned from
     * {@link #evaluate(float, android.graphics.Rect, android.graphics.Rect)} should
     * not be cached because it will change over time as the object is reused on each
     * call.
     *
     * @param reuseRect A Rect to be modified and returned by evaluate.
     */
    RectEvaluator(
        /* [in] */ IRect* reuseRect);

    /**
     * This function returns the result of linearly interpolating the start and
     * end Rect values, with <code>fraction</code> representing the proportion
     * between the start and end values. The calculation is a simple parametric
     * calculation on each of the separate components in the Rect objects
     * (left, top, right, and bottom).
     *
     * <p>If {@link #RectEvaluator(android.graphics.Rect)} was used to construct
     * this RectEvaluator, the object returned will be the <code>reuseRect</code>
     * passed into the constructor.</p>
     *
     * @param fraction   The fraction from the starting to the ending values
     * @param startValue The start Rect
     * @param endValue   The end Rect
     * @return A linear interpolation between the start and end values, given the
     *         <code>fraction</code> parameter.
     */
    // @Override
    virtual CARAPI Evaluate(
        /* [in] */ Float fraction,
        /* [in] */ IInterface* startValue,
        /* [in] */ IInterface* endValue,
        /* [out] */ IInterface** rect);

private:
    /**
     * When null, a new Rect is returned on every evaluate call. When non-null,
     * mRect will be modified and returned on every evaluate.
     */
    AutoPtr<IRect> mRect;
};

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos

#endif  // __ELASTOS_DROID_ANIMATION_RECTEVALUATOR_H__
