#ifndef __ORG_CONSCRYPT_COPENSSLCIPHERAESCBCNOPADDING_H__
#define __ORG_CONSCRYPT_COPENSSLCIPHERAESCBCNOPADDING_H__

#include "_Org_Conscrypt_COpenSSLCipherAESCBCNoPadding.h"
#include "org/conscrypt/OpenSSLCipher.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLCipherAESCBCNoPadding)
    , public OpenSSLCipher_AES_CBC_NoPadding
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif // __ORG_CONSCRYPT_COPENSSLCIPHERAESCBCNOPADDING_H__
