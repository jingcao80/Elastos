
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_CSTATELISTDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_CSTATELISTDRAWABLE_H__

#include "_Elastos_Droid_Graphics_Drawable_CStateListDrawable.h"
#include "elastos/droid/graphics/drawable/StateListDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CarClass(CStateListDrawable), public StateListDrawable
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res);

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state);
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_DRAWABLE_CSTATELISTDRAWABLE_H__
