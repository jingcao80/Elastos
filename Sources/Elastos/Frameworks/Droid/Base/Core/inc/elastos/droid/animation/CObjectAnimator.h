
#ifndef __ELASTOS_DROID_ANIMATION_COBJECTANIMATOR_H__
#define __ELASTOS_DROID_ANIMATION_COBJECTANIMATOR_H__

#include "_Elastos_Droid_Animation_CObjectAnimator.h"
#include "elastos/droid/animation/ObjectAnimator.h"

using Elastos::Droid::Utility::IProperty;

namespace Elastos {
namespace Droid {
namespace Animation {

/**
 * This subclass of {@link ValueAnimator} provides support for animating properties on target objects.
 * The constructors of this class take parameters to define the target object that will be animated
 * as well as the name of the property that will be animated. Appropriate set/get functions
 * are then determined internally and the animation will call these functions as necessary to
 * animate the property.
 *
 * <div class="special reference">
 * <h3>Developer Guides</h3>
 * <p>For more information about animating with {@code ObjectAnimator}, read the
 * <a href="{@docRoot}guide/topics/graphics/prop-animation.html#object-animator">Property
 * Animation</a> developer guide.</p>
 * </div>
 *
 * @see #setPropertyName(String)
 *
 */
CarClass(CObjectAnimator) , public ObjectAnimator
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Animation
} // namepsace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_ANIMATION_COBJECTANIMATOR_H__
