
#include "org/apache/http/impl/cookie/BasicPathHandler.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Cookie::EIID_ICookieAttributeHandler;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

CAR_INTERFACE_IMPL(BasicPathHandler, Object, ICookieAttributeHandler)

ECode BasicPathHandler::Parse(
    /* [in] */ ISetCookie* cookie,
    /* [in] */ const String& _value)
{
    String value = _value;
    if (cookie == NULL) {
        Logger::E("BasicPathHandler", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (value.IsNull() || value.Trim().GetLength() == 0) {
        value = String("/");
    }
    return cookie->SetPath(value);
}

ECode BasicPathHandler::Validate(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin)
{
    Boolean result;
    if (Match(cookie, origin, &result), !result) {
        String cookieP, originP;
        cookie->GetPath(&cookieP);
        origin->GetPath(&originP);
        Logger::E("BasicPathHandler", "Illegal path attribute \"%s\". Path of origin: \"%s\"", cookieP.string(), originP.string());
        return E_MALFORMED_COOKIE_EXCEPTION;
    }
    return NOERROR;
}

ECode BasicPathHandler::Match(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (cookie == NULL) {
        Logger::E("BasicPathHandler", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (origin == NULL) {
        Logger::E("BasicPathHandler", "Cookie origin may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String targetpath, topmostPath;
    origin->GetPath(&targetpath);
    cookie->GetPath(&topmostPath);
    if (topmostPath.IsNull()) {
        topmostPath = String("/");
    }
    if (topmostPath.GetLength() > 1 && topmostPath.EndWith("/")) {
        topmostPath = topmostPath.Substring(0, topmostPath.GetLength() - 1);
    }
    Boolean match = targetpath.StartWith(topmostPath);
    // if there is a match and these values are not exactly the same we have
    // to make sure we're not matcing "/foobar" and "/foo"
    if (match && targetpath.GetLength() != topmostPath.GetLength()) {
        if (!topmostPath.EndWith("/")) {
            match = (targetpath.GetChar(topmostPath.GetLength()) == '/');
        }
    }
    *result = match;
    return NOERROR;
}

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org