
#ifndef __ORG_CONSCRYPT_COpenSSLMessageDigestJDKMD5_H__
#define __ORG_CONSCRYPT_COpenSSLMessageDigestJDKMD5_H__

#include "_Org_Conscrypt_COpenSSLMessageDigestJDKMD5.h"
#include "OpenSSLMessageDigestJDK.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLMessageDigestJDKMD5)
    , public OpenSSLMessageDigestJDKMD5
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COpenSSLMessageDigestJDKMD5_H__
