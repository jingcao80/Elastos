
#ifndef __ELASTOSX_CRYPTO_KEYGENERATORSPI_H__
#define __ELASTOSX_CRYPTO_KEYGENERATORSPI_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Security::ISecureRandom;
using Elastos::Security::Spec::IAlgorithmParameterSpec;

namespace Elastosx {
namespace Crypto {

class KeyGeneratorSpi
    : public Object
    , public IKeyGeneratorSpi
{
public:
    CAR_INTERFACE_DECL()

    KeyGeneratorSpi();

    virtual ~KeyGeneratorSpi();

    /**
     * Creates a new {@code KeyGeneratorSpi} instance.
     */
    CARAPI constructor();

    /**
     * Generates a secret key.
     *
     * @return the generated secret key.
     */
    virtual CARAPI EngineGenerateKey(
        /* [out] */ ISecretKey** result) = 0;

    /**
     * Initializes this {@code KeyGeneratorSpi} instance with the specified
     * algorithm parameters and randomness source.
     *
     * @param params
     *            the parameters for the key generation algorithm.
     * @param random
     *            the randomness source for any random bytes.
     * @throws InvalidAlgorithmParameterException
     *             if the parameters cannot be uses to initialize this key
     *             generator algorithm.
     */
    virtual CARAPI EngineInit(
        /* [in] */ IAlgorithmParameterSpec* params,
        /* [in] */ ISecureRandom* random) = 0;

    /**
     * Initializes this {@code KeyGenerator} instance for the specified key
     * size (in bits) using the specified randomness source.
     *
     * @param keysize
     *            the size of the key (in bits).
     * @param random
     *            the randomness source for any random bytes.
     */
    virtual CARAPI EngineInit(
        /* [in] */ Int32 keysize,
        /* [in] */ ISecureRandom* random) = 0;

    /**
     * Initializes this {@code KeyGenerator} with the specified randomness
     * source.
     *
     * @param random
     *            the randomness source for any random bytes.
     */
    virtual CARAPI EngineInit(
        /* [in] */ ISecureRandom* random) = 0;

};

}
}

#endif // __ELASTOSX_CRYPTO_KEYGENERATORSPI_H__
