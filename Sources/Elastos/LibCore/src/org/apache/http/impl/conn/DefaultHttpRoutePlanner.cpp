
#include "org/apache/http/impl/conn/DefaultHttpRoutePlanner.h"
#include "org/apache/http/conn/params/ConnRouteParams.h"
#include "org/apache/http/conn/routing/CHttpRoute.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Net::IInetAddress;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::Conn::Scheme::IScheme;
using Org::Apache::Http::Conn::Params::ConnRouteParams;
using Org::Apache::Http::Conn::Routing::CHttpRoute;
using Org::Apache::Http::Conn::Routing::EIID_IHttpRoutePlanner;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

DefaultHttpRoutePlanner::DefaultHttpRoutePlanner(
    /* [in] */ ISchemeRegistry* schreg)
{
    if (schreg == NULL) {
        Logger::E("DefaultHttpRoutePlanner", "Scheme registry must not be null.");
        assert(0);
        // return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mSchemeRegistry = schreg;
}

CAR_INTERFACE_IMPL(DefaultHttpRoutePlanner, Object, IHttpRoutePlanner)

ECode DefaultHttpRoutePlanner::DetermineRoute(
    /* [in] */ IHttpHost* target,
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpContext* context,
    /* [out] */ IHttpRoute** httpRoute)
{
    VALIDATE_NOT_NULL(httpRoute)
    *httpRoute = NULL;

    if (request == NULL) {
        Logger::E("DefaultClientConnection", "Request must not be null.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // If we have a forced route, we can do without a target.
    AutoPtr<IHttpParams> params;
    IHttpMessage::Probe(request)->GetParams((IHttpParams**)&params);
    AutoPtr<IHttpRoute> route;
    ConnRouteParams::GetForcedRoute(params, (IHttpRoute**)&route);
    if (route != NULL) {
        *httpRoute = route;
        REFCOUNT_ADD(*httpRoute)
        return NOERROR;
    }

    // If we get here, there is no forced route.
    // So we need a target to compute a route.

    if (target == NULL) {
        Logger::E("DefaultClientConnection", "Target host must not be null.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IInetAddress> local;
    ConnRouteParams::GetLocalAddress(params, (IInetAddress**)&local);
    AutoPtr<IHttpHost> proxy;
    ConnRouteParams::GetDefaultProxy(params, (IHttpHost**)&proxy);

    String name;
    target->GetSchemeName(&name);
    AutoPtr<IScheme> schm;
    mSchemeRegistry->GetScheme(name, (IScheme**)&schm);
    // as it is typically used for TLS/SSL, we assume that
    // a layered scheme implies a secure connection
    Boolean secure;
    schm->IsLayered(&secure);

    if (proxy == NULL) {
        return CHttpRoute::New(target, local, secure, httpRoute);
    }
    else {
        return CHttpRoute::New(target, local, proxy, secure, httpRoute);
    }
}

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org