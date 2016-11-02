
#ifndef __ORG_CONSCRYPT_COPENSSLCIPHERAESECBNOPADDING_H__
#define __ORG_CONSCRYPT_COPENSSLCIPHERAESECBNOPADDING_H__

#include "_Org_Conscrypt_COpenSSLCipherAESECBNoPadding.h"
#include "OpenSSLCipher.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLCipherAESECBNoPadding)
    , public OpenSSLCipher_AES_ECB_NoPadding
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLCIPHERAESECBNOPADDING_H__
