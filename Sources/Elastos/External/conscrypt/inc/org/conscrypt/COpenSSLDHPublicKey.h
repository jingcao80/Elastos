
#ifndef __ORG_CONSCRYPT_COPENSSLDHPUBLICKEY_H__
#define __ORG_CONSCRYPT_COPENSSLDHPUBLICKEY_H__

#include "_Org_Conscrypt_COpenSSLDHPublicKey.h"
#include "OpenSSLDHPublicKey.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLDHPublicKey)
    , public OpenSSLDHPublicKey
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLDHPUBLICKEY_H__
