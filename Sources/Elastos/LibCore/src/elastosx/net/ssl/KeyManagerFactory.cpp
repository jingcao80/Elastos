
#include "Elastos.CoreLibrary.Security.h"
#include "KeyManagerFactory.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

const String KeyManagerFactory::SERVICE = String("KeyManagerFactory");
//TODO Engine KeyManagerFactory::ENGINE = new Engine(SERVICE);
const String KeyManagerFactory::PROPERTY_NAME = String("ssl.KeyManagerFactory.algorithm");
const String KeyManagerFactory::DEFAULT_PROPERTY = String("PKIX");

CAR_INTERFACE_IMPL(KeyManagerFactory, Object, IKeyManagerFactory)

ECode KeyManagerFactory::GetDefaultAlgorithm(
        /* [out] */ String* algorithm)
{
    VALIDATE_NOT_NULL(algorithm)

    String _algorithm;
    assert(0 && "TODO");
    // TODO Security::GetProperty(PROPERTY_NAME);
    *algorithm = (!_algorithm.IsNull() ? _algorithm : DEFAULT_PROPERTY);
    return NOERROR;
}

ECode KeyManagerFactory::GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ IKeyManagerFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    if (algorithm.IsNull()) {
        //throw new NullPointerException("algorithm == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    assert(0 && "TODO");
    // TODO Engine.SpiAndProvider sap = ENGINE.getInstance(algorithm, null);
    // return new KeyManagerFactory((KeyManagerFactorySpi) sap.spi, sap.provider, algorithm);
    return NOERROR;
}

ECode KeyManagerFactory::GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ IKeyManagerFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    if ((provider.IsNull()) || (provider.GetLength() == 0)) {
        //throw new IllegalArgumentException("Provider is null or empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IProvider> impProvider;
    assert(0 && "TODO");
    // TODO Security::GetProvider(provider);
    if (impProvider == NULL) {
        //throw new NoSuchProviderException(provider);
        return E_NO_SUCH_PROVIDER_EXCEPTION;
    }
    return GetInstance(algorithm, impProvider, factory);
}

ECode KeyManagerFactory::GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider* provider,
        /* [out] */ IKeyManagerFactory** factory)
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
    assert(0 && "TODO");
    // TODO Object spi = ENGINE.getInstance(algorithm, provider, null);
    // return new KeyManagerFactory((KeyManagerFactorySpi) spi, provider, algorithm);
    return NOERROR;
}

KeyManagerFactory::KeyManagerFactory()
{}

ECode KeyManagerFactory::constructor(
    /* [in] */ IKeyManagerFactorySpi* factorySpi,
    /* [in] */ IProvider* provider,
    /* [in] */ const String& algorithm)
{
    mProvider = provider;
    mAlgorithm = algorithm;
    mSpiImpl = factorySpi;
    return NOERROR;
}

ECode KeyManagerFactory::contructor(
    /* [in] */ IKeyManagerFactorySpi* factorySpi,
    /* [in] */ IProvider* provider,
    /* [in] */ const String& algorithm)
{
    mProvider = provider;
    mAlgorithm = algorithm;
    mSpiImpl = factorySpi;
    return NOERROR;
}

ECode KeyManagerFactory::GetAlgorithm(
    /* [out] */ String* algorithm)
{
    VALIDATE_NOT_NULL(algorithm)

    *algorithm = mAlgorithm;
    return NOERROR;
}

ECode KeyManagerFactory::GetProvider(
    /* [out] */ IProvider** provider)
{
    VALIDATE_NOT_NULL(provider)

    *provider = mProvider;
    REFCOUNT_ADD(*provider)
    return NOERROR;
}

ECode KeyManagerFactory::Init(
    /* [in] */ IKeyStore* ks,
    /* [in] */ ArrayOf<Char32>* password)
{
   return mSpiImpl->EngineInit(ks, password);
}

ECode KeyManagerFactory::Init(
    /* [in] */ IManagerFactoryParameters* spec)
{
    return mSpiImpl->EngineInit(spec);
}

ECode KeyManagerFactory::GetKeyManagers(
    /* [out, callee] */ ArrayOf<IKeyManager*>** managers)
{
    VALIDATE_NOT_NULL(managers)

    return mSpiImpl->EngineGetKeyManagers(managers);
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
