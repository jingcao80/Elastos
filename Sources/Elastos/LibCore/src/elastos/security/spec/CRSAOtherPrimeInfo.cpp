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

#include "CRSAOtherPrimeInfo.h"

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CRSAOtherPrimeInfo)
CAR_INTERFACE_IMPL(CRSAOtherPrimeInfo, Object, IRSAOtherPrimeInfo)
ECode CRSAOtherPrimeInfo::GetCrtCoefficient(
    /* [out] */ IBigInteger **crtCoefficient)
{
    VALIDATE_NOT_NULL(crtCoefficient)
    *crtCoefficient = mCrtCoefficient;
    REFCOUNT_ADD(*crtCoefficient)
    return NOERROR;
}

ECode CRSAOtherPrimeInfo::GetPrime(
    /* [out] */ IBigInteger **prime)
{
    VALIDATE_NOT_NULL(prime)
    *prime = mPrime;
    REFCOUNT_ADD(*prime)
    return NOERROR;
}

ECode CRSAOtherPrimeInfo::GetExponent(
    /* [out] */ IBigInteger **exponent)
{
    VALIDATE_NOT_NULL(exponent)
    *exponent = mPrimeExponent;
    REFCOUNT_ADD(*exponent)
    return NOERROR;
}

ECode CRSAOtherPrimeInfo::constructor(
    /* [in] */ IBigInteger *prime,
    /* [in] */ IBigInteger *primeExponent,
    /* [in] */ IBigInteger *crtCoefficient)
{
    if (prime == NULL) {
        //throw new NullPointerException("prime == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (primeExponent == NULL) {
        //throw new NullPointerException("primeExponent == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (crtCoefficient == NULL) {
        //throw new NullPointerException("crtCoefficient == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mPrime = prime;
    mPrimeExponent = primeExponent;
    mCrtCoefficient = crtCoefficient;
    return NOERROR;
}

}
}
}
