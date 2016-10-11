
#ifndef __ORG_CONSCRYPT_COPENSSLMESSAGEDIGESTJDK_H__
#define __ORG_CONSCRYPT_COPENSSLMESSAGEDIGESTJDK_H__

#include "_Org_Conscrypt_COpenSSLMessageDigestJDK.h"
#include "OpenSSLMessageDigestJDK.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLMessageDigestJDK)
    , public OpenSSLMessageDigestJDK
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLMESSAGEDIGESTJDK_H__
