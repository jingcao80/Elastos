#ifndef __ORG_CONSCRYPT_COPENSSLMACHMACMD5_H__
#define __ORG_CONSCRYPT_COPENSSLMACHMACMD5_H__

#include "_Org_Conscrypt_COpenSSLMacHmacMD5.h"
#include "org/conscrypt/OpenSSLMac.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLMacHmacMD5)
    , public HmacMD5
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif // __ORG_CONSCRYPT_COPENSSLMACHMACMD5_H__
