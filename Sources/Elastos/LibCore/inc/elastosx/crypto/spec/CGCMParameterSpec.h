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

#ifndef __ELASTOSX_CRYPTO_SPEC_CGCMPARAMETERSPEC_H__
#define __ELASTOSX_CRYPTO_SPEC_CGCMPARAMETERSPEC_H__

#include "_Elastosx_Crypto_Spec_CGCMParameterSpec.h"
#include <elastos/core/Object.h>

using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastos::Security::Spec::EIID_IAlgorithmParameterSpec;

namespace Elastosx {
namespace Crypto {
namespace Spec {

CarClass(CGCMParameterSpec)
    , public Object
    , public IGCMParameterSpec
    , public IAlgorithmParameterSpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CGCMParameterSpec();

    /**
     * Creates a new {@code GCMParameterSpec} instance from the specified
     * Initial Vector (IV) from buffer {@code iv} and a tag length of
     * {@code tagLen} in bits.
     *
     * @throws IllegalArgumentException if the specified {@code iv} is null or
     *             {@code offset} and {@code byteCount} do not specify a valid
     *             chunk in the specified buffer.
     */
    CARAPI constructor(
        /* [in] */ Int32 tagLen,
        /* [in] */ ArrayOf<Byte> * iv);

    /**
     * Creates a new {@code GCMParameterSpec} instance with the Initial Vector
     * (IV) of {@code byteCount} bytes from the specified buffer {@code iv}
     * starting at {@code offset} and a tag length of {@code tagLen} in bits.
     *
     * @throws IllegalArgumentException if the specified {@code iv} is null or
     *             {@code offset} and {@code byteCount} do not specify a valid
     *             chunk in the specified buffer.
     * @throws ArrayIndexOutOfBoundsException if {@code offset} or
     *             {@code byteCount} are negative.
     */
    CARAPI constructor(
        /* [in] */ Int32 tagLen,
        /* [in] */ ArrayOf<Byte> * iv,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount);

    /**
     * Returns the size of the tag in bits.
     */
    CARAPI GetTLen(
        /* [out] */ Int32 * result);

    /**
     * Returns the Initial Vector (IV) used by this parameter spec.
     */
    CARAPI GetIV(
        /* [out, callee] */ ArrayOf<Byte> ** result);

private:
    Int32 mTagLen;
    AutoPtr<ArrayOf<Byte> > mIv;
};

}
}
}

#endif // __ELASTOSX_CRYPTO_SPEC_CGCMPARAMETERSPEC_H__
