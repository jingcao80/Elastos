#include "elastos/droid/media/CFace.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CFace, Object, IFace)

CAR_OBJECT_IMPL(CFace)

CFace::~CFace()
{
}

CFace::CFace()
    : mConfidence(0)
    , mMidPointX(0)
    , mMidPointY(0)
    , mEyesDist(0)
    , mPoseEulerX(0)
    , mPoseEulerY(0)
    , mPoseEulerZ(0)
{
}

ECode CFace::constructor()
{
    return NOERROR;
}

ECode CFace::Confidence(
    /* [out] */ Float* factor)
{
    VALIDATE_NOT_NULL(factor);
    *factor = mConfidence;
    return NOERROR;
}

ECode CFace::GetMidPoint(
    /* [in] */ IPointF* point)
{
    VALIDATE_NOT_NULL(point);
    point->Set(mMidPointX, mMidPointY);
    return NOERROR;
}

ECode CFace::EyesDistance(
    /* [out] */ Float* distance)
{
    VALIDATE_NOT_NULL(distance);
    *distance = mEyesDist;
    return NOERROR;
}

ECode CFace::Pose(
    /* [in] */ Int32 euler,
    /* [out] */ Float* angle)
{
    VALIDATE_NOT_NULL(angle);
    *angle = 0.0f;

    // don't use an array to avoid allocations
    if (euler == IFace::EULER_X)
        *angle =  mPoseEulerX;
    else if (euler == IFace::EULER_Y)
        *angle =  mPoseEulerY;
    else if (euler == IFace::EULER_Z)
        *angle = mPoseEulerZ;
    else
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
