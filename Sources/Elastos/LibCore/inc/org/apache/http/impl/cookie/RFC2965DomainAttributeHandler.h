
#ifndef __ORG_APACHE_HTTP_IMPL_COOKIE_RFC2965DOMAINATTRIBUTEHANDLER_H__
#define __ORG_APACHE_HTTP_IMPL_COOKIE_RFC2965DOMAINATTRIBUTEHANDLER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::Cookie::ICookie;
using Org::Apache::Http::Cookie::ICookieAttributeHandler;
using Org::Apache::Http::Cookie::ICookieOrigin;
using Org::Apache::Http::Cookie::ISetCookie;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

class RFC2965DomainAttributeHandler
    : public Object
    , public ICookieAttributeHandler
{
public:
    CAR_INTERFACE_DECL()

    CARAPI Parse(
        /* [in] */ ISetCookie* cookie,
        /* [in] */ const String& domain);

    /**
     * Performs domain-match as defined by the RFC2965.
     * <p>
     * Host A's name domain-matches host B's if
     * <ol>
     *   <ul>their host name strings string-compare equal; or</ul>
     *   <ul>A is a HDN string and has the form NB, where N is a non-empty
     *       name string, B has the form .B', and B' is a HDN string.  (So,
     *       x.y.com domain-matches .Y.com but not Y.com.)</ul>
     * </ol>
     *
     * @param host host name where cookie is received from or being sent to.
     * @param domain The cookie domain attribute.
     * @return true if the specified host matches the given domain.
     */
    CARAPI_(Boolean) DomainMatch(
        /* [in] */ const String& host,
        /* [in] */ const String& domain);

    CARAPI Validate(
        /* [in] */ ICookie* cookie,
        /* [in] */ ICookieOrigin* origin);

    CARAPI Match(
        /* [in] */ ICookie* cookie,
        /* [in] */ ICookieOrigin* origin,
        /* [out] */ Boolean* result);
};

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_COOKIE_RFC2965DOMAINATTRIBUTEHANDLER_H__
