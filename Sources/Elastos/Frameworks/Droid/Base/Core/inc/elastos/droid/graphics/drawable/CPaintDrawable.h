
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_CPAINTDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_CPAINTDRAWABLE_H__

#include "_Elastos_Droid_Graphics_Drawable_CPaintDrawable.h"
#include "elastos/droid/graphics/drawable/PaintDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CarClass(CPaintDrawable), public PaintDrawable
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 color);
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_CPAINTDRAWABLE_H__
