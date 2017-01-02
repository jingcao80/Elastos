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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CTONEMAPCURVE_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CTONEMAPCURVE_H__

#include "_Elastos_Droid_Hardware_Camera2_Params_CTonemapCurve.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IPointF;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Params {

CarClass(CTonemapCurve)
    , public Object
    , public ITonemapCurve
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CTonemapCurve();

    virtual ~CTonemapCurve();

    CARAPI constructor();

    /**
     * Create a new immutable TonemapCurve instance.
     *
     * <p>Values are stored as a contiguous array of {@code (Pin, Pout)} points.</p>
     *
     * <p>All parameters may have independent length but should have at most
     * {@link CameraCharacteristics#TONEMAP_MAX_CURVE_POINTS} * {@value #POINT_SIZE} elements and
     * at least 2 * {@value #POINT_SIZE} elements.</p>
     *
     * <p>All sub-elements must be in the inclusive range of
     * [{@value #LEVEL_BLACK}, {@value #LEVEL_WHITE}].</p>
     *
     * <p>This constructor copies the array contents and does not retain ownership of the array.</p>
     *
     * @param red An array of elements whose length is divisible by {@value #POINT_SIZE}
     * @param green An array of elements whose length is divisible by {@value #POINT_SIZE}
     * @param blue An array of elements whose length is divisible by {@value #POINT_SIZE}
     *
     * @throws IllegalArgumentException
     *            if any of input array length is invalid,
     *            or if any of the elements in the array are not in the range of
     *            [{@value #LEVEL_BLACK}, {@value #LEVEL_WHITE}]
     * @throws NullPointerException
     *            if any of the parameters are {@code null}
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Float>* red,
        /* [in] */ ArrayOf<Float>* green,
        /* [in] */ ArrayOf<Float>* blue);

    /**
     * Get the number of points stored in this tonemap curve for the specified color channel.
     *
     * @param colorChannel one of {@link #CHANNEL_RED}, {@link #CHANNEL_GREEN}, {@link #CHANNEL_BLUE}
     * @return number of points stored in this tonemap for that color's curve (>= 0)
     *
     * @throws IllegalArgumentException if {@code colorChannel} was out of range
     */
    CARAPI GetPointCount(
        /* [in] */ Int32 colorChannel,
        /* [out] */ Int32* value);

    /**
     * Get the point for a color channel at a specified index.
     *
     * <p>The index must be at least 0 but no greater than {@link #getPointCount(int)} for
     * that {@code colorChannel}.</p>
     *
     * <p>All returned coordinates in the point are between the range of
     * [{@value #LEVEL_BLACK}, {@value #LEVEL_WHITE}].</p>
     *
     * @param colorChannel {@link #CHANNEL_RED}, {@link #CHANNEL_GREEN}, or {@link #CHANNEL_BLUE}
     * @param index at least 0 but no greater than {@code getPointCount(colorChannel)}
     * @return the {@code (Pin, Pout)} pair mapping the tone for that index
     *
     * @throws IllegalArgumentException if {@code colorChannel} or {@code index} was out of range
     *
     * @see #LEVEL_BLACK
     * @see #LEVEL_WHITE
     */
    CARAPI GetPoint(
        /* [in] */ Int32 colorChannel,
        /* [in] */ Int32 index,
        /* [out] */ IPointF** outpf);

    /**
     * Copy the color curve for a single color channel from this tonemap curve into the destination.
     *
     * <p>
     * <!--The output is encoded the same as in the constructor -->
     * Values are stored as packed {@code (Pin, Pout}) points, and there are a total of
     * {@link #getPointCount} points for that respective channel.</p>
     *
     * <p>All returned coordinates are between the range of
     * [{@value #LEVEL_BLACK}, {@value #LEVEL_WHITE}].</p>
     *
     * @param destination
     *          an array big enough to hold at least {@link #getPointCount} {@code *}
     *          {@link #POINT_SIZE} elements after the {@code offset}
     * @param offset
     *          a non-negative offset into the array
     * @throws NullPointerException
     *          If {@code destination} was {@code null}
     * @throws IllegalArgumentException
     *          If offset was negative
     * @throws ArrayIndexOutOfBoundsException
     *          If there's not enough room to write the elements at the specified destination and
     *          offset.
     *
     * @see CaptureRequest#TONEMAP_CURVE_BLUE
     * @see CaptureRequest#TONEMAP_CURVE_RED
     * @see CaptureRequest#TONEMAP_CURVE_GREEN
     * @see #LEVEL_BLACK
     * @see #LEVEL_WHITE
     */
    CARAPI CopyColorCurve(
        /* [in] */ Int32 colorChannel,
        /* [in] */ ArrayOf<Float>* destination,
        /* [in] */ Int32 offset);

    /**
     * Check if this TonemapCurve is equal to another TonemapCurve.
     *
     * <p>Two matrices are equal if and only if all of their elements are
     * {@link Object#equals equal}.</p>
     *
     * @return {@code true} if the objects were equal, {@code false} otherwise
     */
    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* value);

    /**
     * {@inheritDoc}
     */
    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* value);

    /**
     * Return the TonemapCurve as a string representation.
     *
     * <p> {@code "TonemapCurve{R:[(%f, %f), (%f, %f) ... (%f, %f)], G:[(%f, %f), (%f, %f) ...
     * (%f, %f)], B:[(%f, %f), (%f, %f) ... (%f, %f)]}"},
     * where each {@code (%f, %f)} respectively represents one point of the corresponding
     * tonemap curve. </p>
     *
     * @return string representation of {@link TonemapCurve}
     */
    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

private:
    static CARAPI CheckArgumentArrayLengthDivisibleBy(
        /* [in] */ ArrayOf<Float>* array,
        /* [in] */ Int32 divisible,
        /* [in] */ const String& arrayName);

    static CARAPI CheckArgumentColorChannel(
        /* [in] */ Int32 colorChannel,
        /* [out] */ Int32* result);

    static CARAPI CheckArgumentArrayLengthNoLessThan(
        /* [in] */ ArrayOf<Float>* array,
        /* [in] */ Int32 minLength,
        /* [in] */ const String& arrayName);

    CARAPI CurveToString(
        /* [in] */ Int32 colorChannel,
        /* [out] */ String* str);

    CARAPI GetCurve(
        /* [in] */ Int32 colorChannel,
        /* [out, callee] */ ArrayOf<Float>** curve);

private:
    const static Int32 OFFSET_POINT_IN;
    const static Int32 OFFSET_POINT_OUT;
    const static Int32 TONEMAP_MIN_CURVE_POINTS;
    const static Int32 MIN_CURVE_LENGTH;

    AutoPtr<ArrayOf<Float> > mRed;
    AutoPtr<ArrayOf<Float> > mGreen;
    AutoPtr<ArrayOf<Float> > mBlue;

    Int32 mHashCode;
    Boolean mHashCalculated;
};

} // namespace Params
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CTONEMAPCURVE_H__
