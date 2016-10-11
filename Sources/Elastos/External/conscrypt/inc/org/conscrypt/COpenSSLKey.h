
#ifndef __ORG_CONSCRYPT_COPENSSLKEY_H__
#define __ORG_CONSCRYPT_COPENSSLKEY_H__

#include "_Org_Conscrypt_COpenSSLKey.h"
#include "OpenSSLKey.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLKey)
    , public OpenSSLKey
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLKEY_H__
