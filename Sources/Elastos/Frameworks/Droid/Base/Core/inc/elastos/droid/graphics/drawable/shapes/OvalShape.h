
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_OVALSHAPE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_OVALSHAPE_H__

#include "elastos/droid/graphics/drawable/shapes/RectShape.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {
namespace Shapes {

/**
 * Defines an oval shape.
 * The oval can be drawn to a Canvas with its own draw() method,
 * but more graphical control is available if you instead pass
 * the OvalShape to a {@link android.graphics.drawable.ShapeDrawable}.
 */
class OvalShape
    : public RectShape
    , public IOvalShape
{
public:
    CAR_INTERFACE_DECL()

    /**
     * OvalShape constructor.
     */
    OvalShape();

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IPaint* paint);

    CARAPI GetOutline(
        /* [in] */ IOutline* outline);
};

} // namespace Shapes
} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_OVALSHAPE_H__
