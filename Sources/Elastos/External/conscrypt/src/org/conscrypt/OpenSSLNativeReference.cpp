
#include "OpenSSLNativeReference.h"

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLNativeReference::
//=========================================

OpenSSLNativeReference::OpenSSLNativeReference(
    /* [in] */ Int64 ctx)
{
    if (ctx == 0) {
        //throw new NullPointerException("ctx == 0");
        return;
    }

    mContext = ctx;
}

} // namespace Conscrypt
} // namespace Org
