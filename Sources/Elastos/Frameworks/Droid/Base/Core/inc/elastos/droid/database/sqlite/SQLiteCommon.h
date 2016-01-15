
#ifndef __ELASTOS_DROID_DATABASE_SQLITE_SQLITECOMMON_H__
#define __ELASTOS_DROID_DATABASE_SQLITE_SQLITECOMMON_H__

#include "Elastos.Droid.Core.h"
#include <sqlite3.h>

// Special log tags defined in SQLiteDebug.java.
#define SQLITE_LOG_TAG "SQLiteLog"
#define SQLITE_TRACE_TAG "SQLiteStatements"
#define SQLITE_PROFILE_TAG "SQLiteTime"

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

/* throw a SQLiteException with a message appropriate for the error in handle */
ECode throw_sqlite3_exception(sqlite3* handle);

/* throw a SQLiteException with the given message */
ECode throw_sqlite3_exception(const char* message);

/* throw a SQLiteException with a message appropriate for the error in handle
   concatenated with the given message
 */
ECode throw_sqlite3_exception(sqlite3* handle, const char* message);

/* throw a SQLiteException for a given error code */
ECode throw_sqlite3_exception_errcode(int errcode, const char* message);

ECode throw_sqlite3_exception(int errcode, const char* sqlite3Message, const char* message);

} //Sqlite
} //Database
} //Droid
} //Elastos

#endif // __ELASTOS_DROID_DATABASE_SQLITE_SQLITECOMMON_H__
