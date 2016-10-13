#include "org/conscrypt/OpenSSLDigestContext.h"
#include "org/conscrypt/NativeCrypto.h"

namespace Org {
namespace Conscrypt {

CAR_INTERFACE_IMPL(OpenSSLDigestContext, OpenSSLNativeReference, IOpenSSLDigestContext)

OpenSSLDigestContext::OpenSSLDigestContext()
{
}

OpenSSLDigestContext::~OpenSSLDigestContext()
{
    // try {
    NativeCrypto::EVP_MD_CTX_destroy(mContext);
    // } finally {
    //     super.finalize();
    // }
}

ECode OpenSSLDigestContext::constructor(
    /* [in] */ Int64 ctx)
{
    return OpenSSLNativeReference::constructor(ctx);
}

} // namespace Conscrypt
} // namespace Org
