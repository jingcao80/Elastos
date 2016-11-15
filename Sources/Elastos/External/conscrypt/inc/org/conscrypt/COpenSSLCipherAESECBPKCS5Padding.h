#ifndef __ORG_CONSCRYPT_COPENSSLCIPHERAESECBPKCS5PADDING_H__
#define __ORG_CONSCRYPT_COPENSSLCIPHERAESECBPKCS5PADDING_H__

#include "_Org_Conscrypt_COpenSSLCipherAESECBPKCS5Padding.h"
#include "org/conscrypt/OpenSSLCipher.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLCipherAESECBPKCS5Padding)
    , public OpenSSLCipher_AES_ECB_PKCS5Padding
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif // __ORG_CONSCRYPT_COPENSSLCIPHERAESECBPKCS5PADDING_H__
