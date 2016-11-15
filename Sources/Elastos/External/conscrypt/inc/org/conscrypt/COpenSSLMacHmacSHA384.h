#ifndef __ORG_CONSCRYPT_COPENSSLMACHMACSHA384_H__
#define __ORG_CONSCRYPT_COPENSSLMACHMACSHA384_H__

#include "_Org_Conscrypt_COpenSSLMacHmacSHA384.h"
#include "org/conscrypt/OpenSSLMac.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLMacHmacSHA384)
    , public HmacSHA384
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif // __ORG_CONSCRYPT_COPENSSLMACHMACSHA384_H__
