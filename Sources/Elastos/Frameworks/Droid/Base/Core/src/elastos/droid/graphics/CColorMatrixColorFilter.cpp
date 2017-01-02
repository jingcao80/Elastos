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
#include "elastos/droid/graphics/CColorMatrixColorFilter.h"
#include "elastos/droid/graphics/CColorMatrix.h"
#include <skia/effects/SkColorMatrixFilter.h>
#include <skia/core/SkColorFilter.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CColorMatrixColorFilter);
CAR_INTERFACE_IMPL(CColorMatrixColorFilter, ColorFilter, IColorMatrixColorFilter);
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
