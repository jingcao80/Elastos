
#ifndef __ORG_CONSCRYPT_COPENSSLSIGNATURESHA256ECDSA_H__
#define __ORG_CONSCRYPT_COPENSSLSIGNATURESHA256ECDSA_H__

#include "_Org_Conscrypt_COpenSSLSignatureSHA256ECDSA.h"
#include "OpenSSLSignature.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLSignatureSHA256ECDSA)
    , public OpenSSLSignatureSHA256ECDSA
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLSIGNATURESHA256ECDSA_H__
