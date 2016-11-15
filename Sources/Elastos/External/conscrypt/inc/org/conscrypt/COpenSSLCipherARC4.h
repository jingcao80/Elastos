#ifndef __ORG_CONSCRYPT_COPENSSLCIPHERARC4_H__
#define __ORG_CONSCRYPT_COPENSSLCIPHERARC4_H__

#include "_Org_Conscrypt_COpenSSLCipherARC4.h"
#include "org/conscrypt/OpenSSLCipher.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLCipherARC4)
    , public OpenSSLCipher_ARC4
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif // __ORG_CONSCRYPT_COPENSSLCIPHERARC4_H__
