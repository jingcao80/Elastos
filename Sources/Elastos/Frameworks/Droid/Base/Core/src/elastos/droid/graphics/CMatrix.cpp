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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CMatrix.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Graphics {

CMatrix::IdentityMatrix::IdentityMatrix()
{
    Matrix::constructor();
}

CMatrix::IdentityMatrix::IdentityMatrix(
    /* [in] */ IMatrix* src)
{
    Matrix::constructor(src);
}

ECode CMatrix::IdentityMatrix::Oops()
{
    // throw new IllegalStateException("Matrix can not be modified");
    return E_ILLEGAL_STATE_EXCEPTION;
}

ECode CMatrix::IdentityMatrix::Set(
    /* [in] */ IMatrix* src)
{
    return Oops();
}

ECode CMatrix::IdentityMatrix::Reset()
{
    return Oops();
}

ECode CMatrix::IdentityMatrix::SetTranslate(
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    return Oops();
}

ECode CMatrix::IdentityMatrix::SetScale(
    /* [in] */ Float sx,
    /* [in] */ Float sy,
    /* [in] */ Float px,
    /* [in] */ Float py)
{
    return Oops();
}

ECode CMatrix::IdentityMatrix::SetScale(
    /* [in] */ Float sx,
    /* [in] */ Float sy)
{
    return Oops();
}

ECode CMatrix::IdentityMatrix::SetRotate(
    /* [in] */ Float degrees,
    /* [in] */ Float px,
    /* [in] */ Float py)
{
    return Oops();
}

ECode CMatrix::IdentityMatrix::SetRotate(
    /* [in] */ Float degrees)
{
    return Oops();
}

ECode CMatrix::IdentityMatrix::SetSinCos(
    /* [in] */ Float sinValue,
    /* [in] */ Float cosValue,
    /* [in] */ Float px,
    /* [in] */ Float py)
{
    return Oops();
}

ECode CMatrix::IdentityMatrix::SetSinCos(
    /* [in] */ Float sinValue,
    /* [in] */ Float cosValue)
{
    return Oops();
}

ECode CMatrix::IdentityMatrix::SetSkew(
    /* [in] */ Float kx,
    /* [in] */ Float ky,
    /* [in] */ Float px,
    /* [in] */ Float py)
{
    return Oops();
}

ECode CMatrix::IdentityMatrix::SetSkew(
    /* [in] */ Float kx,
    /* [in] */ Float ky)
{
    return Oops();
}

ECode CMatrix::IdentityMatrix::SetConcat(
    /* [in] */ IMatrix* a,
    /* [in] */ IMatrix* b,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return Oops();
}

ECode CMatrix::IdentityMatrix::PreTranslate(
    /* [in] */ Float dx,
    /* [in] */ Float dy,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return Oops();
}

ECode CMatrix::IdentityMatrix::PreScale(
    /* [in] */ Float sx,
    /* [in] */ Float sy,
    /* [in] */ Float px,
    /* [in] */ Float py,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return Oops();
}

ECode CMatrix::IdentityMatrix::PreScale(
    /* [in] */ Float sx,
    /* [in] */ Float sy,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return Oops();
}

ECode CMatrix::IdentityMatrix::PreRotate(
    /* [in] */ Float degrees,
    /* [in] */ Float px,
    /* [in] */ Float py,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return Oops();
}

ECode CMatrix::IdentityMatrix::PreRotate(
    /* [in] */ Float degrees,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return Oops();
}

ECode CMatrix::IdentityMatrix::PreSkew(
    /* [in] */ Float kx,
    /* [in] */ Float ky,
    /* [in] */ Float px,
    /* [in] */ Float py,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return Oops();
}

ECode CMatrix::IdentityMatrix::PreSkew(
    /* [in] */ Float kx,
    /* [in] */ Float ky,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return Oops();
}

ECode CMatrix::IdentityMatrix::PreConcat(
    /* [in] */ IMatrix* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return Oops();
}

ECode CMatrix::IdentityMatrix::PostTranslate(
    /* [in] */ Float dx,
    /* [in] */ Float dy,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return Oops();
}

ECode CMatrix::IdentityMatrix::PostScale(
    /* [in] */ Float sx,
    /* [in] */ Float sy,
    /* [in] */ Float px,
    /* [in] */ Float py,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return Oops();
}

ECode CMatrix::IdentityMatrix::PostScale(
    /* [in] */ Float sx,
    /* [in] */ Float sy,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return Oops();
}

ECode CMatrix::IdentityMatrix::PostRotate(
    /* [in] */ Float degrees,
    /* [in] */ Float px,
    /* [in] */ Float py,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return Oops();
}

ECode CMatrix::IdentityMatrix::PostRotate(
    /* [in] */ Float degrees,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return Oops();
}

ECode CMatrix::IdentityMatrix::PostSkew(
    /* [in] */ Float kx,
    /* [in] */ Float ky,
    /* [in] */ Float px,
    /* [in] */ Float py,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return Oops();
}

ECode CMatrix::IdentityMatrix::PostSkew(
    /* [in] */ Float kx,
    /* [in] */ Float ky,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return Oops();
}

ECode CMatrix::IdentityMatrix::PostConcat(
    /* [in] */ IMatrix* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return Oops();
}

ECode CMatrix::IdentityMatrix::SetRectToRect(
    /* [in] */ IRectF* src,
    /* [in] */ IRectF* dst,
    /* [in] */ MatrixScaleToFit stf,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return Oops();
}

ECode CMatrix::IdentityMatrix::SetPolyToPoly(
    /* [in] */ ArrayOf<Float>* src,
    /* [in] */ Int32 srcIndex,
    /* [out] */ ArrayOf<Float>* dst,
    /* [in] */ Int32 dstIndex,
    /* [in] */ Int32 pointCount,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return Oops();
}

ECode CMatrix::IdentityMatrix::SetValues(
    /* [in] */ ArrayOf<Float>* values)
{
    return Oops();
}

AutoPtr<IMatrix> CMatrix::IDENTITY_MATRIX = new IdentityMatrix();
CAR_OBJECT_IMPL(CMatrix);

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
