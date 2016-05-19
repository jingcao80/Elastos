#include "elastos/droid/graphics/drawable/CVGroup.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CAR_OBJECT_IMPL(CVGroup)

ECode CVGroup::GetRotation(
    /* [out] */ Float* rotation)
{
    VALIDATE_NOT_NULL(rotation);
    *rotation = VectorDrawable::VGroup::GetRotation();
    return NOERROR;
}

//ECode CVGroup::SetRotation(
//    /* [in] */ Float rotation)
//{
//    VectorDrawable::VGroup::SetRotation(rotation);
//    return NOERROR;
//}

ECode CVGroup::GetPivotX(
    /* [out] */ Float* pivotx)
{
    VALIDATE_NOT_NULL(pivotx);
    *pivotx = VectorDrawable::VGroup::GetPivotX();
    return NOERROR;
}

//ECode CVGroup::SetPivotX(
//    /* [in] */ Float pivotx)
//{
//    VectorDrawable::VGroup::SetPivotX(pivotx);
//    return NOERROR;
//}

ECode CVGroup::GetPivotY(
    /* [out] */ Float* pivoty)
{
    VALIDATE_NOT_NULL(pivoty);
    *pivoty = VectorDrawable::VGroup::GetPivotY();
    return NOERROR;
}

//ECode CVGroup::SetPivotY(
//    /* [in] */ Float pivoty)
//{
//    VectorDrawable::VGroup::SetPivotY(pivoty);
//    return NOERROR;
//}

ECode CVGroup::GetScaleX(
    /* [out] */ Float* scalex)
{
    VALIDATE_NOT_NULL(scalex);
    *scalex = VectorDrawable::VGroup::GetScaleX();
    return NOERROR;
}

//ECode CVGroup::SetScaleX(
//    /* [in] */ Float scalex)
//{
//    VectorDrawable::VGroup::SetScaleX(scalex);;
//    return NOERROR;
//}

ECode CVGroup::GetScaleY(
    /* [out] */ Float* scaley)
{
    VALIDATE_NOT_NULL(scaley);
    *scaley = VectorDrawable::VGroup::GetScaleY();
    return NOERROR;
}

//ECode CVGroup::SetScaleY(
//    /* [in] */ Float scaley)
//{
//    VectorDrawable::VGroup::SetScaleY(scaley);
//    return NOERROR;
//}

ECode CVGroup::GetTranslateX(
    /* [out] */ Float* translatex)
{
    VALIDATE_NOT_NULL(translatex);
    *translatex = VectorDrawable::VGroup::GetTranslateX();
    return NOERROR;
}

//ECode CVGroup::SetTranslateX(
//    /* [in] */ Float translatex)
//{
//    VectorDrawable::VGroup::SetTranslateX(translatex);
//    return NOERROR;
//}

ECode CVGroup::GetTranslateY(
    /* [out] */ Float* translatey)
{
    VALIDATE_NOT_NULL(translatey);
    *translatey = VectorDrawable::VGroup::GetTranslateY();
    return NOERROR;
}

//ECode CVGroup::SetTranslateY(
//    /* [in] */ Float translatey)
//{
//    VectorDrawable::VGroup::SetTranslateY(translatey);;
//    return NOERROR;
//}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
