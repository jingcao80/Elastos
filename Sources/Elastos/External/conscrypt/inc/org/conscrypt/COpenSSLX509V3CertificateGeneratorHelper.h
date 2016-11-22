#ifndef __ORG_CONSCRYPT_COPENSSLX509V3CERTIFICATEGENERATORHELPER_H__
#define __ORG_CONSCRYPT_COPENSSLX509V3CERTIFICATEGENERATORHELPER_H__

#include "_Org_Conscrypt_COpenSSLX509V3CertificateGeneratorHelper.h"
#include "org/conscrypt/OpenSSLX509V3CertificateGenerator.h"
#include <elastos/core/Singleton.h>

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLX509V3CertificateGeneratorHelper)
    , public Singleton
    , public IOpenSSLX509V3CertificateGeneratorHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Generate(
        /* [in] */ IPublicKey *pubkey,
        /* [in] */ IPrivateKey *privkey,
        /* [in] */ IBigInteger* serialNumber,
        /* [in] */ IDate* start,
        /* [in] */ IDate* end,
        /* [in] */ const String& subjectName,
        /* [in] */ const String& hashalg,
        /* [out] */ IX509Certificate** result);

};

} // namespace Conscrypt
} // namespace Org

#endif // __ORG_CONSCRYPT_COPENSSLX509V3CERTIFICATEGENERATORHELPER_H__
