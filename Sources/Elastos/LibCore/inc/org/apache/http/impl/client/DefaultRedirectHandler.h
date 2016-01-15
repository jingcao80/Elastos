
#ifndef __ORG_APACHE_HTTP_IMPL_CLIENT_DEFAULTREDIRECTHANDLER_H__
#define __ORG_APACHE_HTTP_IMPL_CLIENT_DEFAULTREDIRECTHANDLER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Net::IURI;
using Org::Apache::Http::IHttpResponse;
using Org::Apache::Http::Client::IRedirectHandler;
using Org::Apache::Http::Protocol::IHttpContext;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

/**
 * Default implementation of {@link RedirectHandler}.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 *
 * <!-- empty lines to avoid svn diff problems -->
 * @version $Revision: 673450 $
 *
 * @since 4.0
 */
class DefaultRedirectHandler
    : public Object
    , public IRedirectHandler
{
public:
    DefaultRedirectHandler() {}

    CAR_INTERFACE_DECL()

    CARAPI IsRedirectRequested(
        /* [in] */ IHttpResponse* response,
        /* [in] */ IHttpContext* context,
        /* [out] */ Boolean* redirected);

    CARAPI GetLocationURI(
        /* [in] */ IHttpResponse* response,
        /* [in] */ IHttpContext* context,
        /* [out] */ IURI** uri);

private:
    static const String REDIRECT_LOCATIONS;
};

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CLIENT_DEFAULTREDIRECTHANDLER_H__
