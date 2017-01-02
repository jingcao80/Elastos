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

#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/Matrix.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include <skia/core/SkMatrix.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Graphics {

static const String TAG("Matrix");

CAR_INTERFACE_IMPL(Matrix, Object, IMatrix);

Matrix::~Matrix()
{
    NativeFinalizer(mNativeMatrix);
}

ECode Matrix::constructor()
{
    mNativeMatrix = NativeCreate(0);
    return NOERROR;
}

ECode Matrix::constructor(
    /* [in] */ IMatrix* src)
{
    Int64 nSrc = src != NULL ? ((Matrix*)src)->mNativeMatrix : 0;
    mNativeMatrix = NativeCreate(nSrc);
    return NOERROR;
}

ECode Matrix::IsIdentity(
    /* [out] */ Boolean* isIdentity)
{
    VALIDATE_NOT_NULL(isIdentity)
    *isIdentity = NativeIsIdentity(mNativeMatrix);
    return NOERROR;
}

ECode Matrix::IsAffine(
    /* [out] */ Boolean* affine)
{
    VALIDATE_NOT_NULL(affine);
    *affine = NativeIsAffine(mNativeMatrix);
    return NOERROR;
}

ECode Matrix::RectStaysRect(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = NativeRectStaysRect(mNativeMatrix);
    return NOERROR;
}

/**
 * (deep) copy the src matrix into this matrix. If src is null, reset this
 * matrix to the identity matrix.
 */
ECode Matrix::Set(
    /* [in] */ IMatrix* src)
{
    if (src == NULL) {
        Reset();
    }
    else {
        NativeSet(mNativeMatrix, ((Matrix*)src)->mNativeMatrix);
    }
    return NOERROR;
}

/** Returns true iff obj is a Matrix and its values equal our values.
 */
ECode Matrix::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual);
    //if (obj == this) return true;     -- NaN value would mean matrix != itself
    if (IMatrix::Probe(obj) == NULL) {
        *isEqual = FALSE;
        return NOERROR;
    }
    *isEqual = NativeEquals(mNativeMatrix, ((Matrix*)IMatrix::Probe(obj))->mNativeMatrix);
    return NOERROR;
}

ECode Matrix::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash);
    // This should generate the hash code by performing some arithmetic operation on all
    // the matrix elements -- our equals() does an element-by-element comparison, and we
    // need to ensure that the hash code for two equal objects is the same.  We're not
    // really using this at the moment, so we take the easy way out.
    *hash = 44;
    return NOERROR;
}

/** Set the matrix to identity */
ECode Matrix::Reset()
{
    NativeReset(mNativeMatrix);
    return NOERROR;
}

/** Set the matrix to translate by (dx, dy). */
ECode Matrix::SetTranslate(
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    NativeSetTranslate(mNativeMatrix, dx, dy);
    return NOERROR;
}

/**
 * Set the matrix to scale by sx and sy, with a pivot point at (px, py).
 * The pivot point is the coordinate that should remain unchanged by the
 * specified transformation.
 */
ECode Matrix::SetScale(
    /* [in] */ Float sx,
    /* [in] */ Float sy,
    /* [in] */ Float px,
    /* [in] */ Float py)
{
    NativeSetScale(mNativeMatrix, sx, sy, px, py);
    return NOERROR;
}

/** Set the matrix to scale by sx and sy. */
ECode Matrix::SetScale(
    /* [in] */ Float sx,
    /* [in] */ Float sy)
{
    NativeSetScale(mNativeMatrix, sx, sy);
    return NOERROR;
}

/**
 * Set the matrix to rotate by the specified number of degrees, with a pivot
 * point at (px, py). The pivot point is the coordinate that should remain
 * unchanged by the specified transformation.
 */
ECode Matrix::SetRotate(
    /* [in] */ Float degrees,
    /* [in] */ Float px,
    /* [in] */ Float py)
{
    NativeSetRotate(mNativeMatrix, degrees, px, py);
    return NOERROR;
}

/**
 * Set the matrix to rotate about (0,0) by the specified number of degrees.
 */
ECode Matrix::SetRotate(
    /* [in] */ Float degrees)
{
    NativeSetRotate(mNativeMatrix, degrees);
    return NOERROR;
}

/**
 * Set the matrix to rotate by the specified sine and cosine values, with a
 * pivot point at (px, py). The pivot point is the coordinate that should
 * remain unchanged by the specified transformation.
 */
ECode Matrix::SetSinCos(
    /* [in] */ Float sinValue,
    /* [in] */ Float cosValue,
    /* [in] */ Float px,
    /* [in] */ Float py)
{
    NativeSetSinCos(mNativeMatrix, sinValue, cosValue, px, py);
    return NOERROR;
}

/** Set the matrix to rotate by the specified sine and cosine values. */
ECode Matrix::SetSinCos(
    /* [in] */ Float sinValue,
    /* [in] */ Float cosValue)
{
    NativeSetSinCos(mNativeMatrix, sinValue, cosValue);
    return NOERROR;
}

/**
 * Set the matrix to skew by sx and sy, with a pivot point at (px, py).
 * The pivot point is the coordinate that should remain unchanged by the
 * specified transformation.
 */
ECode Matrix::SetSkew(
    /* [in] */ Float kx,
    /* [in] */ Float ky,
    /* [in] */ Float px,
    /* [in] */ Float py)
{
    NativeSetSkew(mNativeMatrix, kx, ky, px, py);
    return NOERROR;
}

/** Set the matrix to skew by sx and sy. */
ECode Matrix::SetSkew(
    /* [in] */ Float kx,
    /* [in] */ Float ky)
{
    NativeSetSkew(mNativeMatrix, kx, ky);
    return NOERROR;
}

/**
 * Set the matrix to the concatenation of the two specified matrices,
 * returning true if the the result can be represented. Either of the two
 * matrices may also be the target matrix. this = a * b
 */
ECode Matrix::SetConcat(
    /* [in] */ IMatrix* a,
    /* [in] */ IMatrix* b,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    NativeSetConcat(mNativeMatrix,
            ((Matrix*)a)->mNativeMatrix,
            ((Matrix*)b)->mNativeMatrix);
    *result = TRUE;
    return NOERROR;
}

/**
 * Preconcats the matrix with the specified translation.
 * M' = M * T(dx, dy)
 */
ECode Matrix::PreTranslate(
    /* [in] */ Float dx,
    /* [in] */ Float dy,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    NativePreTranslate(mNativeMatrix, dx, dy);
    *result = TRUE;
    return NOERROR;
}

/**
 * Preconcats the matrix with the specified scale.
 * M' = M * S(sx, sy, px, py)
 */
ECode Matrix::PreScale(
    /* [in] */ Float sx,
    /* [in] */ Float sy,
    /* [in] */ Float px,
    /* [in] */ Float py,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    NativePreScale(mNativeMatrix, sx, sy, px, py);
    *result = TRUE;
    return NOERROR;
}

/**
 * Preconcats the matrix with the specified scale.
 * M' = M * S(sx, sy)
 */
ECode Matrix::PreScale(
    /* [in] */ Float sx,
    /* [in] */ Float sy,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    NativePreScale(mNativeMatrix, sx, sy);
    *result = TRUE;
    return NOERROR;
}

/**
 * Preconcats the matrix with the specified rotation.
 * M' = M * R(degrees, px, py)
 */
ECode Matrix::PreRotate(
    /* [in] */ Float degrees,
    /* [in] */ Float px,
    /* [in] */ Float py,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    NativePreRotate(mNativeMatrix, degrees, px, py);
    *result = TRUE;
    return NOERROR;
}

/**
 * Preconcats the matrix with the specified rotation.
 * M' = M * R(degrees)
 */
ECode Matrix::PreRotate(
    /* [in] */ Float degrees,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    NativePreRotate(mNativeMatrix, degrees);
    *result = TRUE;
    return NOERROR;
}

/**
 * Preconcats the matrix with the specified skew.
 * M' = M * K(kx, ky, px, py)
 */
ECode Matrix::PreSkew(
    /* [in] */ Float kx,
    /* [in] */ Float ky,
    /* [in] */ Float px,
    /* [in] */ Float py,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    NativePreSkew(mNativeMatrix, kx, ky, px, py);
    *result = TRUE;
    return NOERROR;
}

/**
 * Preconcats the matrix with the specified skew.
 * M' = M * K(kx, ky)
 */
ECode Matrix::PreSkew(
    /* [in] */ Float kx,
    /* [in] */ Float ky,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    NativePreSkew(mNativeMatrix, kx, ky);
    *result = TRUE;
    return NOERROR;
}

/**
 * Preconcats the matrix with the specified matrix.
 * M' = M * other
 */
ECode Matrix::PreConcat(
    /* [in] */ IMatrix* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    NativePreConcat(mNativeMatrix, ((Matrix*)other)->mNativeMatrix);
    *result = TRUE;
    return NOERROR;
}

/**
 * Postconcats the matrix with the specified translation.
 * M' = T(dx, dy) * M
 */
ECode Matrix::PostTranslate(
    /* [in] */ Float dx,
    /* [in] */ Float dy,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    NativePostTranslate(mNativeMatrix, dx, dy);
    *result = TRUE;
    return NOERROR;
}

/**
 * Postconcats the matrix with the specified scale.
 * M' = S(sx, sy, px, py) * M
 */
ECode Matrix::PostScale(
    /* [in] */ Float sx,
    /* [in] */ Float sy,
    /* [in] */ Float px,
    /* [in] */ Float py,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    NativePostScale(mNativeMatrix, sx, sy, px, py);
    *result = TRUE;
    return NOERROR;
}

/**
 * Postconcats the matrix with the specified scale.
 * M' = S(sx, sy) * M
 */
ECode Matrix::PostScale(
    /* [in] */ Float sx,
    /* [in] */ Float sy,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    NativePostScale(mNativeMatrix, sx, sy);
    *result = TRUE;
    return NOERROR;
}

/**
 * Postconcats the matrix with the specified rotation.
 * M' = R(degrees, px, py) * M
 */
ECode Matrix::PostRotate(
    /* [in] */ Float degrees,
    /* [in] */ Float px,
    /* [in] */ Float py,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    NativePostRotate(mNativeMatrix, degrees, px, py);
    *result = TRUE;
    return NOERROR;
}

/**
 * Postconcats the matrix with the specified rotation.
 * M' = R(degrees) * M
 */
ECode Matrix::PostRotate(
    /* [in] */ Float degrees,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    NativePostRotate(mNativeMatrix, degrees);
    *result = TRUE;
    return NOERROR;
}

/**
 * Postconcats the matrix with the specified skew.
 * M' = K(kx, ky, px, py) * M
 */
ECode Matrix::PostSkew(
    /* [in] */ Float kx,
    /* [in] */ Float ky,
    /* [in] */ Float px,
    /* [in] */ Float py,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    NativePostSkew(mNativeMatrix, kx, ky, px, py);
    *result = TRUE;
    return NOERROR;
}

/**
 * Postconcats the matrix with the specified skew.
 * M' = K(kx, ky) * M
 */
ECode Matrix::PostSkew(
    /* [in] */ Float kx,
    /* [in] */ Float ky,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    NativePostSkew(mNativeMatrix, kx, ky);
    *result = TRUE;
    return NOERROR;
}

/**
 * Postconcats the matrix with the specified matrix.
 * M' = other * M
 */
ECode Matrix::PostConcat(
    /* [in] */ IMatrix* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    NativePostConcat(mNativeMatrix, ((Matrix*)other)->mNativeMatrix);
    *result = TRUE;
    return NOERROR;
}

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
ECode Matrix::SetRectToRect(
    /* [in] */ IRectF* src,
    /* [in] */ IRectF* dst,
    /* [in] */ MatrixScaleToFit stf,
    /* [out] */ Boolean* result)
{
    if (dst == NULL || src == NULL) {
//        throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }
    *result = NativeSetRectToRect(mNativeMatrix, src, dst,
            (SkMatrix::ScaleToFit)stf);
    return NOERROR;
}

// private helper to perform range checks on arrays of "points"
ECode Matrix::CheckPointArrays(
    /* [in] */ ArrayOf<Float>* src,
    /* [in] */ Int32 srcIndex,
    /* [out] */ ArrayOf<Float>* dst,
    /* [in] */ Int32 dstIndex,
    /* [in] */ Int32 pointCount)
{
    if (dst == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    // check for too-small and too-big indices
    Int32 srcStop = srcIndex + (pointCount << 1);
    Int32 dstStop = dstIndex + (pointCount << 1);
    if ((pointCount | srcIndex | dstIndex | srcStop | dstStop) < 0 ||
            srcStop > src->GetLength() || dstStop > dst->GetLength()) {
//        throw new ArrayIndexOutOfBoundsException();
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

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
ECode Matrix::SetPolyToPoly(
    /* [in] */ ArrayOf<Float>* src,
    /* [in] */ Int32 srcIndex,
    /* [out] */ ArrayOf<Float>* dst,
    /* [in] */ Int32 dstIndex,
    /* [in] */ Int32 pointCount,
    /* [out] */ Boolean* result)
{
    if (pointCount > 4) {
//        throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(CheckPointArrays(src, srcIndex, dst, dstIndex, pointCount));
    *result = NativeSetPolyToPoly(mNativeMatrix, src, srcIndex,
                                  dst, dstIndex, pointCount);
    return NOERROR;
}

/**
 * If this matrix can be inverted, return true and if inverse is not null,
 * set inverse to be the inverse of this matrix. If this matrix cannot be
 * inverted, ignore inverse and return false.
 */
ECode Matrix::Invert(
    /* [in] */ IMatrix* inverse,
    /* [out] */ Boolean* result)
{
    *result = NativeInvert(mNativeMatrix, ((Matrix*)inverse)->mNativeMatrix);
    return NOERROR;
}

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
ECode Matrix::MapPoints(
    /* [out] */ ArrayOf<Float>* dst,
    /* [in] */ Int32 dstIndex,
    /* [in] */ ArrayOf<Float>* src,
    /* [in] */ Int32 srcIndex,
    /* [in] */ Int32 pointCount)
{
    FAIL_RETURN(CheckPointArrays(src, srcIndex, dst, dstIndex, pointCount));
    NativeMapPoints(mNativeMatrix, dst, dstIndex, src, srcIndex,
                    pointCount, TRUE);
    return NOERROR;
}

/**
* Apply this matrix to the array of 2D vectors specified by src, and write
 * the transformed vectors into the array of vectors specified by dst. The
 * two arrays represent their "vectors" as pairs of floats [x, y].
 *
 * Note: this method does not apply the translation associated with the matrix. Use
 * {@link Matrix#mapPoints(float[], int, float[], int, int)} if you want the translation
 * to be applied.
 *
 * @param dst   The array of dst vectors (x,y pairs)
 * @param dstIndex The index of the first [x,y] pair of dst floats
 * @param src   The array of src vectors (x,y pairs)
 * @param srcIndex The index of the first [x,y] pair of src floats
 * @param vectorCount The number of vectors (x,y pairs) to transform
 */
ECode Matrix::MapVectors(
    /* [out] */ ArrayOf<Float>* dst,
    /* [in] */ Int32 dstIndex,
    /* [in] */ ArrayOf<Float>* src,
    /* [in] */ Int32 srcIndex,
    /* [in] */ Int32 vectorCount)
{
    FAIL_RETURN(CheckPointArrays(src, srcIndex, dst, dstIndex, vectorCount));
    NativeMapPoints(mNativeMatrix, dst, dstIndex, src, srcIndex,
                    vectorCount, FALSE);
    return NOERROR;
}

/**
 * Apply this matrix to the array of 2D points specified by src, and write
 * the transformed points into the array of points specified by dst. The
 * two arrays represent their "points" as pairs of floats [x, y].
 *
 * @param dst   The array of dst points (x,y pairs)
 * @param src   The array of src points (x,y pairs)
 */
ECode Matrix::MapPoints(
    /* [out] */ ArrayOf<Float>* dst,
    /* [in] */ ArrayOf<Float>* src)
{
    if (dst == NULL) return E_NULL_POINTER_EXCEPTION;
    if (dst->GetLength() != src->GetLength()) {
//        throw new ArrayIndexOutOfBoundsException();
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return MapPoints(dst, 0, src, 0, dst->GetLength() >> 1);
}

/**
 * Apply this matrix to the array of 2D vectors specified by src, and write
 * the transformed vectors into the array of vectors specified by dst. The
 * two arrays represent their "vectors" as pairs of floats [x, y].
 *
 * Note: this method does not apply the translation associated with the matrix. Use
 * {@link Matrix#mapPoints(float[], float[])} if you want the translation to be applied.
 *
 * @param dst   The array of dst vectors (x,y pairs)
 * @param src   The array of src vectors (x,y pairs)
 */
ECode Matrix::MapVectors(
    /* [out] */ ArrayOf<Float>* dst,
    /* [in] */ ArrayOf<Float>* src)
{
    if (dst == NULL) return E_NULL_POINTER_EXCEPTION;
    if (dst->GetLength() != src->GetLength()) {
//        throw new ArrayIndexOutOfBoundsException();
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return MapVectors(dst, 0, src, 0, dst->GetLength() >> 1);
}

/**
 * Apply this matrix to the array of 2D points, and write the transformed
 * points back into the array
 *
 * @param pts The array [x0, y0, x1, y1, ...] of points to transform.
 */
ECode Matrix::MapPoints(
    /* [in, out] */ ArrayOf<Float>* pts)
{
    return MapPoints(pts, 0, pts, 0, pts->GetLength() >> 1);
}

/**
 * Apply this matrix to the array of 2D vectors, and write the transformed
 * vectors back into the array.
 *
 * Note: this method does not apply the translation associated with the matrix. Use
 * {@link Matrix#mapPoints(float[])} if you want the translation to be applied.
 *
 * @param vecs The array [x0, y0, x1, y1, ...] of vectors to transform.
 */
ECode Matrix::MapVectors(
    /* [in, out] */ ArrayOf<Float>* vecs)
{
    return MapVectors(vecs, 0, vecs, 0, vecs->GetLength() >> 1);
}

/**
 * Apply this matrix to the src rectangle, and write the transformed
 * rectangle into dst. This is accomplished by transforming the 4 corners of
 * src, and then setting dst to the bounds of those points.
 *
 * @param dst Where the transformed rectangle is written.
 * @param src The original rectangle to be transformed.
 * @return the result of calling rectStaysRect()
 */
ECode Matrix::MapRect(
    /* [in] */ IRectF* dst,
    /* [in] */ IRectF* src,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (dst == NULL || src == NULL) {
        Logger::E(TAG, "dst or src is NULL in MapRect.");
        return E_NULL_POINTER_EXCEPTION;
    }
    *result = NativeMapRect(mNativeMatrix, dst, src);
    return NOERROR;
}

/**
 * Apply this matrix to the rectangle, and write the transformed rectangle
 * back into it. This is accomplished by transforming the 4 corners of rect,
 * and then setting it to the bounds of those points
 *
 * @param rect The rectangle to transform.
 * @return the result of calling rectStaysRect()
 */
ECode Matrix::MapRect(
    /* [in] */ IRectF* rect,
    /* [out] */ Boolean* result)
{
    return MapRect(rect, rect, result);
}

/**
 * Return the mean radius of a circle after it has been mapped by
 * this matrix. NOTE: in perspective this value assumes the circle
 * has its center at the origin.
 */
ECode Matrix::MapRadius(
    /* [in] */ Float radius,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = NativeMapRadius(mNativeMatrix, radius);
    return NOERROR;
}

/** Copy 9 values from the matrix into the array.
*/
ECode Matrix::GetValues(
    /* [out] */ ArrayOf<Float>* values)
{
    if (values == NULL || values->GetLength() < 9) {
//        throw new ArrayIndexOutOfBoundsException();
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    NativeGetValues(mNativeMatrix, values);
    return NOERROR;
}

/** Copy 9 values from the array into the matrix.
    Depending on the implementation of Matrix, these may be
    transformed into 16.16 integers in the Matrix, such that
    a subsequent call to getValues() will not yield exactly
    the same values.
*/
ECode Matrix::SetValues(
    /* [in] */ ArrayOf<Float>* values)
{
    if (values->GetLength() < 9) {
//        throw new ArrayIndexOutOfBoundsException();
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    NativeSetValues(mNativeMatrix, values);
    return NOERROR;
}

ECode Matrix::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb(64);
    sb.Append("Matrix{");
    ToShortString((IStringBuilder*)&sb);
    sb.AppendChar('}');
    sb.ToString(str);
    return NOERROR;
}

ECode Matrix::ToShortString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb(64);
    ToShortString((IStringBuilder*)&sb);
    sb.ToString(str);
    return NOERROR;
}

/**
 * @hide
 */
ECode Matrix::ToShortString(
    /* [in] */ IStringBuilder* sb)
{
    AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(9);
    FAIL_RETURN(GetValues(values));
    sb->AppendChar('[');
    sb->Append((*values)[0]);
    sb->Append(String(", "));
    sb->Append((*values)[1]);
    sb->Append(String(", "));
    sb->Append((*values)[2]);
    sb->Append(String("]["));
    sb->Append((*values)[3]);
    sb->Append(String(", "));
    sb->Append((*values)[4]);
    sb->Append(String(", "));
    sb->Append((*values)[5]);
    sb->Append(String("]["));
    sb->Append((*values)[6]);
    sb->Append(String(", "));
    sb->Append((*values)[7]);
    sb->Append(String(", "));
    sb->Append((*values)[8]);
    sb->AppendChar(']');
    return NOERROR;
}

/**
 * Print short string, to optimize dumping.
 * @hide
 */
ECode Matrix::PrintShortString(
     /* [in] */ IPrintWriter* pw)
{
    AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(9);
    FAIL_RETURN(GetValues(values));
    pw->PrintChar('[');
    pw->Print((*values)[0]);
    pw->Print(String(", "));
    pw->Print((*values)[1]);
    pw->Print(String(", "));
    pw->Print((*values)[2]);
    pw->Print(String("]["));
    pw->Print((*values)[3]);
    pw->Print(String(", "));
    pw->Print((*values)[4]);
    pw->Print(String(", "));
    pw->Print((*values)[5]);
    pw->Print(String("]["));
    pw->Print((*values)[6]);
    pw->Print(String(", "));
    pw->Print((*values)[7]);
    pw->Print(String(", "));
    pw->Print((*values)[8]);
    pw->PrintChar(']');
    return NOERROR;
}

Int64 Matrix::NativeCreate(
    /* [in] */ Int64 nSrc)
{
    const SkMatrix* src = reinterpret_cast<SkMatrix*>(nSrc);
    SkMatrix* obj = new SkMatrix();
    if (src)
        *obj = *src;
    else
        obj->reset();
    return reinterpret_cast<Int64>(obj);
}

Boolean Matrix::NativeIsIdentity(
    /* [in] */ Int64 nObj)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    return obj->isIdentity() ? TRUE : FALSE;
}

Boolean Matrix::NativeRectStaysRect(
    /* [in] */ Int64 objHandle)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(objHandle);
    return obj->rectStaysRect() ? TRUE : FALSE;
}

void Matrix::NativeReset(
    /* [in] */ Int64 nObj)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    obj->reset();
}

void Matrix::NativeSet(
    /* [in] */ Int64 objHandle,
    /* [in] */ Int64 otherHandle)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(objHandle);
    SkMatrix* other = reinterpret_cast<SkMatrix*>(otherHandle);
    *obj = *other;
}

void Matrix::NativeSetTranslate(
    /* [in] */ Int64 nObj,
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    obj->setTranslate(dx, dy);
}

void Matrix::NativeSetScale(
    /* [in] */ Int64 nObj,
    /* [in] */ Float sx,
    /* [in] */ Float sy,
    /* [in] */ Float px,
    /* [in] */ Float py)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    obj->setScale(sx, sy, px, py);
}

void Matrix::NativeSetScale(
    /* [in] */ Int64 nObj,
    /* [in] */ Float sx,
    /* [in] */ Float sy)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    obj->setScale(sx, sy);
}

void Matrix::NativeSetRotate(
    /* [in] */ Int64 nObj,
    /* [in] */ Float degrees,
    /* [in] */ Float px,
    /* [in] */ Float py)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    obj->setRotate(degrees, px, py);
}

void Matrix::NativeSetRotate(
    /* [in] */ Int64 nObj,
    /* [in] */ Float degrees)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    obj->setRotate(degrees);
}

void Matrix::NativeSetSinCos(
    /* [in] */ Int64 nObj,
    /* [in] */ Float sinValue,
    /* [in] */ Float cosValue,
    /* [in] */ Float px,
    /* [in] */ Float py)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    obj->setSinCos(sinValue, cosValue, px, py);
}

void Matrix::NativeSetSinCos(
    /* [in] */ Int64 nObj,
    /* [in] */ Float sinValue,
    /* [in] */ Float cosValue)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    obj->setSinCos(sinValue, cosValue);
}

void Matrix::NativeSetSkew(
    /* [in] */ Int64 nObj,
    /* [in] */ Float kx,
    /* [in] */ Float ky,
    /* [in] */ Float px,
    /* [in] */ Float py)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    obj->setSkew(kx, ky, px, py);
}

void Matrix::NativeSetSkew(
    /* [in] */ Int64 nObj,
    /* [in] */ Float kx,
    /* [in] */ Float ky)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    obj->setSkew(kx, ky);
}

void Matrix::NativeSetConcat(
    /* [in] */ Int64 objHandle,
    /* [in] */ Int64 aHandle,
    /* [in] */ Int64 bHandle)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(objHandle);
    SkMatrix* a = reinterpret_cast<SkMatrix*>(aHandle);
    SkMatrix* b = reinterpret_cast<SkMatrix*>(bHandle);
    obj->setConcat(*a, *b);
}

void Matrix::NativePreTranslate(
    /* [in] */ Int64 nObj,
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    obj->preTranslate(dx, dy);
}

void Matrix::NativePreScale(
    /* [in] */ Int64 nObj,
    /* [in] */ Float sx,
    /* [in] */ Float sy,
    /* [in] */ Float px,
    /* [in] */ Float py)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    obj->preScale(sx, sy, px, py);
}

void Matrix::NativePreScale(
    /* [in] */ Int64 nObj,
    /* [in] */ Float sx,
    /* [in] */ Float sy)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    obj->preScale(sx, sy);
}

void Matrix::NativePreRotate(
    /* [in] */ Int64 nObj,
    /* [in] */ Float degrees,
    /* [in] */ Float px,
    /* [in] */ Float py)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    obj->preRotate(degrees, px, py);
}

void Matrix::NativePreRotate(
    /* [in] */ Int64 nObj,
    /* [in] */ Float degrees)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    obj->preRotate(degrees);
}

void Matrix::NativePreSkew(
    /* [in] */ Int64 nObj,
    /* [in] */ Float kx,
    /* [in] */ Float ky,
    /* [in] */ Float px,
    /* [in] */ Float py)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    obj->preSkew(kx, ky, px, py);
}

void Matrix::NativePreSkew(
    /* [in] */ Int64 nObj,
    /* [in] */ Float kx,
    /* [in] */ Float ky)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    obj->preSkew(kx, ky);
}

void Matrix::NativePreConcat(
    /* [in] */ Int64 objHandle,
    /* [in] */ Int64 otherHandle)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(objHandle);
    SkMatrix* other = reinterpret_cast<SkMatrix*>(otherHandle);
    obj->preConcat(*other);
}

void Matrix::NativePostTranslate(
    /* [in] */ Int64 nObj,
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    obj->postTranslate(dx, dy);
}

void Matrix::NativePostScale(
    /* [in] */ Int64 nObj,
    /* [in] */ Float sx,
    /* [in] */ Float sy,
    /* [in] */ Float px,
    /* [in] */ Float py)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    obj->postScale(sx, sy, px, py);
}

void Matrix::NativePostScale(
    /* [in] */ Int64 nObj,
    /* [in] */ Float sx,
    /* [in] */ Float sy)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    obj->postScale(sx, sy);
}

void Matrix::NativePostRotate(
    /* [in] */ Int64 nObj,
    /* [in] */ Float degrees,
    /* [in] */ Float px,
    /* [in] */ Float py)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    obj->postRotate(degrees, px, py);
}

void Matrix::NativePostRotate(
    /* [in] */ Int64 nObj,
    /* [in] */ Float degrees)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    obj->postRotate(degrees);
}

void Matrix::NativePostSkew(
    /* [in] */ Int64 nObj,
    /* [in] */ Float kx,
    /* [in] */ Float ky,
    /* [in] */ Float px,
    /* [in] */ Float py)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    obj->postSkew(kx, ky, px, py);
}

void Matrix::NativePostSkew(
    /* [in] */ Int64 nObj,
    /* [in] */ Float kx,
    /* [in] */ Float ky)
{
    SkMatrix* matrix = reinterpret_cast<SkMatrix*>(nObj);
    matrix->postSkew(kx, ky);
}

void Matrix::NativePostConcat(
    /* [in] */ Int64 nObj,
    /* [in] */ Int64 nOther)
{
    SkMatrix* matrix = reinterpret_cast<SkMatrix*>(nObj);
    SkMatrix* other = reinterpret_cast<SkMatrix*>(nOther);
    matrix->postConcat(*other);
}

Boolean Matrix::NativeSetRectToRect(
    /* [in] */ Int64 nObj,
    /* [in] */ IRectF* src,
    /* [in] */ IRectF* dst,
    /* [in] */ Int32 stfHandle)
{
    SkMatrix* matrix = reinterpret_cast<SkMatrix*>(nObj);
    SkMatrix::ScaleToFit stf = static_cast<SkMatrix::ScaleToFit>(stfHandle);
    SkRect src_;
    GraphicsNative::IRectF2SkRect(src, &src_);
    SkRect dst_;
    GraphicsNative::IRectF2SkRect(dst, &dst_);
    return matrix->setRectToRect(src_, dst_, stf) ? TRUE : FALSE;
}

Boolean Matrix::NativeSetPolyToPoly(
    /* [in] */ Int64 nObj,
    /* [in] */ ArrayOf<Float>* _src,
    /* [in] */ Int32 srcIndex,
    /* [out] */ ArrayOf<Float>* _dst,
    /* [in] */ Int32 dstIndex,
    /* [in] */ Int32 ptCount)
{
    SkASSERT(srcIndex >= 0);
    SkASSERT(dstIndex >= 0);
    SkASSERT(ptCount <= 4);

    SkASSERT(_src->GetLength() >= srcIndex + (ptCount << 1));
    SkASSERT(_dst->GetLength() >= dstIndex + (ptCount << 1));

    SkMatrix* matrix = reinterpret_cast<SkMatrix*>(nObj);
    SkASSERT(srcIndex >= 0);
    SkASSERT(dstIndex >= 0);
    SkASSERT((unsigned)ptCount <= 4);

    // AutoJavaFloatArray autoSrc(env, jsrc, srcIndex + (ptCount << 1), kRO_JNIAccess);
    // AutoJavaFloatArray autoDst(env, jdst, dstIndex + (ptCount << 1), kRW_JNIAccess);
    Float* src = _src->GetPayload() + srcIndex;
    Float* dst = _dst->GetPayload() + dstIndex;
    bool result;

#ifdef SK_SCALAR_IS_FLOAT
    result = matrix->setPolyToPoly((const SkPoint*)src, (const SkPoint*)dst,
                                 ptCount);
#else
    SkASSERT(FALSE);
#endif
    return result ? TRUE : FALSE;
}

Boolean Matrix::NativeInvert(
    /* [in] */ Int64 matrixHandle,
    /* [in] */ Int64 inverseHandle)
{
    SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixHandle);
    SkMatrix* inverse = reinterpret_cast<SkMatrix*>(inverseHandle);
    return matrix->invert(inverse);
}

void Matrix::NativeMapPoints(
    /* [in] */ Int64 matrixHandle,
    /* [out] */ ArrayOf<Float>* dst,
    /* [in] */ Int32 dstIndex,
    /* [in] */ ArrayOf<Float>* src,
    /* [in] */ Int32 srcIndex,
    /* [in] */ Int32 ptCount,
    /* [in] */ Boolean isPts)
{
    SkASSERT(ptCount >= 0);
    SkASSERT(src->GetLength() >= srcIndex + (ptCount << 1));
    SkASSERT(dst->GetLength() >= dstIndex + (ptCount << 1));

    SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixHandle);
    // AutoJavaFloatArray autoSrc(env, src, srcIndex + (ptCount << 1), kRO_JNIAccess);
    // AutoJavaFloatArray autoDst(env, dst, dstIndex + (ptCount << 1), kRW_JNIAccess);
    Float* srcArray = src->GetPayload() + srcIndex;
    Float* dstArray = dst->GetPayload() + dstIndex;
#ifdef SK_SCALAR_IS_FLOAT
    if (isPts)
        matrix->mapPoints((SkPoint*)dstArray, (const SkPoint*)srcArray,
                          ptCount);
    else
        matrix->mapVectors((SkVector*)dstArray, (const SkVector*)srcArray,
                           ptCount);
#else
    SkASSERT(FALSE);
#endif
}

Boolean Matrix::NativeMapRect(
    /* [in] */ Int64 matrixHandle,
    /* [in] */ IRectF* dst,
    /* [in] */ IRectF* src)
{
    SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixHandle);
    SkRect dst_, src_;
    GraphicsNative::IRectF2SkRect(src, &src_);
    Boolean rectStaysRect = matrix->mapRect(&dst_, src_);
    GraphicsNative::SkRect2IRectF(dst_, dst);
    return rectStaysRect;
}

Float Matrix::NativeMapRadius(
    /* [in] */ Int64 matrixHandle,
    /* [in] */ Float radius)
{
    SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixHandle);
    Float result;
    result = SkScalarToFloat(matrix->mapRadius(radius));
    return static_cast<Float>(result);
}

void Matrix::NativeGetValues(
    /* [in] */ Int64 matrixHandle,
    /* [out] */ ArrayOf<Float>* values)
{
    SkASSERT(values->GetLength() >= 9);
    SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixHandle);
    // AutoJavaFloatArray autoValues(env, values, 9, kRW_JNIAccess);
    float* dst = values->GetPayload();
#ifdef SK_SCALAR_IS_FLOAT
    for (int i = 0; i < 9; i++) {
        dst[i] = matrix->get(i);
    }
#else
    SkASSERT(FALSE);
#endif
}

void Matrix::NativeSetValues(
    /* [in] */ Int64 matrixHandle,
    /* [in] */ ArrayOf<Float>* values)
{
    SkASSERT(values->GetLength() >= 9);
    SkMatrix* matrix = reinterpret_cast<SkMatrix*>(matrixHandle);
    // AutoJavaFloatArray autoValues(env, values, 9, kRO_JNIAccess);
    const float* src = values->GetPayload();

#ifdef SK_SCALAR_IS_FLOAT
    for (int i = 0; i < 9; i++) {
        matrix->set(i, src[i]);
    }
#else
    SkASSERT(FALSE);
#endif
}

Boolean Matrix::NativeEquals(
    /* [in] */ Int64 aHandle,
    /* [in] */ Int64 bHandle)
{
    const SkMatrix* a = reinterpret_cast<SkMatrix*>(aHandle);
    const SkMatrix* b = reinterpret_cast<SkMatrix*>(bHandle);
    return *a == *b;
}

void Matrix::NativeFinalizer(
    /* [in] */ Int64 nObj)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(nObj);
    delete obj;
}

Boolean Matrix::NativeIsAffine(
    /* [in] */ Int64 objHandle)
{
    SkMatrix* obj = reinterpret_cast<SkMatrix*>(objHandle);
    return obj->asAffine(NULL) ? TRUE : FALSE;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
