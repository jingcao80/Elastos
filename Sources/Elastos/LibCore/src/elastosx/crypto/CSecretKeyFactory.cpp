
#include "Elastos.CoreLibrary.Security.h"
#include "CSecretKeyFactory.h"

using Elastos::Security::ISecurity;

namespace Elastosx {
namespace Crypto {

CAR_OBJECT_IMPL(CSecretKeyFactory)
CAR_INTERFACE_IMPL(CSecretKeyFactory, Object, ISecretKeyFactory)

//TODO: Need IEngine
//AutoPtr<IEngine> CSecretKeyFactory::mENGINE;

CSecretKeyFactory::CSecretKeyFactory()
    : mAlgorithm(String(NULL))
{
//TODO: Need IEngine
    // CEngine::New("SecretKeyFactory", (IEngine**)&mEngine);
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
    assert(0 && "TODO");
//TODO: Need IEngine
    // AutoPtr<IEngine.SpiAndProvider> sap = ENGINE.getInstance(algorithm, NULL);
    // return new SecretKeyFactory((SecretKeyFactorySpi) sap.spi, sap.provider, algorithm);
    return NOERROR;
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
//TODO: Need CSecurity
    // CSecurity::AcquireSingleton((ISecurity**)&security);
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
//TODO: Need IEngine
    // Object spi = ENGINE.getInstance(algorithm, provider, NULL);
    // return new SecretKeyFactory((SecretKeyFactorySpi) spi, provider, algorithm);
    return NOERROR;
}

} // Crypto
} // Elastosx