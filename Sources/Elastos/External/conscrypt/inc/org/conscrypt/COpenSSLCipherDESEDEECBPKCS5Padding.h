#ifndef __ORG_CONSCRYPT_COPENSSLCIPHERDESEDEECBPKCS5PADDING_H__
#define __ORG_CONSCRYPT_COPENSSLCIPHERDESEDEECBPKCS5PADDING_H__

#include "_Org_Conscrypt_COpenSSLCipherDESEDEECBPKCS5Padding.h"
#include "org/conscrypt/OpenSSLCipher.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLCipherDESEDEECBPKCS5Padding)
    , public OpenSSLCipher_DESEDE_ECB_PKCS5Padding
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif // __ORG_CONSCRYPT_COPENSSLCIPHERDESEDEECBPKCS5PADDING_H__
