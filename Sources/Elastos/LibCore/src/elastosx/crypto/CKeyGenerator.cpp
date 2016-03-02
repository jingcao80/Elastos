
#include "Elastos.CoreLibrary.Security.h"
#include "CKeyGenerator.h"
#include "AutoLock.h"

using Elastos::Security::ISecurity;

namespace Elastosx {
namespace Crypto {

CAR_OBJECT_IMPL(CKeyGenerator)
CAR_INTERFACE_IMPL(CKeyGenerator, Object, IKeyGenerator)

AutoPtr<ISecureRandom> CKeyGenerator::mRANDOM;
//TODO: Need IEngine
//AutoPtr<IEngine> CKeyGenerator::mENGINE;

CKeyGenerator::CKeyGenerator()
    : mAlgorithm(String(NULL))
{
//TODO: Need IEngine
    // CEngine::New(mSERVICE, (IEngine**)&mEngine);
//TODO: Need CSecureRandom
    // CSecureRandom::New((ISecureRandom**)&mRANDOM);
}

ECode CKeyGenerator::constructor(
    /* [in] */ IKeyGeneratorSpi* keyGenSpi,
    /* [in] */ IProvider* provider,
    /* [in] */ const String& algorithm)
{
    mProvider = provider;
    mAlgorithm = algorithm;
    mSpiImpl = keyGenSpi;
    return NOERROR;
}

ECode CKeyGenerator::GetAlgorithm(
    /* [out] */ String * name)
{
    VALIDATE_NOT_NULL(name)
    *name = mAlgorithm;
    return NOERROR;
}

ECode CKeyGenerator::GetProvider(
    /* [out] */ IProvider ** provider)
{
    VALIDATE_NOT_NULL(provider)
    *provider = mProvider;
    REFCOUNT_ADD(*provider)
    return NOERROR;
}

ECode CKeyGenerator::GenerateKey(
    /* [out] */ ISecretKey** skey)
{
    VALIDATE_NOT_NULL(skey)
    return mSpiImpl->EngineGenerateKey(skey);
}

ECode CKeyGenerator::Init(
    /* [in] */ IAlgorithmParameterSpec * params)
{
    return mSpiImpl->EngineInit(params, mRANDOM);//new SecureRandom());
}

ECode CKeyGenerator::Init(
    /* [in] */ IAlgorithmParameterSpec * params,
    /* [in] */ ISecureRandom * random)
{
    return mSpiImpl->EngineInit(params, random);
}

ECode CKeyGenerator::Init(
    /* [in] */ Int32 keysize)
{
    return mSpiImpl->EngineInit(keysize, mRANDOM);//new SecureRandom());
}

ECode CKeyGenerator::Init(
    /* [in] */ Int32 keysize,
    /* [in] */ ISecureRandom * random)
{
    return mSpiImpl->EngineInit(keysize, random);
}

ECode CKeyGenerator::Init(
    /* [in] */ ISecureRandom * random)
{
    return mSpiImpl->EngineInit(random);
}

ECode CKeyGenerator::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IKeyGenerator ** ka)
{
    VALIDATE_NOT_NULL(ka)
    *ka = NULL;
    if (algorithm == NULL) {
        // throw new NULLPointerException("algorithm == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
//TODO: Need IEngine
    // Engine.SpiAndProvider sap = ENGINE.getInstance(algorithm, NULL);
    // return new KeyGenerator((KeyGeneratorSpi) sap.spi, sap.provider, algorithm);
    return NOERROR;
}

ECode CKeyGenerator::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IKeyGenerator ** ka)
{
    VALIDATE_NOT_NULL(ka)
    *ka = NULL;
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
    return GetInstance(algorithm, impProvider, ka);
}

ECode CKeyGenerator::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider * provider,
    /* [out] */ IKeyGenerator ** ka)
{
    VALIDATE_NOT_NULL(ka)
    *ka = NULL;
    if (provider == NULL) {
        // throw new IllegalArgumentException("provider == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (algorithm == NULL) {
        // throw new NULLPointerException("algorithm == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
//TODO: Need IEngine
    // Object spi = ENGINE.getInstance(algorithm, provider, NULL);
    // return new KeyGenerator((KeyGeneratorSpi) spi, provider, algorithm);
    return NOERROR;
}

} // Crypto
} // Elastosx