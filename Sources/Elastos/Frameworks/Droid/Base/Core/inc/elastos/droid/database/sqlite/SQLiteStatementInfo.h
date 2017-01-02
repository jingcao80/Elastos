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

#ifndef __ELASTOS_DROID_DATABASE_SQLITE_SQLITESTATEMENTINFO_H__
#define __ELASTOS_DROID_DATABASE_SQLITE_SQLITESTATEMENTINFO_H__

#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

/**
 * Describes a SQLite statement.
 *
 * @hide
 */
class SQLiteStatementInfo : public Object
{
public:
    SQLiteStatementInfo()
        : mNumParameters(0)
        , mReadOnly(FALSE)
    {}

public:
    /**
     * The number of parameters that the statement has.
     */
    Int32 mNumParameters;

    /**
     * The names of all columns in the result set of the statement.
     */
    AutoPtr< ArrayOf<String> > mColumnNames;

    /**
     * True if the statement is read-only.
     */
    Boolean mReadOnly;
};

} //Sqlite
} //Database
} //Droid
} //Elastos

#endif // __ELASTOS_DROID_DATABASE_SQLITE_SQLITESTATEMENTINFO_H__
