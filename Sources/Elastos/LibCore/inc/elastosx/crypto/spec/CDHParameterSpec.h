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

#ifndef __ELASTOSX_CRYPTO_SPEC_CDHPARAMETERSPEC_H__
#define __ELASTOSX_CRYPTO_SPEC_CDHPARAMETERSPEC_H__

#include "_Elastosx_Crypto_Spec_CDHParameterSpec.h"
#include <elastos/core/Object.h>

using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastos::Security::Spec::EIID_IAlgorithmParameterSpec;
using Elastos::Math::IBigInteger;

namespace Elastosx {
namespace Crypto {
namespace Spec {

CarClass(CDHParameterSpec)
    , public Object
    , public IDHParameterSpec
    , public IAlgorithmParameterSpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CDHParameterSpec();

    /**
     * Creates a new <code>DHParameterSpec</code> instance with the specified
     * <i>prime modulus</i> and <i>base generator</i>.
     *
     * @param p
     *            the prime modulus.
     * @param g
     *            the base generator.
     */
    CARAPI constructor(
        /* [in] */ IBigInteger * p,
        /* [in] */ IBigInteger * g);

    /**
     * Creates a new <code>DHParameterSpec</code> instance with the specified
     * <i>prime modulus</i>, <i>base generator</i> and size (in bits) of the
     * <i>random exponent</i>.
     *
     * @param p
     *            the prime modulus.
     * @param g
     *            the base generator.
     * @param l
     *            the size of the random exponent (in bits).
     */
     CARAPI constructor(
        /* [in] */ IBigInteger * p,
        /* [in] */ IBigInteger * g,
        /* [in] */ Int32 l);

    /**
     * Returns the <i>prime modulus</i> of this parameter specification.
     *
     * @return the prime modulus.
     */
    CARAPI GetP(
        /* [out] */ IBigInteger ** bi);

    /**
     * Returns the <i>base generator</i> of this parameter specification.
     *
     * @return the base generator.
     */
    CARAPI GetG(
        /* [out] */ IBigInteger ** bi);

    /**
     * Returns the size (in bits) of the <i>random exponent</i>.
     *
     * @return the size (in bits) of the random exponent.
     */
    CARAPI GetL(
        /* [out] */ Int32 * result);


private:
    AutoPtr<IBigInteger> mP;
    AutoPtr<IBigInteger> mG;
    Int32 mL;
};

}
}
}

#endif // __ELASTOSX_CRYPTO_SPEC_CDHPARAMETERSPEC_H__
