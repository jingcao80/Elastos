//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
