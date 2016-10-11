
#ifndef __ORG_CONSCRYPT_COPENSSLSIGNATURE_H__
#define __ORG_CONSCRYPT_COPENSSLSIGNATURE_H__

#include "_Org_Conscrypt_COpenSSLSignature.h"
#include "OpenSSLSignature.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLSignature)
    , public OpenSSLSignature
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLSIGNATURE_H__
