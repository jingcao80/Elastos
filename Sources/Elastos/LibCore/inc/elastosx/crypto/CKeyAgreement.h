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

#ifndef __ELASTOSX_CRYPTO_CKEYAGREEMENT_H__
#define __ELASTOSX_CRYPTO_CKEYAGREEMENT_H__

#include "Elastos.CoreLibrary.Security.h"
#include "_Elastosx_Crypto_CKeyAgreement.h"
#include <elastos/core/Object.h>

using Elastos::Security::IKey;
using Elastos::Security::IProvider;
using Elastos::Security::IProviderService;
using Elastos::Security::ISecureRandom;
using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Org::Apache::Harmony::Security::Fortress::IEngine;
using Org::Apache::Harmony::Security::Fortress::ISpiAndProvider;

namespace Elastosx {
namespace Crypto {

CarClass(CKeyAgreement)
    , public Object
    , public IKeyAgreement
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CKeyAgreement();

    /**
     * Creates a new {@code KeyAgreement} instance.
     *
     * @param keyAgreeSpi
     *            the <b>SPI</b> delegate.
     * @param provider
     *            the provider providing this KeyAgreement.
     * @param algorithm
     *            the name of the key agreement algorithm.
     */
    CARAPI constructor(
        /* [in] */ IKeyAgreementSpi* keyAgreeSpi,
        /* [in] */ IProvider* provider,
        /* [in] */ const String& algorithm);

    /**
     * Returns the name of the key agreement algorithm.
     *
     * @return the name of the key agreement algorithm.
     */
    CARAPI GetAlgorithm(
        /* [out] */ String * name);

    /**
     * Returns the provider for this {@code KeyAgreement} instance.
     *
     * @return the provider for this {@code KeyAgreement} instance.
     */
    CARAPI GetProvider(
        /* [out] */ IProvider ** provider);

    /**
     * Initializes this {@code KeyAgreement} with the specified key.
     *
     * @param key the key to initialize this key agreement.
     * @throws InvalidKeyException if the specified key cannot be used to
     *             initialize this key agreement.
     */
    CARAPI Init(
        /* [in] */ IKey * key);

    /**
     * Initializes this {@code KeyAgreement} with the specified key and the
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
    CARAPI Init(
        /* [in] */ IKey * key,
        /* [in] */ ISecureRandom * random);

    /**
     * Initializes this {@code KeyAgreement} with the specified key and the
     * algorithm parameters.
     *
     * @param key
     *            the key to initialize this key agreement.
     * @param params
     *            the parameters for this key agreement algorithm.
     * @throws InvalidKeyException
     *             if the specified key cannot be used to initialize this key
     *             agreement.
     * @throws InvalidAlgorithmParameterException
     *             if the specified parameters are invalid for this key
     *             agreement algorithm.
     */
    CARAPI Init(
        /* [in] */ IKey * key,
        /* [in] */ IAlgorithmParameterSpec * random);

    /**
     * Initializes this {@code KeyAgreement} with the specified key, algorithm
     * parameters and randomness source.
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
    CARAPI Init(
        /* [in] */ IKey * key,
        /* [in] */ IAlgorithmParameterSpec * random,
        /* [in] */ ISecureRandom * sr);

    /**
     * Does the next (or the last) phase of the key agreement, using the
     * specified key.
     *
     * @param key
     *            the key received from the other party for this phase.
     * @param lastPhase
     *            set to {@code true} if this is the last phase of this key
     *            agreement.
     * @return the intermediate key from this phase or {@code null} if there is
     *         no intermediate key for this phase.
     * @throws InvalidKeyException
     *             if the specified key cannot be used in this key agreement or
     *             this phase,
     * @throws IllegalStateException
     *             if this instance has not been initialized.
     */
    CARAPI DoPhase(
        /* [in] */ IKey * key,
        /* [in] */ Boolean lastPhase,
        /* [out] */ IKey ** result);

    /**
     * Generates the shared secret.
     *
     * @return the generated shared secret.
     * @throws IllegalStateException
     *             if this key agreement is not complete.
     */
    CARAPI GenerateSecret(
        /* [out, callee] */ ArrayOf<Byte> ** secret);

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
    CARAPI GenerateSecret(
        /* [in] */ ArrayOf<Byte> * sharedSecret,
        /* [in] */ Int32 offset,
        /* [out] */ Int32 * number);

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
    CARAPI GenerateSecret(
        /* [in] */ const String& algorithm,
        /* [out] */ ISecretKey ** key);

    /**
     * Creates a new {@code KeyAgreement} for the specified algorithm.
     *
     * @param algorithm
     *            the name of the key agreement algorithm to create.
     * @return a key agreement for the specified algorithm.
     * @throws NoSuchAlgorithmException
     *             if no installed provider can provide the requested algorithm.
     * @throws NullPointerException
     *             if the specified algorithm is {@code null}.
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ IKeyAgreement ** keyAgreement);

    /**
     * Creates a new {@code KeyAgreement} for the specified algorithm from the
     * specified provider.
     *
     * @param algorithm
     *            the name of the key agreement algorithm to create.
     * @param provider
     *            the name of the provider that provides the requested
     *            algorithm.
     * @return a key agreement for the specified algorithm from the specified
     *         provider.
     * @throws NoSuchAlgorithmException
     *             if the specified provider cannot provide the requested
     *             algorithm.
     * @throws NoSuchProviderException
     *             if the specified provider does not exist.
     * @throws IllegalArgumentException
     *             if the specified provider name is {@code null} or empty.
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ IKeyAgreement ** keyAgreement);

    /**
     * Create a new {@code KeyAgreement} for the specified algorithm from the
     * specified provider. The {@code provider} supplied does not have to be
     * registered.
     *
     * @param algorithm
     *            the name of the key agreement algorithm to create.
     * @param provider
     *            the provider that provides the requested algorithm.
     * @return a key agreement for the specified algorithm from the specified
     *         provider.
     * @throws NoSuchAlgorithmException
     *             if the specified provider cannot provide the requested
     *             algorithm.
     * @throws IllegalArgumentException
     *             if the specified provider is {@code null}.
     * @throws NullPointerException
     *             if the specified algorithm name is {@code null}.
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider * provider,
        /* [out] */ IKeyAgreement ** keyAgreement);

private:
    static CARAPI GetKeyAgreement(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider * provider,
        /* [out] */ IKeyAgreement ** keyAgreement);

    static CARAPI_(AutoPtr<ISpiAndProvider>) TryAlgorithm(
        /* [in] */ IKey* key,
        /* [in] */ IProvider * provider,
        /* [in] */ const String& algorithm);

    static CARAPI_(AutoPtr<ISpiAndProvider>) TryAlgorithmWithProvider(
        /* [in] */ IKey* key,
        /* [in] */ IProviderService* service);

    /**
     * Makes sure a KeyAgreementSpi that matches this type is selected.
     */
    CARAPI GetSpi(
        /* [in] */ IKey* key,
        /* [out] */ IKeyAgreementSpi ** spi);

    /**
     * Convenience call when the Key is not available.
     */
    CARAPI GetSpi(
        /* [out] */ IKeyAgreementSpi ** spi);

private:
    // The service name.
    static String SERVICE; // = "KeyAgreement";

    // Used to access common engine functionality
    static AutoPtr<IEngine> ENGINE; // = new Engine(SERVICE);

    // Store SecureRandom
    static AutoPtr<ISecureRandom> RANDOM; // = new SecureRandom();

    // Store used provider
    AutoPtr<IProvider> mProvider;

    // Provider that was requested during creation.
    AutoPtr<IProvider> mSpecifiedProvider;

    // Store used spi implementation
    AutoPtr<IKeyAgreementSpi> mSpiImpl;

    // Store used algorithm name
    String mAlgorithm;

    /**
     * Lock held while the SPI is initializing.
     */
    Object mInitLock; // = new Object();
};

}
}

#endif // __ELASTOSX_CRYPTO_CKEYAGREEMENT_H__
