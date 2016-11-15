#ifndef __ORG_CONSCRYPT_COPENSSLMACHMACSHA224_H__
#define __ORG_CONSCRYPT_COPENSSLMACHMACSHA224_H__

#include "_Org_Conscrypt_COpenSSLMacHmacSHA224.h"
#include "org/conscrypt/OpenSSLMac.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLMacHmacSHA224)
    , public HmacSHA224
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif // __ORG_CONSCRYPT_COPENSSLMACHMACSHA224_H__
