#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_CRECTSHAPE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_CRECTSHAPE_H__

#include "elastos/droid/graphics/drawable/shapes/RectShape.h"
#include "_Elastos_Droid_Graphics_Drawable_Shapes_CRectShape.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {
namespace Shapes {

CarClass(CRectShape), public RectShape
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor();

    CARAPI Clone(
        /* [out] */ IInterface** shape);
};

} // namespace Shapes
} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_CRECTSHAPE_H__
