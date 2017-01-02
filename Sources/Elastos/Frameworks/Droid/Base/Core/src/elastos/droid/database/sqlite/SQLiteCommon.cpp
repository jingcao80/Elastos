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

#include "elastos/droid/database/sqlite/SQLiteCommon.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

/* throw a SQLiteException with a message appropriate for the error in handle */
ECode throw_sqlite3_exception(sqlite3* handle) {
    return throw_sqlite3_exception(handle, NULL);
}

/* throw a SQLiteException with the given message */
ECode throw_sqlite3_exception(const char* message) {
    return throw_sqlite3_exception(NULL, message);
}

/* throw a SQLiteException with a message appropriate for the error in handle
   concatenated with the given message
 */
ECode throw_sqlite3_exception(sqlite3* handle, const char* message) {
    if (handle) {
        // get the error code and message from the SQLite connection
        // the error message may contain more information than the error code
        // because it is based on the extended error code rather than the simplified
        // error code that SQLite normally returns.
        return throw_sqlite3_exception(sqlite3_extended_errcode(handle),
                sqlite3_errmsg(handle), message);
    }
    else {
        // we use SQLITE_OK so that a generic SQLiteException is thrown;
        // any code not specified in the switch statement below would do.
        return throw_sqlite3_exception(SQLITE_OK, "unknown error", message);
    }
}

/* throw a SQLiteException for a given error code
 * should only be used when the database connection is not available because the
 * error information will not be quite as rich */
ECode throw_sqlite3_exception_errcode(int errcode, const char* message) {
    return throw_sqlite3_exception(errcode, "unknown error", message);
}

/* throw a SQLiteException for a given error code, sqlite3message, and
   user message
 */
ECode throw_sqlite3_exception(int errcode, const char* sqlite3Message, const char* message) {
    ECode ec = NOERROR;
    char* temp = const_cast<char*>(sqlite3Message);
    switch (errcode & 0xff) { /* mask off extended error code */
    case SQLITE_IOERR:
        ec = E_SQLITE_DISK_IO_EXCEPTION;
        break;
    case SQLITE_CORRUPT:
    case SQLITE_NOTADB: // treat "unsupported file format" error as corruption also
        ec = E_SQLITE_DATABASE_CORRUPT_EXCEPTION;
        break;
    case SQLITE_CONSTRAINT:
        ec = E_SQLITE_CONSTRAINT_EXCEPTION;
        break;
    case SQLITE_ABORT:
        ec = E_SQLITE_ABORT_EXCEPTION;
        break;
    case SQLITE_DONE:
        ec = E_SQLITE_DONE_EXCEPTION;
        temp = NULL; // SQLite error message is irrelevant in this case
        break;
    case SQLITE_FULL:
        ec = E_SQLITE_FULL_EXCEPTION;
        break;
    case SQLITE_MISUSE:
        ec = E_SQLITE_MISUSE_EXCEPTION;
        break;
    case SQLITE_PERM:
        ec = E_SQLITE_ACCESS_PERM_EXCEPTION;
        break;
    case SQLITE_BUSY:
        ec = E_SQLITE_DATABASE_LOCKED_EXCEPTION;
        break;
    case SQLITE_LOCKED:
        ec = E_SQLITE_TABLE_LOCKED_EXCEPTION;
        break;
    case SQLITE_READONLY:
        ec = E_SQLITE_READONLY_DATABASE_EXCEPTION;
        break;
    case SQLITE_CANTOPEN:
        ec = E_SQLITE_CANT_OPEN_DATABASE_EXCEPTION;
        break;
    case SQLITE_TOOBIG:
        ec = E_SQLITE_BLOB_TOO_BIG_EXCEPTION;
        break;
    case SQLITE_RANGE:
        ec = E_SQLITE_BIND_OR_COLUMN_INDEX_OUTOF_RANGE_EXCEPTION;
        break;
    case SQLITE_NOMEM:
        ec = E_SQLITE_OUTOF_MEMORY_EXCEPTION;
        break;
    case SQLITE_MISMATCH:
        ec = E_SQLITE_DATA_TYPE_MISMATCH_EXCEPTION;
        break;
    case SQLITE_INTERRUPT:
        ec = E_OPERATION_CANCELED_EXCEPTION;
        break;
    default:
        ec = E_SQLITE_EXCEPTION;
        break;
    }

    if (temp) {
        StringBuilder fullMessage(sqlite3Message);
        fullMessage.Append(" (code %d)");
        fullMessage.Append(errcode); // print extended error code
        if (message) {
            fullMessage.Append(": ");
            fullMessage.Append(message);
        }
        Slogger::E("SQLiteException", "%s", fullMessage.ToString().string());
        return ec;
    }
    else {
        Slogger::E("SQLiteException", "%s", message);
        return ec;
    }
}

} //Sqlite
} //Database
} //Droid
} //Elastos
