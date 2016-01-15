
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CColorMatrixColorFilter.h"
#include "elastos/droid/graphics/CColorMatrix.h"
#include <skia/effects/SkColorMatrixFilter.h>
#include <skia/core/SkColorFilter.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CColorMatrixColorFilter);

CColorMatrixColorFilter::CColorMatrixColorFilter()
{
    CColorMatrix::New((IColorMatrix**)&mMatrix);
}

ECode CColorMatrixColorFilter::constructor(
    /* [in] */ IColorMatrix* matrix)
{
    mMatrix->Set(matrix);
    Update();
    return NOERROR;
}

ECode CColorMatrixColorFilter::constructor(
    /* [in] */ const ArrayOf<Float>& array)
{
    if (array.GetLength() < 20) {
        // throw new ArrayIndexOutOfBoundsException();
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    mMatrix->Set(const_cast<ArrayOf<Float>*>(&array));
    Update();
    return NOERROR;
}

PInterface CColorMatrixColorFilter::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_ColorFilter) {
        return reinterpret_cast<PInterface>((ColorFilter*)this);
    }
    else if (riid == EIID_IColorMatrixColorFilter) {
        return (IColorMatrixColorFilter*)this;
    }
    return ColorFilter::Probe(riid);
}

ECode CColorMatrixColorFilter::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid);

    if (object == reinterpret_cast<PInterface>((ColorFilter*)this)) {
        *iid = EIID_ColorFilter;
        return NOERROR;
    }
    else if (object == (IColorMatrixColorFilter*)this) {
        *iid = EIID_IColorMatrixColorFilter;
        return NOERROR;
    }
    return ColorFilter::GetInterfaceID(object, iid);
}

UInt32 CColorMatrixColorFilter::AddRef()
{
    return ColorFilter::AddRef();
}

UInt32 CColorMatrixColorFilter::Release()
{
    return ColorFilter::Release();
}

ECode CColorMatrixColorFilter::GetColorMatrix(
    /* [out] */ IColorMatrix** matrix)
{
    VALIDATE_NOT_NULL(matrix);
    *matrix = mMatrix;
    REFCOUNT_ADD(*matrix);
    return NOERROR;
}

ECode CColorMatrixColorFilter::SetColorMatrix(
    /* [in] */ IColorMatrix* matrix)
{
    if (matrix == NULL) {
        mMatrix->Reset();
    } else if (matrix != mMatrix.Get()) {
        mMatrix->Set(matrix);
    }
    Update();
    return NOERROR;
}

ECode CColorMatrixColorFilter::SetColorMatrix(
    /* [in] */ ArrayOf<Float>* array)
{
    if (array == NULL) {
        mMatrix->Reset();
    } else {
        if (array->GetLength() < 20) {
            // throw new ArrayIndexOutOfBoundsException();
            return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        }

        mMatrix->Set(array);
    }
    Update();
    return NOERROR;
}

void CColorMatrixColorFilter::Update()
{
    AutoPtr<ArrayOf<Float> > colorMatrix;
    mMatrix->GetArray((ArrayOf<Float>**)&colorMatrix);
    DestroyFilter(mNativeInstance);
    mNativeInstance = NativeColorMatrixFilter(colorMatrix);
}

Int64 CColorMatrixColorFilter::NativeColorMatrixFilter(
    /* [in] */ ArrayOf<Float>* array)
{
    assert(array != NULL);
    const Float* src = array->GetPayload();

#ifdef SK_SCALAR_IS_FLOAT
    return reinterpret_cast<Int64>(SkColorMatrixFilter::Create(src));
#else
    SkASSERT(false);
#endif
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
