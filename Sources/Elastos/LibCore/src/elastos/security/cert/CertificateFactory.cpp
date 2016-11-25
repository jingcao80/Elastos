
#include "Elastos.CoreLibrary.Utility.h"
#include "CSecurity.h"
#include "cert/CertificateFactory.h"
#include "utility/logging/Logger.h"
#include "org/apache/harmony/security/fortress/CEngine.h"

using Elastos::Core::ICharSequence;
using Elastos::Security::ISecurity;
using Elastos::Security::CSecurity;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Harmony::Security::Fortress::CEngine;
using Org::Apache::Harmony::Security::Fortress::ISpiAndProvider;

namespace Elastos {
namespace Security {
namespace Cert {

const String CertificateFactory::SERVICE("CertificateFactory");
const AutoPtr<IEngine> CertificateFactory::ENGINE = InitENGINE();

AutoPtr<IEngine> CertificateFactory::InitENGINE()
{
    AutoPtr<IEngine> engine;
    CEngine::New(SERVICE, (IEngine**)&engine);
    return engine;
}

CertificateFactory::CertificateFactory(
    /* [in] */ ICertificateFactorySpi* certFacSpi,
    /* [in] */ IProvider* provider,
    /* [in] */ const String& type)
    : mProvider(provider)
    , mSpiImpl(certFacSpi)
    , mType(type)
{}

CAR_INTERFACE_IMPL(CertificateFactory, Object, ICertificateFactory)

ECode CertificateFactory::Aggregate(
    /* [in] */ AggregateType type,
    /* [in] */ IInterface* object)
{
    return E_NOT_IMPLEMENTED;
}

ECode CertificateFactory::GetDomain(
    /* [out] */ IInterface** object)
{
    return E_NOT_IMPLEMENTED;
}

ECode CertificateFactory::GetClassID(
    /* [out] */ ClassID* clsid)
{
    return E_NOT_IMPLEMENTED;
}

ECode CertificateFactory::GetInstance(
    /* [in] */ const String& type,
    /* [out] */ ICertificateFactory** certFact)
{
    if (type.IsNull()) {
        Logger::E(SERVICE, "type == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ISpiAndProvider> sap;
    ECode ec = ENGINE->GetInstance(type, NULL, (ISpiAndProvider**)&sap);
    if (FAILED(ec)) return E_CERTIFICATE_EXCEPTION;
    AutoPtr<IInterface> spi;
    sap->GetSpi((IInterface**)&spi);
    AutoPtr<IProvider> provider;
    sap->GetProvider((IProvider**)&provider);
    AutoPtr<ICertificateFactory> tmp = new CertificateFactory(ICertificateFactorySpi::Probe(spi), provider, type);
    *certFact = tmp;
    REFCOUNT_ADD(*certFact);
    return NOERROR;
}

ECode CertificateFactory::GetInstance(
    /* [in] */ const String& type,
    /* [in] */ const String& provider,
    /* [out] */ ICertificateFactory** certFact)
{
    VALIDATE_NOT_NULL(certFact)
    if (provider.IsNullOrEmpty()) {
        Logger::E(SERVICE, "provider == null || provider.isEmpty()");
        *certFact = NULL;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ISecurity> security;
    CSecurity::AcquireSingleton((ISecurity**)&security);
    AutoPtr<IProvider> impProvider;
    security->GetProvider(provider, (IProvider**)&impProvider);
    if (impProvider == NULL) {
        return E_NO_SUCH_PROVIDER_EXCEPTION;
    }
    return GetInstance(type, impProvider, certFact);
}

ECode CertificateFactory::GetInstance(
    /* [in] */ const String& type,
    /* [in] */ IProvider* provider,
    /* [out] */ ICertificateFactory** certFact)
{
    if (provider == NULL) {
        Logger::E(SERVICE, "provider == null");
        *certFact = NULL;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (type.IsNull()) {
        Logger::E(SERVICE, "type == null");
        *certFact = NULL;
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IInterface> spi;
    ECode ec = ENGINE->GetInstance(type, provider, NULL, (IInterface**)&spi);
    if (FAILED(ec)) return E_CERTIFICATE_EXCEPTION;
    AutoPtr<ICertificateFactory> tmp = new CertificateFactory(ICertificateFactorySpi::Probe(spi), provider, type);
    *certFact = tmp;
    REFCOUNT_ADD(*certFact);
    return NOERROR;
}

ECode CertificateFactory::GetProvider(
    /* [out] */ IProvider** provider)
{
    VALIDATE_NOT_NULL(provider)
    *provider = mProvider;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode CertificateFactory::GetType(
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type)
    *type = mType;
    return NOERROR;
}

ECode CertificateFactory::GenerateCertificate(
    /* [in] */ IInputStream* inStream,
    /* [out] */ ICertificate** cert)
{
    return mSpiImpl->EngineGenerateCertificate(inStream, cert);
}

ECode CertificateFactory::GetCertPathEncodings(
    /* [out] */ IIterator** it)
{
    return mSpiImpl->EngineGetCertPathEncodings(it);
}

ECode CertificateFactory::GenerateCertPath(
    /* [in] */ IInputStream* inStream,
    /* [out] */ ICertPath** certPath)
{
    VALIDATE_NOT_NULL(certPath)
    AutoPtr<IIterator> it;
    FAIL_RETURN(GetCertPathEncodings((IIterator**)&it))
    Boolean hasNext;
    if ((it->HasNext(&hasNext), !hasNext)) {
        Logger::E(SERVICE, "There are no CertPath encodings");
        *certPath = NULL;
        return E_CERTIFICATE_EXCEPTION;
    }
    AutoPtr<IInterface> next;
    it->GetNext((IInterface**)&next);
    String encoding;
    ICharSequence::Probe(next)->ToString(&encoding);
    return mSpiImpl->EngineGenerateCertPath(inStream, encoding, certPath);
}

ECode CertificateFactory::GenerateCertPath(
    /* [in] */ IInputStream* inStream,
    /* [in] */ const String& encoding,
    /* [out] */ ICertPath** certPath)
{
    return mSpiImpl->EngineGenerateCertPath(inStream, encoding, certPath);
}

ECode CertificateFactory::GenerateCertPath(
    /* [in] */ IList* certificates,
    /* [out] */ ICertPath** certPath)
{
    VALIDATE_NOT_NULL(certPath)
    return mSpiImpl->EngineGenerateCertPath(certificates, certPath);
}

ECode CertificateFactory::GenerateCertificates(
    /* [in] */ IInputStream* inStream,
    /* [out] */ ICollection** certs)
{
    return mSpiImpl->EngineGenerateCertificates(inStream, certs);
}

ECode CertificateFactory::GenerateCRL(
    /* [in] */ IInputStream* inStream,
    /* [out] */ ICRL** crl)
{
    return mSpiImpl->EngineGenerateCRL(inStream, crl);
}

ECode CertificateFactory::GenerateCRLs(
    /* [in] */ IInputStream* inStream,
    /* [out] */ ICollection** crls)
{
    return mSpiImpl->EngineGenerateCRLs(inStream, crls);
}

} // end Cert
} // end Security
} // end Elastos
