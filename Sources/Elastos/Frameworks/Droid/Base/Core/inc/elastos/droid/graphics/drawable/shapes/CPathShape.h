
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_CPATHSHAPE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_CPATHSHAPE_H__

#include "elastos/droid/graphics/drawable/shapes/PathShape.h"
#include "_Elastos_Droid_Graphics_Drawable_Shapes_CPathShape.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {
namespace Shapes {

CarClass(CPathShape), public PathShape
{
public:
    CAR_OBJECT_DECL()

    /**
     * PathShape constructor.
     *
     * @param path       a Path that defines the geometric paths for this shape
     * @param stdWidth   the standard width for the shape. Any changes to the
     *                   width with resize() will result in a width scaled based
     *                   on the new width divided by this width.
     * @param stdHeight  the standard height for the shape. Any changes to the
     *                   height with resize() will result in a height scaled based
     *                   on the new height divided by this height.
     */
    CARAPI constructor(
        /* [in] */ IPath* path,
        /* [in] */ Float stdWidth,
        /* [in] */ Float stdHeight);

    CARAPI Clone(
        /* [out] */ IInterface** shape);
};

} // namespace Shapes
} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_CPATHSHAPE_H__
