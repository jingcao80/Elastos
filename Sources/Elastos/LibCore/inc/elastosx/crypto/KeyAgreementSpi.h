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

#ifndef __ELASTOSX_CRYPTO_KEYAGREEMENTSPI_H__
#define __ELASTOSX_CRYPTO_KEYAGREEMENTSPI_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Security::IKey;
using Elastos::Security::ISecureRandom;
using Elastos::Security::Spec::IAlgorithmParameterSpec;

namespace Elastosx {
namespace Crypto {

class ECO_PUBLIC KeyAgreementSpi
    : public Object
    , public IKeyAgreementSpi
{
public:
    CAR_INTERFACE_DECL()

    KeyAgreementSpi();

    virtual ~KeyAgreementSpi();

    /**
     * Creates a new {@code KeyAgreementSpi} instance.
     */
    CARAPI constructor();

    /**
     * Does the next (or the last) phase of the key agreement, using the
     * specified key.
     *
     * @param key
     *            the key received from the other party for this phase.
     * @param lastPhase
     *            set to {@code true} if this is the last phase of this key
     *            agreement.
     * @return the intermediate key from this phase or null if there is no
     *         intermediate key for this phase.
     * @throws InvalidKeyException
     *             if the specified key cannot be used in this key agreement or
     *             this phase,
     * @throws IllegalStateException
     *             if this instance has not been initialized.
     */
    virtual CARAPI EngineDoPhase(
        /* [in] */ IKey* key,
        /* [in] */ Boolean lastPhase,
        /* [out] */ IKey** result) = 0;

    /**
     * Generates the shared secret.
     *
     * @return the generated shared secret.
     * @throws IllegalStateException
     *             if this key agreement is not complete.
     */
    virtual CARAPI EngineGenerateSecret(
        /* [out] */ ArrayOf<Byte> ** result) = 0;

    /**
     * Generates the shared secret and stores it into the buffer {@code
     * sharedSecred} at {@code offset}.
     *
     * @param sharedSecret
     *            the buffer to store the shared secret.
     * @param offset
     *            the offset in the buffer.
     * @return the number of bytes stored in the buffer.
     * @throws IllegalStateException
     *             if this key agreement is not complete.
     * @throws ShortBufferException
     *             if the specified buffer is too small for the shared secret.
     */
    virtual CARAPI EngineGenerateSecret(
        /* [in] */ ArrayOf<Byte> * sharedSecret,
        /* [in] */ Int32 offset,
        /* [out] */ Int32* result) = 0;

    /**
     * Generates the shared secret.
     *
     * @param algorithm
     *            the algorithm to for the {@code SecretKey}
     * @return the shared secret as a {@code SecretKey} of the specified
     *         algorithm.
     * @throws IllegalStateException
     *             if this key agreement is not complete.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm for the secret key does not
     *             exists.
     * @throws InvalidKeyException
     *             if a {@code SecretKey} with the specified algorithm cannot be
     *             created using the generated shared secret.
     */
    virtual CARAPI EngineGenerateSecret(
        /* [in] */ const String& algorithm,
        /* [out] */ ISecretKey** skey) = 0;

    /**
     * Initializes this {@code KeyAgreementSpi} with the specified key and the
     * specified randomness source.
     *
     * @param key
     *            the key to initialize this key agreement.
     * @param random
     *            the source for any randomness needed.
     * @throws InvalidKeyException
     *             if the specified key cannot be used to initialize this key
     *             agreement.
     */
    virtual CARAPI EngineInit(
        /* [in] */ IKey* key,
        /* [in] */ ISecureRandom* random) = 0;

    /**
     * Initializes this {@code KeyAgreementSpi} with the specified key,
     * algorithm parameters and randomness source.
     *
     * @param key
     *            the key to initialize this key agreement.
     * @param params
     *            the parameters for this key agreement algorithm.
     * @param random
     *            the source for any randomness needed.
     * @throws InvalidKeyException
     *             if the specified key cannot be used to initialize this key
     *             agreement.
     * @throws InvalidAlgorithmParameterException
     *             if the specified parameters are invalid for this key
     *             agreement algorithm.
     */
    virtual CARAPI EngineInit(
        /* [in] */ IKey* key,
        /* [in] */ IAlgorithmParameterSpec* params,
        /* [in] */ ISecureRandom* random) = 0;
};

}
}

#endif // __ELASTOSX_CRYPTO_KEYAGREEMENTSPI_H__
