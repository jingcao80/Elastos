#ifndef __ORG_CONSCRYPT_COPENSSLCIPHERAESCBCPKCS5PADDING_H__
#define __ORG_CONSCRYPT_COPENSSLCIPHERAESCBCPKCS5PADDING_H__

#include "_Org_Conscrypt_COpenSSLCipherAESCBCPKCS5Padding.h"
#include "org/conscrypt/OpenSSLCipher.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLCipherAESCBCPKCS5Padding)
    , public OpenSSLCipher_AES_CBC_PKCS5Padding
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif // __ORG_CONSCRYPT_COPENSSLCIPHERAESCBCPKCS5PADDING_H__
