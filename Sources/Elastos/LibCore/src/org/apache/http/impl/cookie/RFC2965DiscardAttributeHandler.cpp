
#include "org/apache/http/impl/cookie/RFC2965DiscardAttributeHandler.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Cookie::EIID_ICookieAttributeHandler;
using Org::Apache::Http::Cookie::ISetCookie2;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

CAR_INTERFACE_IMPL(RFC2965DiscardAttributeHandler, Object, ICookieAttributeHandler)

ECode RFC2965DiscardAttributeHandler::Parse(
    /* [in] */ ISetCookie* cookie,
    /* [in] */ const String& value)
{
    AutoPtr<ISetCookie2> cookie2 = ISetCookie2::Probe(cookie);
    if (cookie2 != NULL) {
        cookie2->SetDiscard(TRUE);
    }
    return NOERROR;
}

ECode RFC2965DiscardAttributeHandler::Validate(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin)
{

    return NOERROR;
}

ECode RFC2965DiscardAttributeHandler::Match(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org