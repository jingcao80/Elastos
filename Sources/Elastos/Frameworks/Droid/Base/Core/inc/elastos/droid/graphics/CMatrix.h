
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

        CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

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
            /* [in] */ const ArrayOf<Float>& src,
            /* [in] */ Int32 srcIndex,
            /* [out] */ ArrayOf<Float>* dst,
            /* [in] */ Int32 dstIndex,
            /* [in] */ Int32 pointCount,
            /* [out] */ Boolean* result);

        CARAPI SetValues(
            /* [in] */ const ArrayOf<Float>& values);
    };

public:
    CAR_OBJECT_DECL();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

public:
    static AutoPtr<IMatrix> IDENTITY_MATRIX;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_CMATRIX_H__
