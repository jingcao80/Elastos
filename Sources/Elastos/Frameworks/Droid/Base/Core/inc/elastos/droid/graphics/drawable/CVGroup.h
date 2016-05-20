#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_CVGROUP_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_CVGROUP_H__

#include "elastos/droid/graphics/drawable/VectorDrawable.h"
#include "_Elastos_Droid_Graphics_Drawable_CVGroup.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CarClass(CVGroup)
    , public VectorDrawable::VGroup
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetRotation(
        /* [out] */ Float* rotation);

    //CARAPI SetRotation(
    //    /* [in] */ Float rotation);

    CARAPI GetPivotX(
        /* [out] */ Float* pivotx);

    //CARAPI SetPivotX(
    //    /* [in] */ Float pivotx);

    CARAPI GetPivotY(
        /* [out] */ Float* pivoty);

    //CARAPI SetPivotY(
    //    /* [in] */ Float pivoty);

    CARAPI GetScaleX(
        /* [out] */ Float* scalex);

    //CARAPI SetScaleX(
    //    /* [in] */ Float scalex);

    CARAPI GetScaleY(
        /* [out] */ Float* scaley);

    //CARAPI SetScaleY(
    //    /* [in] */ Float scaley);

    CARAPI GetTranslateX(
        /* [out] */ Float* translatex);

    //CARAPI SetTranslateX(
    //    /* [in] */ Float translatex);

    CARAPI GetTranslateY(
        /* [out] */ Float* translatey);

    //CARAPI SetTranslateY(
    //    /* [in] */ Float translatey);
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_DRAWABLE_CVGROUP_H__
