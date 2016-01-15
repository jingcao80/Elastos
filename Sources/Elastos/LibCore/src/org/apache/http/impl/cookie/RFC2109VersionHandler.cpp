
#include "org/apache/http/impl/cookie/RFC2109VersionHandler.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

RFC2109VersionHandler::RFC2109VersionHandler()
{}

ECode RFC2109VersionHandler::Parse(
    /* [in] */ ISetCookie* cookie,
    /* [in] */ const String& value)
{
    if (cookie == NULL) {
        Logger::E("RFC2109VersionHandler", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (value.IsNull()) {
        Logger::E("RFC2109VersionHandler", "Missing value for version attribute");
        return E_MALFORMED_COOKIE_EXCEPTION;
    }
    if (value.Trim().GetLength() == 0) {
        Logger::E("RFC2109VersionHandler", "Blank value for version attribute");
        return E_MALFORMED_COOKIE_EXCEPTION;
    }
    // try {
    return cookie->SetVersion(StringUtils::ParseInt32(value));
    // } catch (NumberFormatException e) {
    //     throw new MalformedCookieException("Invalid version: "
    //         + e.getMessage());
    // }
}

ECode RFC2109VersionHandler::Validate(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin)
{
    if (cookie == NULL) {
        Logger::E("RFC2109VersionHandler", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 version;
    if (cookie->GetVersion(&version), version < 0) {
        Logger::E("RFC2109VersionHandler", "Cookie version may not be negative");
        return E_MALFORMED_COOKIE_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org