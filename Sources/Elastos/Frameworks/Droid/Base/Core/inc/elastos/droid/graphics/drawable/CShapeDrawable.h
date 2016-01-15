
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_CSHAPEDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_CSHAPEDRAWABLE_H__

#include "_Elastos_Droid_Graphics_Drawable_CShapeDrawable.h"
#include "elastos/droid/graphics/drawable/CShapeDrawable.h"
#include "elastos/droid/graphics/drawable/ShapeDrawable.h"


namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CarClass(CShapeDrawable), public ShapeDrawable
{
public:
    CAR_OBJECT_DECL();

    /**
     * ShapeDrawable constructor.
     */
    CARAPI constructor();

    /**
     * Creates a ShapeDrawable with a specified Shape.
     *
     * @param s the Shape that this ShapeDrawable should be
     */
    CARAPI constructor(
        /* [in] */ IShape* s);

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme);
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_CSHAPEDRAWABLE_H__
