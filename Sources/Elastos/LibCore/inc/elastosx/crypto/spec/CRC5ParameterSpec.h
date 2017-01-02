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

#ifndef __ELASTOSX_CRYPTO_SPEC_CRC5PARAMETERSPEC_H__
#define __ELASTOSX_CRYPTO_SPEC_CRC5PARAMETERSPEC_H__

#include "_Elastosx_Crypto_Spec_CRC5ParameterSpec.h"
#include <elastos/core/Object.h>

using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastos::Security::Spec::EIID_IAlgorithmParameterSpec;

namespace Elastosx {
namespace Crypto {
namespace Spec {

CarClass(CRC5ParameterSpec)
    , public Object
    , public IRC5ParameterSpec
    , public IAlgorithmParameterSpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CRC5ParameterSpec();

    /**
     * Creates a new <code>RC5ParameterSpec</code> instance with the specified
     * version, round count an word size (in bits).
     *
     * @param version
     *            the version.
     * @param rounds
     *            the round count.
     * @param wordSize
     *            the word size (in bits).
     */
    CARAPI constructor(
        /* [in] */ Int32 version,
        /* [in] */ Int32 rounds,
        /* [in] */ Int32 wordSize);

    /**
     * Creates a new <code>RC5ParameterSpec</code> instance with the specified
     * version, round count, word size (in bits) and an <i>initialization
     * vector</i>.
     * <p>
     * The size of the <i>initialization vector</i> must be at least
     * <code>2 * (wordSize / 8)</code> bytes which are copied to protect them
     * against modification.
     *
     * @param version
     *            the version.
     * @param rounds
     *            the round count.
     * @param wordSize
     *            the word size (in bits).
     * @param iv
     *            the initialization vector.
     * @throws IllegalArgumentException
     *             if the initialization vector is null or shorter than <code>2
     *             * (wordSize / 8)</code>.
     */
    CARAPI constructor(
        /* [in] */ Int32 version,
        /* [in] */ Int32 rounds,
        /* [in] */ Int32 wordSize,
        /* [in] */ ArrayOf<Byte> * iv);

    /**
     * Creates a new <code>RC5ParameterSpec</code> instance with the specified
     * version, round count, wordSize (in bits), an <i>initialization vector</i>
     * and an offset.
     * <p>
     * The size of the <i>initialization vector</i> must be at least
     * <code>offset + (2 * (wordSize / 8))</code> bytes. The bytes starting at
     * <code>offset</code> are copied to protect them against modification.
     *
     * @param version
     *            the version.
     * @param rounds
     *            the round count.
     * @param wordSize
     *            the word size (in bits).
     * @param iv
     *            the initialization vector.
     * @param offset
     *            the offset in the initialization vector.
     * @throws IllegalArgumentException
     *             if the initialization vector is null of shorter than
     *             <code>offset + (2 * (wordSize / 8))</code>.
     * @throws ArrayIndexOutOfBoundsException
     *             if <code>offset</code> is negative.
     */
    CARAPI constructor(
        /* [in] */ Int32 version,
        /* [in] */ Int32 rounds,
        /* [in] */ Int32 wordSize,
        /* [in] */ ArrayOf<Byte> * iv,
        /* [in] */ Int32 offset);

    /**
     * Returns the version.
     *
     * @return the version.
     */
    CARAPI GetVersion(
        /* [out] */ Int32 * result);

    /**
     * Returns the round count.
     *
     * @return the round count.
     */
    CARAPI GetRounds(
        /* [out] */ Int32 * result);

    /**
     * Returns the word size (in bits).
     *
     * @return the word size (in bits).
     */
    CARAPI GetWordSize(
        /* [out] */ Int32 * result);

    /**
     * Returns the Initial Vector (IV) used by this parameter spec.
     */
    CARAPI GetIV(
        /* [out, callee] */ ArrayOf<Byte> ** result);

    /**
     * Returns the hash code of this <code>RC5ParameterSpec</code> instance.
     *
     * @return the hash code.
     */
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    /**
     * Compares the specified object with this <code>RC5ParameterSpec</code>
     * instance.
     *
     * @param obj
     *            the object to compare.
     * @return true if version, round count, word size and initializaion vector
     *         of both objects are equal, otherwise false.
     */
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

private:
    Int32 mVersion;
    Int32 mRounds;
    Int32 mWordSize;
    AutoPtr<ArrayOf<Byte> > mIv;
};

}
}
}

#endif // __ELASTOSX_CRYPTO_SPEC_CRC5PARAMETERSPEC_H__
