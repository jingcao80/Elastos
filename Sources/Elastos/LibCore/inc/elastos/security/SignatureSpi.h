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

#ifndef __ELASTOS_SECURITY_SIGNATURESPI_H__
#define __ELASTOS_SECURITY_SIGNATURESPI_H__

#include "Elastos.CoreLibrary.Security.h"
#include "elastos/core/Object.h"

using Elastos::Core::Object;
using Elastos::IO::IByteBuffer;
using Elastos::Security::IPrivateKey;
using Elastos::Security::IPublicKey;
using Elastos::Security::ISecureRandom;
using Elastos::Security::IAlgorithmParameters;
using Elastos::Security::Spec::IAlgorithmParameterSpec;

namespace Elastos {
namespace Security {

class ECO_PUBLIC SignatureSpi
    : public Object
    , public ISignatureSpi
{
    friend class SignatureImpl;

public:
    CAR_INTERFACE_DECL()

    SignatureSpi();

    virtual ~SignatureSpi();

    // @Override
    CARAPI Clone(
        /* [out] */ IInterface** object);

protected:
    /**
     * Initializes this {@code SignatureSpi} instance for signature
     * verification, using the public key of the identity whose signature is
     * going to be verified.
     *
     * @param publicKey
     *            the public key.
     * @throws InvalidKeyException
     *             if {@code publicKey} is not valid.
     */
    virtual CARAPI EngineInitVerify(
        /* [in] */ IPublicKey* publicKey) = 0;

    /**
     * Initializes this {@code SignatureSpi} instance for signing, using the
     * private key of the identity whose signature is going to be generated.
     *
     * @param privateKey
     *            the private key.
     * @throws InvalidKeyException
     *             if {@code privateKey} is not valid.
     */
    virtual CARAPI EngineInitSign(
        /* [in] */ IPrivateKey* privateKey) = 0;

    /**
     * Initializes this {@code SignatureSpi} instance for signing, using the
     * private key of the identity whose signature is going to be generated and
     * the specified source of randomness.
     *
     * @param privateKey
     *            the private key.
     * @param random
     *            the {@code SecureRandom} to use.
     * @throws InvalidKeyException
     *             if {@code privateKey} is not valid.
     */
    virtual CARAPI EngineInitSign(
        /* [in] */ IPrivateKey* privateKey,
        /* [in] */ ISecureRandom* random);

    /**
     * Updates the data to be verified or to be signed, using the specified
     * {@code byte}.
     *
     * @param b
     *            the byte to update with.
     * @throws SignatureException
     *             if this {@code SignatureSpi} instance is not initialized
     *             properly.
     */
    virtual CARAPI EngineUpdate(
        /* [in] */ Byte b) = 0;

    /**
     * Updates the data to be verified or to be signed, using the given {@code
     * byte[]}, starting form the specified index for the specified length.
     *
     * @param b
     *            the byte array to update with.
     * @param off
     *            the start index in {@code b} of the data.
     * @param len
     *            the number of bytes to use.
     * @throws SignatureException
     *             if this {@code SignatureSpi} instance is not initialized
     *             properly.
     */
    virtual CARAPI EngineUpdate(
        /* [in, out] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len) = 0;

    /**
     * Updates the data to be verified or to be signed, using the specified
     * {@code ByteBuffer}.
     *
     * @param input
     *            the {@code ByteBuffer} to update with.
     * @throws RuntimeException
     *             since {@code SignatureException} is not specified for this
     *             method it throws a {@code RuntimeException} if underlying
     *             {@link #engineUpdate(byte[], int, int)} throws {@code
     *             SignatureException}.
     */
    virtual CARAPI EngineUpdate(
        /* [in] */ IByteBuffer* input);

    /**
     * Generates and returns the signature of all updated data.
     * <p>
     * This {@code SignatureSpi} instance is reset to the state of its last
     * initialization for signing and thus can be used for another signature
     * from the same identity.
     *
     * @return the signature of all updated data.
     * @throws SignatureException
     *             if this {@code SignatureSpi} instance is not initialized
     *             properly.
     */
    virtual CARAPI EngineSign(
        /* [out, callee] */ ArrayOf<Byte>** sign) = 0;

    /**
     * Generates and stores the signature of all updated data in the provided
     * {@code byte[]} at the specified position with the specified length.
     * <p>
     * This {@code SignatureSpi} instance is reset to the state of its last
     * initialization for signing and thus can be used for another signature
     * from the same identity.
     *
     * @param outbuf
     *            the buffer to store the signature.
     * @param offset
     *            the index of the first byte in {@code outbuf} to store.
     * @param len
     *            the number of bytes allocated for the signature.
     * @return the number of bytes stored in {@code outbuf}.
     * @throws SignatureException
     *             if this {@code SignatureSpi} instance is not initialized
     *             properly.
     * @throws IllegalArgumentException
     *             if {@code offset} or {@code len} are not valid in respect to
     *             {@code outbuf}.
     */
    virtual CARAPI EngineSign(
        /* [in, out] */ ArrayOf<Byte>* outbuf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len,
        /* [out] */ Int32* number);

    /**
     * Indicates whether the given {@code sigBytes} can be verified using the
     * public key or a certificate of the signer.
     * <p>
     * This {@code SignatureSpi} instance is reset to the state of its last
     * initialization for verifying and thus can be used to verify another
     * signature of the same signer.
     *
     * @param sigBytes
     *            the signature to verify.
     * @return {@code true} if the signature was verified, {@code false}
     *         otherwise.
     * @throws SignatureException
     *             if this {@code SignatureSpi} instance is not initialized
     *             properly.
     */
    virtual CARAPI EngineVerify(
        /* [in] */ ArrayOf<Byte>* sigBytes,
        /* [out] */ Boolean* result) = 0;

    /**
     * Indicates whether the given {@code sigBytes} starting at index {@code
     * offset} with {@code length} bytes can be verified using the public key or
     * a certificate of the signer.
     * <p>
     * This {@code SignatureSpi} instance is reset to the state of its last
     * initialization for verifying and thus can be used to verify another
     * signature of the same signer.
     *
     * @param sigBytes
     *            the {@code byte[]} containing the signature to verify.
     * @param offset
     *            the start index in {@code sigBytes} of the signature
     * @param length
     *            the number of bytes allocated for the signature.
     * @return {@code true} if the signature was verified, {@code false}
     *         otherwise.
     * @throws SignatureException
     *             if this {@code SignatureSpi} instance is not initialized
     *             properly.
     * @throws IllegalArgumentException
     *             if {@code offset} or {@code length} are not valid in respect
     *             to {@code sigBytes}.
     */
    virtual CARAPI EngineVerify(
        /* [in] */ ArrayOf<Byte>* sigBytes,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Boolean* result);

    /**
     * Sets the specified parameter to the given value.
     *
     * @param param
     *            the name of the parameter.
     * @param value
     *            the parameter value.
     * @throws InvalidParameterException
     *             if the parameter is invalid, already set or is not allowed to
     *             be changed.
     * @deprecated Use {@link #engineSetParameter(AlgorithmParameterSpec)}
     */
    // @Deprecated
    virtual CARAPI EngineSetParameter(
        /* [in] */ const String& param,
        /* [in] */ IInterface* value) = 0;

    /**
     * Sets the specified {@code AlgorithmParameterSpec}.
     *
     * @param params
     *            the parameter to set.
     * @throws InvalidAlgorithmParameterException
     *             if the parameter is invalid, already set or is not allowed to
     *             be changed.
     */
    virtual CARAPI EngineSetParameter(
        /* [in] */ IAlgorithmParameterSpec* params);

    /**
     * Returns the {@code AlgorithmParameters} of this {@link SignatureSpi}
     * instance.
     *
     * @return the {@code AlgorithmParameters} of this {@link SignatureSpi}
     *         instance, maybe {@code null}.
     */
    virtual CARAPI EngineGetParameters(
        /* [out] */ IAlgorithmParameters** params);

    /**
     * Returns the value of the parameter with the specified name.
     *
     * @param param
     *            the name of the requested parameter value.
     * @return the value of the parameter with the specified name, maybe {@code
     *         null}.
     * @throws InvalidParameterException
     *             if {@code param} is not a valid parameter for this {@code
     *             SignatureSpi} or an other error occurs.
     * @deprecated There is no generally accepted parameter naming convention.
     */
    // @Deprecated
    virtual CARAPI EngineGetParameter(
        /* [in] */ const String& param,
        /* [out] */ IInterface** value) = 0;

protected:
    /**
     * Implementation specific source of randomness.
     */
    AutoPtr<ISecureRandom> mAppRandom;
};

} // namespace Security
} // namespace Elastos

#endif //__ELASTOS_SECURITY_SIGNATURESPI_H__
