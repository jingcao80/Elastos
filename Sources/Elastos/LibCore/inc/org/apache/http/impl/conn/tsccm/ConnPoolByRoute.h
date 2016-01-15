
#ifndef __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_CONNPOOLBYROUTE_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_CONNPOOLBYROUTE_H__

#include "org/apache/http/impl/conn/tsccm/AbstractConnPool.h"
#include "org/apache/http/impl/conn/tsccm/BasicPoolEntry.h"
#include "org/apache/http/impl/conn/tsccm/WaitingThreadAborter.h"
#include "org/apache/http/impl/conn/tsccm/WaitingThread.h"

using Elastos::Utility::IMap;
using Elastos::Utility::IQueue;
using Elastos::Utility::Concurrent::ITimeUnit;
using Elastos::Utility::Concurrent::Locks::ICondition;
using Org::Apache::Http::Conn::IClientConnectionOperator;
using Org::Apache::Http::Conn::Routing::IHttpRoute;
using Org::Apache::Http::Conn::Params::IConnPerRoute;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {
namespace Tsccm {

/**
 * A connection pool that maintains connections by route.
 * This class is derived from <code>MultiThreadedHttpConnectionManager</code>
 * in HttpClient 3.x, see there for original authors. It implements the same
 * algorithm for connection re-use and connection-per-host enforcement:
 * <ul>
 * <li>connections are re-used only for the exact same route</li>
 * <li>connection limits are enforced per route rather than per host</li>
 * </ul>
 * Note that access to the pool datastructures is synchronized via the
 * {@link AbstractConnPool#poolLock poolLock} in the base class,
 * not via <code>synchronized</code> methods.
 *
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 * @author <a href="mailto:becke@u.washington.edu">Michael Becke</a>
 * @author and others
 */
class ConnPoolByRoute : public AbstractConnPool
{
private:
    class MyPoolEntryRequest
        : public Object
        , public IPoolEntryRequest
    {
    public:
        MyPoolEntryRequest(
            /* [in] */ IHttpRoute* route,
            /* [in] */ IObject* state,
            /* [in] */ WaitingThreadAborter* aborter,
            /* [in] */ ConnPoolByRoute* host);

        CAR_INTERFACE_DECL()

        CARAPI GetPoolEntry(
            /* [in] */ Int64 timeout,
            /* [in] */ ITimeUnit* tunit,
            /* [out] */ IInterface** basicPoolEntry);

        CARAPI AbortRequest();

    private:
        AutoPtr<IHttpRoute> mRoute;
        AutoPtr<IObject> mState;
        AutoPtr<WaitingThreadAborter> mAborter;
        ConnPoolByRoute* mHost;
    };

public:
    /**
     * Creates a new adapter.
     *
     * @param tsccm   the connection manager
     * @param entry   the pool entry for the connection being wrapped
     */
    ConnPoolByRoute(
        /* [in] */ IClientConnectionOperator* oper,
        /* [in] */ IHttpParams* params);

    //@@@ consider alternatives for gathering statistics
    CARAPI_(Int32) GetConnectionsInPool(
        /* [in] */ IHttpRoute* route);

    CARAPI_(AutoPtr<IPoolEntryRequest>) RequestPoolEntry(
        /* [in] */ IHttpRoute* route,
        /* [in] */ IObject* state);

    CARAPI_(void) FreeEntry(
        /* [in] */ BasicPoolEntry* entry,
        /* [in] */ Boolean reusable,
        /* [in] */ Int64 validDuration,
        /* [in] */ ITimeUnit* timeUnit);

    CARAPI_(void) DeleteClosedConnections();

    CARAPI_(void) Shutdown();

protected:
    /**
     * Creates the queue for {@link #freeConnections}.
     * Called once by the constructor.
     *
     * @return  a queue
     */
    CARAPI_(AutoPtr<IQueue>) CreateFreeConnQueue();

    /**
     * Creates the queue for {@link #waitingThreads}.
     * Called once by the constructor.
     *
     * @return  a queue
     */
    CARAPI_(AutoPtr<IQueue>) CreateWaitingThreadQueue();

    /**
     * Creates the map for {@link #routeToPool}.
     * Called once by the constructor.
     *
     * @return  a map
     */
    CARAPI_(AutoPtr<IMap>) CreateRouteToPoolMap();

    /**
     * Creates a new route-specific pool.
     * Called by {@link #getRoutePool} when necessary.
     *
     * @param route     the route
     *
     * @return  the new pool
     */
    CARAPI_(AutoPtr<RouteSpecificPool>) NewRouteSpecificPool(
        /* [in] */ IHttpRoute* route);

    /**
     * Creates a new waiting thread.
     * Called by {@link #getRoutePool} when necessary.
     *
     * @param cond      the condition to wait for
     * @param rospl     the route specific pool, or <code>null</code>
     *
     * @return  a waiting thread representation
     */
    CARAPI_(AutoPtr<WaitingThread>) NewWaitingThread(
        /* [in] */ ICondition* cond,
        /* [in] */ RouteSpecificPool* rospl);

    /**
     * Get a route-specific pool of available connections.
     *
     * @param route   the route
     * @param create    whether to create the pool if it doesn't exist
     *
     * @return  the pool for the argument route,
     *     never <code>null</code> if <code>create</code> is <code>true</code>
     */
    CARAPI_(AutoPtr<RouteSpecificPool>) GetRoutePool(
        /* [in] */ IHttpRoute* route,
        /* [in] */ Boolean create);

    /**
     * Obtains a pool entry with a connection within the given timeout.
     * If a {@link WaitingThread} is used to block, {@link WaitingThreadAborter#setWaitingThread(WaitingThread)}
     * must be called before blocking, to allow the thread to be interrupted.
     *
     * @param route     the route for which to get the connection
     * @param timeout   the timeout, 0 or negative for no timeout
     * @param tunit     the unit for the <code>timeout</code>,
     *                  may be <code>null</code> only if there is no timeout
     * @param aborter   an object which can abort a {@link WaitingThread}.
     *
     * @return  pool entry holding a connection for the route
     *
     * @throws ConnectionPoolTimeoutException
     *         if the timeout expired
     * @throws InterruptedException
     *         if the calling thread was interrupted
     */
    CARAPI GetEntryBlocking(
        /* [in] */ IHttpRoute* route,
        /* [in] */ IObject* state,
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* tunit,
        /* [in] */ WaitingThreadAborter* aborter,
        /* [out] */ BasicPoolEntry** poolEntry);

    /**
     * If available, get a free pool entry for a route.
     *
     * @param rospl       the route-specific pool from which to get an entry
     *
     * @return  an available pool entry for the given route, or
     *          <code>null</code> if none is available
     */
    CARAPI_(AutoPtr<BasicPoolEntry>) GetFreeEntry(
        /* [in] */ RouteSpecificPool* rospl,
        /* [in] */ IObject* state);

    /**
     * Creates a new pool entry.
     * This method assumes that the new connection will be handed
     * out immediately.
     *
     * @param rospl       the route-specific pool for which to create the entry
     * @param op        the operator for creating a connection
     *
     * @return  the new pool entry for a new connection
     */
    CARAPI_(AutoPtr<BasicPoolEntry>) CreateEntry(
        /* [in] */ RouteSpecificPool* rospl,
        /* [in] */ IClientConnectionOperator* op);

    /**
     * Deletes a given pool entry.
     * This closes the pooled connection and removes all references,
     * so that it can be GCed.
     *
     * <p><b>Note:</b> Does not remove the entry from the freeConnections list.
     * It is assumed that the caller has already handled this step.</p>
     * <!-- @@@ is that a good idea? or rather fix it? -->
     *
     * @param entry         the pool entry for the connection to delete
     */
    CARAPI_(void) DeleteEntry(
        /* [in] */ BasicPoolEntry* entry);

    /**
     * Delete an old, free pool entry to make room for a new one.
     * Used to replace pool entries with ones for a different route.
     */
    CARAPI_(void) DeleteLeastUsedEntry();

    CARAPI_(void) HandleLostEntry(
        /* [in] */ IHttpRoute* route);

    /**
     * Notifies a waiting thread that a connection is available.
     * This will wake a thread waiting in the specific route pool,
     * if there is one.
     * Otherwise, a thread in the connection pool will be notified.
     *
     * @param rospl     the pool in which to notify, or <code>null</code>
     */
    CARAPI_(void) NotifyWaitingThread(
        /* [in] */ RouteSpecificPool* rospl);

protected:
    /** Connection operator for this pool */
    AutoPtr<IClientConnectionOperator> mOperator;

    /** The list of free connections */
    AutoPtr<IQueue> mFreeConnections;

    /** The list of WaitingThreads waiting for a connection */
    AutoPtr<IQueue> mWaitingThreads;

    /**
     * A map of route-specific pools.
     * Keys are of class {@link HttpRoute},
     * values of class {@link RouteSpecificPool}.
     */
    AutoPtr<IMap> mRouteToPool;

    Int32 mMaxTotalConnections;

private:
    // final Log log = LogFactory.getLog(getClass());

    AutoPtr<IConnPerRoute> mConnPerRoute;
};

} // namespace Tsccm
} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_CONNPOOLBYROUTE_H__
