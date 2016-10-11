
#ifndef __ORG_CONSCRYPT_COPENSSLX509CERTIFICATE_H__
#define __ORG_CONSCRYPT_COPENSSLX509CERTIFICATE_H__

#include "_Org_Conscrypt_COpenSSLX509Certificate.h"
#include "OpenSSLX509Certificate.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLX509Certificate)
    , public OpenSSLX509Certificate
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLX509CERTIFICATE_H__
