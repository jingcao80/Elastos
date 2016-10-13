
#include "OpenSSLNativeReference.h"

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLNativeReference::
//=========================================

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

} // namespace Conscrypt
} // namespace Org
