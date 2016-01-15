
#ifndef __ELASTOSX_CRYPTO_SECRETKEYFACTORYSPI_H__
#define __ELASTOSX_CRYPTO_SECRETKEYFACTORYSPI_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include <elastos/core/Object.h>

using Elastos::Security::Spec::IKeySpec;
using Elastosx::Crypto::ISecretKey;;

namespace Elastosx {
namespace Crypto {

class SecretKeyFactorySpi
    : public Object
    , public ISecretKeyFactorySpi
{
public:
    CAR_INTERFACE_DECL()

    SecretKeyFactorySpi();

    virtual ~SecretKeyFactorySpi();

    /**
     * Creates a new {@code SecretKeyFactorySpi} instance.
     */
    CARAPI constructor();

    /**
     * Generate a secret key from the specified key specification.
     *
     * @param keySpec
     *            the key specification.
     * @return a secret key.
     * @throws InvalidKeySpecException
     *             if the specified key specification cannot be used to generate
     *             a secret key.
     */
    virtual CARAPI EngineGenerateSecret(
        /* [in] */ IKeySpec * keySpec,
        /* [out] */ ISecretKey** key) = 0;

    /**
     * Returns the key specification of the specified secret key.
     *
     * @param key
     *            the secret key to get the specification from.
     * @param keySpec
     *            the target key specification class.
     * @return an instance of the specified key specification class.
     * @throws InvalidKeySpecException
     *             if the specified secret key cannot be transformed into the
     *             requested key specification.
     */
    //@SuppressWarnings("unchecked")
    virtual CARAPI EngineGetKeySpec(
        /* [in] */ ISecretKey * key,
        /* [in] */ ClassID keySpec,
        /* [out] */ IKeySpec** spec) = 0;

    /**
     * Translates the specified secret key into an instance of the corresponding
     * key from the provider of this key factory.
     *
     * @param key
     *            the secret key to translate.
     * @return the corresponding translated key.
     * @throws InvalidKeyException
     *             if the specified key cannot be translated using this key
     *             factory.
     */
    virtual CARAPI EngineTranslateKey(
        /* [in] */ ISecretKey * key,
        /* [out] */ ISecretKey** out) = 0;

};

}
}

#endif // __ELASTOSX_CRYPTO_SECRETKEYFACTORYSPI_H__
