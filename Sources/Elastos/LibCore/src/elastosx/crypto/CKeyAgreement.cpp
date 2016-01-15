
#include "Elastos.CoreLibrary.Security.h"
#include "CKeyAgreement.h"
#include "AutoLock.h"

using Elastos::Security::ISecurity;

namespace Elastosx {
namespace Crypto {

CAR_OBJECT_IMPL(CKeyAgreement)
CAR_INTERFACE_IMPL(CKeyAgreement, Object, IKeyAgreement)

String CKeyAgreement::mSERVICE = String("KeyAgreement");
AutoPtr<ISecureRandom> CKeyAgreement::mRANDOM;
//TODO: Need IEngine
//AutoPtr<IEngine> CKeyAgreement::mENGINE;

CKeyAgreement::CKeyAgreement()
    : mAlgorithm(String(NULL))
{
//TODO: Need IEngine
    // CEngine::New(mSERVICE, (IEngine**)&mEngine);
//TODO: Need CSecureRandom
    // CSecureRandom::New((ISecureRandom**)&mRANDOM);
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
    spi->EngineInit(key, mRANDOM);//new SecureRandom());
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
    return spi->EngineInit(key, params, mRANDOM);//new SecureRandom());
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
//TODO: Need CSecurity
    // CSecurity::AcquireSingleton((ISecurity**)&security);
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

//TODO: Need IEngine
    // if (TryAlgorithm(NULL, provider, algorithm) == NULL) {
    //     if (provider == NULL) {
    //         // throw new NoSuchAlgorithmException("No provider found for " + algorithm);
    //         return E_NO_SUCH_ALGORITHM_EXCEPTION;
    //     } else {
    //         // throw new NoSuchAlgorithmException("Provider " + provider.getName()
    //         //         + " does not provide " + algorithm);
    //         return E_NO_SUCH_ALGORITHM_EXCEPTION;
    //     }
    // }
    return CKeyAgreement::New(NULL, provider, algorithm, keyAgreement);
}

//TODO: Need IEngine
// static Engine.SpiAndProvider TryAlgorithm(
//     /* [in] */ IKey* key,
//     /* [in] */ IProvider * provider,
//     /* [in] */ const String& algorithm);

// static Engine.SpiAndProvider TryAlgorithmWithProvider(
//     /* [in] */ IKey* key,
//     /* [in] */ Provider.Service service);

ECode CKeyAgreement::GetSpi(
    /* [in] */ IKey* key,
    /* [out] */ IKeyAgreementSpi ** spi)
{
    VALIDATE_NOT_NULL(spi)
    *spi = NULL;
    synchronized(mInitLock) {
        if (mSpiImpl != NULL && key == NULL) {
            *spi = mSpiImpl;
            REFCOUNT_ADD(*spi)
            return NOERROR;
        }

//TODO: Need IEngine
        // Engine.SpiAndProvider sap = TryAlgorithm(key, specifiedProvider, algorithm);
        // if (sap == NULL) {
        //     // throw new ProviderException("No provider for " + getAlgorithm());
        //     return E_PROVIDER_EXCEPTION;
        // }

        // spiImpl = (KeyAgreementSpi) sap.spi;
        // provider = sap.provider;

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