
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CColorMatrix.h"
#include <elastos/utility/Arrays.h>
#include <elastos/core/Math.h>

using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CColorMatrix);
CAR_INTERFACE_IMPL(CColorMatrix, Object, IColorMatrix);
CColorMatrix::CColorMatrix()
{
    mArray = ArrayOf<Float>::Alloc(20);
}

CColorMatrix::~CColorMatrix()
{
    mArray = NULL;
}

ECode CColorMatrix::constructor()
{
    Reset();
    return NOERROR;
}

ECode CColorMatrix::constructor(
    /* [in] */ const ArrayOf<Float>& src)
{
    mArray->Copy(0, &src, 0, 20);
    return NOERROR;
}

ECode CColorMatrix::constructor(
    /* [in] */ IColorMatrix* array)
{
    AutoPtr<ArrayOf<Float> > _arr;
    array->GetArray((ArrayOf<Float>**)&_arr);
    mArray->Copy(0, _arr, 0, 20);
    return NOERROR;
}

ECode CColorMatrix::GetArray(
    /* [out, callee] */ ArrayOf<Float>** array)
{
    VALIDATE_NOT_NULL(array);
    *array = mArray;
    REFCOUNT_ADD(*array);
    return NOERROR;
}

ECode CColorMatrix::Reset()
{
    ArrayOf<Float>& a = *mArray;

    Arrays::Fill(&a, 0.f);
    a[0] = a[6] = a[12] = a[18] = 1;
    return NOERROR;
}

ECode CColorMatrix::Set(
    /* [in] */ IColorMatrix* src)
{
    AutoPtr< ArrayOf<Float> > array;
    src->GetArray((ArrayOf<Float>**)&array);
    mArray->Copy(0, array, 0, 20);
    return NOERROR;
}

ECode CColorMatrix::Set(
    /* [in] */ ArrayOf<Float>* src)
{
    mArray->Copy(0, src, 0, 20);
    return NOERROR;
}

ECode CColorMatrix::SetScale(
    /* [in] */ Float rScale,
    /* [in] */ Float gScale,
    /* [in] */ Float bScale,
    /* [in] */ Float aScale)
{
    ArrayOf<Float>& a = *mArray;

    for (Int32 i = 19; i > 0; --i) {
        a[i] = 0;
    }
    a[0] = rScale;
    a[6] = gScale;
    a[12] = bScale;
    a[18] = aScale;
    return NOERROR;
}

ECode CColorMatrix::SetRotate(
    /* [in] */ Int32 axis,
    /* [in] */ Float degrees)
{
    Reset();
    Float radians = degrees * (Float)Elastos::Core::Math::PI / 180;
    Float cosine = (Float)Elastos::Core::Math::Cos((Double)radians);
    Float sine = (Float)Elastos::Core::Math::Sin((Double)radians);
    ArrayOf<Float>& a = *mArray;
    switch (axis) {
    // Rotation around the red color
        case 0:
        a[6] = a[12] = cosine;
        a[7] = sine;
        a[11] = -sine;
        break;
        // Rotation around the green color
        case 1:
        a[0] = a[12] = cosine;
        a[2] = -sine;
        a[10] = sine;
        break;
        // Rotation around the blue color
        case 2:
        a[0] = a[6] = cosine;
        a[1] = sine;
        a[5] = -sine;
        break;
        default:
        // throw new RuntimeException();
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode CColorMatrix::SetConcat(
    /* [in] */ IColorMatrix* matA,
    /* [in] */ IColorMatrix* matB)
{
    AutoPtr< ArrayOf<Float> > tmp;

    if (matA == this || matB == this) {
        tmp = ArrayOf<Float>::Alloc(20);
    }
    else {
        tmp = mArray;
    }

    AutoPtr< ArrayOf<Float> > a;
    AutoPtr< ArrayOf<Float> > b;
    matA->GetArray((ArrayOf<Float>**)&a);
    matB->GetArray((ArrayOf<Float>**)&b);
    Int32 index = 0;
    for (Int32 j = 0; j < 20; j += 5) {
        for (Int32 i = 0; i < 4; i++) {
            (*tmp)[index++] = (*a)[j + 0] * (*b)[i + 0] +  (*a)[j + 1] * (*b)[i + 5] +
                           (*a)[j + 2] * (*b)[i + 10] + (*a)[j + 3] * (*b)[i + 15];
        }
        (*tmp)[index++] = (*a)[j + 0] * (*b)[4] +  (*a)[j + 1] * (*b)[9] +
                        (*a)[j + 2] * (*b)[14] + (*a)[j + 3] * (*b)[19] +
                        (*a)[j + 4];
    }

    if (tmp.Get() != mArray) {
        mArray->Copy(0, tmp, 0, 20);
    }
    return NOERROR;
}

ECode CColorMatrix::PreConcat(
    /* [in] */ IColorMatrix* prematrix)
{
    return SetConcat(this, prematrix);
}

ECode CColorMatrix::PostConcat(
    /* [in] */ IColorMatrix* postmatrix)
{
    return SetConcat(postmatrix, this);
}

ECode CColorMatrix::SetSaturation(
    /* [in] */ Float sat)
{
    Reset();
    ArrayOf<Float>& a = *mArray;

    Float invSat = 1 - sat;
    Float R = 0.213f * invSat;
    Float G = 0.715f * invSat;
    Float B = 0.072f * invSat;

    a[0] = R + sat;      a[1] = G;    a[2] = B;
    a[5] = R;    a[6] = G + sat;  a[7] = B;
    a[10] = R;   a[11] = G;       a[12] = B + sat;
    return NOERROR;
}

ECode CColorMatrix::SetRGB2YUV()
{
    Reset();
    ArrayOf<Float>& a = *mArray;
    // these coefficients match those in libjpeg
    a[0]  = 0.299f;    a[1]  = 0.587f;    a[2]  = 0.114f;
    a[5]  = -0.16874f; a[6]  = -0.33126f; a[7]  = 0.5f;
    a[10] = 0.5f;      a[11] = -0.41869f; a[12] = -0.08131f;
    return NOERROR;
}

ECode CColorMatrix::SetYUV2RGB()
{
    Reset();
    ArrayOf<Float>& a = *mArray;
    // these coefficients match those in libjpeg
    a[2] = 1.402f;
    a[5] = 1;   a[6] = -0.34414f;   a[7] = -0.71414f;
    a[10] = 1;  a[11] = 1.772f;     a[12] = 0;
    return NOERROR;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
