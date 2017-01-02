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

#ifndef __ELASTOS_SECURITY_SPEC_CRSAKEYGENPARAMETERSPEC_H__
#define __ELASTOS_SECURITY_SPEC_CRSAKEYGENPARAMETERSPEC_H__

#include "_Elastos_Security_Spec_CRSAKeyGenParameterSpec.h"
#include "core/Object.h"

using Elastos::Math::IBigInteger;

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CRSAKeyGenParameterSpec)
    , public Object
    , public IRSAKeyGenParameterSpec
    , public IAlgorithmParameterSpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    static CARAPI_(const AutoPtr<IBigInteger>) InitStatic(
        /* [in] */ Int64 bi);

    CARAPI GetKeysize(
        /* [out] */ Int32 *keySize);

    CARAPI GetPublicExponent(
        /* [out] */ IBigInteger **pubExponent);

    CARAPI constructor(
        /* [in] */ Int32 keysize,
        /* [in] */ IBigInteger *publicExponent);

public:
    /**
     * The value of the public exponent {@code F0} = 3.
     */
    static const AutoPtr<IBigInteger> F0;

    /**
     * The value of the public exponent {@code F4} = 65537.
     */
    static const AutoPtr<IBigInteger> F4;

private:
    // Key size
    Int32 mKeysize;
    // Public Exponent
    AutoPtr<IBigInteger> mPublicExponent;
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_CRSAKEYGENPARAMETERSPEC_H__
