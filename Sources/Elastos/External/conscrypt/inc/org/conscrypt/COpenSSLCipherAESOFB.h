#ifndef __ORG_CONSCRYPT_COPENSSLCIPHERAESOFB_H__
#define __ORG_CONSCRYPT_COPENSSLCIPHERAESOFB_H__

#include "_Org_Conscrypt_COpenSSLCipherAESOFB.h"
#include "org/conscrypt/OpenSSLCipher.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLCipherAESOFB)
    , public OpenSSLCipher_AES_OFB
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif // __ORG_CONSCRYPT_COPENSSLCIPHERAESOFB_H__
