
#ifndef __ORG_APACHE_HTTP_IMPL_CLIENT_ROUTEDREQUEST_H__
#define __ORG_APACHE_HTTP_IMPL_CLIENT_ROUTEDREQUEST_H__

#include "org/apache/http/impl/client/RequestWrapper.h"

using Org::Apache::Http::Conn::Routing::IHttpRoute;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

/**
 * A request with the route along which it should be sent.
 *
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 *
 *
 * <!-- empty lines to avoid svn diff problems -->
 * @version $Revision: 645846 $
 *
 * @since 4.0
 */
class RoutedRequest : public Object
{
public:
    /**
     * Creates a new routed request.
     *
     * @param req   the request
     * @param route   the route
     */
    RoutedRequest(
        /* [in] */ RequestWrapper* request,
        /* [in] */ IHttpRoute* route);

    CARAPI_(AutoPtr<RequestWrapper>) GetRequest();

    CARAPI_(AutoPtr<IHttpRoute>) GetRoute();

protected:
    AutoPtr<RequestWrapper> mRequest;
    AutoPtr<IHttpRoute> mRoute;
};

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CLIENT_ROUTEDREQUEST_H__
