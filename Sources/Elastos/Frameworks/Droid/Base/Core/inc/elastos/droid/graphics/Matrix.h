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

#ifndef __ELASTOS_DROID_GRAPHICS_MATRIX_H__
#define __ELASTOS_DROID_GRAPHICS_MATRIX_H__

#include "Elastos.Droid.Graphics.h"
#include <Elastos.CoreLibrary.h>
#include <elastos/core/Object.h>

using Elastos::Core::IStringBuilder;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Graphics {

extern const InterfaceID EIID_Matrix;

/**
 * The Matrix class holds a 3x3 matrix for transforming coordinates.
 * Matrix does not have a constructor, so it must be explicitly initialized
 * using either reset() - to construct an identity matrix, or one of the set..()
 * functions (e.g. setTranslate, setRotate, etc.).
 */
class Matrix
    : public Object
    , public IMatrix
{
public:
    CAR_INTERFACE_DECL()

    virtual ~Matrix();

    /**
     * Returns true if the matrix is identity.
     * This maybe faster than testing if (getType() == 0)
     */
    virtual CARAPI IsIdentity(
        /* [out] */ Boolean* isIdentity);

    /**
     * Gets whether this matrix is affine. An affine matrix preserves
     * straight lines and has no perspective.
     *
     * @return Whether the matrix is affine.
     */
    virtual CARAPI IsAffine(
        /* [out] */ Boolean* affine);

    /* Returns true if will map a rectangle to another rectangle. This can be
     * true if the matrix is identity, scale-only, or rotates a multiple of 90
     * degrees.
     */
    virtual CARAPI RectStaysRect(
        /* [out] */ Boolean* result);

    /**
     * (deep) copy the src matrix into this matrix. If src is null, reset this
     * matrix to the identity matrix.
     */
    virtual CARAPI Set(
        /* [in] */ IMatrix* src);

    /** Returns true iff obj is a Matrix and its values equal our values.
     */
    virtual CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* isEqual);

    virtual CARAPI GetHashCode(
        /* [out] */ Int32* count);

    /** Set the matrix to identity */
    virtual CARAPI Reset();

    /** Set the matrix to translate by (dx, dy). */
    virtual CARAPI SetTranslate(
        /* [in] */ Float dx,
        /* [in] */ Float dy);

    /**
     * Set the matrix to scale by sx and sy, with a pivot point at (px, py).
     * The pivot point is the coordinate that should remain unchanged by the
     * specified transformation.
     */
    virtual CARAPI SetScale(
        /* [in] */ Float sx,
        /* [in] */ Float sy,
        /* [in] */ Float px,
        /* [in] */ Float py);

    /** Set the matrix to scale by sx and sy. */
    virtual CARAPI SetScale(
        /* [in] */ Float sx,
        /* [in] */ Float sy);

    /**
     * Set the matrix to rotate by the specified number of degrees, with a pivot
     * point at (px, py). The pivot point is the coordinate that should remain
     * unchanged by the specified transformation.
     */
    virtual CARAPI SetRotate(
        /* [in] */ Float degrees,
        /* [in] */ Float px,
        /* [in] */ Float py);

    /**
     * Set the matrix to rotate about (0,0) by the specified number of degrees.
     */
    virtual CARAPI SetRotate(
        /* [in] */ Float degrees);

    /**
     * Set the matrix to rotate by the specified sine and cosine values, with a
     * pivot point at (px, py). The pivot point is the coordinate that should
     * remain unchanged by the specified transformation.
     */
    virtual CARAPI SetSinCos(
        /* [in] */ Float sinValue,
        /* [in] */ Float cosValue,
        /* [in] */ Float px,
        /* [in] */ Float py);

    /** Set the matrix to rotate by the specified sine and cosine values. */
    virtual CARAPI SetSinCos(
        /* [in] */ Float sinValue,
        /* [in] */ Float cosValue);

    /**
     * Set the matrix to skew by sx and sy, with a pivot point at (px, py).
     * The pivot point is the coordinate that should remain unchanged by the
     * specified transformation.
     */
    virtual CARAPI SetSkew(
        /* [in] */ Float kx,
        /* [in] */ Float ky,
        /* [in] */ Float px,
        /* [in] */ Float py);

    /** Set the matrix to skew by sx and sy. */
    virtual CARAPI SetSkew(
        /* [in] */ Float kx,
        /* [in] */ Float ky);

    /**
     * Set the matrix to the concatenation of the two specified matrices,
     * returning true if the the result can be represented. Either of the two
     * matrices may also be the target matrix. this = a * b
     */
    virtual CARAPI SetConcat(
        /* [in] */ IMatrix* a,
        /* [in] */ IMatrix* b,
        /* [out] */ Boolean* result);

    /**
     * Preconcats the matrix with the specified translation.
     * M' = M * T(dx, dy)
     */
    virtual CARAPI PreTranslate(
        /* [in] */ Float dx,
        /* [in] */ Float dy,
        /* [out] */ Boolean* result);

    /**
     * Preconcats the matrix with the specified scale.
     * M' = M * S(sx, sy, px, py)
     */
    virtual CARAPI PreScale(
        /* [in] */ Float sx,
        /* [in] */ Float sy,
        /* [in] */ Float px,
        /* [in] */ Float py,
        /* [out] */ Boolean* result);

    /**
     * Preconcats the matrix with the specified scale.
     * M' = M * S(sx, sy)
     */
    virtual CARAPI PreScale(
        /* [in] */ Float sx,
        /* [in] */ Float sy,
        /* [out] */ Boolean* result);

    /**
     * Preconcats the matrix with the specified rotation.
     * M' = M * R(degrees, px, py)
     */
    virtual CARAPI PreRotate(
        /* [in] */ Float degrees,
        /* [in] */ Float px,
        /* [in] */ Float py,
        /* [out] */ Boolean* result);

    /**
     * Preconcats the matrix with the specified rotation.
     * M' = M * R(degrees)
     */
    virtual CARAPI PreRotate(
        /* [in] */ Float degrees,
        /* [out] */ Boolean* result);

    /**
     * Preconcats the matrix with the specified skew.
     * M' = M * K(kx, ky, px, py)
     */
    virtual CARAPI PreSkew(
        /* [in] */ Float kx,
        /* [in] */ Float ky,
        /* [in] */ Float px,
        /* [in] */ Float py,
        /* [out] */ Boolean* result);

    /**
     * Preconcats the matrix with the specified skew.
     * M' = M * K(kx, ky)
     */
    virtual CARAPI PreSkew(
        /* [in] */ Float kx,
        /* [in] */ Float ky,
        /* [out] */ Boolean* result);

    /**
     * Preconcats the matrix with the specified matrix.
     * M' = M * other
     */
    virtual CARAPI PreConcat(
        /* [in] */ IMatrix* other,
        /* [out] */ Boolean* result);

    /**
     * Postconcats the matrix with the specified translation.
     * M' = T(dx, dy) * M
     */
    virtual CARAPI PostTranslate(
        /* [in] */ Float dx,
        /* [in] */ Float dy,
        /* [out] */ Boolean* result);

    /**
     * Postconcats the matrix with the specified scale.
     * M' = S(sx, sy, px, py) * M
     */
    virtual CARAPI PostScale(
        /* [in] */ Float sx,
        /* [in] */ Float sy,
        /* [in] */ Float px,
        /* [in] */ Float py,
        /* [out] */ Boolean* result);

    /**
     * Postconcats the matrix with the specified scale.
     * M' = S(sx, sy) * M
     */
    virtual CARAPI PostScale(
        /* [in] */ Float sx,
        /* [in] */ Float sy,
        /* [out] */ Boolean* result);

    /**
     * Postconcats the matrix with the specified rotation.
     * M' = R(degrees, px, py) * M
     */
    virtual CARAPI PostRotate(
        /* [in] */ Float degrees,
        /* [in] */ Float px,
        /* [in] */ Float py,
        /* [out] */ Boolean* result);

    /**
     * Postconcats the matrix with the specified rotation.
     * M' = R(degrees) * M
     */
    virtual CARAPI PostRotate(
        /* [in] */ Float degrees,
        /* [out] */ Boolean* result);

    /**
     * Postconcats the matrix with the specified skew.
     * M' = K(kx, ky, px, py) * M
     */
    virtual CARAPI PostSkew(
        /* [in] */ Float kx,
        /* [in] */ Float ky,
        /* [in] */ Float px,
        /* [in] */ Float py,
        /* [out] */ Boolean* result);

    /**
     * Postconcats the matrix with the specified skew.
     * M' = K(kx, ky) * M
     */
    virtual CARAPI PostSkew(
        /* [in] */ Float kx,
        /* [in] */ Float ky,
        /* [out] */ Boolean* result);

    /**
     * Postconcats the matrix with the specified matrix.
     * M' = other * M
     */
    virtual CARAPI PostConcat(
        /* [in] */ IMatrix* other,
        /* [out] */ Boolean* result);

    /**
     * Set the matrix to the scale and translate values that map the source
     * rectangle to the destination rectangle, returning true if the the result
     * can be represented.
     *
     * @param src the source rectangle to map from.
     * @param dst the destination rectangle to map to.
     * @param stf the ScaleToFit option
     * @return true if the matrix can be represented by the rectangle mapping.
     */
    virtual CARAPI SetRectToRect(
        /* [in] */ IRectF* src,
        /* [in] */ IRectF* dst,
        /* [in] */ MatrixScaleToFit stf,
        /* [out] */ Boolean* result);

    /**
     * Set the matrix such that the specified src points would map to the
     * specified dst points. The "points" are represented as an array of floats,
     * order [x0, y0, x1, y1, ...], where each "point" is 2 float values.
     *
     * @param src   The array of src [x,y] pairs (points)
     * @param srcIndex Index of the first pair of src values
     * @param dst   The array of dst [x,y] pairs (points)
     * @param dstIndex Index of the first pair of dst values
     * @param pointCount The number of pairs/points to be used. Must be [0..4]
     * @return true if the matrix was set to the specified transformation
     */
    virtual CARAPI SetPolyToPoly(
        /* [in] */ ArrayOf<Float>* src,
        /* [in] */ Int32 srcIndex,
        /* [out] */ ArrayOf<Float>* dst,
        /* [in] */ Int32 dstIndex,
        /* [in] */ Int32 pointCount,
        /* [out] */ Boolean* result);

    /**
     * If this matrix can be inverted, return true and if inverse is not null,
     * set inverse to be the inverse of this matrix. If this matrix cannot be
     * inverted, ignore inverse and return false.
     */
    virtual CARAPI Invert(
        /* [in] */ IMatrix* inverse,
        /* [out] */ Boolean* result);

    /**
    * Apply this matrix to the array of 2D points specified by src, and write
     * the transformed points into the array of points specified by dst. The
     * two arrays represent their "points" as pairs of floats [x, y].
     *
     * @param dst   The array of dst points (x,y pairs)
     * @param dstIndex The index of the first [x,y] pair of dst floats
     * @param src   The array of src points (x,y pairs)
     * @param srcIndex The index of the first [x,y] pair of src floats
     * @param pointCount The number of points (x,y pairs) to transform
     */
    virtual CARAPI MapPoints(
        /* [out] */ ArrayOf<Float>* dst,
        /* [in] */ Int32 dstIndex,
        /* [in] */ ArrayOf<Float>* src,
        /* [in] */ Int32 srcIndex,
        /* [in] */ Int32 pointCount);

    /**
    * Apply this matrix to the array of 2D vectors specified by src, and write
     * the transformed vectors into the array of vectors specified by dst. The
     * two arrays represent their "vectors" as pairs of floats [x, y].
     *
     * @param dst   The array of dst vectors (x,y pairs)
     * @param dstIndex The index of the first [x,y] pair of dst floats
     * @param src   The array of src vectors (x,y pairs)
     * @param srcIndex The index of the first [x,y] pair of src floats
     * @param vectorCount The number of vectors (x,y pairs) to transform
     */
    virtual CARAPI MapVectors(
        /* [out] */ ArrayOf<Float>* dst,
        /* [in] */ Int32 dstIndex,
        /* [in] */ ArrayOf<Float>* src,
        /* [in] */ Int32 srcIndex,
        /* [in] */ Int32 vectorCount);

    /**
     * Apply this matrix to the array of 2D points specified by src, and write
     * the transformed points into the array of points specified by dst. The
     * two arrays represent their "points" as pairs of floats [x, y].
     *
     * @param dst   The array of dst points (x,y pairs)
     * @param src   The array of src points (x,y pairs)
     */
    virtual CARAPI MapPoints(
        /* [out] */ ArrayOf<Float>* dst,
        /* [in] */ ArrayOf<Float>* src);

    /**
     * Apply this matrix to the array of 2D vectors specified by src, and write
     * the transformed vectors into the array of vectors specified by dst. The
     * two arrays represent their "vectors" as pairs of floats [x, y].
     *
     * @param dst   The array of dst vectors (x,y pairs)
     * @param src   The array of src vectors (x,y pairs)
     */
    virtual CARAPI MapVectors(
        /* [out] */ ArrayOf<Float>* dst,
        /* [in] */ ArrayOf<Float>* src);

    /**
     * Apply this matrix to the array of 2D points, and write the transformed
     * points back into the array
     *
     * @param pts The array [x0, y0, x1, y1, ...] of points to transform.
     */
    virtual CARAPI MapPoints(
        /* [in, out] */ ArrayOf<Float>* pts);

    /**
     * Apply this matrix to the array of 2D vectors, and write the transformed
     * vectors back into the array.
     * @param vecs The array [x0, y0, x1, y1, ...] of vectors to transform.
     */
    virtual CARAPI MapVectors(
        /* [in, out] */ ArrayOf<Float>* vecs);

    /**
     * Apply this matrix to the src rectangle, and write the transformed
     * rectangle into dst. This is accomplished by transforming the 4 corners of
     * src, and then setting dst to the bounds of those points.
     *
     * @param dst Where the transformed rectangle is written.
     * @param src The original rectangle to be transformed.
     * @return the result of calling rectStaysRect()
     */
    virtual CARAPI MapRect(
        /* [in] */ IRectF* dst,
        /* [in] */ IRectF* src,
        /* [out] */ Boolean* result);

    /**
     * Apply this matrix to the rectangle, and write the transformed rectangle
     * back into it. This is accomplished by transforming the 4 corners of rect,
     * and then setting it to the bounds of those points
     *
     * @param rect The rectangle to transform.
     * @return the result of calling rectStaysRect()
     */
    virtual CARAPI MapRect(
        /* [in] */ IRectF* rect,
        /* [out] */ Boolean* result);

    /**
     * Return the mean radius of a circle after it has been mapped by
     * this matrix. NOTE: in perspective this value assumes the circle
     * has its center at the origin.
     */
    virtual CARAPI MapRadius(
        /* [in] */ Float radius,
        /* [out] */ Float* result);

    /** Copy 9 values from the matrix into the array.
    */
    virtual CARAPI GetValues(
        /* [out] */ ArrayOf<Float>* values);

    /** Copy 9 values from the array into the matrix.
        Depending on the implementation of Matrix, these may be
        transformed into 16.16 integers in the Matrix, such that
        a subsequent call to getValues() will not yield exactly
        the same values.
    */
    virtual CARAPI SetValues(
        /* [in] */ ArrayOf<Float>* values);

    virtual CARAPI ToString(
        /* [out] */ String* str) ;

    /**
     * Print short string, to optimize dumping.
     * @hide
     */
    virtual CARAPI PrintShortString(
        /* [in] */ IPrintWriter* pw);

    virtual CARAPI ToShortString(
        /* [out] */ String* str);

    /**
     * @hide
     */
    virtual CARAPI ToShortString(
        /* [in] */ IStringBuilder* sb);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IMatrix* src);

private:
    // private helper to perform range checks on arrays of "points"
    static CARAPI CheckPointArrays(
        /* [in] */ ArrayOf<Float>* src,
        /* [in] */ Int32 srcIndex,
        /* [in] */ ArrayOf<Float>* dst,
        /* [in] */ Int32 dstIndex,
        /* [in] */ Int32 pointCount);

    static CARAPI_(Int64) NativeCreate(
        /* [in] */ Int64 nSrc);

    static CARAPI_(Boolean) NativeIsIdentity(
        /* [in] */ Int64 nObj);

    static CARAPI_(Boolean) NativeRectStaysRect(
        /* [in] */ Int64 nObj);

    static CARAPI_(Boolean) NativeIsAffine(
        /* [in] */ Int64 native_object);

    static CARAPI_(void) NativeReset(
        /* [in] */ Int64 nObj);

    static CARAPI_(void) NativeSet(
        /* [in] */ Int64 nObj,
        /* [in] */ Int64 nOther);

    static CARAPI_(void) NativeSetTranslate(
        /* [in] */ Int64 nObj,
        /* [in] */ Float dx,
        /* [in] */ Float dy);

    static CARAPI_(void) NativeSetScale(
        /* [in] */ Int64 nObj,
        /* [in] */ Float sx,
        /* [in] */ Float sy,
        /* [in] */ Float px,
        /* [in] */ Float py);

    static CARAPI_(void) NativeSetScale(
        /* [in] */ Int64 nObj,
        /* [in] */ Float sx,
        /* [in] */ Float sy);

    static CARAPI_(void) NativeSetRotate(
        /* [in] */ Int64 nObj,
        /* [in] */ Float degrees,
        /* [in] */ Float px,
        /* [in] */ Float py);

    static CARAPI_(void) NativeSetRotate(
        /* [in] */ Int64 nObj,
        /* [in] */ Float degrees);

    static CARAPI_(void) NativeSetSinCos(
        /* [in] */ Int64 nObj,
        /* [in] */ Float sinValue,
        /* [in] */ Float cosValue,
        /* [in] */ Float px,
        /* [in] */ Float py);

    static CARAPI_(void) NativeSetSinCos(
        /* [in] */ Int64 nObj,
        /* [in] */ Float sinValue,
        /* [in] */ Float cosValue);

    static CARAPI_(void) NativeSetSkew(
        /* [in] */ Int64 nObj,
        /* [in] */ Float kx,
        /* [in] */ Float ky,
        /* [in] */ Float px,
        /* [in] */ Float py);

    static CARAPI_(void) NativeSetSkew(
        /* [in] */ Int64 nObj,
        /* [in] */ Float kx,
        /* [in] */ Float ky);

    static CARAPI_(void) NativeSetConcat(
        /* [in] */ Int64 nObj,
        /* [in] */ Int64 nA,
        /* [in] */ Int64 nB);

    static CARAPI_(void) NativePreTranslate(
        /* [in] */ Int64 nObj,
        /* [in] */ Float dx,
        /* [in] */ Float dy);

    static CARAPI_(void) NativePreScale(
        /* [in] */ Int64 nObj,
        /* [in] */ Float sx,
        /* [in] */ Float sy,
        /* [in] */ Float px,
        /* [in] */ Float py);

    static CARAPI_(void) NativePreScale(
        /* [in] */ Int64 nObj,
        /* [in] */ Float sx,
        /* [in] */ Float sy);

    static CARAPI_(void) NativePreRotate(
        /* [in] */ Int64 nObj,
        /* [in] */ Float degrees,
        /* [in] */ Float px,
        /* [in] */ Float py);

    static CARAPI_(void) NativePreRotate(
        /* [in] */ Int64 nObj,
        /* [in] */ Float degrees);

    static CARAPI_(void) NativePreSkew(
        /* [in] */ Int64 nObj,
        /* [in] */ Float kx,
        /* [in] */ Float ky,
        /* [in] */ Float px,
        /* [in] */ Float py);

    static CARAPI_(void) NativePreSkew(
        /* [in] */ Int64 nObj,
        /* [in] */ Float kx,
        /* [in] */ Float ky);

    static CARAPI_(void) NativePreConcat(
        /* [in] */ Int64 nObj,
        /* [in] */ Int64 nOther);

    static CARAPI_(void) NativePostTranslate(
        /* [in] */ Int64 nObj,
        /* [in] */ Float dx,
        /* [in] */ Float dy);

    static CARAPI_(void) NativePostScale(
        /* [in] */ Int64 nObj,
        /* [in] */ Float sx,
        /* [in] */ Float sy,
        /* [in] */ Float px,
        /* [in] */ Float py);

    static CARAPI_(void) NativePostScale(
        /* [in] */ Int64 nObj,
        /* [in] */ Float sx,
        /* [in] */ Float sy);

    static CARAPI_(void) NativePostRotate(
        /* [in] */ Int64 nObj,
        /* [in] */ Float degrees,
        /* [in] */ Float px,
        /* [in] */ Float py);

    static CARAPI_(void) NativePostRotate(
        /* [in] */ Int64 nObj,
        /* [in] */ Float degrees);

    static CARAPI_(void) NativePostSkew(
        /* [in] */ Int64 nObj,
        /* [in] */ Float kx,
        /* [in] */ Float ky,
        /* [in] */ Float px,
        /* [in] */ Float py);

    static CARAPI_(void) NativePostSkew(
        /* [in] */ Int64 nObj,
        /* [in] */ Float kx,
        /* [in] */ Float ky);

    static CARAPI_(void) NativePostConcat(
        /* [in] */ Int64 nObj,
        /* [in] */ Int64 nOther);

    static CARAPI_(Boolean) NativeSetRectToRect(
        /* [in] */ Int64 nObj,
        /* [in] */ IRectF* src,
        /* [in] */ IRectF* dst,
        /* [in] */ Int32 stf);

    static CARAPI_(Boolean) NativeSetPolyToPoly(
        /* [in] */ Int64 nObj,
        /* [in] */ ArrayOf<Float>* src,
        /* [in] */ Int32 srcIndex,
        /* [out] */ ArrayOf<Float>* dst,
        /* [in] */ Int32 dstIndex,
        /* [in] */ Int32 ptCount);

    static CARAPI_(Boolean) NativeInvert(
        /* [in] */ Int64 nObj,
        /* [in] */ Int64 inverse);

    static CARAPI_(void) NativeMapPoints(
        /* [in] */ Int64 nObj,
        /* [out] */ ArrayOf<Float>* dst,
        /* [in] */ Int32 dstIndex,
        /* [in] */ ArrayOf<Float>* src,
        /* [in] */ Int32 srcIndex,
        /* [in] */ Int32 ptCount,
        /* [in] */ Boolean isPts);

    static CARAPI_(Boolean) NativeMapRect(
        /* [in] */ Int64 nObj,
        /* [in] */ IRectF* dst,
        /* [in] */ IRectF* src);

    static CARAPI_(Float) NativeMapRadius(
        /* [in] */ Int64 nObj,
        /* [in] */ Float radius);

    static CARAPI_(void) NativeGetValues(
        /* [in] */ Int64 nObj,
        /* [out] */ ArrayOf<Float>* values);

    static CARAPI_(void) NativeSetValues(
        /* [in] */ Int64 nObj,
        /* [in] */ ArrayOf<Float>* values);

    static CARAPI_(Boolean) NativeEquals(
        /* [in] */ Int64 nA,
        /* [in] */ Int64 nB);

    static CARAPI_(void) NativeFinalizer(
        /* [in] */ Int64 nObj);

public:
    Int64 mNativeMatrix;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_MATRIX_H__
