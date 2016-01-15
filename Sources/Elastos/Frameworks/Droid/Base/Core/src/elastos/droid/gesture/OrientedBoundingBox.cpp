#include "elastos/droid/gesture/OrientedBoundingBox.h"

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CPath.h"
#include "elastos/droid/graphics/CMatrix.h"

using Elastos::Droid::Graphics::CPath;
using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::CMatrix;

using Elastos::Droid::Gesture::IOrientedBoundingBox;

namespace Elastos {
namespace Droid {
namespace Gesture {

CAR_INTERFACE_IMPL(OrientedBoundingBox, Object, IOrientedBoundingBox);

OrientedBoundingBox::OrientedBoundingBox()
    : mSquareness(0)
    , mWidth(0)
    , mHeight(0)
    , mOrientation(0)
    , mCenterX(0)
    , mCenterY(0)
{}

OrientedBoundingBox::~OrientedBoundingBox()
{}

ECode OrientedBoundingBox::constructor(
    /* [in] */ Float angle,
    /* [in] */ Float cx,
    /* [in] */ Float cy,
    /* [in] */ Float w,
    /* [in] */ Float h)
{
    mOrientation = angle;
    mWidth = w;
    mHeight = h;
    mCenterX = cx;
    mCenterY = cy;
    Float ratio = w / h;
    if (ratio > 1) {
        mSquareness = 1 / ratio;
    }
    else {
        mSquareness = ratio;
    }

    return NOERROR;
}

ECode OrientedBoundingBox::ToPath(
    /* [out] */ IPath **outPath)
{
    VALIDATE_NOT_NULL(outPath);

    AutoPtr<IPath> path;
    CPath::New((IPath**)&path);
    AutoPtr<ArrayOf<Float> > point = ArrayOf<Float>::Alloc(2);
    (*point)[0] = -mWidth / 2;
    (*point)[1] = mHeight / 2;
    AutoPtr<IMatrix> matrix;
    CMatrix::New((IMatrix**)&matrix);
    matrix->SetRotate(mOrientation);
    Boolean isPostTranslated;
    matrix->PostTranslate(mCenterX, mCenterY, &isPostTranslated);
    matrix->MapPoints(point);
    path->MoveTo((*point)[0], (*point)[1]);

    (*point)[0] = -mWidth / 2;
    (*point)[1] = -mHeight / 2;
    matrix->MapPoints(point);
    path->LineTo((*point)[0], (*point)[1]);

    (*point)[0] = mWidth / 2;
    (*point)[1] = -mHeight / 2;
    matrix->MapPoints(point);
    path->LineTo((*point)[0], (*point)[1]);

    (*point)[0] = mWidth / 2;
    (*point)[1] = mHeight / 2;
    matrix->MapPoints(point);
    path->LineTo((*point)[0], (*point)[1]);

    path->Close();

    *outPath = path;
    REFCOUNT_ADD(*outPath);
    return NOERROR;
}

ECode OrientedBoundingBox::GetSquareness(
    /* [out] */ Float* squareness)
{
    VALIDATE_NOT_NULL(squareness);

    *squareness = mSquareness;
    return NOERROR;
}

ECode OrientedBoundingBox::GetWidth(
    /* [out] */ Float *width)
{
    VALIDATE_NOT_NULL(width);
    *width = mWidth;
    return NOERROR;
}

ECode OrientedBoundingBox::GetHeight(
    /* [out] */ Float *height)
{
    VALIDATE_NOT_NULL(height);
    *height = mHeight;
    return NOERROR;
}

ECode OrientedBoundingBox::GetOrientation(
    /* [out] */ Float *orientation)
{
    VALIDATE_NOT_NULL(orientation);
    *orientation = mOrientation;
    return NOERROR;
}

ECode OrientedBoundingBox::GetCenterX(
    /* [out] */ Float *centerX)
{
    VALIDATE_NOT_NULL(centerX);
    *centerX = mCenterX;
    return NOERROR;
}

ECode OrientedBoundingBox::GetCenterY(
    /* [out] */ Float *centerY)
{
    VALIDATE_NOT_NULL(centerY);
    *centerY = mCenterY;
    return NOERROR;
}

} // namespace Gesture
} // namespace Droid
} // namespace Elastos
