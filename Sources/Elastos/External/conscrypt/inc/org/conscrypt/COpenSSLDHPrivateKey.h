
#ifndef __ORG_CONSCRYPT_COPENSSLDHPRIVATEKEY_H__
#define __ORG_CONSCRYPT_COPENSSLDHPRIVATEKEY_H__

#include "_Org_Conscrypt_COpenSSLDHPrivateKey.h"
#include "OpenSSLDHPrivateKey.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLDHPrivateKey)
    , public OpenSSLDHPrivateKey
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLDHPRIVATEKEY_H__
