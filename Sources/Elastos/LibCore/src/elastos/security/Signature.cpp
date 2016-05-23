
#include "Signature.h"
#include "CSecurity.h"
#include "core/AutoLock.h"
#include "core/StringBuilder.h"
#include "utility/logging/Logger.h"
#include "org/apache/harmony/security/fortress/CEngine.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::Cert::IX509Extension;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Harmony::Security::Fortress::CEngine;

namespace Elastos {
namespace Security {

//-------------------------------------------------------------
//  SignatureImpl
//-------------------------------------------------------------
/**
 * Internal Signature implementation
 */
class SignatureImpl : public Signature
{
public:
    SignatureImpl(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider* provider)
    {
        Signature::constructor(algorithm);
        mSpecifiedProvider = provider;
    }

    SignatureImpl(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider* provider,
        /* [in] */ SignatureSpi* spi)
    {
        Signature::constructor(algorithm);
        mSpecifiedProvider = provider;
        mSpiImpl = spi;
    }

    // @Override
    CARAPI EnsureProviderChosen()
    {
        AutoPtr<SignatureSpi> spi;
        return GetSpi(NULL, (SignatureSpi**)&spi);
    }

    // @Override
    CARAPI EngineSign(
        /* [out, callee] */ ArrayOf<Byte>** sign)
    {
        AutoPtr<SignatureSpi> spi;
        FAIL_RETURN(GetSpi((SignatureSpi**)&spi));
        return spi->EngineSign(sign);
    }

    // @Override
    CARAPI EngineUpdate(
        /* [in] */ Byte arg0)
    {
        AutoPtr<SignatureSpi> spi;
        FAIL_RETURN(GetSpi((SignatureSpi**)&spi));
        return spi->EngineUpdate(arg0);
    }

    // @Override
    CARAPI EngineVerify(
        /* [in] */ ArrayOf<Byte>* arg0,
        /* [out] */ Boolean* result)
    {
        AutoPtr<SignatureSpi> spi;
        FAIL_RETURN(GetSpi((SignatureSpi**)&spi));
        return spi->EngineVerify(arg0, result);
    }

    // @Override
    CARAPI EngineUpdate(
        /* [in] */ ArrayOf<Byte>* arg0,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2)
    {
        AutoPtr<SignatureSpi> spi;
        FAIL_RETURN(GetSpi((SignatureSpi**)&spi));
        return spi->EngineUpdate(arg0, arg1, arg2);
    }

    // @Override
    CARAPI EngineInitSign(
        /* [in] */ IPrivateKey* arg0)
    {
        AutoPtr<SignatureSpi> spi;
        FAIL_RETURN(GetSpi(IKey::Probe(arg0), (SignatureSpi**)&spi));
        return spi->EngineInitSign(arg0);
    }

    // @Override
    CARAPI EngineInitVerify(
        /* [in] */ IPublicKey* arg0)
    {
        AutoPtr<SignatureSpi> spi;
        FAIL_RETURN(GetSpi(IKey::Probe(arg0), (SignatureSpi**)&spi));
        return spi->EngineInitVerify(arg0);
    }

    // @Override
    CARAPI EngineGetParameter(
        /* [in] */ const String& arg0,
        /* [out] */ IInterface** object)
    {
        AutoPtr<SignatureSpi> spi;
        FAIL_RETURN(GetSpi((SignatureSpi**)&spi));
        return spi->EngineGetParameter(arg0, object);
    }

    // @Override
    CARAPI EngineSetParameter(
        /* [in] */ const String& arg0,
        /* [in] */ IInterface* arg1)
    {
        AutoPtr<SignatureSpi> spi;
        FAIL_RETURN(GetSpi((SignatureSpi**)&spi));
        return spi->EngineSetParameter(arg0, arg1);
    }

    // @Override
    CARAPI EngineSetParameter(
        /* [in] */ IAlgorithmParameterSpec* arg0)
    {
        AutoPtr<SignatureSpi> spi;
        FAIL_RETURN(GetSpi((SignatureSpi**)&spi));
        return spi->EngineSetParameter(arg0);
    }

    // @Override
    CARAPI Clone(
        /* [out] */ IInterface** object)
    {
        AutoPtr<SignatureSpi> spi;
        if (mSpiImpl != NULL) {
            AutoPtr<IInterface> cloneObj;
            mSpiImpl->Clone((IInterface**)&cloneObj);
            spi = (SignatureSpi*)ISignatureSpi::Probe(cloneObj);
        }
        String algo;
        GetAlgorithm(&algo);
        AutoPtr<IProvider> provider;
        GetProvider((IProvider**)&provider);
        *object = (ISignature*)new SignatureImpl(algo, provider, spi);
        REFCOUNT_ADD(*object);
        return NOERROR;
    }

private:
    /**
     * Makes sure a CipherSpi that matches this type is selected.
     */
    CARAPI GetSpi(
        /* [in] */ IKey* key,
        /* [out] */ SignatureSpi** spi)
    {
        {    AutoLock syncLock(mInitLock);
            if (mSpiImpl != NULL && key == NULL) {
                *spi = mSpiImpl;
                REFCOUNT_ADD(*spi);
                return NOERROR;
            }

            AutoPtr<ISpiAndProvider> sap;
            TryAlgorithm(key, mSpecifiedProvider, mAlgorithm, (ISpiAndProvider**)&sap);
            if (sap == NULL) {
                String algo;
                GetAlgorithm(&algo);
                Logger::E("SignatureImpl", "No provider for %s", algo.string());
                return E_PROVIDER_EXCEPTION;
            }

            AutoPtr<IInterface> obj;
            sap->GetSpi((IInterface**)&obj);
            mSpiImpl = (SignatureSpi*)ISignatureSpi::Probe(obj);
            sap->GetProvider((IProvider**)&mProvider);

            *spi = mSpiImpl;
            REFCOUNT_ADD(*spi);
            return NOERROR;
        }
        return NOERROR;
    }

    /**
     * Convenience call when the Key is not available.
     */
    CARAPI GetSpi(
        /* [out] */ SignatureSpi** spi)
    {
        return GetSpi(NULL, spi);
    }

private:
    /**
     * Lock held while the SPI is initializing.
     */
    Object mInitLock;

    // The provider specified when creating this instance.
    AutoPtr<IProvider> mSpecifiedProvider;

    AutoPtr<SignatureSpi> mSpiImpl;
};


//-------------------------------------------------------------
//  Signature
//-------------------------------------------------------------
const String Signature::SERVICE("Signature");
AutoPtr<IEngine> Signature::ENGINE = Init_ENGINE();
const Int32 Signature::UNINITIALIZED = 0;
const Int32 Signature::SIGN = 2;
const Int32 Signature::VERIFY = 3;

AutoPtr<IEngine> Signature::Init_ENGINE()
{
    AutoPtr<CEngine> engine;
    CEngine::NewByFriend(SERVICE, (CEngine**)&engine);
    return engine.Get();
}

ECode Signature::constructor(
    /* [in] */ const String& algorithm)
{
    mAlgorithm = algorithm;
    return NOERROR;
}

CAR_INTERFACE_IMPL(Signature, SignatureSpi, ISignature)

ECode Signature::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ ISignature** instance)
{
    VALIDATE_NOT_NULL(instance);
    if (algorithm.IsNull()) {
        Logger::E("Signature", "algorithm == null");
        *instance = NULL;
        return E_NULL_POINTER_EXCEPTION;
    }
    return GetSignature(algorithm, NULL, instance);
}

ECode Signature::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ ISignature** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = NULL;
    if (algorithm.IsNull()) {
        Logger::E("Signature", "algorithm == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (provider.IsNullOrEmpty()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ISecurity> security;
    CSecurity::AcquireSingleton((ISecurity**)&security);
    AutoPtr<IProvider> p;
    security->GetProvider(provider, (IProvider**)&p);
    if (p == NULL) {
        return E_NO_SUCH_PROVIDER_EXCEPTION;
    }
    return GetSignature(algorithm, p, instance);
}

ECode Signature::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider* provider,
    /* [out] */ ISignature** instance)
{
    VALIDATE_NOT_NULL(instance);
    *instance = NULL;
    if (algorithm.IsNull()) {
        Logger::E("Signature", "algorithm == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (provider == NULL) {
        Logger::E("Signature", "provider == null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return GetSignature(algorithm, provider, instance);
}

ECode Signature::GetSignature(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider* provider,
    /* [out] */ ISignature** signature)
{
    if (algorithm.IsNullOrEmpty()) {
        Logger::E("Signature", "Unknown algorithm: %s", algorithm.string());
        return E_NO_SUCH_ALGORITHM_EXCEPTION;
    }

    AutoPtr<ISpiAndProvider> spiAndProvider;
    FAIL_RETURN(TryAlgorithm(NULL, provider, algorithm, (ISpiAndProvider**)&spiAndProvider))
    if (spiAndProvider == NULL) {
        if (provider == NULL) {
            Logger::E("Signature", "No provider found for %s", algorithm.string());
            return E_NO_SUCH_ALGORITHM_EXCEPTION;
        }
        else {
            String name;
            provider->GetName(&name);
            Logger::E("Signature", "Provider %s does not provide %s", name.string(), algorithm.string());
            return E_NO_SUCH_ALGORITHM_EXCEPTION;
        }
    }
    AutoPtr<IInterface> spi;
    spiAndProvider->GetSpi((IInterface**)&spi);
    if (ISignature::Probe(spi)) {
        *signature = ISignature::Probe(spi);
        REFCOUNT_ADD(*signature);
        return NOERROR;
    }
    *signature = new SignatureImpl(algorithm, provider);
    REFCOUNT_ADD(*signature);
    return NOERROR;
}

ECode Signature::TryAlgorithm(
    /* [in] */ IKey* key,
    /* [in] */ IProvider* provider,
    /* [in] */ const String& algorithm,
    /* [out] */ ISpiAndProvider** spi)
{
    VALIDATE_NOT_NULL(spi);
    if (provider != NULL) {
        AutoPtr<IProviderService> service;
        provider->GetService(SERVICE, algorithm, (IProviderService**)&service);
        if (service == NULL) {
            *spi = NULL;
            return NOERROR;
        }
        return TryAlgorithmWithProvider(key, service, spi);
    }
    AutoPtr<IArrayList> services;
    ENGINE->GetServices(algorithm, (IArrayList**)&services);
    if (services == NULL) {
        *spi = NULL;
        return NOERROR;
    }
    AutoPtr<IIterator> it;
    services->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> service;
        it->GetNext((IInterface**)&service);
        AutoPtr<ISpiAndProvider> sap;
        TryAlgorithmWithProvider(key, IProviderService::Probe(service), (ISpiAndProvider**)&sap);
        if (sap != NULL) {
            *spi = sap;
            REFCOUNT_ADD(*spi);
            return NOERROR;
        }
    }
    *spi = NULL;
    return NOERROR;
}

ECode Signature::TryAlgorithmWithProvider(
    /* [in] */ IKey* key,
    /* [in] */ IProviderService* service,
    /* [out] */ ISpiAndProvider** spi)
{
    Boolean result;
    if (key != NULL && (service->SupportsParameter(key, &result), !result)) {
        *spi = NULL;
        return NOERROR;
    }

    AutoPtr<ISpiAndProvider> sap;
    ECode ec = ENGINE->GetInstance(service, String(NULL), (ISpiAndProvider**)&sap);
    if (FAILED(ec)) {
        *spi = NULL;
        return NOERROR;
    }
    AutoPtr<IInterface> spiObj;
    AutoPtr<IProvider> provider;
    if ((sap->GetSpi((IInterface**)&spiObj), spiObj == NULL) ||
        (sap->GetProvider((IProvider**)&provider), provider == NULL)) {
        *spi = NULL;
        return NOERROR;
    }
    if (ISignatureSpi::Probe(spiObj) == NULL) {
        *spi = NULL;
        return NOERROR;
    }
    *spi = sap;
    REFCOUNT_ADD(*spi);
    return NOERROR;
}

ECode Signature::GetProvider(
    /* [out] */ IProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    FAIL_RETURN(EnsureProviderChosen());
    *provider = mProvider;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode Signature::EnsureProviderChosen()
{
    return NOERROR;
}

ECode Signature::GetAlgorithm(
    /* [out] */ String* algo)
{
    VALIDATE_NOT_NULL(algo);
    *algo = mAlgorithm;
    return NOERROR;
}

ECode Signature::InitVerify(
    /* [in] */ IPublicKey* publicKey)
{
    FAIL_RETURN(EngineInitVerify(publicKey));
    mState = VERIFY;
    return NOERROR;
}

ECode Signature::InitVerify(
    /* [in] */ Elastos::Security::Cert::ICertificate* certificate)
{
    if (IX509Certificate::Probe(certificate)) {
        AutoPtr<ISet> ce;
        IX509Extension::Probe(certificate)->GetCriticalExtensionOIDs((ISet**)&ce);
        Boolean critical = FALSE;
        Boolean isEmpty = FALSE;
        if (ce != NULL && (ce->IsEmpty(&isEmpty), !isEmpty)) {
            AutoPtr<IIterator> i;
            ce->GetIterator((IIterator**)&i);
            Boolean hasNext;
            while (i->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> strObj;
                i->GetNext((IInterface**)&strObj);
                String str;
                ICharSequence::Probe(strObj)->ToString(&str);
                if (String("2.5.29.15").Equals(str)) {
                    //KeyUsage OID = 2.5.29.15
                    critical = TRUE;
                    break;
                }
            }
            if (critical) {
                AutoPtr< ArrayOf<Boolean> > keyUsage;
                IX509Certificate::Probe(certificate)->GetKeyUsage((ArrayOf<Boolean>**)&keyUsage);
                // As specified in RFC 3280 -
                // Internet X.509 Public Key Infrastructure
                // Certificate and Certificate Revocation List (CRL) Profile.
                // (http://www.ietf.org/rfc/rfc3280.txt)
                //
                // KeyUsage ::= BIT STRING { digitalSignature (0), <skipped> }
                if ((keyUsage != NULL) && (!(*keyUsage)[0])) { // digitalSignature
                    Logger::E("Signature", "The public key in the certificate cannot be used for digital signature purposes");
                    return E_INVALID_KEY_EXCEPTION;
                }
            }
        }
    }
    AutoPtr<IPublicKey> key;
    certificate->GetPublicKey((IPublicKey**)&key);
    FAIL_RETURN(EngineInitVerify(key));
    mState = VERIFY;
    return NOERROR;
}

ECode Signature::InitSign(
    /* [in] */ IPrivateKey* privateKey)
{
    FAIL_RETURN(EngineInitSign(privateKey));
    mState = SIGN;
    return NOERROR;
}

ECode Signature::InitSign(
    /* [in] */ IPrivateKey* privateKey,
    /* [in] */ ISecureRandom* random)
{
    FAIL_RETURN(EngineInitSign(privateKey, random));
    mState = SIGN;
    return NOERROR;
}

ECode Signature::Sign(
    /* [out, callee] */ ArrayOf<Byte>** sign)
{
    VALIDATE_NOT_NULL(sign);
    if (mState != SIGN) {
        Logger::E("Signature", "Signature object is not initialized properly");
        return E_SIGNATURE_EXCEPTION;
    }
    return EngineSign(sign);
}

ECode Signature::Sign(
    /* [in, out] */ ArrayOf<Byte>* outbuf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 len,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    if (outbuf == NULL || offset < 0 || len < 0 ||
            offset + len > outbuf->GetLength()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mState != SIGN) {
        Logger::E("Signature", "Signature object is not initialized properly");
        return E_SIGNATURE_EXCEPTION;
    }
    return EngineSign(outbuf, offset, len, number);
}

ECode Signature::Verify(
    /* [in] */ ArrayOf<Byte>* signature,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mState != VERIFY) {
        Logger::E("Signature", "Signature object is not initialized properly");
        return E_SIGNATURE_EXCEPTION;
    }
    return EngineVerify(signature, result);
}

ECode Signature::Verify(
    /* [in] */ ArrayOf<Byte>* signature,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mState != VERIFY) {
        Logger::E("Signature", "Signature object is not initialized properly");
        return E_SIGNATURE_EXCEPTION;
    }
    if (signature == NULL || offset < 0 || length < 0 ||
            offset + length > signature->GetLength()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return EngineVerify(signature, offset, length, result);
}

ECode Signature::Update(
    /* [in] */ Byte b)
{
    if (mState == UNINITIALIZED) {
        Logger::E("Signature", "Signature object is not initialized properly");
        return E_SIGNATURE_EXCEPTION;
    }
    return EngineUpdate(b);
}

ECode Signature::Update(
    /* [in] */ ArrayOf<Byte>* data)
{
    if (mState == UNINITIALIZED) {
        Logger::E("Signature", "Signature object is not initialized properly");
        return E_SIGNATURE_EXCEPTION;
    }
    return EngineUpdate(data, 0, data->GetLength());
}

ECode Signature::Update(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    if (mState == UNINITIALIZED) {
        Logger::E("Signature", "Signature object is not initialized properly");
        return E_SIGNATURE_EXCEPTION;
    }
    if (data == NULL || off < 0 || len < 0 ||
            off + len > data->GetLength()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return EngineUpdate(data, off, len);
}

ECode Signature::Update(
    /* [in] */ IByteBuffer* data)
{
    if (mState == UNINITIALIZED) {
        Logger::E("Signature", "Signature object is not initialized properly");
        return E_SIGNATURE_EXCEPTION;
    }
    return EngineUpdate(data);
}

ECode Signature::ToString(
    /* [out] */ String* str)
{
    StringBuilder sb;
    sb += "SIGNATURE ";
    sb += mAlgorithm;
    sb += " state: ";
    sb += StateToString(mState);
    *str = sb.ToString();
    return NOERROR;
}

String Signature::StateToString(
    /* [in] */ Int32 state)
{
    switch (mState) {
    case UNINITIALIZED:
        return String("UNINITIALIZED");
    case SIGN:
        return String("SIGN");
    case VERIFY:
        return String("VERIFY");
    default:
        return String("");
    }
}

ECode Signature::SetParameter(
    /* [in] */ const String& param,
    /* [in] */ IInterface* value)
{
    return EngineSetParameter(param, value);
}

ECode Signature::SetParameter(
    /* [in] */ IAlgorithmParameterSpec* params)
{
    return EngineSetParameter(params);
}

ECode Signature::GetParameters(
    /* [out] */ IAlgorithmParameters** params)
{
    VALIDATE_NOT_NULL(params);
    return EngineGetParameters(params);
}

ECode Signature::GetParameter(
    /* [in] */ const String& param,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);
    return EngineGetParameter(param, value);
}

} // namespace Security
} // namespace Elastos
