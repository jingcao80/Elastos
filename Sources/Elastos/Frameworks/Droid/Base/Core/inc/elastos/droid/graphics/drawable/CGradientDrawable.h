
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_CGRADIENTDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_CGRADIENTDRAWABLE_H__

#include "_Elastos_Droid_Graphics_Drawable_CGradientDrawable.h"
#include "elastos/droid/graphics/drawable/GradientDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CarClass(CGradientDrawable), public GradientDrawable
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor();

    /**
     * Create a new gradient drawable given an orientation and an array
     * of colors for the gradient.
     */
    CARAPI constructor(
        /* [in] */ GradientDrawableOrientation orientation,
        /* [in] */ ArrayOf<Int32>* colors);

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResourcesTheme* theme);
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_CGRADIENTDRAWABLE_H__
