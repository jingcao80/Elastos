
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_CROUNDRECTSHAPE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_CROUNDRECTSHAPE_H__

#include "elastos/droid/graphics/drawable/shapes/RoundRectShape.h"
#include "_Elastos_Droid_Graphics_Drawable_Shapes_CRoundRectShape.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {
namespace Shapes {

CarClass(CRoundRectShape), public RoundRectShape
{
public:
    CAR_OBJECT_DECL()

    /**
     * RoundRectShape constructor.
     * Specifies an outer (round)rect and an optional inner (round)rect.
     *
     * @param outerRadii An array of 8 radius values, for the outer roundrect.
     *                   The first two floats are for the
     *                   top-left corner (remaining pairs correspond clockwise).
     *                   For no rounded corners on the outer rectangle,
     *                   pass null.
     * @param inset      A RectF that specifies the distance from the inner
     *                   rect to each side of the outer rect.
     *                   For no inner, pass null.
     * @param innerRadii An array of 8 radius values, for the inner roundrect.
     *                   The first two floats are for the
     *                   top-left corner (remaining pairs correspond clockwise).
     *                   For no rounded corners on the inner rectangle,
     *                   pass null.
     *                   If inset parameter is null, this parameter is ignored.
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Float>* outerRadii,
        /* [in] */ IRectF* inset,
        /* [in] */ ArrayOf<Float>* innerRadii);

    CARAPI Clone(
        /* [out] */ IInterface** shape);
};

} // namespace Shapes
} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_CROUNDRECTSHAPE_H__
