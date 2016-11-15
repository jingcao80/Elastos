#ifndef __ORG_CONSCRYPT_COPENSSLCIPHERDESEDEECBNOPADDING_H__
#define __ORG_CONSCRYPT_COPENSSLCIPHERDESEDEECBNOPADDING_H__

#include "_Org_Conscrypt_COpenSSLCipherDESEDEECBNoPadding.h"
#include "org/conscrypt/OpenSSLCipher.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLCipherDESEDEECBNoPadding)
    , public OpenSSLCipher_DESEDE_ECB_NoPadding
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif // __ORG_CONSCRYPT_COPENSSLCIPHERDESEDEECBNOPADDING_H__
