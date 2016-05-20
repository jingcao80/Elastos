
#ifndef __ELASTOS_SECURITY_SIGNATURE_H__
#define __ELASTOS_SECURITY_SIGNATURE_H__

#include "coredef.h"
#include "SignatureSpi.h"

// using Elastos::Security::Cert::ICertificate;
using Org::Apache::Harmony::Security::Fortress::IEngine;
using Org::Apache::Harmony::Security::Fortress::ISpiAndProvider;

namespace Elastos {
namespace Security {

class Signature
    : public SignatureSpi
    , public ISignature
{
    friend class SignatureImpl;

public:
    CAR_INTERFACE_DECL()

    /**
     * Returns a new instance of {@code Signature} that utilizes the specified
     * algorithm.
     *
     * @param algorithm
     *            the name of the algorithm to use.
     * @return a new instance of {@code Signature} that utilizes the specified
     *         algorithm.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not available.
     * @throws NullPointerException
     *             if {@code algorithm} is {@code null}.
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ ISignature** instance);

    /**
     * Returns a new instance of {@code Signature} that utilizes the specified
     * algorithm from the specified provider.
     *
     * @param algorithm
     *            the name of the algorithm to use.
     * @param provider
     *            the name of the provider.
     * @return a new instance of {@code Signature} that utilizes the specified
     *         algorithm from the specified provider.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not available.
     * @throws NoSuchProviderException
     *             if the specified provider is not available.
     * @throws NullPointerException
     *             if {@code algorithm} is {@code null}.
     * @throws IllegalArgumentException if {@code provider == null || provider.isEmpty()}
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ ISignature** instance);

    /**
     * Returns a new instance of {@code Signature} that utilizes the specified
     * algorithm from the specified provider. The {@code provider} supplied
     * does not have to be registered.
     *
     * @param algorithm
     *            the name of the algorithm to use.
     * @param provider
     *            the security provider.
     * @return a new instance of {@code Signature} that utilizes the specified
     *         algorithm from the specified provider.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not available.
     * @throws NullPointerException
     *             if {@code algorithm} is {@code null}.
     * @throws IllegalArgumentException if {@code provider == null}
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider* provider,
        /* [out] */ ISignature** instance);

    /**
     * Returns the provider associated with this {@code Signature}.
     *
     * @return the provider associated with this {@code Signature}.
     */
    CARAPI GetProvider(
        /* [out] */ IProvider** provider);

    /**
     * This makes sure the provider is chosen since Signature is abstract and
     * getProvider is final but we need to support late binding.
     */
    CARAPI EnsureProviderChosen();

    /**
     * Returns the name of the algorithm of this {@code Signature}.
     *
     * @return the name of the algorithm of this {@code Signature}.
     */
    CARAPI GetAlgorithm(
        /* [out] */ String* algo);

    /**
     * Initializes this {@code Signature} instance for signature verification,
     * using the public key of the identity whose signature is going to be
     * verified.
     *
     * @param publicKey
     *            the public key.
     * @throws InvalidKeyException
     *             if {@code publicKey} is not valid.
     */
    CARAPI InitVerify(
        /* [in] */ IPublicKey* publicKey);

    /**
     * Initializes this {@code Signature} instance for signature verification,
     * using the certificate of the identity whose signature is going to be
     * verified.
     * <p>
     * If the given certificate is an instance of {@link X509Certificate} and
     * has a key usage parameter that indicates, that this certificate is not to
     * be used for signing, an {@code InvalidKeyException} is thrown.
     *
     * @param certificate
     *            the certificate used to verify a signature.
     * @throws InvalidKeyException
     *             if the publicKey in the certificate is not valid or not to be
     *             used for signing.
     */
    CARAPI InitVerify(
        /* [in] */ Elastos::Security::Cert::ICertificate* certificate);

    /**
     * Initializes this {@code Signature} instance for signing, using the
     * private key of the identity whose signature is going to be generated.
     *
     * @param privateKey
     *            the private key.
     * @throws InvalidKeyException
     *             if {@code privateKey} is not valid.
     */
    CARAPI InitSign(
        /* [in] */ IPrivateKey* privateKey);

    /**
     * Initializes this {@code Signature} instance for signing, using the
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
    CARAPI InitSign(
        /* [in] */ IPrivateKey* privateKey,
        /* [in] */ ISecureRandom* random);

    /**
     * Generates and returns the signature of all updated data.
     * <p>
     * This {@code Signature} instance is reset to the state of its last
     * initialization for signing and thus can be used for another signature
     * from the same identity.
     *
     * @return the signature of all updated data.
     * @throws SignatureException
     *             if this {@code Signature} instance is not initialized
     *             properly.
     */
    CARAPI Sign(
        /* [out, callee] */ ArrayOf<Byte>** sign);

    /**
     * Generates and stores the signature of all updated data in the provided
     * {@code byte[]} at the specified position with the specified length.
     * <p>
     * This {@code Signature} instance is reset to the state of its last
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
     *             if this {@code Signature} instance is not initialized
     *             properly.
     * @throws IllegalArgumentException
     *             if {@code offset} or {@code len} are not valid in respect to
     *             {@code outbuf}.
     */
    CARAPI Sign(
        /* [in, out] */ ArrayOf<Byte>* outbuf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len,
        /* [out] */ Int32* number);

    /**
     * Indicates whether the given {@code signature} can be verified using the
     * public key or a certificate of the signer.
     * <p>
     * This {@code Signature} instance is reset to the state of its last
     * initialization for verifying and thus can be used to verify another
     * signature of the same signer.
     *
     * @param signature
     *            the signature to verify.
     * @return {@code true} if the signature was verified, {@code false}
     *         otherwise.
     * @throws SignatureException
     *             if this {@code Signature} instance is not initialized
     *             properly.
     */
    CARAPI Verify(
        /* [in] */ ArrayOf<Byte>* signature,
        /* [out] */ Boolean* result);

    /**
     * Indicates whether the given {@code signature} starting at index {@code
     * offset} with {@code length} bytes can be verified using the public key or
     * a certificate of the signer.
     * <p>
     * This {@code Signature} instance is reset to the state of its last
     * initialization for verifying and thus can be used to verify another
     * signature of the same signer.
     *
     * @param signature
     *            the {@code byte[]} containing the signature to verify.
     * @param offset
     *            the start index in {@code signature} of the signature.
     * @param length
     *            the number of bytes allocated for the signature.
     * @return {@code true} if the signature was verified, {@code false}
     *         otherwise.
     * @throws SignatureException
     *             if this {@code Signature} instance is not initialized
     *             properly.
     * @throws IllegalArgumentException
     *             if {@code offset} or {@code length} are not valid in respect
     *             to {@code signature}.
     */
    CARAPI Verify(
        /* [in] */ ArrayOf<Byte>* signature,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Boolean* result);

    /**
     * Updates the data to be verified or to be signed, using the specified
     * {@code byte}.
     *
     * @param b
     *            the byte to update with.
     * @throws SignatureException
     *             if this {@code Signature} instance is not initialized
     *             properly.
     */
    CARAPI Update(
        /* [in] */ Byte b);

    /**
     * Updates the data to be verified or to be signed, using the specified
     * {@code byte[]}.
     *
     * @param data
     *            the byte array to update with.
     * @throws SignatureException
     *             if this {@code Signature} instance is not initialized
     *             properly.
     */
    CARAPI Update(
        /* [in] */ ArrayOf<Byte>* data);

    /**
     * Updates the data to be verified or to be signed, using the given {@code
     * byte[]}, starting form the specified index for the specified length.
     *
     * @param data
     *            the byte array to update with.
     * @param off
     *            the start index in {@code data} of the data.
     * @param len
     *            the number of bytes to use.
     * @throws SignatureException
     *             if this {@code Signature} instance is not initialized
     *             properly.
     */
    CARAPI Update(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len);

    /**
     * Updates the data to be verified or to be signed, using the specified
     * {@code ByteBuffer}.
     *
     * @param data
     *            the {@code ByteBuffer} to update with.
     * @throws SignatureException
     *             if this {@code Signature} instance is not initialized
     *             properly.
     */
    CARAPI Update(
        /* [in] */ IByteBuffer* data);

    /**
     * Returns a string containing a concise, human-readable description of this
     * {@code Signature} including its algorithm and its state.
     *
     * @return a printable representation for this {@code Signature}.
     */
    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

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
     * @deprecated Use {@link #setParameter(AlgorithmParameterSpec)} instead.
     */
    // @Deprecated
    CARAPI SetParameter(
        /* [in] */ const String& param,
        /* [in] */ IInterface* value);

    /**
     * Sets the specified {@code AlgorithmParameterSpec}.
     *
     * @param params
     *            the parameter to set.
     * @throws InvalidAlgorithmParameterException
     *             if the parameter is invalid, already set or is not allowed to
     *             be changed.
     */
    CARAPI SetParameter(
        /* [in] */ IAlgorithmParameterSpec* params);

    /**
     * Returns the {@code AlgorithmParameters} of this {@link Signature}
     * instance.
     *
     * @return the {@code AlgorithmParameters} of this {@link Signature}
     *         instance, maybe {@code null}.
     */
    CARAPI GetParameters(
        /* [out] */ IAlgorithmParameters** params);

    /**
     * Returns the value of the parameter with the specified name.
     *
     * @param param
     *            the name of the requested parameter value
     * @return the value of the parameter with the specified name, maybe {@code
     *         null}.
     * @throws InvalidParameterException
     *             if {@code param} is not a valid parameter for this {@code
     *             Signature} or an other error occurs.
     * @deprecated There is no generally accepted parameter naming convention.
     */
    // @Deprecated
    CARAPI GetParameter(
        /* [in] */ const String& param,
        /* [out] */ IInterface** value);

protected:
    /**
     * Constructs a new instance of {@code Signature} with the name of
     * the algorithm to use.
     *
     * @param algorithm
     *            the name of algorithm to use.
     */
    CARAPI constructor(
        /* [in] */ const String& algorithm);

private:
    static CARAPI GetSignature(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider* provider,
        /* [out] */ ISignature** signature);

    static CARAPI TryAlgorithm(
        /* [in] */ IKey* key,
        /* [in] */ IProvider* provider,
        /* [in] */ const String& algorithm,
        /* [out] */ ISpiAndProvider** spi);

    static CARAPI TryAlgorithmWithProvider(
        /* [in] */ IKey* key,
        /* [in] */ IProviderService* service,
        /* [out] */ ISpiAndProvider** spi);

    // Convert state to string
    CARAPI_(String) StateToString(
        /* [in] */ Int32 state);

    static CARAPI_(AutoPtr<IEngine>) Init_ENGINE();

public:
    // The provider
    AutoPtr<IProvider> mProvider;

    // The algorithm.
    String mAlgorithm;

protected:
    /**
     * Constant that indicates that this {@code Signature} instance has not yet
     * been initialized.
     */
    static const Int32 UNINITIALIZED;

    /**
     * Constant that indicates that this {@code Signature} instance has been
     * initialized for signing.
     */
    static const Int32 SIGN;

    /**
     * Constant that indicates that this {@code Signature} instance has been
     * initialized for verification.
     */
    static const Int32 VERIFY;

    /**
     * Represents the current state of this {@code Signature}. The three
     * possible states are {@link #UNINITIALIZED}, {@link #SIGN} or
     * {@link #VERIFY}.
     */
    Int32 mState;

private:
    // The service name.
    static const String SERVICE;

    // Used to access common engine functionality
    static AutoPtr<IEngine> ENGINE;
};

} // namespace Security
} // namespace Elastos

#endif //__ELASTOS_SECURITY_SIGNATURE_H__
