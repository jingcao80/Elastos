
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_ARCSHAPE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_ARCSHAPE_H__

#include "elastos/droid/graphics/drawable/shapes/RectShape.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {
namespace Shapes {

/**
 * Creates an arc shape. The arc shape starts at a specified
 * angle and sweeps clockwise, drawing slices of pie.
 * The arc can be drawn to a Canvas with its own draw() method,
 * but more graphical control is available if you instead pass
 * the ArcShape to a {@link android.graphics.drawable.ShapeDrawable}.
 */
class ArcShape
    : public RectShape
    , public IArcShape
{
public:
    CAR_INTERFACE_DECL();

    /**
     * ArcShape constructor.
     *
     * @param startAngle the angle (in degrees) where the arc begins
     * @param sweepAngle the sweep angle (in degrees). Anything equal to or
     *                   greater than 360 results in a complete circle/oval.
     */
    ArcShape(
        /* [in] */ Float startAngle,
        /* [in] */ Float sweepAngle);

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IPaint* paint);

protected:
    ArcShape();

    CARAPI constructor(
        /* [in] */ Float startAngle,
        /* [in] */ Float sweepAngle);

    CARAPI CloneImpl(
        /* [in] */ IArcShape* other);

private:
    Float mStart;
    Float mSweep;
};

} // namespace Shapes
} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_ARCSHAPE_H__
