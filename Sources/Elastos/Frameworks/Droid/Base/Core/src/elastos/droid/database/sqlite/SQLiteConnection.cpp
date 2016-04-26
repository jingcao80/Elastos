
#include <Elastos.CoreLibrary.Text.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/database/sqlite/SQLiteConnection.h"
#include "elastos/droid/database/sqlite/SQLiteConnectionPool.h"
#include "elastos/droid/database/sqlite/SQLiteGlobal.h"
#include "elastos/droid/database/sqlite/SQLiteCustomFunction.h"
#include "elastos/droid/database/sqlite/SQLiteCommon.h"
#include "elastos/droid/database/NativeCursorWindow.h"
#include "elastos/droid/database/DatabaseUtils.h"
#include "elastos/droid/database/CCursorWindow.h"
#include "elastos/droid/os/CParcelFileDescriptor.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>
#include <cutils/ashmem.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <sqlite3.h>
#include <sqlite3_android.h>

using Elastos::Droid::Os::CParcelFileDescriptor;
using Elastos::Droid::Os::EIID_ICancellationSignalOnCancelListener;
using Elastos::Core::IByte;
using Elastos::Core::ICloseGuardHelper;
using Elastos::Core::CCloseGuardHelper;
using Elastos::Core::CString;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::CArrayOf;
using Elastos::Core::INumber;
using Elastos::Core::IBoolean;
using Elastos::Core::EIID_IByte;
using Elastos::Core::StringUtils;
using Elastos::Core::IBlockGuard;
using Elastos::Core::CBlockGuard;
using Elastos::Core::ICharSequence;
using Elastos::Core::IBlockGuardPolicy;
using Elastos::Text::IFormat;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Utility::IDate;
using Elastos::Utility::CDate;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

SQLiteConnection::PreparedStatement::PreparedStatement()
    : mStatementPtr(0)
    , mNumParameters(0)
    , mType(0)
    , mReadOnly(FALSE)
    , mInCache(FALSE)
    , mInUse(FALSE)
{}

SQLiteConnection::PreparedStatementCache::PreparedStatementCache(
    /* [in] */ Int32 size,
    /* [in]) */ SQLiteConnection* host)
    : LruCache<String, AutoPtr<PreparedStatement> >(size)
    , mHost(host)
{}

void SQLiteConnection::PreparedStatementCache::EntryRemoved(
    /* [in] */ Boolean evicted,
    /* [in] */ const String& key,
    /* [in] */ PreparedStatement* oldValue,
    /* [in] */ PreparedStatement* newValue)
{
    oldValue->mInCache = FALSE;
    if (!oldValue->mInUse) {
        mHost->FinalizePreparedStatement(oldValue);
    }
}

void SQLiteConnection::PreparedStatementCache::Dump(
    /* [in] */ IPrinter* printer)
{
    // printer->Println("  Prepared statement cache:");
    // Map<String, PreparedStatement> cache = snapshot();
    // if (!cache.isEmpty()) {
    //     int i = 0;
    //     for (Map.Entry<String, PreparedStatement> entry : cache.entrySet()) {
    //         PreparedStatement statement = entry.getValue();
    //         if (statement.mInCache) { // might be false due to a race with entryRemoved
    //             String sql = entry.getKey();
    //             printer.println("    " + i + ": statementPtr=0x"
    //                     + Integer.toHexString(statement.mStatementPtr)
    //                     + ", numParameters=" + statement.mNumParameters
    //                     + ", type=" + statement.mType
    //                     + ", readOnly=" + statement.mReadOnly
    //                     + ", sql=\"" + trimSqlForDisplay(sql) + "\"");
    //         }
    //         i += 1;
    //     }
    // } else {
    //     printer.println("    <none>");
    // }
}


AutoPtr<ISimpleDateFormat> SQLiteConnection::Operation::sDateFormat;

AutoPtr<ISimpleDateFormat> SQLiteConnection::Operation::GetDateFormat()
{
    if (sDateFormat == NULL) {
        CSimpleDateFormat::New(String("yyyy-MM-dd HH:mm:ss.SSS"), (ISimpleDateFormat**)&sDateFormat);
    }
    return sDateFormat;
}

SQLiteConnection::Operation::Operation()
    : mStartTime(0)
    , mEndTime(0)
    , mFinished(FALSE)
    , mCookie(0)
{}

SQLiteConnection::Operation::~Operation()
{
}

void SQLiteConnection::Operation::Describe(
    /* [in] */ StringBuilder& msg,
    /* [in] */ Boolean verbose)
{
    msg += mKind;
    if (mFinished) {
        msg += " took ";
        msg.Append(mEndTime - mStartTime);
        msg += "ms";
    }
    else {
        msg += " started ";
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int64 start;
        system->GetCurrentTimeMillis(&start);
        msg.Append(start - mStartTime);
        msg += "ms ago";
    }
    msg += " - ";
    msg.Append(GetStatus());
    if (mSql != NULL) {
        msg += ", sql=\"";
        msg.Append(TrimSqlForDisplay(mSql));
        msg += "\"";
    }
    if (verbose && mBindArgs->IsEmpty() && mBindArgs->IsEmpty() == FALSE) {
        msg += ", bindArgs=[";
        Int32 count = mBindArgs->GetSize();
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> arg = (*mBindArgs)[i];
            if (i != 0) {
                msg += ", ";
            }
            if (arg == NULL) {
                msg += "NULL";
            }
            else if (IArrayOf::Probe(arg) != NULL) {
                msg += "ArrayOf<Byte>*";
            }
            else if ((ICharSequence::Probe(arg) != NULL)) {
                msg += "\"";
                String str;
                AutoPtr<ICharSequence> cs = ICharSequence::Probe(arg);
                cs->ToString(&str);
                msg += str;
                msg += "\"";
            }
            else {
                msg += arg;
            }
        }
        msg += "]";
    }
    if (mException != NOERROR) {
        msg += ", exception=\"";
        msg += (Int32)mException;
        msg += "\"";
    }
}

String SQLiteConnection::Operation::GetStatus()
{
    if (!mFinished) {
        return String("running");
    }
    return mException != NOERROR ? String("failed") : String("succeeded");
}

String SQLiteConnection::Operation::GetFormattedStartTime()
{
    AutoPtr<IDate> date;
    CDate::New(mStartTime, (IDate**)&date);
    String str;
    IFormat::Probe(GetDateFormat())->Format(date, &str);
    return str;
}


const Int32 SQLiteConnection::OperationLog::MAX_RECENT_OPERATIONS;
const Int32 SQLiteConnection::OperationLog::COOKIE_GENERATION_SHIFT;
const Int32 SQLiteConnection::OperationLog::COOKIE_INDEX_MASK;

SQLiteConnection::OperationLog::OperationLog()
    : mIndex(0)
    , mGeneration(0)
{
    mOperations = ArrayOf<Operation*>::Alloc(MAX_RECENT_OPERATIONS);
}

Int32 SQLiteConnection::OperationLog::BeginOperation(
    /* [in] */ const String& kind,
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<IInterface*>* bindArgs)
{
    AutoPtr<Operation> operation;
    synchronized(mOperationsLock) {
        Int32 index = (mIndex + 1) % MAX_RECENT_OPERATIONS;
        operation = (*mOperations)[index];
        if (operation == NULL) {
            operation = new Operation();
            mOperations->Set(index, operation);
        }
        else {
            operation->mFinished = FALSE;
            operation->mException = NOERROR;
            if (operation->mBindArgs != NULL) {
                operation->mBindArgs->Clear();
            }
        }

        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&operation->mStartTime);
        operation->mKind = kind;
        operation->mSql = sql;
        if (bindArgs != NULL) {
            if (operation->mBindArgs == NULL) {
                operation->mBindArgs = new List<AutoPtr<IInterface> >();
            }
            else {
                operation->mBindArgs->Clear();
            }
            for (Int32 i = 0; i < bindArgs->GetLength(); i++) {
                AutoPtr<IInterface> arg = (*bindArgs)[i];
                if (arg != NULL && IArrayOf::Probe(arg) != NULL) {
                    // Don't hold onto the real byte array longer than necessary.
                    //operation->mBindArgs->PushBack(EMPTY_BYTE_ARRAY);
                    operation->mBindArgs->PushBack(SQLiteConnection::EMPTY_BYTE_ARRAY);
                }
                else {
                    operation->mBindArgs->PushBack(arg);
                }
            }
        }
        operation->mCookie = NewOperationCookieLocked(index);
        mIndex = index;
    }
    return operation->mCookie;
}

void SQLiteConnection::OperationLog::FailOperation(
    /* [in] */ Int32 cookie,
    /* [in] */ ECode ec)
{
    synchronized(mOperationsLock) {
        AutoPtr<Operation> operation = GetOperationLocked(cookie);
        if (operation != NULL) {
            operation->mException = ec;
        }
    }
}

void SQLiteConnection::OperationLog::EndOperation(
    /* [in] */ Int32 cookie)
{
    synchronized(mOperationsLock) {
        if (EndOperationDeferLogLocked(cookie)) {
            LogOperationLocked(cookie, String(NULL));
        }
    }
}

Boolean SQLiteConnection::OperationLog::EndOperationDeferLog(
    /* [in] */ Int32 cookie)
{
    Boolean ret = FALSE;
    synchronized(mOperationsLock) {
        ret = EndOperationDeferLogLocked(cookie);
    }
    return ret;
}

void SQLiteConnection::OperationLog::LogOperation(
    /* [in] */ Int32 cookie,
    /* [in] */ const String& detail)
{
    synchronized(mOperationsLock) {
        LogOperationLocked(cookie, detail);
    }
}

Boolean SQLiteConnection::OperationLog::EndOperationDeferLogLocked(
    /* [in] */ Int32 cookie)
{
    AutoPtr<Operation> operation = GetOperationLocked(cookie);
    if (operation != NULL) {
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&operation->mEndTime);
        operation->mFinished = TRUE;
        return SQLiteDebug::DEBUG_LOG_SLOW_QUERIES && SQLiteDebug::ShouldLogSlowQuery(operation->mEndTime - operation->mStartTime);
    }
    return FALSE;
}

void SQLiteConnection::OperationLog::LogOperationLocked(
    /* [in] */ Int32 cookie,
    /* [in] */ const String& detail)
{
    AutoPtr<Operation> operation = GetOperationLocked(cookie);
    StringBuilder msg;
    operation->Describe(msg, FALSE);
    if (detail != NULL) {
        msg += ", ";
        msg += detail;
    }
    Slogger::D(TAG, "%s", msg.ToString().string());
}

Int32 SQLiteConnection::OperationLog::NewOperationCookieLocked(
    /* [in] */ Int32 index)
{
    Int32 generation = mGeneration++;
    return generation << COOKIE_GENERATION_SHIFT | index;
}

AutoPtr<SQLiteConnection::Operation> SQLiteConnection::OperationLog::GetOperationLocked(
    /* [in] */ Int32 cookie)
{
    Int32 index = cookie & COOKIE_INDEX_MASK;
    AutoPtr<Operation> operation = (*mOperations)[index];
    return operation->mCookie == cookie ? operation : NULL;
}

String SQLiteConnection::OperationLog::DescribeCurrentOperation()
{
    synchronized(mOperationsLock) {
        AutoPtr<Operation> operation = (*mOperations)[mIndex];
        if (operation != NULL && !operation->mFinished) {
            StringBuilder msg;
            operation->Describe(msg, FALSE);
            return msg.ToString();
        }
    }
    return String(NULL);
}

void SQLiteConnection::OperationLog::Dump(
    /* [in] */ IPrinter* printer,
    /* [in] */ Boolean verbose)
{
    synchronized(mOperationsLock) {
        printer->Println(String("  Most recently executed operations:"));
        Int32 index = mIndex;
        AutoPtr<Operation> operation = (*mOperations)[mIndex];
        if (operation != NULL) {
            Int32 n = 0;
            do {
                StringBuilder msg("    ");
                msg += n;
                msg += ": [";
                msg.Append(operation->GetFormattedStartTime());
                msg += "] ";
                operation->Describe(msg, verbose);
                printer->Println(msg.ToString());

                if (index > 0) {
                    index -= 1;
                }
                else {
                    index = MAX_RECENT_OPERATIONS - 1;
                }
                n += 1;
                operation = (*mOperations)[index];
            } while (operation != NULL && n < MAX_RECENT_OPERATIONS);
        }
        else {
            printer->Println(String("    <none>"));
        }
    }
}


// Set to 1 to use UTF16 storage for localized indexes.
#define UTF16_STORAGE 0

/* Busy timeout in milliseconds.
 * If another connection (possibly in another process) has the database locked for
 * longer than this amount of time then SQLite will generate a SQLITE_BUSY error.
 * The SQLITE_BUSY error is then raised as a SQLiteDatabaseLockedException.
 *
 * In ordinary usage, busy timeouts are quite rare.  Most databases only ever
 * have a single open connection at a time unless they are using WAL.  When using
 * WAL, a timeout could occur if one connection is busy performing an auto-checkpoint
 * operation.  The busy timeout needs to be long enough to tolerate slow I/O write
 * operations but not so long as to cause the application to hang indefinitely if
 * there is a problem acquiring a database lock.
 */
static const int BUSY_TIMEOUT_MS = 2500;

struct SQLiteConnectionNative {
    // Open flags.
    // Must be kept in sync with the constants defined in SQLiteDatabase.java.
    enum {
        OPEN_READWRITE          = 0x00000000,
        OPEN_READONLY           = 0x00000001,
        OPEN_READ_MASK          = 0x00000001,
        NO_LOCALIZED_COLLATORS  = 0x00000010,
        CREATE_IF_NECESSARY     = 0x10000000,
    };

    sqlite3* const db;
    const int openFlags;
    const String path;
    const String label;

    volatile bool canceled;

    SQLiteConnectionNative(sqlite3* db, int openFlags, const String& path, const String& label) :
        db(db), openFlags(openFlags), path(path), label(label), canceled(false) { }
};

const String SQLiteConnection::TAG("SQLiteConnection");
const Boolean SQLiteConnection::DEBUG = FALSE;
const AutoPtr< ArrayOf<String> > SQLiteConnection::EMPTY_STRING_ARRAY = ArrayOf<String>::Alloc(0);

static AutoPtr<IArrayOf> InitEmptyByteArray()
{
    AutoPtr<IArrayOf> array;
    CArrayOf::New(EIID_IByte, 0, (IArrayOf**)&array);
    return array;
}
const AutoPtr<IArrayOf> SQLiteConnection::EMPTY_BYTE_ARRAY = InitEmptyByteArray();

static AutoPtr<IPattern> InitTrimSQLPattern()
{
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    AutoPtr<IPattern> pattern;
    helper->Compile(String("[\\s]*\\n+[\\s]*"), (IPattern**)&pattern);
    return pattern;
}
AutoPtr<IPattern> SQLiteConnection::TRIM_SQL_PATTERN = InitTrimSQLPattern();

// Called each time a statement begins execution, when tracing is enabled.
static void SqliteTraceCallback(
    /* [in] */ void *data,
    /* [in] */ const char *sql)
{
    SQLiteConnectionNative* connection = static_cast<SQLiteConnectionNative*>(data);
    ALOG(LOG_VERBOSE, SQLITE_TRACE_TAG, "%s: \"%s\"\n", connection->label.string(), sql);
}

// Called each time a statement finishes execution, when profiling is enabled.
static void SqliteProfileCallback(
    /* [in] */ void *data,
    /* [in] */ const char *sql,
    /* [in] */ sqlite3_uint64 tm)
{
    SQLiteConnectionNative* connection = static_cast<SQLiteConnectionNative*>(data);
    ALOG(LOG_VERBOSE, SQLITE_PROFILE_TAG, "%s: \"%s\" took %0.3f ms\n", connection->label.string(), sql, tm * 0.000001f);
}

// Called after each SQLite VM instruction when cancelation is enabled.
static Int32 SqliteProgressHandlerCallback(
    /* [in] */ void* data)
{
    SQLiteConnectionNative* connection = static_cast<SQLiteConnectionNative*>(data);
    return connection->canceled;
}

ECode SQLiteConnection::NativeOpen(
    /* [in] */ const String& path,
    /* [in] */ Int32 openFlags,
    /* [in] */ const String& label,
    /* [in] */ Boolean enableTrace,
    /* [in] */ Boolean enableProfile,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Int32 sqliteFlags;
    if (openFlags & SQLiteConnectionNative::CREATE_IF_NECESSARY) {
        sqliteFlags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    }
    else if (openFlags & SQLiteConnectionNative::OPEN_READONLY) {
        sqliteFlags = SQLITE_OPEN_READONLY;
    }
    else {
        sqliteFlags = SQLITE_OPEN_READWRITE;
    }

    sqlite3* db;
    Int32 err = sqlite3_open_v2(path.string(), &db, sqliteFlags, NULL);
    if (err != SQLITE_OK) {
        *result = 0;
        return throw_sqlite3_exception_errcode(err, "Could not open database");
    }

    // Check that the database is really read/write when that is what we asked for.
    if ((sqliteFlags & SQLITE_OPEN_READWRITE) && sqlite3_db_readonly(db, NULL)) {
        *result = 0;
        sqlite3_close(db);
        return throw_sqlite3_exception(db, "Could not open the database in read/write mode.");
    }

    // Set the default busy handler to retry automatically before returning SQLITE_BUSY.
    err = sqlite3_busy_timeout(db, BUSY_TIMEOUT_MS);
    if (err != SQLITE_OK) {
        sqlite3_close(db);
        *result = 0;
        return throw_sqlite3_exception(db, "Could not set busy timeout");
    }

    // Register custom Android functions.
    err = register_android_functions(db, UTF16_STORAGE);
    if (err) {
        *result = 0;
        sqlite3_close(db);
        return throw_sqlite3_exception(db, "Could not register Android SQL functions.");
    }

    // Create wrapper object.
    SQLiteConnectionNative* connection = new SQLiteConnectionNative(db, openFlags, path, label);

    // Enable tracing and profiling if requested.
    if (enableTrace) {
        sqlite3_trace(db, &SqliteTraceCallback, connection);
    }
    if (enableProfile) {
        sqlite3_profile(db, &SqliteProfileCallback, connection);
    }

    Slogger::I(TAG, "Opened connection %s: %p with label '%s'", path.string(), db, label.string());
    *result = reinterpret_cast<Int64>(connection);
    return NOERROR;
}

ECode SQLiteConnection::NativeClose(
    /* [in] */ Int64 connectionPtr)
{
    SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);

    if (connection) {
        ALOGV("Closing connection %p", connection->db);
        int err = sqlite3_close(connection->db);
        if (err != SQLITE_OK) {
            // This can happen if sub-objects aren't closed first.  Make sure the caller knows.
            ALOGE("sqlite3_close(%p) failed: %d", connection->db, err);
            return throw_sqlite3_exception(connection->db, "Count not close db.");
        }

        delete connection;
    }
    return NOERROR;
}

// Called each time a custom function is evaluated.
static void sqliteCustomFunctionCallback(sqlite3_context *context,
        int argc, sqlite3_value **argv)
{
    // Get the callback function object.
    // Create a new local reference to it in case the callback tries to do something
    // dumb like unregister the function (thereby destroying the global ref) while it is running.
    AutoPtr<SQLiteCustomFunction> cunstomFunction = reinterpret_cast<SQLiteCustomFunction*>(sqlite3_user_data(context));

    AutoPtr< ArrayOf<String> > argsArray = ArrayOf<String>::Alloc(argc);
    if (argsArray) {
        for (int i = 0; i < argc; i++) {
            const char* arg = static_cast<const char*>(sqlite3_value_text16(argv[i]));
            if (!arg) {
                ALOGW("NULL argument in custom_function_callback.  This should not happen.");
            }
            else {
                size_t argLen = sqlite3_value_bytes16(argv[i]) / sizeof(char);
                String argStr(arg, argLen);
                (*argsArray)[i] = argStr;
            }
        }

        cunstomFunction->DispatchCallback(argsArray);
    }
}

ECode SQLiteConnection::NativeRegisterCustomFunction(
    /* [in] */ Int64 connectionPtr,
    /* [in] */ SQLiteCustomFunction* function)
{
    SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);
    const char* name = function->mName;
    Int32 numArgs = function->mNumArgs;
    Int32 err = sqlite3_create_function_v2(connection->db, name, numArgs, SQLITE_UTF16,
            reinterpret_cast<void*>(function),
            &sqliteCustomFunctionCallback, NULL, NULL, NULL);

    if (err != SQLITE_OK) {
        ALOGE("sqlite3_create_function returned %d", err);
        return throw_sqlite3_exception(connection->db);
    }
    return NOERROR;
}

ECode SQLiteConnection::NativeRegisterLocalizedCollators(
    /* [in] */ Int64 connectionPtr,
    /* [in] */ const String& locale)
{
    SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);

    Int32 err = register_localized_collators(connection->db, locale, UTF16_STORAGE);

    if (err != SQLITE_OK) {
        return throw_sqlite3_exception(connection->db);
    }
    return NOERROR;
}

ECode SQLiteConnection::NativePrepareStatement(
    /* [in] */ Int64 connectionPtr,
    /* [in] */ const String& sql,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);
    Int32 sqlLength = sql.GetByteLength();
    sqlite3_stmt* statement;
    Int32 err = sqlite3_prepare_v2(connection->db, sql.string(), sqlLength, &statement, NULL);
    if (err != SQLITE_OK) {
        // Error messages like 'near ")": syntax error' are not
        // always helpful enough, so construct an error string that
        // includes the query itself.
        char *message = (char*) malloc(sqlLength + 50);
        if (message) {
            strcpy(message, ", while compiling: "); // less than 50 chars
            strcat(message, sql.string());
        }
        ECode ec = throw_sqlite3_exception(connection->db, message);
        free(message);
        *result = 0;
        return ec;
    }

    // ALOGV("Prepared statement %p on connection %p", statement, connection->db);
    *result = reinterpret_cast<Int32>(statement);
    return NOERROR;
}

void SQLiteConnection::NativeFinalizeStatement(
    /* [in] */ Int64 connectionPtr,
    /* [in] */ Int64 statementPtr)
{
    // SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);
    sqlite3_stmt* statement = reinterpret_cast<sqlite3_stmt*>(statementPtr);

    // We ignore the result of sqlite3_finalize because it is really telling us about
    // whether any errors occurred while executing the statement.  The statement itself
    // is always finalized regardless.
    // ALOGV("Finalized statement %p on connection %p", statement, connection->db);
    sqlite3_finalize(statement);
}

Int32 SQLiteConnection::NativeGetParameterCount(
    /* [in] */ Int64 connectionPtr,
    /* [in] */ Int64 statementPtr)
{
    // SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);
    sqlite3_stmt* statement = reinterpret_cast<sqlite3_stmt*>(statementPtr);

    return sqlite3_bind_parameter_count(statement);
}

Boolean SQLiteConnection::NativeIsReadOnly(
    /* [in] */ Int64 connectionPtr,
    /* [in] */ Int64 statementPtr)
{
    // SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);
    sqlite3_stmt* statement = reinterpret_cast<sqlite3_stmt*>(statementPtr);

    return sqlite3_stmt_readonly(statement) != 0;
}

Int32 SQLiteConnection::NativeGetColumnCount(
    /* [in] */ Int64 connectionPtr,
    /* [in] */ Int64 statementPtr)
{
    // SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);
    sqlite3_stmt* statement = reinterpret_cast<sqlite3_stmt*>(statementPtr);

    return sqlite3_column_count(statement);
}

String SQLiteConnection::NativeGetColumnName(
    /* [in] */ Int64 connectionPtr,
    /* [in] */ Int64 statementPtr,
    /* [in] */ Int32 index)
{
    // SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);
    sqlite3_stmt* statement = reinterpret_cast<sqlite3_stmt*>(statementPtr);
    const char* name = static_cast<const char*>(sqlite3_column_name(statement, index));
    return String(name);
}

ECode SQLiteConnection::NativeBindNull(
    /* [in] */ Int64 connectionPtr,
    /* [in] */ Int64 statementPtr,
    /* [in] */ Int32 index)
{
    SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);
    sqlite3_stmt* statement = reinterpret_cast<sqlite3_stmt*>(statementPtr);

    Int32 err = sqlite3_bind_null(statement, index);
    if (err != SQLITE_OK) {
        return throw_sqlite3_exception(connection->db, NULL);
    }
    return NOERROR;
}

ECode SQLiteConnection::NativeBindInt64(
    /* [in] */ Int64 connectionPtr,
    /* [in] */ Int64 statementPtr,
    /* [in] */ Int32 index,
    /* [in] */ Int64 value)
{
    SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);
    sqlite3_stmt* statement = reinterpret_cast<sqlite3_stmt*>(statementPtr);

    Int32 err = sqlite3_bind_int64(statement, index, value);
    if (err != SQLITE_OK) {
        return throw_sqlite3_exception(connection->db, NULL);
    }
    return NOERROR;
}

ECode SQLiteConnection::NativeBindDouble(
    /* [in] */ Int64 connectionPtr,
    /* [in] */ Int64 statementPtr,
    /* [in] */ Int32 index,
    /* [in] */ Double value)
{
    SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);
    sqlite3_stmt* statement = reinterpret_cast<sqlite3_stmt*>(statementPtr);

    Int32 err = sqlite3_bind_double(statement, index, value);
    if (err != SQLITE_OK) {
        return throw_sqlite3_exception(connection->db, NULL);
    }
    return NOERROR;
}

ECode SQLiteConnection::NativeBindString(
    /* [in] */ Int64 connectionPtr,
    /* [in] */ Int64 statementPtr,
    /* [in] */ Int32 index,
    /* [in] */ const String& value)
{
    SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);
    sqlite3_stmt* statement = reinterpret_cast<sqlite3_stmt*>(statementPtr);

    Int32 valueLength = value.GetByteLength();
    Int32 err = sqlite3_bind_text(statement, index, value, valueLength * sizeof(char),
            SQLITE_TRANSIENT);
    if (err != SQLITE_OK) {
        return throw_sqlite3_exception(connection->db, NULL);
    }
    return NOERROR;
}

ECode SQLiteConnection::NativeBindBlob(
    /* [in] */ Int64 connectionPtr,
    /* [in] */ Int64 statementPtr,
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Byte>* value)
{
    SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);
    sqlite3_stmt* statement = reinterpret_cast<sqlite3_stmt*>(statementPtr);

    Int32 valueLength = value->GetLength();
    //jbyte* value = static_cast<jbyte*>(env->GetPrimitiveArrayCritical(valueArray, NULL));
    int err = sqlite3_bind_blob(statement, index, value, valueLength, SQLITE_TRANSIENT);
    if (err != SQLITE_OK) {
        return throw_sqlite3_exception(connection->db, NULL);
    }

    return NOERROR;
}

ECode SQLiteConnection::NativeResetStatementAndClearBindings(
    /* [in] */ Int64 connectionPtr,
    /* [in] */ Int64 statementPtr)
{
    SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);
    sqlite3_stmt* statement = reinterpret_cast<sqlite3_stmt*>(statementPtr);

    Int32 err = sqlite3_reset(statement);
    if (err == SQLITE_OK) {
        err = sqlite3_clear_bindings(statement);
    }
    if (err != SQLITE_OK) {
        return throw_sqlite3_exception(connection->db, NULL);
    }

    return NOERROR;
}

static ECode ExecuteNonQuery(
    /* [in] */ SQLiteConnectionNative* connection,
    /* [in] */ sqlite3_stmt* statement,
    /* [out] */ Int32* err)
{
    VALIDATE_NOT_NULL(err)

    *err = sqlite3_step(statement);
    if (*err == SQLITE_ROW) {
        return throw_sqlite3_exception(
               "Queries can be performed using SQLiteDatabase query or rawQuery methods only.");
    }
    else if (*err != SQLITE_DONE) {
        return throw_sqlite3_exception(connection->db);
    }
    return NOERROR;
}

ECode SQLiteConnection::NativeExecute(
    /* [in] */ Int64 connectionPtr,
    /* [in] */ Int64 statementPtr)
{
    SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);
    sqlite3_stmt* statement = reinterpret_cast<sqlite3_stmt*>(statementPtr);
    Int32 err;
    return ExecuteNonQuery(connection, statement, &err);
}

ECode SQLiteConnection::NativeExecuteForChangedRowCount(
    /* [in] */ Int64 connectionPtr,
    /* [in] */ Int64 statementPtr,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);
    sqlite3_stmt* statement = reinterpret_cast<sqlite3_stmt*>(statementPtr);

    Int32 err;
    ECode ec = ExecuteNonQuery(connection, statement, &err);
    *result = err == SQLITE_DONE ? sqlite3_changes(connection->db) : -1;
    return ec;
}

ECode SQLiteConnection::NativeExecuteForLastInsertedRowId(
    /* [in] */ Int64 connectionPtr,
    /* [in] */ Int64 statementPtr,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);
    sqlite3_stmt* statement = reinterpret_cast<sqlite3_stmt*>(statementPtr);

    Int32 err;
    ECode ec = ExecuteNonQuery(connection, statement, &err);
    *result = err == SQLITE_DONE && sqlite3_changes(connection->db) > 0
            ? sqlite3_last_insert_rowid(connection->db) : -1;
    return ec;
}

static ECode ExecuteOneRowQuery(
    /* [in] */ SQLiteConnectionNative* connection,
    /* [in] */ sqlite3_stmt* statement,
    /* [out] */ Int32* err)
{
    VALIDATE_NOT_NULL(err)
    *err = sqlite3_step(statement);
    if (*err != SQLITE_ROW) {
        return throw_sqlite3_exception(connection->db);
    }
    return NOERROR;
}

ECode SQLiteConnection::NativeExecuteForInt64(
    /* [in] */ Int64 connectionPtr,
    /* [in] */ Int64 statementPtr,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);
    sqlite3_stmt* statement = reinterpret_cast<sqlite3_stmt*>(statementPtr);

    Int32 err;
    ECode ec = ExecuteOneRowQuery(connection, statement, &err);
    if (err == SQLITE_ROW && sqlite3_column_count(statement) >= 1) {
        *result = sqlite3_column_int64(statement, 0);
        return ec;
    }
    *result = -1;
    return ec;
}

ECode SQLiteConnection::NativeExecuteForString(
    /* [in] */ Int64 connectionPtr,
    /* [in] */ Int64 statementPtr,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);
    sqlite3_stmt* statement = reinterpret_cast<sqlite3_stmt*>(statementPtr);

    Int32 err;
    ECode ec = ExecuteOneRowQuery(connection, statement, &err);
    if (err == SQLITE_ROW && sqlite3_column_count(statement) >= 1) {
        const char* text = reinterpret_cast<const char*>(sqlite3_column_text(statement, 0));
        *result = String(text);
        return ec;
    }
    *result = NULL;
    return ec;
}

Int32 SQLiteConnection::CreateAshmemRegionWithData(
    /* [in] */ const void* data,
    /* [in] */ Int32 length)
{
    Int32 error = 0;
    Int32 fd = ashmem_create_region(NULL, length);
    if (fd < 0) {
        error = errno;
        ALOGE("ashmem_create_region failed: %s", strerror(error));
    }
    else {
        if (length > 0) {
            void* ptr = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if (ptr == MAP_FAILED) {
                error = errno;
                ALOGE("mmap failed: %s", strerror(error));
            }
            else {
                memcpy(ptr, data, length);
                munmap(ptr, length);
            }
        }

        if (!error) {
            if (ashmem_set_prot_region(fd, PROT_READ) < 0) {
                error = errno;
                ALOGE("ashmem_set_prot_region failed: %s", strerror(errno));
            }
            else {
                return fd;
            }
        }

        close(fd);
    }

    //jniThrowIOException(env, error);
    return -1;
}

ECode SQLiteConnection::NativeExecuteForBlobFileDescriptor(
    /* [in] */ Int64 connectionPtr,
    /* [in] */ Int64 statementPtr,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);
    sqlite3_stmt* statement = reinterpret_cast<sqlite3_stmt*>(statementPtr);

    Int32 err;
    ECode ec = ExecuteOneRowQuery(connection, statement, &err);
    if (err == SQLITE_ROW && sqlite3_column_count(statement) >= 1) {
        const void* blob = sqlite3_column_blob(statement, 0);
        if (blob) {
            Int32 length = sqlite3_column_bytes(statement, 0);
            if (length >= 0) {
                *result = CreateAshmemRegionWithData(blob, length);
                return ec;
            }
        }
    }
    *result = -1;
    return ec;
}

enum CopyRowResult {
    CPR_OK,
    CPR_FULL,
    CPR_ERROR,
};

static ECode CopyRow(
    /* [in] */ NativeCursorWindow* window,
    /* [in] */ sqlite3_stmt* statement,
    /* [in] */ Int32 numColumns,
    /* [in] */ Int32 startPos,
    /* [in] */ Int32 addedRows,
    /* [out] */ CopyRowResult* rowResult)
{
    VALIDATE_NOT_NULL(rowResult)
    // Allocate a new field directory for the row.
    ECode ec = NOERROR;
    android::status_t status = window->allocRow();
    if (status) {
        ALOGE("Failed allocating fieldDir at startPos %d row %d, error=%d",
                startPos, addedRows, status);
        *rowResult = CPR_FULL;
        return ec;
    }

    // Pack the row into the window.
    CopyRowResult result = CPR_OK;
    for (Int32 i = 0; i < numColumns; i++) {
        int type = sqlite3_column_type(statement, i);
        if (type == SQLITE_TEXT) {
            // TEXT data
            const char* text = reinterpret_cast<const char*>(
                    sqlite3_column_text(statement, i));
            // SQLite does not include the NULL terminator in size, but does
            // ensure all strings are NULL terminated, so increase size by
            // one to make sure we store the terminator.
            size_t sizeIncludingNull = sqlite3_column_bytes(statement, i) + 1;
            status = window->putString(addedRows, i, text, sizeIncludingNull);
            if (status) {
                ALOGE("Failed allocating %u bytes for text at %d,%d, error=%d",
                        sizeIncludingNull, startPos + addedRows, i, status);
                result = CPR_FULL;
                break;
            }
            // ALOGW("%d,%d is TEXT with %u bytes", startPos + addedRows, i, sizeIncludingNull);
        }
        else if (type == SQLITE_INTEGER) {
            // INTEGER data
            int64_t value = sqlite3_column_int64(statement, i);
            status = window->putLong(addedRows, i, value);
            if (status) {
                ALOGE("Failed allocating space for a long in column %d, error=%d", i, status);
                result = CPR_FULL;
                break;
            }
            ALOGW("%d,%d is INTEGER 0x%016llx", startPos + addedRows, i, value);
        }
        else if (type == SQLITE_FLOAT) {
            // FLOAT data
            double value = sqlite3_column_double(statement, i);
            status = window->putDouble(addedRows, i, value);
            if (status) {
                ALOGE("Failed allocating space for a double in column %d, error=%d",
                        i, status);
                result = CPR_FULL;
                break;
            }
            ALOGW("%d,%d is FLOAT %lf", startPos + addedRows, i, value);
        }
        else if (type == SQLITE_BLOB) {
            // BLOB data
            const void* blob = sqlite3_column_blob(statement, i);
            size_t size = sqlite3_column_bytes(statement, i);
            status = window->putBlob(addedRows, i, blob, size);
            if (status) {
                ALOGE("Failed allocating %u bytes for blob at %d,%d, error=%d",
                        size, startPos + addedRows, i, status);
                result = CPR_FULL;
                break;
            }
            ALOGW("%d,%d is Blob with %u bytes", startPos + addedRows, i, size);
        }
        else if (type == SQLITE_NULL) {
            // NULL field
            status = window->putNull(addedRows, i);
            if (status) {
                ALOGE("Failed allocating space for a null in column %d, error=%d", i, status);
                result = CPR_FULL;
                break;
            }

            ALOGE("%d,%d is NULL", startPos + addedRows, i);
        }
        else {
            // Unknown data
            ALOGE("Unknown column type when filling database window");
            result = CPR_ERROR;
            ec = throw_sqlite3_exception("Unknown column type when filling window");
        }
    }

    // Free the last row if if was not successfully copied.
    if (result != CPR_OK) {
        window->freeLastRow();
    }
    *rowResult = result;
    return ec;
}

ECode SQLiteConnection::NativeExecuteForCursorWindow(
    /* [in] */ Int64 connectionPtr,
    /* [in] */ Int64 statementPtr,
    /* [in] */ Int32 windowPtr,
    /* [in] */ Int32 startPos,
    /* [in] */ Int32 requiredPos,
    /* [in] */ Boolean countAllRows,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    ECode ec = NOERROR;
    SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);
    sqlite3_stmt* statement = reinterpret_cast<sqlite3_stmt*>(statementPtr);
    NativeCursorWindow* window = reinterpret_cast<NativeCursorWindow*>(windowPtr);

    android::status_t status = window->clear();
    if (status) {
        StringBuilder msg("Failed to clear the cursor window, status=");
        msg += status;
        *result = 0;
        return throw_sqlite3_exception(connection->db, msg.ToString().string());
    }

    Int32 numColumns = sqlite3_column_count(statement);
    status = window->setNumColumns(numColumns);
    if (status) {
        StringBuilder msg("Failed to set the cursor window column count to");
        msg += numColumns;
        msg += ", status=";
        msg += status;
        *result = 0;
        return throw_sqlite3_exception(connection->db, msg.ToString().string());
    }
    Int32 retryCount = 0;
    Int32 totalRows = 0;
    Int32 addedRows = 0;
    Boolean windowFull = FALSE;
    Boolean gotException = FALSE;
    while (!gotException && (!windowFull || countAllRows)) {
        Int32 err = sqlite3_step(statement);
        if (err == SQLITE_ROW) {
            // ALOGW("Stepped statement %p to row %d", statement, totalRows);
            retryCount = 0;
            totalRows += 1;

            // Skip the row if the window is full or we haven't reached the start position yet.
            if (startPos >= totalRows || windowFull) {
                continue;
            }
            CopyRowResult cpr;
            FAIL_RETURN(CopyRow(window, statement, numColumns, startPos, addedRows, &cpr));
            if (cpr == CPR_FULL && addedRows && startPos + addedRows < requiredPos) {
                // We filled the window before we got to the one row that we really wanted.
                // Clear the window and start filling it again from here.
                // TODO: Would be nicer if we could progressively replace earlier rows.
                window->clear();
                window->setNumColumns(numColumns);
                startPos += addedRows;
                addedRows = 0;
                FAIL_RETURN(CopyRow(window, statement, numColumns, startPos, addedRows, &cpr));
            }

            if (cpr == CPR_OK) {
                addedRows += 1;
            }
            else if (cpr == CPR_FULL) {
                windowFull = TRUE;
            }
            else {
                gotException = TRUE;
            }
        }
        else if (err == SQLITE_DONE) {
            // All rows processed, bail
            ALOGW("Processed all rows");
            break;
        }
        else if (err == SQLITE_LOCKED || err == SQLITE_BUSY) {
            // The table is locked, retry
            ALOGW("Database locked, retrying");
            if (retryCount > 50) {
                ALOGE("Bailing on database busy retry");
                ec = throw_sqlite3_exception(connection->db, "retrycount exceeded");
                gotException = TRUE;
            }
            else {
                // Sleep to give the thread holding the lock a chance to finish
                usleep(1000);
                retryCount++;
            }
        }
        else {
            ec = throw_sqlite3_exception(connection->db);
            gotException = TRUE;
        }
    }

    ALOGW("Resetting statement %p after fetching %d rows and adding %d rows"
           "to the window in %d bytes",
           statement, totalRows, addedRows, window->size() - window->freeSpace());
    sqlite3_reset(statement);

    // Report the total number of rows on request.
    if (startPos > totalRows) {
       ALOGE("startPos %d > actual rows %d", startPos, totalRows);
    }
    *result = Int64(startPos) << 32 | Int64(totalRows);
    return ec;
}

Int32 SQLiteConnection::NativeGetDbLookaside(
    /* [in] */ Int64 connectionPtr)
{
    SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);

    Int32 cur = -1;
    Int32 unused;
    sqlite3_db_status(connection->db, SQLITE_DBSTATUS_LOOKASIDE_USED, &cur, &unused, 0);
    return cur;
}

void SQLiteConnection::NativeCancel(
    /* [in] */ Int64 connectionPtr)
{
    SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);
    connection->canceled = TRUE;
}

void SQLiteConnection::NativeResetCancel(
    /* [in] */ Int64 connectionPtr,
    /* [in] */ Boolean cancelable)
{
    SQLiteConnectionNative* connection = reinterpret_cast<SQLiteConnectionNative*>(connectionPtr);
    connection->canceled = FALSE;

    if (cancelable) {
        sqlite3_progress_handler(connection->db, 4, SqliteProgressHandlerCallback,
                connection);
    }
    else {
        sqlite3_progress_handler(connection->db, 0, NULL, NULL);
    }
}

SQLiteConnection::SQLiteConnection(
    /* [in] */ SQLiteConnectionPool* pool,
    /* [in] */ SQLiteDatabaseConfiguration* configuration,
    /* [in] */ Int32 connectionId,
    /* [in] */ Boolean primaryConnection)
    : mPool(pool)
    , mConnectionId(connectionId)
    , mIsPrimaryConnection(primaryConnection)
    , mConnectionPtr(0)
    , mOnlyAllowReadOnlyOperations(FALSE)
    , mCancellationSignalAttachCount(0)
{
    mRecentOperations = new OperationLog();

    mConfiguration = new SQLiteDatabaseConfiguration(configuration);
    mIsReadOnlyConnection = (configuration->mOpenFlags & ISQLiteDatabase::OPEN_READONLY) != 0;

    mPreparedStatementCache = new PreparedStatementCache(mConfiguration->mMaxSqlCacheSize, this);

    AutoPtr<ICloseGuardHelper> helper;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&helper);
    helper->Get((ICloseGuard**)&mCloseGuard);
    mCloseGuard->Open(String("close"));
}

SQLiteConnection::~SQLiteConnection()
{
    //try {
    if (mPool != NULL && mConnectionPtr != 0) {
        mPool->OnConnectionLeaked();
    }
    Dispose(TRUE);

    //} finally {
        //super.Finalize();
    //}
}

CAR_INTERFACE_IMPL(SQLiteConnection, Object, ICancellationSignalOnCancelListener);

ECode SQLiteConnection::Open(
    /* [in] */ SQLiteConnectionPool* pool,
    /* [in] */ SQLiteDatabaseConfiguration* configuration,
    /* [in] */ Int32 connectionId,
    /* [in] */ Boolean primaryConnection,
    /* [out] */ SQLiteConnection** connect)
{
    AutoPtr<SQLiteConnection> connection = new SQLiteConnection(pool, configuration, connectionId, primaryConnection);
    // try {
    ECode ec = connection->Open();
    if (ec == (ECode)E_SQLITE_EXCEPTION) {
        connection->Dispose(FALSE);
        *connect = NULL;
        return ec;
    }
    *connect = connection;
    REFCOUNT_ADD(*connect);
    return NOERROR;
    // } catch (SQLiteException ex) {
    //     connection.dispose(false);
    //     throw ex;
    // }
}

void SQLiteConnection::Close()
{
    Dispose(FALSE);
}

ECode SQLiteConnection::Open()
{
    assert(mConnectionPtr == 0);
    AutoPtr<SQLiteDatabaseConfiguration> sdc = mConfiguration;
    ECode ec = NativeOpen(mConfiguration->mPath, mConfiguration->mOpenFlags, mConfiguration->mLabel,
            SQLiteDebug::DEBUG_SQL_STATEMENTS, SQLiteDebug::DEBUG_SQL_TIME, &mConnectionPtr);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Failed to open:%s, label:%s, ec=%08x",
            mConfiguration->mPath.string(), mConfiguration->mLabel.string(), ec);
        return ec;
    }
    FAIL_RETURN(SetPageSize())
    FAIL_RETURN(SetForeignKeyModeFromConfiguration())
    FAIL_RETURN(SetWalModeFromConfiguration())
    FAIL_RETURN(SetJournalSizeLimit())
    FAIL_RETURN(SetAutoCheckpointInterval())
    FAIL_RETURN(SetLocaleFromConfiguration())
    return NOERROR;
}

void SQLiteConnection::Dispose(
    /* [in] */ Boolean finalized)
{
    if (mCloseGuard != NULL) {
        if (finalized) {
            mCloseGuard->WarnIfOpen();
        }
        mCloseGuard->Close();
    }

    if (mConnectionPtr != 0) {
        Int32 cookie = mRecentOperations->BeginOperation(String("close"), String(NULL), NULL);
        //try {
        mPreparedStatementCache->EvictAll();
        NativeClose(mConnectionPtr);
        mConnectionPtr = 0;
        //} finally {
        mRecentOperations->EndOperation(cookie);
        //}
    }
}

ECode SQLiteConnection::SetPageSize()
{
    if (!mConfiguration->IsInMemoryDb() && !mIsReadOnlyConnection) {
        Int32 newValue = SQLiteGlobal::GetDefaultPageSize();
        Int64 value;
        FAIL_RETURN(ExecuteForInt64(String("PRAGMA page_size"), NULL, NULL, &value))
        if (value != (Int64)newValue) {
            return Execute(String("PRAGMA page_size=") + StringUtils::ToString(newValue), NULL, NULL);
        }
    }
    return NOERROR;
}

ECode SQLiteConnection::SetAutoCheckpointInterval()
{
    if (!mConfiguration->IsInMemoryDb() && !mIsReadOnlyConnection) {
        Int32 newValue = SQLiteGlobal::GetWALAutoCheckpoint();
        Int64 value;
        FAIL_RETURN(ExecuteForInt64(String("PRAGMA wal_autocheckpoint"), NULL, NULL, &value))
        if (value != (Int64)newValue) {
            StringBuilder sb("PRAGMA wal_autocheckpoint=");
            sb += newValue;
            FAIL_RETURN(ExecuteForInt64(sb.ToString(), NULL, NULL, &value))
        }
    }
    return NOERROR;
}

ECode SQLiteConnection::SetJournalSizeLimit()
{
    if (!mConfiguration->IsInMemoryDb() && !mIsReadOnlyConnection) {
        Int32 newValue = SQLiteGlobal::GetJournalSizeLimit();
        Int64 value;
        FAIL_RETURN(ExecuteForInt64(String("PRAGMA journal_size_limit"), NULL, NULL, &value))
        if (value != (Int64)newValue) {
            StringBuilder sb("PRAGMA journal_size_limit=");
            sb += newValue;
            return ExecuteForInt64(sb.ToString(), NULL, NULL, &value);
        }
    }
    return NOERROR;
}

ECode SQLiteConnection::SetForeignKeyModeFromConfiguration()
{
    if (!mIsReadOnlyConnection) {
        Int64 newValue = mConfiguration->mForeignKeyConstraintsEnabled ? 1 : 0;
        Int64 value;
        FAIL_RETURN(ExecuteForInt64(String("PRAGMA foreign_keys"), NULL, NULL, &value))
        if (value != newValue) {
            StringBuilder sb("PRAGMA foreign_keys=");
            sb += newValue;
            return Execute(sb.ToString(), NULL, NULL);
        }
    }
    return NOERROR;
}

ECode SQLiteConnection::SetWalModeFromConfiguration()
{
    if (!mConfiguration->IsInMemoryDb() && !mIsReadOnlyConnection) {
        if ((mConfiguration->mOpenFlags & ISQLiteDatabase::ENABLE_WRITE_AHEAD_LOGGING) != 0) {
            FAIL_RETURN(SetJournalMode(String("WAL")))
            FAIL_RETURN(SetSyncMode(SQLiteGlobal::GetWALSyncMode()))
        }
        else {
            FAIL_RETURN(SetJournalMode(SQLiteGlobal::GetDefaultJournalMode()))
            FAIL_RETURN(SetSyncMode(SQLiteGlobal::GetDefaultSyncMode()))
        }
    }
    return NOERROR;
}

ECode SQLiteConnection::SetSyncMode(
    /* [in] */ const String& newValue)
{
    String value;
    FAIL_RETURN(ExecuteForString(String("PRAGMA synchronous"), NULL, NULL, &value))
    if (!CanonicalizeSyncMode(value).EqualsIgnoreCase(CanonicalizeSyncMode(newValue))) {
        StringBuilder sb("PRAGMA synchronous=");
        sb += newValue;
        return Execute(sb.ToString(), NULL, NULL);
    }
    return NOERROR;
}

String SQLiteConnection::CanonicalizeSyncMode(
    /* [in] */ const String& value)
{
    if (value.Equals("0")) {
        return String("OFF");
    }
    else if (value.Equals("1")) {
        return String("NORMAL");
    }
    else if (value.Equals("2")) {
        return String("FULL");
    }
    return value;
}

ECode SQLiteConnection::SetJournalMode(
    /* [in] */ const String& newValue)
{
    String value;
    FAIL_RETURN(ExecuteForString(String("PRAGMA journal_mode"), NULL, NULL, &value))
    if (!value.EqualsIgnoreCase(newValue)) {
        //try {
        String result;
        StringBuilder sb("PRAGMA journal_mode=");
        sb += newValue;
        ExecuteForString(sb.ToString(), NULL, NULL, &result);
        if (result.EqualsIgnoreCase(newValue)) {
            return NOERROR;
        }
        // PRAGMA journal_mode silently fails and returns the original journal
        // mode in some cases if the journal mode could not be changed.
        //} catch (SQLiteDatabaseLockedException ex) {
            // This error (SQLITE_BUSY) occurs if one connection has the database
            // open in WAL mode and another tries to change it to non-WAL.
        //}
        // Because we always disable WAL mode when a database is first opened
        // (even if we intend to re-enable it), we can encounter problems if
        // there is another open connection to the database somewhere.
        // This can happen for a variety of reasons such as an application opening
        // the same database in multiple processes at the same time or if there is a
        // crashing content provider service that the ActivityManager has
        // removed from its registry but whose process hasn't quite died yet
        // by the time it is restarted in a new process.
        //
        // If we don't change the journal mode, nothing really bad happens.
        // In the worst case, an application that enables WAL might not actually
        // get it, although it can still use connection pooling.
        Slogger::W(TAG, "Could not change the database journal mode of '%s' from '%s' to '%s' because the database is locked.  This usually means that the database from enabling or disabling write-ahead logging mode.  Proceeding without changing the journal mode."
                , mConfiguration->mLabel.string(), value.string(), newValue.string());
    }
    return NOERROR;
}

ECode SQLiteConnection::SetLocaleFromConfiguration()
{
    if ((mConfiguration->mOpenFlags & ISQLiteDatabase::NO_LOCALIZED_COLLATORS) != 0) {
        return NOERROR;
    }

    // Register the localized collators.
    String newLocale;
    mConfiguration->mLocale->ToString(&newLocale);
    FAIL_RETURN(NativeRegisterLocalizedCollators(mConnectionPtr, newLocale))

    // If the database is read-only, we cannot modify the android metadata table
    // or existing indexes.
    if (mIsReadOnlyConnection) {
        return NOERROR;
    }

    //try {
    // Ensure the  android metadata table exists.
    FAIL_RETURN(Execute(String("CREATE TABLE IF NOT EXISTS android_metadata (locale TEXT)"), NULL, NULL))

    // Check whether the locale was actually changed.
    String oldLocale;
    FAIL_RETURN(ExecuteForString(
            String("SELECT locale FROM android_metadata UNION SELECT NULL ORDER BY locale DESC LIMIT 1"),
            NULL, NULL, &oldLocale))
    if (!oldLocale.IsNullOrEmpty() && oldLocale.Equals(newLocale)) {
        return NOERROR;
    }

    // Go ahead and update the indexes using the new locale.
    FAIL_RETURN(Execute(String("BEGIN"), NULL, NULL))
    Boolean success = FALSE;
    //try {
    AutoPtr<ArrayOf<IInterface*> > bindArgs = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr<ICharSequence> seq;
    CString::New(newLocale, (ICharSequence**)&seq);
    bindArgs->Set(0, seq);
    FAIL_GOTO(Execute(String("DELETE FROM android_metadata"), NULL, NULL), Exit);
    FAIL_GOTO(Execute(String("INSERT INTO android_metadata (locale) VALUES(?)"), bindArgs, NULL), Exit);
    FAIL_GOTO(Execute(String("REINDEX LOCALIZED"), NULL, NULL), Exit);
    success = TRUE;
Exit:
    //} finally {
    return Execute(success ? String("COMMIT") : String("ROLLBACK"), NULL, NULL);
    //}
    //} catch (SQLiteDatabaseCorruptException ex) {
        // throw ex;
    //} catch (RuntimeException ex) {
        // throw new SQLiteException("Failed to change locale for db '" + mConfiguration.label
        //             + "' to '" + newLocale + "'.", ex);
    //}
}

ECode SQLiteConnection::Reconfigure(
    /* [in] */ SQLiteDatabaseConfiguration* configuration)
{
    mOnlyAllowReadOnlyOperations = FALSE;

    // Register custom functions.
    Int32 functionCount = configuration->mCustomFunctions.GetSize();
    for (Int32 i = 0; i < functionCount; i++) {
        AutoPtr<SQLiteCustomFunction> function = configuration->mCustomFunctions[i];
        List<AutoPtr<SQLiteCustomFunction> >::Iterator it = Find(
                mConfiguration->mCustomFunctions.Begin(), mConfiguration->mCustomFunctions.End(), function);
        if (it == mConfiguration->mCustomFunctions.End()) {
            FAIL_RETURN(NativeRegisterCustomFunction(mConnectionPtr, function))
        }
    }

    // Remember what changed.
    Boolean foreignKeyModeChanged = configuration->mForeignKeyConstraintsEnabled
            != mConfiguration->mForeignKeyConstraintsEnabled;
    Boolean walModeChanged = ((configuration->mOpenFlags ^ mConfiguration->mOpenFlags)
            & ISQLiteDatabase::ENABLE_WRITE_AHEAD_LOGGING) != 0;
    Boolean temp;
    configuration->mLocale->Equals(mConfiguration->mLocale, &temp);
    Boolean localeChanged = !temp;

    // Update configuration parameters.
    mConfiguration->UpdateParametersFrom(configuration);

    // Update prepared statement cache size.
    mPreparedStatementCache->Resize(configuration->mMaxSqlCacheSize);

    // Update foreign key mode.
    if (foreignKeyModeChanged) {
        FAIL_RETURN(SetForeignKeyModeFromConfiguration())
    }

    // Update WAL.
    if (walModeChanged) {
        FAIL_RETURN(SetWalModeFromConfiguration())
    }

    // Update locale.
    if (localeChanged) {
        FAIL_RETURN(SetLocaleFromConfiguration())
    }
    return NOERROR;
}

void SQLiteConnection::SetOnlyAllowReadOnlyOperations(
    /* [in] */ Boolean readOnly)
{
    mOnlyAllowReadOnlyOperations = readOnly;
}

Boolean SQLiteConnection::IsPreparedStatementInCache(
    /* [in] */ const String& sql)
{
    return mPreparedStatementCache->Get(sql) != NULL;
}

Int32 SQLiteConnection::GetConnectionId()
{
    return mConnectionId;
}

Boolean SQLiteConnection::IsPrimaryConnection()
{
    return mIsPrimaryConnection;
}

ECode SQLiteConnection::Prepare(
    /* [in] */ const String& sql,
    /* [in] */ SQLiteStatementInfo* outStatementInfo)
{
    if (sql.IsNullOrEmpty()) {
        //throw new IllegalArgumentException("sql must not be null.");
        Slogger::E(TAG, "sql must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 cookie = mRecentOperations->BeginOperation(String("prepare"), sql, NULL);
    //try {
    AutoPtr<PreparedStatement> statement;
    ECode ec = AcquirePreparedStatement(sql, (PreparedStatement**)&statement);
    FAIL_GOTO(ec, fail);
    //try {
    if (outStatementInfo != NULL) {
        outStatementInfo->mNumParameters = statement->mNumParameters;
        outStatementInfo->mReadOnly = statement->mReadOnly;

        Int32 columnCount = NativeGetColumnCount(mConnectionPtr, statement->mStatementPtr);
        if (columnCount == 0) {
            outStatementInfo->mColumnNames = EMPTY_STRING_ARRAY;
        }
        else {
            outStatementInfo->mColumnNames = ArrayOf<String>::Alloc(columnCount);
            for (Int32 i = 0; i < columnCount; i++) {
                (*outStatementInfo->mColumnNames)[i] = NativeGetColumnName(
                        mConnectionPtr, statement->mStatementPtr, i);
            }
        }
    }
    //} finally {
    ReleasePreparedStatement(statement);
    //}
    //} catch (RuntimeException ex) {
        // mRecentOperations.failOperation(cookie, ex);
        // throw ex;
    //} finally {
    // mRecentOperations.endOperation(cookie);s
    //}
fail:
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        mRecentOperations->FailOperation(cookie, ec);
    }
    mRecentOperations->EndOperation(cookie);
    return ec;
}

ECode SQLiteConnection::Execute(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<IInterface*>* bindArgs,
    /* [in] */ ICancellationSignal* cancellationSignal)
{
    if (sql.IsNullOrEmpty()) {
        //throw new IllegalArgumentException("sql must not be null.");
        Slogger::E(TAG, "sql must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 cookie = mRecentOperations->BeginOperation(String("execute"), sql, bindArgs);
    //try {
    AutoPtr<PreparedStatement> statement;
    ECode ec = AcquirePreparedStatement(sql, (PreparedStatement**)&statement);
    FAIL_GOTO(ec, fail);
    //try {
    ec = ThrowIfStatementForbidden(statement);
    FAIL_GOTO(ec, fail);
    ec = BindArguments(statement, bindArgs);
    FAIL_GOTO(ec, fail);
    ec = ApplyBlockGuardPolicy(statement);
    FAIL_GOTO(ec, fail);
    ec = AttachCancellationSignal(cancellationSignal);
    FAIL_GOTO(ec, fail);
    //try {
    ec = NativeExecute(mConnectionPtr, statement->mStatementPtr);
    //} finally {
    DetachCancellationSignal(cancellationSignal);
    //}
    // } finally {
    //     releasePreparedStatement(statement);
    // }
    // } catch (RuntimeException ex) {
    //     mRecentOperations.failOperation(cookie, ex);
    //     throw ex;
    // } finally {
    //     mRecentOperations.endOperation(cookie);
    // }
fail:
    ReleasePreparedStatement(statement);
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        mRecentOperations->FailOperation(cookie, ec);
    }
    mRecentOperations->EndOperation(cookie);
    return ec;
}

ECode SQLiteConnection::ExecuteForInt64(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<IInterface*>* bindArgs,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    if (sql.IsNullOrEmpty()) {
        //throw new IllegalArgumentException("sql must not be null.");
        Slogger::E(TAG, "sql must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 cookie = mRecentOperations->BeginOperation(String("executeForLong"), sql, bindArgs);

    //try {
    AutoPtr<PreparedStatement> statement;
    ECode ec = AcquirePreparedStatement(sql, (PreparedStatement**)&statement);
    FAIL_GOTO(ec, fail);
    //try {
    ec = ThrowIfStatementForbidden(statement);
    FAIL_GOTO(ec, fail)
    ec = BindArguments(statement, bindArgs);
    FAIL_GOTO(ec, fail)
    ec = ApplyBlockGuardPolicy(statement);
    FAIL_GOTO(ec, fail)
    ec = AttachCancellationSignal(cancellationSignal);
    FAIL_GOTO(ec, fail)
    //try {
    ec = NativeExecuteForInt64(mConnectionPtr, statement->mStatementPtr, result);
    //} finally {
    DetachCancellationSignal(cancellationSignal);
    //}
    //} finally {
    // ReleasePreparedStatement(statement);
    //}
    //} catch (RuntimeException ex) {
        //mRecentOperations.failOperation(cookie, ex);
        //throw ex;
    //}
    //} finally {
    // mRecentOperations->EndOperation(cookie);
    //}
fail:
    ReleasePreparedStatement(statement);
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        mRecentOperations->FailOperation(cookie, ec);
    }
    mRecentOperations->EndOperation(cookie);
    return ec;
}

ECode SQLiteConnection::ExecuteForString(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<IInterface*>* bindArgs,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    if (sql.IsNullOrEmpty()) {
        //throw new IllegalArgumentException("sql must not be null.");
        Slogger::E(TAG, "sql must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 cookie = mRecentOperations->BeginOperation(String("executeForString"), sql, bindArgs);
    //try {
    AutoPtr<PreparedStatement> statement;
    ECode ec = AcquirePreparedStatement(sql, (PreparedStatement**)&statement);
    FAIL_GOTO(ec, fail);
    //try {
    ec = ThrowIfStatementForbidden(statement);
    FAIL_GOTO(ec, fail)
    ec = BindArguments(statement, bindArgs);
    FAIL_GOTO(ec, fail)
    ec = ApplyBlockGuardPolicy(statement);
    FAIL_GOTO(ec, fail)
    ec = AttachCancellationSignal(cancellationSignal);
    FAIL_GOTO(ec, fail)
    //try {
    ec = NativeExecuteForString(mConnectionPtr, statement->mStatementPtr, str);
    //} finally {
    DetachCancellationSignal(cancellationSignal);
    //}
    // } finally {
    //     releasePreparedStatement(statement);
    // }
    // } catch (RuntimeException ex) {
    //     mRecentOperations.failOperation(cookie, ex);
    //     throw ex;
    // } finally {
    //     mRecentOperations.endOperation(cookie);
    // }
fail:
    ReleasePreparedStatement(statement);
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        mRecentOperations->FailOperation(cookie, ec);
    }
    mRecentOperations->EndOperation(cookie);
    return ec;
}

ECode SQLiteConnection::ExecuteForBlobFileDescriptor(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<IInterface*>* bindArgs,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    VALIDATE_NOT_NULL(descriptor)
    if (sql.IsNullOrEmpty()) {
        //throw new IllegalArgumentException("sql must not be null.");
        Slogger::E(TAG, "sql must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 cookie = mRecentOperations->BeginOperation(String("executeForBlobFileDescriptor"), sql, bindArgs);
    //try {
    AutoPtr<PreparedStatement> statement;
    ECode ec = AcquirePreparedStatement(sql, (PreparedStatement**)&statement);
    FAIL_GOTO(ec, fail);
    //try {
    ec = ThrowIfStatementForbidden(statement);
    FAIL_GOTO(ec, fail)
    ec = BindArguments(statement, bindArgs);
    FAIL_GOTO(ec, fail)
    ec = ApplyBlockGuardPolicy(statement);
    FAIL_GOTO(ec, fail)
    ec = AttachCancellationSignal(cancellationSignal);
    FAIL_GOTO(ec, fail)
    //try {
    Int32 fd;
    ec = NativeExecuteForBlobFileDescriptor(mConnectionPtr, statement->mStatementPtr, &fd);
    if (fd >= 0) {
        CParcelFileDescriptor::AdoptFd(fd, descriptor);
    }
    else {
        *descriptor = NULL;
    }
    //} finally {
    DetachCancellationSignal(cancellationSignal);
    //}
    // } finally {
    //     releasePreparedStatement(statement);
    // }
    // } catch (RuntimeException ex) {
    //     mRecentOperations.failOperation(cookie, ex);
    //     throw ex;
    // } finally {
    //     mRecentOperations.endOperation(cookie);
    // }
fail:
    ReleasePreparedStatement(statement);
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        mRecentOperations->FailOperation(cookie, ec);
    }
    mRecentOperations->EndOperation(cookie);
    return ec;
}

ECode SQLiteConnection::ExecuteForChangedRowCount(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<IInterface*>* bindArgs,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    if (sql.IsNullOrEmpty()) {
        //throw new IllegalArgumentException("sql must not be null.");
        Slogger::E(TAG, "sql must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 cookie = mRecentOperations->BeginOperation(String("executeForChangedRowCount"), sql, bindArgs);
    //try {
    AutoPtr<PreparedStatement> statement;
    ECode ec = AcquirePreparedStatement(sql, (PreparedStatement**)&statement);
    FAIL_GOTO(ec, fail);
    //try {
    ec = ThrowIfStatementForbidden(statement);
    FAIL_GOTO(ec, fail)
    ec = BindArguments(statement, bindArgs);
    FAIL_GOTO(ec, fail)
    ec = ApplyBlockGuardPolicy(statement);
    FAIL_GOTO(ec, fail)
    ec = AttachCancellationSignal(cancellationSignal);
    FAIL_GOTO(ec, fail)
    //try {
    ec = NativeExecuteForChangedRowCount(mConnectionPtr, statement->mStatementPtr, count);
    //} finally {
    DetachCancellationSignal(cancellationSignal);
    //}
    // } finally {
    //     releasePreparedStatement(statement);
    // }
    // } catch (RuntimeException ex) {
    //     mRecentOperations.failOperation(cookie, ex);
    //     throw ex;
    // } finally {
    //     if (mRecentOperations.endOperationDeferLog(cookie)) {
    //         mRecentOperations.logOperation(cookie, "changedRows=" + changedRows);
    //     }
    // }
fail:
    ReleasePreparedStatement(statement);
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        mRecentOperations->FailOperation(cookie, ec);
    }
    if (mRecentOperations->EndOperationDeferLog(cookie)) {
        StringBuilder sb("changedRows=");
        sb += *count;
        mRecentOperations->LogOperation(cookie, sb.ToString());
    }
    return ec;
}

ECode SQLiteConnection::ExecuteForLastInsertedRowId(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<IInterface*>* bindArgs,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id)
    *id = -1;

    if (sql.IsNullOrEmpty()) {
        //throw new IllegalArgumentException("sql must not be null.");
        Slogger::E(TAG, "sql must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 cookie = mRecentOperations->BeginOperation(String("executeForLastInsertedRowId"), sql, bindArgs);
    //try {
    AutoPtr<PreparedStatement> statement;
    ECode ec = AcquirePreparedStatement(sql, (PreparedStatement**)&statement);
    FAIL_GOTO(ec, fail);
    //try {
    ec = ThrowIfStatementForbidden(statement);
    FAIL_GOTO(ec, fail)
    ec = BindArguments(statement, bindArgs);
    FAIL_GOTO(ec, fail)
    ec = ApplyBlockGuardPolicy(statement);
    FAIL_GOTO(ec, fail)
    ec = AttachCancellationSignal(cancellationSignal);
    FAIL_GOTO(ec, fail)
    //try {
    ec = NativeExecuteForLastInsertedRowId(mConnectionPtr, statement->mStatementPtr, id);
    //} finally {
    DetachCancellationSignal(cancellationSignal);
    //}
    // } finally {
    //     releasePreparedStatement(statement);
    // }
    // } catch (RuntimeException ex) {
    //     mRecentOperations.failOperation(cookie, ex);
    //     throw ex;
    // } finally {
    //     mRecentOperations.endOperation(cookie);
    // }
fail:
    if (FAILED(ec)) {
        Slogger::E(TAG, "failed to execute insert with [%s]", sql.string());
    }
    ReleasePreparedStatement(statement);
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        mRecentOperations->FailOperation(cookie, ec);
    }
    mRecentOperations->EndOperation(cookie);
    return ec;
}

ECode SQLiteConnection::ExecuteForCursorWindow(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<IInterface*>* bindArgs,
    /* [in] */ ICursorWindow* window,
    /* [in] */ Int32 startPos,
    /* [in] */ Int32 requiredPos,
    /* [in] */ Boolean countAllRows,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (sql.IsNullOrEmpty()) {
        //throw new IllegalArgumentException("sql must not be null.");
        Slogger::E(TAG, "sql must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (window == NULL) {
        //throw new IllegalArgumentException("window must not be null.");
        Slogger::E(TAG, "window must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    ISQLiteClosable::Probe(window)->AcquireReference();
    //try {
    Int32 actualPos = -1;
    Int32 countedRows = -1;
    Int32 filledRows = -1;
    Int32 cookie = mRecentOperations->BeginOperation(String("executeForCursorWindow"), sql, bindArgs);
    //try {
    AutoPtr<PreparedStatement> statement;
    ECode ec = AcquirePreparedStatement(sql, (PreparedStatement**)&statement);
    FAIL_GOTO(ec, fail);
    //try {
    ec = ThrowIfStatementForbidden(statement);
    FAIL_GOTO(ec, fail)
    ec = BindArguments(statement, bindArgs);
    FAIL_GOTO(ec, fail)
    ec = ApplyBlockGuardPolicy(statement);
    FAIL_GOTO(ec, fail)
    ec = AttachCancellationSignal(cancellationSignal);
    FAIL_GOTO(ec, fail)
    //try {
    Int64 _result;
    ec = NativeExecuteForCursorWindow(mConnectionPtr, statement->mStatementPtr,
            ((CCursorWindow*)(ICursorWindow*)window)->mWindowPtr, startPos, requiredPos, countAllRows, &_result);
    if (FAILED(ec)) {
        DetachCancellationSignal(cancellationSignal);
        goto fail;
    }
    actualPos = (Int32)(_result >> 32);
    countedRows = (Int32)_result;
    window->GetNumRows(&filledRows);
    window->SetStartPosition(actualPos);
    *result = countedRows;
    //} finally {
    DetachCancellationSignal(cancellationSignal);
    //}
    // } finally {
    //     releasePreparedStatement(statement);
    // }
    // } catch (RuntimeException ex) {
    //     mRecentOperations.failOperation(cookie, ex);
    //     throw ex;
    // } finally {
    //     if (mRecentOperations.endOperationDeferLog(cookie)) {
    //         mRecentOperations.logOperation(cookie, "window='" + window
    //                 + "', startPos=" + startPos
    //                 + ", actualPos=" + actualPos
    //                 + ", filledRows=" + filledRows
    //                 + ", countedRows=" + countedRows);
    //     }
    // }
fail:
    ReleasePreparedStatement(statement);
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        mRecentOperations->FailOperation(cookie, ec);
    }
    if (mRecentOperations->EndOperationDeferLog(cookie)) {
        StringBuilder sb("window='");
        sb.Append(window);
        sb += "', startPos=";
        sb += startPos;
        sb += ", actualPos=";
        sb += actualPos;
        sb += ", filledRows=";
        sb += filledRows;
        sb += ", countedRows=";
        sb += countedRows;
        mRecentOperations->LogOperation(cookie, sb.ToString());
    }
    ISQLiteClosable::Probe(window)->ReleaseReference();
    return ec;
}

ECode SQLiteConnection::AcquirePreparedStatement(
    /* [in] */ const String& sql,
    /* [out] */ SQLiteConnection::PreparedStatement** _statement)
{
    VALIDATE_NOT_NULL(_statement)
    *_statement = NULL;
    AutoPtr<PreparedStatement> statement = mPreparedStatementCache->Get(sql);
    Boolean skipCache = FALSE;
    if (statement != NULL) {
        if (!statement->mInUse) {
            *_statement = statement;
            REFCOUNT_ADD(*_statement)
            return NOERROR;
        }
        // The statement is already in the cache but is in use (this statement appears
        // to be not only re-entrant but recursive!).  So prepare a new copy of the
        // statement but do not cache it.
        skipCache = TRUE;
    }

    Int64 statementPtr;
    FAIL_RETURN(NativePrepareStatement(mConnectionPtr, sql, &statementPtr))
    //try {
    Int32 numParameters = NativeGetParameterCount(mConnectionPtr, statementPtr);
    Int32 type = DatabaseUtils::GetSqlStatementType(sql);
    Boolean readOnly = NativeIsReadOnly(mConnectionPtr, statementPtr);
    statement = ObtainPreparedStatement(sql, statementPtr, numParameters, type, readOnly);
    if (!skipCache && IsCacheable(type)) {
        mPreparedStatementCache->Put(sql, statement);
        statement->mInCache = TRUE;
    }
    // } catch (RuntimeException ex) {
        // Finalize the statement if an exception occurred and we did not add
        // it to the cache.  If it is already in the cache, then leave it there.
        // if (statement == NULL || !statement->mInCache) {
        //     NativeFinalizeStatement(mConnectionPtr, statementPtr);
        // }
        //throw ex;
    // }
    statement->mInUse = TRUE;
    *_statement = statement;
    REFCOUNT_ADD(*_statement)
    return NOERROR;
}

void SQLiteConnection::ReleasePreparedStatement(
    /* [in] */ PreparedStatement* statement)
{
    if (statement == NULL)
        return;

    statement->mInUse = FALSE;
    if (statement->mInCache) {
        //try {
        ECode ec = NativeResetStatementAndClearBindings(mConnectionPtr, statement->mStatementPtr);
        //} catch (SQLiteException ex) {
        // The statement could not be reset due to an error.  Remove it from the cache.
        // When remove() is called, the cache will invoke its entryRemoved() callback,
        // which will in turn call finalizePreparedStatement() to finalize and
        // recycle the statement.
        if (ec == (ECode)E_SQLITE_EXCEPTION) {
            if (DEBUG) {
                Slogger::D(TAG, "Could not reset prepared statement due to an exception.  Removing it from the cache.  SQL: %d, 0x%08x"
                        , TrimSqlForDisplay(statement->mSql).string(), ec);
            }

            mPreparedStatementCache->Remove(statement->mSql);
        }
        //}
    }
    else {
        FinalizePreparedStatement(statement);
    }
}

void SQLiteConnection::FinalizePreparedStatement(
    /* [in] */ PreparedStatement* statement)
{
    NativeFinalizeStatement(mConnectionPtr, statement->mStatementPtr);
    RecyclePreparedStatement(statement);
}

ECode SQLiteConnection::AttachCancellationSignal(
   /* [in] */ ICancellationSignal* cancellationSignal)
{
    if (cancellationSignal != NULL) {
        FAIL_RETURN(cancellationSignal->ThrowIfCanceled())

        mCancellationSignalAttachCount += 1;
        if (mCancellationSignalAttachCount == 1) {
            // Reset cancellation flag before executing the statement.
            NativeResetCancel(mConnectionPtr, TRUE /*cancelable*/);

            // After this point, onCancel() may be called concurrently.
            return cancellationSignal->SetOnCancelListener(this);
        }
    }
    return NOERROR;
}

void SQLiteConnection::DetachCancellationSignal(
   /* [in] */ ICancellationSignal* cancellationSignal)
{
    if (cancellationSignal != NULL) {
        assert(mCancellationSignalAttachCount > 0);

        mCancellationSignalAttachCount -= 1;
        if (mCancellationSignalAttachCount == 0) {
            // After this point, onCancel() cannot be called concurrently.
            cancellationSignal->SetOnCancelListener(NULL);

            // Reset cancellation flag after executing the statement.
            NativeResetCancel(mConnectionPtr, FALSE /*cancelable*/);
        }
    }
}

ECode SQLiteConnection::OnCancel()
{
    NativeCancel(mConnectionPtr);
    return NOERROR;
}

ECode SQLiteConnection::BindArguments(
    /* [in] */ PreparedStatement* statement,
    /* [in] */ ArrayOf<IInterface*>* bindArgs)
{
    Int32 count = bindArgs != NULL ? bindArgs->GetLength() : 0;
    if (count != statement->mNumParameters) {
        //throw new SQLiteBindOrColumnIndexOutOfRangeException(
        //        "Expected " + statement.mNumParameters + " bind arguments but "
        //        + count + " were provided.");
        Slogger::E(TAG, "Expected %d bind arguments but %d were provided.", statement->mNumParameters, bindArgs->GetLength());
        return E_SQLITE_BIND_OR_COLUMN_INDEX_OUTOF_RANGE_EXCEPTION;
    }
    if (count == 0) {
        return NOERROR;
    }

    Int64 statementPtr = statement->mStatementPtr;
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> arg = (*bindArgs)[i];
        Int32 type = DatabaseUtils::GetTypeOfObject(arg);
        switch (type) {
            case ICursor::FIELD_TYPE_NULL:
                FAIL_RETURN(NativeBindNull(mConnectionPtr, statementPtr, i + 1));
                break;
            case ICursor::FIELD_TYPE_INTEGER:
            {
                Int64 valueInt64;
                FAIL_RETURN(NativeBindInt64(mConnectionPtr, statementPtr, i + 1,
                        (INumber::Probe(arg)->Int64Value(&valueInt64), valueInt64)));
                break;
            }
            case ICursor::FIELD_TYPE_FLOAT:
            {
                Double valueDouble;
                FAIL_RETURN(NativeBindDouble(mConnectionPtr, statementPtr, i + 1,
                        (INumber::Probe(arg)->DoubleValue(&valueDouble), valueDouble)));
                break;
            }
            case ICursor::FIELD_TYPE_BLOB:
            {
                 AutoPtr<IArrayOf> argIArrayOf = IArrayOf::Probe(arg);
                 Int32 length;
                 argIArrayOf->GetLength(&length);
                 AutoPtr< ArrayOf<Byte> > argArray = ArrayOf<Byte>::Alloc(length);
                 for (Int32 j = 0; j < length; ++j) {
                    AutoPtr<IInterface> ibyte;
                    argIArrayOf->Get(j, (IInterface**)&ibyte);
                    Byte argByte;
                    IByte::Probe(ibyte)->GetValue(&argByte);
                    (*argArray)[j] = argByte;
                 }
                 FAIL_RETURN(NativeBindBlob(mConnectionPtr, statementPtr, i + 1, argArray));
                 break;
            }
            case ICursor::FIELD_TYPE_STRING:
            default:
            {
                if (IBoolean::Probe(arg) != NULL) {
                    // Provide compatibility with legacy applications which may pass
                    // Boolean values in bind args.
                    Boolean valueBoolean;
                    FAIL_RETURN(NativeBindInt64(mConnectionPtr, statementPtr, i + 1,
                            (IBoolean::Probe(arg)->GetValue(&valueBoolean), valueBoolean) ? 1 : 0));
                }
                else {
                    String valueString;
                    FAIL_RETURN(NativeBindString(mConnectionPtr, statementPtr, i + 1,
                            (ICharSequence::Probe(arg)->ToString(&valueString), valueString)));
                }
                break;
            }
        }
    }
    return NOERROR;
}

ECode SQLiteConnection::ThrowIfStatementForbidden(
    /* [in] */ PreparedStatement* statement)
{
    if (mOnlyAllowReadOnlyOperations && !statement->mReadOnly) {
        //throw new SQLiteException("Cannot execute this statement because it "
        //        + "might modify the database but the connection is read-only.");
        Slogger::E(TAG, "Cannot execute this statement because it might modify the database but the connection is read-only.");
        return E_SQLITE_EXCEPTION;
    }
    return NOERROR;
}

Boolean SQLiteConnection::IsCacheable(
    /* [in] */ Int32 statementType)
{
    if (statementType == DatabaseUtils_STATEMENT_UPDATE
            || statementType == DatabaseUtils_STATEMENT_SELECT) {
        return TRUE;
    }
    return FALSE;
}

ECode SQLiteConnection::ApplyBlockGuardPolicy(
    /* [in] */ PreparedStatement* statement)
{
    if (!mConfiguration->IsInMemoryDb()) {
        // TODO CBlockGuard
        // AutoPtr<IBlockGuard> helper;
        // CBlockGuard::AcquireSingleton((IBlockGuard**)&helper);
        // AutoPtr<IBlockGuardPolicy> policy;
        // helper->GetThreadPolicy((IBlockGuardPolicy**)&policy);
        // if (statement->mReadOnly) {
        //     policy->OnReadFromDisk();
        // }
        // else {
        //     policy->OnWriteToDisk();
        // }
    }
    return NOERROR;
}

void SQLiteConnection::Dump(
    /* [in] */ IPrinter* printer,
    /* [in] */ Boolean verbose)
{
    return DumpUnsafe(printer, verbose);
}

void SQLiteConnection::DumpUnsafe(
    /* [in] */ IPrinter* printer,
    /* [in] */ Boolean verbose)
{
    StringBuilder sb("Connection #");
    sb += mConnectionId;
    sb += ":";
    printer->Println(sb.ToString());
    if (verbose) {
        StringBuilder sb1("  connectionPtr: 0x");
        sb1 += mConnectionPtr;
        printer->Println(sb1.ToString());
    }
    StringBuilder sb2("  isPrimaryConnection: ");
    sb2 += mIsPrimaryConnection;
    printer->Println(sb2.ToString());
    StringBuilder sb3("  onlyAllowReadOnlyOperations: ");
    sb3 += mOnlyAllowReadOnlyOperations;
    printer->Println(sb3.ToString());

    mRecentOperations->Dump(printer, verbose);

    if (verbose) {
        mPreparedStatementCache->Dump(printer);
    }
}

String SQLiteConnection::DescribeCurrentOperationUnsafe()
{
    return mRecentOperations->DescribeCurrentOperation();
}

void SQLiteConnection::CollectDbStats(
    /* [in] */ List<AutoPtr<SQLiteDebug::DbStats> >* dbStatsList)
{
    // Get information about the main database.
    Int32 lookaside = NativeGetDbLookaside(mConnectionPtr);
    Int64 pageCount = 0;
    Int64 pageSize = 0;
    // try {
    ExecuteForInt64(String("PRAGMA page_count;"), NULL, NULL, &pageCount);
    ExecuteForInt64(String("PRAGMA page_size;"), NULL, NULL, &pageSize);
    // } catch (SQLiteException ex) {
    //     // Ignore.
    // }
    dbStatsList->PushBack(GetMainDbStatsUnsafe(lookaside, pageCount, pageSize));

    // Get information about attached databases.
    // We ignore the first row in the database list because it corresponds to
    // the main database which we have already described.
    AutoPtr<ICursorWindow> window;
    CCursorWindow::New(String("collectDbStats"), (ICursorWindow**)&window);
    // try {
    Int32 result;
    FAIL_GOTO(ExecuteForCursorWindow(String("PRAGMA database_list;"), NULL, window, 0, 0, FALSE, NULL, &result), fail)
    Int32 rows;
    window->GetNumRows(&rows);
    for (Int32 i = 1; i < rows; i++) {
        String name, path;
        window->GetString(i, 1, &name);
        window->GetString(i, 2, &path);
        pageCount = 0;
        pageSize = 0;
        // try {
        ExecuteForInt64(String("PRAGMA ") + name + String(".page_count;"), NULL, NULL, &pageCount);
        ExecuteForInt64(String("PRAGMA ") + name + String(".page_size;"), NULL, NULL, &pageSize);
        // } catch (SQLiteException ex) {
        //     // Ignore.
        // }
        String label = String("  (attached) ") + name;
        if (!path.IsEmpty()) {
            label += String(": ") + path;
        }
        AutoPtr<SQLiteDebug::DbStats> dbStates = new SQLiteDebug::DbStats(label, pageCount, pageSize, 0, 0, 0, 0);
        dbStatsList->PushBack(dbStates);
    }
    // } catch (SQLiteException ex) {
    //     // Ignore.
    // } finally {
    //     window.close();
    // }
fail:
    ICloseable::Probe(window)->Close();
}

void SQLiteConnection::CollectDbStatsUnsafe(
    /* [in] */ List<AutoPtr<SQLiteDebug::DbStats> >* dbStatsList)
{
    dbStatsList->PushBack(GetMainDbStatsUnsafe(0, 0, 0));
}

AutoPtr<SQLiteDebug::DbStats> SQLiteConnection::GetMainDbStatsUnsafe(
    /* [in] */ Int32 lookaside,
    /* [in] */ Int64 pageCount,
    /* [in] */ Int64 pageSize)
{
    // The prepared statement cache is thread-safe so we can access its statistics
    // even if we do not own the database connection.
    StringBuilder sb(mConfiguration->mPath);
    if (!mIsPrimaryConnection) {
        sb += " (";
        sb += mConnectionId;
        sb += ")";
    }
    AutoPtr<SQLiteDebug::DbStats> dbStates = new SQLiteDebug::DbStats(sb.ToString(), pageCount, pageSize, lookaside,
           mPreparedStatementCache->HitCount(),
           mPreparedStatementCache->MissCount(),
           mPreparedStatementCache->Size());
    return dbStates;
}

String SQLiteConnection::ToString()
{
    StringBuilder sb("SQLiteConnection: ");
    sb += mConfiguration->mPath;
    sb += " (";
    sb += mConnectionId;
    sb += ")";
    return sb.ToString();
}

AutoPtr<SQLiteConnection::PreparedStatement> SQLiteConnection::ObtainPreparedStatement(
    /* [in] */ const String& sql,
    /* [in] */ Int64 statementPtr,
    /* [in] */ Int32 numParameters,
    /* [in] */ Int32 type,
    /* [in] */ Boolean readOnly)
{
    AutoPtr<PreparedStatement> statement = mPreparedStatementPool;
    if (statement != NULL) {
        mPreparedStatementPool = statement->mPoolNext;
        statement->mPoolNext = NULL;
        statement->mInCache = FALSE;
    }
    else {
        statement = new PreparedStatement();
    }
    statement->mSql = sql;
    statement->mStatementPtr = statementPtr;
    statement->mNumParameters = numParameters;
    statement->mType = type;
    statement->mReadOnly = readOnly;
    return statement;
}

void SQLiteConnection::RecyclePreparedStatement(
    /* [in] */ PreparedStatement* statement)
{
    statement->mSql = NULL;
    statement->mPoolNext = mPreparedStatementPool;
    mPreparedStatementPool = statement;
}

String SQLiteConnection::TrimSqlForDisplay(
    /* [in] */ const String& sql)
{
    AutoPtr<IMatcher> matcher;
    TRIM_SQL_PATTERN->Matcher(sql, (IMatcher**)&matcher);
    String str;
    matcher->ReplaceAll(String(" "), &str);
    return str;
}

} //Sqlite
} //Database
} //Droid
} //Elastos
