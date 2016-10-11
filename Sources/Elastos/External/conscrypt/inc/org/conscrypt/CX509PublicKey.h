
#ifndef __ORG_CONSCRYPT_CX509PUBLICKEY_H__
#define __ORG_CONSCRYPT_CX509PUBLICKEY_H__

#include "_Org_Conscrypt_CX509PublicKey.h"
#include "X509PublicKey.h"

namespace Org {
namespace Conscrypt {

CarClass(CX509PublicKey)
    , public X509PublicKey
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_CX509PUBLICKEY_H__
