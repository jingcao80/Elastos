
#ifndef __ORG_CONSCRYPT_COPENSSLX509CERTPATH_H__
#define __ORG_CONSCRYPT_COPENSSLX509CERTPATH_H__

#include "_Org_Conscrypt_COpenSSLX509CertPath.h"
#include "OpenSSLX509CertPath.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLX509CertPath)
    , public OpenSSLX509CertPath
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLX509CERTPATH_H__
