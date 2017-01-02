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

#ifndef __ELASTOSX_CRYPTO_SPEC_CRC2PARAMETERSPEC_H__
#define __ELASTOSX_CRYPTO_SPEC_CRC2PARAMETERSPEC_H__

#include "_Elastosx_Crypto_Spec_CRC2ParameterSpec.h"
#include <elastos/core/Object.h>

using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastos::Security::Spec::EIID_IAlgorithmParameterSpec;

namespace Elastosx {
namespace Crypto {
namespace Spec {

CarClass(CRC2ParameterSpec)
    , public Object
    , public IRC2ParameterSpec
    , public IAlgorithmParameterSpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CRC2ParameterSpec();

    /**
     * Creates a new <code>RC2ParameterSpec</code> instance with the specified
     * effective key length (in bits),
     *
     * @param effectiveKeyBits
     *            the effective key length (in bits).
     */
    CARAPI constructor(
        /* [in] */ Int32 effectiveKeyBits);

    /**
     * Creates a new <code>RC2ParameterSpec</code> instance with the specified
     * effective key length (in bits) and <i>initialization vector</i>.
     * <p>
     * The size of the <i>initialization vector</i> must be at least 8 bytes
     * which are copied to protect them against modification.
     *
     * @param effectiveKeyBits
     *            the effective key length (in bits).
     * @param iv
     *            the initialization vector.
     * @throws IllegalArgumentException
     *             if the initialization vector is null or shorter than 8 bytes.
     */
    CARAPI constructor(
        /* [in] */ Int32 effectiveKeyBits,
        /* [in] */ ArrayOf<Byte> * iv);

    /**
     * Creates a new <code>RC2ParameterSpec</code> instance with the specified
     * effective key length (in bits) and <i>initialization vector<i>.
     * <p>
     * The size of the <i>initialization vector</i> starting at
     * <code>offset</code> must be at least 8 bytes which are copied to protect
     * them against modification.
     *
     * @param effectiveKeyBits
     *            the effective key length (in bits).
     * @param iv
     *            the initialization vector.
     * @param offset
     *            the offset in the initialization vector to start at.
     * @throws IllegalArgumentException
     *             if the initialization vector is null or starting at
     *             <code>offset</code> is shorter than 8 bytes.
     */
    CARAPI constructor(
        /* [in] */ Int32 effectiveKeyBits,
        /* [in] */ ArrayOf<Byte> * iv,
        /* [in] */ Int32 offset);

    /**
     * Returns the size of the tag in bits.
     */
    CARAPI GetEffectiveKeyBits(
        /* [out] */ Int32 * result);

    /**
     * Returns the Initial Vector (IV) used by this parameter spec.
     */
    CARAPI GetIV(
        /* [out, callee] */ ArrayOf<Byte> ** result);

    /**
     * Returns the hash code of this <code>RC2ParameterSpec</code> instance.
     *
     * @return the hash code.
     */
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    /**
     * Compares the specified object to this <code>RC2ParameterSpec</code>
     * instance.
     *
     * @param obj
     *            the object to compare.
     * @return true if the effective key length and the initialization vector of
     *         both objects are equal, otherwise false.
     */
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

private:
    Int32 mEffectiveKeyBits;
    AutoPtr<ArrayOf<Byte> > mIv;
};

}
}
}

#endif // __ELASTOSX_CRYPTO_SPEC_CRC2PARAMETERSPEC_H__
