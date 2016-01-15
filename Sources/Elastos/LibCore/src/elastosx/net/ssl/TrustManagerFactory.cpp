
#include "Elastos.CoreLibrary.Security.h"
#include "TrustManagerFactory.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

const String TrustManagerFactory::SERVICE = String("TrustManagerFactory");
//TODO Engine TrustManagerFactory::ENGINE = new Engine(SERVICE);
const String TrustManagerFactory::PROPERTY_NAME = String("ssl.TrustManagerFactory.algorithm");
const String TrustManagerFactory::DEFAULT_PROPERTY = String("PKIX");

CAR_INTERFACE_IMPL(TrustManagerFactory, Object, ITrustManagerFactory)

ECode TrustManagerFactory::GetDefaultAlgorithm(
    /* [out] */ String* algorithm)
{
    VALIDATE_NOT_NULL(algorithm)

    String _algorithm;
    assert(0 && "TODO");
    // TODO Security::GetProperty(PROPERTY_NAME, &_algorithm);
    *algorithm = (!_algorithm.IsNull() ? _algorithm : DEFAULT_PROPERTY);
    return NOERROR;
}

ECode TrustManagerFactory::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ ITrustManagerFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    if (algorithm.IsNull()) {
        //throw new NullPointerException("algorithm == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    assert(0 && "TODO");
    // TODO Engine.SpiAndProvider sap = ENGINE.getInstance(algorithm, null);
    // return new TrustManagerFactory((TrustManagerFactorySpi) sap.spi, sap.provider, algorithm);
    return NOERROR;
}

ECode TrustManagerFactory::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ ITrustManagerFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    if ((provider.IsNull()) || (provider.GetLength() == 0)) {
        //throw new IllegalArgumentException("Provider is null or empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IProvider> impProvider;
    // TODO Security::GetProvider(provider);
    if (impProvider == NULL) {
        //throw new NoSuchProviderException(provider);
        return E_NO_SUCH_PROVIDER_EXCEPTION;
    }
    return GetInstance(algorithm, impProvider, factory);
}

ECode TrustManagerFactory::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider* provider,
    /* [out] */ ITrustManagerFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    if (provider == NULL) {
        //throw new IllegalArgumentException("Provider is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (algorithm.IsNull()) {
        //throw new NullPointerException("algorithm == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    // TODO Object spi = ENGINE.getInstance(algorithm, provider, null);
    // return new TrustManagerFactory((TrustManagerFactorySpi) spi, provider, algorithm);
    return NOERROR;
}

ECode TrustManagerFactory::constructor(
    /* [in] */ ITrustManagerFactorySpi* factorySpi,
    /* [in] */ IProvider* provider,
    /* [in] */ const String& algorithm)
{
    mProvider = provider;
    mAlgorithm = algorithm;
    mSpiImpl = factorySpi;
    return NOERROR;
}

ECode TrustManagerFactory::GetAlgorithm(
    /* [out] */ String* algorithm)
{
    VALIDATE_NOT_NULL(algorithm)

    *algorithm = mAlgorithm;
    return NOERROR;
}

ECode TrustManagerFactory::GetProvider(
    /* [out] */ IProvider** provider)
{
    VALIDATE_NOT_NULL(provider)

    *provider = mProvider;
    REFCOUNT_ADD(*provider)
    return NOERROR;
}

ECode TrustManagerFactory::Init(
    /* [in] */ IKeyStore* ks)
{
   return mSpiImpl->EngineInit(ks);
}

ECode TrustManagerFactory::Init(
    /* [in] */ IManagerFactoryParameters* spec)
{
    return mSpiImpl->EngineInit(spec);
}

ECode TrustManagerFactory::GetTrustManagers(
    /* [out, callee] */ ArrayOf<ITrustManager*>** managers)
{
    VALIDATE_NOT_NULL(managers)

    return mSpiImpl->EngineGetTrustManagers(managers);
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
