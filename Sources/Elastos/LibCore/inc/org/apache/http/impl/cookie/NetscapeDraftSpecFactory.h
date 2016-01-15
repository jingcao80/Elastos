
#ifndef __ORG_APACHE_HTTP_IMPL_COOKIE_NETSCAPEDRAFTSPECFACTORY_H__
#define __ORG_APACHE_HTTP_IMPL_COOKIE_NETSCAPEDRAFTSPECFACTORY_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::Cookie::ICookieSpec;
using Org::Apache::Http::Cookie::ICookieSpecFactory;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

/**
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @since 4.0
 */
class NetscapeDraftSpecFactory
    : public Object
    , public ICookieSpecFactory
{
public:
    CAR_INTERFACE_DECL()

    CARAPI NewInstance(
        /* [in] */ IHttpParams* params,
        /* [out] */ ICookieSpec** spec);
};

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_COOKIE_NETSCAPEDRAFTSPECFACTORY_H__
