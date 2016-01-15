
#ifndef __ORG_APACHE_HTTP_IMPL_CONN_ABSTRACTPOOLENTRY_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_ABSTRACTPOOLENTRY_H__

#include "elastos/core/Object.h"

using Org::Apache::Http::IHttpHost;
using Org::Apache::Http::Conn::IOperatedClientConnection;
using Org::Apache::Http::Conn::IClientConnectionOperator;
using Org::Apache::Http::Conn::Routing::IHttpRoute;
using Org::Apache::Http::Conn::Routing::IRouteTracker;
using Org::Apache::Http::Params::IHttpParams;
using Org::Apache::Http::Protocol::IHttpContext;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

class AbstractPooledConnAdapter;

/**
 * A pool entry for use by connection manager implementations.
 * Pool entries work in conjunction with an
 * {@link AbstractClientConnAdapter adapter}.
 * The adapter is handed out to applications that obtain a connection.
 * The pool entry stores the underlying connection and tracks the
 * {@link HttpRoute route} established.
 * The adapter delegates methods for establishing the route to
 * it's pool entry.
 * <br/>
 * If the managed connections is released or revoked, the adapter
 * gets disconnected, but the pool entry still contains the
 * underlying connection and the established route.
 *
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 * @author <a href="mailto:becke@u.washington.edu">Michael Becke</a>
 *
 *
 * <!-- empty lines to avoid svn diff problems -->
 * @version   $Revision: 658775 $
 *
 * @since 4.0
 */
class AbstractPoolEntry : public Object
{
public:
    /**
     * Returns the state object associated with this pool entry.
     *
     * @return The state object
     */
    CARAPI_(AutoPtr<IObject>) GetState();

    /**
     * Assigns a state object to this pool entry.
     *
     * @param state The state object
     */
    CARAPI_(void) SetState(
        /* [in] */ IObject* state);

    /**
     * Opens the underlying connection.
     *
     * @param route         the route along which to open the connection
     * @param context       the context for opening the connection
     * @param params        the parameters for opening the connection
     *
     * @throws IOException  in case of a problem
     */
    CARAPI Open(
        /* [in] */ IHttpRoute* route,
        /* [in] */ IHttpContext* context,
        /* [in] */ IHttpParams* params);

    /**
     * Tracks tunnelling of the connection to the target.
     * The tunnel has to be established outside by sending a CONNECT
     * request to the (last) proxy.
     *
     * @param secure    <code>true</code> if the tunnel should be
     *                  considered secure, <code>false</code> otherwise
     * @param params    the parameters for tunnelling the connection
     *
     * @throws IOException  in case of a problem
     */
    CARAPI TunnelTarget(
        /* [in] */ Boolean secure,
        /* [in] */ IHttpParams* params);

    /**
     * Tracks tunnelling of the connection to a chained proxy.
     * The tunnel has to be established outside by sending a CONNECT
     * request to the previous proxy.
     *
     * @param next      the proxy to which the tunnel was established.
     *  See {@link org.apache.http.conn.ManagedClientConnection#tunnelProxy
     *                                  ManagedClientConnection.tunnelProxy}
     *                  for details.
     * @param secure    <code>true</code> if the tunnel should be
     *                  considered secure, <code>false</code> otherwise
     * @param params    the parameters for tunnelling the connection
     *
     * @throws IOException  in case of a problem
     */
    CARAPI TunnelProxy(
        /* [in] */ IHttpHost* next,
        /* [in] */ Boolean secure,
        /* [in] */ IHttpParams* params);

    /**
     * Layers a protocol on top of an established tunnel.
     *
     * @param context   the context for layering
     * @param params    the parameters for layering
     *
     * @throws IOException  in case of a problem
     */
    CARAPI LayerProtocol(
        /* [in] */ IHttpContext* context,
        /* [in] */ IHttpParams* params);

protected:
    /**
     * Creates a new pool entry.
     *
     * @param connOperator     the Connection Operator for this entry
     * @param route   the planned route for the connection,
     *                or <code>null</code>
     */
    AbstractPoolEntry(
        /* [in] */ IClientConnectionOperator* connOperator,
        /* [in] */ IHttpRoute* route);

    /**
     * Shuts down the entry.
     *
     * If {@link #open(HttpRoute, HttpContext, HttpParams)} is in progress,
     * this will cause that open to possibly throw an {@link IOException}.
     */
    CARAPI_(void) ShutdownEntry();

protected:
    /** The connection operator. */
    AutoPtr<IClientConnectionOperator> mConnOperator;

    /** The underlying connection being pooled or used. */
    AutoPtr<IOperatedClientConnection> mConnection;

    /** The route for which this entry gets allocated. */
    //@@@ currently accessed from connection manager(s) as attribute
    //@@@ avoid that, derived classes should decide whether update is allowed
    //@@@ SCCM: yes, TSCCM: no
    AutoPtr<IHttpRoute> mRoute;

    /** Connection state object */
    AutoPtr<IObject> mState;

    /** The tracked route, or <code>null</code> before tracking starts. */
    AutoPtr<IRouteTracker> mTracker;

    friend class AbstractPooledConnAdapter;
};

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_ABSTRACTPOOLENTRY_H__
