
#ifndef __ORG_CONSCRYPT_COPENSSLRSAPRIVATEKEY_H__
#define __ORG_CONSCRYPT_COPENSSLRSAPRIVATEKEY_H__

#include "_Org_Conscrypt_COpenSSLRSAPrivateKey.h"
#include "OpenSSLRSAPrivateKey.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLRSAPrivateKey)
    , public OpenSSLRSAPrivateKey
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLRSAPRIVATEKEY_H__
