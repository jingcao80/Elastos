
#ifndef __ORG_CONSCRYPT_COPENSSLSIGNATURESHA384RSA_H__
#define __ORG_CONSCRYPT_COPENSSLSIGNATURESHA384RSA_H__

#include "_Org_Conscrypt_COpenSSLSignatureSHA384RSA.h"
#include "OpenSSLSignature.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLSignatureSHA384RSA)
    , public OpenSSLSignatureSHA384RSA
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLSIGNATURESHA384RSA_H__
