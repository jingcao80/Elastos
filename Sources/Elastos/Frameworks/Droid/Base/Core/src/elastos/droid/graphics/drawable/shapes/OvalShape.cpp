
#include "elastos/droid/graphics/drawable/shapes/OvalShape.h"
#include <elastos/core/Math.h>

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {
namespace Shapes {

CAR_INTERFACE_IMPL(OvalShape, RectShape, IOvalShape);
OvalShape::OvalShape()
{}

ECode OvalShape::Draw(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IPaint* paint)
{
    return canvas->DrawOval(Rect().Get(), paint);
}

ECode OvalShape::GetOutline(
    /* [in] */ IOutline* outline)
{
    AutoPtr<IRectF> rect = Rect();
    Float left = 0.f, top = 0.f, right = 0.f, bottom = 0.f;
    rect->Get(&left, &top, &right, &bottom);
    return outline->SetOval((Int32) Elastos::Core::Math::Ceil(left), (Int32) Elastos::Core::Math::Ceil(top),
            (Int32) Elastos::Core::Math::Floor(right), (Int32) Elastos::Core::Math::Floor(bottom));
}

} // namespace Shapes
} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
