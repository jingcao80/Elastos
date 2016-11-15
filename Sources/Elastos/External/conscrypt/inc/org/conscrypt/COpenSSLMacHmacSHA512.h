#ifndef __ORG_CONSCRYPT_COPENSSLMACHMACSHA512_H__
#define __ORG_CONSCRYPT_COPENSSLMACHMACSHA512_H__

#include "_Org_Conscrypt_COpenSSLMacHmacSHA512.h"
#include "org/conscrypt/OpenSSLMac.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLMacHmacSHA512)
    , public HmacSHA512
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif // __ORG_CONSCRYPT_COPENSSLMACHMACSHA512_H__
