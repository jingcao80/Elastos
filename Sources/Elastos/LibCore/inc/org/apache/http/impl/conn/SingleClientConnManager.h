
#ifndef __ORG_APACHE_HTTP_IMPL_CONN_SINGLECLIENTCONNMANAGER_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_SINGLECLIENTCONNMANAGER_H__

#include "org/apache/http/impl/conn/AbstractPoolEntry.h"
#include "org/apache/http/impl/conn/AbstractPooledConnAdapter.h"
#include "elastos/core/Object.h"

using Elastos::Net::ISocket;
using Elastos::Utility::Concurrent::ITimeUnit;
using Org::Apache::Http::Conn::IClientConnectionManager;
using Org::Apache::Http::Conn::IClientConnectionOperator;
using Org::Apache::Http::Conn::IClientConnectionRequest;
using Org::Apache::Http::Conn::IManagedClientConnection;
using Org::Apache::Http::Conn::Routing::IRouteTracker;
using Org::Apache::Http::Conn::Routing::IHttpRoute;
using Org::Apache::Http::Conn::Scheme::ISchemeRegistry;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

extern "C" const InterfaceID EIID_ConnAdapter;

/**
 * A connection "manager" for a single connection.
 * This manager is good only for single-threaded use.
 * Allocation <i>always</i> returns the connection immediately,
 * even if it has not been released after the previous allocation.
 * In that case, a {@link #MISUSE_MESSAGE warning} is logged
 * and the previously issued connection is revoked.
 * <p>
 * This class is derived from <code>SimpleHttpConnectionManager</code>
 * in HttpClient 3. See there for original authors.
 * </p>
 *
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 * @author <a href="mailto:becke@u.washington.edu">Michael Becke</a>
 *
 *
 * <!-- empty lines to avoid svn diff problems -->
 * @version   $Revision: 673450 $
 *
 * @since 4.0
 */
class SingleClientConnManager
    : public Object
    , public IClientConnectionManager
{
protected:
    /**
     * The pool entry for this connection manager.
     */
    class PoolEntry : public AbstractPoolEntry
    {
    protected:
        /**
         * Creates a new pool entry.
         *
         */
        PoolEntry(
            /* [in] */ SingleClientConnManager* host);

        /**
         * Closes the connection in this pool entry.
         */
        CARAPI Close();

        /**
         * Shuts down the connection in this pool entry.
         */
        CARAPI Shutdown();

    private:
        friend class SingleClientConnManager;
    };



    /**
     * The connection adapter used by this manager.
     */
    class ConnAdapter : public AbstractPooledConnAdapter
    {
    protected:
        /**
         * Creates a new connection adapter.
         *
         * @param entry   the pool entry for the connection being wrapped
         * @param route   the planned route for this connection
         */
        ConnAdapter(
            /* [in] */ PoolEntry* entry,
            /* [in] */ IHttpRoute* route,
            /* [in] */ SingleClientConnManager* host);

        CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

    private:
        friend class SingleClientConnManager;
    };

private:
    class MyClientConnectionRequest
        : public Object
        , public IClientConnectionRequest
    {
    public:
        MyClientConnectionRequest(
            /* [in] */ SingleClientConnManager* host,
            /* [in] */ IHttpRoute* route,
            /* [in] */ IObject* state);

        CAR_INTERFACE_DECL()

        CARAPI AbortRequest();

        CARAPI GetConnection(
            /* [in] */ Int64 timeout,
            /* [in] */ ITimeUnit* tunit,
            /* [out] */ IManagedClientConnection** connection);

    private:
        SingleClientConnManager* mHost;
        AutoPtr<IHttpRoute> mRoute;
        AutoPtr<IObject> mState;
    };

public:
    SingleClientConnManager(
        /* [in] */ IHttpParams* params,
        /* [in] */ ISchemeRegistry* schreg);

    ~SingleClientConnManager();

    CAR_INTERFACE_DECL()

    CARAPI GetSchemeRegistry(
        /* [out] */ ISchemeRegistry** schemeRegistry);

    CARAPI RequestConnection(
        /* [in] */ IHttpRoute* route,
        /* [in] */ IObject* state,
        /* [out] */ IClientConnectionRequest** request);

    /**
     * Obtains a connection.
     * This method does not block.
     *
     * @param route     where the connection should point to
     *
     * @return  a connection that can be used to communicate
     *          along the given route
     */
    CARAPI GetConnection(
        /* [in] */ IHttpRoute* route,
        /* [in] */ IObject* state,
        /* [out] */ IManagedClientConnection** connection);

    CARAPI ReleaseConnection(
        /* [in] */ IManagedClientConnection* conn,
        /* [in] */ Int64 validDuration,
        /* [in] */ ITimeUnit* timeUnit);

    CARAPI CloseExpiredConnections();

    CARAPI CloseIdleConnections(
        /* [in] */ Int64 validDuration,
        /* [in] */ ITimeUnit* timeUnit);

    CARAPI Shutdown();

    /**
     * Revokes the currently issued connection.
     * The adapter gets disconnected, the connection will be shut down.
     */
    CARAPI RevokeConnection();

protected:
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

    /**
     * Asserts that this manager is not shut down.
     *
     * @throws IllegalStateException    if this manager is shut down
     */
    CARAPI AssertStillUp();

public:
    /** The message to be logged on multiple allocation. */
    static const String MISUSE_MESSAGE;

protected:
    /** The schemes supported by this connection manager. */
    AutoPtr<ISchemeRegistry> mSchemeRegistry;

    /** The operator for opening and updating connections. */
    AutoPtr<IClientConnectionOperator> mConnOperator;

    /** The one and only entry in this pool. */
    AutoPtr<PoolEntry> mUniquePoolEntry;

    /** The currently issued managed connection, if any. */
    AutoPtr<ConnAdapter> mManagedConn;

    /** The time of the last connection release, or -1. */
    Int64 mLastReleaseTime;

    /** The time the last released connection expires and shouldn't be reused. */
    Int64 mConnectionExpiresTime;

    /** Whether the connection should be shut down  on release. */
    Boolean mAlwaysShutDown;

    /** Indicates whether this connection manager is shut down. */
    Boolean mIsShutDown;

private:
    // private final Log log = LogFactory.getLog(getClass());
};

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_SINGLECLIENTCONNMANAGER_H__
