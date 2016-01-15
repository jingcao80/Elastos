
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_CBITMAPDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_CBITMAPDRAWABLE_H__

#include "_Elastos_Droid_Graphics_Drawable_CBitmapDrawable.h"
#include "elastos/droid/graphics/drawable/BitmapDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CarClass(CBitmapDrawable), public BitmapDrawable
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IResources* res);

    CARAPI constructor(
        /* [in] */ IBitmap* bitmap);

    CARAPI constructor(
        /* [in] */ IResources* res,
        /* [in] */ IBitmap* bitmap);

    CARAPI constructor(
        /* [in] */ const String& filepath);

    CARAPI constructor(
        /* [in] */ IResources* res,
        /* [in] */ const String& filepath);

    CARAPI constructor(
        /* [in] */ IInputStream* is);

    CARAPI constructor(
        /* [in] */ IResources* res,
        /* [in] */ IInputStream* is);

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme);
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_CBITMAPDRAWABLE_H__
