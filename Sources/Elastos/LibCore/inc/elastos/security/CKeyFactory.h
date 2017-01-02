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

#ifndef __ELASTOS_SECURITY_CKEYFACTORY_H__
#define __ELASTOS_SECURITY_CKEYFACTORY_H__

#include "_Elastos_Security_CKeyFactory.h"
#include "Object.h"

using Elastos::Security::Spec::IKeySpec;
using Org::Apache::Harmony::Security::Fortress::IEngine;

namespace Elastos {
namespace Security {

CarClass(CKeyFactory)
    , public Object
    , public IKeyFactory
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Constructs a new instance of {@code KeyFactory} with the specified
     * arguments.
     *
     * @param keyFacSpi
     *            the concrete key factory service.
     * @param provider
     *            the provider.
     * @param algorithm
     *            the algorithm to use.
     */
    CARAPI constructor(
        /* [in] */ IKeyFactorySpi* keyFacSpi,
        /* [in] */ IProvider* provider,
        /* [in] */ const String& algorithm);

    /**
     * Returns a new instance of {@code KeyFactory} that utilizes the specified
     * algorithm.
     *
     * @param algorithm
     *            the name of the algorithm.
     * @return a new instance of {@code KeyFactory} that utilizes the specified
     *         algorithm.
     * @throws NoSuchAlgorithmException
     *             if no provider provides the requested algorithm.
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ IKeyFactory** result) /*throws NoSuchAlgorithmException*/;

    /**
     * Returns a new instance of {@code KeyFactory} that utilizes the specified
     * algorithm from the specified provider.
     *
     * @param algorithm
     *            the name of the algorithm.
     * @param provider
     *            the name of the provider.
     * @return a new instance of {@code KeyFactory} that utilizes the specified
     *         algorithm from the specified provider.
     * @throws NoSuchAlgorithmException
     *             if the provider does not provide the requested algorithm.
     * @throws NoSuchProviderException
     *             if the requested provider is not available.
     * @throws IllegalArgumentException if {@code provider == NULL || provider.isEmpty()}
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ IKeyFactory** result) /*throws NoSuchAlgorithmException, NoSuchProviderException*/;

    /**
     * Returns a new instance of {@code KeyFactory} that utilizes the specified
     * algorithm from the specified provider. The {@code provider} supplied
     * does not have to be registered.
     *
     * @param algorithm
     *            the name of the algorithm.
     * @param provider
     *            the security provider.
     * @return a new instance of {@code KeyFactory} that utilizes the specified
     *         algorithm from the specified provider.
     * @throws NoSuchAlgorithmException
     *             if the provider does not provide the requested algorithm.
     * @throws IllegalArgumentException if {@code provider == NULL}
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider* provider,
        /* [out] */ IKeyFactory** result) /*throws NoSuchAlgorithmException*/;

    /**
     * Returns the provider associated with this {@code KeyFactory}.
     *
     * @return the provider associated with this {@code KeyFactory}.
     */
    CARAPI GetProvider(
        /* [out] */ IProvider** result);

    /**
     * Returns the name of the algorithm associated with this {@code
     * KeyFactory}.
     *
     * @return the name of the algorithm associated with this {@code
     *         KeyFactory}.
     */
    CARAPI GetAlgorithm(
        /* [out] */ String* result);

    /**
     * Generates a instance of {@code PublicKey} from the given key
     * specification.
     *
     * @param keySpec
     *            the specification of the public key
     * @return the public key
     * @throws InvalidKeySpecException
     *             if the specified {@code keySpec} is invalid
     */
    CARAPI GeneratePublic(
        /* [in] */ IKeySpec* keySpec,
        /* [out] */ IPublicKey** result) /*throws InvalidKeySpecException*/;

    /**
     * Generates a instance of {@code PrivateKey} from the given key
     * specification.
     *
     * @param keySpec
     *            the specification of the private key.
     * @return the private key.
     * @throws InvalidKeySpecException
     *             if the specified {@code keySpec} is invalid.
     */
    CARAPI GeneratePrivate(
        /* [in] */ IKeySpec* keySpec,
        /* [out] */ IPrivateKey** result) /*throws InvalidKeySpecException*/;

    /**
     * Returns the key specification for the specified key.
     *
     * @param key
     *            the key from which the specification is requested.
     * @param keySpec
     *            the type of the requested {@code KeySpec}.
     * @return the key specification for the specified key.
     * @throws InvalidKeySpecException
     *             if the key can not be processed, or the requested requested
     *             {@code KeySpec} is inappropriate for the given key.
     */
    CARAPI GetKeySpec(
        /* [in] */ IKey* key,
        /* [in] */ const ClassID& keySpec,
        /* [out] */ IKeySpec** result) /*throws InvalidKeySpecException*/;

    /**
     * Translates the given key into a key from this key factory.
     *
     * @param key
     *            the key to translate.
     * @return the translated key.
     * @throws InvalidKeyException
     *             if the specified key can not be translated by this key
     *             factory.
     */
    CARAPI TranslateKey(
        /* [in] */ IKey* key,
        /* [out] */ IKey** result) /*throws InvalidKeyException*/;

private:
    // The service name.
    static const String SERVICE;

    // Used to access common engine functionality
    static AutoPtr<IEngine> ENGINE;

    // The provider
    AutoPtr<IProvider> mProvider;

    // The SPI implementation.
    AutoPtr<IKeyFactorySpi> mSpiImpl;

    // The algorithm.
    String mAlgorithm;
};

}
}

#endif // __ELASTOS_SECURITY_CKEYFACTORY_H__
