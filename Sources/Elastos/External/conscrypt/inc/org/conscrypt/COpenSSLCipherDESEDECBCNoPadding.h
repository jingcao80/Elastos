#ifndef __ORG_CONSCRYPT_COPENSSLCIPHERDESEDECBCNOPADDING_H__
#define __ORG_CONSCRYPT_COPENSSLCIPHERDESEDECBCNOPADDING_H__

#include "_Org_Conscrypt_COpenSSLCipherDESEDECBCNoPadding.h"
#include "org/conscrypt/OpenSSLCipher.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLCipherDESEDECBCNoPadding)
    , public OpenSSLCipher_DESEDE_CBC_NoPadding
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif // __ORG_CONSCRYPT_COPENSSLCIPHERDESEDECBCNOPADDING_H__
