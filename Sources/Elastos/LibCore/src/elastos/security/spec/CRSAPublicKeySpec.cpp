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

#include "CRSAPublicKeySpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CRSAPublicKeySpec)
CAR_INTERFACE_IMPL_2(CRSAPublicKeySpec, Object, IRSAPublicKeySpec, IKeySpec)
ECode CRSAPublicKeySpec::GetModulus(
    /* [out] */ IBigInteger **modulus)
{
    VALIDATE_NOT_NULL(modulus)
    *modulus = mModulus;
    REFCOUNT_ADD(*modulus)
    return NOERROR;
}

ECode CRSAPublicKeySpec::GetPublicExponent(
    /* [out] */ IBigInteger **exponent)
{
    VALIDATE_NOT_NULL(exponent)
    *exponent = mPublicExponent;
    REFCOUNT_ADD(*exponent)
    return NOERROR;
}

ECode CRSAPublicKeySpec::constructor(
    /* [in] */ IBigInteger *modulus,
    /* [in] */ IBigInteger *publicExponent)
{
    mModulus = modulus;
    mPublicExponent = publicExponent;
    return NOERROR;
}

}
}
}
