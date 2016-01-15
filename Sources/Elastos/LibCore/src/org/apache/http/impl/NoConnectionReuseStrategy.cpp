
#include "org/apache/http/impl/NoConnectionReuseStrategy.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {

CAR_INTERFACE_IMPL(NoConnectionReuseStrategy, Object, IConnectionReuseStrategy)

ECode NoConnectionReuseStrategy::KeepAlive(
    /* [in] */ IHttpResponse* response,
    /* [in] */ IHttpContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (response == NULL) {
        Logger::E("NoConnectionReuseStrategy", "HTTP response may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (context == NULL) {
        Logger::E("NoConnectionReuseStrategy", "HTTP context may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org