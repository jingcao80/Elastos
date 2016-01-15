
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_CANIMATEDROTATEDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_CANIMATEDROTATEDRAWABLE_H__

#include "_Elastos_Droid_Graphics_Drawable_CAnimatedRotateDrawable.h"
#include "elastos/droid/graphics/drawable/AnimatedRotateDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CarClass(CAnimatedRotateDrawable), public AnimatedRotateDrawable
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res);
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_DRAWABLE_CANIMATEDROTATEDRAWABLE_H__

