
#ifndef __ORG_CONSCRYPT_COPENSSLDIGESTCONTEXT_H__
#define __ORG_CONSCRYPT_COPENSSLDIGESTCONTEXT_H__

#include "_Org_Conscrypt_COpenSSLDigestContext.h"
#include "OpenSSLDigestContext.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLDigestContext)
    , public OpenSSLDigestContext
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLDIGESTCONTEXT_H__
