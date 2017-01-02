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

#include "CRC2ParameterSpec.h"
#include "Arrays.h"

using Elastos::Utility::Arrays;

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_OBJECT_IMPL(CRC2ParameterSpec)
CAR_INTERFACE_IMPL_2(CRC2ParameterSpec, Object, IRC2ParameterSpec, IAlgorithmParameterSpec)

CRC2ParameterSpec::CRC2ParameterSpec()
    : mEffectiveKeyBits(0)
{
}

ECode CRC2ParameterSpec::constructor(
    /* [in] */ Int32 effectiveKeyBits)
{
    mEffectiveKeyBits = effectiveKeyBits;
    return NOERROR;
}

ECode CRC2ParameterSpec::constructor(
    /* [in] */ Int32 effectiveKeyBits,
    /* [in] */ ArrayOf<Byte> * iv)
{
    if (iv == NULL) {
        // throw new IllegalArgumentException("iv == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if(iv->GetLength() < 8) {
        // throw new IllegalArgumentException("iv.length < 8");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mEffectiveKeyBits = effectiveKeyBits;
    mIv = ArrayOf<Byte>::Alloc(8);
    mIv->Copy(0, iv, 0, 8);
    return NOERROR;
}

ECode CRC2ParameterSpec::constructor(
    /* [in] */ Int32 effectiveKeyBits,
    /* [in] */ ArrayOf<Byte> * iv,
    /* [in] */ Int32 offset)
{
    if (iv == NULL) {
        // throw new IllegalArgumentException("iv == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if(iv->GetLength() - offset < 8) {
        // throw new IllegalArgumentException("iv.length < 8");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mEffectiveKeyBits = effectiveKeyBits;
    mIv = ArrayOf<Byte>::Alloc(8);
    mIv->Copy(0, iv, 0, 8);
    return NOERROR;
}

ECode CRC2ParameterSpec::GetEffectiveKeyBits(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEffectiveKeyBits;
    return NOERROR;
}

ECode CRC2ParameterSpec::GetIV(
    /* [out, callee] */ ArrayOf<Byte> ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (mIv == NULL) {
        return NOERROR;
    }
    *result = ArrayOf<Byte>::Alloc(mIv->GetLength());
    (*result)->Copy(0, mIv, 0, mIv->GetLength());
    return NOERROR;
}

ECode CRC2ParameterSpec::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    *hashCode = mEffectiveKeyBits;
    if (mIv == NULL) {
        return NOERROR;
    }
    for(Int32 i = 0; i < mIv->GetLength(); i++) {
        *hashCode += (*mIv)[i];
    }
    return NOERROR;
}

ECode CRC2ParameterSpec::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if ((CRC2ParameterSpec*)(IRC2ParameterSpec*)obj == this) {
        *result = TRUE;
        return NOERROR;
    }
    if (IRC2ParameterSpec::Probe(obj) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<CRC2ParameterSpec> ps = (CRC2ParameterSpec*)(IRC2ParameterSpec*) obj;
    *result = (mEffectiveKeyBits == ps->mEffectiveKeyBits)
        && (Arrays::Equals(mIv, ps->mIv));
    return NOERROR;
}

} // Spec
} // Crypto
} // Elastosx