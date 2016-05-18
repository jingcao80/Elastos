#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_RECTSHAPE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_RECTSHAPE_H__

#include "elastos/droid/graphics/drawable/shapes/Shape.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {
namespace Shapes {

/**
 * Defines a rectangle shape.
 * The rectangle can be drawn to a Canvas with its own draw() method,
 * but more graphical control is available if you instead pass
 * the RectShape to a {@link android.graphics.drawable.ShapeDrawable}.
 */
class RectShape
    : public Shape
    , public IRectShape
{
public:
    CAR_INTERFACE_DECL()

    /**
     * RectShape constructor.
     */
    RectShape();

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IPaint* paint);

    CARAPI GetOutline(
        /* [in] */ IOutline* outline);

protected:
    //@Override
    CARAPI_(void) OnResize(
        /* [in] */ Float width,
        /* [in] */ Float height);

    /**
     * Returns the RectF that defines this rectangle's bounds.
     */
    CARAPI_(AutoPtr<IRectF>) Rect();

    CARAPI CloneImpl(
        /* [in] */ IRectShape* other);

private:
    AutoPtr<IRectF> mRect;
};

} // namespace Shapes
} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_RECTSHAPE_H__
