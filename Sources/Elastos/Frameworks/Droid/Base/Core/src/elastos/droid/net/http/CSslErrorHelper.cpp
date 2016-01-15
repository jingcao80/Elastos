
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/http/CSslError.h"
#include "elastos/droid/net/http/CSslErrorHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CAR_INTERFACE_IMPL(CSslErrorHelper, Singleton, ISslErrorHelper)

CAR_SINGLETON_IMPL(CSslErrorHelper)

ECode CSslErrorHelper::SslErrorFromChromiumErrorCode(
    /* [in] */ Int32 error,
    /* [in] */ ISslCertificate* cert,
    /* [in] */ const String& url,
    /* [out] */ ISslError** sslError)
{
    return SslError::SslErrorFromChromiumErrorCode(error, cert, url, sslError);
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
