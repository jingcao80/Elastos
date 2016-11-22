#include "org/conscrypt/COpenSSLX509V3CertificateGeneratorHelper.h"

namespace Org {
namespace Conscrypt {

CAR_SINGLETON_IMPL(COpenSSLX509V3CertificateGeneratorHelper)

CAR_INTERFACE_IMPL(COpenSSLX509V3CertificateGeneratorHelper, Singleton, IOpenSSLX509V3CertificateGeneratorHelper)

ECode COpenSSLX509V3CertificateGeneratorHelper::Generate(
    /* [in] */ IPublicKey *pubkey,
    /* [in] */ IPrivateKey *privkey,
    /* [in] */ IBigInteger* serialNumber,
    /* [in] */ IDate* start,
    /* [in] */ IDate* end,
    /* [in] */ const String& subjectName,
    /* [in] */ const String& hashalg,
    /* [out] */ IX509Certificate** result)
{
    return OpenSSLX509V3CertificateGenerator::Generate(pubkey, privkey, serialNumber, start, end, subjectName, hashalg, result);
}

} // namespace Conscrypt
} // namespace Org
