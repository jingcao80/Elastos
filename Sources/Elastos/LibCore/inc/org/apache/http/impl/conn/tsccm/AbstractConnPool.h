//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_ABSTRACTCONNPOOL_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_ABSTRACTCONNPOOL_H__

#include "org/apache/http/impl/conn/IdleConnectionHandler.h"
#include "org/apache/http/impl/conn/tsccm/RefQueueWorker.h"
#include "org/apache/http/impl/conn/tsccm/BasicPoolEntry.h"

using Elastos::Utility::ISet;
using Elastos::Utility::IQueue;
using Elastos::Utility::Concurrent::Locks::IReentrantLock;
using Elastos::Utility::Concurrent::ITimeUnit;
using Org::Apache::Http::Conn::Routing::IHttpRoute;
using Org::Apache::Http::Conn::IOperatedClientConnection;
using Org::Apache::Http::Impl::Conn::IdleConnectionHandler;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {
namespace Tsccm {

class ThreadSafeClientConnManager;

/**
 * An abstract connection pool.
 * It is used by the {@link ThreadSafeClientConnManager}.
 * The abstract pool includes a {@link #poolLock}, which is used to
 * synchronize access to the internal pool datastructures.
 * Don't use <code>synchronized</code> for that purpose!
 */
class AbstractConnPool
    : public Object
    , public IRefQueueHandler
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Enables connection garbage collection (GC).
     * This method must be called immediately after creating the
     * connection pool. It is not possible to enable connection GC
     * after pool entries have been created. Neither is it possible
     * to disable connection GC.
     *
     * @throws IllegalStateException
     *         if connection GC is already enabled, or if it cannot be
     *         enabled because there already are pool entries
     */
    CARAPI EnableConnectionGC();

    /**
     * Obtains a pool entry with a connection within the given timeout.
     *
     * @param route     the route for which to get the connection
     * @param timeout   the timeout, 0 or negative for no timeout
     * @param tunit     the unit for the <code>timeout</code>,
     *                  may be <code>null</code> only if there is no timeout
     *
     * @return  pool entry holding a connection for the route
     *
     * @throws ConnectionPoolTimeoutException
     *         if the timeout expired
     * @throws InterruptedException
     *         if the calling thread was interrupted
     */
    CARAPI GetEntry(
        /* [in] */ IHttpRoute* route,
        /* [in] */ IObject* state,
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* tunit,
        /* [out] */ BasicPoolEntry** entry);

    /**
     * Returns a new {@link PoolEntryRequest}, from which a {@link BasicPoolEntry}
     * can be obtained, or the request can be aborted.
     */
    virtual CARAPI_(AutoPtr<IPoolEntryRequest>) RequestPoolEntry(
        /* [in] */ IHttpRoute* route,
        /* [in] */ IObject* state) = 0;

    /**
     * Returns an entry into the pool.
     * The connection of the entry is expected to be in a suitable state,
     * either open and re-usable, or closed. The pool will not make any
     * attempt to determine whether it can be re-used or not.
     *
     * @param entry     the entry for the connection to release
     * @param reusable  <code>true</code> if the entry is deemed
     *                  reusable, <code>false</code> otherwise.
     * @param validDuration The duration that the entry should remain free and reusable.
     * @param timeUnit The unit of time the duration is measured in.
     */
    virtual CARAPI_(void) FreeEntry(
        /* [in] */ BasicPoolEntry* entry,
        /* [in] */ Boolean reusable,
        /* [in] */ Int64 validDuration,
        /* [in] */ ITimeUnit* timeUnit) = 0;

    CARAPI HandleReference(
        /* [in] */ IInterface* ref);

    /**
     * Closes idle connections.
     *
     * @param idletime  the time the connections should have been idle
     *                  in order to be closed now
     * @param tunit     the unit for the <code>idletime</code>
     */
    CARAPI CloseIdleConnections(
        /* [in] */ Int64 idletime,
        /* [in] */ ITimeUnit* tunit);

    CARAPI CloseExpiredConnections();

    //@@@ revise this cleanup stuff (closeIdle+deleteClosed), it's not good

    /**
     * Deletes all entries for closed connections.
     */
    virtual CARAPI_(void) DeleteClosedConnections() = 0;

    /**
     * Shuts down this pool and all associated resources.
     * Overriding methods MUST call the implementation here!
     */
    CARAPI_(void) Shutdown();

protected:
    AbstractConnPool();

    /**
     * Handles cleaning up for a lost pool entry with the given route.
     * A lost pool entry corresponds to a connection that was
     * garbage collected instead of being properly released.
     *
     * @param route     the route of the pool entry that was lost
     */
    virtual CARAPI_(void) HandleLostEntry(
        /* [in] */ IHttpRoute* route) = 0;

    /**
     * Closes a connection from this pool.
     *
     * @param conn      the connection to close, or <code>null</code>
     */
    CARAPI CloseConnection(
        /* [in] */ IOperatedClientConnection* conn);

protected:
    /**
     * The global lock for this pool.
     */
    AutoPtr<IReentrantLock> mPoolLock;

    /**
     * References to issued connections.
     * Objects in this set are of class
     * {@link BasicPoolEntryRef BasicPoolEntryRef},
     * and point to the pool entry for the issued connection.
     * GCed connections are detected by the missing pool entries.
     */
    AutoPtr<ISet> mIssuedConnections;

    /** The handler for idle connections. */
    AutoPtr<IdleConnectionHandler> mIdleConnHandler;

    /** The current total number of connections. */
    Int32 mNumConnections;

    /**
     * A reference queue to track loss of pool entries to GC.
     * The same queue is used to track loss of the connection manager,
     * so we cannot specialize the type.
     */
    AutoPtr<IQueue> mRefQueue;// ReferenceQueue<Object> mRefQueue;

    /** Indicates whether this pool is shut down. */
    Boolean mIsShutDown;

private:
    /** A worker (thread) to track loss of pool entries to GC. */
    AutoPtr<RefQueueWorker> mRefWorker;

// private final Log log = LogFactory.getLog(getClass());

    friend class ThreadSafeClientConnManager;

};

} // namespace Tsccm
} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_ABSTRACTCONNPOOL_H__
