
#ifndef __ORG_CONSCRYPT_COPENSSLSIGNATURESHA256RSA_H__
#define __ORG_CONSCRYPT_COPENSSLSIGNATURESHA256RSA_H__

#include "_Org_Conscrypt_COpenSSLSignatureSHA256RSA.h"
#include "OpenSSLSignature.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLSignatureSHA256RSA)
    , public OpenSSLSignatureSHA256RSA
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLSIGNATURESHA256RSA_H__
