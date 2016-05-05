
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/shapes/CRoundRectShape.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {
namespace Shapes {

CAR_OBJECT_IMPL(CRoundRectShape);
ECode CRoundRectShape::constructor(
    /* [in] */ ArrayOf<Float>* outerRadii,
    /* [in] */ IRectF* inset,
    /* [in] */ ArrayOf<Float>* innerRadii)
{
    return RoundRectShape::constructor(outerRadii, inset, innerRadii);
}

ECode CRoundRectShape::Clone(
    /* [out] */ IInterface** shape)
{
    VALIDATE_NOT_NULL(shape);
    AutoPtr<IRoundRectShape> s;
    CRoundRectShape::New(NULL, NULL, NULL, (IRoundRectShape**)&s);
    RoundRectShape::CloneImpl(s);
    *shape = IShape::Probe(s);
    REFCOUNT_ADD(*shape);
    return NOERROR;
}

} // namespace Shapes
} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
