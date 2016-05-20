
#include "CAlgorithmParameterGenerator.h"

namespace Elastos {
namespace Security {

const String CAlgorithmParameterGenerator::SERVICE = String("AlgorithmParameterGenerator");
AutoPtr<IEngine> CAlgorithmParameterGenerator::ENGINE;
AutoPtr<ISecureRandom> CAlgorithmParameterGenerator::RANDOM;
Boolean CAlgorithmParameterGenerator::sInit = sInitStatic();

CAR_OBJECT_IMPL(CAlgorithmParameterGenerator)
CAR_INTERFACE_IMPL(CAlgorithmParameterGenerator, Object, IAlgorithmParameterGenerator)

Boolean CAlgorithmParameterGenerator::sInitStatic()
{
    CEngine::New(SERVICE, (IEngine**)&ENGINE);
    CSecureRandom((ISecureRandom**)&RANDOM);
    return TRUE;
}

ECode CAlgorithmParameterGenerator::constructor(
    /* [in] */ AlgorithmParameterGeneratorSpi* paramGenSpi,
    /* [in] */ IProvider* provider,
    /* [in] */ const String& algorithm)
{
    mProvider = provider;
    mAlgorithm = algorithm;
    mSpiImpl = paramGenSpi;
}

ECode CAlgorithmParameterGenerator::GetAlgorithm(
    /* [out] */ String* algorithm)
{
    VALIDATE_NOT_NULL(algorithm);
    *algorithm = mAlgorithm;
    return NOERROR;
}

ECode CAlgorithmParameterGenerator::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IAlgorithmParameterGenerator** generator) /*throws NoSuchAlgorithmException*/
{
    VALIDATE_NOT_NULL(generator);
    *generator = NULL;
    if (algorithm == NULL) {
        // throw new NullPointerException("algorithm == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ISpiAndProvider> sap;
    ENGINE->GetInstance(algorithm, NULL, (ISpiAndProvider**)&sap);
    AutoPtr<IAlgorithmParameterGeneratorSpi> spi;
    sap->GetSpi((IAlgorithmParameterGeneratorSpi**)&spi);

    AutoPtr<IProvider> provider;
    sap->GetProvider((IProvider**)&provider);
    return CAlgorithmParameterGenerator::New(spi, provider, algorithm, generator);
}

ECode CAlgorithmParameterGenerator::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IAlgorithmParameterGenerator** generator) /*throws NoSuchAlgorithmException, NoSuchProviderException*/
{
    VALIDATE_NOT_NULL(generator);
    *generator = NULL;
    if (provider == NULL || provider.IsEmpty()) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IProvider> impProvider;
    Security::GetProvider(provider, (IProvider**)&impProvider);
    if (impProvider == NULL) {
        // throw new NoSuchProviderException(provider);
        return E_NO_SUCH_PROVIDER_EXCEPTION;
    }
    return GetInstance(algorithm, impProvider, generator);
}

ECode CAlgorithmParameterGenerator::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider* provider,
    /* [out] */ IAlgorithmParameterGenerator** generator) /*throws NoSuchAlgorithmException*/
{
    VALIDATE_NOT_NULL(generator);
    *generator = NULL;
    if (provider == NULL) {
        // throw new IllegalArgumentException("provider == null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (algorithm == NULL) {
        // throw new NullPointerException("algorithm == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IInterface> spi;
    ENGINE->GetInstance(algorithm, provider, NULL, (IInterface**)&spi);
    return CAlgorithmParameterGenerator::New(IAlgorithmParameterGeneratorSpi::Probe(spi), provider,
                    algorithm, generator);
}

ECode CAlgorithmParameterGenerator::GetProvider(
    /* [out] */ IProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    *provider = mProvider;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode CAlgorithmParameterGenerator::Init(
    /* [in] */ Int32 size)
{
    return mSpiImpl->EngineInit(size, RANDOM);
}

ECode CAlgorithmParameterGenerator::Init(
    /* [in] */ Int32 size,
    /* [in] */ ISecureRandom* random)
{
    return mSpiImpl->EngineInit(size, random);
}

ECode CAlgorithmParameterGenerator::Init(
    /* [in] */ IAlgorithmParameterSpec* genParamSpec) /*throws InvalidAlgorithmParameterException*/
{
    return mSpiImpl->EngineInit(genParamSpec, RANDOM);
}

ECode CAlgorithmParameterGenerator::Init(
    /* [in] */ IAlgorithmParameterSpec* genParamSpec,
    /* [in] */ ISecureRandom* random) /*throws InvalidAlgorithmParameterException*/
{
    return mSpiImpl->EngineInit(genParamSpec, random);
}

ECode CAlgorithmParameterGenerator::GenerateParameters(
    /* [out] */ IAlgorithmParameters** param)
{
    VALIDATE_NOT_NULL(param);
    return mSpiImpl->EngineGenerateParameters(param);
}

} // namespace Security
} // namespace Elastos
