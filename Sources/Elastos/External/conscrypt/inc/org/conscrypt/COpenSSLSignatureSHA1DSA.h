
#ifndef __ORG_CONSCRYPT_COPENSSLSIGNATURESHA1DSA_H__
#define __ORG_CONSCRYPT_COPENSSLSIGNATURESHA1DSA_H__

#include "_Org_Conscrypt_COpenSSLSignatureSHA1DSA.h"
#include "OpenSSLSignature.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLSignatureSHA1DSA)
    , public OpenSSLSignatureSHA1DSA
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLSIGNATURESHA1DSA_H__
