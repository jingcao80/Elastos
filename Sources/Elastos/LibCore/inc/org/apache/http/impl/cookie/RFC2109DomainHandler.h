
#ifndef __ORG_APACHE_HTTP_IMPL_COOKIE_RFC2109DOMAINHANDLER_H__
#define __ORG_APACHE_HTTP_IMPL_COOKIE_RFC2109DOMAINHANDLER_H__

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

class RFC2109DomainHandler
    : public Object
    , public ICookieAttributeHandler
{
public:
    CAR_INTERFACE_DECL()

    CARAPI Parse(
        /* [in] */ ISetCookie* cookie,
        /* [in] */ const String& value);

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

#endif // __ORG_APACHE_HTTP_IMPL_COOKIE_RFC2109DOMAINHANDLER_H__
