
#ifndef __ORG_CONSCRYPT_COPENSSLMESSAGEDIGESTJDKSHA224_H__
#define __ORG_CONSCRYPT_COPENSSLMESSAGEDIGESTJDKSHA224_H__

#include "_Org_Conscrypt_COpenSSLMessageDigestJDKSHA224.h"
#include "OpenSSLMessageDigestJDK.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLMessageDigestJDKSHA224)
    , public OpenSSLMessageDigestJDKSHA224
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLMESSAGEDIGESTJDKSHA224_H__
