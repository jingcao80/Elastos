
#include "org/apache/http/impl/cookie/BasicSecureHandler.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

ECode BasicSecureHandler::Parse(
    /* [in] */ ISetCookie* cookie,
    /* [in] */ const String& value)
{
    if (cookie == NULL) {
        Logger::E("BasicSecureHandler", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return cookie->SetSecure(TRUE);
}

ECode BasicSecureHandler::Match(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (cookie == NULL) {
        Logger::E("BasicSecureHandler", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (origin == NULL) {
        Logger::E("BasicSecureHandler", "Cookie origin may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean cookieIsSecure, originIsSecure;
    cookie->IsSecure(&cookieIsSecure);
    origin->IsSecure(&originIsSecure);
    *result = !cookieIsSecure || originIsSecure;
    return NOERROR;
}

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org