
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_CSCALEDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_CSCALEDRAWABLE_H__

#include "_Elastos_Droid_Graphics_Drawable_CScaleDrawable.h"
#include "elastos/droid/graphics/drawable/ScaleDrawable.h"


namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CarClass(CScaleDrawable), public ScaleDrawable
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IDrawable* drawable,
        /* [in] */ Int32 gravity,
        /* [in] */ Float scaleWidth,
        /* [in] */ Float scaleHeight);

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res);
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_CSCALEDRAWABLE_H__
