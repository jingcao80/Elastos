
#ifndef __ORG_CONSCRYPT_COPENSSLDSAPRIVATEKEY_H__
#define __ORG_CONSCRYPT_COPENSSLDSAPRIVATEKEY_H__

#include "_Org_Conscrypt_COpenSSLDSAPrivateKey.h"
#include "OpenSSLDSAPrivateKey.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLDSAPrivateKey)
    , public OpenSSLDSAPrivateKey
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLDSAPRIVATEKEY_H__
