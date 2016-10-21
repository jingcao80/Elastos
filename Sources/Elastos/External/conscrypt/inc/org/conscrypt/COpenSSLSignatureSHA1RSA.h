
#ifndef __ORG_CONSCRYPT_COPENSSLSIGNATURESHA1RSA_H__
#define __ORG_CONSCRYPT_COPENSSLSIGNATURESHA1RSA_H__

#include "_Org_Conscrypt_COpenSSLSignatureSHA1RSA.h"
#include "OpenSSLSignature.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLSignatureSHA1RSA)
    , public OpenSSLSignatureSHA1RSA
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLSIGNATURESHA1RSA_H__
