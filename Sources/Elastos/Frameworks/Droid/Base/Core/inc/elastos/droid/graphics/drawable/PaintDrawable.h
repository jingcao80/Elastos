
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_PAINTDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_PAINTDRAWABLE_H__

#include "elastos/droid/graphics/drawable/ShapeDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

/**
 * Drawable that draws its bounds in the given paint, with optional
 * rounded corners.
*/
class PaintDrawable
    : public ShapeDrawable
    , public IPaintDrawable
{
public:
    CAR_INTERFACE_DECL()

    PaintDrawable();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 color);

    /**
     * Specify radius for the corners of the rectangle. If this is > 0, then the
     * drawable is drawn in a round-rectangle, rather than a rectangle.
     * @param radius the radius for the corners of the rectangle
     */
    virtual CARAPI SetCornerRadius(
        /* [in] */ Float radius);

    /**
     * Specify radii for each of the 4 corners. For each corner, the array
     * contains 2 values, [X_radius, Y_radius]. The corners are ordered
     * top-left, top-right, bottom-right, bottom-left
     * @param radii the x and y radii of the corners
     */
    virtual CARAPI SetCornerRadii(
        /* [in] */ ArrayOf<Float>* radii);

protected:
    //@Override
    CARAPI_(Boolean) InflateTag(
        /* [in] */ const String& name,
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs);
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_PAINTDRAWABLE_H__
