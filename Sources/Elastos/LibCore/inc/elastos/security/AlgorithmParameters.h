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

#ifndef __ELASTOS_SECURITY_ALGORITHMPARAMETERS_H__
#define __ELASTOS_SECURITY_ALGORITHMPARAMETERS_H__

#include "Elastos.CoreLibrary.Security.h"
#include "AlgorithmParametersSpi.h"

using Org::Apache::Harmony::Security::Fortress::IEngine;

namespace Elastos {
namespace Security {

/**
 * {@code AlgorithmParameters} is an engine class which provides algorithm
 * parameters.
 */
class AlgorithmParameters
    : public Object
    , public IAlgorithmParameters
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Returns a new instance of {@code AlgorithmParameters} for the specified
     * algorithm.
     *
     * @param algorithm
     *            the name of the algorithm to use.
     * @return a new instance of {@code AlgorithmParameters} for the specified
     *         algorithm.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not available.
     * @throws NullPointerException
     *             if {@code algorithm} is {@code NULL}.
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ IAlgorithmParameters** result) /*throws NoSuchAlgorithmException*/;

    /**
     * Returns a new instance of {@code AlgorithmParameters} from the specified
     * provider for the specified algorithm.
     *
     * @param algorithm
     *            the name of the algorithm to use.
     * @param provider
     *            name of the provider of the {@code AlgorithmParameters}.
     * @return a new instance of {@code AlgorithmParameters} for the specified
     *         algorithm.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not available.
     * @throws NoSuchProviderException
     *             if the specified provider is not available.
     * @throws IllegalArgumentException if {@code provider == NULL || provider.isEmpty()}
     * @throws NullPointerException
     *             if {@code algorithm} is {@code NULL}.
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ IAlgorithmParameters** result) /*throws NoSuchAlgorithmException, NoSuchProviderException*/;

    /**
     * Returns a new instance of {@code AlgorithmParameters} from the specified
     * provider for the specified algorithm. The {@code provider} supplied does
     * not have to be registered.
     *
     * @param algorithm
     *            the name of the algorithm to use.
     * @param provider
     *            the provider of the {@code AlgorithmParameters}.
     * @return a new instance of {@code AlgorithmParameters} for the specified
     *         algorithm.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not available.
     * @throws NullPointerException
     *             if {@code algorithm} is {@code NULL}.
     * @throws IllegalArgumentException if {@code provider == NULL}
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider* provider,
        /* [out] */ IAlgorithmParameters** result) /*throws NoSuchAlgorithmException*/;

    /**
     * Returns the provider associated with this {@code AlgorithmParameters}.
     *
     * @return the provider associated with this {@code AlgorithmParameters}.
     */
    CARAPI GetProvider(
        /* [out] */ IProvider** result);

    /**
     * Returns the name of the algorithm.
     *
     * @return the name of the algorithm.
     */
    CARAPI GetAlgorithm(
        /* [out] */ String* algorithm);

    /**
     * Initializes this {@code AlgorithmParameters} with the specified {@code
     * AlgorithmParameterSpec}.
     *
     * @param paramSpec
     *            the parameter specification.
     * @throws InvalidParameterSpecException
     *             if this {@code AlgorithmParameters} has already been
     *             initialized or the given {@code paramSpec} is not appropriate
     *             for initializing this {@code AlgorithmParameters}.
     */
    CARAPI Init(
        /* [in] */ IAlgorithmParameterSpec* paramSpec) /*throws InvalidParameterSpecException*/;

    /**
     * Initializes this {@code AlgorithmParameters} with the specified {@code
     * byte[]} using the default decoding format for parameters. The default
     * encoding format is ASN.1.
     *
     * @param params
     *            the encoded parameters.
     * @throws IOException
     *             if this {@code AlgorithmParameters} has already been
     *             initialized, or the parameter could not be encoded.
     */
    CARAPI Init(
        /* [in] */ ArrayOf<Byte>* params) /*throws IOException*/;

    /**
     * Initializes this {@code AlgorithmParameters} with the specified {@code
     * byte[]} using the specified decoding format.
     *
     * @param params
     *            the encoded parameters.
     * @param format
     *            the name of the decoding format.
     * @throws IOException
     *             if this {@code AlgorithmParameters} has already been
     *             initialized, or the parameter could not be encoded.
     */
    CARAPI Init(
        /* [in] */ ArrayOf<Byte>* params,
        /* [in] */ const String& format) /*throws IOException*/;

    /**
     * Returns the {@code AlgorithmParameterSpec} for this {@code
     * AlgorithmParameters}.
     *
     * @param paramSpec
     *            the type of the parameter specification in which this
     *            parameters should be converted.
     * @return the {@code AlgorithmParameterSpec} for this {@code
     *         AlgorithmParameters}.
     * @throws InvalidParameterSpecException
     *             if this {@code AlgorithmParameters} has already been
     *             initialized, or if this parameters could not be converted to
     *             the specified class.
     */
    CARAPI GetParameterSpec(
        /* [in] */ const ClassID& paramSpec,
        /* [out] */ IInterface** result) /*throws InvalidParameterSpecException*/;

    /**
     * Returns this {@code AlgorithmParameters} in their default encoding
     * format. The default encoding format is ASN.1.
     *
     * @return the encoded parameters.
     * @throws IOException
     *             if this {@code AlgorithmParameters} has already been
     *             initialized, or if this parameters could not be encoded.
     */
    CARAPI GetEncoded(
        /* [out] */ ArrayOf<Byte>** result) /*throws IOException*/;

    /**
     * Returns this {@code AlgorithmParameters} in the specified encoding
     * format.
     *
     * @param format
     *            the name of the encoding format.
     * @return the encoded parameters.
     * @throws IOException
     *             if this {@code AlgorithmParameters} has already been
     *             initialized, or if this parameters could not be encoded.
     */
    CARAPI GetEncoded(
        /* [in] */ const String& format,
        /* [out] */ ArrayOf<Byte>** result) /*throws IOException*/;

    /**
     * Returns a string containing a concise, human-readable description of this
     * {@code AlgorithmParameters}.
     *
     * @return a printable representation for this {@code AlgorithmParameters}.
     */
    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

protected:
    /**
     * Constructs a new instance of {@code AlgorithmParameters} with the given
     * arguments.
     *
     * @param algPramSpi
     *            the concrete implementation.
     * @param provider
     *            the security provider.
     * @param algorithm
     *            the name of the algorithm.
     */
    AlgorithmParameters(
        /* [in] */ AlgorithmParametersSpi* algPramSpi,
        /* [in] */ IProvider* provider,
        /* [in] */ const String& algorithm);

private:
    /**
     * The service name.
     */
    static const String SEVICE;

    /**
     * Used to access common engine functionality.
     */
    static AutoPtr<IEngine> ENGINE;

    /**
     * The security provider.
     */
    AutoPtr<IProvider> mProvider;

    /**
     * The SPI implementation.
     */
    AutoPtr<AlgorithmParametersSpi> mSpiImpl;

    /**
     * The security algorithm.
     */
    String mAlgorithm;

    /**
     * The initialization state.
     */
    Boolean mInitialized;
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_ALGORITHMPARAMETERS_H__
