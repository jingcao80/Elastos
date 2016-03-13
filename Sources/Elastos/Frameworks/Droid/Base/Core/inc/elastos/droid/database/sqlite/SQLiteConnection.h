
#ifndef __ELASTOS_DROID_DATABASE_SQLITE_SQLITECONNECTION_H__
#define __ELASTOS_DROID_DATABASE_SQLITE_SQLITECONNECTION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/utility/LruCache.h"
#include "elastos/droid/database/sqlite/SQLiteDebug.h"
#include "elastos/droid/database/sqlite/SQLiteStatementInfo.h"
#include "elastos/droid/database/sqlite/SQLiteDatabaseConfiguration.h"
#include <elastos/core/Object.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Utility::IPrinter;
using Elastos::Droid::Utility::LruCache;
using Elastos::Droid::Database::ICursorWindow;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::ICancellationSignalOnCancelListener;
using Elastos::Core::IArrayOf;
using Elastos::Core::ICloseGuard;
using Elastos::Core::StringBuilder;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

class SQLiteConnectionPool;
class SQLiteCustomFunction;

class SQLiteConnection
    : public Object
    , public ICancellationSignalOnCancelListener
{
private:
    /**
     * Holder type for a prepared statement.
     *
     * Although this object holds a pointer to a native statement object, it
     * does not have a finalizer.  This is deliberate.  The {@link SQLiteConnection}
     * owns the statement object and will take care of freeing it when needed.
     * In particular, closing the connection requires a guarantee of deterministic
     * resource disposal because all native statement objects must be freed before
     * the native database object can be closed.  So no finalizers here.
     */
    class PreparedStatement : public Object
    {
    public:
        PreparedStatement();

    public:
        // Next item in pool.
        AutoPtr<PreparedStatement> mPoolNext;

        // The SQL from which the statement was prepared.
        String mSql;

        // The native sqlite3_stmt object pointer.
        // Lifetime is managed explicitly by the connection.
        Int64 mStatementPtr;

        // The number of parameters that the prepared statement has.
        Int32 mNumParameters;

        // The statement type.
        Int32 mType;

        // True if the statement is read-only.
        Boolean mReadOnly;

        // True if the statement is in the cache.
        Boolean mInCache;

        // True if the statement is in use (currently executing).
        // We need this flag because due to the use of custom functions in triggers, it's
        // possible for SQLite calls to be re-entrant.  Consequently we need to prevent
        // in use statements from being finalized until they are no longer in use.
        Boolean mInUse;
    };

    class PreparedStatementCache : public LruCache<String, AutoPtr<PreparedStatement> >
    {
    public:
        PreparedStatementCache(
            /* [in] */ Int32 size,
            /* [in]) */ SQLiteConnection* host);

        CARAPI_(void) Dump(
            /* [in] */ IPrinter* printer);

    protected:
        CARAPI_(void) EntryRemoved(
            /* [in] */ Boolean evicted,
            /* [in] */ const String& key,
            /* [in] */ PreparedStatement* oldValue,
            /* [in] */ PreparedStatement* newValue);

    private:
        SQLiteConnection* mHost;
    };

    class OperationLog;

    class Operation : public Object
    {
    public:
        Operation();

        ~Operation();

        CARAPI_(void) Describe(
            /* [in] */ StringBuilder& msg,
            /* [in] */ Boolean verbose);

    private:
        CARAPI_(String) GetStatus();

        CARAPI_(String) GetFormattedStartTime();

        static AutoPtr<ISimpleDateFormat> GetDateFormat();
    public:
        Int64 mStartTime;
        Int64 mEndTime;
        String mKind;
        String mSql;
        AutoPtr< List<AutoPtr<IInterface> > > mBindArgs;
        Boolean mFinished;
        ECode mException;
        Int32 mCookie;

    private:
        static AutoPtr<ISimpleDateFormat> sDateFormat;

        friend class OperationLog;
    };

    class OperationLog : public Object
    {
    public:
        OperationLog();

        CARAPI_(Int32) BeginOperation(
            /* [in] */ const String& kind,
            /* [in] */ const String& sql,
            /* [in] */ ArrayOf<IInterface*>* bindArgs);

        CARAPI_(void) FailOperation(
            /* [in] */ Int32 cookie,
            /* [in] */ ECode ex);

        CARAPI_(void) EndOperation(
            /* [in] */ Int32 cookie);

        CARAPI_(Boolean) EndOperationDeferLog(
            /* [in] */ Int32 cookie);

        CARAPI_(void) LogOperation(
            /* [in] */ Int32 cookie,
            /* [in] */ const String& detail);

        CARAPI_(String) DescribeCurrentOperation();

        CARAPI_(void) Dump(
            /* [in] */ IPrinter* printer,
            /* [in] */ Boolean verbose);

    private:
        CARAPI_(Boolean) EndOperationDeferLogLocked(
            /* [in] */ Int32 cookie);

        CARAPI_(void) LogOperationLocked(
            /* [in] */ Int32 cookie,
            /* [in] */ const String& detail);

        CARAPI_(Int32) NewOperationCookieLocked(
            /* [in] */ Int32 index);

        CARAPI_(AutoPtr<Operation>) GetOperationLocked(
            /* [in] */ Int32 cookie);

    private:
        static const Int32 MAX_RECENT_OPERATIONS = 20;
        static const Int32 COOKIE_GENERATION_SHIFT = 8;
        static const Int32 COOKIE_INDEX_MASK = 0xff;

        AutoPtr< ArrayOf<Operation*> > mOperations;
        Object mOperationsLock;
        Int32 mIndex;
        Int32 mGeneration;
    };

public:
    ~SQLiteConnection();

    CAR_INTERFACE_DECL()

    // Called by SQLiteConnectionPool only.
    static CARAPI Open(
        /* [in] */ SQLiteConnectionPool* pool,
        /* [in] */ SQLiteDatabaseConfiguration* configuration,
        /* [in] */ Int32 connectionId,
        /* [in] */ Boolean primaryConnection,
        /* [out] */ SQLiteConnection** connect);

    // Called by SQLiteConnectionPool only.
    // Closes the database closes and releases all of its associated resources.
    // Do not call methods on the connection after it is closed.  It will probably crash.
    CARAPI_(void) Close();

    // Called by SQLiteConnectionPool only.
    CARAPI Reconfigure(
        /* [in] */ SQLiteDatabaseConfiguration* configuration);

    // Called by SQLiteConnectionPool only.
    // When set to true, executing write operations will throw SQLiteException.
    // Preparing statements that might write is ok, just don't execute them.
    CARAPI_(void) SetOnlyAllowReadOnlyOperations(
        /* [in] */ Boolean readOnly);

    // Called by SQLiteConnectionPool only.
    // Returns true if the prepared statement cache contains the specified SQL.
    CARAPI_(Boolean) IsPreparedStatementInCache(
        /* [in] */ const String& sql);

    /**
     * Gets the unique id of this connection.
     * @return The connection id.
     */
    CARAPI_(Int32) GetConnectionId();

    /**
     * Returns true if this is the primary database connection.
     * @return True if this is the primary database connection.
     */
    CARAPI_(Boolean) IsPrimaryConnection();

    /**
     * Prepares a statement for execution but does not bind its parameters or execute it.
     * <p>
     * This method can be used to check for syntax errors during compilation
     * prior to execution of the statement.  If the {@code outStatementInfo} argument
     * is not null, the provided {@link SQLiteStatementInfo} object is populated
     * with information about the statement.
     * </p><p>
     * A prepared statement makes no reference to the arguments that may eventually
     * be bound to it, consequently it it possible to cache certain prepared statements
     * such as SELECT or INSERT/UPDATE statements.  If the statement is cacheable,
     * then it will be stored in the cache for later.
     * </p><p>
     * To take advantage of this behavior as an optimization, the connection pool
     * provides a method to acquire a connection that already has a given SQL statement
     * in its prepared statement cache so that it is ready for execution.
     * </p>
     *
     * @param sql The SQL statement to prepare.
     * @param outStatementInfo The {@link SQLiteStatementInfo} object to populate
     * with information about the statement, or null if none.
     *
     * @throws SQLiteException if an error occurs, such as a syntax error.
     */
    CARAPI Prepare(
        /* [in] */ const String& sql,
        /* [in] */ SQLiteStatementInfo* outStatementInfo);

    /**
     * Executes a statement that does not return a result.
     *
     * @param sql The SQL statement to execute.
     * @param bindArgs The arguments to bind, or null if none.
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     *
     * @throws SQLiteException if an error occurs, such as a syntax error
     * or invalid number of bind arguments.
     * @throws OperationCanceledException if the operation was canceled.
     */
    CARAPI Execute(
        /* [in] */ const String& sql,
        /* [in] */ ArrayOf<IInterface*>* bindArgs,
        /* [in] */ ICancellationSignal* cancellationSignal);

    /**
     * Executes a statement that returns a single <code>long</code> result.
     *
     * @param sql The SQL statement to execute.
     * @param bindArgs The arguments to bind, or null if none.
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     * @return The value of the first column in the first row of the result set
     * as a <code>long</code>, or zero if none.
     *
     * @throws SQLiteException if an error occurs, such as a syntax error
     * or invalid number of bind arguments.
     * @throws OperationCanceledException if the operation was canceled.
     */
    CARAPI ExecuteForInt64(
        /* [in] */ const String& sql,
        /* [in] */ ArrayOf<IInterface*>* bindArgs,
        /* [in] */ ICancellationSignal* cancellationSignal,
        /* [out] */ Int64* result);

    /**
     * Executes a statement that returns a single {@link String} result.
     *
     * @param sql The SQL statement to execute.
     * @param bindArgs The arguments to bind, or null if none.
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     * @return The value of the first column in the first row of the result set
     * as a <code>String</code>, or null if none.
     *
     * @throws SQLiteException if an error occurs, such as a syntax error
     * or invalid number of bind arguments.
     * @throws OperationCanceledException if the operation was canceled.
     */
    CARAPI ExecuteForString(
        /* [in] */ const String& sql,
        /* [in] */ ArrayOf<IInterface*>* bindArgs,
        /* [in] */ ICancellationSignal* cancellationSignal,
        /* [out] */ String* str);

    /**
     * Executes a statement that returns a single BLOB result as a
     * file descriptor to a shared memory region.
     *
     * @param sql The SQL statement to execute.
     * @param bindArgs The arguments to bind, or null if none.
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     * @return The file descriptor for a shared memory region that contains
     * the value of the first column in the first row of the result set as a BLOB,
     * or null if none.
     *
     * @throws SQLiteException if an error occurs, such as a syntax error
     * or invalid number of bind arguments.
     * @throws OperationCanceledException if the operation was canceled.
     */
    CARAPI ExecuteForBlobFileDescriptor(
        /* [in] */ const String& sql,
        /* [in] */ ArrayOf<IInterface*>* bindArgs,
        /* [in] */ ICancellationSignal* cancellationSignal,
        /* [out] */ IParcelFileDescriptor** descriptor);

    /**
     * Executes a statement that returns a count of the number of rows
     * that were changed.  Use for UPDATE or DELETE SQL statements.
     *
     * @param sql The SQL statement to execute.
     * @param bindArgs The arguments to bind, or null if none.
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     * @return The number of rows that were changed.
     *
     * @throws SQLiteException if an error occurs, such as a syntax error
     * or invalid number of bind arguments.
     * @throws OperationCanceledException if the operation was canceled.
     */
    CARAPI ExecuteForChangedRowCount(
        /* [in] */ const String& sql,
        /* [in] */ ArrayOf<IInterface*>* bindArgs,
        /* [in] */ ICancellationSignal* cancellationSignal,
        /* [out] */ Int32* count);

    /**
     * Executes a statement that returns the row id of the last row inserted
     * by the statement.  Use for INSERT SQL statements.
     *
     * @param sql The SQL statement to execute.
     * @param bindArgs The arguments to bind, or null if none.
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     * @return The row id of the last row that was inserted, or 0 if none.
     *
     * @throws SQLiteException if an error occurs, such as a syntax error
     * or invalid number of bind arguments.
     * @throws OperationCanceledException if the operation was canceled.
     */
    CARAPI ExecuteForLastInsertedRowId(
        /* [in] */ const String& sql,
        /* [in] */ ArrayOf<IInterface*>* bindArgs,
        /* [in] */ ICancellationSignal* cancellationSignal,
        /* [out] */ Int64* id);

    /**
     * Executes a statement and populates the specified {@link CursorWindow}
     * with a range of results.  Returns the number of rows that were counted
     * during query execution.
     *
     * @param sql The SQL statement to execute.
     * @param bindArgs The arguments to bind, or null if none.
     * @param window The cursor window to clear and fill.
     * @param startPos The start position for filling the window.
     * @param requiredPos The position of a row that MUST be in the window.
     * If it won't fit, then the query should discard part of what it filled
     * so that it does.  Must be greater than or equal to <code>startPos</code>.
     * @param countAllRows True to count all rows that the query would return
     * regagless of whether they fit in the window.
     * @param cancellationSignal A signal to cancel the operation in progress, or null if none.
     * @return The number of rows that were counted during query execution.  Might
     * not be all rows in the result set unless <code>countAllRows</code> is true.
     *
     * @throws SQLiteException if an error occurs, such as a syntax error
     * or invalid number of bind arguments.
     * @throws OperationCanceledException if the operation was canceled.
     */
    CARAPI ExecuteForCursorWindow(
        /* [in] */ const String& sql,
        /* [in] */ ArrayOf<IInterface*>* bindArgs,
        /* [in] */ ICursorWindow* window,
        /* [in] */ Int32 startPos,
        /* [in] */ Int32 requiredPos,
        /* [in] */ Boolean countAllRows,
        /* [in] */ ICancellationSignal* cancellationSignal,
        /* [out] */ Int32* result);

    // CancellationSignal.OnCancelListener callback.
    // This method may be called on a different thread than the executing statement.
    // However, it will only be called between calls to attachCancellationSignal and
    // detachCancellationSignal, while a statement is executing.  We can safely assume
    // that the SQLite connection is still alive.
    //@Override
    CARAPI OnCancel();

    /**
     * Dumps debugging information about this connection.
     *
     * @param printer The printer to receive the dump, not null.
     * @param verbose True to dump more verbose information.
     */
    CARAPI_(void) Dump(
        /* [in] */ IPrinter* printer,
        /* [in] */ Boolean verbose);

    /**
     * Dumps debugging information about this connection, in the case where the
     * caller might not actually own the connection.
     *
     * This function is written so that it may be called by a thread that does not
     * own the connection.  We need to be very careful because the connection state is
     * not synchronized.
     *
     * At worst, the method may return stale or slightly wrong data, however
     * it should not crash.  This is ok as it is only used for diagnostic purposes.
     *
     * @param printer The printer to receive the dump, not null.
     * @param verbose True to dump more verbose information.
     */
    CARAPI_(void) DumpUnsafe(
        /* [in] */ IPrinter* printer,
        /* [in] */ Boolean verbose);

    /**
     * Describes the currently executing operation, in the case where the
     * caller might not actually own the connection.
     *
     * This function is written so that it may be called by a thread that does not
     * own the connection.  We need to be very careful because the connection state is
     * not synchronized.
     *
     * At worst, the method may return stale or slightly wrong data, however
     * it should not crash.  This is ok as it is only used for diagnostic purposes.
     *
     * @return A description of the current operation including how long it has been running,
     * or null if none.
     */
    CARAPI_(String) DescribeCurrentOperationUnsafe();

    /**
     * Collects statistics about database connection memory usage.
     *
     * @param dbStatsList The list to populate.
     */
    CARAPI_(void) CollectDbStats(
        /* [in] */ List<AutoPtr<SQLiteDebug::DbStats> >* dbStatsList);

    /**
     * Collects statistics about database connection memory usage, in the case where the
     * caller might not actually own the connection.
     *
     * @return The statistics object, never null.
     */
    CARAPI_(void) CollectDbStatsUnsafe(
        /* [in] */ List<AutoPtr<SQLiteDebug::DbStats> >* dbStatsList);

    //@Override
    CARAPI_(String) ToString();

private:
    static CARAPI NativeOpen(
        /* [in] */ const String& path,
        /* [in] */ Int32 openFlags,
        /* [in] */ const String& label,
        /* [in] */ Boolean enableTrace,
        /* [in] */ Boolean enableProfile,
        /* [out] */ Int64* result);

    static CARAPI NativeClose(
        /* [in] */ Int64 connectionPtr);

    static CARAPI NativeRegisterCustomFunction(
        /* [in] */ Int64 connectionPtr,
        /* [in] */ SQLiteCustomFunction* function);

    static CARAPI NativeRegisterLocalizedCollators(
        /* [in] */ Int64 connectionPtr,
        /* [in] */ const String& locale);

    static CARAPI NativePrepareStatement(
        /* [in] */ Int64 connectionPtr,
        /* [in] */ const String& sql,
        /* [out] */ Int64* result);

    static CARAPI_(void) NativeFinalizeStatement(
        /* [in] */ Int64 connectionPtr,
        /* [in] */ Int64 statementPtr);

    static CARAPI_(Int32) NativeGetParameterCount(
        /* [in] */ Int64 connectionPtr,
        /* [in] */ Int64 statementPtr);

    static CARAPI_(Boolean) NativeIsReadOnly(
        /* [in] */ Int64 connectionPtr,
        /* [in] */ Int64 statementPtr);

    static CARAPI_(Int32) NativeGetColumnCount(
        /* [in] */ Int64 connectionPtr,
        /* [in] */ Int64 statementPtr);

    static CARAPI_(String) NativeGetColumnName(
        /* [in] */ Int64 connectionPtr,
        /* [in] */ Int64 statementPtr,
        /* [in] */ Int32 index);

    static CARAPI NativeBindNull(
        /* [in] */ Int64 connectionPtr,
        /* [in] */ Int64 statementPtr,
        /* [in] */ Int32 index);

    static CARAPI NativeBindInt64(
        /* [in] */ Int64 connectionPtr,
        /* [in] */ Int64 statementPtr,
        /* [in] */ Int32 index,
        /* [in] */ Int64 value);

    static CARAPI NativeBindDouble(
        /* [in] */ Int64 connectionPtr,
        /* [in] */ Int64 statementPtr,
        /* [in] */ Int32 index,
        /* [in] */ Double value);

    static CARAPI NativeBindString(
        /* [in] */ Int64 connectionPtr,
        /* [in] */ Int64 statementPtr,
        /* [in] */ Int32 index,
        /* [in] */ const String& value);

    static CARAPI NativeBindBlob(
        /* [in] */ Int64 connectionPtr,
        /* [in] */ Int64 statementPtr,
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Byte>* value);

    static CARAPI NativeResetStatementAndClearBindings(
        /* [in] */ Int64 connectionPtr,
        /* [in] */ Int64 statementPtr);

    static CARAPI NativeExecute(
        /* [in] */ Int64 connectionPtr,
        /* [in] */ Int64 statementPtr);

    static CARAPI NativeExecuteForInt64(
        /* [in] */ Int64 connectionPtr,
        /* [in] */ Int64 statementPtr,
        /* [out] */ Int64* result);

    static CARAPI NativeExecuteForString(
        /* [in] */ Int64 connectionPtr,
        /* [in] */ Int64 statementPtr,
        /* [out] */ String* result);

    static CARAPI_(Int32) CreateAshmemRegionWithData(
        /* [in] */ const void* data,
        /* [in] */ Int32 length);

    static CARAPI NativeExecuteForBlobFileDescriptor(
        /* [in] */ Int64 connectionPtr,
        /* [in] */ Int64 statementPtr,
        /* [out] */ Int32* result);

    static CARAPI NativeExecuteForChangedRowCount(
        /* [in] */ Int64 connectionPtr,
        /* [in] */ Int64 statementPtr,
        /* [out] */ Int32* result);

    static CARAPI NativeExecuteForLastInsertedRowId(
        /* [in] */ Int64 connectionPtr,
        /* [in] */ Int64 statementPtr,
        /* [out] */ Int64* result);

    static CARAPI NativeExecuteForCursorWindow(
        /* [in] */ Int64 connectionPtr,
        /* [in] */ Int64 statementPtr,
        /* [in] */ Int32 windowPtr,
        /* [in] */ Int32 startPos,
        /* [in] */ Int32 requiredPos,
        /* [in] */ Boolean countAllRows,
        /* [out] */ Int64* result);

    static CARAPI_(Int32) NativeGetDbLookaside(
        /* [in] */ Int64 connectionPtr);

    static CARAPI_(void) NativeCancel(
        /* [in] */ Int64 connectionPtr);

    static CARAPI_(void) NativeResetCancel(
        /* [in] */ Int64 connectionPtr,
        /* [in] */ Boolean cancelable);

    SQLiteConnection(
        /* [in] */ SQLiteConnectionPool* pool,
        /* [in] */ SQLiteDatabaseConfiguration* configuration,
        /* [in] */ Int32 connectionId,
        /* [in] */ Boolean primaryConnection);

    CARAPI Open();

    CARAPI_(void) Dispose(
        /* [in] */ Boolean finalized);

    CARAPI SetPageSize();

    CARAPI SetAutoCheckpointInterval();

    CARAPI SetJournalSizeLimit();

    CARAPI SetForeignKeyModeFromConfiguration();

    CARAPI SetWalModeFromConfiguration();

    CARAPI SetSyncMode(
        /* [in] */ const String& newValue);

    static CARAPI_(String) CanonicalizeSyncMode(
        /* [in] */ const String& value);

    CARAPI SetJournalMode(
        /* [in] */ const String& newValue);

    CARAPI SetLocaleFromConfiguration();

    CARAPI AcquirePreparedStatement(
        /* [in] */ const String& sql,
        /* [out] */ PreparedStatement** statement);

    CARAPI_(void) ReleasePreparedStatement(
        /* [in] */ PreparedStatement* statement);

    CARAPI_(void) FinalizePreparedStatement(
        /* [in] */ PreparedStatement* statement);

    CARAPI AttachCancellationSignal(
        /* [in] */ ICancellationSignal* cancellationSignal);

    CARAPI_(void) DetachCancellationSignal(
        /* [in] */ ICancellationSignal* cancellationSignal);

    CARAPI BindArguments(
        /* [in] */ PreparedStatement* statement,
        /* [in] */ ArrayOf<IInterface*>* bindArgs);

    CARAPI ThrowIfStatementForbidden(
        /* [in] */ PreparedStatement* statement);

    static CARAPI_(Boolean) IsCacheable(
        /* [in] */ Int32 statementType);

    CARAPI ApplyBlockGuardPolicy(
        /* [in] */ PreparedStatement* statement);

    CARAPI_(AutoPtr<SQLiteDebug::DbStats>) GetMainDbStatsUnsafe(
        /* [in] */ Int32 lookaside,
        /* [in] */ Int64 pageCount,
        /* [in] */ Int64 pageSize);

    CARAPI_(AutoPtr<PreparedStatement>) ObtainPreparedStatement(
        /* [in] */ const String& sql,
        /* [in] */ Int64 statementPtr,
        /* [in] */ Int32 numParameters,
        /* [in] */ Int32 type,
        /* [in] */ Boolean readOnly);

    CARAPI_(void) RecyclePreparedStatement(
        /* [in] */ PreparedStatement* statement);

    static CARAPI_(String) TrimSqlForDisplay(
        /* [in] */ const String& sql);

private:
    static const String TAG;
    static const Boolean DEBUG;

    static const AutoPtr< ArrayOf<String> > EMPTY_STRING_ARRAY;
    static const AutoPtr<IArrayOf> EMPTY_BYTE_ARRAY;

    static AutoPtr<IPattern> TRIM_SQL_PATTERN;

    AutoPtr<ICloseGuard> mCloseGuard;

    AutoPtr<SQLiteConnectionPool> mPool;
    AutoPtr<SQLiteDatabaseConfiguration> mConfiguration;
    Int32 mConnectionId;
    Boolean mIsPrimaryConnection;
    Boolean mIsReadOnlyConnection;
    AutoPtr<PreparedStatementCache> mPreparedStatementCache;
    AutoPtr<PreparedStatement> mPreparedStatementPool;

    // The recent operations log.
    AutoPtr<OperationLog> mRecentOperations;

    // The native SQLiteConnection pointer.  (FOR INTERNAL USE ONLY)
    Int64 mConnectionPtr;

    Boolean mOnlyAllowReadOnlyOperations;

    // The number of times attachCancellationSignal has been called.
    // Because SQLite statement execution can be reentrant, we keep track of how many
    // times we have attempted to attach a cancellation signal to the connection so that
    // we can ensure that we detach the signal at the right time.
    Int32 mCancellationSignalAttachCount;

    friend class PreparedStatementCache;
};

} //Sqlite
} //Database
} //Droid
} //Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Database::Sqlite::SQLiteConnection::Operation, IInterface)

#endif //__ELASTOS_DROID_DATABASE_SQLITE_SQLITECONNECTION_H__
