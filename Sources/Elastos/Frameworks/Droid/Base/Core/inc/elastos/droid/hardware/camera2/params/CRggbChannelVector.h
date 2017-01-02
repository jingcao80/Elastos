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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CRGGBCHANNELVECTOR_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CRGGBCHANNELVECTOR_H__

#include "_Elastos_Droid_Hardware_Camera2_Params_CRggbChannelVector.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Params {

CarClass(CRggbChannelVector)
    , public Object
    , public IRggbChannelVector
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CRggbChannelVector();

    virtual ~CRggbChannelVector();

    CARAPI constructor();

    /**
     * Create a new {@link RggbChannelVector} from an RGGB 2x2 pixel.
     *
     * <p>All pixel values are considered normalized within {@code [0.0f, 1.0f]}
     * (i.e. {@code 1.0f} could be linearized to {@code 255} if converting to a
     * non-floating point pixel representation).</p>
     *
     * <p>All arguments must be finite; NaN and infinity is not allowed.</p>
     *
     * @param red red pixel
     * @param greenEven green pixel (even row)
     * @param greenOdd green pixel (odd row)
     * @param blue blue pixel
     *
     * @throws IllegalArgumentException if any of the arguments were not finite
     */
    CARAPI constructor(
        /* [in] */ Float red,
        /* [in] */ Float greenEven,
        /* [in] */ Float greenOdd,
        /* [in] */ Float blue);

    /**
     * Get the red component.
     *
     * @return a floating point value (guaranteed to be finite)
     */
    CARAPI GetRed(
        /* [out] */ Float* value);

    /**
     * Get the green (even rows) component.
     *
     * @return a floating point value (guaranteed to be finite)
     */
    CARAPI GetGreenEven(
        /* [out] */ Float* value);

    /**
     * Get the green (odd rows) component.
     *
     * @return a floating point value (guaranteed to be finite)
     */
    CARAPI GetGreenOdd(
        /* [out] */ Float* value);

    /**
     * Get the blue component.
     *
     * @return a floating point value (guaranteed to be finite)
     */
    CARAPI GetBlue(
        /* [out] */ Float* value);

    /**
     * Get the component by the color channel index.
     *
     * <p>{@code colorChannel} must be one of {@link #RED}, {@link #GREEN_EVEN}, {@link #GREEN_ODD},
     * {@link #BLUE}.</p>
     *
     * @param colorChannel greater or equal to {@code 0} and less than {@link #COUNT}
     * @return a floating point value (guaranteed to be finite)
     *
     * @throws IllegalArgumentException if {@code colorChannel} was out of range
     */
    CARAPI GetComponent(
        /* [in] */ Int32 colorChannel,
        /* [out] */ Float* value);

    /**
     * Copy the vector into the destination in the order {@code [R, Geven, Godd, B]}.
     *
     * @param destination
     *          an array big enough to hold at least {@value #COUNT} elements after the
     *          {@code offset}
     * @param offset
     *          a non-negative offset into the array
     *
     * @throws NullPointerException
     *          If {@code destination} was {@code null}
     * @throws ArrayIndexOutOfBoundsException
     *          If there's not enough room to write the elements at the specified destination and
     *          offset.
     */
    CARAPI CopyTo(
        /* [in] */ ArrayOf<Float>* destination,
        /* [in] */ Int32 offset);

    /**
     * Check if this {@link RggbChannelVector} is equal to another {@link RggbChannelVector}.
     *
     * <p>Two vectors are only equal if and only if each of the respective elements is equal.</p>
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
     * Return the RggbChannelVector as a string representation.
     *
     * <p> {@code "RggbChannelVector{R:%f, G_even:%f, G_odd:%f, B:%f}"}, where each
     * {@code %f} respectively represents one of the the four color channels. </p>
     *
     * @return string representation of {@link RggbChannelVector}
     */
    // @Override
    CARAPI ToString(
       /* [out] */ String* str);

private:
    /**
     * Return the RggbChannelVector as a string in compact form.
     *
     * <p> {@code "{R:%f, G_even:%f, G_odd:%f, B:%f}"}, where each {@code %f}
     * respectively represents one of the the four color channels. </p>
     *
     * @return compact string representation of {@link RggbChannelVector}
     */
    CARAPI_(String) ToShortString();

private:
    Float mRed;
    Float mGreenEven;
    Float mGreenOdd;
    Float mBlue;
};

} // namespace Params
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CRGGBCHANNELVECTOR_H__
