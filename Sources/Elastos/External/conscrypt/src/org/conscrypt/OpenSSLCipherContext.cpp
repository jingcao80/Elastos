#include "org/conscrypt/OpenSSLCipherContext.h"
#include "org/conscrypt/NativeCrypto.h"

namespace Org {
namespace Conscrypt {

CAR_INTERFACE_IMPL(OpenSSLCipherContext, Object, IOpenSSLCipherContext)

OpenSSLCipherContext::OpenSSLCipherContext()
    : mContext(0)
{}

OpenSSLCipherContext::~OpenSSLCipherContext()
{
    // try {
    NativeCrypto::EVP_CIPHER_CTX_cleanup(mContext);
    // } finally {
    //     super.finalize();
    // }
}

ECode OpenSSLCipherContext::constructor(
    /* [in] */ Int64 ctx)
{
    if (ctx == 0) {
        // throw new NullPointerException("ctx == 0");
        return E_NULL_POINTER_EXCEPTION;
    }

    mContext = ctx;
    return NOERROR;
}

ECode OpenSSLCipherContext::GetContext(
    /* [out] */  Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mContext;
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
