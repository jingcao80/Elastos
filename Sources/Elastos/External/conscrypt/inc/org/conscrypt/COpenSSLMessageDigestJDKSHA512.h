
#ifndef __ORG_CONSCRYPT_COPENSSLMESSAGEDIGESTJDKSHA512_H__
#define __ORG_CONSCRYPT_COPENSSLMESSAGEDIGESTJDKSHA512_H__

#include "_Org_Conscrypt_COpenSSLMessageDigestJDKSHA512.h"
#include "OpenSSLMessageDigestJDK.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLMessageDigestJDKSHA512)
    , public OpenSSLMessageDigestJDKSHA512
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLMESSAGEDIGESTJDKSHA512_H__
