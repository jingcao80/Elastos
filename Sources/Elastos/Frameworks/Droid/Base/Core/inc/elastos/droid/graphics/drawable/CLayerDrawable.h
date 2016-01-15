
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_CLAYERDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_CLAYERDRAWABLE_H__

#include "_Elastos_Droid_Graphics_Drawable_CLayerDrawable.h"
#include "elastos/droid/graphics/drawable/LayerDrawable.h"


namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CarClass(CLayerDrawable), public LayerDrawable
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ArrayOf<IDrawable*>* layers);

    CARAPI constructor(
        /* [in] */ ArrayOf<IDrawable*>* layers,
        /* [in] */ IDrawableConstantState* state);

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme);
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_DRAWABLE_CLAYERDRAWABLE_H__
