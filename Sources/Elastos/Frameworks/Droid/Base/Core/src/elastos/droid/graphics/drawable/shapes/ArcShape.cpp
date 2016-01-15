
#include "elastos/droid/graphics/drawable/shapes/ArcShape.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {
namespace Shapes {

CAR_INTERFACE_IMPL(ArcShape, RectShape, IArcShape);
ArcShape::ArcShape()
    : mStart(0)
    , mSweep(0)
{}

ArcShape::ArcShape(
    /* [in] */ Float startAngle,
    /* [in] */ Float sweepAngle)
    : mStart(startAngle)
    , mSweep(sweepAngle)
{}

ECode ArcShape::Draw(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IPaint* paint)
{
    return canvas->DrawArc(Rect(), mStart, mSweep, TRUE, paint);
}

ECode ArcShape::constructor(
    /* [in] */ Float startAngle,
    /* [in] */ Float sweepAngle)
{
    mStart = startAngle;
    mSweep = sweepAngle;
    return NOERROR;
}

ECode ArcShape::CloneImpl(
    /* [in] */ IArcShape* other)
{
    RectShape::CloneImpl(IRectShape::Probe(other));
    ((ArcShape*)other)->mStart = mStart;
    ((ArcShape*)other)->mSweep = mSweep;
    return NOERROR;
}

} // namespace Shapes
} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
