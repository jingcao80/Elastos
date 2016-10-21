
#ifndef __ORG_CONSCRYPT_COPENSSLSIGNATURESHA384ECDSA_H__
#define __ORG_CONSCRYPT_COPENSSLSIGNATURESHA384ECDSA_H__

#include "_Org_Conscrypt_COpenSSLSignatureSHA384ECDSA.h"
#include "OpenSSLSignature.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLSignatureSHA384ECDSA)
    , public OpenSSLSignatureSHA384ECDSA
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLSIGNATURESHA384ECDSA_H__
