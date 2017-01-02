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

#ifndef __ELASTOSX_CRYPTO_SPEC_CDHGENPARAMETERSPEC_H__
#define __ELASTOSX_CRYPTO_SPEC_CDHGENPARAMETERSPEC_H__

#include "_Elastosx_Crypto_Spec_CDHGenParameterSpec.h"
#include <elastos/core/Object.h>

using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastos::Security::Spec::EIID_IAlgorithmParameterSpec;

namespace Elastosx {
namespace Crypto {
namespace Spec {

CarClass(CDHGenParameterSpec)
    , public Object
    , public IDHGenParameterSpec
    , public IAlgorithmParameterSpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CDHGenParameterSpec();

    /**
     * Creates a new <code>DHGenParameterSpec</code> instance with the specified
     * parameters.
     *
     * @param primeSize
     *            the size of the <i>prime modulus</i> in bits.
     * @param exponentSize
     *            the size of the <i>random exponent</i> in bits.
     */
    CARAPI constructor(
        /* [in] */ Int32 primeSize,
        /* [in] */ Int32 exponentSize);

    /**
     * Returns the size of the <i>prime modulus</i> in bits.
     *
     * @return the size of the prime modulus in bits.
     */
    CARAPI GetPrimeSize(
        /* [out] */ Int32 * result);

    /**
     * Returns the size of the <i>random exponent</i> in bits.
     *
     * @return the size of the random exponent in bits.
     */
    CARAPI GetExponentSize(
        /* [out] */ Int32 * result);


private:
    Int32 mPrimeSize;
    Int32 mExponentSize;
};

}
}
}

#endif // __ELASTOSX_CRYPTO_SPEC_CDHGENPARAMETERSPEC_H__
