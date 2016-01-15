#ifndef __ELASTOS_DROID_DATABASE_SQLITE_SQLITEGLOBAL_H__
#define __ELASTOS_DROID_DATABASE_SQLITE_SQLITEGLOBAL_H__

#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

/**
 * Provides access to SQLite functions that affect all database connection,
 * such as memory management.
 *
 * The native code associated with SQLiteGlobal is also sets global configuration options
 * using sqlite3_config() then calls sqlite3_initialize() to ensure that the SQLite
 * library is properly initialized exactly once before any other framework or application
 * code has a chance to run.
 *
 * Verbose SQLite logging is enabled if the "log.tag.SQLiteLog" property is set to "V".
 * (per {@link SQLiteDebug#DEBUG_SQL_LOG}).
 *
 * @hide
 */
class SQLiteGlobal : public Object
{
public:
    /**
     * Attempts to release memory by pruning the SQLite page cache and other
     * internal data structures.
     *
     * @return The number of bytes that were freed.
     */
    static CARAPI_(Int32) ReleaseMemory();

    /**
     * Gets the default page size to use when creating a database.
     */
    static CARAPI_(Int32) GetDefaultPageSize();

    /**
     * Gets the default journal mode when WAL is not in use.
     */
    static CARAPI_(String) GetDefaultJournalMode();

    /**
     * Gets the journal size limit in bytes.
     */
    static CARAPI_(Int32) GetJournalSizeLimit();

    /**
     * Gets the default database synchronization mode when WAL is not in use.
     */
    static CARAPI_(String) GetDefaultSyncMode();

    /**
     * Gets the database synchronization mode when in WAL mode.
     */
    static CARAPI_(String) GetWALSyncMode();

    /**
     * Gets the WAL auto-checkpoint integer in database pages.
     */
    static CARAPI_(Int32) GetWALAutoCheckpoint();

    /**
     * Gets the connection pool size when in WAL mode.
     */
    static CARAPI_(Int32) GetWALConnectionPoolSize();

private:
    SQLiteGlobal() {}

    static CARAPI_(Int32) NativeReleaseMemory();

private:
    static const String TAG;
    static Object mLock;
    static Int32 sDefaultPageSize;
};

} //Sqlite
} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_SQLITE_SQLITEGLOBAL_H__
