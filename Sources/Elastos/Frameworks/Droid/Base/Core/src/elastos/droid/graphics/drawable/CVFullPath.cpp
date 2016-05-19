#include "elastos/droid/graphics/drawable/CVFullPath.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CAR_OBJECT_IMPL(CVFullPath)

ECode CVFullPath::GetStrokeColor(
    /* [out] */ Int32* strokeColor)
{
    VALIDATE_NOT_NULL(strokeColor);
    *strokeColor = VectorDrawable::VFullPath::GetStrokeColor();
    return NOERROR;
}

//ECode CVFullPath::SetStrokeColor(
//    /* [in] */ Int32 strokeColor)
//{
//    VectorDrawable::VFullPath::SetStrokeColor(strokeColor);
//    return NOERROR;
//}

ECode CVFullPath::GetStrokeWidth(
    /* [out] */ Float* strokeWidth)
{
    VALIDATE_NOT_NULL(strokeWidth);
    *strokeWidth = VectorDrawable::VFullPath::GetStrokeWidth();
    return NOERROR;
}

//ECode CVFullPath::SetStrokeWidth(
//    /* [in] */ Float strokeWidth)
//{
//    VectorDrawable::VFullPath::SetStrokeWidth(strokeWidth);
//    return NOERROR;
//}

ECode CVFullPath::GetStrokeAlpha(
     /* [out] */ Float* strokeAlpha)
{
    VALIDATE_NOT_NULL(strokeAlpha);
    *strokeAlpha = VectorDrawable::VFullPath::GetStrokeAlpha();
    return NOERROR;
}

//ECode CVFullPath::SetStrokeAlpha(
//    /* [in] */ Float strokeAlpha)
//{
//    VectorDrawable::VFullPath::SetStrokeAlpha(strokeAlpha);
//    return NOERROR;
//}

ECode CVFullPath::GetFillColor(
    /* [out] */ Int32* fillColor)
{
    VALIDATE_NOT_NULL(fillColor);
    *fillColor = VectorDrawable::VFullPath::GetFillColor();
    return NOERROR;
}

//ECode CVFullPath::SetFillColor(
//    /* [in] */ Int32 fillColor)
//{
//    VectorDrawable::VFullPath::SetFillColor(fillColor);
//    return NOERROR;
//}

ECode CVFullPath::GetFillAlpha(
    /* [out] */ Float* fillAlpha)
{
    VALIDATE_NOT_NULL(fillAlpha);
    *fillAlpha = VectorDrawable::VFullPath::GetFillAlpha();
    return NOERROR;
}

//ECode CVFullPath::SetFillAlpha(
//    /* [in] */ Float fillAlpha)
//{
//    VectorDrawable::VFullPath::SetFillAlpha(fillAlpha);
//    return NOERROR;
//}

ECode CVFullPath::GetTrimPathStart(
    /* [out] */ Float* trimPathStart)
{
    VALIDATE_NOT_NULL(trimPathStart);
    *trimPathStart = VectorDrawable::VFullPath::GetTrimPathStart();
    return NOERROR;
}

//ECode CVFullPath::SetTrimPathStart(
//    /* [in] */ Float trimPathStart)
//{
//    VectorDrawable::VFullPath::SetTrimPathStart(trimPathStart);
//    return NOERROR;
//}

ECode CVFullPath::GetTrimPathEnd(
    /* [out] */ Float* trimPathEnd)
{
    VALIDATE_NOT_NULL(trimPathEnd);
    *trimPathEnd = VectorDrawable::VFullPath::GetTrimPathEnd();
    return NOERROR;
}

//ECode CVFullPath::SetTrimPathEnd(
//    /* [in] */ Float trimPathEnd)
//{
//    VectorDrawable::VFullPath::SetTrimPathEnd(trimPathEnd);
//    return NOERROR;
//}

ECode CVFullPath::GetTrimPathOffset(
    /* [out] */ Float* trimPathOffset)
{
    VALIDATE_NOT_NULL(trimPathOffset);
    *trimPathOffset = VectorDrawable::VFullPath::GetTrimPathOffset();
    return NOERROR;
}

//ECode CVFullPath::SetTrimPathOffset(
//    /* [in] */ Float trimPathOffset)
//{
//    VectorDrawable::VFullPath::SetTrimPathOffset(trimPathOffset);
//    return NOERROR;
//}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
