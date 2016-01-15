
#include "org/apache/http/conn/params/CConnRouteParams.h"
#include "org/apache/http/conn/params/ConnRouteParams.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Params {

CAR_INTERFACE_IMPL_2(CConnRouteParams, Singleton, IConnRouteParams, IConnRoutePNames)

CAR_SINGLETON_IMPL(CConnRouteParams)

ECode CConnRouteParams::GetDefaultProxy(
    /* [in] */ IHttpParams* params,
    /* [out] */ IHttpHost** proxy)
{
    return ConnRouteParams::GetDefaultProxy(params, proxy);
}

ECode CConnRouteParams::SetDefaultProxy(
    /* [in] */ IHttpParams* params,
    /* [in] */ IHttpHost* proxy)
{
    return ConnRouteParams::SetDefaultProxy(params, proxy);
}

ECode CConnRouteParams::GetForcedRoute(
    /* [in] */ IHttpParams* params,
    /* [out] */ IHttpRoute** route)
{
    return ConnRouteParams::GetForcedRoute(params, route);
}

ECode CConnRouteParams::SetForcedRoute(
    /* [in] */ IHttpParams* params,
    /* [in] */ IHttpRoute* route)
{
    return ConnRouteParams::SetForcedRoute(params, route);
}

ECode CConnRouteParams::GetLocalAddress(
    /* [in] */ IHttpParams* params,
    /* [out] */ IInetAddress** local)
{
    return ConnRouteParams::GetLocalAddress(params, local);
}

ECode CConnRouteParams::SetLocalAddress(
    /* [in] */ IHttpParams* params,
    /* [in] */ IInetAddress* local)
{
    return ConnRouteParams::SetLocalAddress(params, local);
}

ECode CConnRouteParams::GetNoHost(
    /* [out] */ IHttpHost** host)
{
    VALIDATE_NOT_NULL(host)
    *host = ConnRouteParams::NO_HOST;
    REFCOUNT_ADD(*host)
    return NOERROR;
}

ECode CConnRouteParams::GetNoRoute(
    /* [out] */ IHttpRoute** route)
{
    VALIDATE_NOT_NULL(route)
    *route = ConnRouteParams::NO_ROUTE;
    REFCOUNT_ADD(*route)
    return NOERROR;
}

} // namespace Params
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org