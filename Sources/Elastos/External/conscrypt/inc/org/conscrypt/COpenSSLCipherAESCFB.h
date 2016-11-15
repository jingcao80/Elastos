#ifndef __ORG_CONSCRYPT_COPENSSLCIPHERAESCFB_H__
#define __ORG_CONSCRYPT_COPENSSLCIPHERAESCFB_H__

#include "_Org_Conscrypt_COpenSSLCipherAESCFB.h"
#include "org/conscrypt/OpenSSLCipher.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLCipherAESCFB)
    , public OpenSSLCipher_AES_CFB
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif // __ORG_CONSCRYPT_COPENSSLCIPHERAESCFB_H__
