#ifndef __ORG_CONSCRYPT_COPENSSLCIPHERDESEDECFB_H__
#define __ORG_CONSCRYPT_COPENSSLCIPHERDESEDECFB_H__

#include "_Org_Conscrypt_COpenSSLCipherDESEDECFB.h"
#include "org/conscrypt/OpenSSLCipher.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLCipherDESEDECFB)
    , public OpenSSLCipher_DESEDE_CFB
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif // __ORG_CONSCRYPT_COPENSSLCIPHERDESEDECFB_H__
