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

#ifndef __ELASTOSX_CRYPTO_CEXEMPTIONMECHANISM_H__
#define __ELASTOSX_CRYPTO_CEXEMPTIONMECHANISM_H__

#include "Elastos.CoreLibrary.Security.h"
#include "_Elastosx_Crypto_CExemptionMechanism.h"
#include <elastos/core/Object.h>

using Elastos::Security::IKey;
using Elastos::Security::IProvider;
using Elastos::Security::IAlgorithmParameters;
using Elastos::Security::Spec::IAlgorithmParameterSpec;
//TODO: Need IEngine
//using Org::Apache::Harmony::Security::Fortress::IEngine;

namespace Elastosx {
namespace Crypto {

CarClass(CExemptionMechanism)
    , public Object
    , public IExemptionMechanism
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CExemptionMechanism();

    /**
     * Creates a {@code ExemptionMechanism} instance.
     *
     * @param exmechSpi
     *            the implementation delegate.
     * @param provider
     *            the associated provider.
     * @param mechanism
     *            the name of the mechanism.
     */
    CARAPI constructor(
        /* [in] */ IExemptionMechanismSpi* exmechSpi,
        /* [in] */ IProvider* provider,
        /* [in] */ const String& mechanism);

    /**
     * Returns the name of this {@code ExemptionMechanism}.
     *
     * @return the name of this {@code ExemptionMechanism}.
     */
    CARAPI GetName(
        /* [out] */ String * name);

    /**
     * Returns the provider of this {@code ExemptionMechanism} instance.
     *
     * @return the provider of this {@code ExemptionMechanism} instance.
     */
    CARAPI GetProvider(
        /* [out] */ IProvider ** provider);

    /**
     * Returns whether the result blob for this {@code ExemptionMechanism}
     * instance has been generated successfully and that the specified key is
     * the same as the one that was used to initialize and generate.
     *
     * @param key
     *            the key to verify.
     * @return whether the result blob for this {@code ExemptionMechanism}
     *         instance has been generated successfully.
     * @throws ExemptionMechanismException
     *             if an error occurs while determining whether the result blob
     *             has been generated successfully.
     */
    CARAPI IsCryptoAllowed(
        /* [in] */ IKey * key,
        /* [out] */ Boolean * value);

    /**
     * Returns the size in bytes for the output buffer needed to hold the output
     * of the next {@link #genExemptionBlob} call, given the specified {@code
     * inputLen} (in bytes).
     *
     * @param inputLen
     *            the specified input length (in bytes).
     * @return the size in bytes for the output buffer.
     * @throws IllegalStateException
     *             if this {@code ExemptionMechanism} instance is not
     *             initialized.
     */
    CARAPI GetOutputSize(
        /* [in] */ Int32 inputLen,
        /* [out] */ Int32 * size);

    /**
     * Initializes this {@code ExemptionMechanism} instance with the
     * specified key.
     *
     * @param key
     *            the key to initialize this instance with.
     * @throws InvalidKeyException
     *             if the key cannot be used to initialize this mechanism.
     * @throws ExemptionMechanismException
     *             if error(s) occur during initialization.
     */
    CARAPI Init(
        /* [in] */ IKey * key);

    /**
     * Initializes this {@code ExemptionMechanism} instance with the
     * specified key and algorithm parameters.
     *
     * @param key
     *            the key to initialize this instance with.
     * @param param
     *            the parameters for this exemption mechanism algorithm.
     * @throws InvalidKeyException
     *             if the key cannot be used to initialize this mechanism.
     * @throws InvalidAlgorithmParameterException
     *             if the parameters cannot be used to initialize this
     *             mechanism.
     * @throws ExemptionMechanismException
     *             if error(s) occur during initialization.
     */
    CARAPI Init(
        /* [in] */ IKey * key,
        /* [in] */ IAlgorithmParameters * params);

    /**
     * Initializes this {@code ExemptionMechanism} instance with the
     * specified key and algorithm parameters.
     *
     * @param key
     *            the key to initialize this instance with.
     * @param param
     *            the parameters for this exemption mechanism algorithm.
     * @throws InvalidKeyException
     *             if the key cannot be used to initialize this mechanism.
     * @throws InvalidAlgorithmParameterException
     *             the the parameters cannot be used to initialize this
     *             mechanism.
     * @throws ExemptionMechanismException
     *             if error(s) occur during initialization.
     */
    CARAPI Init(
        /* [in] */ IKey * key,
        /* [in] */ IAlgorithmParameterSpec * params);

    /**
     * Generates the result key blob for this exemption mechanism.
     *
     * @return the result key blob for this exemption mechanism.
     * @throws IllegalStateException
     *             if this {@code ExemptionMechanism} instance is not
     *             initialized.
     * @throws ExemptionMechanismException
     *             if error(s) occur during generation.
     */
    CARAPI GenExemptionBlob(
        /* [out, callee] */ ArrayOf<Byte> ** blob);

    /**
     * Generates the result key blob for this exemption mechanism and stores it
     * into the {@code output} buffer.
     *
     * @param output
     *            the output buffer for the result key blob.
     * @return the number of bytes written to the {@code output} buffer.
     * @throws IllegalStateException
     *             if this {@code ExemptionMechanism} instance is not
     *             initialized.
     * @throws ShortBufferException
     *             if the provided buffer is too small for the result key blob.
     * @throws ExemptionMechanismException
     *             if error(s) occur during generation.
     */
    CARAPI GenExemptionBlob(
        /* [in] */ ArrayOf<Byte> * output,
        /* [out] */ Int32 * number);

    /**
     * Generates the result key blob for this exemption mechanism and stores it
     * into the {@code output} buffer at offset {@code outputOffset}.
     *
     * @param output
     *            the output buffer for the result key blob.
     * @param outputOffset
     *            the offset in the output buffer to start.
     * @return the number of bytes written to the {@code output} buffer.
     * @throws IllegalStateException
     *             if this {@code ExemptionMechanism} instance is not
     *             initialized.
     * @throws ShortBufferException
     *             if the provided buffer is too small for the result key blob.
     * @throws ExemptionMechanismException
     *             if error(s) occur during generation.
     */
    CARAPI GenExemptionBlob(
        /* [in] */ ArrayOf<Byte> * output,
        /* [in] */ Int32 outputOffset,
        /* [out] */ Int32 * number);

    /**
     * Returns a new {@code ExemptionMechanism} instance that provides the
     * specified exemption mechanism algorithm.
     *
     * @param algorithm
     *            the name of the requested exemption mechanism.
     * @return the new {@code ExemptionMechanism} instance.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not available by any provider.
     * @throws NullPointerException
     *             if the algorithm parameter is {@code null}.
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ IExemptionMechanism ** exemptionMechanism);

    /**
     * Returns a new {@code ExemptionMechansm} instance that provides the
     * specified exemption mechanism algorithm from the specified provider.
     *
     * @param algorithm
     *            the name of the requested exemption mechanism.
     * @param provider
     *            the name of the provider that is providing the algorithm.
     * @return the new {@code ExemptionMechanism} instance.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not provided by the specified
     *             provider.
     * @throws NoSuchProviderException
     *             if the specified provider is not available.
     * @throws NullPointerException
     *             if the algorithm parameter is {@code null}.
     * @throws IllegalArgumentException
     *             if the provider parameter is {@code null}.
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ IExemptionMechanism ** exemptionMechanism);

    /**
     * Returns a new {@code ExemptionMechanism} instance that provides the
     * specified exemption mechanism algorithm from the specified provider.
     * The {@code provider} supplied does not have to be registered.
     *
     * @param algorithm
     *            the name of the requested exemption mechanism.
     * @param provider
     *            the provider that is providing the algorithm.
     * @return the new {@code ExemptionMechanism} instance.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not provided by the specified
     *             provider.
     * @throws NullPointerException
     *             if the algorithm parameter is {@code null}.
     * @throws IllegalArgumentException
     *             if the provider parameter is {@code null}.
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider * provider,
        /* [out] */ IExemptionMechanism ** exemptionMechanism);

private:
    // Used to access common engine functionality
//TODO: Need IEngine
    // static AutoPtr<IEngine> mENGINE; // = new Engine("ExemptionMechanism");

    // Store used provider
    AutoPtr<IProvider> mProvider;

    // Store used spi implementation
    AutoPtr<IExemptionMechanismSpi> mSpiImpl;

    // Store mechanism name
    String mMechanism;

    // Store state (initialized or not)
    Boolean mIsInit;

    // Store initKey value
    AutoPtr<IKey> mInitKey;

    // Indicates if blob generated successfully
    Boolean mGenerated;
};

}
}

#endif // __ELASTOSX_CRYPTO_CEXEMPTIONMECHANISM_H__
