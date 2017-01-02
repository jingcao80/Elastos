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

#ifndef __ELASTOS_DROID_GRAPHICS_CMATRIXIDENTY_H__
#define __ELASTOS_DROID_GRAPHICS_CMATRIXIDENTY_H__

#include "_Elastos_Droid_Graphics_CMatrixIdenty.h"
#include "elastos/droid/graphics/CMatrix.h"
#include <SkMatrix.h>
#include "elastos/droid/graphics/Matrix.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

class Canvas;

class CMatrixIdenty : public Matrix {

    friend class Canvas;
    friend class Shader;
    friend class CPath;

public:
    void Oops();
    virtual CARAPI Set(
        /* [in] */ IMatrix* src);
    virtual CARAPI Reset();
    virtual CARAPI SetTranslate(
        /* [in] */ Float dx,
        /* [in] */ Float dy);
    virtual CARAPI SetScale(
        /* [in] */ Float sx,
        /* [in] */ Float sy,
        /* [in] */ Float px,
        /* [in] */ Float py);
    virtual CARAPI SetScale(
        /* [in] */ Float sx,
        /* [in] */ Float sy);
    virtual CARAPI SetRotate(
        /* [in] */ Float degrees,
        /* [in] */ Float px,
        /* [in] */ Float py);
    virtual CARAPI SetRotate(
        /* [in] */ Float degrees);
    virtual CARAPI SetSinCos(
        /* [in] */ Float sinValue,
        /* [in] */ Float cosValue,
        /* [in] */ Float px,
        /* [in] */ Float py);
    virtual CARAPI SetSinCos(
        /* [in] */ Float sinValue,
        /* [in] */ Float cosValue);
    virtual CARAPI SetSkew(
        /* [in] */ Float kx,
        /* [in] */ Float ky,
        /* [in] */ Float px,
        /* [in] */ Float py);
    virtual CARAPI SetSkew(
        /* [in] */ Float kx,
        /* [in] */ Float ky);
    virtual CARAPI SetConcat(
        /* [in] */ IMatrix* a,
        /* [in] */ IMatrix* b,
        /* [out] */ Boolean* result);
    virtual CARAPI PreTranslate(
        /* [in] */ Float dx,
        /* [in] */ Float dy,
        /* [out] */ Boolean* result);
    virtual CARAPI PreScale(
        /* [in] */ Float sx,
        /* [in] */ Float sy,
        /* [in] */ Float px,
        /* [in] */ Float py,
        /* [out] */ Boolean* result);
    virtual CARAPI PreScale(
        /* [in] */ Float sx,
        /* [in] */ Float sy,
        /* [out] */ Boolean* result);
    virtual CARAPI PreRotate(
        /* [in] */ Float degrees,
        /* [in] */ Float px,
        /* [in] */ Float py,
        /* [out] */ Boolean* result);
    virtual CARAPI PreRotate(
        /* [in] */ Float degrees,
        /* [out] */ Boolean* result);
    virtual CARAPI PreSkew(
        /* [in] */ Float kx,
        /* [in] */ Float ky,
        /* [in] */ Float px,
        /* [in] */ Float py,
        /* [out] */ Boolean* result);
    virtual CARAPI PreSkew(
        /* [in] */ Float kx,
        /* [in] */ Float ky,
        /* [out] */ Boolean* result);
    virtual CARAPI PreConcat(
        /* [in] */ IMatrix* other,
        /* [out] */ Boolean* result);
    virtual CARAPI PostTranslate(
        /* [in] */ Float dx,
        /* [in] */ Float dy,
        /* [out] */ Boolean* result);
    virtual CARAPI PostScale(
        /* [in] */ Float sx,
        /* [in] */ Float sy,
        /* [in] */ Float px,
        /* [in] */ Float py,
        /* [out] */ Boolean* result);
    virtual CARAPI PostScale(
        /* [in] */ Float sx,
        /* [in] */ Float sy,
        /* [out] */ Boolean* result);
    virtual CARAPI PostRotate(
        /* [in] */ Float degrees,
        /* [in] */ Float px,
        /* [in] */ Float py,
        /* [out] */ Boolean* result);
    virtual CARAPI PostRotate(
        /* [in] */ Float degrees,
        /* [out] */ Boolean* result);
    virtual CARAPI PostSkew(
        /* [in] */ Float kx,
        /* [in] */ Float ky,
        /* [in] */ Float px,
        /* [in] */ Float py,
        /* [out] */ Boolean* result);
    virtual CARAPI PostSkew(
        /* [in] */ Float kx,
        /* [in] */ Float ky,
        /* [out] */ Boolean* result);
    virtual CARAPI PostConcat(
        /* [in] */ IMatrix* other,
        /* [out] */ Boolean* result);
    virtual CARAPI SetRectToRect(
        /* [in] */ IRectF* src,
        /* [in] */ IRectF* dst,
        /* [in] */ MatrixScaleToFit stf,
        /* [out] */ Boolean* result);
    virtual CARAPI SetPolyToPoly(
        /* [in] */ const ArrayOf<Float>& src,
        /* [in] */ Int32 srcIndex,
        /* [in] */ const ArrayOf<Float>& dst,
        /* [in] */ Int32 dstIndex,
        /* [in] */ Int32 pointCount,
        /* [out] */ Boolean* result);
    virtual CARAPI SetValues(
        /* [in] */ const ArrayOf<Float>& values);
};

#endif //__ELASTOS_DROID_GRAPHICS_CMATRIXIDENTY_H__
