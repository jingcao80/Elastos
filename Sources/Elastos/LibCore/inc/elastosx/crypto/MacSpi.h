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

#ifndef __ELASTOSX_CRYPTO_MACSPI_H__
#define __ELASTOSX_CRYPTO_MACSPI_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IBuffer;
using Elastos::Security::IKey;
using Elastos::Security::Spec::IAlgorithmParameterSpec;

namespace Elastosx {
namespace Crypto {

class CMac;

class ECO_PUBLIC MacSpi
    : public Object
    , public IMacSpi
{
public:
    CAR_INTERFACE_DECL()

    MacSpi();

    virtual ~MacSpi();

    /**
     * Creates a new {@code MacSpi} instance.
     */
    CARAPI constructor();

    /**
     * Returns the length of this MAC (in bytes).
     *
     * @return the length of this MAC (in bytes).
     */
    virtual CARAPI EngineGetMacLength(
        /* [out] */ Int32 * result) = 0;

    /**
     * Initializes this {@code MacSpi} instance with the specified key and
     * algorithm parameters.
     *
     * @param key
     *            the key to initialize this algorithm.
     * @param params
     *            the parameters for this algorithm.
     * @throws InvalidKeyException
     *             if the specified key cannot be used to initialize this
     *             algorithm, or it is {@code null}.
     * @throws InvalidAlgorithmParameterException
     *             if the specified parameters cannot be used to initialize this
     *             algorithm.
     */
    virtual CARAPI EngineInit(
        /* [in] */ IKey * key,
        /* [in] */ IAlgorithmParameterSpec * params) = 0;

    /**
     * Updates this {@code MacSpi} instance with the specified byte.
     *
     * @param input
     *            the byte.
     */
    virtual CARAPI EngineUpdate(
        /* [in] */ Byte input) = 0;

    /**
     * Updates this {@code MacSpi} instance with the data from the specified
     * buffer {@code input} from the specified {@code offset} and length {@code
     * len}.
     *
     * @param input
     *            the buffer.
     * @param offset
     *            the offset in the buffer.
     * @param len
     *            the length of the data in the buffer.
     */
    virtual CARAPI EngineUpdate(
        /* [in] */ ArrayOf<Byte> * input,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len) = 0;

    /**
     * Computes the digest of this MAC based on the data previously specified in
     * {@link #engineUpdate} calls.
     * <p>
     * This {@code MacSpi} instance is reverted to its initial state and
     * can be used to start the next MAC computation with the same parameters or
     * initialized with different parameters.
     *
     * @return the generated digest.
     */
    virtual CARAPI EngineDoFinal(
        /* [out, callee] */ ArrayOf<Byte> ** input) = 0;

    /**
     * Resets this {@code MacSpi} instance to its initial state.
     * <p>
     * This {@code MacSpi} instance is reverted to its initial state and can be
     * used to start the next MAC computation with the same parameters or
     * initialized with different parameters.
     */
    virtual CARAPI EngineReset() = 0;

    CARAPI Clone(
        /* [out] */ IInterface** result);

protected:
    CARAPI EngineUpdate(
        /* [in] */ IByteBuffer * input);

    CARAPI CloneImpl(
        /* [in] */ IMacSpi* spi);

    friend class CMac;
};

}
}

#endif // __ELASTOSX_CRYPTO_MACSPI_H__
