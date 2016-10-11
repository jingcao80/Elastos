
#ifndef __ORG_CONSCRYPT_COPENSSLRSAPUBLICKEY_H__
#define __ORG_CONSCRYPT_COPENSSLRSAPUBLICKEY_H__

#include "_Org_Conscrypt_COpenSSLRSAPublicKey.h"
#include "OpenSSLRSAPublicKey.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLRSAPublicKey)
    , public OpenSSLRSAPublicKey
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLRSAPUBLICKEY_H__
