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

#include "CRSAKeyGenParameterSpec.h"
#include "elastos/math/CBigInteger.h"

using Elastos::Math::CBigInteger;

namespace Elastos {
namespace Security {
namespace Spec {

const AutoPtr<IBigInteger> CRSAKeyGenParameterSpec::F0 = CRSAKeyGenParameterSpec::InitStatic(3LL);

const AutoPtr<IBigInteger> CRSAKeyGenParameterSpec::F4 = CRSAKeyGenParameterSpec::InitStatic(65537LL);

const AutoPtr<IBigInteger> CRSAKeyGenParameterSpec::InitStatic(
    /* [in] */ Int64 bi)
{
    AutoPtr<IBigInteger> tmp;
    CBigInteger::ValueOf(bi, (IBigInteger**)&tmp);
    return tmp;
}

CAR_OBJECT_IMPL(CRSAKeyGenParameterSpec)
CAR_INTERFACE_IMPL_2(CRSAKeyGenParameterSpec, Object, IRSAKeyGenParameterSpec, IAlgorithmParameterSpec)
ECode CRSAKeyGenParameterSpec::GetKeysize(
    /* [out] */ Int32 *keySize)
{
    VALIDATE_NOT_NULL(keySize)
    *keySize = mKeysize;
    return NOERROR;
}

ECode CRSAKeyGenParameterSpec::GetPublicExponent(
    /* [out] */ IBigInteger **pubExponent)
{
    VALIDATE_NOT_NULL(pubExponent)
    *pubExponent = mPublicExponent;
    REFCOUNT_ADD(*pubExponent)
    return NOERROR;
}

ECode CRSAKeyGenParameterSpec::constructor(
    /* [in] */ Int32 keysize,
    /* [in] */ IBigInteger *publicExponent)
{
    mKeysize = keysize;
    mPublicExponent = publicExponent;
    return NOERROR;
}

}
}
}
