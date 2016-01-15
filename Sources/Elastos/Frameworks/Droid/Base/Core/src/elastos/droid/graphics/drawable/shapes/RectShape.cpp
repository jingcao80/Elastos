
#include "elastos/droid/graphics/drawable/shapes/RectShape.h"
#include "elastos/droid/graphics/CRectF.h"
#include <elastos/core/Math.h>

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {
namespace Shapes {

CAR_INTERFACE_IMPL(RectShape, Shape, IRectShape);
RectShape::RectShape()
{
    CRectF::New((IRectF**)&mRect);
}

ECode RectShape::Draw(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IPaint* paint)
{
    return canvas->DrawRect(mRect.Get(), paint);
}

ECode RectShape::GetOutline(
    /* [in] */ IOutline* outline)
{
    AutoPtr<IRectF> rect = Rect();
    Float left = 0.f, top = 0.f, right = 0.f, bottom = 0.f;
    rect->Get(&left, &top, &right, &bottom);
    return outline->SetRect((Int32) Elastos::Core::Math::Ceil(left), (Int32) Elastos::Core::Math::Ceil(top),
            (Int32) Elastos::Core::Math::Floor(right), (Int32) Elastos::Core::Math::Floor(bottom));
}

void RectShape::OnResize(
    /* [in] */ Float width,
    /* [in] */ Float height)
{
    mRect->Set(0, 0, width, height);
}

AutoPtr<IRectF> RectShape::Rect()
{
    return mRect;
}

ECode RectShape::CloneImpl(
    /* [in] */ IRectShape* other)
{
    assert(other != NULL);
    Shape::CloneImpl(IShape::Probe(other));
    return CRectF::New(mRect.Get(), (IRectF**)&((RectShape*)other)->mRect);
}

} // namespace Shapes
} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
