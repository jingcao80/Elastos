
#include "Elastos.CoreLibrary.Security.h"
#include "CExemptionMechanism.h"
#include "Arrays.h"

using Elastos::Security::ISecurity;
using Elastos::Utility::Arrays;

namespace Elastosx {
namespace Crypto {

CAR_OBJECT_IMPL(CExemptionMechanism)
CAR_INTERFACE_IMPL(CExemptionMechanism, Object, IExemptionMechanism)

//TODO: Need IEngine
//AutoPtr<IEngine> CExemptionMechanism::mENGINE;

CExemptionMechanism::CExemptionMechanism()
    : mIsInit(FALSE)
    , mGenerated(FALSE)
{
//TODO: Need IEngine
    // CEngine::New("ExemptionMechanism", (IEngine**)&mEngine);
}

ECode CExemptionMechanism::constructor(
    /* [in] */ IExemptionMechanismSpi* exmechSpi,
    /* [in] */ IProvider* provider,
    /* [in] */ const String& mechanism)
{
    mMechanism = mechanism;
    mSpiImpl = exmechSpi;
    mProvider = provider;
    mIsInit = FALSE;
    return NOERROR;
}

ECode CExemptionMechanism::GetName(
    /* [out] */ String * name)
{
    VALIDATE_NOT_NULL(name)
    *name = mMechanism;
    return NOERROR;
}

ECode CExemptionMechanism::GetProvider(
    /* [out] */ IProvider ** provider)
{
    VALIDATE_NOT_NULL(provider)
    *provider = mProvider;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode CExemptionMechanism::IsCryptoAllowed(
    /* [in] */ IKey * key,
    /* [out] */ Boolean * value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;
    AutoPtr<ArrayOf<Byte> > encoded1;
    mInitKey->GetEncoded((ArrayOf<Byte>**)&encoded1);
    AutoPtr<ArrayOf<Byte> > encoded2;
    key->GetEncoded((ArrayOf<Byte>**)&encoded2);

    if (mGenerated && (Object::Equals(mInitKey, key) || Arrays::Equals(encoded1,encoded2))) {
        *value = TRUE;
        return NOERROR;
    }
    *value = FALSE;
    return NOERROR;
}

ECode CExemptionMechanism::GetOutputSize(
    /* [in] */ Int32 inputLen,
    /* [out] */ Int32 * size)
{
    VALIDATE_NOT_NULL(size)
    *size = 0;
    if (!mIsInit) {
        // throw new IllegalStateException("ExemptionMechanism is not initialized");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return mSpiImpl->EngineGetOutputSize(inputLen, size);
}

ECode CExemptionMechanism::Init(
    /* [in] */ IKey * key)
{
    mGenerated = FALSE;
    mSpiImpl->EngineInit(key);
    mInitKey = key;
    mIsInit = TRUE;
    return NOERROR;
}

ECode CExemptionMechanism::Init(
    /* [in] */ IKey * key,
    /* [in] */ IAlgorithmParameters * params)
{
    mGenerated = FALSE;
    mSpiImpl->EngineInit(key, params);
    mInitKey = key;
    mIsInit = TRUE;
    return NOERROR;
}

ECode CExemptionMechanism::Init(
    /* [in] */ IKey * key,
    /* [in] */ IAlgorithmParameterSpec * params)
{
    mGenerated = FALSE;
    mSpiImpl->EngineInit(key, params);
    mInitKey = key;
    mIsInit = TRUE;
    return NOERROR;
}

ECode CExemptionMechanism::GenExemptionBlob(
    /* [out, callee] */ ArrayOf<Byte> ** blob)
{
    if (!mIsInit) {
        // throw new IllegalStateException("ExemptionMechanism is not initialized");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mGenerated = FALSE;
    mSpiImpl->EngineGenExemptionBlob(blob);
    mGenerated = TRUE;
    return NOERROR;
}

ECode CExemptionMechanism::GenExemptionBlob(
    /* [in] */ ArrayOf<Byte> * output,
    /* [out] */ Int32 * number)
{
    return GenExemptionBlob(output, 0, number);
}

ECode CExemptionMechanism::GenExemptionBlob(
    /* [in] */ ArrayOf<Byte> * output,
    /* [in] */ Int32 outputOffset,
    /* [out] */ Int32 * number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;
    if (!mIsInit) {
        // throw new IllegalStateException("ExemptionMechanism is not initialized");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mGenerated = FALSE;
    Int32 len;
    mSpiImpl->EngineGenExemptionBlob(output, outputOffset, &len);
    mGenerated = TRUE;
    *number = len;
    return NOERROR;
}

ECode CExemptionMechanism::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IExemptionMechanism ** exemptionMechanism)
{
    VALIDATE_NOT_NULL(exemptionMechanism)
    *exemptionMechanism = NULL;
    if (algorithm == NULL) {
        // throw new NULLPointerException("algorithm == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    assert(0 && "TODO");
//TODO: Need IEngine
    // Engine.SpiAndProvider sap = ENGINE.getInstance(algorithm, NULL);
    // return new ExemptionMechanism((ExemptionMechanismSpi) sap.spi, sap.provider, algorithm);
    return NOERROR;
}

ECode CExemptionMechanism::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IExemptionMechanism ** exemptionMechanism)
{
    VALIDATE_NOT_NULL(exemptionMechanism)
    *exemptionMechanism = NULL;
    if (provider == NULL) {
        // throw new IllegalArgumentException("provider == NULL");
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
    if (algorithm == NULL) {
        // throw new NULLPointerException("algorithm == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    return GetInstance(algorithm, impProvider, exemptionMechanism);
}

ECode CExemptionMechanism::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider * provider,
    /* [out] */ IExemptionMechanism ** exemptionMechanism)
{
    VALIDATE_NOT_NULL(exemptionMechanism)
    *exemptionMechanism = NULL;
    if (algorithm == NULL) {
        // throw new NULLPointerException("algorithm == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (provider == NULL) {
        // throw new IllegalArgumentException("provider == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
//TODO: Need IEngine
    // Object spi = ENGINE.getInstance(algorithm, provider, NULL);
    // return new ExemptionMechanism((ExemptionMechanismSpi) spi, provider, algorithm);
    return NOERROR;
}

} // Crypto
} // Elastosx