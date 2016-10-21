
#ifndef __ORG_CONSCRYPT_COPENSSLSIGNATUREMD5RSA_H__
#define __ORG_CONSCRYPT_COPENSSLSIGNATUREMD5RSA_H__

#include "_Org_Conscrypt_COpenSSLSignatureMD5RSA.h"
#include "OpenSSLSignature.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLSignatureMD5RSA)
    , public OpenSSLSignatureMD5RSA
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLSIGNATUREMD5RSA_H__
