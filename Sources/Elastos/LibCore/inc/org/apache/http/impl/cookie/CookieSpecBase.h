
#ifndef __ORG_APACHE_HTTP_IMPL_COOKIE_COOKIESPECBASE_H__
#define __ORG_APACHE_HTTP_IMPL_COOKIE_COOKIESPECBASE_H__

#include "org/apache/http/impl/cookie/AbstractCookieSpec.h"

using Elastos::Utility::IList;
using Org::Apache::Http::Cookie::ICookieSpec;
using Org::Apache::Http::Cookie::ICookie;
using Org::Apache::Http::Cookie::ICookieOrigin;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

/**
 * Cookie management functions shared by all specification.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @since 4.0
 */
class CookieSpecBase : public AbstractCookieSpec
{
public:
    CARAPI Validate(
        /* [in] */ ICookie* cookie,
        /* [in] */ ICookieOrigin* origin);

    CARAPI Match(
        /* [in] */ ICookie* cookie,
        /* [in] */ ICookieOrigin* origin,
        /* [out] */ Boolean* result);

protected:
    static CARAPI_(String) GetDefaultPath(
        /* [in] */ ICookieOrigin* origin);

    static CARAPI_(String) GetDefaultDomain(
        /* [in] */ ICookieOrigin* origin);

    CARAPI Parse(
        /* [in] */ ArrayOf<IHeaderElement*>* elems,
        /* [in] */ ICookieOrigin* origin,
        /* [out] */ IList** cookies);
};

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_COOKIE_COOKIESPECBASE_H__
