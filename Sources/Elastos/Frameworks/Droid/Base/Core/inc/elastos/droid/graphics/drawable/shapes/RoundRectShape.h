#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_ROUNDRECTSHAPE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_ROUNDRECTSHAPE_H__

#include "elastos/droid/graphics/drawable/shapes/RectShape.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {
namespace Shapes {

/**
 * Creates a rounded-corner rectangle. Optionally, an inset (rounded) rectangle
 * can be included (to make a sort of "O" shape).
 * The rounded rectangle can be drawn to a Canvas with its own draw() method,
 * but more graphical control is available if you instead pass
 * the RoundRectShape to a {@link android.graphics.drawable.ShapeDrawable}.
 */
class RoundRectShape
    : public RectShape
    , public IRoundRectShape
{
public:
    CAR_INTERFACE_DECL();

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
    RoundRectShape(
        /* [in] */ ArrayOf<Float>* outerRadii,
        /* [in] */ IRectF* inset,
        /* [in] */ ArrayOf<Float>* innerRadii);

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IPaint* paint);

    // @Override
    CARAPI GetOutline(
        /* [in] */ IOutline* outline);

protected:
    RoundRectShape();

    CARAPI constructor(
        /* [in] */ ArrayOf<Float>* outerRadii,
        /* [in] */ IRectF* inset,
        /* [in] */ ArrayOf<Float>* innerRadii);

    //@Override
    CARAPI_(void) OnResize(
        /* [in] */ Float width,
        /* [in] */ Float height);

    CARAPI CloneImpl(
        /* [in] */ IRoundRectShape* other);

private:
    AutoPtr< ArrayOf<Float> > mOuterRadii;
    AutoPtr<IRectF> mInset;
    AutoPtr< ArrayOf<Float> > mInnerRadii;

    AutoPtr<IRectF> mInnerRect;
    AutoPtr<IPath> mPath;    // this is what we actually draw
};

} // namespace Shapes
} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_ROUNDRECTSHAPE_H__
