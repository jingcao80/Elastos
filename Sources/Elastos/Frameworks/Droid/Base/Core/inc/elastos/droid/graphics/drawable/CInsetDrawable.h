
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_CINSETDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_CINSETDRAWABLE_H__

#include "_Elastos_Droid_Graphics_Drawable_CInsetDrawable.h"
#include "elastos/droid/graphics/drawable/InsetDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CarClass(CInsetDrawable), public InsetDrawable
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IDrawable* drawable,
        /* [in] */ Int32 inset);

    CARAPI constructor(
        /* [in] */ IDrawable* drawable,
        /* [in] */ Int32 insetLeft,
        /* [in] */ Int32 insetTop,
        /* [in] */ Int32 insetRight,
        /* [in] */ Int32 insetBottom);

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res);
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_CINSETDRAWABLE_H__
