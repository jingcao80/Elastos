
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_CTRANSITIONDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_CTRANSITIONDRAWABLE_H__

#include "_Elastos_Droid_Graphics_Drawable_CTransitionDrawable.h"
#include "elastos/droid/graphics/drawable/TransitionDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CarClass(CTransitionDrawable), public TransitionDrawable
{
public:
    CAR_OBJECT_DECL();

    /**
     * Create a new transition drawable with the specified list of layers. At least
     * 2 layers are required for this drawable to work properly.
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<IDrawable*>* layers);

    /**
     * Create a new transition drawable with no layer. To work correctly, at least 2
     * layers must be added to this drawable.
     *
     * @see #TransitionDrawable(Drawable[])
     */
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme);

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ ArrayOf<IDrawable*>* layers);
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_CTRANSITIONDRAWABLE_H__
