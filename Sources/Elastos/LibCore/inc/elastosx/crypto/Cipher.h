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

#ifndef __ELASTOSX_CRYPTO_CIPHER_H__
#define __ELASTOSX_CRYPTO_CIPHER_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Security.h"
#include <elastos/core/Object.h>

using Elastos::IO::IByteBuffer;
using Elastos::Security::IKey;
using Elastos::Security::IProvider;
using Elastos::Security::IProviderService;
using Elastos::Security::ISecureRandom;
using Elastos::Security::IAlgorithmParameters;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Org::Apache::Harmony::Security::Fortress::IEngine;
using Org::Apache::Harmony::Security::Fortress::ISpiAndProvider;

namespace Elastosx {
namespace Crypto {

class ECO_PUBLIC Cipher
    : public Object
    , public ICipher
{
public:
    CAR_INTERFACE_DECL()

    Cipher();

    /**
     * Creates a new Cipher instance.
     *
     * @param cipherSpi
     *            the implementation delegate of the cipher.
     * @param provider
     *            the provider of the implementation of this cipher.
     * @param transformation
     *            the name of the transformation that this cipher performs.
     * @throws NullPointerException
     *             if either cipherSpi is {@code null} or provider is {@code
     *             null} and {@code cipherSpi} is a {@code NullCipherSpi}.
     */
    CARAPI constructor(
        /* [in] */ ICipherSpi* cipherSpi,
        /* [in] */ IProvider* provider,
        /* [in] */ const String& transformation);

    CARAPI constructor(
        /* [in] */ const String& transformation,
        /* [in] */ ArrayOf<String> * transformParts,
        /* [in] */ IProvider* provider);

    /**
     * Returns the provider of this cipher instance.
     *
     * @return the provider of this cipher instance.
     */
    CARAPI GetProvider(
        /* [out] */ IProvider ** provider);

    /**
     * Returns the name of the algorithm of this cipher instance.
     * <p>
     * This is the name of the <i>transformation</i> argument used in the
     * {@code getInstance} call creating this object.
     *
     * @return the name of the algorithm of this cipher instance.
     */
    CARAPI GetAlgorithm(
        /* [out] */ String * value);

    /**
     * Returns this ciphers block size (in bytes).
     *
     * @return this ciphers block size.
     */
    CARAPI GetBlockSize(
        /* [out] */ Int32 * value);

    /**
     * Returns the length in bytes an output buffer needs to be when this cipher
     * is updated with {@code inputLen} bytes.
     *
     * @param inputLen
     *            the number of bytes of the input.
     * @return the output buffer length for the input length.
     * @throws IllegalStateException
     *             if this cipher instance is in an invalid state.
     */
    CARAPI GetOutputSize(
        /* [in] */ Int32 inputLen,
        /* [out] */ Int32 * value);

    /**
     * Returns the <i>initialization vector</i> for this cipher instance.
     *
     * @return the <i>initialization vector</i> for this cipher instance.
     */
    CARAPI GetIV(
        /* [out, callee] */ ArrayOf<Byte> ** value);

    /**
     * Returns the parameters that where used to create this cipher instance.
     * <p>
     * These may be a the same parameters that were used to create this cipher
     * instance, or may be a combination of default and random parameters,
     * depending on the underlying cipher implementation.
     *
     * @return the parameters that where used to create this cipher instance, or
     *         {@code null} if this cipher instance does not have any
     *         parameters.
    */
    CARAPI GetParameters(
        /* [out] */ IAlgorithmParameters ** parameters);

    /**
     * Returns the exemption mechanism associated with this cipher.
     *
     * @return currently {@code null}
     */
    CARAPI GetExemptionMechanism(
        /* [out] */ IExemptionMechanism ** parameters);

    /**
     * Initializes this cipher instance with the specified key.
     * <p>
     * The cipher is initialized for the specified operational mode (one of:
     * encryption, decryption, key wrapping or key unwrapping) depending on
     * {@code opmode}.
     * <p>
     * If this cipher instance needs any algorithm parameters or random values
     * that the specified key can not provide, the underlying implementation of
     * this cipher is supposed to generate the required parameters (using its
     * provider or random values).
     * <p>
     * When a cipher instance is initialized by a call to any of the {@code
     * init} methods, the state of the instance is overridden, meaning that it
     * is equivalent to creating a new instance and calling its {@code init}
     * method.
     *
     * @param opmode
     *            the operation this cipher instance should be initialized for
     *            (one of: {@code ENCRYPT_MODE}, {@code DECRYPT_MODE}, {@code
     *            WRAP_MODE} or {@code UNWRAP_MODE}).
     * @param key
     *            the input key for the operation.
     * @throws InvalidKeyException
     *             if the specified key can not be used to initialize this
     *             cipher instance.
     */
    CARAPI Init(
        /* [in] */ Int32 opmode,
        /* [in] */ IKey * key);

    /**
     * Initializes this cipher instance with the specified key and a source of
     * randomness.
     * <p>
     * The cipher is initialized for the specified operational mode (one of:
     * encryption, decryption, key wrapping or key unwrapping) depending on
     * {@code opmode}.
     * <p>
     * If this cipher instance needs any algorithm parameters or random values
     * that the specified key can not provide, the underlying implementation of
     * this cipher is supposed to generate the required parameters (using its
     * provider or random values). Random values are generated using {@code
     * random};
     * <p>
     * When a cipher instance is initialized by a call to any of the {@code
     * init} methods, the state of the instance is overridden, means it is
     * equivalent to creating a new instance and calling it {@code init} method.
     *
     * @param opmode
     *            the operation this cipher instance should be initialized for
     *            (one of: {@code ENCRYPT_MODE}, {@code DECRYPT_MODE}, {@code
     *            WRAP_MODE} or {@code UNWRAP_MODE}).
     * @param key
     *            the input key for the operation.
     * @param random
     *            the source of randomness to use.
     * @throws InvalidKeyException
     *             if the specified key can not be used to initialize this
     *             cipher instance.
     * @throws InvalidParameterException
     *             if the specified opmode is invalid.
     */
    CARAPI Init(
        /* [in] */ Int32 opmode,
        /* [in] */ IKey * key,
        /* [in] */ ISecureRandom * sr);

    /**
     * Initializes this cipher instance with the specified key and algorithm
     * parameters.
     * <p>
     * The cipher is initialized for the specified operational mode (one of:
     * encryption, decryption, key wrapping or key unwrapping).
     * <p>
     * If this cipher instance needs any algorithm parameters and {@code params}
     * is {@code null}, the underlying implementation of this cipher is supposed
     * to generate the required parameters (using its provider or random
     * values).
     * <p>
     * When a cipher instance is initialized by a call to any of the {@code
     * init} methods, the state of the instance is overridden, means it is
     * equivalent to creating a new instance and calling it {@code init} method.
     *
     * @param opmode
     *            the operation this cipher instance should be initialized for
     *            (one of: {@code ENCRYPT_MODE}, {@code DECRYPT_MODE}, {@code
     *            WRAP_MODE} or {@code UNWRAP_MODE}).
     * @param key
     *            the input key for the operation.
     * @param params
     *            the algorithm parameters.
     * @throws InvalidKeyException
     *             if the specified key can not be used to initialize this
     *             cipher instance.
     * @throws InvalidAlgorithmParameterException
     *             it the specified parameters are inappropriate for this
     *             cipher.
     */
    CARAPI Init(
        /* [in] */ Int32 opmode,
        /* [in] */ IKey * key,
        /* [in] */ IAlgorithmParameterSpec * sr);

    /**
     * Initializes this cipher instance with the specified key, algorithm
     * parameters and a source of randomness.
     * <p>
     * The cipher is initialized for the specified operational mode (one of:
     * encryption, decryption, key wrapping or key unwrapping) depending on
     * {@code opmode}.
     * <p>
     * If this cipher instance needs any algorithm parameters and {@code params}
     * is {@code null}, the underlying implementation of this cipher is supposed
     * to generate the required parameters (using its provider or random
     * values). Random values are generated using {@code random};
     * <p>
     * When a cipher instance is initialized by a call to any of the {@code
     * init} methods, the state of the instance is overridden, meaning that it
     * is equivalent to creating a new instance and calling it {@code init}
     * method.
     *
     * @param opmode
     *            the operation this cipher instance should be initialized for
     *            (one of: {@code ENCRYPT_MODE}, {@code DECRYPT_MODE}, {@code
     *            WRAP_MODE} or {@code UNWRAP_MODE}).
     * @param key
     *            the input key for the operation.
     * @param params
     *            the algorithm parameters.
     * @param random
     *            the source of randomness to use.
     * @throws InvalidKeyException
     *             if the specified key can not be used to initialize this
     *             cipher instance.
     * @throws InvalidAlgorithmParameterException
     *             it the specified parameters are inappropriate for this
     *             cipher.
     * @throws InvalidParameterException
     *             if the specified {@code opmode} is invalid.
     */
    CARAPI Init(
        /* [in] */ Int32 opmode,
        /* [in] */ IKey * key,
        /* [in] */ IAlgorithmParameterSpec * aps,
        /* [in] */ ISecureRandom * sr);

    /**
     * Initializes this cipher instance with the specified key and algorithm
     * parameters.
     * <p>
     * The cipher is initialized for the specified operation (one of:
     * encryption, decryption, key wrapping or key unwrapping) depending on
     * {@code opmode}.
     * <p>
     * If this cipher instance needs any algorithm parameters and {@code params}
     * is {@code null}, the underlying implementation of this cipher is supposed
     * to generate the required parameters (using its provider or random
     * values).
     * <p>
     * When a cipher instance is initialized by a call to any of the {@code
     * init} methods, the state of the instance is overridden, meaning that it
     * is equivalent to creating a new instance and calling it {@code init}
     * method.
     *
     * @param opmode
     *            the operation this cipher instance should be initialized for
     *            (one of: {@code ENCRYPT_MODE}, {@code DECRYPT_MODE}, {@code
     *            WRAP_MODE} or {@code UNWRAP_MODE}).
     * @param key
     *            the input key for the operation.
     * @param params
     *            the algorithm parameters.
     * @throws InvalidKeyException
     *             if the specified key can not be used to initialize this
     *             cipher instance.
     * @throws InvalidAlgorithmParameterException
     *             it the specified parameters are inappropriate for this
     *             cipher.
     */
    CARAPI Init(
        /* [in] */ Int32 opmode,
        /* [in] */ IKey * key,
        /* [in] */ IAlgorithmParameters * ap);

    /**
     * Initializes this cipher instance with the specified key, algorithm
     * parameters and a source of randomness.
     * <p>
     * The cipher will be initialized for the specified operation (one of:
     * encryption, decryption, key wrapping or key unwrapping) depending on
     * {@code opmode}.
     * <p>
     * If this cipher instance needs any algorithm parameters and {@code params}
     * is {@code null}, the underlying implementation of this cipher is supposed
     * to generate the required parameters (using its provider or random
     * values). Random values are generated using {@code random}.
     * <p>
     * When a cipher instance is initialized by a call to any of the {@code
     * init} methods, the state of the instance is overridden, means it is
     * equivalent to creating a new instance and calling it {@code init} method.
     *
     * @param opmode
     *            the operation this cipher instance should be initialized for
     *            (one of: {@code ENCRYPT_MODE}, {@code DECRYPT_MODE}, {@code
     *            WRAP_MODE} or {@code UNWRAP_MODE}).
     * @param key
     *            the input key for the operation.
     * @param params
     *            the algorithm parameters.
     * @param random
     *            the source of randomness to use.
     * @throws InvalidKeyException
     *             if the specified key can not be used to initialize this
     *             cipher instance.
     * @throws InvalidAlgorithmParameterException
     *             if the specified parameters are inappropriate for this
     *             cipher.
     * @throws InvalidParameterException
     *             if the specified {@code opmode} is invalid.
     */
    CARAPI Init(
        /* [in] */ Int32 opmode,
        /* [in] */ IKey * key,
        /* [in] */ IAlgorithmParameters * ap,
        /* [in] */ ISecureRandom * sr);

    /**
     * Initializes this cipher instance with the public key from the specified
     * certificate.
     * <p>
     * The cipher will be initialized for the specified operation (one of:
     * encryption, decryption, key wrapping or key unwrapping) depending on
     * {@code opmode}.
     * <p>
     * It the type of the certificate is X.509 and the certificate has a <i>key
     * usage</i> extension field marked as critical, the specified {@code
     * opmode} has the be enabled for this key, otherwise an {@code
     * InvalidKeyException} is thrown.
     * <p>
     * If this cipher instance needs any algorithm parameters that the key in
     * the certificate can not provide, the underlying implementation of this
     * cipher is supposed to generate the required parameters (using its
     * provider or random values).
     * <p>
     * When a cipher instance is initialized by a call to any of the {@code
     * init} methods, the state of the instance is overridden, means it is
     * equivalent to creating a new instance and calling it {@code init} method.
     *
     * @param opmode
     *            the operation this cipher instance should be initialized for
     *            (one of: {@code ENCRYPT_MODE}, {@code DECRYPT_MODE}, {@code
     *            WRAP_MODE} or {@code UNWRAP_MODE}).
     * @param certificate
     *            the certificate.
     * @throws InvalidKeyException
     *             if the public key in the certificate can not be used to
     *             initialize this cipher instance.
     */
    CARAPI Init(
        /* [in] */ Int32 opmode,
        /* [in] */ ICertificate * certificate);

    /**
     * Initializes this cipher instance with the public key from the specified
     * certificate and a source of randomness.
     * <p>
     * The cipher will be initialized for the specified operation (one of:
     * encryption, decryption, key wrapping or key unwrapping) depending on
     * {@code opmode}.
     * <p>
     * It the type of the certificate is X.509 and the certificate has a <i>key
     * usage</i> extension field marked as critical, the specified {@code
     * opmode} has the be enabled for this key, otherwise an {@code
     * InvalidKeyException} is thrown.
     * <p>
     * If this cipher instance needs any algorithm parameters that the key in
     * the certificate can not provide, the underlying implementation of this
     * cipher is supposed to generate the required parameters (using its
     * provider or random values). Random values are generated using {@code
     * random}.
     * <p>
     * When a cipher instance is initialized by a call to any of the {@code
     * init} methods, the state of the instance is overridden, means it is
     * equivalent to creating a new instance and calling it {@code init} method.
     *
     * @param opmode
     *            the operation this cipher instance should be initialized for
     *            (one of: {@code ENCRYPT_MODE}, {@code DECRYPT_MODE}, {@code
     *            WRAP_MODE} or {@code UNWRAP_MODE}).
     * @param certificate
     *            the certificate.
     * @param random
     *            the source of randomness to be used.
     * @throws InvalidKeyException
     *             if the public key in the certificate can not be used to
     *             initialize this cipher instance.
     */
    CARAPI Init(
        /* [in] */ Int32 opmode,
        /* [in] */ ICertificate * certificate,
        /* [in] */ ISecureRandom * sr);

    /**
     * Continues a multi-part transformation (encryption or decryption). The
     * transformed bytes are returned.
     *
     * @param input
     *            the input bytes to transform.
     * @return the transformed bytes in a new buffer, or {@code null} if the
     *         input has zero length.
     * @throws IllegalStateException
     *             if this cipher instance is not initialized for encryption or
     *             decryption.
     * @throws IllegalArgumentException
     *             if the input is {@code null}.
     */
    CARAPI Update(
        /* [in] */ ArrayOf<Byte> * input,
        /* [out, callee] */ ArrayOf<Byte> ** output);

    /**
     * Continues a multi-part transformation (encryption or decryption). The
     * transformed bytes are returned.
     *
     * @param input
     *            the input bytes to transform.
     * @param inputOffset
     *            the offset in the input to start.
     * @param inputLen
     *            the length of the input to transform.
     * @return the transformed bytes in a new buffer, or {@code null} if the
     *         input has zero length.
     * @throws IllegalStateException
     *             if this cipher instance is not initialized for encryption or
     *             decryption.
     * @throws IllegalArgumentException
     *             if {@code input} is {@code null}, or if {@code inputOffset} and
     *             {@code inputLen} do not specify a valid chunk in the input
     *             buffer.
     */
    CARAPI Update(
        /* [in] */ ArrayOf<Byte> * input,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen,
        /* [out, callee] */ ArrayOf<Byte> ** output);

    /**
     * Continues a multi-part transformation (encryption or decryption). The
     * transformed bytes are stored in the {@code output} buffer.
     * <p>
     * If the size of the {@code output} buffer is too small to hold the result,
     * a {@code ShortBufferException} is thrown. Use
     * {@link Cipher#getOutputSize getOutputSize} to check for the size of the
     * output buffer.
     *
     * @param input
     *            the input bytes to transform.
     * @param inputOffset
     *            the offset in the input to start.
     * @param inputLen
     *            the length of the input to transform.
     * @param output
     *            the output buffer.
     * @return the number of bytes placed in output.
     * @throws ShortBufferException
     *             if the size of the {@code output} buffer is too small.
     * @throws IllegalStateException
     *             if this cipher instance is not initialized for encryption or
     *             decryption.
     * @throws IllegalArgumentException
     *             if the input is {@code null}, the output is {@code null}, or
     *             if {@code inputOffset} and {@code inputLen} do not specify a
     *             valid chunk in the input buffer.
     */
    CARAPI Update(
        /* [in] */ ArrayOf<Byte> * input,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen,
        /* [in] */ ArrayOf<Byte> * output,
        /* [out] */ Int32 * value);

    /**
     * Continues a multi-part transformation (encryption or decryption). The
     * transformed bytes are stored in the {@code output} buffer.
     * <p>
     * If the size of the {@code output} buffer is too small to hold the result,
     * a {@code ShortBufferException} is thrown. Use
     * {@link Cipher#getOutputSize getOutputSize} to check for the size of the
     * output buffer.
     *
     * @param input
     *            the input bytes to transform.
     * @param inputOffset
     *            the offset in the input to start.
     * @param inputLen
     *            the length of the input to transform.
     * @param output
     *            the output buffer.
     * @param outputOffset
     *            the offset in the output buffer.
     * @return the number of bytes placed in output.
     * @throws ShortBufferException
     *             if the size of the {@code output} buffer is too small.
     * @throws IllegalStateException
     *             if this cipher instance is not initialized for encryption or
     *             decryption.
     * @throws IllegalArgumentException
     *             if the input is {@code null}, the output is {@code null}, or
     *             if {@code inputOffset} and {@code inputLen} do not specify a
     *             valid chunk in the input buffer.
     */
    CARAPI Update(
        /* [in] */ ArrayOf<Byte> * input,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen,
        /* [in] */ ArrayOf<Byte> * output,
        /* [in] */ Int32 outputOffset,
        /* [out] */ Int32 * value);

    /**
     * Continues a multi-part transformation (encryption or decryption). The
     * {@code input.remaining()} bytes starting at {@code input.position()} are
     * transformed and stored in the {@code output} buffer.
     * <p>
     * If the {@code output.remaining()} is too small to hold the transformed
     * bytes a {@code ShortBufferException} is thrown. Use
     * {@link Cipher#getOutputSize getOutputSize} to check for the size of the
     * output buffer.
     *
     * @param input
     *            the input buffer to transform.
     * @param output
     *            the output buffer to store the result within.
     * @return the number of bytes stored in the output buffer.
     * @throws ShortBufferException
     *             if the size of the {@code output} buffer is too small.
     * @throws IllegalStateException
     *             if this cipher instance is not initialized for encryption or
     *             decryption.
     * @throws IllegalArgumentException
     *             if the input buffer and the output buffer are the identical
     *             object.
     */
    CARAPI Update(
        /* [in] */ IByteBuffer * input,
        /* [in] */ IByteBuffer * output,
        /* [out] */ Int32 * value);

    /**
     * Continues a multi-part transformation (encryption or decryption) with
     * Authenticated Additional Data (AAD). AAD may only be added after the
     * {@code Cipher} is initialized and before any data is passed to the
     * instance.
     * <p>
     * This is only usable with cipher modes that support Authenticated
     * Encryption with Additional Data (AEAD) such as Galois/Counter Mode (GCM).
     *
     * @param input bytes of AAD to use with the cipher
     * @throws IllegalStateException
     *             if this cipher instance is not initialized for encryption or
     *             decryption.
     * @throws IllegalArgumentException
     *             if {@code input} is {@code null}
     * @throws UnsupportedOperationException if the cipher does not support AEAD
     * @since 1.7
     */
    CARAPI UpdateAAD(
        /* [in] */ ArrayOf<Byte> * input);

    /**
     * Continues a multi-part transformation (encryption or decryption) with
     * Authenticated Additional Data (AAD). AAD may only be added after the
     * {@code Cipher} is initialized and before any data is passed to the
     * instance.
     * <p>
     * This is only usable with cipher modes that support Authenticated
     * Encryption with Additional Data (AEAD) such as Galois/Counter Mode (GCM).
     *
     * @param input bytes of AAD to use with the cipher
     * @param inputOffset offset within bytes of additional data to add to cipher
     * @param inputLen length of bytes of additional data to add to cipher
     * @throws IllegalStateException
     *             if this cipher instance is not initialized for encryption or
     *             decryption.
     * @throws IllegalArgumentException
     *             if {@code input} is {@code null}, or if {@code inputOffset} and
     *             {@code inputLen} do not specify a valid chunk in the input
     *             buffer.
     * @throws UnsupportedOperationException if the cipher does not support AEAD
     * @since 1.7
     */
    CARAPI UpdateAAD(
        /* [in] */ ArrayOf<Byte> * input,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen);

    /**
     * Continues a multi-part transformation (encryption or decryption) with
     * Authenticated Additional Data (AAD). AAD may only be added after the
     * {@code Cipher} is initialized and before any data is passed to the
     * instance.
     * <p>
     * This is only usable with cipher modes that support Authenticated
     * Encryption with Additional Data (AEAD) such as Galois/Counter Mode (GCM).
     *
     * @param input buffer of AAD to be used
     * @throws IllegalStateException
     *             if this cipher instance is not initialized for encryption or
     *             decryption.
     * @throws UnsupportedOperationException if the cipher does not support AEAD
     * @since 1.7
     */
    CARAPI UpdateAAD(
        /* [in] */ IByteBuffer * input);

    /**
     * Finishes a multi-part transformation (encryption or decryption).
     * <p>
     * Processes any bytes that may have been buffered in previous {@code
     * update} calls.
     *
     * @return the final bytes from the transformation.
     * @throws IllegalBlockSizeException
     *             if the size of the resulting bytes is not a multiple of the
     *             cipher block size.
     * @throws BadPaddingException
     *             if the padding of the data does not match the padding scheme.
     * @throws IllegalStateException
     *             if this cipher instance is not initialized for encryption or
     *             decryption.
     */
    CARAPI DoFinal(
        /* [out, callee] */ ArrayOf<Byte> ** buf);

    /**
     * Finishes a multi-part transformation (encryption or decryption).
     * <p>
     * Processes any bytes that may have been buffered in previous {@code
     * update} calls.
     * <p>
     * The final transformed bytes are stored in the {@code output} buffer.
     *
     * @param output
     *            the output buffer.
     * @param outputOffset
     *            the offset in the output buffer.
     * @return the number of bytes placed in the output buffer.
     * @throws IllegalBlockSizeException
     *             if the size of the resulting bytes is not a multiple of the
     *             cipher block size.
     * @throws ShortBufferException
     *             if the size of the {@code output} buffer is too small.
     * @throws BadPaddingException
     *             if the padding of the data does not match the padding scheme.
     * @throws IllegalStateException
     *             if this cipher instance is not initialized for encryption or
     *             decryption.
     */
    CARAPI DoFinal(
        /* [in] */ ArrayOf<Byte> * output,
        /* [in] */ Int32 outputOffset,
        /* [out] */ Int32 * number);

    /**
     * Finishes a multi-part transformation (encryption or decryption).
     * <p>
     * Processes the bytes in {@code input} buffer, and any bytes that have been
     * buffered in previous {@code update} calls.
     *
     * @param input
     *            the input buffer.
     * @return the final bytes from the transformation.
     * @throws IllegalBlockSizeException
     *             if the size of the resulting bytes is not a multiple of the
     *             cipher block size.
     * @throws BadPaddingException
     *             if the padding of the data does not match the padding scheme.
     * @throws IllegalStateException
     *             if this cipher instance is not initialized for encryption or
     *             decryption.
     */
    CARAPI DoFinal(
        /* [in] */ ArrayOf<Byte> * input,
        /* [out, callee] */ ArrayOf<Byte> ** output);

    /**
     * Finishes a multi-part transformation (encryption or decryption).
     * <p>
     * Processes the {@code inputLen} bytes in {@code input} buffer at {@code
     * inputOffset}, and any bytes that have been buffered in previous {@code
     * update} calls.
     *
     * @param input
     *            the input buffer.
     * @param inputOffset
     *            the offset in the input buffer.
     * @param inputLen
     *            the length of the input
     * @return the final bytes from the transformation.
     * @throws IllegalBlockSizeException
     *             if the size of the resulting bytes is not a multiple of the
     *             cipher block size.
     * @throws BadPaddingException
     *             if the padding of the data does not match the padding scheme.
     * @throws IllegalStateException
     *             if this cipher instance is not initialized for encryption or
     *             decryption.
     * @throws IllegalArgumentException
     *             if {@code inputOffset} and {@code inputLen} do not specify an
     *             valid chunk in the input buffer.
     */
    CARAPI DoFinal(
        /* [in] */ ArrayOf<Byte> * input,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen,
        /* [out, callee] */ ArrayOf<Byte> ** output);

    /**
     * Finishes a multi-part transformation (encryption or decryption).
     * <p>
     * Processes the {@code inputLen} bytes in {@code input} buffer at {@code
     * inputOffset}, and any bytes that have been buffered in previous {@code
     * update} calls.
     *
     * @param input
     *            the input buffer.
     * @param inputOffset
     *            the offset in the input buffer.
     * @param inputLen
     *            the length of the input.
     * @param output
     *            the output buffer for the transformed bytes.
     * @return the number of bytes placed in the output buffer.
     * @throws ShortBufferException
     *             if the size of the {@code output} buffer is too small.
     * @throws IllegalBlockSizeException
     *             if the size of the resulting bytes is not a multiple of the
     *             cipher block size.
     * @throws BadPaddingException
     *             if the padding of the data does not match the padding scheme.
     * @throws IllegalStateException
     *             if this cipher instance is not initialized for encryption or
     *             decryption.
     * @throws IllegalArgumentException
     *             if {@code inputOffset} and {@code inputLen} do not specify an
     *             valid chunk in the input buffer.
     */
    CARAPI DoFinal(
        /* [in] */ ArrayOf<Byte> * input,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen,
        /* [in] */ ArrayOf<Byte> * output,
        /* [out] */ Int32 * number);

    /**
     * Finishes a multi-part transformation (encryption or decryption).
     * <p>
     * Processes the {@code inputLen} bytes in {@code input} buffer at {@code
     * inputOffset}, and any bytes that have been buffered in previous {@code
     * update} calls.
     *
     * @param input
     *            the input buffer.
     * @param inputOffset
     *            the offset in the input buffer.
     * @param inputLen
     *            the length of the input.
     * @param output
     *            the output buffer for the transformed bytes.
     * @param outputOffset
     *            the offset in the output buffer.
     * @return the number of bytes placed in the output buffer.
     * @throws ShortBufferException
     *             if the size of the {@code output} buffer is too small.
     * @throws IllegalBlockSizeException
     *             if the size of the resulting bytes is not a multiple of the
     *             cipher block size.
     * @throws BadPaddingException
     *             if the padding of the data does not match the padding scheme.
     * @throws IllegalStateException
     *             if this cipher instance is not initialized for encryption or
     *             decryption.
     * @throws IllegalArgumentException
     *             if {@code inputOffset} and {@code inputLen} do not specify an
     *             valid chunk in the input buffer.
     */
    CARAPI DoFinal(
        /* [in] */ ArrayOf<Byte> * input,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen,
        /* [in] */ ArrayOf<Byte> * output,
        /* [in] */ Int32 outputOffset,
        /* [out] */ Int32 * number);

    /**
     * Finishes a multi-part transformation (encryption or decryption).
     * <p>
     * Processes the {@code input.remaining()} bytes in {@code input} buffer at
     * {@code input.position()}, and any bytes that have been buffered in
     * previous {@code update} calls. The transformed bytes are placed into
     * {@code output} buffer.
     *
     * @param input
     *            the input buffer.
     * @param output
     *            the output buffer.
     * @return the number of bytes placed into the output buffer.
     * @throws ShortBufferException
     *             if the size of the {@code output} buffer is too small.
     * @throws IllegalBlockSizeException
     *             if the size of the resulting bytes is not a multiple of the
     *             cipher block size.
     * @throws BadPaddingException
     *             if the padding of the data does not match the padding scheme.
     * @throws IllegalArgumentException
     *             if the input buffer and the output buffer are the same
     *             object.
     * @throws IllegalStateException
     *             if this cipher instance is not initialized for encryption or
     *             decryption.
     */
    CARAPI DoFinal(
        /* [in] */ IByteBuffer * input,
        /* [in] */ IByteBuffer * output,
        /* [out] */ Int32 * number);

    /**
     * Wraps a key using this cipher instance.
     *
     * @param key
     *            the key to wrap.
     * @return the wrapped key.
     * @throws IllegalBlockSizeException
     *             if the size of the resulting bytes is not a multiple of the
     *             cipher block size.
     * @throws InvalidKeyException
     *             if this cipher instance can not wrap this key.
     * @throws IllegalStateException
     *             if this cipher instance is not initialized for wrapping.
     */
    CARAPI Wrap(
        /* [in] */ IKey * key,
        /* [out, callee] */ ArrayOf<Byte> ** array);

    /**
     * Unwraps a key using this cipher instance.
     *
     * @param wrappedKey
     *            the wrapped key to unwrap.
     * @param wrappedKeyAlgorithm
     *            the algorithm for the wrapped key.
     * @param wrappedKeyType
     *            the type of the wrapped key (one of: {@code SECRET_KEY
     *            <code>, <code>PRIVATE_KEY} or {@code PUBLIC_KEY})
     * @return the unwrapped key
     * @throws InvalidKeyException
     *             if the {@code wrappedKey} can not be unwrapped to a key of
     *             type {@code wrappedKeyType} for the {@code
     *             wrappedKeyAlgorithm}.
     * @throws NoSuchAlgorithmException
     *             if no provider can be found that can create a key of type
     *             {@code wrappedKeyType} for the {@code wrappedKeyAlgorithm}.
     * @throws IllegalStateException
     *             if this cipher instance is not initialized for unwrapping.
     */
    CARAPI Unwrap(
        /* [in] */ ArrayOf<Byte> * wrappedKey,
        /* [in] */ const String& wrappedKeyAlgorithm,
        /* [in] */ Int32 wrappedKeyType,
        /* [out] */ IKey ** key);

    /**
     * Creates a new Cipher for the specified transformation. The installed
     * providers are searched in order for an implementation of the specified
     * transformation. The first found provider providing the transformation is
     * used to create the cipher. If no provider is found an exception is
     * thrown.
     *
     * @param transformation
     *            the name of the transformation to create a cipher for.
     * @return a cipher for the requested transformation.
     * @throws NoSuchAlgorithmException
     *             if no installed provider can provide the
     *             <i>transformation</i>, or it is {@code null}, empty or in an
     *             invalid format.
     * @throws NoSuchPaddingException
     *             if no installed provider can provide the padding scheme in
     *             the <i>transformation</i>.
     */
    static CARAPI GetInstance(
        /* [in] */ const String& transformation,
        /* [out] */ ICipher ** cipher);

    /**
     * Creates a new cipher for the specified transformation provided by the
     * specified provider.
     *
     * @param transformation
     *            the name of the transformation to create a cipher for.
     * @param provider
     *            the name of the provider to ask for the transformation.
     * @return a cipher for the requested transformation.
     * @throws NoSuchAlgorithmException
     *             if the specified provider can not provide the
     *             <i>transformation</i>, or it is {@code null}, empty or in an
     *             invalid format.
     * @throws NoSuchProviderException
     *             if no provider with the specified name can be found.
     * @throws NoSuchPaddingException
     *             if the requested padding scheme in the <i>transformation</i>
     *             is not available.
     * @throws IllegalArgumentException
     *             if the specified provider is {@code null}.
     */
    static CARAPI GetInstance(
        /* [in] */ const String& transformation,
        /* [in] */ const String& provider,
        /* [out] */ ICipher ** cipher);

    /**
     * Creates a new cipher for the specified transformation. The
     * {@code provider} supplied does not have to be registered.
     *
     * @param transformation
     *            the name of the transformation to create a cipher for.
     * @param provider
     *            the provider to ask for the transformation.
     * @return a cipher for the requested transformation.
     * @throws NoSuchAlgorithmException
     *             if the specified provider can not provide the
     *             <i>transformation</i>, or it is {@code null}, empty or in an
     *             invalid format.
     * @throws NoSuchPaddingException
     *             if the requested padding scheme in the <i>transformation</i>
     *             is not available.
     * @throws IllegalArgumentException
     *             if the provider is {@code null}.
     */
    static CARAPI GetInstance(
        /* [in] */ const String& transformation,
        /* [in] */ IProvider * provider,
        /* [out] */ ICipher ** cipher);

    /**
     * Returns the maximum key length for the specified transformation.
     *
     * @param transformation
     *            the transformation name.
     * @return the maximum key length, currently {@code Integer.MAX_VALUE}.
     * @throws NoSuchAlgorithmException
     *             if no provider for the specified {@code transformation} can
     *             be found.
     * @throws NullPointerException
     *             if {@code transformation} is {@code null}.
     */
    static CARAPI GetMaxAllowedKeyLength(
        /* [in] */ const String& transformation,
        /* [out] */ Int32 * value);

    /**
     * Returns the maximum cipher parameter value for the specified
     * transformation. If there is no maximum limit, {@code null} is returned.
     *
     * @param transformation
     *            the transformation name.
     * @return a parameter spec holding the maximum value or {@code null}.
     *         Currently {@code null}.
     * @throws NoSuchAlgorithmException
     *             if no provider for the specified {@code transformation} can
     *             be found.
     * @throws NullPointerException
     *             if {@code transformation} is {@code null}.
    */
    static CARAPI GetMaxAllowedParameterSpec(
        /* [in] */ const String& transformation,
        /* [out] */ IAlgorithmParameterSpec ** value);

private:
    // static NoSuchAlgorithmException invalidTransformation(String transformation);

    /**
     * Create a Cipher instance but don't choose a CipherSpi until we have more
     * information.
     */
    static CARAPI GetCipher(
        /* [in] */ const String& transformation,
        /* [in] */ IProvider* provider,
        /* [out] */ ICipher** out);

    static CARAPI CheckTransformation(
        /* [in] */ const String& transformation,
        /* [out] */ ArrayOf<String>** out);

    /**
     * Makes sure a CipherSpi that matches this type is selected.
     */
    CARAPI GetSpi(
        /* [in] */ IKey* key,
        /* [out] */ ICipherSpi** spi);

    /**
     * Convenience call when the Key is not available.
     */
    CARAPI GetSpi(
        /* [out] */ ICipherSpi** spi);

    /**
     * Try all combinations of mode strings:
     *
     * <pre>
     *   [cipher]/[mode]/[padding]
     *   [cipher]/[mode]
     *   [cipher]//[padding]
     *   [cipher]
     * </pre>
     */
   static CARAPI TryCombinations(
        /* [in] */ IKey* key,
        /* [in] */ IProvider* provider,
        /* [in] */ ArrayOf<String> * transformParts,
        /* [out] */ ISpiAndProvider** result);

   static CARAPI TryTransform(
        /* [in] */ IKey* key,
        /* [in] */ IProvider* provider,
        /* [in] */ const String& transform,
        /* [in] */ ArrayOf<String> * transformParts,
        /* [in] */ NeedToSet type,
        /* [out] */ ISpiAndProvider** result);

   static CARAPI TryTransformWithProvider(
        /* [in] */ IKey* key,
        /* [in] */ ArrayOf<String> * transformParts,
        /* [in] */ NeedToSet type,
        /* [in] */ IProviderService * service,
        /* [out] */ ISpiAndProvider** result);

    /**
     * If the attribute listed exists, check that it matches the regular
     * expression.
     */
    static CARAPI_(Boolean) MatchAttribute(
        /* [in] */ IProviderService* service,
        /* [in] */ const String& attr,
        /* [in] */ const String& value);

    CARAPI CheckMode(
        /* [in] */ Int32 mode);

    static CARAPI CheckInputOffsetAndCount(
        /* [in] */ Int32 inputArrayLength,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen);

private:
    Int32 mMode;

    /**
     * The service name.
     */
    static const String SERVICE; // = "Cipher";

    /**
     * Used to access common engine functionality.
     */
    static AutoPtr<IEngine> ENGINE; // = new Engine(SERVICE);

    /** The attribute used for supported paddings. */
    static const String ATTRIBUTE_PADDINGS; // = "SupportedPaddings";

    /** The attribute used for supported modes. */
    static const String ATTRIBUTE_MODES; // = "SupportedModes";

    /**
     * The provider.
     */
    AutoPtr<IProvider> mProvider;

    /**
     * The provider specified when instance created.
     */
    AutoPtr<IProvider> mSpecifiedProvider;

    /**
     * The SPI implementation.
     */
    AutoPtr<ICipherSpi> mSpiImpl;

    /**
     * The SPI implementation.
     */
    AutoPtr<ICipherSpi> mSpecifiedSpi;

    /**
     * The transformation.
     */
    String mTransformation;

    /**
     * The transformation split into parts.
     */
    AutoPtr<ArrayOf<String> > mTransformParts;

    /**
     * Lock held while the SPI is initializing.
     */
    Object mInitLock;

    static AutoPtr<ISecureRandom> mSecureRandom;
};

}
}

#endif // __ELASTOSX_CRYPTO_CIPHER_H__
