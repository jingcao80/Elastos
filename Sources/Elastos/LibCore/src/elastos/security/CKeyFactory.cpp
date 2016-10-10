
#include "CKeyFactory.h"
#include "CSecurity.h"
#include "org/apache/harmony/security/fortress/CEngine.h"

using Org::Apache::Harmony::Security::Fortress::CEngine;
using Org::Apache::Harmony::Security::Fortress::ISpiAndProvider;

namespace Elastos {
namespace Security {

static AutoPtr<IEngine> InitEngine()
{
    AutoPtr<CEngine> e;
    CEngine::NewByFriend(String("KeyFactory")/*SERVICE*/, (CEngine**)&e);
    return e;
}

const String CKeyFactory::SERVICE("KeyFactory");
AutoPtr<IEngine> CKeyFactory::ENGINE = InitEngine();
CAR_OBJECT_IMPL(CKeyFactory)
CAR_INTERFACE_IMPL(CKeyFactory, Object, IKeyFactory)
ECode CKeyFactory::constructor(
    /* [in] */ IKeyFactorySpi* keyFacSpi,
    /* [in] */ IProvider* provider,
    /* [in] */ const String& algorithm)
{
    mProvider = provider;
    mAlgorithm = algorithm;
    mSpiImpl = keyFacSpi;
    return NOERROR;
}

ECode CKeyFactory::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IKeyFactory** result) /*throws NoSuchAlgorithmException*/
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (algorithm == NULL) {
        // throw new NullPointerException("algorithm == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ISpiAndProvider> sap;
    FAIL_RETURN(ENGINE->GetInstance(algorithm, NULL, (ISpiAndProvider**)&sap));
    AutoPtr<IInterface> spi;
    sap->GetSpi((IInterface**)&spi);
    AutoPtr<IProvider> provider;
    sap->GetProvider((IProvider**)&provider);
    return CKeyFactory::New(IKeyFactorySpi::Probe(spi), provider, algorithm, result);
}

ECode CKeyFactory::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IKeyFactory** result) /*throws NoSuchAlgorithmException, NoSuchProviderException*/
{
    VALIDATE_NOT_NULL(result);
    if (provider == NULL || provider.IsEmpty()) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ISecurity> security;
    CSecurity::AcquireSingleton((ISecurity**)&security);
    AutoPtr<IProvider> p;
    security->GetProvider(provider, (IProvider**)&p);
    if (p == NULL) {
        // throw new NoSuchProviderException(provider);
        return E_NO_SUCH_PROVIDER_EXCEPTION;
    }
    return GetInstance(algorithm, p, result);
}

ECode CKeyFactory::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider* provider,
    /* [out] */ IKeyFactory** result) /*throws NoSuchAlgorithmException*/
{
    VALIDATE_NOT_NULL(result);
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
    return CKeyFactory::New(IKeyFactorySpi::Probe(spi), provider, algorithm, result);
}

ECode CKeyFactory::GetProvider(
    /* [out] */ IProvider** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mProvider;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CKeyFactory::GetAlgorithm(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAlgorithm;
    return NOERROR;
}

ECode CKeyFactory::GeneratePublic(
    /* [in] */ IKeySpec* keySpec,
    /* [out] */ IPublicKey** result) /*throws InvalidKeySpecException*/
{
    VALIDATE_NOT_NULL(result);
    return mSpiImpl->EngineGeneratePublic(keySpec, result);
}

ECode CKeyFactory::GeneratePrivate(
    /* [in] */ IKeySpec* keySpec,
    /* [out] */ IPrivateKey** result) /*throws InvalidKeySpecException*/
{
    VALIDATE_NOT_NULL(result);
    return mSpiImpl->EngineGeneratePrivate(keySpec, result);
}

ECode CKeyFactory::GetKeySpec(
    /* [in] */ IKey* key,
    /* [in] */ const ClassID& keySpec,
    /* [out] */ IKeySpec** result) /*throws InvalidKeySpecException*/
{
    VALIDATE_NOT_NULL(result);
    return mSpiImpl->EngineGetKeySpec(key, keySpec, result);
}

ECode CKeyFactory::TranslateKey(
    /* [in] */ IKey* key,
    /* [out] */ IKey** result) /*throws InvalidKeyException*/
{
    VALIDATE_NOT_NULL(result);
    return mSpiImpl->EngineTranslateKey(key, result);
}

}
}
