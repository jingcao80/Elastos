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

#include "AlgorithmParameters.h"
#include "CSecurity.h"
#include "org/apache/harmony/security/fortress/CEngine.h"

using Org::Apache::Harmony::Security::Fortress::CEngine;
using Org::Apache::Harmony::Security::Fortress::ISpiAndProvider;

namespace Elastos {
namespace Security {

static AutoPtr<IEngine> InitEngine()
{
    AutoPtr<CEngine> e;
    CEngine::NewByFriend(String("AlgorithmParameters")/*SERVICE*/, (CEngine**)&e);
    return e;
}

const String AlgorithmParameters::SEVICE("AlgorithmParameters");
AutoPtr<IEngine> AlgorithmParameters::ENGINE = InitEngine();
CAR_INTERFACE_IMPL(AlgorithmParameters, Object, IAlgorithmParameters)
AlgorithmParameters::AlgorithmParameters(
    /* [in] */ AlgorithmParametersSpi* algPramSpi,
    /* [in] */ IProvider* provider,
    /* [in] */ const String& algorithm)
{
    mProvider = provider;
    mAlgorithm = algorithm;
    mSpiImpl = algPramSpi;
}

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
ECode AlgorithmParameters::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IAlgorithmParameters** result) /*throws NoSuchAlgorithmException*/
{
    VALIDATE_NOT_NULL(result);
    if (algorithm == NULL) {
        // throw new NullPointerException("algorithm == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ISpiAndProvider> sap;
    ENGINE->GetInstance(algorithm, NULL, (ISpiAndProvider**)&sap);
    AutoPtr<IInterface> spi;
    sap->GetSpi((IInterface**)&spi);
    AutoPtr<IProvider> p;
    sap->GetProvider((IProvider**)&p);
    *result = new AlgorithmParameters((AlgorithmParametersSpi*)IObject::Probe(spi), p, algorithm);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

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
ECode AlgorithmParameters::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IAlgorithmParameters** result) /*throws NoSuchAlgorithmException, NoSuchProviderException*/
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    if (provider == NULL || provider.IsEmpty()) {
        // throw new IllegalArgumentException("provider == NULL || provider.isEmpty()");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ISecurity> security;
    CSecurity::AcquireSingleton((ISecurity**)&security);
    AutoPtr<IProvider> p;
    security->GetProvider(provider, (IProvider**)&p);
    if (p == NULL) {
        // throw new NoSuchProviderException(provider);
        return E_NO_SUCH_PROVIDER_EXCEPTION;
    }
    return GetInstance(algorithm, p, result);
}

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
ECode AlgorithmParameters::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider* provider,
    /* [out] */ IAlgorithmParameters** result) /*throws NoSuchAlgorithmException*/
{
    VALIDATE_NOT_NULL(result);
    if (provider == NULL) {
        // throw new IllegalArgumentException("provider == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (algorithm == NULL) {
        // throw new NullPointerException("algorithm == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IInterface> spi;
    ENGINE->GetInstance(algorithm, provider, NULL, (IInterface**)&spi);
    *result = new AlgorithmParameters((AlgorithmParametersSpi*)IObject::Probe(spi), provider, algorithm);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * Returns the provider associated with this {@code AlgorithmParameters}.
 *
 * @return the provider associated with this {@code AlgorithmParameters}.
 */
ECode AlgorithmParameters::GetProvider(
    /* [out] */ IProvider** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mProvider;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * Returns the name of the algorithm.
 *
 * @return the name of the algorithm.
 */
ECode AlgorithmParameters::GetAlgorithm(
    /* [out] */ String* algorithm)
{
    VALIDATE_NOT_NULL(algorithm);
    *algorithm = mAlgorithm;
    return NOERROR;
}

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
ECode AlgorithmParameters::Init(
    /* [in] */ IAlgorithmParameterSpec* paramSpec) /*throws InvalidParameterSpecException*/
{
    if (mInitialized) {
        // throw new InvalidParameterSpecException("Parameter has already been initialized");
        return E_INVALIDPARAMETERSPEC_EXCEPTION;
    }
    mSpiImpl->EngineInit(paramSpec);
    mInitialized = TRUE;
    return NOERROR;
}

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
ECode AlgorithmParameters::Init(
    /* [in] */ ArrayOf<Byte>* params) /*throws IOException*/
{
    if (mInitialized) {
        // throw new IOException("Parameter has already been initialized");
        return E_IO_EXCEPTION;
    }
    mSpiImpl->EngineInit(params);
    mInitialized = TRUE;
    return NOERROR;
}

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
ECode AlgorithmParameters::Init(
    /* [in] */ ArrayOf<Byte>* params,
    /* [in] */ const String& format) /*throws IOException*/
{
    if (mInitialized) {
        // throw new IOException("Parameter has already been initialized");
        return E_IO_EXCEPTION;
    }
    mSpiImpl->EngineInit(params, format);
    mInitialized = TRUE;
    return NOERROR;
}

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
ECode AlgorithmParameters::GetParameterSpec(
    /* [in] */ const ClassID& paramSpec,
    /* [out] */ IInterface** result) /*throws InvalidParameterSpecException*/
{
    VALIDATE_NOT_NULL(result);
    if (!mInitialized) {
        // throw new InvalidParameterSpecException("Parameter has not been initialized");
        return E_INVALIDPARAMETERSPEC_EXCEPTION;
    }
    return mSpiImpl->EngineGetParameterSpec(paramSpec, result);
}

/**
 * Returns this {@code AlgorithmParameters} in their default encoding
 * format. The default encoding format is ASN.1.
 *
 * @return the encoded parameters.
 * @throws IOException
 *             if this {@code AlgorithmParameters} has already been
 *             initialized, or if this parameters could not be encoded.
 */
ECode AlgorithmParameters::GetEncoded(
    /* [out] */ ArrayOf<Byte>** result) /*throws IOException*/
{
    VALIDATE_NOT_NULL(result);
    if (!mInitialized) {
        // throw new IOException("Parameter has not been initialized");
        return E_IO_EXCEPTION;
    }
    return mSpiImpl->EngineGetEncoded(result);
}

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
ECode AlgorithmParameters::GetEncoded(
    /* [in] */ const String& format,
    /* [out] */ ArrayOf<Byte>** result) /*throws IOException*/
{
    VALIDATE_NOT_NULL(result);
    if (!mInitialized) {
        // throw new IOException("Parameter has not been initialized");
        return E_IO_EXCEPTION;
    }
    return mSpiImpl->EngineGetEncoded(format, result);
}

/**
 * Returns a string containing a concise, human-readable description of this
 * {@code AlgorithmParameters}.
 *
 * @return a printable representation for this {@code AlgorithmParameters}.
 */
// @Override
ECode AlgorithmParameters::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    if (!mInitialized) {
        *result = String(NULL);
        return NOERROR;
    }
    return mSpiImpl->EngineToString(result);
}

} // namespace Security
} // namespace Elastos
