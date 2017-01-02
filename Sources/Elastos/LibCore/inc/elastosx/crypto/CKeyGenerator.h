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

#ifndef __ELASTOSX_CRYPTO_CKEYGENERATOR_H__
#define __ELASTOSX_CRYPTO_CKEYGENERATOR_H__

#include "Elastos.CoreLibrary.Security.h"
#include "_Elastosx_Crypto_CKeyGenerator.h"
#include <elastos/core/Object.h>

using Elastos::Security::IProvider;
using Elastos::Security::ISecureRandom;
using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Org::Apache::Harmony::Security::Fortress::IEngine;

namespace Elastosx {
namespace Crypto {

CarClass(CKeyGenerator)
    , public Object
    , public IKeyGenerator
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CKeyGenerator();

    /**
     * Creates a new {@code KeyGenerator} instance.
     *
     * @param keyGenSpi
     *            the implementation delegate.
     * @param provider
     *            the implementation provider.
     * @param algorithm
     *            the name of the algorithm.
     */
    CARAPI constructor(
        /* [in] */ IKeyGeneratorSpi* keyGenSpi,
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
     * Returns the provider for this {@code KeyGenerator} instance.
     *
     * @return the provider for this {@code KeyGenerator} instance.
     */
    CARAPI GetProvider(
        /* [out] */ IProvider ** provider);

    /**
     * Generates a secret key.
     *
     * @return the generated secret key.
     */
    CARAPI GenerateKey(
        /* [out] */ ISecretKey** skey);

    /**
     * Initializes this {@code KeyGenerator} instance with the specified
     * algorithm parameters.
     *
     * @param params
     *            the parameters for the key generation algorithm.
     * @throws InvalidAlgorithmParameterException
     *             if the parameters cannot be used to initialize this key
     *             generator algorithm.
     */
    CARAPI Init(
        /* [in] */ IAlgorithmParameterSpec * params);

    /**
     * Initializes this {@code KeyGenerator} instance with the specified
     * algorithm parameters and randomness source.
     *
     * @param params
     *            the parameters for the key generation algorithm.
     * @param random
     *            the randomness source for any random bytes.
     * @throws InvalidAlgorithmParameterException
     *             if the parameters cannot be uses to initialize this key
     *             generator algorithm.
     */
    CARAPI Init(
        /* [in] */ IAlgorithmParameterSpec * params,
        /* [in] */ ISecureRandom * random);

    /**
     * Initializes this {@code KeyGenerator} instance for the specified key size
     * (in bits).
     *
     * @param keysize
     *            the size of the key (in bits).
     */
    CARAPI Init(
        /* [in] */ Int32 keysize);

    /**
     * Initializes this {@code KeyGenerator} instance for the specified key size
     * (in bits) using the specified randomness source.
     *
     * @param keysize
     *            the size of the key (in bits).
     * @param random
     *            the randomness source for any random bytes.
     */
    CARAPI Init(
        /* [in] */ Int32 keysize,
        /* [in] */ ISecureRandom * random);

    /**
     * Initializes this {@code KeyGenerator} instance for the specified key size
     * (in bits) using the specified randomness source.
     *
     * @param keysize
     *            the size of the key (in bits).
     * @param random
     *            the randomness source for any random bytes.
     */
    CARAPI Init(
        /* [in] */ ISecureRandom * random);

    /**
     * Creates a new {@code KeyGenerator} instance that provides the specified
     * key algorithm,
     *
     * @param algorithm
     *            the name of the requested key algorithm
     * @return the new {@code KeyGenerator} instance.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not available by any provider.
     * @throws NullPointerException
     *             if {@code algorithm} is {@code null}.
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ IKeyGenerator ** ka);

    /**
     * Creates a new {@code KeyGenerator} instance that provides the specified
     * key algorithm from the specified provider.
     *
     * @param algorithm
     *            the name of the requested key algorithm.
     * @param provider
     *            the name of the provider that is providing the algorithm.
     * @return the new {@code KeyGenerator} instance.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not provided by the specified
     *             provider.
     * @throws NoSuchProviderException
     *             if the specified provider is not available.
     * @throws IllegalArgumentException
     *             if the specified provider is name is {@code null} or empty.
     * @throws NullPointerException
     *             if the specified algorithm name is {@code null}.
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ IKeyGenerator ** ka);

    /**
     * Creates a new {@code KeyGenerator} instance that provides the specified
     * key algorithm from the specified provider. The {@code provider}
     * supplied does not have to be registered.
     *
     * @param algorithm
     *            the name of the requested key algorithm.
     * @param provider
     *            the provider that is providing the algorithm
     * @return the new {@code KeyGenerator} instance.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not provided by the specified
     *             provider.
     * @throws IllegalArgumentException
     *             if the specified provider is {@code null}.
     * @throws NullPointerException
     *             if the specified algorithm name is {@code null}.
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider * provider,
        /* [out] */ IKeyGenerator ** ka);

private:
    // Used to access common engine functionality
    static AutoPtr<IEngine> ENGINE; // = new Engine("KeyGenerator");

    // Store SecureRandom
    static AutoPtr<ISecureRandom> RANDOM; // = new SecureRandom();

    // Store used provider
    AutoPtr<IProvider> mProvider;

    // Store used spi implementation
    AutoPtr<IKeyGeneratorSpi> mSpiImpl;

    // Store used algorithm name
    String mAlgorithm;

};

}
}

#endif // __ELASTOSX_CRYPTO_CKEYGENERATOR_H__
