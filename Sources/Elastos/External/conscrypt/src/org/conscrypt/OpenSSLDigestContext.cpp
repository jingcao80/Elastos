#include "org/conscrypt/OpenSSLDigestContext.h"
#include "org/conscrypt/NativeCrypto.h"

namespace Org {
namespace Conscrypt {

CAR_INTERFACE_IMPL(OpenSSLDigestContext, OpenSSLNativeReference, IOpenSSLDigestContext)

OpenSSLDigestContext::OpenSSLDigestContext(
    /* [in] */ Int64 ctx)
    : OpenSSLNativeReference(ctx)
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

} // namespace Conscrypt
} // namespace Org
