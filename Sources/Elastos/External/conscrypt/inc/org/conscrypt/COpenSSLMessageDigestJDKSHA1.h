
#ifndef __ORG_CONSCRYPT_COPENSSLMESSAGEDIGESTJDKSHA1_H__
#define __ORG_CONSCRYPT_COPENSSLMESSAGEDIGESTJDKSHA1_H__

#include "_Org_Conscrypt_COpenSSLMessageDigestJDKSHA1.h"
#include "OpenSSLMessageDigestJDK.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLMessageDigestJDKSHA1)
    , public OpenSSLMessageDigestJDKSHA1
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLMESSAGEDIGESTJDKSHA1_H__
