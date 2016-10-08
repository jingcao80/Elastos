
#ifndef __ORG_CONSCRYPT_OPENSSLDIGESTCONTEXT_H__
#define __ORG_CONSCRYPT_OPENSSLDIGESTCONTEXT_H__

#include <elastos/coredef.h>
#include <elastos/core/Object.h>
#include "org/conscrypt/OpenSSLNativeReference.h"
#include "_Org.Conscrypt.h"

namespace Org {
namespace Conscrypt {

class OpenSSLDigestContext
    : public IOpenSSLDigestContext
    , public OpenSSLNativeReference
{
public:
    CAR_INTERFACE_DECL()

    OpenSSLDigestContext(
        /* [in] */ Int64 ctx);

    ~OpenSSLDigestContext();
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLDIGESTCONTEXT_H__
