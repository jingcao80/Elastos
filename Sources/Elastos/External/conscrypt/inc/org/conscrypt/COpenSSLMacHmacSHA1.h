#ifndef __ORG_CONSCRYPT_COPENSSLMACHMACSHA1_H__
#define __ORG_CONSCRYPT_COPENSSLMACHMACSHA1_H__

#include "_Org_Conscrypt_COpenSSLMacHmacSHA1.h"
#include "org/conscrypt/OpenSSLMac.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLMacHmacSHA1)
    , public HmacSHA1
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif // __ORG_CONSCRYPT_COPENSSLMACHMACSHA1_H__
