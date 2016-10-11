
#ifndef __ORG_CONSCRYPT_COPENSSLSIGNATURERAWRSA_H__
#define __ORG_CONSCRYPT_COPENSSLSIGNATURERAWRSA_H__

#include "_Org_Conscrypt_COpenSSLSignatureRawRSA.h"
#include "OpenSSLSignatureRawRSA.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLSignatureRawRSA)
    , public OpenSSLSignatureRawRSA
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLSIGNATURERAWRSA_H__
