
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

PInterface CMatrix::IdentityMatrix::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_Matrix) {
        return reinterpret_cast<PInterface>((Matrix*)this);
    }
    return Matrix::Probe(riid);
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
    /* [in] */ const ArrayOf<Float>& src,
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
    /* [in] */ const ArrayOf<Float>& values)
{
    return Oops();
}

AutoPtr<IMatrix> CMatrix::IDENTITY_MATRIX = new IdentityMatrix();
CAR_OBJECT_IMPL(CMatrix);
PInterface CMatrix::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_Matrix) {
        return reinterpret_cast<PInterface>((Matrix*)this);
    }
    return Matrix::Probe(riid);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
