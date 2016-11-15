#ifndef __ORG_CONSCRYPT_COPENSSLCIPHERRSAPKCS1_H__
#define __ORG_CONSCRYPT_COPENSSLCIPHERRSAPKCS1_H__

#include "_Org_Conscrypt_COpenSSLCipherRSAPKCS1.h"
#include "org/conscrypt/OpenSSLCipherRSA.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLCipherRSAPKCS1)
    , public OpenSSLCipherRSA_PKCS1
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif // __ORG_CONSCRYPT_COPENSSLCIPHERRSAPKCS1_H__
