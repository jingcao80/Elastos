
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_CROTATEDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_CROTATEDRAWABLE_H__

#include "_Elastos_Droid_Graphics_Drawable_CRotateDrawable.h"
#include "elastos/droid/graphics/drawable/RotateDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CarClass(CRotateDrawable), public RotateDrawable
{
public:
    CAR_OBJECT_DECL();

    /**
     * <p>Create a new rotating drawable with an empty state.</p>
     */
    CARAPI constructor();

    /**
     * <p>Create a new rotating drawable with the specified state. A copy of
     * this state is used as the internal state for the newly created
     * drawable.</p>
     *
     * @param rotateState the state for this drawable
     */
    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res);
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_CROTATEDRAWABLE_H__
