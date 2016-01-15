
#ifndef __ORG_APACHE_HTTP_IMPL_COOKIE_RFC2109VERSIONHANDLER_H__
#define __ORG_APACHE_HTTP_IMPL_COOKIE_RFC2109VERSIONHANDLER_H__

#include "org/apache/http/impl/cookie/AbstractCookieAttributeHandler.h"

using Org::Apache::Http::Cookie::ISetCookie;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

class RFC2109VersionHandler : public AbstractCookieAttributeHandler
{
public:
    RFC2109VersionHandler();

    CARAPI Parse(
        /* [in] */ ISetCookie* cookie,
        /* [in] */ const String& value);

    CARAPI Validate(
        /* [in] */ ICookie* cookie,
        /* [in] */ ICookieOrigin* origin);
};

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_COOKIE_RFC2109VERSIONHANDLER_H__
