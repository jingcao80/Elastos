
#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/http/impl/conn/ProxySelectorRoutePlanner.h"
#include "org/apache/http/conn/routing/CHttpRoute.h"
#include "org/apache/http/conn/params/ConnRouteParams.h"
#include "org/apache/http/CHttpHost.h"
#include "elastos/net/CURI.h"
#include "elastos/net/ProxySelector.h"
#include "elastos/net/CProxyHelper.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Net::IInetAddress;
using Elastos::Net::ProxySelector;
using Elastos::Net::IURI;
using Elastos::Net::CURI;
using Elastos::Net::ProxyType;
using Elastos::Net::ProxyType_HTTP;
using Elastos::Net::ProxyType_DIRECT;
using Elastos::Net::ProxyType_SOCKS;
using Elastos::Net::IProxyHelper;
using Elastos::Net::CProxyHelper;
using Elastos::Net::ISocketAddress;
using Elastos::Utility::IList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::CHttpHost;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::Conn::Params::ConnRouteParams;
using Org::Apache::Http::Conn::Params::IConnRoutePNames;
using Org::Apache::Http::Conn::Routing::EIID_IHttpRoutePlanner;
using Org::Apache::Http::Conn::Routing::CHttpRoute;
using Org::Apache::Http::Conn::Scheme::IScheme;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

ProxySelectorRoutePlanner::ProxySelectorRoutePlanner(
    /* [in] */ ISchemeRegistry* schreg,
    /* [in] */ IProxySelector* prosel)
{
    if (schreg == NULL) {
        Logger::E("ProxySelectorRoutePlanner", "SchemeRegistry must not be null.");
        assert(0);
        // throw new IllegalArgumentException
        //     ("SchemeRegistry must not be null.");
    }
    mSchemeRegistry = schreg;
    mProxySelector  = prosel;
}

CAR_INTERFACE_IMPL(ProxySelectorRoutePlanner, Object, IHttpRoutePlanner)

AutoPtr<IProxySelector> ProxySelectorRoutePlanner::GetProxySelector()
{
    return mProxySelector;
}

void ProxySelectorRoutePlanner::SetProxySelector(
    /* [in] */ IProxySelector* prosel)
{
    mProxySelector = prosel;
}

ECode ProxySelectorRoutePlanner::DetermineRoute(
    /* [in] */ IHttpHost* target,
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpContext* context,
    /* [out] */ IHttpRoute** _route)
{
    VALIDATE_NOT_NULL(_route)
    *_route = NULL;

    if (request == NULL) {
        Logger::E("ProxySelectorRoutePlanner", "Request must not be null.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // If we have a forced route, we can do without a target.
    AutoPtr<IHttpParams> params;
    IHttpMessage::Probe(request)->GetParams((IHttpParams**)&params);
    AutoPtr<IHttpRoute> route;
    ConnRouteParams::GetForcedRoute(params, (IHttpRoute**)&route);
    if (route != NULL) {
        *_route = route;
        REFCOUNT_ADD(*_route)
        return NOERROR;
    }

    // If we get here, there is no forced route.
    // So we need a target to compute a route.

    if (target == NULL) {
        Logger::E("ProxySelectorRoutePlanner", "Target host must not be null.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IInetAddress> local;
    ConnRouteParams::GetLocalAddress(params, (IInetAddress**)&local);

    // BEGIN android-changed
    //     If the client or request explicitly specifies a proxy (or no
    //     proxy), prefer that over the ProxySelector's VM-wide default.
    AutoPtr<IInterface> param;
    params->GetParameter(IConnRoutePNames::DEFAULT_PROXY, (IInterface**)&param);
    AutoPtr<IHttpHost> proxy;
    Boolean isEquals;
    if (param == NULL) {
        DetermineProxy(target, request, context, (IHttpHost**)&proxy);
    }
    else if (IObject::Probe(ConnRouteParams::NO_HOST)->Equals(param, &isEquals), isEquals) {
        // value is explicitly unset
        proxy = NULL;
    }
    else {
        proxy = IHttpHost::Probe(param);
    }
    // END android-changed

    String name;
    target->GetSchemeName(&name);
    AutoPtr<IScheme> schm;
    mSchemeRegistry->GetScheme(name, (IScheme**)&schm);
    // as it is typically used for TLS/SSL, we assume that
    // a layered scheme implies a secure connection
    Boolean secure;
    schm->IsLayered(&secure);

    if (proxy == NULL) {
        CHttpRoute::New(target, local, secure, (IHttpRoute**)&route);
    }
    else {
        CHttpRoute::New(target, local, proxy, secure, (IHttpRoute**)&route);
    }
    *_route = route;
    REFCOUNT_ADD(*_route)
    return NOERROR;
}

ECode ProxySelectorRoutePlanner::DetermineProxy(
    /* [in] */ IHttpHost* target,
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpContext* context,
    /* [out] */ IHttpHost** proxy)
{
    VALIDATE_NOT_NULL(proxy)
    *proxy = NULL;

    // the proxy selector can be 'unset', so we better deal with null here
    AutoPtr<IProxySelector> psel = mProxySelector;
    if (psel == NULL) {
        ProxySelector::GetDefault((IProxySelector**)&psel);
    }
    if (psel == NULL) {
        return NOERROR;
    }

    AutoPtr<IURI> targetURI;
    // try {
    String uri;
    target->ToURI(&uri);
    if (FAILED(CURI::New(uri, (IURI**)&targetURI))) {
        Logger::E("ProxySelectorRoutePlanner", "Cannot convert host to URI: %p", target);
        return E_HTTP_EXCEPTION;
    }
    // } catch (URISyntaxException usx) {
    //     throw new HttpException
    //         ("Cannot convert host to URI: " + target, usx);
    // }
    AutoPtr<IList> proxies;
    psel->Select(targetURI, (IList**)&proxies);

    AutoPtr<Elastos::Net::IProxy> p;
    ChooseProxy(proxies, target, request, context, (Elastos::Net::IProxy**)&p);

    AutoPtr<IHttpHost> result;
    ProxyType type;
    if (p->GetType(&type), type == ProxyType_HTTP) {
        // convert the socket address to an HttpHost
        AutoPtr<ISocketAddress> addr;
        p->GetAddress((ISocketAddress**)&addr);
        AutoPtr<IInetSocketAddress> isa = IInetSocketAddress::Probe(addr);
        if (isa == NULL) {
            Logger::E("ProxySelectorRoutePlanner", "Unable to handle non-Inet proxy address: %p", addr.Get());
            return E_HTTP_EXCEPTION;
        }
        // assume default scheme (http)
        String host;
        GetHost(isa, &host);
        Int32 port;
        isa->GetPort(&port);
        CHttpHost::New(host, port, (IHttpHost**)&result);
    }

    *proxy = result;
    REFCOUNT_ADD(*proxy)
    return NOERROR;
}

ECode ProxySelectorRoutePlanner::GetHost(
    /* [in] */ IInetSocketAddress* isa,
    /* [out] */ String* host)
{
    VALIDATE_NOT_NULL(host)
    //@@@ Will this work with literal IPv6 addresses, or do we
    //@@@ need to wrap these in [] for the string representation?
    //@@@ Having it in this method at least allows for easy workarounds.
    Boolean isUnresolved;
    isa->IsUnresolved(&isUnresolved);
    if (isUnresolved) {
        return isa->GetHostName(host);
    }
    else {
        AutoPtr<IInetAddress> addr;
        isa->GetAddress((IInetAddress**)&addr);
        return addr->GetHostAddress(host);
    }
}

ECode ProxySelectorRoutePlanner::ChooseProxy(
    /* [in] */ IList* proxies,
    /* [in] */ IHttpHost* target,
    /* [in] */ IHttpRequest* request,
    /* [in] */ IHttpContext* context,
    /* [out] */ Elastos::Net::IProxy** proxy)
{
    VALIDATE_NOT_NULL(proxy)
    *proxy = NULL;

    Boolean isEmpty;
    if ((proxies == NULL) || (proxies->IsEmpty(&isEmpty), isEmpty)) {
        Logger::E("ProxySelectorRoutePlanner", "Proxy list must not be empty.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<Elastos::Net::IProxy> result;

    // check the list for one we can use
    AutoPtr<IIterator> it;
    proxies->GetIterator((IIterator**)&it);
    Boolean hasNest;
    while (it->HasNext(&hasNest), hasNest) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<Elastos::Net::IProxy> p = Elastos::Net::IProxy::Probe(value);
        ProxyType type;
        p->GetType(&type);
        switch (type) {
            case ProxyType_DIRECT:
            case ProxyType_HTTP:
                result = p;
                break;

            case ProxyType_SOCKS:
                // SOCKS hosts are not handled on the route level.
                // The socket may make use of the SOCKS host though.
                break;
        }
    }

    if (result == NULL) {
        //@@@ log as warning or info that only a socks proxy is available?
        // result can only be null if all proxies are socks proxies
        // socks proxies are not handled on the route planning level
        AutoPtr<IProxyHelper> helper;
        CProxyHelper::AcquireSingleton((IProxyHelper**)&helper);
        helper->GetNO_PROXY((Elastos::Net::IProxy**)&result);
    }

    *proxy = result;
    REFCOUNT_ADD(*proxy)
    return NOERROR;
}

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org