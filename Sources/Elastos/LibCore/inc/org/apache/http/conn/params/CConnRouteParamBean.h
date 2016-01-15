
#ifndef __ORG_APACHE_HTTP_CONN_PARAMS_CConnRouteParamBean_H_
#define __ORG_APACHE_HTTP_CONN_PARAMS_CConnRouteParamBean_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "_Org_Apache_Http_Conn_Params_CConnRouteParamBean.h"
#include "org/apache/http/params/HttpAbstractParamBean.h"
#include "elastos/core/Object.h"

using Elastos::Net::IInetAddress;
using Org::Apache::Http::Conn::Routing::IHttpRoute;
using Org::Apache::Http::Params::HttpAbstractParamBean;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Params {

/**
 * Allows for setting parameters relating to connection routes on
 * {@link HttpParams}.  This class ensures that the values set on the params
 * are type-safe.
 */
CarClass(CConnRouteParamBean)
    , public IConnRouteParamBean
    , public HttpAbstractParamBean
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /** @see ConnRoutePNames#DEFAULT_PROXY */
    CARAPI SetDefaultProxy(
        /* [in] */ IHttpHost* defaultProxy);

    /** @see ConnRoutePNames#LOCAL_ADDRESS */
    CARAPI SetLocalAddress(
        /* [in] */ IInetAddress* address);

    /** @see ConnRoutePNames#FORCED_ROUTE */
    CARAPI SetForcedRoute(
        /* [in] */ IHttpRoute* route);

    CARAPI constructor(
        /* [in] */ IHttpParams* params);

};

} // namespace Params
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CONN_PARAMS_CConnRouteParamBean_H_
