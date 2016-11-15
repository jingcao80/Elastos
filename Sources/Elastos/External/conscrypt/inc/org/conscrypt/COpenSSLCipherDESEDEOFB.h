#ifndef __ORG_CONSCRYPT_COPENSSLCIPHERDESEDEOFB_H__
#define __ORG_CONSCRYPT_COPENSSLCIPHERDESEDEOFB_H__

#include "_Org_Conscrypt_COpenSSLCipherDESEDEOFB.h"
#include "org/conscrypt/OpenSSLCipher.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLCipherDESEDEOFB)
    , public OpenSSLCipher_DESEDE_OFB
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif // __ORG_CONSCRYPT_COPENSSLCIPHERDESEDEOFB_H__
