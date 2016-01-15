
#include "elastos/droid/gesture/CGestureUtils.h"

using Elastos::IO::ICloseable;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Gesture {

CAR_SINGLETON_IMPL(CGestureUtils);
CAR_INTERFACE_IMPL(CGestureUtils, Singleton, IGestureUtils);

CGestureUtils::CGestureUtils()
{}

CGestureUtils::~CGestureUtils()
{}

ECode CGestureUtils::CloseStream(
    /* [in] */ ICloseable *stream)
{
    return GestureUtils::CloseStream(stream);
}

ECode CGestureUtils::SpatialSampling(
    /* [in] */ IGesture *gesture,
    /* [in] */ Int32 bitmapSize,
    /* [out, callee] */ ArrayOf<Float> **sample)
{
    AutoPtr<ArrayOf<Float> > ret;

    ret = GestureUtils::SpatialSampling(gesture, bitmapSize);
    *sample = ret;

    return NOERROR;
}

ECode CGestureUtils::SpatialSampling(
    /* [in] */ IGesture *gesture,
    /* [in] */ Int32 bitmapSize,
    /* [in] */ Boolean keepAspectRatio,
    /* [out, callee] */ ArrayOf<Float> **sample)
{
    AutoPtr<ArrayOf<Float> > ret;

    ret = GestureUtils::SpatialSampling(gesture, bitmapSize, keepAspectRatio);
    *sample = ret;

    return NOERROR;
}

ECode CGestureUtils::TemporalSampling(
    /* [in] */ IGestureStroke *stroke,
    /* [in] */ Int32 numPoints,
    /* [out, callee] */ ArrayOf<Float> **vector)
{
    AutoPtr<ArrayOf<Float> > ret;

    ret = GestureUtils::TemporalSampling(stroke, numPoints);
    *vector = ret;

    return NOERROR;
}

ECode CGestureUtils::ComputeCentroid(
    /* [in] */ ArrayOf<Float> *points,
    /* [out, callee] */ ArrayOf<Float> **center)
{
    AutoPtr<ArrayOf<Float> > ret;

    ret = GestureUtils::ComputeCentroid(points);
    *center = ret;

    return NOERROR;
}

ECode CGestureUtils::ComputeTotalLength(
    /* [in] */ ArrayOf<Float> *points,
    /* [out] */ Float *pLength)
{
    *pLength = GestureUtils::ComputeTotalLength(points);
    return NOERROR;
}

ECode CGestureUtils::ComputeStraightness(
    /* [in] */ ArrayOf<Float> *points,
    /* [out] */ Float *pStraightness)
{
    *pStraightness = GestureUtils::ComputeStraightness(points);
    return NOERROR;
}

ECode CGestureUtils::ComputeStraightness(
    /* [in] */ ArrayOf<Float> *points,
    /* [in] */ Float totalLen,
    /* [out] */ Float *pStraightness)
{
    *pStraightness = GestureUtils::ComputeStraightness(points, totalLen);
    return NOERROR;
}

ECode CGestureUtils::SquaredEuclideanDistance(
    /* [in] */ ArrayOf<Float> *vector1,
    /* [in] */ ArrayOf<Float> *vector2,
    /* [out] */ Float *pDistance)
{
    *pDistance = GestureUtils::SquaredEuclideanDistance(vector1, vector2);
    return NOERROR;
}

ECode CGestureUtils::CosineDistance(
    /* [in] */ ArrayOf<Float> *vector1,
    /* [in] */ ArrayOf<Float> *vector2,
    /* [out] */ Float *pDistance)
{
    *pDistance = GestureUtils::CosineDistance(vector1, vector2);
    return NOERROR;
}

ECode CGestureUtils::MinimumCosineDistance(
    /* [in] */ ArrayOf<Float> *vector1,
    /* [in] */ ArrayOf<Float> *vector2,
    /* [in] */ Int32 numOrientations,
    /* [out] */ Float *pDistance)
{
    *pDistance = GestureUtils::MinimumCosineDistance(vector1, vector2, numOrientations);
    return NOERROR;
}

ECode CGestureUtils::ComputeOrientedBoundingBox(
    /* [in] */ Elastos::Utility::IArrayList *originalPoints,
    /* [out] */ IOrientedBoundingBox **ppBox)
{
    AutoPtr<IOrientedBoundingBox> ret;

    ret = GestureUtils::ComputeOrientedBoundingBox(originalPoints);

    *ppBox = ret;
    return NOERROR;

}

ECode CGestureUtils::ComputeOrientedBoundingBox(
    /* [in] */ ArrayOf<Float> *originalPoints,
    /* [out] */ IOrientedBoundingBox **ppBox)
{
    AutoPtr<IOrientedBoundingBox> ret;

    ret = GestureUtils::ComputeOrientedBoundingBox(originalPoints);

    *ppBox = ret;
    return NOERROR;

}

ECode CGestureUtils::Rotate(
    /* [in] */ ArrayOf<Float> *points,
    /* [in] */ Float angle,
    /* [out, callee] */ ArrayOf<Float> **ppPoints)
{
    *ppPoints = GestureUtils::Rotate(points, angle);
    return NOERROR;
}

ECode CGestureUtils::Translate(
    /* [in] */ ArrayOf<Float>* points,
    /* [in] */ Float dx,
    /* [in] */ Float dy,
    /* [out, callee] */ ArrayOf<Float>** ppPoints)
{
    *ppPoints = GestureUtils::Translate(points, dx, dy);
    return NOERROR;
}

ECode CGestureUtils::Scale(
    /* [in] */ ArrayOf<Float>* points,
    /* [in] */ Float sx,
    /* [in] */ Float sy,
    /* [out, callee] */ ArrayOf<Float> **ppPoints)
{
    *ppPoints = GestureUtils::Scale(points, sx, sy);
    return NOERROR;
}

} // namespace Gesture
} // namespace Droid
} // namespace Elastos
