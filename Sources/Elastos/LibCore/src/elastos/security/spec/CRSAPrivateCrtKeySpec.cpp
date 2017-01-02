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

#include "CRSAPrivateCrtKeySpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CRSAPrivateCrtKeySpec)
CAR_INTERFACE_IMPL(CRSAPrivateCrtKeySpec, RSAPrivateKeySpec, IRSAPrivateCrtKeySpec)
ECode CRSAPrivateCrtKeySpec::GetCrtCoefficient(
    /* [out] */ IBigInteger **coefficient)
{
    VALIDATE_NOT_NULL(coefficient)
    *coefficient = mCrtCoefficient;
    REFCOUNT_ADD(*coefficient)
    return NOERROR;
}

ECode CRSAPrivateCrtKeySpec::GetPrimeExponentP(
    /* [out] */ IBigInteger **p)
{
    VALIDATE_NOT_NULL(p)
    *p = mPrimeExponentP;
    REFCOUNT_ADD(*p)
    return NOERROR;
}

ECode CRSAPrivateCrtKeySpec::GetPrimeExponentQ(
    /* [out] */ IBigInteger **q)
{
    VALIDATE_NOT_NULL(q)
    *q = mPrimeExponentQ;
    REFCOUNT_ADD(*q)
    return NOERROR;
}

ECode CRSAPrivateCrtKeySpec::GetPrimeP(
    /* [out] */ IBigInteger **p)
{
    VALIDATE_NOT_NULL(p)
    *p = mPrimeP;
    REFCOUNT_ADD(*p)
    return NOERROR;
}

ECode CRSAPrivateCrtKeySpec::GetPrimeQ(
    /* [out] */ IBigInteger **q)
{
    VALIDATE_NOT_NULL(q)
    *q = mPrimeQ;
    REFCOUNT_ADD(*q)
    return NOERROR;
}

ECode CRSAPrivateCrtKeySpec::GetPublicExponent(
    /* [out] */ IBigInteger **pubExponent)
{
    VALIDATE_NOT_NULL(pubExponent)
    *pubExponent = mPublicExponent;
    REFCOUNT_ADD(*pubExponent)
    return NOERROR;
}

ECode CRSAPrivateCrtKeySpec::constructor(
    /* [in] */ IBigInteger *modulus,
    /* [in] */ IBigInteger *publicExponent,
    /* [in] */ IBigInteger *privateExponent,
    /* [in] */ IBigInteger *primeP,
    /* [in] */ IBigInteger *primeQ,
    /* [in] */ IBigInteger *primeExponentP,
    /* [in] */ IBigInteger *primeExponentQ,
    /* [in] */ IBigInteger *crtCoefficient)
{
    RSAPrivateKeySpec::constructor(modulus, privateExponent);

    mPublicExponent = publicExponent;
    mPrimeP = primeP;
    mPrimeQ = primeQ;
    mPrimeExponentP = primeExponentP;
    mPrimeExponentQ = primeExponentQ;
    mCrtCoefficient = crtCoefficient;
    return NOERROR;
}

}
}
}
