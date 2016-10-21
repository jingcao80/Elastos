
#ifndef __ORG_CONSCRYPT_COPENSSLSIGNATURESHA224ECDSA_H__
#define __ORG_CONSCRYPT_COPENSSLSIGNATURESHA224ECDSA_H__

#include "_Org_Conscrypt_COpenSSLSignatureSHA224ECDSA.h"
#include "OpenSSLSignature.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLSignatureSHA224ECDSA)
    , public OpenSSLSignatureSHA224ECDSA
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLSIGNATURESHA224ECDSA_H__
