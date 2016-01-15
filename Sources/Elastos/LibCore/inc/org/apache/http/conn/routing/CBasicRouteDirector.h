
#ifndef __ORG_APACHE_HTTP_CONN_ROUTING_CBASICROUTEDIRECTOR_H_
#define __ORG_APACHE_HTTP_CONN_ROUTING_CBASICROUTEDIRECTOR_H_

#include "_Org_Apache_Http_Conn_Routing_CBasicRouteDirector.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Routing {

/**
 * Basic implementation of an {@link HttpRouteDirector HttpRouteDirector}.
 * This implementation is stateless and therefore thread-safe.
 *
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 *
 *
 * <!-- empty lines to avoid svn diff problems -->
 * @version $Revision: 620255 $
 *
 * @since 4.0
 */
CarClass(CBasicRouteDirector)
    , public Object
    , public IBasicRouteDirector
    , public IHttpRouteDirector
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Provides the next step.
     *
     * @param plan      the planned route
     * @param fact      the currently established route, or
     *                  <code>null</code> if nothing is established
     *
     * @return  one of the constants defined in this interface, indicating
     *          either the next step to perform, or success, or failure.
     *          0 is for success, a negative value for failure.
     */
    CARAPI NextStep(
        /* [in] */ IRouteInfo* plan,
        /* [in] */ IRouteInfo* fact,
        /* [out] */ Int32* step);

    /**
     * Determines the first step to establish a route.
     *
     * @param plan      the planned route
     *
     * @return  the first step
     */
    CARAPI FirstStep(
        /* [in] */ IRouteInfo* plan,
        /* [out] */ Int32* step);

    /**
     * Determines the next step to establish a direct connection.
     *
     * @param plan      the planned route
     * @param fact      the currently established route
     *
     * @return  one of the constants defined in this class, indicating
     *          either the next step to perform, or success, or failure
     */
    CARAPI DirectStep(
        /* [in] */ IRouteInfo* plan,
        /* [in] */ IRouteInfo* fact,
        /* [out] */ Int32* step);

    /**
     * Determines the next step to establish a connection via proxy.
     *
     * @param plan      the planned route
     * @param fact      the currently established route
     *
     * @return  one of the constants defined in this class, indicating
     *          either the next step to perform, or success, or failure
     */
    CARAPI ProxiedStep(
        /* [in] */ IRouteInfo* plan,
        /* [in] */ IRouteInfo* fact,
        /* [out] */ Int32* step);

};

} // namespace Routing
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CONN_ROUTING_CBASICROUTEDIRECTOR_H_
