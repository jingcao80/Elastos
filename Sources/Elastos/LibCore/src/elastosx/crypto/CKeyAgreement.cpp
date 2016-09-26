
#include "Elastos.CoreLibrary.Security.h"
#include "CKeyAgreement.h"
#include "AutoLock.h"
#include "CSecureRandom.h"
#include "CSecurity.h"
#include "org/apache/harmony/security/fortress/CEngine.h"

using Elastos::Core::AutoLock;
using Elastos::Security::CSecureRandom;
using Elastos::Security::CSecurity;
using Elastos::Security::ISecurity;
using Elastos::Utility::IIterator;
using Org::Apache::Harmony::Security::Fortress::CEngine;

namespace Elastosx {
namespace Crypto {

static AutoPtr<IEngine> InitEngine()
{
    AutoPtr<CEngine> e;
    CEngine::NewByFriend(String("KeyAgreement")/*SERVICE*/, (CEngine**)&e);
    return e;
}

static AutoPtr<ISecureRandom> InitSecureRandom()
{
    AutoPtr<CSecureRandom> r;
    CSecureRandom::NewByFriend((CSecureRandom**)&r);
    return r;
}

CAR_OBJECT_IMPL(CKeyAgreement)
CAR_INTERFACE_IMPL(CKeyAgreement, Object, IKeyAgreement)

String CKeyAgreement::SERVICE("KeyAgreement");
AutoPtr<ISecureRandom> CKeyAgreement::RANDOM = InitSecureRandom();
AutoPtr<IEngine> CKeyAgreement::ENGINE = InitEngine();

CKeyAgreement::CKeyAgreement()
    : mAlgorithm(String(NULL))
{
}

ECode CKeyAgreement::constructor(
    /* [in] */ IKeyAgreementSpi* keyAgreeSpi,
    /* [in] */ IProvider* provider,
    /* [in] */ const String& algorithm)
{
    mSpiImpl = keyAgreeSpi;
    mSpecifiedProvider = provider;
    mAlgorithm = algorithm;
    return NOERROR;
}

ECode CKeyAgreement::GetAlgorithm(
    /* [out] */ String * name)
{
    VALIDATE_NOT_NULL(name)
    *name = mAlgorithm;
    return NOERROR;
}

ECode CKeyAgreement::GetProvider(
    /* [out] */ IProvider ** provider)
{
    VALIDATE_NOT_NULL(provider)
    AutoPtr<IKeyAgreementSpi> spi;
    GetSpi((IKeyAgreementSpi**)&spi);
    *provider = mProvider;
    REFCOUNT_ADD(*provider)
    return NOERROR;
}

ECode CKeyAgreement::Init(
    /* [in] */ IKey * key)
{
    AutoPtr<IKeyAgreementSpi> spi;
    GetSpi((IKeyAgreementSpi**)&spi);
    spi->EngineInit(key, RANDOM);//new SecureRandom());
    return NOERROR;
}

ECode CKeyAgreement::Init(
    /* [in] */ IKey * key,
    /* [in] */ ISecureRandom * random)
{
    AutoPtr<IKeyAgreementSpi> spi;
    GetSpi((IKeyAgreementSpi**)&spi);
    return spi->EngineInit(key, random);
}

ECode CKeyAgreement::Init(
    /* [in] */ IKey * key,
    /* [in] */ IAlgorithmParameterSpec * params)
{
    AutoPtr<IKeyAgreementSpi> spi;
    GetSpi((IKeyAgreementSpi**)&spi);
    return spi->EngineInit(key, params, RANDOM);//new SecureRandom());
}

ECode CKeyAgreement::Init(
    /* [in] */ IKey * key,
    /* [in] */ IAlgorithmParameterSpec * params,
    /* [in] */ ISecureRandom * random)
{
    AutoPtr<IKeyAgreementSpi> spi;
    GetSpi((IKeyAgreementSpi**)&spi);
    return spi->EngineInit(key, params, random);
}

ECode CKeyAgreement::DoPhase(
    /* [in] */ IKey * key,
    /* [in] */ Boolean lastPhase,
    /* [out] */ IKey ** result)
{
    AutoPtr<IKeyAgreementSpi> spi;
    GetSpi((IKeyAgreementSpi**)&spi);
    return spi->EngineDoPhase(key, lastPhase, result);
}

ECode CKeyAgreement::GenerateSecret(
    /* [out, callee] */ ArrayOf<Byte> ** secret)
{
    AutoPtr<IKeyAgreementSpi> spi;
    GetSpi((IKeyAgreementSpi**)&spi);
    return spi->EngineGenerateSecret(secret);
}

ECode CKeyAgreement::GenerateSecret(
    /* [in] */ ArrayOf<Byte> * sharedSecret,
    /* [in] */ Int32 offset,
    /* [out] */ Int32 * number)
{
    AutoPtr<IKeyAgreementSpi> spi;
    GetSpi((IKeyAgreementSpi**)&spi);
    return spi->EngineGenerateSecret(sharedSecret, offset, number);
}

ECode CKeyAgreement::GenerateSecret(
    /* [in] */ const String& algorithm,
    /* [out] */ ISecretKey ** key)
{
    AutoPtr<IKeyAgreementSpi> spi;
    GetSpi((IKeyAgreementSpi**)&spi);
    return spi->EngineGenerateSecret(algorithm, key);
}

ECode CKeyAgreement::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IKeyAgreement ** keyAgreement)
{
    return GetKeyAgreement(algorithm, NULL, keyAgreement);
}

ECode CKeyAgreement::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IKeyAgreement ** keyAgreement)
{
    VALIDATE_NOT_NULL(keyAgreement)
    *keyAgreement = NULL;
    if (provider == NULL || provider.IsEmpty()) {
        // throw new IllegalArgumentException("Provider is NULL or empty");
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
    return GetKeyAgreement(algorithm, impProvider, keyAgreement);
}

ECode CKeyAgreement::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider * provider,
    /* [out] */ IKeyAgreement ** keyAgreement)
{
    VALIDATE_NOT_NULL(keyAgreement)
    *keyAgreement = NULL;
    if (provider == NULL) {
        // throw new IllegalArgumentException("provider == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return GetKeyAgreement(algorithm, provider, keyAgreement);
}

ECode CKeyAgreement::GetKeyAgreement(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider * provider,
    /* [out] */ IKeyAgreement ** keyAgreement)
{
    VALIDATE_NOT_NULL(keyAgreement)
    *keyAgreement = NULL;
    if (algorithm == NULL) {
        // throw new NULLPointerException("algorithm == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }

    if (TryAlgorithm(NULL, provider, algorithm) == NULL) {
        if (provider == NULL) {
            // throw new NoSuchAlgorithmException("No provider found for " + algorithm);
            return E_NO_SUCH_ALGORITHM_EXCEPTION;
        }
        else {
            // throw new NoSuchAlgorithmException("Provider " + provider.getName()
            //         + " does not provide " + algorithm);
            return E_NO_SUCH_ALGORITHM_EXCEPTION;
        }
    }
    return CKeyAgreement::New(NULL, provider, algorithm, keyAgreement);
}

AutoPtr<ISpiAndProvider> CKeyAgreement::TryAlgorithm(
    /* [in] */ IKey* key,
    /* [in] */ IProvider * provider,
    /* [in] */ const String& algorithm)
{
    if (provider != NULL) {
        AutoPtr<IProviderService> service;
        provider->GetService(SERVICE, algorithm, (IProviderService**)&service);
        if (service == NULL) {
            return NULL;
        }
        return TryAlgorithmWithProvider(key, service);
    }
    AutoPtr<IArrayList/*<Provider.Service*/> services;
    ENGINE->GetServices(algorithm, (IArrayList**)&services);
    if (services == NULL) {
        return NULL;
    }

    AutoPtr<IIterator> it;
    services->GetIterator((IIterator**)&it);
    Boolean has = FALSE;
    while(it->HasNext(&has), has) {
        AutoPtr<IInterface> service;
        it->GetNext((IInterface**)&service);
        AutoPtr<ISpiAndProvider> sap = TryAlgorithmWithProvider(key, IProviderService::Probe(service));
        if (sap != NULL) {
            return sap;
        }
    }
    return NULL;
}

AutoPtr<ISpiAndProvider> CKeyAgreement::TryAlgorithmWithProvider(
    /* [in] */ IKey* key,
    /* [in] */ IProviderService* service)
{
    // try {
    if (key != NULL) {
        Boolean tmp = FALSE;
        if (FAILED(service->SupportsParameter(key, &tmp))) {
            return NULL;
        }
        if (!tmp) {
            return NULL;
        }
    }

    AutoPtr<ISpiAndProvider> sap;
    if (FAILED(ENGINE->GetInstance(service, String(NULL), (ISpiAndProvider**)&sap))) {
        return NULL;
    }

    AutoPtr<IInterface> spi;
    sap->GetSpi((IInterface**)&spi);
    AutoPtr<IProvider> provider;
    if (spi.Get() == NULL || (sap->GetProvider((IProvider**)&provider), provider.Get()) == NULL) {
        return NULL;
    }
    if (IKeyAgreementSpi::Probe(spi) == NULL) {
        return NULL;
    }
    return sap;
    // } catch (NoSuchAlgorithmException ignored) {
    // }
    // return NULL;
}

ECode CKeyAgreement::GetSpi(
    /* [in] */ IKey* key,
    /* [out] */ IKeyAgreementSpi ** spi)
{
    VALIDATE_NOT_NULL(spi)
    *spi = NULL;
    {    AutoLock syncLock(mInitLock);
        if (mSpiImpl != NULL && key == NULL) {
            *spi = mSpiImpl;
            REFCOUNT_ADD(*spi)
            return NOERROR;
        }

        AutoPtr<ISpiAndProvider> sap = TryAlgorithm(key, mSpecifiedProvider, mAlgorithm);
        if (sap == NULL) {
            // throw new ProviderException("No provider for " + getAlgorithm());
            return E_PROVIDER_EXCEPTION;
        }

        AutoPtr<IInterface> obj;
        sap->GetSpi((IInterface**)&obj);
        mSpiImpl = IKeyAgreementSpi::Probe(obj);
        sap->GetProvider((IProvider**)&mProvider);

        *spi = mSpiImpl;
        REFCOUNT_ADD(*spi)
        return NOERROR;
    }
    return NOERROR;
}

ECode CKeyAgreement::GetSpi(
    /* [out] */ IKeyAgreementSpi ** spi)
{
    return GetSpi(NULL, spi);
}

} // Crypto
} // Elastosx