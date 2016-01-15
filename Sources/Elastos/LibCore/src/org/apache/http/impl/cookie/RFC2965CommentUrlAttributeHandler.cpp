
#include "org/apache/http/impl/cookie/RFC2965CommentUrlAttributeHandler.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Cookie::EIID_ICookieAttributeHandler;
using Org::Apache::Http::Cookie::ISetCookie2;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

CAR_INTERFACE_IMPL(RFC2965CommentUrlAttributeHandler, Object, ICookieAttributeHandler)

ECode RFC2965CommentUrlAttributeHandler::Parse(
    /* [in] */ ISetCookie* cookie,
    /* [in] */ const String& value)
{
    AutoPtr<ISetCookie2> cookie2 = ISetCookie2::Probe(cookie);
    if (cookie2 != NULL) {
        cookie2->SetCommentURL(value);
    }
    return NOERROR;
}

ECode RFC2965CommentUrlAttributeHandler::Validate(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin)
{
    return NOERROR;
}

ECode RFC2965CommentUrlAttributeHandler::Match(
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