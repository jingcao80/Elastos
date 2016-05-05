
#include "org/apache/http/conn/params/ConnRouteParams.h"
#include "org/apache/http/conn/routing/CHttpRoute.h"
#include "org/apache/http/CHttpHost.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::CHttpHost;
using Org::Apache::Http::Conn::Routing::CHttpRoute;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Params {

static AutoPtr<IHttpHost> InitHttpHost()
{
    AutoPtr<IHttpHost> host;
    CHttpHost::New(String("127.0.0.255"), 0, String("no-host"), (IHttpHost**)&host);
    return host;
}
const AutoPtr<IHttpHost> ConnRouteParams::NO_HOST = InitHttpHost();

static AutoPtr<IHttpRoute> InitHttpRoute()
{
    AutoPtr<IHttpRoute> route;
    CHttpRoute::New(ConnRouteParams::NO_HOST, (IHttpRoute**)&route);
    return route;
}
const AutoPtr<IHttpRoute> ConnRouteParams::NO_ROUTE = InitHttpRoute();

CAR_INTERFACE_IMPL(ConnRouteParams, Object, IConnRoutePNames)

ECode ConnRouteParams::GetDefaultProxy(
    /* [in] */ IHttpParams* params,
    /* [out] */ IHttpHost** proxy)
{
    VALIDATE_NOT_NULL(proxy)
    *proxy = NULL;

    if (params == NULL) {
        Logger::E("ConnRouteParams", "Parameters must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInterface> o;
    params->GetParameter(DEFAULT_PROXY, (IInterface**)&o);
    AutoPtr<IHttpHost> host = IHttpHost::Probe(o);
    Boolean result;
    if ((host != NULL) && (IObject::Probe(NO_HOST)->Equals(host, &result), result)) {
        // value is explicitly unset
        host = NULL;
    }
    *proxy = host;
    REFCOUNT_ADD(*proxy)
    return NOERROR;
}

ECode ConnRouteParams::SetDefaultProxy(
    /* [in] */ IHttpParams* params,
    /* [in] */ IHttpHost* proxy)
{
    if (params == NULL) {
        Logger::E("ConnRouteParams", "Parameters must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpParams> p;
    return params->SetParameter(DEFAULT_PROXY, proxy, (IHttpParams**)&p);
}

ECode ConnRouteParams::GetForcedRoute(
    /* [in] */ IHttpParams* params,
    /* [out] */ IHttpRoute** route)
{
    VALIDATE_NOT_NULL(route)
    *route = NULL;

    if (params == NULL) {
        Logger::E("ConnRouteParams", "Parameters must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInterface> o;
    params->GetParameter(FORCED_ROUTE, (IInterface**)&o);
    AutoPtr<IHttpRoute> httpRoute = IHttpRoute::Probe(o);
    Boolean result;
    if ((httpRoute != NULL) && (IObject::Probe(NO_ROUTE)->Equals(httpRoute, &result), result)) {
        // value is explicitly unset
        httpRoute = NULL;
    }
    *route = httpRoute;
    REFCOUNT_ADD(*route)
    return NOERROR;
}

ECode ConnRouteParams::SetForcedRoute(
    /* [in] */ IHttpParams* params,
    /* [in] */ IHttpRoute* route)
{
    if (params == NULL) {
        Logger::E("ConnRouteParams", "Parameters must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpParams> p;
    return params->SetParameter(FORCED_ROUTE, route, (IHttpParams**)&p);
}

ECode ConnRouteParams::GetLocalAddress(
    /* [in] */ IHttpParams* params,
    /* [out] */ IInetAddress** local)
{
    VALIDATE_NOT_NULL(local)
    *local = NULL;

    if (params == NULL) {
        Logger::E("ConnRouteParams", "Parameters must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInterface> o;
    params->GetParameter(LOCAL_ADDRESS, (IInterface**)&o);
    *local = IInetAddress::Probe(o);
    REFCOUNT_ADD(*local)
    return NOERROR;
}

ECode ConnRouteParams::SetLocalAddress(
    /* [in] */ IHttpParams* params,
    /* [in] */ IInetAddress* local)
{
    if (params == NULL) {
        Logger::E("ConnRouteParams", "Parameters must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHttpParams> p;
    return params->SetParameter(LOCAL_ADDRESS, local, (IHttpParams**)&p);
}

} // namespace Params
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org