
#ifndef __ELASTOSX_NET_SSL_KEYMANAGERFACTORY_H__
#define __ELASTOSX_NET_SSL_KEYMANAGERFACTORY_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Security.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Security::IProvider;
using Elastos::Security::IKeyStore;
using Elastosx::Net::Ssl::IKeyManager;
using Elastosx::Net::Ssl::IManagerFactoryParameters;
using Elastosx::Net::Ssl::IKeyManagerFactorySpi;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * The public API for {@code KeyManagerFactory} implementations.
 */
class KeyManagerFactory
    : public Object
    , public IKeyManagerFactory
{
public:
    CAR_INTERFACE_DECL()

    KeyManagerFactory();

    CARAPI constructor(
        /* [in] */ IKeyManagerFactorySpi* factorySpi,
        /* [in] */ IProvider* provider,
        /* [in] */ const String& algorithm);

    /**
     * Returns the default key manager factory algorithm name.
     * <p>
     * The default algorithm name is specified by the security property:
     * {@code 'ssl.KeyManagerFactory.algorithm'}.
     *
     * @return the default algorithm name.
     */
    static CARAPI GetDefaultAlgorithm(
        /* [out] */ String* algorithm);

    /**
     * Creates a new {@code KeyManagerFactory} instance for the specified key
     * management algorithm.
     *
     * @param algorithm
     *            the name of the requested key management algorithm.
     * @return a key manager factory for the requested algorithm.
     * @throws NoSuchAlgorithmException
     *             if no installed provider can provide the requested algorithm.
     * @throws NullPointerException
     *             if {@code algorithm} is {@code null} (instead of
     *             NoSuchAlgorithmException as in 1.4 release)
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ IKeyManagerFactory** factory);

    /**
     * Creates a new {@code KeyManagerFactory} instance for the specified key
     * management algorithm from the specified provider.
     *
     * @param algorithm
     *            the name of the requested key management algorithm name.
     * @param provider
     *            the name of the provider that provides the requested
     *            algorithm.
     * @return a key manager factory for the requested algorithm.
     * @throws NoSuchAlgorithmException
     *             if the specified provider cannot provide the requested
     *             algorithm.
     * @throws NoSuchProviderException
     *             if the specified provider does not exist.
     * @throws NullPointerException
     *             if {@code algorithm} is {@code null} (instead of
     *             NoSuchAlgorithmException as in 1.4 release)
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ IKeyManagerFactory** factory);

    /**
     * Creates a new {@code KeyManagerFactory} instance for the specified key
     * management algorithm from the specified provider.
     *
     * @param algorithm
     *            the name of the requested key management algorithm name.
     * @param provider
     *            the provider that provides the requested algorithm.
     * @return a key manager factory for the requested algorithm.
     * @throws NoSuchAlgorithmException
     *             if the specified provider cannot provide the requested
     *             algorithm.
     * @throws NullPointerException
     *             if {@code algorithm} is {@code null} (instead of
     *             NoSuchAlgorithmException as in 1.4 release)
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider* provider,
        /* [out] */ IKeyManagerFactory** factory);

    /**
     * Creates a new {@code KeyManagerFactory}.
     *
     * @param factorySpi
     *            the implementation delegate.
     * @param provider
     *            the provider.
     * @param algorithm
     *            the key management algorithm name.
     */
    CARAPI contructor(
        /* [in] */ IKeyManagerFactorySpi* factorySpi,
        /* [in] */ IProvider* provider,
        /* [in] */ const String& algorithm);

    /**
     * Returns the name of the key management algorithm.
     *
     * @return the name of the key management algorithm.
     */
    CARAPI GetAlgorithm(
        /* [out] */ String* algorithm);

    /**
     * Returns the provider for this {@code KeyManagerFactory} instance.
     *
     * @return the provider for this {@code KeyManagerFactory} instance.
     */
    CARAPI GetProvider(
        /* [out] */ IProvider** provider);

    /**
     * Initializes this instance with the specified key store and password.
     *
     * @param ks
     *            the key store or {@code null} to use the default key store.
     * @param password
     *            the password for the specified key store or {@code null} if no
     *            key store is provided.
     * @throws KeyStoreException
     *             if initializing this key manager factory fails.
     * @throws NoSuchAlgorithmException
     *             if a required algorithm is not available.
     * @throws UnrecoverableKeyException
     *             if a key cannot be recovered.
     */
    CARAPI Init(
        /* [in] */ IKeyStore* ks,
        /* [in] */ ArrayOf<Char32>* password);

    /**
     * Initializes this instance with the specified factory parameters.
     *
     * @param spec
     *            the factory parameters.
     * @throws InvalidAlgorithmParameterException
     *             if an error occurs.
     */
    CARAPI Init(
        /* [in] */ IManagerFactoryParameters* spec);

    /**
     * Returns a list of key managers, one instance for each type of key in the
     * key store.
     *
     * @return a list of key managers.
     */
    CARAPI GetKeyManagers(
        /* [out, callee] */ ArrayOf<IKeyManager*>** managers);

private:
    // Store KeyManagerFactory service name
    static const String SERVICE;

    // Used to access common engine functionality
    //TUDO static const Engine ENGINE;

    // Store default property name
    static const String PROPERTY_NAME;

    // Default value of KeyManagerFactory type.
    static const String DEFAULT_PROPERTY;

    // Store used provider
    /*const*/ AutoPtr<IProvider> mProvider;

    // Store used KeyManagerFactorySpi implementation
    /*const*/ AutoPtr<IKeyManagerFactorySpi> mSpiImpl;

    // Store used algorithm
    /*const*/ String mAlgorithm;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_SSL_KEYMANAGERFACTORY_H__
