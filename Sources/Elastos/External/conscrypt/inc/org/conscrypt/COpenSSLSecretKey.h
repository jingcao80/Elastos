
#ifndef __ORG_CONSCRYPT_COPENSSLSECRETKEY_H__
#define __ORG_CONSCRYPT_COPENSSLSECRETKEY_H__

#include "_Org_Conscrypt_COpenSSLSecretKey.h"
#include "OpenSSLSecretKey.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLSecretKey)
    , public OpenSSLSecretKey
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLSECRETKEY_H__
