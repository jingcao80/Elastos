
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_SHAPE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_SHAPE_H__

#include "Elastos.Droid.Graphics.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IOutline;
using Elastos::Core::ICloneable;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {
namespace Shapes {

/**
 * Defines a generic graphical "shape."
 * Any Shape can be drawn to a Canvas with its own draw() method,
 * but more graphical control is available if you instead pass
 * it to a {@link android.graphics.drawable.ShapeDrawable}.
 */
class Shape
    : public Object
    , public IShape
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL();

    Shape();

    /**
     * Returns the width of the Shape.
     */
    virtual CARAPI GetWidth(
        /* [out] */ Float* width);

    /**
     * Returns the height of the Shape.
     */
    virtual CARAPI GetHeight(
        /* [out] */ Float* height);

    /**
     * Draw this shape into the provided Canvas, with the provided Paint.
     * Before calling this, you must call {@link #resize(float,float)}.
     *
     * @param canvas the Canvas within which this shape should be drawn
     * @param paint  the Paint object that defines this shape's characteristics
     */
    virtual CARAPI Draw(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IPaint* paint) = 0;

    /**
     * Resizes the dimensions of this shape.
     * Must be called before {@link #draw(Canvas,Paint)}.
     *
     * @param width the width of the shape (in pixels)
     * @param height the height of the shape (in pixels)
     */
    CARAPI Resize(
        /* [in] */ Float width,
        /* [in] */ Float height);

    /**
     * Checks whether the Shape is opaque.
     * Default impl returns true. Override if your subclass can be opaque.
     *
     * @return true if any part of the drawable is <em>not</em> opaque.
     */
    virtual CARAPI HasAlpha(
        /* [out] */ Boolean* has);

    virtual CARAPI GetOutline(
        /* [in] */ IOutline* outline);

    // virtual CARAPI Clone(
    //     /* [out] */ IInterface** other);

protected:
    /**
     * Callback method called when {@link #resize(float,float)} is executed.
     *
     * @param width the new width of the Shape
     * @param height the new height of the Shape
     */
    virtual CARAPI_(void) OnResize(
        /* [in] */ Float width,
        /* [in] */ Float height);

    CARAPI CloneImpl(
        /* [in] */ IShape* other);

private:
    Float mWidth;
    Float mHeight;
};

} // namespace Shapes
} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_SHAPES_SHAPE_H__
