
#ifndef __ORG_CONSCRYPT_COPENSSLDSAPUBLICKEY_H__
#define __ORG_CONSCRYPT_COPENSSLDSAPUBLICKEY_H__

#include "_Org_Conscrypt_COpenSSLDSAPublicKey.h"
#include "OpenSSLDSAPublicKey.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLDSAPublicKey)
    , public OpenSSLDSAPublicKey
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLDSAPUBLICKEY_H__
