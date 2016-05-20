
#include "CertificateFactorySpi.h"

namespace Elastos {
namespace Security {
namespace Cert {

CAR_INTERFACE_IMPL(CertificateFactorySpi, Object, ICertificateFactorySpi)
CertificateFactorySpi::CertificateFactorySpi()
{}

ECode CertificateFactorySpi::EngineGenerateCertPath(
    /* [in] */ IInputStream* inStream,
    /* [out] */ ICertPath** certPath)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CertificateFactorySpi::EngineGenerateCertPath(
    /* [in] */ IInputStream* inStream,
    /* [in] */ const String& encoding,
    /* [out] */ ICertPath** certPath)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CertificateFactorySpi::EngineGenerateCertPath(
    /* [in] */ IList** certificates,
    /* [out] */ ICertPath** certPath)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CertificateFactorySpi::EngineGetCertPathEncodings(
    /* [out] */ ICertPath** IIterator)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

} // end Cert
} // end Security
} // end Elastos