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

#ifndef __ELASTOS_SECURITY_SPEC_CRSAOTHERPRIMEINFO_H__
#define __ELASTOS_SECURITY_SPEC_CRSAOTHERPRIMEINFO_H__

#include "_Elastos_Security_Spec_CRSAOtherPrimeInfo.h"
#include "EncodedKeySpec.h"

using Elastos::Math::IBigInteger;

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CRSAOtherPrimeInfo)
    , public Object
    , public IRSAOtherPrimeInfo
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCrtCoefficient(
        /* [out] */ IBigInteger **crtCoefficient);

    CARAPI GetPrime(
        /* [out] */ IBigInteger **prime);

    CARAPI GetExponent(
        /* [out] */ IBigInteger **exponent);

    CARAPI constructor(
        /* [in] */ IBigInteger *prime,
        /* [in] */ IBigInteger *primeExponent,
        /* [in] */ IBigInteger *crtCoefficient);

private:
    // Prime
    AutoPtr<IBigInteger> mPrime;
    // Prime Exponent
    AutoPtr<IBigInteger> mPrimeExponent;
    // CRT Coefficient
    AutoPtr<IBigInteger> mCrtCoefficient;
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_CRSAOTHERPRIMEINFO_H__
