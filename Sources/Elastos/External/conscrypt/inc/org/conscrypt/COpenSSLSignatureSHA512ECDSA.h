
#ifndef __ORG_CONSCRYPT_COPENSSLSIGNATURESHA512ECDSA_H__
#define __ORG_CONSCRYPT_COPENSSLSIGNATURESHA512ECDSA_H__

#include "_Org_Conscrypt_COpenSSLSignatureSHA512ECDSA.h"
#include "OpenSSLSignature.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLSignatureSHA512ECDSA)
    , public OpenSSLSignatureSHA512ECDSA
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLSIGNATURESHA512ECDSA_H__
