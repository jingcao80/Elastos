
#ifndef __ORG_CONSCRYPT_COPENSSLRSAPRIVATECRTKEY_H__
#define __ORG_CONSCRYPT_COPENSSLRSAPRIVATECRTKEY_H__

#include "_Org_Conscrypt_COpenSSLRSAPrivateCrtKey.h"
#include "OpenSSLRSAPrivateCrtKey.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLRSAPrivateCrtKey)
    , public OpenSSLRSAPrivateCrtKey
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLRSAPRIVATECRTKEY_H__
