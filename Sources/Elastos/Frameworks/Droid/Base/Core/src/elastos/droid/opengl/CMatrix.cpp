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

#include "elastos/droid/opengl/CMatrix.h"

#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>

using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Opengl {

#define I(_i, _j) ((_j)+ 4*(_i))

AutoPtr<ArrayOf<Float> > CMatrix::sTemp= ArrayOf<Float>::Alloc(32);
Object CMatrix::sLock;

static
void multiplyMM(Float* r, const Float* lhs, const Float* rhs)
{
    for (int i=0 ; i<4 ; i++) {
        register const Float rhs_i0 = rhs[ I(i,0) ];
        register Float ri0 = lhs[ I(0,0) ] * rhs_i0;
        register Float ri1 = lhs[ I(0,1) ] * rhs_i0;
        register Float ri2 = lhs[ I(0,2) ] * rhs_i0;
        register Float ri3 = lhs[ I(0,3) ] * rhs_i0;
        for (int j=1 ; j<4 ; j++) {
            register const Float rhs_ij = rhs[ I(i,j) ];
            ri0 += lhs[ I(j,0) ] * rhs_ij;
            ri1 += lhs[ I(j,1) ] * rhs_ij;
            ri2 += lhs[ I(j,2) ] * rhs_ij;
            ri3 += lhs[ I(j,3) ] * rhs_ij;
        }
        r[ I(i,0) ] = ri0;
        r[ I(i,1) ] = ri1;
        r[ I(i,2) ] = ri2;
        r[ I(i,3) ] = ri3;
    }
}


ECode CMatrix::MultiplyMM(
    /* [in] */ ArrayOf<Float>* result_ref,
    /* [in] */ Int32 resultOffset,
    /* [in] */ ArrayOf<Float>* lhs_ref,
    /* [in] */ Int32 lhsOffset,
    /* [in] */ ArrayOf<Float>* rhs_ref,
    /* [in] */ Int32 rhsOffset)
{
    FloatArrayHelper resultMat(result_ref, resultOffset, 16);
    FloatArrayHelper lhs(lhs_ref, lhsOffset, 16);
    FloatArrayHelper rhs(rhs_ref, rhsOffset, 16);

    Boolean rsChk, lhChk, rhChk;
    resultMat.Check(&rsChk);
    lhs.Check(&lhChk);
    rhs.Check(&rhChk);
    Boolean checkOK = rsChk && lhChk && rhChk;;

    if ( !checkOK ) {
        return NOERROR;
    }

    resultMat.Bind();
    lhs.Bind();
    rhs.Bind();

    multiplyMM(resultMat.mData, lhs.mData, rhs.mData);

    return NOERROR;
}

static inline
void mx4transform(Float x, Float y, Float z, Float w, const Float* pM, Float* pDest) {
    pDest[0] = pM[0 + 4 * 0] * x + pM[0 + 4 * 1] * y + pM[0 + 4 * 2] * z + pM[0 + 4 * 3] * w;
    pDest[1] = pM[1 + 4 * 0] * x + pM[1 + 4 * 1] * y + pM[1 + 4 * 2] * z + pM[1 + 4 * 3] * w;
    pDest[2] = pM[2 + 4 * 0] * x + pM[2 + 4 * 1] * y + pM[2 + 4 * 2] * z + pM[2 + 4 * 3] * w;
    pDest[3] = pM[3 + 4 * 0] * x + pM[3 + 4 * 1] * y + pM[3 + 4 * 2] * z + pM[3 + 4 * 3] * w;
}

static
void multiplyMV(Float* r, const Float* lhs, const Float* rhs)
{
    mx4transform(rhs[0], rhs[1], rhs[2], rhs[3], lhs, r);
}

CAR_INTERFACE_IMPL(CMatrix, Singleton, IMatrix)

CAR_SINGLETON_IMPL(CMatrix)

ECode CMatrix::MultiplyMV(
    /* [in] */ ArrayOf<Float>* result_ref,
    /* [in] */ Int32 resultOffset,
    /* [in] */ ArrayOf<Float>* lhs_ref,
    /* [in] */ Int32 lhsOffset,
    /* [in] */ ArrayOf<Float>* rhs_ref,
    /* [in] */ Int32 rhsOffset)
{
    FloatArrayHelper resultV(result_ref, resultOffset, 4);
    FloatArrayHelper lhs(lhs_ref, lhsOffset, 16);
    FloatArrayHelper rhs(rhs_ref, rhsOffset, 4);

    Boolean rsChk, lhChk, rhChk;
    resultV.Check(&rsChk);
    lhs.Check(&lhChk);
    rhs.Check(&rhChk);
    Boolean checkOK = rsChk && lhChk && rhChk;;

    if ( !checkOK ) {
        return NOERROR;
    }

    resultV.Bind();
    lhs.Bind();
    rhs.Bind();

    multiplyMV(resultV.mData, lhs.mData, rhs.mData);
    return NOERROR;
}

ECode CMatrix::TransposeM(
    /* [in] */ ArrayOf<Float>* mTrans,
    /* [in] */ Int32 mTransOffset,
    /* [in] */ ArrayOf<Float>* m,
    /* [in] */ Int32 mOffset)
{
    for (int i = 0; i < 4; i++) {
        int mBase = i * 4 + mOffset;
        (*mTrans)[i + mTransOffset] = (*m)[mBase];
        (*mTrans)[i + 4 + mTransOffset] = (*m)[mBase + 1];
        (*mTrans)[i + 8 + mTransOffset] = (*m)[mBase + 2];
        (*mTrans)[i + 12 + mTransOffset] = (*m)[mBase + 3];
    }
    return NOERROR;
}

ECode CMatrix::InvertM(
    /* [in] */ ArrayOf<Float>* mInv,
    /* [in] */ Int32 mInvOffset,
    /* [in] */ ArrayOf<Float>* m,
    /* [in] */ Int32 mOffset,
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst)

    // Invert a 4 x 4 matrix using Cramer's Rule

    // transpose matrix
    Float src0  = (*m)[mOffset +  0];
    Float src4  = (*m)[mOffset +  1];
    Float src8  = (*m)[mOffset +  2];
    Float src12 = (*m)[mOffset +  3];

    Float src1  = (*m)[mOffset +  4];
    Float src5  = (*m)[mOffset +  5];
    Float src9  = (*m)[mOffset +  6];
    Float src13 = (*m)[mOffset +  7];

    Float src2  = (*m)[mOffset +  8];
    Float src6  = (*m)[mOffset +  9];
    Float src10 = (*m)[mOffset + 10];
    Float src14 = (*m)[mOffset + 11];

    Float src3  = (*m)[mOffset + 12];
    Float src7  = (*m)[mOffset + 13];
    Float src11 = (*m)[mOffset + 14];
    Float src15 = (*m)[mOffset + 15];

    // calculate pairs for first 8 elements (cofactors)
    Float atmp0  = src10 * src15;
    Float atmp1  = src11 * src14;
    Float atmp2  = src9  * src15;
    Float atmp3  = src11 * src13;
    Float atmp4  = src9  * src14;
    Float atmp5  = src10 * src13;
    Float atmp6  = src8  * src15;
    Float atmp7  = src11 * src12;
    Float atmp8  = src8  * src14;
    Float atmp9  = src10 * src12;
    Float atmp10 = src8  * src13;
    Float atmp11 = src9  * src12;

    // calculate first 8 elements (cofactors)
    Float dst0  = (atmp0 * src5 + atmp3 * src6 + atmp4  * src7)
                      - (atmp1 * src5 + atmp2 * src6 + atmp5  * src7);
    Float dst1  = (atmp1 * src4 + atmp6 * src6 + atmp9  * src7)
                      - (atmp0 * src4 + atmp7 * src6 + atmp8  * src7);
    Float dst2  = (atmp2 * src4 + atmp7 * src5 + atmp10 * src7)
                      - (atmp3 * src4 + atmp6 * src5 + atmp11 * src7);
    Float dst3  = (atmp5 * src4 + atmp8 * src5 + atmp11 * src6)
                      - (atmp4 * src4 + atmp9 * src5 + atmp10 * src6);
    Float dst4  = (atmp1 * src1 + atmp2 * src2 + atmp5  * src3)
                      - (atmp0 * src1 + atmp3 * src2 + atmp4  * src3);
    Float dst5  = (atmp0 * src0 + atmp7 * src2 + atmp8  * src3)
                      - (atmp1 * src0 + atmp6 * src2 + atmp9  * src3);
    Float dst6  = (atmp3 * src0 + atmp6 * src1 + atmp11 * src3)
                      - (atmp2 * src0 + atmp7 * src1 + atmp10 * src3);
    Float dst7  = (atmp4 * src0 + atmp9 * src1 + atmp10 * src2)
                      - (atmp5 * src0 + atmp8 * src1 + atmp11 * src2);

    // calculate pairs for second 8 elements (cofactors)
    Float btmp0  = src2 * src7;
    Float btmp1  = src3 * src6;
    Float btmp2  = src1 * src7;
    Float btmp3  = src3 * src5;
    Float btmp4  = src1 * src6;
    Float btmp5  = src2 * src5;
    Float btmp6  = src0 * src7;
    Float btmp7  = src3 * src4;
    Float btmp8  = src0 * src6;
    Float btmp9  = src2 * src4;
    Float btmp10 = src0 * src5;
    Float btmp11 = src1 * src4;

    // calculate second 8 elements (cofactors)
    Float dst8  = (btmp0  * src13 + btmp3  * src14 + btmp4  * src15)
                      - (btmp1  * src13 + btmp2  * src14 + btmp5  * src15);
    Float dst9  = (btmp1  * src12 + btmp6  * src14 + btmp9  * src15)
                      - (btmp0  * src12 + btmp7  * src14 + btmp8  * src15);
    Float dst10 = (btmp2  * src12 + btmp7  * src13 + btmp10 * src15)
                      - (btmp3  * src12 + btmp6  * src13 + btmp11 * src15);
    Float dst11 = (btmp5  * src12 + btmp8  * src13 + btmp11 * src14)
                      - (btmp4  * src12 + btmp9  * src13 + btmp10 * src14);
    Float dst12 = (btmp2  * src10 + btmp5  * src11 + btmp1  * src9 )
                      - (btmp4  * src11 + btmp0  * src9  + btmp3  * src10);
    Float dst13 = (btmp8  * src11 + btmp0  * src8  + btmp7  * src10)
                      - (btmp6  * src10 + btmp9  * src11 + btmp1  * src8 );
    Float dst14 = (btmp6  * src9  + btmp11 * src11 + btmp3  * src8 )
                      - (btmp10 * src11 + btmp2  * src8  + btmp7  * src9 );
    Float dst15 = (btmp10 * src10 + btmp4  * src8  + btmp9  * src9 )
                      - (btmp8  * src9  + btmp11 * src10 + btmp5  * src8 );

    // calculate determinant
    Float det =
            src0 * dst0 + src1 * dst1 + src2 * dst2 + src3 * dst3;

    if (det == 0.0f) {
        *rst = FALSE;
        return NOERROR;
    }

    // calculate matrix inverse
    Float invdet = 1.0f / det;
    (*mInv)[     mInvOffset] = dst0  * invdet;
    (*mInv)[ 1 + mInvOffset] = dst1  * invdet;
    (*mInv)[ 2 + mInvOffset] = dst2  * invdet;
    (*mInv)[ 3 + mInvOffset] = dst3  * invdet;

    (*mInv)[ 4 + mInvOffset] = dst4  * invdet;
    (*mInv)[ 5 + mInvOffset] = dst5  * invdet;
    (*mInv)[ 6 + mInvOffset] = dst6  * invdet;
    (*mInv)[ 7 + mInvOffset] = dst7  * invdet;

    (*mInv)[ 8 + mInvOffset] = dst8  * invdet;
    (*mInv)[ 9 + mInvOffset] = dst9  * invdet;
    (*mInv)[10 + mInvOffset] = dst10 * invdet;
    (*mInv)[11 + mInvOffset] = dst11 * invdet;

    (*mInv)[12 + mInvOffset] = dst12 * invdet;
    (*mInv)[13 + mInvOffset] = dst13 * invdet;
    (*mInv)[14 + mInvOffset] = dst14 * invdet;
    (*mInv)[15 + mInvOffset] = dst15 * invdet;

    *rst = TRUE;
    return NOERROR;
}

ECode CMatrix::OrthoM(
    /* [in] */ ArrayOf<Float>* m,
    /* [in] */ Int32 mOffset,
    /* [in] */ Float left,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float top,
    /* [in] */ Float near,
    /* [in] */ Float far)
{
    if (left == right) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (bottom == top) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (near == far) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Float r_width  = 1.0f / (right - left);
    Float r_height = 1.0f / (top - bottom);
    Float r_depth  = 1.0f / (far - near);
    Float x =  2.0f * (r_width);
    Float y =  2.0f * (r_height);
    Float z = -2.0f * (r_depth);
    Float tx = -(right + left) * r_width;
    Float ty = -(top + bottom) * r_height;
    Float tz = -(far + near) * r_depth;
    (*m)[mOffset + 0] = x;
    (*m)[mOffset + 5] = y;
    (*m)[mOffset +10] = z;
    (*m)[mOffset +12] = tx;
    (*m)[mOffset +13] = ty;
    (*m)[mOffset +14] = tz;
    (*m)[mOffset +15] = 1.0f;
    (*m)[mOffset + 1] = 0.0f;
    (*m)[mOffset + 2] = 0.0f;
    (*m)[mOffset + 3] = 0.0f;
    (*m)[mOffset + 4] = 0.0f;
    (*m)[mOffset + 6] = 0.0f;
    (*m)[mOffset + 7] = 0.0f;
    (*m)[mOffset + 8] = 0.0f;
    (*m)[mOffset + 9] = 0.0f;
    (*m)[mOffset + 11] = 0.0f;
    return NOERROR;
}

ECode CMatrix::FrustumM(
    /* [in] */ ArrayOf<Float>* m,
    /* [in] */ Int32 offset,
    /* [in] */ Float left,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float top,
    /* [in] */ Float near,
    /* [in] */ Float far)
{
    if (left == right) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (top == bottom) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (near == far) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (near <= 0.0f) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (far <= 0.0f) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Float r_width  = 1.0f / (right - left);
    Float r_height = 1.0f / (top - bottom);
    Float r_depth  = 1.0f / (near - far);
    Float x = 2.0f * (near * r_width);
    Float y = 2.0f * (near * r_height);
    Float A = (right + left) * r_width;
    Float B = (top + bottom) * r_height;
    Float C = (far + near) * r_depth;
    Float D = 2.0f * (far * near * r_depth);
    (*m)[offset + 0] = x;
    (*m)[offset + 5] = y;
    (*m)[offset + 8] = A;
    (*m)[offset +  9] = B;
    (*m)[offset + 10] = C;
    (*m)[offset + 14] = D;
    (*m)[offset + 11] = -1.0f;
    (*m)[offset +  1] = 0.0f;
    (*m)[offset +  2] = 0.0f;
    (*m)[offset +  3] = 0.0f;
    (*m)[offset +  4] = 0.0f;
    (*m)[offset +  6] = 0.0f;
    (*m)[offset +  7] = 0.0f;
    (*m)[offset + 12] = 0.0f;
    (*m)[offset + 13] = 0.0f;
    (*m)[offset + 15] = 0.0f;
    return NOERROR;
}

ECode CMatrix::PerspectiveM(
    /* [in] */ ArrayOf<Float>* m,
    /* [in] */ Int32 offset,
    /* [in] */ Float fovy,
    /* [in] */ Float aspect,
    /* [in] */ Float zNear,
    /* [in] */ Float zFar)
{
    Float f = 1.0f / (Float) Elastos::Core::Math::Tan(fovy * (Elastos::Core::Math::PI / 360.0));
    Float rangeReciprocal = 1.0f / (zNear - zFar);

    (*m)[offset + 0] = f / aspect;
    (*m)[offset + 1] = 0.0f;
    (*m)[offset + 2] = 0.0f;
    (*m)[offset + 3] = 0.0f;

    (*m)[offset + 4] = 0.0f;
    (*m)[offset + 5] = f;
    (*m)[offset + 6] = 0.0f;
    (*m)[offset + 7] = 0.0f;

    (*m)[offset + 8] = 0.0f;
    (*m)[offset + 9] = 0.0f;
    (*m)[offset + 10] = (zFar + zNear) * rangeReciprocal;
    (*m)[offset + 11] = -1.0f;

    (*m)[offset + 12] = 0.0f;
    (*m)[offset + 13] = 0.0f;
    (*m)[offset + 14] = 2.0f * zFar * zNear * rangeReciprocal;
    (*m)[offset + 15] = 0.0f;
    return NOERROR;
}

Float CMatrix::Length(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z)
{
    return (Float) Elastos::Core::Math::Sqrt(x * x + y * y + z * z);
}

ECode CMatrix::Length(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z,
    /* [out] */ Float* len)
{
    VALIDATE_NOT_NULL(len)

    *len = (Float) Elastos::Core::Math::Sqrt(x * x + y * y + z * z);
    return NOERROR;
}

ECode CMatrix::SetIdentityM(
    /* [in] */ ArrayOf<Float>* sm,
    /* [in] */ Int32 smOffset)
{
    for (Int32 i=0 ; i<16 ; i++) {
        (*sm)[smOffset + i] = 0;
    }
    for(Int32 i = 0; i < 16; i += 5) {
        (*sm)[smOffset + i] = 1.0f;
    }
    return NOERROR;
}

ECode CMatrix::ScaleM(
    /* [in] */ ArrayOf<Float>* sm,
    /* [in] */ Int32 smOffset,
    /* [in] */ ArrayOf<Float>* m,
    /* [in] */ Int32 mOffset,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z)
{
    for (int i=0 ; i<4 ; i++) {
        int smi = smOffset + i;
        int mi = mOffset + i;
        (*sm)[     smi] = (*m)[     mi] * x;
        (*sm)[ 4 + smi] = (*m)[ 4 + mi] * y;
        (*sm)[ 8 + smi] = (*m)[ 8 + mi] * z;
        (*sm)[12 + smi] = (*m)[12 + mi];
    }
    return NOERROR;
}

ECode CMatrix::ScaleM(
    /* [in] */ ArrayOf<Float>* m,
    /* [in] */ Int32 mOffset,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z)
{
    for (int i=0 ; i<4 ; i++) {
        int mi = mOffset + i;
        (*m)[     mi] *= x;
        (*m)[ 4 + mi] *= y;
        (*m)[ 8 + mi] *= z;
    }
    return NOERROR;
}

ECode CMatrix::TranslateM(
    /* [in] */ ArrayOf<Float>* tm,
    /* [in] */ Int32 tmOffset,
    /* [in] */ ArrayOf<Float>* m,
    /* [in] */ Int32 mOffset,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z)
{
    for (int i=0 ; i<12 ; i++) {
        (*tm)[tmOffset + i] = (*m)[mOffset + i];
    }
    for (int i=0 ; i<4 ; i++) {
        int tmi = tmOffset + i;
        int mi = mOffset + i;
        (*tm)[12 + tmi] = (*m)[mi] * x + (*m)[4 + mi] * y + (*m)[8 + mi] * z +
            (*m)[12 + mi];
    }
    return NOERROR;
}

ECode CMatrix::TranslateM(
    /* [in] */ ArrayOf<Float>* m,
    /* [in] */ Int32 mOffset,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z)
{
    for (int i=0 ; i<4 ; i++) {
        int mi = mOffset + i;
        (*m)[12 + mi] += (*m)[mi] * x + (*m)[4 + mi] * y + (*m)[8 + mi] * z;
    }
    return NOERROR;
}

ECode CMatrix::RotateM(
    /* [in] */ ArrayOf<Float>* rm,
    /* [in] */ Int32 rmOffset,
    /* [in] */ ArrayOf<Float>* m,
    /* [in] */ Int32 mOffset,
    /* [in] */ Float a,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z)
{
    {
        AutoLock lock(sLock);
        SetRotateM(sTemp, 0, a, x, y, z);
        MultiplyMM(rm, rmOffset, m, mOffset, sTemp, 0);
    }
    return NOERROR;
}

ECode CMatrix::RotateM(
    /* [in] */ ArrayOf<Float>* m,
    /* [in] */ Int32 mOffset,
    /* [in] */ Float a,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z)
{
    {
        AutoLock lock(sLock);
        SetRotateM(sTemp, 0, a, x, y, z);
        MultiplyMM(sTemp, 16, m, mOffset, sTemp, 0);
        m->Copy(mOffset, sTemp, 16, 16);
    }
    return NOERROR;
}

ECode CMatrix::SetRotateM(
    /* [in] */ ArrayOf<Float>* rm,
    /* [in] */ Int32 rmOffset,
    /* [in] */ Float a,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z)
{
    (*rm)[rmOffset + 3] = 0;
    (*rm)[rmOffset + 7] = 0;
    (*rm)[rmOffset + 11]= 0;
    (*rm)[rmOffset + 12]= 0;
    (*rm)[rmOffset + 13]= 0;
    (*rm)[rmOffset + 14]= 0;
    (*rm)[rmOffset + 15]= 1;
    a *= (Float) (Elastos::Core::Math::PI / 180.0f);
    Float s = (Float) Elastos::Core::Math::Sin(a);
    Float c = (Float) Elastos::Core::Math::Cos(a);
    if (1.0f == x && 0.0f == y && 0.0f == z) {
        (*rm)[rmOffset + 5] = c;   (*rm)[rmOffset + 10]= c;
        (*rm)[rmOffset + 6] = s;   (*rm)[rmOffset + 9] = -s;
        (*rm)[rmOffset + 1] = 0;   (*rm)[rmOffset + 2] = 0;
        (*rm)[rmOffset + 4] = 0;   (*rm)[rmOffset + 8] = 0;
        (*rm)[rmOffset + 0] = 1;
    } else if (0.0f == x && 1.0f == y && 0.0f == z) {
        (*rm)[rmOffset + 0] = c;   (*rm)[rmOffset + 10]= c;
        (*rm)[rmOffset + 8] = s;   (*rm)[rmOffset + 2] = -s;
        (*rm)[rmOffset + 1] = 0;   (*rm)[rmOffset + 4] = 0;
        (*rm)[rmOffset + 6] = 0;   (*rm)[rmOffset + 9] = 0;
        (*rm)[rmOffset + 5] = 1;
    } else if (0.0f == x && 0.0f == y && 1.0f == z) {
        (*rm)[rmOffset + 0] = c;   (*rm)[rmOffset + 5] = c;
        (*rm)[rmOffset + 1] = s;   (*rm)[rmOffset + 4] = -s;
        (*rm)[rmOffset + 2] = 0;   (*rm)[rmOffset + 6] = 0;
        (*rm)[rmOffset + 8] = 0;   (*rm)[rmOffset + 9] = 0;
        (*rm)[rmOffset + 10]= 1;
    } else {
        Float len = Length(x, y, z);
        if (1.0f != len) {
            Float recipLen = 1.0f / len;
            x *= recipLen;
            y *= recipLen;
            z *= recipLen;
        }
        Float nc = 1.0f - c;
        Float xy = x * y;
        Float yz = y * z;
        Float zx = z * x;
        Float xs = x * s;
        Float ys = y * s;
        Float zs = z * s;
        (*rm)[rmOffset +  0] = x*x*nc +  c;
        (*rm)[rmOffset +  4] =  xy*nc - zs;
        (*rm)[rmOffset +  8] =  zx*nc + ys;
        (*rm)[rmOffset +  1] =  xy*nc + zs;
        (*rm)[rmOffset +  5] = y*y*nc +  c;
        (*rm)[rmOffset +  9] =  yz*nc - xs;
        (*rm)[rmOffset +  2] =  zx*nc - ys;
        (*rm)[rmOffset +  6] =  yz*nc + xs;
        (*rm)[rmOffset + 10] = z*z*nc +  c;
    }
    return NOERROR;
}

ECode CMatrix::SetRotateEulerM(
    /* [in] */ ArrayOf<Float>* rm,
    /* [in] */ Int32 rmOffset,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float z)
{
    x *= (Float) (Elastos::Core::Math::PI / 180.0f);
    y *= (Float) (Elastos::Core::Math::PI / 180.0f);
    z *= (Float) (Elastos::Core::Math::PI / 180.0f);
    Float cx = (Float) Elastos::Core::Math::Cos(x);
    Float sx = (Float) Elastos::Core::Math::Sin(x);
    Float cy = (Float) Elastos::Core::Math::Cos(y);
    Float sy = (Float) Elastos::Core::Math::Sin(y);
    Float cz = (Float) Elastos::Core::Math::Cos(z);
    Float sz = (Float) Elastos::Core::Math::Sin(z);
    Float cxsy = cx * sy;
    Float sxsy = sx * sy;

    (*rm)[rmOffset + 0]  =   cy * cz;
    (*rm)[rmOffset + 1]  =  -cy * sz;
    (*rm)[rmOffset + 2]  =   sy;
    (*rm)[rmOffset + 3]  =  0.0f;

    (*rm)[rmOffset + 4]  =  cxsy * cz + cx * sz;
    (*rm)[rmOffset + 5]  = -cxsy * sz + cx * cz;
    (*rm)[rmOffset + 6]  =  -sx * cy;
    (*rm)[rmOffset + 7]  =  0.0f;

    (*rm)[rmOffset + 8]  = -sxsy * cz + sx * sz;
    (*rm)[rmOffset + 9]  =  sxsy * sz + sx * cz;
    (*rm)[rmOffset + 10] =  cx * cy;
    (*rm)[rmOffset + 11] =  0.0f;

    (*rm)[rmOffset + 12] =  0.0f;
    (*rm)[rmOffset + 13] =  0.0f;
    (*rm)[rmOffset + 14] =  0.0f;
    (*rm)[rmOffset + 15] =  1.0f;
    return NOERROR;
}

ECode CMatrix::SetLookAtM(
    /* [in] */ ArrayOf<Float>* rm,
    /* [in] */ Int32 rmOffset,
    /* [in] */ Float  eyeX,
    /* [in] */ Float eyeY,
    /* [in] */ Float eyeZ,
    /* [in] */ Float centerX,
    /* [in] */ Float centerY,
    /* [in] */ Float centerZ,
    /* [in] */ Float upX,
    /* [in] */ Float upY,
    /* [in] */ Float upZ)
{

    // See the OpenGL GLUT documentation for gluLookAt for a description
    // of the algorithm. We implement it in a straightforward way:

    Float fx = centerX - eyeX;
    Float fy = centerY - eyeY;
    Float fz = centerZ - eyeZ;

    // Normalize f
    Float rlf = 1.0f / Length(fx, fy, fz);
    fx *= rlf;
    fy *= rlf;
    fz *= rlf;

    // compute s = f x up (x means "cross product")
    Float sx = fy * upZ - fz * upY;
    Float sy = fz * upX - fx * upZ;
    Float sz = fx * upY - fy * upX;

    // and normalize s
    Float rls = 1.0f / Length(sx, sy, sz);
    sx *= rls;
    sy *= rls;
    sz *= rls;

    // compute u = s x f
    Float ux = sy * fz - sz * fy;
    Float uy = sz * fx - sx * fz;
    Float uz = sx * fy - sy * fx;

    (*rm)[rmOffset + 0] = sx;
    (*rm)[rmOffset + 1] = ux;
    (*rm)[rmOffset + 2] = -fx;
    (*rm)[rmOffset + 3] = 0.0f;

    (*rm)[rmOffset + 4] = sy;
    (*rm)[rmOffset + 5] = uy;
    (*rm)[rmOffset + 6] = -fy;
    (*rm)[rmOffset + 7] = 0.0f;

    (*rm)[rmOffset + 8] = sz;
    (*rm)[rmOffset + 9] = uz;
    (*rm)[rmOffset + 10] = -fz;
    (*rm)[rmOffset + 11] = 0.0f;

    (*rm)[rmOffset + 12] = 0.0f;
    (*rm)[rmOffset + 13] = 0.0f;
    (*rm)[rmOffset + 14] = 0.0f;
    (*rm)[rmOffset + 15] = 1.0f;

    TranslateM(rm, rmOffset, -eyeX, -eyeY, -eyeZ);
    return NOERROR;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos
