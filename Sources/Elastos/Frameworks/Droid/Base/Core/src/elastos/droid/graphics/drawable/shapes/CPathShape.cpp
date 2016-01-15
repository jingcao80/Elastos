
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/shapes/CPathShape.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {
namespace Shapes {

CAR_OBJECT_IMPL(CPathShape);
ECode CPathShape::constructor(
    /* [in] */ IPath* path,
    /* [in] */ Float stdWidth,
    /* [in] */ Float stdHeight)
{
    return PathShape::constructor(path, stdWidth, stdHeight);
}

ECode CPathShape::Clone(
    /* [out] */ IInterface** shape)
{
    VALIDATE_NOT_NULL(shape);
    AutoPtr<IPathShape> s;
    CPathShape::New(NULL, 0, 0, (IPathShape**)&s);
    PathShape::CloneImpl(s);
    *shape = s;
    REFCOUNT_ADD(*shape);
    return NOERROR;
}

} // namespace Shapes
} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
