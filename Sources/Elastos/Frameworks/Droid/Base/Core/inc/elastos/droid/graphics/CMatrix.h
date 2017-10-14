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

#ifndef __ELASTOS_DROID_GRAPHICS_CMATRIX_H__
#define __ELASTOS_DROID_GRAPHICS_CMATRIX_H__

#include "_Elastos_Droid_Graphics_CMatrix.h"
#include "Matrix.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CMatrix), public Matrix
{
private:
    class IdentityMatrix
        : public Matrix
    {
    public:
        IdentityMatrix();

        IdentityMatrix(
            /* [in] */ IMatrix* src);

        CARAPI Oops();

        CARAPI Set(
            /* [in] */ IMatrix* src);

        CARAPI Reset();

        CARAPI SetTranslate(
            /* [in] */ Float dx,
            /* [in] */ Float dy);

        CARAPI SetScale(
            /* [in] */ Float sx,
            /* [in] */ Float sy,
            /* [in] */ Float px,
            /* [in] */ Float py);

        CARAPI SetScale(
            /* [in] */ Float sx,
            /* [in] */ Float sy);

        CARAPI SetRotate(
            /* [in] */ Float degrees,
            /* [in] */ Float px,
            /* [in] */ Float py);

        CARAPI SetRotate(
            /* [in] */ Float degrees);

        CARAPI SetSinCos(
            /* [in] */ Float sinValue,
            /* [in] */ Float cosValue,
            /* [in] */ Float px,
            /* [in] */ Float py);

        CARAPI SetSinCos(
            /* [in] */ Float sinValue,
            /* [in] */ Float cosValue);

        CARAPI SetSkew(
            /* [in] */ Float kx,
            /* [in] */ Float ky,
            /* [in] */ Float px,
            /* [in] */ Float py);

        CARAPI SetSkew(
            /* [in] */ Float kx,
            /* [in] */ Float ky);

        CARAPI SetConcat(
            /* [in] */ IMatrix* a,
            /* [in] */ IMatrix* b,
            /* [out] */ Boolean* result);

        CARAPI PreTranslate(
            /* [in] */ Float dx,
            /* [in] */ Float dy,
            /* [out] */ Boolean* result);

        CARAPI PreScale(
            /* [in] */ Float sx,
            /* [in] */ Float sy,
            /* [in] */ Float px,
            /* [in] */ Float py,
            /* [out] */ Boolean* result);

        CARAPI PreScale(
            /* [in] */ Float sx,
            /* [in] */ Float sy,
            /* [out] */ Boolean* result);

        CARAPI PreRotate(
            /* [in] */ Float degrees,
            /* [in] */ Float px,
            /* [in] */ Float py,
            /* [out] */ Boolean* result);

        CARAPI PreRotate(
            /* [in] */ Float degrees,
            /* [out] */ Boolean* result);

        CARAPI PreSkew(
            /* [in] */ Float kx,
            /* [in] */ Float ky,
            /* [in] */ Float px,
            /* [in] */ Float py,
            /* [out] */ Boolean* result);

        CARAPI PreSkew(
            /* [in] */ Float kx,
            /* [in] */ Float ky,
            /* [out] */ Boolean* result);

        CARAPI PreConcat(
            /* [in] */ IMatrix* other,
            /* [out] */ Boolean* result);

        CARAPI PostTranslate(
            /* [in] */ Float dx,
            /* [in] */ Float dy,
            /* [out] */ Boolean* result);

        CARAPI PostScale(
            /* [in] */ Float sx,
            /* [in] */ Float sy,
            /* [in] */ Float px,
            /* [in] */ Float py,
            /* [out] */ Boolean* result);

        CARAPI PostScale(
            /* [in] */ Float sx,
            /* [in] */ Float sy,
            /* [out] */ Boolean* result);

        CARAPI PostRotate(
            /* [in] */ Float degrees,
            /* [in] */ Float px,
            /* [in] */ Float py,
            /* [out] */ Boolean* result);

        CARAPI PostRotate(
            /* [in] */ Float degrees,
            /* [out] */ Boolean* result);

        CARAPI PostSkew(
            /* [in] */ Float kx,
            /* [in] */ Float ky,
            /* [in] */ Float px,
            /* [in] */ Float py,
            /* [out] */ Boolean* result);

        CARAPI PostSkew(
            /* [in] */ Float kx,
            /* [in] */ Float ky,
            /* [out] */ Boolean* result);

        CARAPI PostConcat(
            /* [in] */ IMatrix* other,
            /* [out] */ Boolean* result);

        CARAPI SetRectToRect(
            /* [in] */ IRectF* src,
            /* [in] */ IRectF* dst,
            /* [in] */ MatrixScaleToFit stf,
            /* [out] */ Boolean* result);

        CARAPI SetPolyToPoly(
            /* [in] */ ArrayOf<Float>* src,
            /* [in] */ Int32 srcIndex,
            /* [out] */ ArrayOf<Float>* dst,
            /* [in] */ Int32 dstIndex,
            /* [in] */ Int32 pointCount,
            /* [out] */ Boolean* result);

        CARAPI SetValues(
            /* [in] */ ArrayOf<Float>* values);
    };

public:
    CAR_OBJECT_DECL()

public:
    static AutoPtr<IMatrix> IDENTITY_MATRIX;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_CMATRIX_H__
