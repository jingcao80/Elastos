
#ifndef __ORG_CONSCRYPT_COPENSSLMESSAGEDIGESTJDKSHA256_H__
#define __ORG_CONSCRYPT_COPENSSLMESSAGEDIGESTJDKSHA256_H__

#include "_Org_Conscrypt_COpenSSLMessageDigestJDKSHA256.h"
#include "OpenSSLMessageDigestJDK.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLMessageDigestJDKSHA256)
    , public OpenSSLMessageDigestJDKSHA256
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLMESSAGEDIGESTJDKSHA256_H__
