
#ifndef __ORG_CONSCRYPT_COPENSSLSIGNATURESHA1ECDSA_H__
#define __ORG_CONSCRYPT_COPENSSLSIGNATURESHA1ECDSA_H__

#include "_Org_Conscrypt_COpenSSLSignatureSHA1ECDSA.h"
#include "OpenSSLSignature.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLSignatureSHA1ECDSA)
    , public OpenSSLSignatureSHA1ECDSA
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLSIGNATURESHA1ECDSA_H__
