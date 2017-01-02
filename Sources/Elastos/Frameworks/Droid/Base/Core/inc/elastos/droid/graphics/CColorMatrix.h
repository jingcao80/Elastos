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

#ifndef __ELASTOS_DROID_GRAPHICS_CCOLORMATRIX_H__
#define __ELASTOS_DROID_GRAPHICS_CCOLORMATRIX_H__

#include "_Elastos_Droid_Graphics_CColorMatrix.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Graphics {

/**
 *  5x4 matrix for transforming the color+alpha components of a Bitmap.
 *  The matrix is stored in a single array, and its treated as follows:
 *  [ a, b, c, d, e,
 *    f, g, h, i, j,
 *    k, l, m, n, o,
 *    p, q, r, s, t ]
 *
 * When applied to a color [r, g, b, a], the resulting color is computed as
 * (after clamping)
 *   R' = a*R + b*G + c*B + d*A + e;
 *   G' = f*R + g*G + h*B + i*A + j;
 *   B' = k*R + l*G + m*B + n*A + o;
 *   A' = p*R + q*G + r*B + s*A + t;
 */
CarClass(CColorMatrix)
    , public Object
    , public IColorMatrix
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CColorMatrix();

    ~CColorMatrix();

    /**
     * Create a new colormatrix initialized to identity (as if reset() had
     * been called).
     */
    CARAPI constructor();

    /**
     * Create a new colormatrix initialized with the specified array of values.
     */
    CARAPI constructor(
        /* [in] */ const ArrayOf<Float>& src);

    /**
     * Create a new colormatrix initialized with the specified colormatrix.
     */
    CARAPI constructor(
        /* [in] */ IColorMatrix* src);

    /**
     * Return the array of floats representing this colormatrix.
     */
    CARAPI GetArray(
        /* [out, callee] */ ArrayOf<Float>** array);

    /**
     * Concat this colormatrix with the specified postmatrix. This is logically
     * the same as calling setConcat(postmatrix, this);
     */
    CARAPI PostConcat(
        /* [in] */ IColorMatrix* postmatrix);

    /**
     * Concat this colormatrix with the specified prematrix. This is logically
     * the same as calling setConcat(this, prematrix);
     */
    CARAPI PreConcat(
        /* [in] */ IColorMatrix* prematrix);

    /**
     * Set this colormatrix to identity:
     * [ 1 0 0 0 0   - red vector
     *   0 1 0 0 0   - green vector
     *   0 0 1 0 0   - blue vector
     *   0 0 0 1 0 ] - alpha vector
     */
    CARAPI Reset();

    /**
     * Assign the src colormatrix into this matrix, copying all of its values.
     */
    CARAPI Set(
        /* [in] */ IColorMatrix* src);

    /**
     * Assign the array of floats into this matrix, copying all of its values.
     */
    CARAPI Set(
        /* [in] */ ArrayOf<Float>* src);

    /**
     * Set this colormatrix to the concatenation of the two specified
     * colormatrices, such that the resulting colormatrix has the same effect
     * as applying matB and then applying matA. It is legal for either matA or
     * matB to be the same colormatrix as this.
     */
    CARAPI SetConcat(
        /* [in] */ IColorMatrix* matA,
        /* [in] */ IColorMatrix* matB);

    /**
     * Set the matrix to convert RGB to YUV
     */
    CARAPI SetRGB2YUV();

    /**
     * Set the rotation on a color axis by the specified values.
     * axis=0 correspond to a rotation around the RED color
     * axis=1 correspond to a rotation around the GREEN color
     * axis=2 correspond to a rotation around the BLUE color
     */
    CARAPI SetRotate(
        /* [in] */ Int32 axis,
        /* [in] */ Float degrees);

    /**
     * Set the matrix to affect the saturation of colors. A value of 0 maps the
     * color to gray-scale. 1 is identity.
     */
    CARAPI SetSaturation(
        /* [in] */ Float sat);

    /**
     * Set this colormatrix to scale by the specified values.
     */
    CARAPI SetScale(
        /* [in] */ Float rScale,
        /* [in] */ Float gScale,
        /* [in] */ Float bScale,
        /* [in] */ Float aScale);

    /**
     * Set the matrix to convert from YUV to RGB
     */
    CARAPI SetYUV2RGB();

private:
    AutoPtr<ArrayOf<Float> > mArray;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CCOLORMATRIX_H__
