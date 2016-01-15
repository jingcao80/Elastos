
#ifndef __ORG_APACHE_HTTP_IMPL_COOKIE_NETSCAPEDOMAINHANDLER_H__
#define __ORG_APACHE_HTTP_IMPL_COOKIE_NETSCAPEDOMAINHANDLER_H__

#include "org/apache/http/impl/cookie/BasicDomainHandler.h"

using Org::Apache::Http::Cookie::ICookie;
using Org::Apache::Http::Cookie::ICookieOrigin;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

class NetscapeDomainHandler : public BasicDomainHandler
{
public:
    NetscapeDomainHandler();

    CARAPI Validate(
        /* [in] */ ICookie* cookie,
        /* [in] */ ICookieOrigin* origin);

    CARAPI Match(
        /* [in] */ ICookie* header,
        /* [in] */ ICookieOrigin* origin,
        /* [out] */ Boolean* result);

private:
    /**
    * Checks if the given domain is in one of the seven special
    * top level domains defined by the Netscape cookie specification.
    * @param domain The domain.
    * @return True if the specified domain is "special"
    */
   static CARAPI_(Boolean) IsSpecialDomain(
        /* [in] */ const String& domain);
};

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_COOKIE_NETSCAPEDOMAINHANDLER_H__
