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

#ifndef __ELASTOS_DROID_DATABASE_SQLITE_SQLITEDEBUGHELPER_H__
#define __ELASTOS_DROID_DATABASE_SQLITE_SQLITEDEBUGHELPER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Core.h"
#include <elastos/utility/etl/List.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IPrinter;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

/**
 * Provides debugging info about all SQLite databases running in the current process.
 *
 * {@hide}
 */
class SQLiteDebug : public Object
{
public:
    /**
     * contains statistics about a database
     */
    class DbStats : public Object
    {
    public:
        DbStats(
            /* [in] */ const String& dbName,
            /* [in] */ Int64 pageCount,
            /* [in] */ Int64 pageSize,
            /* [in] */ Int32 lookaside,
            /* [in] */ Int32 hits,
            /* [in] */ Int32 misses,
            /* [in] */ Int32 cachesize);

    public:
        /** name of the database */
        String mDbName;

        /** the page size for the database */
        Int64 mPageSize;

        /** the database size */
        Int64 mDbSize;

        /** documented here http://www.sqlite.org/c3ref/c_dbstatus_lookaside_used.html */
        Int32 mLookaside;

        /** statement cache stats: hits/misses/cachesize */
        String mCache;
    };

    /**
     * Contains statistics about the active pagers in the current process.
     *
     * @see #getPagerStats(PagerStats)
     */
    class PagerStats : public Object
    {
    public:
        PagerStats()
            : mMemoryUsed(0)
            , mPageCacheOverflow(0)
            , mLargestMemAlloc(0)
            , mDbStats(NULL)
        {}

        ~PagerStats();

    public:
        /** the current amount of memory checked out by sqlite using sqlite3_malloc().
         * documented at http://www.sqlite.org/c3ref/c_status_malloc_size.html
         */
        Int32 mMemoryUsed;

        /** the number of bytes of page cache allocation which could not be sattisfied by the
         * SQLITE_CONFIG_PAGECACHE buffer and where forced to overflow to sqlite3_malloc().
         * The returned value includes allocations that overflowed because they where too large
         * (they were larger than the "sz" parameter to SQLITE_CONFIG_PAGECACHE) and allocations
         * that overflowed because no space was left in the page cache.
         * documented at http://www.sqlite.org/c3ref/c_status_malloc_size.html
         */
        Int32 mPageCacheOverflow;

        /** records the largest memory allocation request handed to sqlite3.
         * documented at http://www.sqlite.org/c3ref/c_status_malloc_size.html
         */
        Int32 mLargestMemAlloc;

        /** a list of {@link DbStats} - one for each main database opened by the applications
         * running on the android device
         */
        AutoPtr< List<AutoPtr<DbStats> > > mDbStats;
    };

public:
    /**
     * Determines whether a query should be logged.
     *
     * Reads the "db.log.slow_query_threshold" system property, which can be changed
     * by the user at any time.  If the value is zero, then all queries will
     * be considered slow.  If the value does not exist or is negative, then no queries will
     * be considered slow.
     *
     * This value can be changed dynamically while the system is running.
     * For example, "adb shell setprop db.log.slow_query_threshold 200" will
     * log all queries that take 200ms or longer to run.
     * @hide
     */
    static CARAPI_(Boolean) ShouldLogSlowQuery(
        /* [in] */ Int64 elapsedTimeMillis);

    /**
     * return all pager and database stats for the current process.
     * @return {@link PagerStats}
     */
    static CARAPI_(AutoPtr<PagerStats>) GetDatabaseInfo();

    /**
     * Dumps detailed information about all databases used by the process.
     * @param printer The printer for dumping database state.
     * @param args Command-line arguments supplied to dumpsys dbinfo
     */
    static CARAPI_(void) Dump(
        /* [in] */ IPrinter* printer,
        /* [in] */ const ArrayOf<String>& args);

private:
    SQLiteDebug();

    static CARAPI_(void) NativeGetPagerStats(
        /* [in] */ PagerStats* stats);

public:
    /**
     * Controls the printing of informational SQL log messages.
     *
     * Enable using "adb shell setprop log.tag.SQLiteLog VERBOSE".
     */
    static const Boolean DEBUG_SQL_LOG;
    /**
     * Controls the printing of SQL statements as they are executed.
     *
     * Enable using "adb shell setprop log.tag.SQLiteStatements VERBOSE".
     */
    static const Boolean DEBUG_SQL_STATEMENTS;

    /**
     * Controls the printing of wall-clock time taken to execute SQL statements
     * as they are executed.
     *
     * Enable using "adb shell setprop log.tag.SQLiteTime VERBOSE".
     */
    static const Boolean DEBUG_SQL_TIME;

    /**
     * True to enable database performance testing instrumentation.
     * @hide
     */
    static const Boolean DEBUG_LOG_SLOW_QUERIES;
};

} //Sqlite
} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_SQLITE_SQLITEDEBUGHELPER_H__
