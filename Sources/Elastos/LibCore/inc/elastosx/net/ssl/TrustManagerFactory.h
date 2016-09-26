
#ifndef __ELASTOSX_NET_SSL_TRUSTMANAGERFACTORY_H__
#define __ELASTOSX_NET_SSL_TRUSTMANAGERFACTORY_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Security.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Security::IProvider;
using Elastos::Security::IKeyStore;
using Elastosx::Net::Ssl::ITrustManager;
using Elastosx::Net::Ssl::ITrustManagerFactorySpi;
using Elastosx::Net::Ssl::IManagerFactoryParameters;
using Org::Apache::Harmony::Security::Fortress::IEngine;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * The factory for {@code TrustManager}s based on {@code KeyStore} or provider
 * specific implementation.
 */
class TrustManagerFactory
    : public Object
    , public ITrustManagerFactory
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Returns the default algorithm name for the {@code TrustManagerFactory}. The
     * default algorithm name is specified by the security property
     * {@code 'ssl.TrustManagerFactory.algorithm'}.
     *
     * @return the default algorithm name.
     */
    static CARAPI GetDefaultAlgorithm(
        /* [out] */ String* algorithm);

    /**
     * Creates a new {@code TrustManagerFactory} instance for the specified
     * trust management algorithm.
     *
     * @param algorithm
     *            the name of the requested trust management algorithm.
     * @return a trust manager factory for the requested algorithm.
     * @throws NoSuchAlgorithmException
     *             if no installed provider can provide the requested algorithm.
     * @throws NullPointerException
     *             if {@code algorithm} is {@code null} (instead of
     *             NoSuchAlgorithmException as in 1.4 release)
     */
    static CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ ITrustManagerFactory** factory);

    /**
     * Creates a new {@code TrustManagerFactory} instance for the specified
     * trust management algorithm from the specified provider.
     *
     * @param algorithm
     *            the name of the requested trust management algorithm name.
     * @param provider
     *            the name of the provider that provides the requested
     *            algorithm.
     * @return a trust manager factory for the requested algorithm.
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
        /* [out] */ ITrustManagerFactory** factory);

    /**
     * Creates a new {@code TrustManagerFactory} instance for the specified
     * trust management algorithm from the specified provider.
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
        /* [out] */ ITrustManagerFactory** factory);

    /**
     * Creates a new {@code TrustManagerFactory} instance.
     *
     * @param factorySpi
     *            the implementation delegate.
     * @param provider
     *            the provider
     * @param algorithm
     *            the algorithm name.
     */
    CARAPI constructor(
        /* [in] */ ITrustManagerFactorySpi* factorySpi,
        /* [in] */ IProvider* provider,
        /* [in] */ const String& algorithm);

    /**
     * Returns the name of this {@code TrustManagerFactory} algorithm
     * implementation.
     *
     * @return the name of this {@code TrustManagerFactory} algorithm
     *         implementation.
     */
    CARAPI GetAlgorithm(
        /* [out] */ String* algorithm);

    /**
     * Returns the provider for this {@code TrustManagerFactory} instance.
     *
     * @return the provider for this {@code TrustManagerFactory} instance.
     */
    CARAPI GetProvider(
        /* [out] */ IProvider** provider);

    /**
     * Initializes this factory instance with the specified keystore as source
     * of certificate authorities and trust material.
     *
     * @param ks
     *            the keystore or {@code null}.
     * @throws KeyStoreException
     *             if the initialization fails.
     */
    CARAPI Init(
        /* [in] */ IKeyStore* ks);

    /**
     * Initializes this factory instance with the specified provider-specific
     * parameters for a source of trust material.
     *
     * @param spec
     *            the provider-specific parameters.
     * @throws InvalidAlgorithmParameterException
     *             if the initialization fails.
     */
    CARAPI Init(
        /* [in] */ IManagerFactoryParameters* spec);

    /**
     * Returns the list of {@code TrustManager}s with one entry for each type
     * of trust material.
     *
     * @return the list of {@code TrustManager}s
     */
    CARAPI GetTrustManagers(
        /* [out, callee] */ ArrayOf<ITrustManager*>** managers);

private:
    // Store TrustManager service name
    static const String SERVICE;

    // Used to access common engine functionality
    static AutoPtr<IEngine> ENGINE;

    // Store default property name
    static const String PROPERTY_NAME;

    // Default value of TrustManagerFactory type.
    static const String DEFAULT_PROPERTY;

    // Store used provider
    /*const*/ AutoPtr<IProvider> mProvider;

    // Store used TrustManagerFactorySpi implementation
    /*const*/ AutoPtr<ITrustManagerFactorySpi> mSpiImpl;

    // Store used algorithm
    /*const*/ String mAlgorithm;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_SSL_TRUSTMANAGERFACTORY_H__
