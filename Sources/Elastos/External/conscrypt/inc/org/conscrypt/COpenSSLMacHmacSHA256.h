#ifndef __ORG_CONSCRYPT_COPENSSLMACHMACSHA256_H__
#define __ORG_CONSCRYPT_COPENSSLMACHMACSHA256_H__

#include "_Org_Conscrypt_COpenSSLMacHmacSHA256.h"
#include "org/conscrypt/OpenSSLMac.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLMacHmacSHA256)
    , public HmacSHA256
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif // __ORG_CONSCRYPT_COPENSSLMACHMACSHA256_H__
