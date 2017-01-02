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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CCOLORSPACETRANSFORM_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CCOLORSPACETRANSFORM_H__

#include "_Elastos_Droid_Hardware_Camera2_Params_CColorSpaceTransform.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IRational;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Params {

CarClass(CColorSpaceTransform)
    , public Object
    , public IColorSpaceTransform
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CColorSpaceTransform();

    virtual ~CColorSpaceTransform();

    CARAPI constructor();

    /**
     * Create a new immutable {@link ColorSpaceTransform} instance from a {@link Rational} array.
     *
     * <p>The elements must be stored in a row-major order.</p>
     *
     * @param elements An array of {@code 9} elements
     *
     * @throws IllegalArgumentException
     *            if the count of {@code elements} is not {@code 9}
     * @throws NullPointerException
     *            if {@code elements} or any sub-element is {@code null}
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<IRational*>* elements);

    /**
     * Create a new immutable {@link ColorSpaceTransform} instance from an {@code int} array.
     *
     * <p>The elements must be stored in a row-major order. Each rational is stored
     * contiguously as a {@code (numerator, denominator)} pair.</p>
     *
     * <p>In particular:<pre>{@code
     * int[] elements = new int[
     *     N11, D11, N12, D12, N13, D13,
     *     N21, D21, N22, D22, N23, D23,
     *     N31, D31, N32, D32, N33, D33
     * ];
     *
     * new ColorSpaceTransform(elements)}</pre>
     *
     * where {@code Nij} and {@code Dij} is the numerator and denominator for row {@code i} and
     * column {@code j}.</p>
     *
     * @param elements An array of {@code 18} elements
     *
     * @throws IllegalArgumentException
     *            if the count of {@code elements} is not {@code 18}
     * @throws NullPointerException
     *            if {@code elements} is {@code null}
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Int32>* _elements);

    /**
     * Get an element of this matrix by its row and column.
     *
     * <p>The rows must be within the range [0, 3),
     * and the column must be within the range [0, 3).</p>
     *
     * @return element (non-{@code null})
     *
     * @throws IllegalArgumentException if column or row was out of range
     */
    CARAPI GetElement(
        /* [in] */ Int32 column,
        /* [in] */ Int32 row,
        /* [out] */ IRational** outrat);

    /**
     * Copy the {@link Rational} elements in row-major order from this matrix into the destination.
     *
     * @param destination
     *          an array big enough to hold at least {@code 9} elements after the
     *          {@code offset}
     * @param offset
     *          a non-negative offset into the array
     * @throws NullPointerException
     *          If {@code destination} was {@code null}
     * @throws ArrayIndexOutOfBoundsException
     *          If there's not enough room to write the elements at the specified destination and
     *          offset.
     */
    CARAPI CopyElements(
        /* [in] */ ArrayOf<IRational*>* destination,
        /* [in] */ Int32 offset);

    /**
     * Copy the {@link Rational} elements in row-major order from this matrix into the destination.
     *
     * <p>Each element is stored as a contiguous rational packed as a
     * {@code (numerator, denominator)} pair of ints, identical to the
     * {@link ColorSpaceTransform#ColorSpaceTransform(int[]) constructor}.</p>
     *
     * @param destination
     *          an array big enough to hold at least {@code 18} elements after the
     *          {@code offset}
     * @param offset
     *          a non-negative offset into the array
     * @throws NullPointerException
     *          If {@code destination} was {@code null}
     * @throws ArrayIndexOutOfBoundsException
     *          If there's not enough room to write the elements at the specified destination and
     *          offset.
     *
     * @see ColorSpaceTransform#ColorSpaceTransform(int[])
     */
    CARAPI CopyElements(
        /* [in] */ ArrayOf<Int32>* destination,
        /* [in] */ Int32 offset);

    /**
     * Check if this {@link ColorSpaceTransform} is equal to another {@link ColorSpaceTransform}.
     *
     * <p>Two color space transforms are equal if and only if all of their elements are
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
     * Return the color space transform as a string representation.
     *
     *  <p> Example:
     * {@code "ColorSpaceTransform([1/1, 0/1, 0/1], [0/1, 1/1, 0/1], [0/1, 0/1, 1/1])"} is an
     * identity transform. Elements are printed in row major order. </p>
     *
     * @return string representation of color space transform
     */
    // @Override
    CARAPI ToString(
        /* [out] */ String* str);
private:
    /**
     * Return the color space transform as a compact string representation.
     *
     *  <p> Example:
     * {@code "([1/1, 0/1, 0/1], [0/1, 1/1, 0/1], [0/1, 0/1, 1/1])"} is an identity transform.
     * Elements are printed in row major order. </p>
     *
     * @return compact string representation of color space transform
     */
    CARAPI_(String) ToShortString();

private:
    /** The number of rows in this matrix. */
    static const Int32 ROWS;

    /** The number of columns in this matrix. */
    static const Int32 COLUMNS;

    /** The number of total Rational elements in this matrix. */
    static const Int32 COUNT;

    /** Number of int elements in a rational. */
    static const Int32 RATIONAL_SIZE;

    /** Numerator offset inside a rational (pair). */
    static const Int32 OFFSET_NUMERATOR;

    /** Denominator offset inside a rational (pair). */
    static const Int32 OFFSET_DENOMINATOR;

    /** Number of int elements in this matrix. */
    static const Int32 COUNT_INT;

    AutoPtr<ArrayOf<Int32> > mElements;
};

} // namespace Params
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CCOLORSPACETRANSFORM_H__
