
#include "OpenSSLNativeReference.h"

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLNativeReference::
//=========================================
CAR_INTERFACE_IMPL(OpenSSLNativeReference, Object, IOpenSSLNativeReference);

OpenSSLNativeReference::OpenSSLNativeReference()
    : mContext(0)
{
}

ECode OpenSSLNativeReference::constructor(
    /* [in] */ Int64 ctx)
{
    if (ctx == 0) {
        //throw new NullPointerException("ctx == 0");
        return NOERROR;
    }

    mContext = ctx;

    return NOERROR;
}

ECode OpenSSLNativeReference::GetNativeContext(
    /* [out] */ Int64* ctxRef)
{
    VALIDATE_NOT_NULL(ctxRef);
    *ctxRef = mContext;
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
