
#ifndef __ORG_APACHE_HTTP_IMPL_AUTH_BASICSCHEMEFACTORY_H__
#define __ORG_APACHE_HTTP_IMPL_AUTH_BASICSCHEMEFACTORY_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::Auth::IAuthScheme;
using Org::Apache::Http::Auth::IAuthSchemeFactory;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Auth {

/**
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @since 4.0
 */
class BasicSchemeFactory
    : public Object
    , public IAuthSchemeFactory
{
public:
    CAR_INTERFACE_DECL()

    CARAPI NewInstance(
        /* [in] */ IHttpParams* params,
        /* [out] */ IAuthScheme** scheme);
};

} // namespace Auth
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_AUTH_BASICSCHEMEFACTORY_H__
