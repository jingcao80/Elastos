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

#ifndef __ELASTOS_DROID_DATABASE_SQLITE_SQLITECONNECTIONPOOL_H__
#define __ELASTOS_DROID_DATABASE_SQLITE_SQLITECONNECTIONPOOL_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/database/sqlite/SQLiteConnection.h"
#include "elastos/droid/database/sqlite/SQLiteDatabaseConfiguration.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Utility::IPrinter;
using Elastos::Droid::Os::ICancellationSignalOnCancelListener;
using Elastos::Core::IThread;
using Elastos::Core::ICloseGuard;
using Elastos::IO::ICloseable;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Database::Sqlite::SQLiteConnection)

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

class SQLiteConnectionPool
    : public Object
    , public ICloseable
{
public:
    // Describes what should happen to an acquired connection when it is returned to the pool.
    enum AcquiredConnectionStatus {
        // The connection should be returned to the pool as usual.
        NORMAL,

        // The connection must be reconfigured before being returned.
        RECONFIGURE,

        // The connection must be closed and discarded.
        DISCARD,
    };

private:
    class ConnectionWaiter : public Object
    {
    public:
        ConnectionWaiter()
            : mStartTime(0)
            , mPriority(0)
            , mWantPrimaryConnection(FALSE)
            , mConnectionFlags(0)
            , mException(NOERROR)
            , mNonce(0)
        {}

    public:
        AutoPtr<ConnectionWaiter> mNext;
        AutoPtr<IThread> mThread;
        Int64 mStartTime;
        Int32 mPriority;
        Boolean mWantPrimaryConnection;
        String mSql;
        Int32 mConnectionFlags;
        AutoPtr<SQLiteConnection> mAssignedConnection;
        ECode mException;
        Int32 mNonce;
    };

    class OnCancelListener
        : public Object
        , public ICancellationSignalOnCancelListener

    {
    public:
        OnCancelListener(
            /* [in] */ ConnectionWaiter* waiter,
            /* [in] */ Int32 nonce,
            /* [in] */ SQLiteConnectionPool* host);

        CAR_INTERFACE_DECL()

        CARAPI OnCancel();

    private:
        ConnectionWaiter* mWaiter;
        Int32 mNonce;
        SQLiteConnectionPool* mHost;
    };

public:
    ~SQLiteConnectionPool();

    CAR_INTERFACE_DECL()

    /**
     * Opens a connection pool for the specified database.
     *
     * @param configuration The database configuration.
     * @return The connection pool.
     *
     * @throws SQLiteException if a database error occurs.
     */
    static CARAPI Open(
        /* [in] */ SQLiteDatabaseConfiguration* configuration,
        /* [out] */ SQLiteConnectionPool** result);

    /**
     * Closes the connection pool.
     * <p>
     * When the connection pool is closed, it will refuse all further requests
     * to acquire connections.  All connections that are currently available in
     * the pool are closed immediately.  Any connections that are still in use
     * will be closed as soon as they are returned to the pool.
     * </p>
     *
     * @throws IllegalStateException if the pool has been closed.
     */
    CARAPI Close();

    /**
     * Reconfigures the database configuration of the connection pool and all of its
     * connections.
     * <p>
     * Configuration changes are propagated down to connections immediately if
     * they are available or as soon as they are released.  This includes changes
     * that affect the size of the pool.
     * </p>
     *
     * @param configuration The new configuration.
     *
     * @throws IllegalStateException if the pool has been closed.
     */
    CARAPI Reconfigure(
        /* [in] */ SQLiteDatabaseConfiguration* configuration);

    /**
     * Acquires a connection from the pool.
     * <p>
     * The caller must call {@link #releaseConnection} to release the connection
     * back to the pool when it is finished.  Failure to do so will result
     * in much unpleasantness.
     * </p>
     *
     * @param sql If not null, try to find a connection that already has
     * the specified SQL statement in its prepared statement cache.
     * @param connectionFlags The connection request flags.
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     * @return The connection that was acquired, never null.
     *
     * @throws IllegalStateException if the pool has been closed.
     * @throws SQLiteException if a database error occurs.
     * @throws OperationCanceledException if the operation was canceled.
     */
    CARAPI AcquireConnection(
        /* [in] */ const String& sql,
        /* [in] */ Int32 connectionFlags,
        /* [in] */ ICancellationSignal* cancellationSignal,
        /* [out] */ SQLiteConnection** result);

    /**
     * Releases a connection back to the pool.
     * <p>
     * It is ok to call this method after the pool has closed, to release
     * connections that were still in use at the time of closure.
     * </p>
     *
     * @param connection The connection to release.  Must not be null.
     *
     * @throws IllegalStateException if the connection was not acquired
     * from this pool or if it has already been released.
     */
    CARAPI ReleaseConnection(
        /* [in] */ SQLiteConnection* connection);

    /**
     * Returns true if the session should yield the connection due to
     * contention over available database connections.
     *
     * @param connection The connection owned by the session.
     * @param connectionFlags The connection request flags.
     * @return True if the session should yield its connection.
     *
     * @throws IllegalStateException if the connection was not acquired
     * from this pool or if it has already been released.
     */
    CARAPI ShouldYieldConnection(
        /* [in] */ SQLiteConnection* connection,
        /* [in] */ Int32 connectionFlags,
        /* [out] */ Boolean* result);

    /**
     * Collects statistics about database connection memory usage.
     *
     * @param dbStatsList The list to populate.
     */
    CARAPI_(void) CollectDbStats(
        /* [in] */ List<AutoPtr<SQLiteDebug::DbStats> >* dbStatsList);

    CARAPI_(void) OnConnectionLeaked();

    /**
     * Dumps debugging information about this connection pool.
     *
     * @param printer The printer to receive the dump, not null.
     * @param verbose True to dump more verbose information.
     */
    CARAPI_(void) Dump(
        /* [in] */ IPrinter* printer,
        /* [in] */ Boolean verbose);

    using Object::ToString;

    //@Override
    CARAPI_(String) ToString();

private:
    SQLiteConnectionPool(
        /* [in] */ SQLiteDatabaseConfiguration* configuration);

    // Might throw
    CARAPI Open();

    CARAPI Dispose(
        /* [in] */ Boolean finalized);

    // Can't throw.
    CARAPI_(Boolean) RecycleConnectionLocked(
        /* [in] */ SQLiteConnection* connection,
        /* [in] */ AcquiredConnectionStatus status);

    // Might throw.
    CARAPI OpenConnectionLocked(
        /* [in] */ SQLiteDatabaseConfiguration* configuration,
        /* [in] */ Boolean primaryConnection,
        /* [out] */ SQLiteConnection** result);

    // Can't throw.
    CARAPI_(void) CloseAvailableConnectionsAndLogExceptionsLocked();

    // Can't throw.
    CARAPI_(void) CloseAvailableNonPrimaryConnectionsAndLogExceptionsLocked();

    // Can't throw.
    CARAPI_(void) CloseExcessConnectionsAndLogExceptionsLocked();

    // Can't throw.
    CARAPI_(void) CloseConnectionAndLogExceptionsLocked(
        /* [in] */ SQLiteConnection* connection);

    // Can't throw.
    CARAPI_(void) DiscardAcquiredConnectionsLocked();

    // Can't throw.
    CARAPI_(void) ReconfigureAllConnectionsLocked();

    // Can't throw.
    CARAPI_(void) MarkAcquiredConnectionsLocked(
        /* [in] */ AcquiredConnectionStatus status);

    // Might throw.
    CARAPI WaitForConnection(
        /* [in] */ const String& sql,
        /* [in] */ Int32 connectionFlags,
        /* [in] */ ICancellationSignal* cancellationSignal,
        /* [out] */ SQLiteConnection** result);

    // Can't throw.
    CARAPI_(void) CancelConnectionWaiterLocked(
        /* [in] */ ConnectionWaiter* waiter);

    // Can't throw.
    CARAPI_(void) LogConnectionPoolBusyLocked(
        /* [in] */ Int64 waitMillis,
        /* [in] */ Int32 connectionFlags);

    // Can't throw.
    CARAPI_(void) WakeConnectionWaitersLocked();

    // Might throw.
    CARAPI TryAcquirePrimaryConnectionLocked(
        /* [in] */ Int32 connectionFlags,
        /* [out] */ SQLiteConnection** connection);

    // Might throw.
    CARAPI TryAcquireNonPrimaryConnectionLocked(
        /* [in] */ const String& sql,
        /* [in] */ Int32 connectionFlags,
        /* [out] */ SQLiteConnection** connection);

    // Might throw.
    CARAPI_(void) FinishAcquireConnectionLocked(
        /* [in] */ SQLiteConnection* connection,
        /* [in] */ Int32 connectionFlags);

    CARAPI_(Boolean) IsSessionBlockingImportantConnectionWaitersLocked(
        /* [in] */ Boolean holdingPrimaryConnection,
        /* [in] */ Int32 connectionFlags);

    static CARAPI_(Int32) GetPriority(
        /* [in] */ Int32 connectionFlags);

    CARAPI_(void) SetMaxConnectionPoolSizeLocked();

    CARAPI ThrowIfClosedLocked();

    CARAPI_(AutoPtr<ConnectionWaiter>) ObtainConnectionWaiterLocked(
        /* [in] */ IThread* thread,
        /* [in] */ Int64 startTime,
        /* [in] */ Int32 priority,
        /* [in] */ Boolean wantPrimaryConnection,
        /* [in] */ const String& sql,
        /* [in] */ Int32 connectionFlags);

    CARAPI_(void) RecycleConnectionWaiterLocked(
        /* [in] */ ConnectionWaiter* waiter);

public:
    /**
     * Connection flag: Read-only.
     * <p>
     * This flag indicates that the connection will only be used to
     * perform read-only operations.
     * </p>
     */
    static const Int32 CONNECTION_FLAG_READ_ONLY = 1 << 0;

    /**
     * Connection flag: Primary connection affinity.
     * <p>
     * This flag indicates that the primary connection is required.
     * This flag helps support legacy applications that expect most data modifying
     * operations to be serialized by locking the primary database connection.
     * Setting this flag essentially implements the old "db lock" concept by preventing
     * an operation from being performed until it can obtain exclusive access to
     * the primary connection.
     * </p>
     */
    static const Int32 CONNECTION_FLAG_PRIMARY_CONNECTION_AFFINITY = 1 << 1;

    /**
     * Connection flag: Connection is being used interactively.
     * <p>
     * This flag indicates that the connection is needed by the UI thread.
     * The connection pool can use this flag to elevate the priority
     * of the database connection request.
     * </p>
     */
    static const Int32 CONNECTION_FLAG_INTERACTIVE = 1 << 2;

private:
    static const String TAG;

    // Amount of time to wait in milliseconds before unblocking acquireConnection
    // and logging a message about the connection pool being busy.
    static const Int64 CONNECTION_POOL_BUSY_MILLIS = 30 * 1000; // 30 seconds

    AutoPtr<ICloseGuard> mCloseGuard;

    Object mLock;
    AutoPtr<IAtomicBoolean> mConnectionLeaked;
    AutoPtr<SQLiteDatabaseConfiguration> mConfiguration;
    Int32 mMaxConnectionPoolSize;
    Boolean mIsOpen;
    Int32 mNextConnectionId;

    AutoPtr<ConnectionWaiter> mConnectionWaiterPool;
    AutoPtr<ConnectionWaiter> mConnectionWaiterQueue;

    // Strong references to all available connections.
    List<AutoPtr<SQLiteConnection> > mAvailableNonPrimaryConnections;
    AutoPtr<SQLiteConnection> mAvailablePrimaryConnection;

    // Weak references to all acquired connections.  The associated value
    // indicates whether the connection must be reconfigured before being
    // returned to the available connection list or discarded.
    // For example, the prepared statement cache size may have changed and
    // need to be updated in preparation for the next client.
    HashMap<AutoPtr<SQLiteConnection>, AcquiredConnectionStatus> mAcquiredConnections;
};

} //Sqlite
} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_SQLITE_SQLITECONNECTIONPOOL_H__
