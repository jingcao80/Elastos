#ifndef __ORG_CONSCRYPT_COPENSSLCIPHERAESCTR_H__
#define __ORG_CONSCRYPT_COPENSSLCIPHERAESCTR_H__

#include "_Org_Conscrypt_COpenSSLCipherAESCTR.h"
#include "org/conscrypt/OpenSSLCipher.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLCipherAESCTR)
    , public OpenSSLCipher_AES_CTR
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif // __ORG_CONSCRYPT_COPENSSLCIPHERAESCTR_H__
