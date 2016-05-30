#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_CVFULLPATH_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_CVFULLPATH_H__

#include "elastos/droid/graphics/drawable/VectorDrawable.h"
#include "_Elastos_Droid_Graphics_Drawable_CVFullPath.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CarClass(CVFullPath)
    , public VectorDrawable::VFullPath
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetStrokeColor(
        /* [out] */ Int32* strokeColor);

    CARAPI GetStrokeWidth(
        /* [out] */ Float* strokeWidth);

    CARAPI GetStrokeAlpha(
         /* [out] */ Float* strokeAlpha);

    CARAPI GetFillColor(
        /* [out] */ Int32* fillColor);

    CARAPI GetFillAlpha(
        /* [out] */ Float* fillAlpha);

    CARAPI GetTrimPathStart(
        /* [out] */ Float* trimPathStart);

    CARAPI GetTrimPathEnd(
        /* [out] */ Float* trimPathEnd);

    CARAPI GetTrimPathOffset(
        /* [out] */ Float* trimPathOffset);
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_DRAWABLE_CVFULLPATH_H__
