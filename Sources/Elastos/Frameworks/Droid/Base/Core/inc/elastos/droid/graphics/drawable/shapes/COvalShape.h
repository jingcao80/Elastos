
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_COVALSHAPE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_COVALSHAPE_H__

#include "elastos/droid/graphics/drawable/shapes/OvalShape.h"
#include "_Elastos_Droid_Graphics_Drawable_Shapes_COvalShape.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {
namespace Shapes {

CarClass(COvalShape), public OvalShape
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

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_COVALSHAPE_H__
