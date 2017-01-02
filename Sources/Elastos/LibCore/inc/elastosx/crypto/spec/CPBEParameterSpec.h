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

#ifndef __ELASTOSX_CRYPTO_SPEC_CPBEPARAMETERSPEC_H__
#define __ELASTOSX_CRYPTO_SPEC_CPBEPARAMETERSPEC_H__

#include "_Elastosx_Crypto_Spec_CPBEParameterSpec.h"
#include <elastos/core/Object.h>

using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastos::Security::Spec::EIID_IAlgorithmParameterSpec;

namespace Elastosx {
namespace Crypto {
namespace Spec {

CarClass(CPBEParameterSpec)
    , public Object
    , public IPBEParameterSpec
    , public IAlgorithmParameterSpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CPBEParameterSpec();

    /**
     * Creates a new <code>PBEParameterSpec</code> with the specified salt and
     * iteration count.
     *
     * @param salt
     *            the salt.
     * @param iterationCount
     *            the iteration count.
     * @throws NullPointerException
     *             if salt is null.
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> * salt,
        /* [in] */ Int32 iterationCount);

    /**
     * Returns a copy to the salt.
     *
     * @return a copy to the salt.
     */
    CARAPI GetSalt(
        /* [out, callee] */ ArrayOf<Byte> ** result);

    /**
     * Returns the iteration count.
     *
     * @return the iteration count.
     */
    CARAPI GetIterationCount(
        /* [out] */ Int32 * result);

private:
    Int32 mIterationCount;
    AutoPtr<ArrayOf<Byte> > mSalt;
};

}
}
}

#endif // __ELASTOSX_CRYPTO_SPEC_CPBEPARAMETERSPEC_H__
