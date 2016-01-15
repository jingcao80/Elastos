
#include <Elastos.CoreLibrary.Core.h>
#include "elastos/droid/graphics/drawable/shapes/Shape.h"

using Elastos::Core::EIID_ICloneable;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {
namespace Shapes {

CAR_INTERFACE_IMPL_2(Shape, Object, IShape, ICloneable)

Shape::Shape()
    : mWidth(0)
    , mHeight(0)
{
}

ECode Shape::GetWidth(
    /* [out] */ Float* width)
{
    VALIDATE_NOT_NULL(width);
    *width = mWidth;
    return NOERROR;
}

ECode Shape::GetHeight(
    /* [out] */ Float* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mHeight;
    return NOERROR;
}

ECode Shape::Resize(
    /* [in] */ Float width,
    /* [in] */ Float height)
{
    if (width < 0) {
        width = 0;
    }
    if (height < 0) {
        height =0;
    }
    if (mWidth != width || mHeight != height) {
        mWidth = width;
        mHeight = height;
        OnResize(width, height);
    }

    return NOERROR;
}

ECode Shape::HasAlpha(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    *has = TRUE;
    return NOERROR;
}

ECode Shape::GetOutline(
    /* [in] */ IOutline* outline)
{
    return NOERROR;
}

void Shape::OnResize(
    /* [in] */ Float width,
    /* [in] */ Float height)
{}

// ECode Shape::Clone(
//     /* [out] */ IInterface** other)
// {
//     VALIDATE_NOT_NULL(other);
//     *other = THIS_PROBE(EIID_IInterface);
//     REFCOUNT_ADD(*other);
//     return NOERROR;
// }

ECode Shape::CloneImpl(
    /* [in] */ IShape* other)
{
    ((Shape*)other)->mWidth = mWidth;
    ((Shape*)other)->mHeight = mHeight;
    return NOERROR;
}

} // namespace Shapes
} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
