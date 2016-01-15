
#include "CX509CertificateHelper.h"

namespace Elastosx {
namespace Security {
namespace Cert {

CAR_SINGLETON_IMPL(CX509CertificateHelper)

CAR_INTERFACE_IMPL(CX509CertificateHelper, Singleton, IX509CertificateHelper)

ECode CX509CertificateHelper::GetInstance(
    /* [in] */ IInputStream *inStream,
    /* [out] */ IX509Certificate **cert)
{
    return X509Certificate::GetInstance(inStream, cert);
}

ECode CX509CertificateHelper::GetInstance(
    /* [in] */ ArrayOf<Byte>* certData,
    /* [out] */ IX509Certificate **cert)
{
    return X509Certificate::GetInstance(certData, cert);
}

} // namespace Cert
} // namespace Security
} // namespace Elastosx

