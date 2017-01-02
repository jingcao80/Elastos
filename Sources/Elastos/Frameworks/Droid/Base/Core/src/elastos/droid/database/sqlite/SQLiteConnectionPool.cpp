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

#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/database/sqlite/SQLiteConnectionPool.h"
#include "elastos/droid/database/sqlite/SQLiteGlobal.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/Thread.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::EIID_ICancellationSignalOnCancelListener;
using Elastos::Core::Thread;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::ICloseGuardHelper;
using Elastos::Core::CCloseGuardHelper;
using Elastos::IO::EIID_ICloseable;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::Concurrent::Locks::ILockSupport;
using Elastos::Utility::Concurrent::Locks::CLockSupport;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

const Int32 SQLiteConnectionPool::CONNECTION_FLAG_READ_ONLY;
const Int32 SQLiteConnectionPool::CONNECTION_FLAG_PRIMARY_CONNECTION_AFFINITY;
const Int32 SQLiteConnectionPool::CONNECTION_FLAG_INTERACTIVE;

const String SQLiteConnectionPool::TAG("SQLiteConnectionPool");
const Int64 SQLiteConnectionPool::CONNECTION_POOL_BUSY_MILLIS;

CAR_INTERFACE_IMPL(SQLiteConnectionPool, Object, ICloseable)

SQLiteConnectionPool::SQLiteConnectionPool(
    /* [in] */ SQLiteDatabaseConfiguration* configuration)
    : mMaxConnectionPoolSize(0)
    , mIsOpen(FALSE)
    , mNextConnectionId(0)
{
    CAtomicBoolean::New((IAtomicBoolean**)&mConnectionLeaked);
    mConfiguration = new SQLiteDatabaseConfiguration(configuration);
    SetMaxConnectionPoolSizeLocked();

    AutoPtr<ICloseGuardHelper> helper;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&helper);
    helper->Get((ICloseGuard**)&mCloseGuard);
}

SQLiteConnectionPool::~SQLiteConnectionPool()
{
    // try {
    Dispose(TRUE);
    // } finally {
    //     super.finalize();
    // }
}

ECode SQLiteConnectionPool::Open(
    /* [in] */ SQLiteDatabaseConfiguration* configuration,
    /* [out] */ SQLiteConnectionPool** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (configuration == NULL) {
        Slogger::E(TAG, "configuration must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Create the pool.
    AutoPtr<SQLiteConnectionPool> pool = new SQLiteConnectionPool(configuration);
    FAIL_RETURN(pool->Open()) // might throw
    *result = pool;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SQLiteConnectionPool::Open()
{
    mAvailablePrimaryConnection = NULL;
    // Open the primary connection.
    // This might throw if the database is corrupt.
    FAIL_RETURN(OpenConnectionLocked(mConfiguration, TRUE /*primaryConnection*/,
            (SQLiteConnection**)&mAvailablePrimaryConnection)) // might throw
    // Mark the pool as being open for business.
    mIsOpen = TRUE;

    mCloseGuard->Open(String("SQLiteConnectionPool::Close"));
    return NOERROR;
}

ECode SQLiteConnectionPool::Close()
{
    return Dispose(FALSE);
}

ECode SQLiteConnectionPool::Dispose(
    /* [in] */ Boolean finalized)
{
    if (mCloseGuard != NULL) {
        if (finalized) {
            mCloseGuard->WarnIfOpen();
        }
        mCloseGuard->Close();
    }

    if (!finalized) {
        // Close all connections.  We don't need (or want) to do this
        // when finalized because we don't know what state the connections
        // themselves will be in.  The finalizer is really just here for CloseGuard.
        // The connections will take care of themselves when their own finalizers run.
        {    AutoLock syncLock(mLock);
            FAIL_RETURN(ThrowIfClosedLocked())

            mIsOpen = FALSE;
            CloseAvailableConnectionsAndLogExceptionsLocked();

            Int32 pendingCount = mAcquiredConnections.GetSize();
            if (pendingCount != 0) {
                Slogger::I(TAG, "The connection pool for %s has been closed but there are still %d connections in use."
                    " They will be closed as they are released back to the pool.",
                    mConfiguration->mLabel.string(), pendingCount);
            }

            WakeConnectionWaitersLocked();
        }
    }
    return NOERROR;
}

ECode SQLiteConnectionPool::Reconfigure(
    /* [in] */ SQLiteDatabaseConfiguration* configuration)
{
    if (configuration == NULL) {
        //throw new IllegalArgumentException("configuration must not be null.");
        Slogger::E(TAG, "configuration must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    {    AutoLock syncLock(mLock);
        FAIL_RETURN(ThrowIfClosedLocked())

        Boolean walModeChanged = ((configuration->mOpenFlags ^ mConfiguration->mOpenFlags)
                & ISQLiteDatabase::ENABLE_WRITE_AHEAD_LOGGING) != 0;
        if (walModeChanged) {
            // WAL mode can only be changed if there are no acquired connections
            // because we need to close all but the primary connection first.
            if (!mAcquiredConnections.IsEmpty()) {
                Slogger::E(TAG, "Write Ahead Logging (WAL) mode cannot be enabled or disabled"
                    " while there are transactions in progress.  Finish all transactions"
                    " and release all active database connections first.");
                return E_ILLEGAL_STATE_EXCEPTION;
            }

            // Close all non-primary connections.  This should happen immediately
            // because none of them are in use.
            CloseAvailableNonPrimaryConnectionsAndLogExceptionsLocked();
            assert (mAvailableNonPrimaryConnections.IsEmpty());
        }

        Boolean foreignKeyModeChanged = configuration->mForeignKeyConstraintsEnabled
            != mConfiguration->mForeignKeyConstraintsEnabled;
        if (foreignKeyModeChanged) {
            // Foreign key constraints can only be changed if there are no transactions
            // in progress.  To make this clear, we throw an exception if there are
            // any acquired connections.
            if (!mAcquiredConnections.IsEmpty()) {
                Slogger::E(TAG, "Foreign Key Constraints cannot be enabled or disabled"
                    " while there are transactions in progress.  Finish all transactions"
                    " and release all active database connections first.");
                return E_ILLEGAL_STATE_EXCEPTION;
            }
        }

        if (mConfiguration->mOpenFlags != configuration->mOpenFlags) {
            // If we are changing open flags and WAL mode at the same time, then
            // we have no choice but to close the primary connection beforehand
            // because there can only be one connection open when we change WAL mode.
            if (walModeChanged) {
                CloseAvailableConnectionsAndLogExceptionsLocked();
            }

            // Try to reopen the primary connection using the new open flags then
            // close and discard all existing connections.
            // This might throw if the database is corrupt or cannot be opened in
            // the new mode in which case existing connections will remain untouched.
            AutoPtr<SQLiteConnection> newPrimaryConnection;
            FAIL_RETURN(OpenConnectionLocked(configuration,TRUE , (SQLiteConnection**)&newPrimaryConnection)) // might throw

            CloseAvailableConnectionsAndLogExceptionsLocked();
            DiscardAcquiredConnectionsLocked();

            mAvailablePrimaryConnection = newPrimaryConnection;
            FAIL_RETURN(mConfiguration->UpdateParametersFrom(configuration))
            SetMaxConnectionPoolSizeLocked();
        }
        else {
            // Reconfigure the database connections in place.
            FAIL_RETURN(mConfiguration->UpdateParametersFrom(configuration))
            SetMaxConnectionPoolSizeLocked();

            CloseExcessConnectionsAndLogExceptionsLocked();
            ReconfigureAllConnectionsLocked();
        }

        WakeConnectionWaitersLocked();
    }
    return NOERROR;
}

ECode SQLiteConnectionPool::AcquireConnection(
    /* [in] */ const String& sql,
    /* [in] */ Int32 connectionFlags,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ SQLiteConnection** result)
{
    return WaitForConnection(sql, connectionFlags, cancellationSignal, result);
}

ECode SQLiteConnectionPool::ReleaseConnection(
    /* [in] */ SQLiteConnection* connection)
{
    {    AutoLock syncLock(mLock);
        AcquiredConnectionStatus status;
        HashMap<AutoPtr<SQLiteConnection>, AcquiredConnectionStatus>::Iterator it = mAcquiredConnections.Find(connection);
        if (it == mAcquiredConnections.End()) {
            //throw new IllegalStateException("Cannot perform this operation "
            //        + "because the specified connection was not acquired "
            //        + "from this pool or has already been released.");
            Slogger::E(TAG, "Cannot perform this operation because the specified connection"
                " was not acquired from this pool or has already been released.");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        else {
            status = it->mSecond;
            mAcquiredConnections.Erase(it);
        }

        if (!mIsOpen) {
            CloseConnectionAndLogExceptionsLocked(connection);
        }
        else if (connection->IsPrimaryConnection()) {
            if (RecycleConnectionLocked(connection, status)) {
                assert (mAvailablePrimaryConnection == NULL);
                mAvailablePrimaryConnection = connection;
            }
            WakeConnectionWaitersLocked();
        }
        else if (mAvailableNonPrimaryConnections.GetSize() >= UInt32(mMaxConnectionPoolSize - 1)) {
            CloseConnectionAndLogExceptionsLocked(connection);
        }
        else {
            if (RecycleConnectionLocked(connection, status)) {
                mAvailableNonPrimaryConnections.PushBack(connection);
            }
            WakeConnectionWaitersLocked();
        }
    }
    return NOERROR;
}

Boolean SQLiteConnectionPool::RecycleConnectionLocked(
    /* [in] */ SQLiteConnection* connection,
    /* [in] */ AcquiredConnectionStatus status)
{
    if (status == SQLiteConnectionPool::RECONFIGURE) {
        //try {
        ECode ec = connection->Reconfigure(mConfiguration); // might throw
        //} catch (RuntimeException ex) {
        if (FAILED(ec)) {
            Slogger::E(TAG, "Failed to reconfigure released connection, closing it: %p 0x%08x", connection, ec);
        }
        //}
    }
    if (status == DISCARD) {
        CloseConnectionAndLogExceptionsLocked(connection);
        return FALSE;
    }
    return TRUE;
}

ECode SQLiteConnectionPool::ShouldYieldConnection(
    /* [in] */ SQLiteConnection* connection,
    /* [in] */ Int32 connectionFlags,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    {
        AutoLock lock(mLock);
        HashMap<AutoPtr<SQLiteConnection>, AcquiredConnectionStatus>::Iterator it;
        for (it = mAcquiredConnections.Begin(); it != mAcquiredConnections.End(); it++) {
            if (it->mFirst.Get() == connection) {
                break;
            }
        }
        if (it == mAcquiredConnections.End()) {
            //throw new IllegalStateException("Cannot perform this operation "
            //        + "because the specified connection was not acquired "
            //        + "from this pool or has already been released.");
            Slogger::E(TAG, "Cannot perform this operation because the specified connection"
                " was not acquired from this pool or has already been released.");
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        if (!mIsOpen) {
            *result = FALSE;
            return NOERROR;
        }

        *result = IsSessionBlockingImportantConnectionWaitersLocked(
            connection->IsPrimaryConnection(), connectionFlags);
    }
    return NOERROR;
}

void SQLiteConnectionPool::CollectDbStats(
    /* [in] */ List<AutoPtr<SQLiteDebug::DbStats> >* dbStatsList)
{
    {    AutoLock syncLock(mLock);
        if (mAvailablePrimaryConnection != NULL) {
            mAvailablePrimaryConnection->CollectDbStats(dbStatsList);
        }

        List<AutoPtr<SQLiteConnection> >::Iterator it = mAvailableNonPrimaryConnections.Begin();
        for (; it != mAvailableNonPrimaryConnections.End(); ++it) {
            (*it)->CollectDbStats(dbStatsList);
        }

        HashMap<AutoPtr<SQLiteConnection>, AcquiredConnectionStatus>::Iterator it1;
        it1 = mAcquiredConnections.Begin();
        for (; it1 != mAcquiredConnections.End(); ++it1) {
            it1->mFirst->CollectDbStatsUnsafe(dbStatsList);
        }
    }
}

ECode SQLiteConnectionPool::OpenConnectionLocked(
    /* [in] */ SQLiteDatabaseConfiguration* configuration,
    /* [in] */ Boolean primaryConnection,
    /* [out] */ SQLiteConnection** result)
{
    VALIDATE_NOT_NULL(result)
    Int32 connectionId = mNextConnectionId++;
    return SQLiteConnection::Open(this, configuration, connectionId, primaryConnection, result);
}

void SQLiteConnectionPool::OnConnectionLeaked()
{
    // This code is running inside of the SQLiteConnection finalizer.
    //
    // We don't know whether it is just the connection that has been finalized (and leaked)
    // or whether the connection pool has also been or is about to be finalized.
    // Consequently, it would be a bad idea to try to grab any locks or to
    // do any significant work here.  So we do the simplest possible thing and
    // set a flag.  waitForConnection() periodically checks this flag (when it
    // times out) so that it can recover from leaked connections and wake
    // itself or other threads up if necessary.
    //
    // You might still wonder why we don't try to do more to wake up the waiters
    // immediately.  First, as explained above, it would be hard to do safely
    // unless we started an extra Thread to function as a reference queue.  Second,
    // this is never supposed to happen in normal operation.  Third, there is no
    // guarantee that the GC will actually detect the leak in a timely manner so
    // it's not all that important that we recover from the leak in a timely manner
    // either.  Fourth, if a badly behaved application finds itself hung waiting for
    // several seconds while waiting for a leaked connection to be detected and recreated,
    // then perhaps its authors will have added incentive to fix the problem!

    Slogger::W(TAG, "A SQLiteConnection object for database '%s' was leaked!"
        " Please fix your application to end transactions in progress properly"
        " and to close the database when it is no longer needed.",
        mConfiguration->mLabel.string());

    mConnectionLeaked->Set(TRUE);
}

void SQLiteConnectionPool::CloseAvailableConnectionsAndLogExceptionsLocked()
{
    CloseAvailableNonPrimaryConnectionsAndLogExceptionsLocked();

    if (mAvailablePrimaryConnection != NULL) {
        CloseConnectionAndLogExceptionsLocked(mAvailablePrimaryConnection);
        mAvailablePrimaryConnection = NULL;
    }
}

void SQLiteConnectionPool::CloseAvailableNonPrimaryConnectionsAndLogExceptionsLocked()
{
    List<AutoPtr<SQLiteConnection> >::Iterator it = mAvailableNonPrimaryConnections.Begin();
    for (; it != mAvailableNonPrimaryConnections.End(); ++it) {
        CloseConnectionAndLogExceptionsLocked(*it);
    }
    mAvailableNonPrimaryConnections.Clear();
}

void SQLiteConnectionPool::CloseExcessConnectionsAndLogExceptionsLocked()
{
    Int32 availableCount = mAvailableNonPrimaryConnections.GetSize();
    List<AutoPtr<SQLiteConnection> >::ReverseIterator rit = mAvailableNonPrimaryConnections.RBegin();
    while (availableCount-- > mMaxConnectionPoolSize - 1) {
        AutoPtr<SQLiteConnection> connection = *rit;
        rit = List<AutoPtr<SQLiteConnection> >::ReverseIterator(
            mAvailableNonPrimaryConnections.Erase(--(rit.GetBase())));
        CloseConnectionAndLogExceptionsLocked(connection);
    }
}

void SQLiteConnectionPool::CloseConnectionAndLogExceptionsLocked(
    /* [in] */ SQLiteConnection* connection)
{
    //try {
    connection->Close(); // might throw
    //} catch (RuntimeException ex) {
        // Log.e(TAG, "Failed to close connection, its fate is now in the hands "
        //             + "of the merciful GC: " + connection, ex);
    //}
}

void SQLiteConnectionPool::DiscardAcquiredConnectionsLocked()
{
    MarkAcquiredConnectionsLocked(DISCARD);
}

void SQLiteConnectionPool::ReconfigureAllConnectionsLocked()
{
    if (mAvailablePrimaryConnection != NULL) {
        //try {
        ECode ec = mAvailablePrimaryConnection->Reconfigure(mConfiguration); // might throw
        if (FAILED(ec)) {
            Slogger::E(TAG, "Failed to reconfigure available primary connection, closing it: %p, 0x%08x"
                    , mAvailablePrimaryConnection.Get(), ec);
            CloseConnectionAndLogExceptionsLocked(mAvailablePrimaryConnection);
            mAvailablePrimaryConnection = NULL;
        }
        //} catch (RuntimeException ex) {
            // Log.e(TAG, "Failed to reconfigure available primary connection, closing it: "
            //         + mAvailablePrimaryConnection, ex);
            // closeConnectionAndLogExceptionsLocked(mAvailablePrimaryConnection);
            // mAvailablePrimaryConnection = null;
        //}
    }

    List<AutoPtr<SQLiteConnection> >::Iterator it = mAvailableNonPrimaryConnections.Begin();
    for (; it != mAvailableNonPrimaryConnections.End();) {
        AutoPtr<SQLiteConnection> connection = *it;
        //try {
        ECode ec = connection->Reconfigure(mConfiguration); // might throw
        if (FAILED(ec)) {
            Slogger::E(TAG, "Failed to reconfigure available non-primary connection, closing it: %p, 0x%08x"
                    , connection.Get(), ec);
            CloseConnectionAndLogExceptionsLocked(connection);
            it = mAvailableNonPrimaryConnections.Erase(it);
        }
        else {
            ++it;
        }
        //} catch (RuntimeException ex) {
            // Log.e(TAG, "Failed to reconfigure available non-primary connection, closing it: "
            //         + connection, ex);
            // closeConnectionAndLogExceptionsLocked(connection);
            // mAvailableNonPrimaryConnections.remove(i--);
            // count -= 1;
        //}
    }

    MarkAcquiredConnectionsLocked(RECONFIGURE);
}

void SQLiteConnectionPool::MarkAcquiredConnectionsLocked(
    /* [in] */ AcquiredConnectionStatus status)
{
    if (!mAcquiredConnections.IsEmpty()) {
        List<AutoPtr<SQLiteConnection> > keysToUpdate;
        HashMap<AutoPtr<SQLiteConnection>, AcquiredConnectionStatus>::Iterator entry;
        for (entry = mAcquiredConnections.Begin(); entry != mAcquiredConnections.End(); ++entry) {
            AutoPtr<SQLiteConnection> connection = entry->mFirst;
            AcquiredConnectionStatus oldStatus = entry->mSecond;
            if (status != oldStatus
                    && oldStatus != DISCARD) {
                keysToUpdate.PushBack(connection);
            }
        }

        List<AutoPtr<SQLiteConnection> >::Iterator it1 = keysToUpdate.Begin();
        for (; it1 != keysToUpdate.End(); ++it1) {
            mAcquiredConnections[*it1] = status;
        }
    }
}


SQLiteConnectionPool::OnCancelListener::OnCancelListener(
    /* [in] */ ConnectionWaiter* waiter,
    /* [in] */ Int32 nonce,
    /* [in] */ SQLiteConnectionPool* host)
    : mWaiter(waiter)
    , mNonce(nonce)
    , mHost(host)
{}

CAR_INTERFACE_IMPL(SQLiteConnectionPool::OnCancelListener, Object, ICancellationSignalOnCancelListener)

ECode SQLiteConnectionPool::OnCancelListener::OnCancel()
{
    AutoLock lock(mHost->mLock);
    if (mWaiter->mNonce == mNonce) {
        mHost->CancelConnectionWaiterLocked(mWaiter);
    }
    return NOERROR;
}


ECode SQLiteConnectionPool::WaitForConnection(
    /* [in] */ const String& sql,
    /* [in] */ Int32 connectionFlags,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ SQLiteConnection** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    Boolean wantPrimaryConnection =
            (connectionFlags & CONNECTION_FLAG_PRIMARY_CONNECTION_AFFINITY) != 0;

    AutoPtr<ConnectionWaiter> waiter;
    Int32 nonce = 0;
    {    AutoLock syncLock(mLock);
        FAIL_RETURN(ThrowIfClosedLocked())

        // Try to acquire a connection.
        AutoPtr<SQLiteConnection> connection;
        if (!wantPrimaryConnection) {
            FAIL_RETURN(TryAcquireNonPrimaryConnectionLocked(
                sql, connectionFlags, (SQLiteConnection**)&connection)) // might throw
        }
        if (connection == NULL) {
            FAIL_RETURN(TryAcquirePrimaryConnectionLocked(
                connectionFlags, (SQLiteConnection**)&connection)) // might throw
        }
        if (connection != NULL) {
            *result = connection;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }

        // No connections available.  Enqueue a waiter in priority order.
        Int32 priority = GetPriority(connectionFlags);
        Int64 startTime = SystemClock::GetUptimeMillis();
        waiter = ObtainConnectionWaiterLocked(
            Thread::GetCurrentThread(), startTime, priority, wantPrimaryConnection, sql, connectionFlags);
        AutoPtr<ConnectionWaiter> predecessor;
        AutoPtr<ConnectionWaiter> successor = mConnectionWaiterQueue;
        while (successor != NULL) {
            if (priority > successor->mPriority) {
                waiter->mNext = successor;
                break;
            }
            predecessor = successor;
            successor = successor->mNext;
        }
        if (predecessor != NULL) {
            predecessor->mNext = waiter;
        }
        else {
            mConnectionWaiterQueue = waiter;
        }

        nonce = waiter->mNonce;
    }

    // Set up the cancellation listener.
    if (cancellationSignal != NULL) {
        AutoPtr<ICancellationSignalOnCancelListener> listener;
        listener = new OnCancelListener(waiter, nonce, this);
        cancellationSignal->SetOnCancelListener(listener);
    }
    //try {
    // Park the thread until a connection is assigned or the pool is closed.
    // Rethrow an exception from the wait, if we got one.
    Int64 busyTimeoutMillis = CONNECTION_POOL_BUSY_MILLIS;
    Int64 nextBusyTimeoutTime;
    nextBusyTimeoutTime = waiter->mStartTime + busyTimeoutMillis;
    ECode ec = NOERROR;
    for (;;) {
        // Detect and recover from connection leaks.
        Boolean value;
        if (mConnectionLeaked->CompareAndSet(TRUE, FALSE, &value), value) {
            AutoLock lock(mLock);
            WakeConnectionWaitersLocked();
        }

        // Wait to be unparked (may already have happened), a timeout, or interruption.
        // LockSupport::ParkNanos(this, busyTimeoutMillis * 1000000LL);
        AutoPtr<ILockSupport> lockSupport;
        CLockSupport::AcquireSingleton((ILockSupport**)&lockSupport);
        lockSupport->ParkNanos((IObject*)this, busyTimeoutMillis * 1000000LL);

        // Clear the interrupted flag, just in case.
        Thread::Interrupted();

        // Check whether we are done waiting yet.
        {
            AutoLock lock(mLock);
            ec = ThrowIfClosedLocked();
            FAIL_GOTO(ec, fail)
            AutoPtr<SQLiteConnection> connection = waiter->mAssignedConnection;
            ec = waiter->mException;
            if (connection != NULL || ec != NOERROR) {
                RecycleConnectionWaiterLocked(waiter);
                if (connection != NULL) {
                    *result = connection;
                    REFCOUNT_ADD(*result)
                    ec = NOERROR;
                }
                goto fail; // rethrow!
            }

            Int64 now = SystemClock::GetUptimeMillis();
            if (now < nextBusyTimeoutTime) {
                busyTimeoutMillis = now - nextBusyTimeoutTime;
            }
            else {
                LogConnectionPoolBusyLocked(now - waiter->mStartTime, connectionFlags);
                busyTimeoutMillis = CONNECTION_POOL_BUSY_MILLIS;
                nextBusyTimeoutTime = now + busyTimeoutMillis;
            }
        }
    }

fail:
    // Remove the cancellation listener.
    if (cancellationSignal != NULL) {
       cancellationSignal->SetOnCancelListener(NULL);
    }
    return ec;
}

void SQLiteConnectionPool::CancelConnectionWaiterLocked(
    /* [in] */ ConnectionWaiter* waiter)
{
    if (waiter->mAssignedConnection != NULL || waiter->mException != NOERROR) {
        // Waiter is done waiting but has not woken up yet.
        return;
    }

    // Waiter must still be waiting.  Dequeue it.
    AutoPtr<ConnectionWaiter> predecessor;
    AutoPtr<ConnectionWaiter> current = mConnectionWaiterQueue;
    while (current.Get() != waiter) {
        assert(current != NULL);
        predecessor = current;
        current = current->mNext;
    }
    if (predecessor != NULL) {
        predecessor->mNext = waiter->mNext;
    }
    else {
        mConnectionWaiterQueue = waiter->mNext;
    }

    // Send the waiter an exception and unpark it.
    waiter->mException = E_OPERATION_CANCELED_EXCEPTION;
    assert(0);
    //LockSupport->Unpark(waiter->mThread);

    // Check whether removing this waiter will enable other waiters to make progress.
    WakeConnectionWaitersLocked();
}

void SQLiteConnectionPool::LogConnectionPoolBusyLocked(
    /* [in] */ Int64 waitMillis,
    /* [in] */ Int32 connectionFlags)
{
    AutoPtr<IThread> thread = Thread::GetCurrentThread();
    StringBuilder msg("The connection pool for database '");
    msg += mConfiguration->mLabel;
    msg += "' has been unable to grant a connection to thread ";
    Int64 id;
    thread->GetId(&id);
    msg += id;
    msg += " (";
    String name;
    thread->GetName(&name);
    msg += name;
    msg += ") with flags ";
    msg += connectionFlags;
    msg += " for ";
    msg.Append(waitMillis * 0.001f);
    msg += " seconds.\n";

    List<String> requests;
    Int32 activeConnections = 0;
    Int32 idleConnections = 0;
    if (!mAcquiredConnections.IsEmpty()) {
        //begin from this
        HashMap<AutoPtr<SQLiteConnection>, AcquiredConnectionStatus>::Iterator it;
        for (it = mAcquiredConnections.Begin(); it != mAcquiredConnections.End(); ++it) {
            AutoPtr<SQLiteConnection> connection = it->mFirst;
            String description = connection->DescribeCurrentOperationUnsafe();
            if (!description.IsNullOrEmpty()) {
                requests.PushBack(description);
                activeConnections += 1;
            }
            else {
                idleConnections += 1;
            }
        }
    }
    Int32 availableConnections = mAvailableNonPrimaryConnections.GetSize();
    if (mAvailablePrimaryConnection != NULL) {
        availableConnections += 1;
    }

    msg += "Connections: ";
    msg += activeConnections;
    msg += " active, ";
    msg += idleConnections;
    msg += " idle, ";
    msg += availableConnections;
    msg += " available.\n";

    if (!requests.IsEmpty()) {
        msg += "\nRequests in progress:\n";
        List<String>::Iterator requestIt = requests.Begin();
        for (; requestIt != requests.End(); ++requestIt) {
            String request = *requestIt;
            msg += "  ";
            msg += request;
            msg += "\n";
        }

    }
    Slogger::W(TAG, msg.ToString());
}

void SQLiteConnectionPool::WakeConnectionWaitersLocked()
{
    // Unpark all waiters that have requests that we can fulfill.
    // This method is designed to not throw runtime exceptions, although we might send
    // a waiter an exception for it to rethrow.
    AutoPtr<ConnectionWaiter> predecessor;
    AutoPtr<ConnectionWaiter> waiter = mConnectionWaiterQueue;
    Boolean primaryConnectionNotAvailable = FALSE;
    Boolean nonPrimaryConnectionNotAvailable = FALSE;
    while (waiter != NULL) {
        Boolean unpark = FALSE;
        if (!mIsOpen) {
            unpark = TRUE;
        }
        else {
            //try {
            AutoPtr<SQLiteConnection> connection;
            if (!waiter->mWantPrimaryConnection && !nonPrimaryConnectionNotAvailable) {
                ECode ec = TryAcquireNonPrimaryConnectionLocked(waiter->mSql, waiter->mConnectionFlags,
                        (SQLiteConnection**)&connection); // might throw
                if (FAILED(ec)) {
                    waiter->mException = ec;
                    unpark = TRUE;
                }
                else if (connection == NULL) {
                    nonPrimaryConnectionNotAvailable = TRUE;
                }
            }
            if (connection == NULL && !primaryConnectionNotAvailable) {
                ECode ec = TryAcquirePrimaryConnectionLocked(waiter->mConnectionFlags,
                        (SQLiteConnection**)&connection); // might throw
                if (FAILED(ec)) {
                    waiter->mException = ec;
                    unpark = TRUE;
                }
                else if (connection == NULL) {
                    primaryConnectionNotAvailable = TRUE;
                }
            }
            if (connection != NULL) {
                waiter->mAssignedConnection = connection;
                unpark = TRUE;
            }
            else if (nonPrimaryConnectionNotAvailable && primaryConnectionNotAvailable) {
                // There are no connections available and the pool is still open.
                // We cannot fulfill any more connection requests, so stop here.
                break;
            }
            //} catch (RuntimeException ex) {
                // Let the waiter handle the exception from acquiring a connection.
                // waiter.mException = ex;
                // unpark = true;
            //}
        }

        AutoPtr<ConnectionWaiter> successor = waiter->mNext;
        if (unpark) {
            if (predecessor != NULL) {
                predecessor->mNext = successor;
            }
            else {
                mConnectionWaiterQueue = successor;
            }
            waiter->mNext = NULL;
            AutoPtr<ILockSupport> lockSupport;
            CLockSupport::AcquireSingleton((ILockSupport**)&lockSupport);
            lockSupport->Unpark(waiter->mThread);
        }
        else {
            predecessor = waiter;
        }
        waiter = successor;
    }
}

ECode SQLiteConnectionPool::TryAcquirePrimaryConnectionLocked(
    /* [in] */ Int32 connectionFlags,
    /* [out] */ SQLiteConnection** _connection)
{
    VALIDATE_NOT_NULL(_connection)
    *_connection = NULL;
    // If the primary connection is available, acquire it now.
    AutoPtr<SQLiteConnection> connection = mAvailablePrimaryConnection;
    if (connection != NULL) {
        mAvailablePrimaryConnection = NULL;
        FinishAcquireConnectionLocked(connection, connectionFlags); // might throw
        *_connection = connection;
        REFCOUNT_ADD(*_connection)
        return NOERROR;
    }

    // Make sure that the primary connection actually exists and has just been acquired.
    HashMap<AutoPtr<SQLiteConnection>, AcquiredConnectionStatus >::Iterator it;
    for (it = mAcquiredConnections.Begin(); it != mAcquiredConnections.End(); ++it) {
        AutoPtr<SQLiteConnection> acquiredConnection = it->mFirst;
        if (acquiredConnection->IsPrimaryConnection()) {
            *_connection = NULL;
            return NOERROR;
        }
    }

    // Uhoh.  No primary connection!  Either this is the first time we asked
    // for it, or maybe it leaked?
    FAIL_RETURN(OpenConnectionLocked(mConfiguration,
        TRUE /*primaryConnection*/, (SQLiteConnection**)&connection)) // might throw
    FinishAcquireConnectionLocked(connection, connectionFlags); // might throw
    *_connection = connection;
    REFCOUNT_ADD(*_connection)
    return NOERROR;
}

ECode SQLiteConnectionPool::TryAcquireNonPrimaryConnectionLocked(
    /* [in] */ const String& sql,
    /* [in] */ Int32 connectionFlags,
    /* [out] */ SQLiteConnection** _connection)
{
    VALIDATE_NOT_NULL(_connection)
    *_connection = NULL;
    // Try to acquire the next connection in the queue.
    AutoPtr<SQLiteConnection> connection;
    const Int32 availableCount = mAvailableNonPrimaryConnections.GetSize();
    if (availableCount > 1 && sql != NULL) {
        // If we have a choice, then prefer a connection that has the
        // prepared statement in its cache.
        List<AutoPtr<SQLiteConnection> >::Iterator it = mAvailableNonPrimaryConnections.Begin();
        for (; it != mAvailableNonPrimaryConnections.End(); ++it) {
            connection = *it;
            if (connection->IsPreparedStatementInCache(sql)) {
                mAvailableNonPrimaryConnections.Erase(it);
                FinishAcquireConnectionLocked(connection, connectionFlags); // might throw
                *_connection = connection;
                REFCOUNT_ADD(*_connection)
                return NOERROR;
            }
        }
    }
    if (availableCount > 0) {
        // Otherwise, just grab the next one.
        List<AutoPtr<SQLiteConnection> >::ReverseIterator rIt = mAvailableNonPrimaryConnections.RBegin();
        connection = *rIt;
        mAvailableNonPrimaryConnections.Erase(--(rIt.GetBase()));
        FinishAcquireConnectionLocked(connection, connectionFlags); // might throw
        *_connection = connection;
        REFCOUNT_ADD(*_connection)
        return NOERROR;
    }

    // Expand the pool if needed.
    Int32 openConnections = mAcquiredConnections.GetSize();
    if (mAvailablePrimaryConnection != NULL) {
        openConnections += 1;
    }
    if (openConnections >= mMaxConnectionPoolSize) {
        *_connection = NULL;
        return NOERROR;
    }
    FAIL_RETURN(OpenConnectionLocked(mConfiguration,
        FALSE /*primaryConnection*/, (SQLiteConnection**)&connection)) // might throw
    FinishAcquireConnectionLocked(connection, connectionFlags); // might throw
    *_connection = connection;
    REFCOUNT_ADD(*_connection)
    return NOERROR;
}

void SQLiteConnectionPool::FinishAcquireConnectionLocked(
    /* [in] */ SQLiteConnection* connection,
    /* [in] */ Int32 connectionFlags)
{
    //try {
    Boolean readOnly = (connectionFlags & CONNECTION_FLAG_READ_ONLY) != 0;
    connection->SetOnlyAllowReadOnlyOperations(readOnly);
    mAcquiredConnections[connection] = NORMAL;
    //} catch (RuntimeException ex) {
        // Log.e(TAG, "Failed to prepare acquired connection for session, closing it: "
        //         + connection +", connectionFlags=" + connectionFlags);
        // closeConnectionAndLogExceptionsLocked(connection);
        // throw ex; // rethrow!
    //}
}

Boolean SQLiteConnectionPool::IsSessionBlockingImportantConnectionWaitersLocked(
    /* [in] */ Boolean holdingPrimaryConnection,
    /* [in] */ Int32 connectionFlags)
{
    AutoPtr<ConnectionWaiter> waiter = mConnectionWaiterQueue;
    if (waiter != NULL) {
        Int32 priority = GetPriority(connectionFlags);
        do {
            // Only worry about blocked connections that have same or lower priority.
            if (priority > waiter->mPriority) {
                break;
            }

            // If we are holding the primary connection then we are blocking the waiter.
            // Likewise, if we are holding a non-primary connection and the waiter
            // would accept a non-primary connection, then we are blocking the waier.
            if (holdingPrimaryConnection || !waiter->mWantPrimaryConnection) {
                return TRUE;
            }

            waiter = waiter->mNext;
        }
        while (waiter != NULL);
    }
    return FALSE;
}

Int32 SQLiteConnectionPool::GetPriority(
    /* [in] */ Int32 connectionFlags)
{
    return (connectionFlags & CONNECTION_FLAG_INTERACTIVE) != 0 ? 1 : 0;
}

void SQLiteConnectionPool::SetMaxConnectionPoolSizeLocked()
{
    if ((mConfiguration->mOpenFlags
            & ISQLiteDatabase::ENABLE_WRITE_AHEAD_LOGGING) != 0) {
        mMaxConnectionPoolSize = SQLiteGlobal::GetWALConnectionPoolSize();
    }
    else {
        // TODO: We don't actually need to restrict the connection pool size to 1
        // for non-WAL databases.  There might be reasons to use connection pooling
        // with other journal modes.  For now, enabling connection pooling and
        // using WAL are the same thing in the API.
        mMaxConnectionPoolSize = 1;
    }
}

ECode SQLiteConnectionPool::ThrowIfClosedLocked()
{
    if (!mIsOpen) {
        Slogger::E(TAG, "Cannot perform this operation because the connection pool has been closed. %p", this);
        assert(0 && "TODO");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

AutoPtr<SQLiteConnectionPool::ConnectionWaiter> SQLiteConnectionPool::ObtainConnectionWaiterLocked(
    /* [in] */ IThread* thread,
    /* [in] */ Int64 startTime,
    /* [in] */ Int32 priority,
    /* [in] */ Boolean wantPrimaryConnection,
    /* [in] */ const String& sql,
    /* [in] */ Int32 connectionFlags)
{
    AutoPtr<ConnectionWaiter> waiter = mConnectionWaiterPool;
    if (waiter != NULL) {
        mConnectionWaiterPool = waiter->mNext;
        waiter->mNext = NULL;
    }
    else {
        waiter = new ConnectionWaiter();
    }
    waiter->mThread = thread;
    waiter->mStartTime = startTime;
    waiter->mPriority = priority;
    waiter->mWantPrimaryConnection = wantPrimaryConnection;
    waiter->mSql = sql;
    waiter->mConnectionFlags = connectionFlags;
    return waiter;
}

void SQLiteConnectionPool::RecycleConnectionWaiterLocked(
    /* [in] */ ConnectionWaiter* waiter)
{
    waiter->mNext = mConnectionWaiterPool;
    waiter->mThread = NULL;
    waiter->mSql = NULL;
    waiter->mAssignedConnection = NULL;
    waiter->mException = NOERROR;
    waiter->mNonce += 1;
    mConnectionWaiterPool = waiter;
}

void SQLiteConnectionPool::Dump(
    /* [in] */ IPrinter* printer,
    /* [in] */ Boolean verbose)
{
    AutoPtr<IPrinter> indentedPrinter;
    //indentedPrinter = PrefixPrinter->Create(printer, "    ");
    {
        AutoLock lock(mLock);
        printer->Println(String("Connection pool for ") + mConfiguration->mPath + String(":"));
        printer->Println(String("  Open: ") + StringUtils::ToString((Int32)mIsOpen));
        printer->Println(String("  Max connections: ") + StringUtils::ToString(mMaxConnectionPoolSize));

        printer->Println(String("  Available primary connection:"));
        if (mAvailablePrimaryConnection != NULL) {
            mAvailablePrimaryConnection->Dump(indentedPrinter, verbose);
        }
        else {
            indentedPrinter->Println(String("<none>"));
        }

        printer->Println(String("  Available non-primary connections:"));
        if (!mAvailableNonPrimaryConnections.IsEmpty()) {
            List<AutoPtr<SQLiteConnection> >::Iterator it = mAvailableNonPrimaryConnections.Begin();
            for (; it != mAvailableNonPrimaryConnections.End(); ++it) {
                (*it)->Dump(indentedPrinter, verbose);
            }
        }
        else {
            indentedPrinter->Println(String("<none>"));
        }

        printer->Println(String("  Acquired connections:"));
        if (!mAcquiredConnections.IsEmpty()) {
            HashMap<AutoPtr<SQLiteConnection>, AcquiredConnectionStatus>::Iterator it;
            it = mAcquiredConnections.Begin();
            for (; it != mAcquiredConnections.End(); ++it) {
                it->mFirst->DumpUnsafe(indentedPrinter, verbose);
                indentedPrinter->Println(String("  Status: ") + StringUtils::ToString(it->mSecond));
            }
        }
        else {
            indentedPrinter->Println(String("<none>"));
        }

        printer->Println(String("  Connection waiters:"));
        if (mConnectionWaiterQueue != NULL) {
            Int32 i = 0;
            Int64 now = SystemClock::GetUptimeMillis();
            for (AutoPtr<ConnectionWaiter> waiter = mConnectionWaiterQueue; waiter != NULL;
                    waiter = waiter->mNext, i++) {
                StringBuilder sb(i);
                sb += ": waited for ";
                sb += ((now - waiter->mStartTime) * 0.001f);
                sb += " ms - thread=";
                sb += Object::ToString(waiter->mThread);
                sb += ", priority=";
                sb += waiter->mPriority;
                sb += ", sql='";
                sb += waiter->mSql;
                sb += "'";
                indentedPrinter->Println(sb.ToString());
            }
        }
        else {
            indentedPrinter->Println(String("<none>"));
        }
    }
}

String SQLiteConnectionPool::ToString()
{
    StringBuilder sb("SQLiteConnectionPool: ");
    sb += mConfiguration->mPath;
    return sb.ToString();
}

} //Sqlite
} //Database
} //Droid
} //Elastos
