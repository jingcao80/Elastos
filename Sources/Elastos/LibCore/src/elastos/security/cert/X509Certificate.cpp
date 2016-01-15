
#include "cert/X509Certificate.h"
#include "cert/CertificateFactory.h"
#include "io/CByteArrayInputStream.h"
#include "utility/logging/Logger.h"

using Elastos::IO::IInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Security {
namespace Cert {

CAR_INTERFACE_IMPL_2(X509Certificate, Certificate, IX509Certificate, IX509Extension);

ECode X509Certificate::constructor()
{
    return Certificate::constructor(String("X.509"));
}

ECode X509Certificate::GetIssuerX500Principal(
    /* [out] */ IX500Principal** issuer)
{
    VALIDATE_NOT_NULL(issuer)
    // TODO if there is no X.509 certificate provider installed
    // should we try to access Harmony X509CertImpl via classForName?
    AutoPtr<ICertificateFactory> factory;
    ECode ec = CertificateFactory::GetInstance(String("X.509"), (ICertificateFactory**)&factory);
    if (FAILED(ec)) {
        Logger::E("X509Certificate", "Failed to get X500Principal issuer");
        *issuer = NULL;
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<ArrayOf<Byte> > encode;
    ec = GetEncoded((ArrayOf<Byte>**)&encode);
    if (FAILED(ec)) {
        Logger::E("X509Certificate", "Failed to get X500Principal issuer");
        *issuer = NULL;
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<IInputStream> bais;
    CByteArrayInputStream::New(encode, (IInputStream**)&bais);
    AutoPtr<ICertificate> cert;
    ec = factory->GenerateCertificate(bais, (ICertificate**)&cert);
    if (FAILED(ec)) {
        Logger::E("X509Certificate", "Failed to get X500Principal issuer");
        *issuer = NULL;
        return E_RUNTIME_EXCEPTION;
    }

    IX509Certificate* x509Cert = IX509Certificate::Probe(cert);
    ec = x509Cert->GetIssuerX500Principal(issuer);
    if (FAILED(ec)) {
        Logger::E("X509Certificate", "Failed to get X500Principal issuer");
        *issuer = NULL;
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode X509Certificate::GetSubjectX500Principal(
    /* [out] */ IX500Principal** principal)
{
    VALIDATE_NOT_NULL(principal)
    // TODO if there is no X.509 certificate provider installed
    // should we try to access Harmony X509CertImpl via classForName?
    AutoPtr<ICertificateFactory> factory;
    ECode ec = CertificateFactory::GetInstance(String("X.509"), (ICertificateFactory**)&factory);
    if (FAILED(ec)) {
        Logger::E("X509Certificate", "Failed to get X500Principal subject");
        *principal = NULL;
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<ArrayOf<Byte> > encode;
    ec = GetEncoded((ArrayOf<Byte>**)&encode);
    if (FAILED(ec)) {
        Logger::E("X509Certificate", "Failed to get X500Principal subject");
        *principal = NULL;
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<IInputStream> bais;
    CByteArrayInputStream::New(encode, (IInputStream**)&bais);
    AutoPtr<ICertificate> cert;
    ec = factory->GenerateCertificate(bais, (ICertificate**)&cert);
    if (FAILED(ec)) {
        Logger::E("X509Certificate", "Failed to get X500Principal subject");
        *principal = NULL;
        return E_RUNTIME_EXCEPTION;
    }

    IX509Certificate* x509Cert = IX509Certificate::Probe(cert);
    ec = x509Cert->GetSubjectX500Principal(principal);
    if (FAILED(ec)) {
        Logger::E("X509Certificate", "Failed to get X500Principal subject");
        *principal = NULL;
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode X509Certificate::GetExtendedKeyUsage(
    /* [out] */ IList** usage)
{
    VALIDATE_NOT_NULL(usage);
    *usage = NULL;
    return NOERROR;
}

ECode X509Certificate::GetSubjectAlternativeNames(
    /* [out] */ ICollection** names)
{
    VALIDATE_NOT_NULL(names);
    *names = NULL;
    return NOERROR;
}

ECode X509Certificate::GetIssuerAlternativeNames(
    /* [out] */ ICollection** names)
{
    VALIDATE_NOT_NULL(names);
    *names = NULL;
    return NOERROR;
}

} // end Cert
} // end Security
} // end Elastos
