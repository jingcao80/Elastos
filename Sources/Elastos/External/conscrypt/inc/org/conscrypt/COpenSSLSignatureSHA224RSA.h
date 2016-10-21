
#ifndef __ORG_CONSCRYPT_COPENSSLSIGNATURESHA224RSA_H__
#define __ORG_CONSCRYPT_COPENSSLSIGNATURESHA224RSA_H__

#include "_Org_Conscrypt_COpenSSLSignatureSHA224RSA.h"
#include "OpenSSLSignature.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLSignatureSHA224RSA)
    , public OpenSSLSignatureSHA224RSA
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLSIGNATURESHA224RSA_H__
