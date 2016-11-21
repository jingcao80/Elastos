
#include "Elastos.CoreLibrary.Security.h"
#include "CSecretKeyFactory.h"
#include "security/CSecurity.h"
#include "org/apache/harmony/security/fortress/CEngine.h"

using Elastos::Security::CSecurity;
using Elastos::Security::ISecurity;
using Org::Apache::Harmony::Security::Fortress::CEngine;
using Org::Apache::Harmony::Security::Fortress::ISpiAndProvider;

namespace Elastosx {
namespace Crypto {

CAR_OBJECT_IMPL(CSecretKeyFactory)
CAR_INTERFACE_IMPL(CSecretKeyFactory, Object, ISecretKeyFactory)

static AutoPtr<IEngine> InitEngine()
{
    AutoPtr<IEngine> e;
    CEngine::New(String("SecretKeyFactory"), (IEngine**)&e);
    return e;
}

AutoPtr<IEngine> CSecretKeyFactory::ENGINE = InitEngine();

CSecretKeyFactory::CSecretKeyFactory()
    : mAlgorithm(String(NULL))
{
}

ECode CSecretKeyFactory::constructor(
    /* [in] */ ISecretKeyFactorySpi* secretKeyFactorySpi,
    /* [in] */ IProvider* provider,
    /* [in] */ const String& algorithm)
{
    mProvider = provider;
    mAlgorithm = algorithm;
    mSpiImpl = secretKeyFactorySpi;
    return NOERROR;
}

ECode CSecretKeyFactory::GetAlgorithm(
    /* [out] */ String * name)
{
    VALIDATE_NOT_NULL(name)
    *name = mAlgorithm;
    return NOERROR;
}

ECode CSecretKeyFactory::GetProvider(
    /* [out] */ IProvider ** provider)
{
    VALIDATE_NOT_NULL(provider)
    *provider = mProvider;
    REFCOUNT_ADD(*provider)
    return NOERROR;
}

ECode CSecretKeyFactory::GenerateSecret(
    /* [in] */ IKeySpec * keySpec,
    /* [out] */ ISecretKey ** key)
{
    return mSpiImpl->EngineGenerateSecret(keySpec, key);
}

ECode CSecretKeyFactory::GetKeySpec(
    /* [in] */ ISecretKey * key,
    /* [in] */ const ClassID& keySpec,
    /* [out] */ IKeySpec ** spec)
{
    return mSpiImpl->EngineGetKeySpec(key, keySpec, spec);
}

ECode CSecretKeyFactory::TranslateKey(
    /* [in] */ ISecretKey * key,
    /* [out] */ ISecretKey ** out)
{
    return mSpiImpl->EngineTranslateKey(key, out);
}

ECode CSecretKeyFactory::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ ISecretKeyFactory ** secretKeyFactory)
{
    VALIDATE_NOT_NULL(secretKeyFactory)
    *secretKeyFactory = NULL;
    if (algorithm == NULL) {
        // throw new NullPointerException("algorithm == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ISpiAndProvider> sap;
    ENGINE->GetInstance(algorithm, NULL, (ISpiAndProvider**)&sap);
    AutoPtr<IInterface> spi;
    sap->GetSpi((IInterface**)&spi);
    AutoPtr<IProvider> provider;
    sap->GetProvider((IProvider**)&provider);

    return CSecretKeyFactory::New(ISecretKeyFactorySpi::Probe(spi), provider, algorithm, secretKeyFactory);
}

ECode CSecretKeyFactory::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ ISecretKeyFactory ** secretKeyFactory)
{
    VALIDATE_NOT_NULL(secretKeyFactory)
    *secretKeyFactory = NULL;
    if (provider == NULL || provider.IsEmpty()) {
        // throw new IllegalArgumentException("Provider is null or empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IProvider> impProvider;
    AutoPtr<ISecurity> security;
    CSecurity::AcquireSingleton((ISecurity**)&security);
    security->GetProvider(provider, (IProvider**)&impProvider);
    if (impProvider == NULL) {
        // throw new NoSuchProviderException(provider);
        return E_NO_SUCH_PROVIDER_EXCEPTION;
    }
    return GetInstance(algorithm, impProvider, secretKeyFactory);
}

ECode CSecretKeyFactory::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider * provider,
    /* [out] */ ISecretKeyFactory ** secretKeyFactory)
{
    VALIDATE_NOT_NULL(secretKeyFactory)
    *secretKeyFactory = NULL;
    if (provider == NULL) {
        // throw new IllegalArgumentException("provider == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (algorithm == NULL) {
        // throw new NullPointerException("algorithm == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<IInterface> spi;
    ENGINE->GetInstance(algorithm, provider, NULL, (IInterface**)&spi);
    return CSecretKeyFactory::New(ISecretKeyFactorySpi::Probe(spi),
            provider, algorithm, secretKeyFactory);
}

} // Crypto
} // Elastosx