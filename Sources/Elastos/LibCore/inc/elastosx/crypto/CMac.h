
#ifndef __ELASTOSX_CRYPTO_CMAC_H__
#define __ELASTOSX_CRYPTO_CMAC_H__

#include "Elastos.CoreLibrary.Security.h"
#include "_Elastosx_Crypto_CMac.h"
#include <elastos/core/Object.h>

using Elastos::IO::IByteBuffer;
using Elastos::Core::ICloneable;
using Elastos::Security::IKey;
using Elastos::Security::IProvider;
using Elastos::Security::IProviderService;
using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Org::Apache::Harmony::Security::Fortress::IEngine;
using Org::Apache::Harmony::Security::Fortress::ISpiAndProvider;

namespace Elastosx {
namespace Crypto {

CarClass(CMac)
    , public Object
    , public IMac
    , public ICloneable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CMac();

    /**
     * Creates a new {@code Mac} instance.
     *
     * @param macSpi
     *            the implementation delegate.
     * @param provider
     *            the implementation provider.
     * @param algorithm
     *            the name of the MAC algorithm.
     */
    CARAPI constructor(
        /* [in] */ IMacSpi* macSpi,
        /* [in] */ IProvider* provider,
        /* [in] */ const String& algorithm);

    /**
     * Returns the name of the MAC algorithm.
     *
     * @return the name of the MAC algorithm.
     */
    CARAPI GetAlgorithm(
        /* [out] */ String * name);

    /**
     * Returns the provider for this {@code Mac} instance.
     *
     * @return the provider for this {@code Mac} instance.
     */
    CARAPI GetProvider(
        /* [out] */ IProvider ** provider);

    /**
     * Returns the length of this MAC (in bytes).
     *
     * @return the length of this MAC (in bytes).
     */
    CARAPI GetMacLength(
        /* [out] */ Int32 * len);

    /**
     * Initializes this {@code Mac} instance with the specified key and
     * algorithm parameters.
     *
     * @param key
     *            the key to initialize this algorithm.
     * @param params
     *            the parameters for this algorithm.
     * @throws InvalidKeyException
     *             if the specified key cannot be used to initialize this
     *             algorithm, or it is null.
     * @throws InvalidAlgorithmParameterException
     *             if the specified parameters cannot be used to initialize this
     *             algorithm.
     */
    CARAPI Init(
        /* [in] */ IKey * key,
        /* [in] */ IAlgorithmParameterSpec* spec);

    /**
     * Initializes this {@code Mac} instance with the specified key.
     *
     * @param key
     *            the key to initialize this algorithm.
     * @throws InvalidKeyException
     *             if initialization fails because the provided key is {@code
     *             null}.
     * @throws RuntimeException
     *             if the specified key cannot be used to initialize this
     *             algorithm.
     */
    CARAPI Init(
        /* [in] */ IKey * key);

    /**
     * Updates this {@code Mac} instance with the specified byte.
     *
     * @param input
     *            the byte
     * @throws IllegalStateException
     *             if this MAC is not initialized.
     */
    CARAPI Update(
        /* [in] */ Byte input);

    /**
     * Updates this {@code Mac} instance with the data from the specified buffer
     * {@code input} from the specified {@code offset} and length {@code len}.
     *
     * @param input
     *            the buffer.
     * @param offset
     *            the offset in the buffer.
     * @param len
     *            the length of the data in the buffer.
     * @throws IllegalStateException
     *             if this MAC is not initialized.
     * @throws IllegalArgumentException
     *             if {@code offset} and {@code len} do not specified a valid
     *             chunk in {@code input} buffer.
     */
    CARAPI Update(
        /* [in] */ ArrayOf<Byte> * input,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len);

    /**
     * Copies the buffer provided as input for further processing.
     *
     * @param input
     *            the buffer.
     * @throws IllegalStateException
     *             if this MAC is not initialized.
     */
    CARAPI Update(
        /* [in] */ ArrayOf<Byte> * input);

    /**
     * Updates this {@code Mac} instance with the data from the specified
     * buffer, starting at {@link ByteBuffer#position()}, including the next
     * {@link ByteBuffer#remaining()} bytes.
     *
     * @param input
     *            the buffer.
     * @throws IllegalStateException
     *             if this MAC is not initialized.
     */
    CARAPI Update(
        /* [in] */ IByteBuffer * input);

    /**
     * Computes the digest of this MAC based on the data previously specified in
     * {@link #update} calls.
     * <p>
     * This {@code Mac} instance is reverted to its initial state and can be
     * used to start the next MAC computation with the same parameters or
     * initialized with different parameters.
     *
     * @return the generated digest.
     * @throws IllegalStateException
     *             if this MAC is not initialized.
     */
    CARAPI DoFinal(
        /* [out, callee] */ ArrayOf<Byte> ** result);

    /**
     * Computes the digest of this MAC based on the data previously specified in
     * {@link #update} calls and stores the digest in the specified {@code
     * output} buffer at offset {@code outOffset}.
     * <p>
     * This {@code Mac} instance is reverted to its initial state and can be
     * used to start the next MAC computation with the same parameters or
     * initialized with different parameters.
     *
     * @param output
     *            the output buffer
     * @param outOffset
     *            the offset in the output buffer
     * @throws ShortBufferException
     *             if the specified output buffer is either too small for the
     *             digest to be stored, the specified output buffer is {@code
     *             null}, or the specified offset is negative or past the length
     *             of the output buffer.
     * @throws IllegalStateException
     *             if this MAC is not initialized.
     */
    CARAPI DoFinal(
        /* [in] */ ArrayOf<Byte> * output,
        /* [in] */ Int32 outOffset);

    /**
     * Computes the digest of this MAC based on the data previously specified on
     * {@link #update} calls and on the final bytes specified by {@code input}
     * (or based on those bytes only).
     * <p>
     * This {@code Mac} instance is reverted to its initial state and can be
     * used to start the next MAC computation with the same parameters or
     * initialized with different parameters.
     *
     * @param input
     *            the final bytes.
     * @return the generated digest.
     * @throws IllegalStateException
     *             if this MAC is not initialized.
     */
    CARAPI DoFinal(
        /* [in] */ ArrayOf<Byte> * output,
        /* [out, callee] */ ArrayOf<Byte> ** result);

    /**
     * Resets this {@code Mac} instance to its initial state.
     * <p>
     * This {@code Mac} instance is reverted to its initial state and can be
     * used to start the next MAC computation with the same parameters or
     * initialized with different parameters.
     */
    CARAPI Reset();

    /**
     * Creates a new {@code Mac} instance that provides the specified MAC
     * algorithm.
     *
     * @param algorithm
     *            the name of the requested MAC algorithm.
     * @return the new {@code Mac} instance.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not available by any provider.
     * @throws NullPointerException
     *             if {@code algorithm} is {@code null} (instead of
     *             NoSuchAlgorithmException as in 1.4 release).
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ IMac ** mac);

    /**
     * Creates a new {@code Mac} instance that provides the specified MAC
     * algorithm from the specified provider.
     *
     * @param algorithm
     *            the name of the requested MAC algorithm.
     * @param provider
     *            the name of the provider that is providing the algorithm.
     * @return the new {@code Mac} instance.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not provided by the specified
     *             provider.
     * @throws NoSuchProviderException
     *             if the specified provider is not available.
     * @throws IllegalArgumentException
     *             if the specified provider name is {@code null} or empty.
     * @throws NullPointerException
     *             if {@code algorithm} is {@code null} (instead of
     *             NoSuchAlgorithmException as in 1.4 release).
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ IMac ** mac);

    /**
     * Creates a new {@code Mac} instance that provides the specified MAC
     * algorithm from the specified provider. The {@code provider} supplied
     * does not have to be registered.
     *
     * @param algorithm
     *            the name of the requested MAC algorithm.
     * @param provider
     *            the provider that is providing the algorithm.
     * @return the new {@code Mac} instance.
     * @throws NoSuchAlgorithmException
     *             if the specified algorithm is not provided by the specified
     *             provider.
     * @throws IllegalArgumentException
     *             if {@code provider} is {@code null}.
     * @throws NullPointerException
     *             if {@code algorithm} is {@code null} (instead of
     *             NoSuchAlgorithmException as in 1.4 release).
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider * provider,
        /* [out] */ IMac ** mac);

    CARAPI Clone(
        /* [out] */ IInterface** result);

private:
    static CARAPI GetMac(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider * provider,
        /* [out] */ IMac ** mac);

    static CARAPI_(AutoPtr<ISpiAndProvider>) TryAlgorithm(
        /* [in] */ IKey * key,
        /* [in] */ IProvider * provider,
        /* [in] */ const String& algorithm);

    static CARAPI_(AutoPtr<ISpiAndProvider>) TryAlgorithmWithProvider(
        /* [in] */ IKey * key,
        /* [in] */ IProviderService* service);

    /**
     * Makes sure a MacSpi that matches this type is selected.
     */
    CARAPI GetSpi(
        /* [in] */ IKey* key,
        /* [out] */ IMacSpi** spi);

    /**
     * Convenience call when the Key is not available.
     */
    CARAPI GetSpi(
        /* [out] */ IMacSpi** spi);

private:
    // The service name.
    static String SERVICE; // = "Mac";

    // Used to access common engine functionality
    static AutoPtr<IEngine> ENGINE; // = new Engine(mSERVICE);

    // Store used provider
    AutoPtr<IProvider> mProvider;

    // Provider that was requested during creation.
    AutoPtr<IProvider> mSpecifiedProvider;

    // Store used spi implementation
    AutoPtr<IMacSpi> mSpiImpl;

    // Store used algorithm name
    String mAlgorithm;

    /**
     * Lock held while the SPI is initializing.
     */
    Object mInitLock; // = new Object();

    // Store Mac state (initialized or not initialized)
    Boolean mIsInitMac;

};

}
}

#endif // __ELASTOSX_CRYPTO_CMAC_H__
