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

#include "CIvParameterSpec.h"
#include "Arrays.h"

using Elastos::Utility::Arrays;

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_OBJECT_IMPL(CIvParameterSpec)
CAR_INTERFACE_IMPL_2(CIvParameterSpec, Object, IIvParameterSpec, IAlgorithmParameterSpec)

CIvParameterSpec::CIvParameterSpec()
{
}

ECode CIvParameterSpec::constructor(
    /* [in] */ ArrayOf<Byte> * iv)
{
    if (iv == NULL) {
        // throw new IllegalArgumentException("iv == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mIv = ArrayOf<Byte>::Alloc(iv->GetLength());
    mIv->Copy(0, iv, 0, iv->GetLength());
    return NOERROR;
}

ECode CIvParameterSpec::constructor(
    /* [in] */ ArrayOf<Byte> * iv,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount)
{
    if ((iv == NULL)|| (iv->GetLength() - offset < byteCount)) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Arrays::CheckOffsetAndCount(iv->GetLength(), offset, byteCount);
    mIv = ArrayOf<Byte>::Alloc(byteCount);
    // System.arraycopy(iv, offset, this.iv, 0, byteCount);
    mIv->Copy(0, iv, offset, byteCount);
    return NOERROR;
}

ECode CIvParameterSpec::GetIV(
    /* [out, callee] */ ArrayOf<Byte> ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = ArrayOf<Byte>::Alloc(mIv->GetLength());
    (*result)->Copy(0, mIv, 0, mIv->GetLength());
    return NOERROR;
}

} // Spec
} // Crypto
} // Elastosx