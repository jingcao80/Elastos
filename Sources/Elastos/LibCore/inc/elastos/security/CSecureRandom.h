
#ifndef __ELASTOS_SECURITY_CSECURERANDOM_H__
#define __ELASTOS_SECURITY_CSECURERANDOM_H__

#include "_Elastos_Security_CSecureRandom.h"
#include "Random.h"

using Elastos::Utility::Random;
using Org::Apache::Harmony::Security::Fortress::IEngine;

namespace Elastos {
namespace Security {

CarClass(CSecureRandom)
    , public Random
    , public ISecureRandom
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* seed);

    CARAPI constructor(
        /* [in] */ ISecureRandomSpi* secureRandomSpi,
        /* [in] */ IProvider* provider,
        /* [in] */ String algorithm);

    CARAPI constructor(
        /* [in] */ ISecureRandomSpi* secureRandomSpi,
        /* [in] */ IProvider* provider);

    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ ISecureRandom** result) /*throws NoSuchAlgorithmException*/;

    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ ISecureRandom** result) /*throws NoSuchAlgorithmException, NoSuchProviderException*/;

    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider* provider,
        /* [out] */ ISecureRandom** result) /*throws NoSuchAlgorithmException*/;

    CARAPI GetProvider(
        /* [out] */ IProvider** provider);

    /**
     * Returns the name of the algorithm of this {@code SecureRandom}.
     *
     * @return the name of the algorithm of this {@code SecureRandom}.
     */
    CARAPI GetAlgorithm(
        /* [out] */ String* result);

    /**
     * Seeds this {@code SecureRandom} instance with the specified {@code
     * seed}. <a href="#insecure_seed">Seeding {@code SecureRandom} may be
     * insecure</a>.
     */
    CARAPI SetSeed(
        /* [in] */ ArrayOf<Byte>* seed);

    /**
     * Seeds this {@code SecureRandom} instance with the specified eight-byte
     * {@code seed}. <a href="#insecure_seed">Seeding {@code SecureRandom} may
     * be insecure</a>.
     */
    // @Override
    CARAPI SetSeed(
        /* [in] */ Int64 seed);

    /**
     * Generates and stores random bytes in the given {@code byte[]} for each
     * array element.
     *
     * @param bytes
     *            the {@code byte[]} to be filled with random bytes.
     */
    // @Override
    CARAPI NextBytes(
        /* [in] */ ArrayOf<Byte>* bytes);

    /**
     * Generates and returns the specified number of seed bytes, computed using
     * the seed generation algorithm used by this {@code SecureRandom}.
     *
     * @param numBytes
     *            the number of seed bytes.
     * @return the seed bytes
     */
    static CARAPI GetSeed(
        /* [in] */ Int32 numBytes,
        /* [out] */ ArrayOf<Byte>** seed);

    /**
     * Generates and returns the specified number of seed bytes, computed using
     * the seed generation algorithm used by this {@code SecureRandom}.
     *
     * @param numBytes
     *            the number of seed bytes.
     * @return the seed bytes.
     */
    CARAPI GenerateSeed(
        /* [in] */ Int32 numBytes,
        /* [out] */ ArrayOf<Byte>** seed);

protected:
    /**
     * Generates and returns an {@code Int32} containing the specified number of
     * random bits (right justified, with leading zeros).
     *
     * @param numBits
     *            number of bits to be generated. An input value should be in
     *            the range [0, 32].
     * @return an {@code Int32} containing the specified number of random bits.
     */
    // @Override
    CARAPI_(Int32) Next(
        /* [in] */ Int32 numBits);

private:
    // private static final long serialVersionUID = 4940670005562187L;

    // The service name.
    static const String SERVICE;

    // Used to access common engine functionality
    static AutoPtr<IEngine> ENGINE;

    AutoPtr<IProvider> mProvider;

    AutoPtr<ISecureRandomSpi> mSecureRandomSpi;

    String mAlgorithm;

    // Internal SecureRandom used for getSeed(Int32)
    static volatile AutoPtr<ISecureRandom> internalSecureRandom;
    Object THIS;
};

}
}

#endif // __ELASTOS_SECURITY_CSECURERANDOM_H__
