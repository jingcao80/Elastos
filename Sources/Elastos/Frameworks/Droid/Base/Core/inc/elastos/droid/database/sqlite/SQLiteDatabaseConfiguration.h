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

#ifndef __ELASTOS_DROID_DATABASE_SQLITE_SQLITEDATABASECONFIGURATION_H__
#define __ELASTOS_DROID_DATABASE_SQLITE_SQLITEDATABASECONFIGURATION_H__

#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>
#include "elastos/droid/database/sqlite/SQLiteCustomFunction.h"
#include <elastos/utility/etl/List.h>

using Elastos::Core::Object;
using Elastos::Utility::ILocale;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

class SQLiteDatabaseConfiguration : public Object
{
public:
    /**
     * Creates a database configuration with the required parameters for opening a
     * database and default values for all other parameters.
     *
     * @param path The database path.
     * @param openFlags Open flags for the database, such as {@link SQLiteDatabase#OPEN_READWRITE}.
     */
    SQLiteDatabaseConfiguration(
        /* [in] */ const String& path,
        /* [in] */ Int32 openFlags);
    /**
     * Creates a database configuration as a copy of another configuration.
     *
     * @param other The other configuration.
     */
    SQLiteDatabaseConfiguration(
        /* [in] */ SQLiteDatabaseConfiguration* other);

    /**
     * Updates the non-immutable parameters of this configuration object
     * from the other configuration object.
     *
     * @param other The object from which to copy the parameters.
     */
    CARAPI UpdateParametersFrom(
        /* [in] */ SQLiteDatabaseConfiguration* other);

    /**
     * Returns true if the database is in-memory.
     * @return True if the database is in-memory.
     */
    CARAPI_(Boolean) IsInMemoryDb();

private:
    static CARAPI_(String) StripPathForLogs(
        /* [in] */ const String& path);

public:
    /**
     * Special path used by in-memory databases.
     */
    static const String MEMORY_DB_PATH;

    /**
     * The database path.
     */
    String mPath;

    /**
     * The label to use to describe the database when it appears in logs.
     * This is derived from the path but is stripped to remove PII.
     */
    String mLabel;

    /**
     * The flags used to open the database.
     */
    Int32 mOpenFlags;

    /**
     * The maximum size of the prepared statement cache for each database connection.
     * Must be non-negative.
     *
     * Default is 25.
     */
    Int32 mMaxSqlCacheSize;

    /**
     * The database locale.
     *
     * Default is the value returned by {@link Locale#getDefault()}.
     */
    AutoPtr<ILocale> mLocale;

    /**
     * True if foreign key constraints are enabled.
     *
     * Default is false.
     */
    Boolean mForeignKeyConstraintsEnabled;

    /**
     * The custom functions to register.
     */
    List<AutoPtr<SQLiteCustomFunction> > mCustomFunctions;

private:
    // The pattern we use to strip email addresses from database paths
    // when constructing a label to use in log messages.
    //static const Pattern EMAIL_IN_DB_PATTERN =
    //        Pattern.compile("[\\w\\.\\-]+@[\\w\\.\\-]+");
    static const AutoPtr<IPattern> EMAIL_IN_DB_PATTERN;
};

} //Sqlite
} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_SQLITE_SQLITEDATABASECONFIGURATION_H__
