
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_CCLIPDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_CCLIPDRAWABLE_H__

#include "_Elastos_Droid_Graphics_Drawable_CClipDrawable.h"
#include "elastos/droid/graphics/drawable/ClipDrawable.h"


namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CarClass(CClipDrawable), public ClipDrawable
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor();

    /**
     * @param orientation Bitwise-or of {@link #HORIZONTAL} and/or {@link #VERTICAL}
     */
    CARAPI constructor(
        /* [in] */ IDrawable* drawable,
        /* [in] */ Int32 gravity,
        /* [in] */ Int32 orientation);

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res);
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_CCLIPDRAWABLE_H__
