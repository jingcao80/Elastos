
#include "elastos/droid/graphics/drawable/shapes/PathShape.h"
#include "elastos/droid/graphics/CPath.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {
namespace Shapes {

CAR_INTERFACE_IMPL(PathShape, Shape, IPathShape);
PathShape::PathShape()
    : mStdWidth(0)
    , mStdHeight(0)
    , mScaleX(0)
    , mScaleY(0)
{}

PathShape::PathShape(
    /* [in] */ IPath* path,
    /* [in] */ Float stdWidth,
    /* [in] */ Float stdHeight)
    : mPath(path)
    , mStdWidth(stdWidth)
    , mStdHeight(stdHeight)
{}

ECode PathShape::constructor(
    /* [in] */ IPath* path,
    /* [in] */ Float stdWidth,
    /* [in] */ Float stdHeight)
{
    mPath = path;
    mStdWidth = stdWidth;
    mStdHeight = stdHeight;
    return NOERROR;
}

ECode PathShape::Draw(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IPaint* paint)
{
    Int32 count;
    canvas->Save(&count);
    canvas->Scale(mScaleX, mScaleY);
    canvas->DrawPath(mPath, paint);
    canvas->Restore();
    return NOERROR;
}

void PathShape::OnResize(
    /* [in] */ Float width,
    /* [in] */ Float height)
{
    mScaleX = width / mStdWidth;
    mScaleY = height / mStdHeight;
}

ECode PathShape::CloneImpl(
    /* [in] */ IPathShape* _other)
{
    PathShape* other = (PathShape*)_other;
    Shape::CloneImpl(IShape::Probe(other));
    CPath::New(mPath, (IPath**)&other->mPath);
    other->mStdWidth = mStdWidth;
    other->mStdHeight = mStdHeight;
    other->mScaleX = mScaleX;
    other->mScaleY = mScaleY;
    return NOERROR;
}

} // namespace Shapes
} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
