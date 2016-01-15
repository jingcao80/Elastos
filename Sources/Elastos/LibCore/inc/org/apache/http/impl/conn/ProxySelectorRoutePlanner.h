
#ifndef __ORG_APACHE_HTTP_IMPL_CONN_PROXYSELECTORROUTEPLANNER_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_PROXYSELECTORROUTEPLANNER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Net::IInetSocketAddress;
using Elastos::Net::IProxySelector;
using Elastos::Utility::IList;
using Org::Apache::Http::IHttpHost;
using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::Conn::Routing::IHttpRoute;
using Org::Apache::Http::Conn::Routing::IHttpRoutePlanner;
using Org::Apache::Http::Conn::Scheme::ISchemeRegistry;
using Org::Apache::Http::Protocol::IHttpContext;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

/**
 * Default implementation of an {@link HttpRoutePlanner}.
 * This implementation is based on {@link java.net.ProxySelector}.
 * By default, it will pick up the proxy settings of the JVM, either
 * from system properties or from the browser running the application.
 * Additionally, it interprets some
 * {@link org.apache.http.conn.params.ConnRoutePNames parameters},
 * though not the {@link
 * org.apache.http.conn.params.ConnRoutePNames#DEFAULT_PROXY DEFAULT_PROXY}.
 */
class ProxySelectorRoutePlanner
    : public Object
    , public IHttpRoutePlanner
{
public:
    ProxySelectorRoutePlanner(
        /* [in] */ ISchemeRegistry* schreg,
        /* [in] */ IProxySelector* prosel);

    CAR_INTERFACE_DECL()

    /**
     * Obtains the proxy selector to use.
     *
     * @return the proxy selector, or <code>null</code> for the system default
     */
    CARAPI_(AutoPtr<IProxySelector>) GetProxySelector();

    /**
     * Sets the proxy selector to use.
     *
     * @param prosel    the proxy selector, or
     *                  <code>null</code> to use the system default
     */
    CARAPI_(void) SetProxySelector(
        /* [in] */ IProxySelector* prosel);

    CARAPI DetermineRoute(
        /* [in] */ IHttpHost* target,
        /* [in] */ IHttpRequest* request,
        /* [in] */ IHttpContext* context,
        /* [out] */ IHttpRoute** route);

protected:
    /**
     * Determines a proxy for the given target.
     *
     * @param target    the planned target, never <code>null</code>
     * @param request   the request to be sent, never <code>null</code>
     * @param context   the context, or <code>null</code>
     *
     * @return  the proxy to use, or <code>null</code> for a direct route
     *
     * @throws HttpException
     *         in case of system proxy settings that cannot be handled
     */
    CARAPI DetermineProxy(
        /* [in] */ IHttpHost* target,
        /* [in] */ IHttpRequest* request,
        /* [in] */ IHttpContext* context,
        /* [out] */ IHttpHost** proxy);

    /**
     * Obtains a host from an {@link InetSocketAddress}.
     *
     * @param isa       the socket address
     *
     * @return  a host string, either as a symbolic name or
     *          as a literal IP address string
     * <br/>
     * (TODO: determine format for IPv6 addresses, with or without [brackets])
     */
    CARAPI GetHost(
        /* [in] */ IInetSocketAddress* isa,
        /* [out] */ String* host);

    /*
     * Chooses a proxy from a list of available proxies.
     * The default implementation just picks the first non-SOCKS proxy
     * from the list. If there are only SOCKS proxies,
     * {@link Proxy#NO_PROXY Proxy.NO_PROXY} is returned.
     * Derived classes may implement more advanced strategies,
     * such as proxy rotation if there are multiple options.
     *
     * @param proxies   the list of proxies to choose from,
     *                  never <code>null</code> or empty
     * @param target    the planned target, never <code>null</code>
     * @param request   the request to be sent, never <code>null</code>
     * @param context   the context, or <code>null</code>
     *
     * @return  a proxy of type {@link Proxy.Type#DIRECT DIRECT}
     *          or {@link Proxy.Type#HTTP HTTP}, never <code>null</code>
     */
    CARAPI ChooseProxy(
        /* [in] */ IList* proxies,
        /* [in] */ IHttpHost* target,
        /* [in] */ IHttpRequest* request,
        /* [in] */ IHttpContext* context,
        /* [out] */ Elastos::Net::IProxy** proxy);

private:
    /** The scheme registry. */
    AutoPtr<ISchemeRegistry> mSchemeRegistry;

    /** The proxy selector to use, or <code>null</code> for system default. */
    AutoPtr<IProxySelector> mProxySelector;
};

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_PROXYSELECTORROUTEPLANNER_H__
