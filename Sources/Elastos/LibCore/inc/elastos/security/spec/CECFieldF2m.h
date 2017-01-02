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

#ifndef __ELASTOS_SECURITY_SPEC_CECFIELDF2M_H__
#define __ELASTOS_SECURITY_SPEC_CECFIELDF2M_H__

#include "_Elastos_Security_Spec_CECFieldF2m.h"
#include "core/Object.h"

using Elastos::Math::IBigInteger;

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CECFieldF2m)
    , public Object
    , public IECFieldF2m
    , public IECField
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetFieldSize(
        /* [out] */ Int32 *size);

    CARAPI GetM(
        /* [out] */ Int32* m);

    CARAPI Equals(
        /* [in] */ IInterface *obj,
        /* [out] */ Boolean *result);

    CARAPI GetMidTermsOfReductionPolynomial(
        /* [out, callee] */ ArrayOf<Int32> **terms);

    CARAPI GetReductionPolynomial(
        /* [out] */ IBigInteger **rp);

    CARAPI GetHashCode(
        /* [out] */ Int32 *hashCode);

    CARAPI constructor(
        /* [in] */ Int32 m);

    CARAPI constructor(
        /* [in] */ Int32 m,
        /* [in] */ IBigInteger *rp);

    CARAPI constructor(
        /* [in] */ Int32 m,
        /* [in] */ ArrayOf<Int32> *ks);

private:
    // Mid terms array length for trinomial basis
    static const Int32 TPB_MID_LEN = 1;
    // Mid terms array length for pentanomial basis
    static const Int32 PPB_MID_LEN = 3;
    // All terms number for trinomial basis
    static const Int32 TPB_LEN = 3;
    // All terms number for pentanomial basis
    static const Int32 PPB_LEN = 5;
    // m value
    Int32 mM;
    // Reduction polynomial
    AutoPtr<IBigInteger> mRp;
    // Mid term(s) of reduction polynomial
    AutoPtr<ArrayOf<Int32> > mKs;
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_CECFIELDF2M_H__
