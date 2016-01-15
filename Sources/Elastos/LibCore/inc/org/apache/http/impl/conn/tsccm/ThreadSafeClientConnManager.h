
#ifndef __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_THREADSAFECLIENTCONNMANAGER_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_THREADSAFECLIENTCONNMANAGER_H__

#include "org/apache/http/impl/conn/tsccm/AbstractConnPool.h"

using Elastos::Utility::Concurrent::ITimeUnit;
using Org::Apache::Http::Conn::IClientConnectionManager;
using Org::Apache::Http::Conn::IClientConnectionOperator;
using Org::Apache::Http::Conn::IClientConnectionRequest;
using Org::Apache::Http::Conn::IManagedClientConnection;
using Org::Apache::Http::Conn::Routing::IHttpRoute;
using Org::Apache::Http::Conn::Scheme::ISchemeRegistry;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {
namespace Tsccm {

/**
 * Manages a pool of {@link OperatedClientConnection client connections}.
 * <p>
 * This class is derived from <code>MultiThreadedHttpConnectionManager</code>
 * in HttpClient 3. See there for original authors.
 * </p>
 *
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 * @author <a href="mailto:becke@u.washington.edu">Michael Becke</a>
 *
 *
 * <!-- empty lines to avoid svn diff problems -->
 * @version $Revision: 673450 $ $Date: 2008-07-02 10:35:05 -0700 (Wed, 02 Jul 2008) $
 *
 * @since 4.0
 */
class ThreadSafeClientConnManager
    : public Object
    , public IClientConnectionManager
{
private:
    class MyClientConnectionRequest
        : public Object
        , public IClientConnectionRequest
    {
    public:
        MyClientConnectionRequest(
            /* [in] */ IHttpRoute* route,
            /* [in] */ IPoolEntryRequest* request,
            /* [in] */ ThreadSafeClientConnManager* host);

        CAR_INTERFACE_DECL()

        CARAPI AbortRequest();

        CARAPI GetConnection(
            /* [in] */ Int64 timeout,
            /* [in] */ ITimeUnit* tunit,
            /* [out] */ IManagedClientConnection** connection);

    private:
        AutoPtr<IHttpRoute> mRoute;
        AutoPtr<IPoolEntryRequest> mRequest;
        ThreadSafeClientConnManager* mHost;
    };

public:
    /**
     * Creates a new thread safe connection manager.
     *
     * @param params    the parameters for this manager
     * @param schreg    the scheme registry
     */
    ThreadSafeClientConnManager(
        /* [in] */ IHttpParams* params,
        /* [in] */ ISchemeRegistry* schreg);

    CAR_INTERFACE_DECL()

    ~ThreadSafeClientConnManager();

    CARAPI GetSchemeRegistry(
        /* [out] */ ISchemeRegistry** schemeRegistry);

    CARAPI RequestConnection(
        /* [in] */ IHttpRoute* route,
        /* [in] */ IObject* state,
        /* [out] */ IClientConnectionRequest** request);

    CARAPI ReleaseConnection(
        /* [in] */ IManagedClientConnection* conn,
        /* [in] */ Int64 validDuration,
        /* [in] */ ITimeUnit* timeUnit);

    CARAPI Shutdown();

    /**
     * Gets the total number of pooled connections for the given route.
     * This is the total number of connections that have been created and
     * are still in use by this connection manager for the route.
     * This value will not exceed the maximum number of connections per host.
     *
     * @param route     the route in question
     *
     * @return  the total number of pooled connections for that route
     */
    CARAPI_(Int32) GetConnectionsInPool(
        /* [in] */ IHttpRoute* route);

    /**
     * Gets the total number of pooled connections.  This is the total number of
     * connections that have been created and are still in use by this connection
     * manager.  This value will not exceed the maximum number of connections
     * in total.
     *
     * @return the total number of pooled connections
     */
    CARAPI_(Int32) GetConnectionsInPool();

    CARAPI CloseIdleConnections(
        /* [in] */ Int64 validDuration,
        /* [in] */ ITimeUnit* timeUnit);

    CARAPI CloseExpiredConnections();

protected:
    /**
     * Hook for creating the connection pool.
     *
     * @return  the connection pool to use
     */
    CARAPI_(AutoPtr<AbstractConnPool>) CreateConnectionPool(
        /* [in] */ IHttpParams* params);

    /**
     * Hook for creating the connection operator.
     * It is called by the constructor.
     * Derived classes can override this method to change the
     * instantiation of the operator.
     * The default implementation here instantiates
     * {@link DefaultClientConnectionOperator DefaultClientConnectionOperator}.
     *
     * @param schreg    the scheme registry to use, or <code>null</code>
     *
     * @return  the connection operator to use
     */
    CARAPI_(AutoPtr<IClientConnectionOperator>) CreateConnectionOperator(
        /* [in] */ ISchemeRegistry* schreg);

protected:
    /** The schemes supported by this connection manager. */
    AutoPtr<ISchemeRegistry> mSchemeRegistry;

    /** The pool of connections being managed. */
    AutoPtr<AbstractConnPool> mConnectionPool;

    /** The operator for opening and updating connections. */
    AutoPtr<IClientConnectionOperator> mConnOperator;

private:
    // private final Log log = LogFactory.getLog(getClass());
};

} // namespace Tsccm
} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_THREADSAFECLIENTCONNMANAGER_H__
