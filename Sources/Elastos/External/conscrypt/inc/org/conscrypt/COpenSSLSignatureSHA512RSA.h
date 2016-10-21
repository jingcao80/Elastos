
#ifndef __ORG_CONSCRYPT_COPENSSLSIGNATURESHA512RSA_H__
#define __ORG_CONSCRYPT_COPENSSLSIGNATURESHA512RSA_H__

#include "_Org_Conscrypt_COpenSSLSignatureSHA512RSA.h"
#include "OpenSSLSignature.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLSignatureSHA512RSA)
    , public OpenSSLSignatureSHA512RSA
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLSIGNATURESHA512RSA_H__
