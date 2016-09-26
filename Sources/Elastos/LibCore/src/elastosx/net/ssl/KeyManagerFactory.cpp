
#include "Elastos.CoreLibrary.Security.h"
#include "KeyManagerFactory.h"
#include "CKeyManagerFactory.h"
#include "CSecurity.h"
#include "org/apache/harmony/security/fortress/CEngine.h"

using Elastos::Security::CSecurity;
using Elastos::Security::ISecurity;
using Org::Apache::Harmony::Security::Fortress::CEngine;
using Org::Apache::Harmony::Security::Fortress::ISpiAndProvider;

namespace Elastosx {
namespace Net {
namespace Ssl {

static AutoPtr<IEngine> InitEngine()
{
    AutoPtr<CEngine> e;
    CEngine::NewByFriend(String("KeyManagerFactory")/*SERVICE*/, (CEngine**)&e);
    return e;
}

const String KeyManagerFactory::SERVICE = String("KeyManagerFactory");
AutoPtr<IEngine> KeyManagerFactory::ENGINE = InitEngine();
const String KeyManagerFactory::PROPERTY_NAME = String("ssl.KeyManagerFactory.algorithm");
const String KeyManagerFactory::DEFAULT_PROPERTY = String("PKIX");

CAR_INTERFACE_IMPL(KeyManagerFactory, Object, IKeyManagerFactory)

ECode KeyManagerFactory::GetDefaultAlgorithm(
        /* [out] */ String* algorithm)
{
    VALIDATE_NOT_NULL(algorithm)

    String _algorithm;
    AutoPtr<ISecurity> security;
    CSecurity::AcquireSingleton((ISecurity**)&security);
    security->GetProperty(PROPERTY_NAME, &_algorithm);
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
    AutoPtr<ISpiAndProvider> sap;
    ENGINE->GetInstance(algorithm, NULL, (ISpiAndProvider**)&sap);
    AutoPtr<IInterface> spi;
    sap->GetSpi((IInterface**)&spi);
    AutoPtr<IProvider> provider;
    sap->GetProvider((IProvider**)&provider);
    return CKeyManagerFactory::New(IKeyManagerFactorySpi::Probe(spi), provider, algorithm, factory);
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
    AutoPtr<ISecurity> security;
    CSecurity::AcquireSingleton((ISecurity**)&security);
    security->GetProvider(provider, (IProvider**)&impProvider);
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
    AutoPtr<IInterface> spi;
    ENGINE->GetInstance(algorithm, provider, NULL, (IInterface**)&spi);
    return CKeyManagerFactory::New(IKeyManagerFactorySpi::Probe(spi), provider, algorithm, factory);
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
