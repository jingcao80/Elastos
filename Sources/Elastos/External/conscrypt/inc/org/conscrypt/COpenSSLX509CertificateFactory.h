
#ifndef __ORG_CONSCRYPT_COPENSSLX509CERTIFICATEFACTORY_H__
#define __ORG_CONSCRYPT_COPENSSLX509CERTIFICATEFACTORY_H__

#include "_Org_Conscrypt_COpenSSLX509CertificateFactory.h"
#include "OpenSSLX509CertificateFactory.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLX509CertificateFactory)
    , public OpenSSLX509CertificateFactory
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLX509CERTIFICATEFACTORY_H__
