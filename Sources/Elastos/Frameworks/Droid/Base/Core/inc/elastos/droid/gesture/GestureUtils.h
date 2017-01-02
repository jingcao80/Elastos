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

#ifndef __ELASTOS_DROID_GESTURE_GESTUREUTILS_H__
#define __ELASTOS_DROID_GESTURE_GESTUREUTILS_H__

#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/utility/etl/List.h>

using Elastos::IO::ICloseable;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Gesture {
/**
 * Utility functions for gesture processing & analysis, including methods for:
 * <ul>
 * <li>feature extraction (e.g., samplers and those for calculating bounding
 * boxes and gesture path lengths);
 * <li>geometric transformation (e.g., translation, rotation and scaling);
 * <li>gesture similarity comparison (e.g., calculating Euclidean or Cosine
 * distances between two gestures).
 * </ul>
 */
class GestureUtils
{
public:
    /**
     * Closes the specified stream.
     *
     * @param stream The stream to close.
     */
    static CARAPI CloseStream(
        /* [in] */ ICloseable* stream);

    /**
     * Samples the gesture spatially by rendering the gesture into a 2D
     * grayscale bitmap. Scales the gesture to fit the size of the bitmap.
     * The scaling does not necessarily keep the aspect ratio of the gesture.
     *
     * @param gesture the gesture to be sampled
     * @param bitmapSize the size of the bitmap
     * @return a bitmapSize x bitmapSize grayscale bitmap that is represented
     *         as a 1D array. The float at index i represents the grayscale
     *         value at pixel [i%bitmapSize, i/bitmapSize]
     */
    static CARAPI_(AutoPtr<ArrayOf<Float> >) SpatialSampling(
        /* [in] */ IGesture *gesture,
        /* [in] */ Int32 bitmapSize);

    /**
     * Samples the gesture spatially by rendering the gesture into a 2D
     * grayscale bitmap. Scales the gesture to fit the size of the bitmap.
     *
     * @param gesture the gesture to be sampled
     * @param bitmapSize the size of the bitmap
     * @param keepAspectRatio if the scaling should keep the gesture's
     *        aspect ratio
     *
     * @return a bitmapSize x bitmapSize grayscale bitmap that is represented
     *         as a 1D array. The float at index i represents the grayscale
     *         value at pixel [i%bitmapSize, i/bitmapSize]
     */
    static CARAPI_(AutoPtr<ArrayOf<Float> >) SpatialSampling(
        /* [in] */ IGesture *gesture,
        /* [in] */ Int32 bitmapSize,
        /* [in] */ Boolean keepAspectRatio);

    /**
     * Samples a stroke temporally into a given number of evenly-distributed
     * points.
     *
     * @param stroke the gesture stroke to be sampled
     * @param numPoints the number of points
     * @return the sampled points in the form of [x1, y1, x2, y2, ..., xn, yn]
     */
    static CARAPI_(AutoPtr<ArrayOf<Float> >) TemporalSampling(
        /* [in] */ IGestureStroke *stroke,
        /* [in] */ Int32 numPoints);

    /**
     * Computes an oriented, minimum bounding box of a set of points.
     *
     * @param originalPoints
     * @return an oriented bounding box
     */
    static CARAPI_(AutoPtr<IOrientedBoundingBox>) ComputeOrientedBoundingBox(
        /* [in] */ IArrayList *originalPoints);

    /**
     * Computes an oriented, minimum bounding box of a set of points.
     *
     * @param originalPoints
     * @return an oriented bounding box
     */
    static CARAPI_(AutoPtr<IOrientedBoundingBox>) ComputeOrientedBoundingBox(
        /* [in] */ ArrayOf<Float> *originalPoints);

    static CARAPI_(AutoPtr<IOrientedBoundingBox>) ComputeOrientedBoundingBox(
            /* [in] */ ArrayOf<Float> *points,
            /* [in] */ ArrayOf<Float> *centroid);


    /**
     * Calculates the centroid of a set of points.
     *
     * @param points the points in the form of [x1, y1, x2, y2, ..., xn, yn]
     * @return the centroid
     */
    static CARAPI_(AutoPtr<ArrayOf<Float> >) ComputeCentroid(
            /* [in] */ ArrayOf<Float>* points);

    static CARAPI_(Float) ComputeTotalLength(
            /* [in] */ ArrayOf<Float> *points);

    static CARAPI_(Float) ComputeStraightness(
            /* [in] */ ArrayOf<Float> *points);

    static CARAPI_(Float) ComputeStraightness(
            /* [in] */ ArrayOf<Float> *points,
            /* [in] */ Float totalLen);

    static CARAPI_(Float) SquaredEuclideanDistance(
            /* [in] */ ArrayOf<Float> *vector1,
            /* [in] */ ArrayOf<Float> *vector2);

    static CARAPI_(Float) CosineDistance(
            /* [in] */ ArrayOf<Float> *vector1,
            /* [in] */ ArrayOf<Float> *vector2);

    static CARAPI_(Float) MinimumCosineDistance(
            /* [in] */ ArrayOf<Float> *vector1,
            /* [in] */ ArrayOf<Float> *vector2,
            /* [in] */ Int32 numOrientations);

    static CARAPI_(AutoPtr<ArrayOf<Float> >) ComputeOrientation(
                /* [in] */ ArrayOf<FloatArray> *covarianceMatrix);

    static CARAPI_(AutoPtr<ArrayOf<Float> >) Rotate(
            /* [in] */ ArrayOf<Float> *points,
            /* [in] */ Float angle);

    static CARAPI_(AutoPtr<ArrayOf<Float> >) Translate(
            /* [in] */ ArrayOf<Float> *points,
            /* [in] */ Float dx,
            /* [in] */ Float dy);

    static CARAPI_(AutoPtr<ArrayOf<Float> >) Scale(
            /* [in] */ ArrayOf<Float> *points,
            /* [in] */ Float sx,
            /* [in] */ Float sy);

    /**
     * Calculates the variance-covariance matrix of a set of points.
     *
     * @param points the points in the form of [x1, y1, x2, y2, ..., xn, yn]
     * @return the variance-covariance matrix
     */
    static CARAPI_(AutoPtr<ArrayOf<FloatArray> >) ComputeCoVariance(
        /* [in] */ ArrayOf<Float>* points);

    static CARAPI_(void) Plot(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ ArrayOf<Float>* sample,
        /* [in] */ Int32 sampleSize);

private:
    GestureUtils();
    GestureUtils(const GestureUtils&);

private:
    const static Float SCALING_THRESHOLD;
    const static Float NONUNIFORM_SCALE;
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GESTURE_GESTUREUTILS_H__
