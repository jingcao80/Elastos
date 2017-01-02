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

#ifndef __ELASTOSX_CRYPTO_CSECRETKEYFACTORY_H__
#define __ELASTOSX_CRYPTO_CSECRETKEYFACTORY_H__

#include "Elastos.CoreLibrary.Security.h"
#include "_Elastosx_Crypto_CSecretKeyFactory.h"
#include <elastos/core/Object.h>

using Elastos::Security::IProvider;
using Elastos::Security::Spec::IKeySpec;
using Org::Apache::Harmony::Security::Fortress::IEngine;

namespace Elastosx {
namespace Crypto {

CarClass(CSecretKeyFactory)
    , public Object
    , public ISecretKeyFactory
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CSecretKeyFactory();

    /**
     * Creates a new {@code SecretKeyFactory}
     *
     * @param keyFacSpi
     *            the SPI delegate.
     * @param provider
     *            the provider providing this key factory.
     * @param algorithm
     *            the algorithm name for the secret key.
     */
    CARAPI constructor(
        /* [in] */ ISecretKeyFactorySpi* secretKeyFactorySpi,
        /* [in] */ IProvider* provider,
        /* [in] */ const String& algorithm);

    /**
     * Returns the name of the secret key algorithm.
     *
     * @return the name of the secret key algorithm.
     */
    CARAPI GetAlgorithm(
        /* [out] */ String * name);

    /**
     * Returns the provider for this {@code SecretKeyFactory} instance.
     *
     * @return the provider for this {@code SecretKeyFactory} instance.
     */
    CARAPI GetProvider(
        /* [out] */ IProvider ** provider);

    /**
     * Generate a secret key from the specified key specification.
     *
     * @param keySpec
     *            the key specification.
     * @return a secret key.
     * @throws InvalidKeySpecException
     *             if the specified key specification cannot be used to generate
     *             a secret key.
     */
    CARAPI GenerateSecret(
        /* [in] */ IKeySpec * keySpec,
        /* [out] */ ISecretKey ** key);

    /**
     * Returns the key specification of the specified secret key.
     *
     * @param key
     *            the secret key to get the specification from.
     * @param keySpec
     *            the target key specification class.
     * @return an instance of the specified key specification class.
     * @throws InvalidKeySpecException
     *             if the specified secret key cannot be transformed into the
     *             requested key specification.
     */
    //@SuppressWarnings("unchecked")
    CARAPI GetKeySpec(
        /* [in] */ ISecretKey * key,
        /* [in] */ const ClassID& keySpec,
        /* [out] */ IKeySpec ** spec);

    /**
     * Translates the specified secret key into an instance of the corresponding
     * key from the provider of this key factory.
     *
     * @param key
     *            the secret key to translate.
     * @return the corresponding translated key.
     * @throws InvalidKeyException
     *             if the specified key cannot be translated using this key
     *             factory.
     */
    CARAPI TranslateKey(
        /* [in] */ ISecretKey * key,
        /* [out] */ ISecretKey ** out);

    /**
     * Creates a new {@code SecretKeyFactory} instance for the specified key
     * algorithm.
     *
     * @param algorithm
     *            the name of the key algorithm.
     * @return a secret key factory for the specified key algorithm.
     * @throws NoSuchAlgorithmException
     *             if no installed provider can provide the requested algorithm.
     * @throws NullPointerException
     *             if the specified algorithm is {@code null}.
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ ISecretKeyFactory ** secretKeyFactory);

    /**
     * Creates a new {@code SecretKeyFactory} instance for the specified key
     * algorithm from the specified {@code provider}.
     *
     * @param algorithm
     *            the name of the key algorithm.
     * @param provider
     *            the name of the provider that provides the requested
     *            algorithm.
     * @return a secret key factory for the specified key algorithm from the
     *         specified provider.
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
        /* [out] */ ISecretKeyFactory ** SecretKeyFactory);

    /**
     * Creates a new {@code SecretKeyFactory} instance for the specified key
     * algorithm from the specified provider. The {@code provider} supplied
     * does not have to be registered.
     *
     * @param algorithm
     *            the name of the key algorithm.
     * @param provider
     *            the provider that provides the requested algorithm.
     * @return a secret key factory for the specified key algorithm from the
     *         specified provider.
     * @throws NoSuchAlgorithmException
     *             if the specified provider cannot provider the requested
     *             algorithm.
     * @throws IllegalArgumentException
     *             if the specified provider is {@code null}.
     * @throws NullPointerException
     *             is the specified algorithm name is {@code null}.
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider * provider,
        /* [out] */ ISecretKeyFactory ** secretKeyFactory);

private:
    // Used to access common engine functionality
    static AutoPtr<IEngine> ENGINE; // = new Engine("SecretKeyFactory");

    // Store used provider
    AutoPtr<IProvider> mProvider;

    // Store used spi implementation
    AutoPtr<ISecretKeyFactorySpi> mSpiImpl;

    // Store used algorithm name
    String mAlgorithm;
};

}
}

#endif // __ELASTOSX_CRYPTO_CSECRETKEYFACTORY_H__
