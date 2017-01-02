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

#ifndef __ELASTOS_SECURITY_KEYPAIRGENERATOR_H__
#define __ELASTOS_SECURITY_KEYPAIRGENERATOR_H__

#include "KeyPairGeneratorSpi.h"

using Org::Apache::Harmony::Security::Fortress::IEngine;

namespace Elastos {
namespace Security {

/**
 * {@code KeyPairGenerator} is an engine class which is capable of generating a
 * private key and its related public key utilizing the algorithm it was
 * initialized with.
 *
 * @see KeyPairGeneratorSpi
 */
class ECO_PUBLIC KeyPairGenerator
    : public KeyPairGeneratorSpi
    , public IKeyPairGenerator
{
public:
    CAR_INTERFACE_DECL()

    virtual ~KeyPairGenerator() = 0;

    /**
     * Constructs a new instance of {@code KeyPairGenerator} with the name of
     * the algorithm to use.
     *
     * @param algorithm
     *            the name of algorithm to use
     */
    CARAPI constructor(
        /* [in] */ const String& algorithm);

    /**
     * Returns the name of the algorithm of this {@code KeyPairGenerator}.
     *
     * @return the name of the algorithm of this {@code KeyPairGenerator}
     */
    CARAPI GetAlgorithm(
        /* [out] */ String* result);

    /**
     * Returns a new instance of {@code KeyPairGenerator} that utilizes the
     * specified algorithm.
     *
     * @param algorithm
     *            the name of the algorithm to use
     * @return a new instance of {@code KeyPairGenerator} that utilizes the
     *         specified algorithm
     * @throws NoSuchAlgorithmException if the specified algorithm is not available
     * @throws NullPointerException
     *             if {@code algorithm} is {@code NULL}
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ IKeyPairGenerator** result) /*throws NoSuchAlgorithmException*/;

    /**
     * Returns a new instance of {@code KeyPairGenerator} that utilizes the
     * specified algorithm from the specified provider.
     *
     * @param algorithm
     *            the name of the algorithm to use
     * @param provider
     *            the name of the provider
     * @return a new instance of {@code KeyPairGenerator} that utilizes the
     *         specified algorithm from the specified provider
     * @throws NoSuchAlgorithmException if the specified algorithm is not available
     * @throws NoSuchProviderException if the specified provider is not available
     * @throws NullPointerException
     *             if {@code algorithm} is {@code NULL}
     * @throws IllegalArgumentException if {@code provider == NULL || provider.isEmpty()}
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ IKeyPairGenerator** result) /*throws NoSuchAlgorithmException, NoSuchProviderException*/;

    /**
     * Returns a new instance of {@code KeyPairGenerator} that utilizes the
     * specified algorithm from the specified provider. The {@code provider}
     * supplied does not have to be registered.
     *
     * @param algorithm
     *            the name of the algorithm to use
     * @param provider
     *            the provider
     * @return a new instance of {@code KeyPairGenerator} that utilizes the
     *         specified algorithm from the specified provider
     * @throws NoSuchAlgorithmException if the specified algorithm is not available
     * @throws NullPointerException
     *             if {@code algorithm} is {@code NULL}
     * @throws IllegalArgumentException if {@code provider == NULL}
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider* provider,
        /* [out] */ IKeyPairGenerator** result) /*throws NoSuchAlgorithmException*/;

    /**
     * Returns the provider associated with this {@code KeyPairGenerator}.
     *
     * @return the provider associated with this {@code KeyPairGenerator}
     */
    CARAPI GetProvider(
        /* [out] */ IProvider** result);

    /**
     * Initializes this {@code KeyPairGenerator} with the given key size. The
     * default parameter set and a default {@code SecureRandom} instance will be
     * used.
     *
     * @param keysize
     *            the size of the key (number of bits)
     */
    CARAPI Initialize(
        /* [in] */ Int32 keysize);

    /**
     * Initializes this {@code KeyPairGenerator} with the given {@code
     * AlgorithmParameterSpec}. A default {@code SecureRandom} instance will be
     * used.
     *
     * @param param
     *            the parameters to use
     * @throws InvalidAlgorithmParameterException
     *             if the specified parameters are not supported
     */
    CARAPI Initialize(
        /* [in] */ IAlgorithmParameterSpec* param) /*throws InvalidAlgorithmParameterException*/;

    /**
     * Computes and returns a new unique {@code KeyPair} each time this method
     * is called.
     * <p>
     * This does exactly the same as {@link #generateKeyPair()}.
     *
     * @return a new unique {@code KeyPair} each time this method is called
     */
    CARAPI GenKeyPair(
        /* [out] */ IKeyPair** result);

    /**
     * Computes and returns a new unique {@code KeyPair} each time this method
     * is called.
     * <p>
     * This does exactly the same as {@link #genKeyPair()}.
     *
     * @return a new unique {@code KeyPair} each time this method is called
     */
    // @Override
    CARAPI GenerateKeyPair(
        /* [out] */ IKeyPair** result);

    /**
     * Initializes this {@code KeyPairGenerator} with the given key size and the
     * given {@code SecureRandom}. The default parameter set will be used.
     *
     * @param keysize
     *            the key size
     * @param random
     *            the source of randomness
     */
    // @Override
    CARAPI Initialize(
        /* [in] */ Int32 keysize,
        /* [in] */ ISecureRandom* random);

    /**
     * Initializes this {@code KeyPairGenerator} with the given {@code
     * AlgorithmParameterSpec} and the given {@code SecureRandom}.
     *
     * @param param
     *            the parameters to use
     * @param random
     *            the source of randomness
     * @throws InvalidAlgorithmParameterException
     *             if the specified parameters are not supported
     */
    // @Override
    CARAPI Initialize(
        /* [in] */ IAlgorithmParameterSpec* param,
        /* [in] */ ISecureRandom* random) /*throws InvalidAlgorithmParameterException*/;

protected:
    // Store KeyPairGenerator SERVICE name
    static const String SERVICE;

    // Used to access common engine functionality
    static AutoPtr<IEngine> ENGINE;

    // Store SecureRandom
    static AutoPtr<ISecureRandom> RANDOM;

    // Store used provider
    AutoPtr<IProvider> mProvider;

    // Store used algorithm
    String mAlgorithm;
};

/**
 *
 * Internal class: KeyPairGenerator implementation
 *
 */
class KeyPairGeneratorImpl: public KeyPairGenerator
{
public:
    KeyPairGeneratorImpl();

    CARAPI constructor(
        /* [in] */ KeyPairGeneratorSpi* keyPairGeneratorSpi,
        /* [in] */ IProvider* provider,
        /* [in] */ const String& algorithm);

    // implementation of initialize(int keysize, SecureRandom random)
    // using corresponding spi initialize() method
    // @Override
    CARAPI Initialize(
        /* [in] */ Int32 keysize,
        /* [in] */ ISecureRandom* random);

    // implementation of generateKeyPair()
    // using corresponding spi generateKeyPair() method
    // @Override
    CARAPI GenerateKeyPair(
        /* [out] */ IKeyPair** result);

    // implementation of initialize(int keysize, SecureRandom random)
    // using corresponding spi initialize() method
    // @Override
    CARAPI Initialize(
        /* [in] */ IAlgorithmParameterSpec* param,
        /* [in] */ ISecureRandom* random) /*throws InvalidAlgorithmParameterException*/;

private:
    // Save KeyPairGeneratorSpi
    AutoPtr<KeyPairGeneratorSpi> mSpiImpl;
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_KEYPAIRGENERATOR_H__
