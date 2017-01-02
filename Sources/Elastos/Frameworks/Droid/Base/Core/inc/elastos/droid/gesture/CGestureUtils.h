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

#ifndef __ELASTOS_DROID_GESTURE_CGESTUREUTILS_H__
#define __ELASTOS_DROID_GESTURE_CGESTUREUTILS_H__

#include "elastos/droid/gesture/GestureUtils.h"
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Gesture_CGestureUtils.h"
#include <elastos/core/Singleton.h>

using Elastos::Utility::IArrayList;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Gesture {

CarClass(CGestureUtils)
    , public Singleton
    , public IGestureUtils
{
public:
    CAR_SINGLETON_DECL();

    CAR_INTERFACE_DECL()

    CGestureUtils();

    virtual ~CGestureUtils();

    CARAPI CloseStream(
        /* [in] */ ICloseable* stream);

    CARAPI SpatialSampling(
        /* [in] */ IGesture* gesture,
        /* [in] */ Int32 bitmapSize,
        /* [out, callee] */ ArrayOf<Float>** sample);

    CARAPI SpatialSampling(
        /* [in] */ IGesture* gesture,
        /* [in] */ Int32 bitmapSize,
        /* [in] */ Boolean keepAspectRatio,
        /* [out, callee] */ ArrayOf<Float>** sample);

    CARAPI TemporalSampling(
        /* [in] */ IGestureStroke* stroke,
        /* [in] */ Int32 numPoints,
        /* [out, callee] */ ArrayOf<Float>** vector);

    CARAPI ComputeCentroid(
        /* [in] */ ArrayOf<Float>* points,
        /* [out, callee] */ ArrayOf<Float>** center);

    CARAPI ComputeTotalLength(
        /* [in] */ ArrayOf<Float>* points,
        /* [out] */ Float* pLength);

    CARAPI ComputeStraightness(
        /* [in] */ ArrayOf<Float>* points,
        /* [out] */ Float* pStraightness);

    CARAPI ComputeStraightness(
        /* [in] */ ArrayOf<Float>* points,
        /* [in] */ Float totalLen,
        /* [out] */ Float* pStraightness);

    CARAPI SquaredEuclideanDistance(
        /* [in] */ ArrayOf<Float>* vector1,
        /* [in] */ ArrayOf<Float>* vector2,
        /* [out] */ Float* pDistance);

    CARAPI CosineDistance(
        /* [in] */ ArrayOf<Float>* vector1,
        /* [in] */ ArrayOf<Float>* vector2,
        /* [out] */ Float* pDistance);

    CARAPI MinimumCosineDistance(
        /* [in] */ ArrayOf<Float>* vector1,
        /* [in] */ ArrayOf<Float>* vector2,
        /* [in] */ Int32 numOrientations,
        /* [out] */ Float* pDistance);

    CARAPI ComputeOrientedBoundingBox(
        /* [in] */ IArrayList* originalPoints,
        /* [out] */ IOrientedBoundingBox** ppBox);

    CARAPI ComputeOrientedBoundingBox(
        /* [in] */ ArrayOf<Float>* originalPoints,
        /* [out] */ IOrientedBoundingBox **ppBox);

    CARAPI Rotate(
        /* [in] */ ArrayOf<Float>* points,
        /* [in] */ Float angle,
        /* [out, callee] */ ArrayOf<Float>** ppPoints);

    CARAPI Translate(
        /* [in] */ ArrayOf<Float>* points,
        /* [in] */ Float dx,
        /* [in] */ Float dy,
        /* [out, callee] */ ArrayOf<Float>** ppPoints);

    CARAPI Scale(
        /* [in] */ ArrayOf<Float>* points,
        /* [in] */ Float sx,
        /* [in] */ Float sy,
        /* [out, callee] */ ArrayOf<Float>** ppPoints);
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GESTURE_CGESTUREUTILS_H__
