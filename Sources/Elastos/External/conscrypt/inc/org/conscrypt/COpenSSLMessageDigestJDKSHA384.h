
#ifndef __ORG_CONSCRYPT_COPENSSLMESSAGEDIGESTJDKSHA384_H__
#define __ORG_CONSCRYPT_COPENSSLMESSAGEDIGESTJDKSHA384_H__

#include "_Org_Conscrypt_COpenSSLMessageDigestJDKSHA384.h"
#include "OpenSSLMessageDigestJDK.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLMessageDigestJDKSHA384)
    , public OpenSSLMessageDigestJDKSHA384
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLMESSAGEDIGESTJDKSHA384_H__
