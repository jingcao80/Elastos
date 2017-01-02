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

#ifndef __ELASTOS_DROID_DATABASE_SQLITE_SQLITECUSTOMFUNCTION_H__
#define __ELASTOS_DROID_DATABASE_SQLITE_SQLITECUSTOMFUNCTION_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/database/sqlite/SQLiteDatabase.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

/**
 * Describes a custom SQL function.
 *
 * @hide
 */
class SQLiteCustomFunction : public Object
{
public:
    /**
     * Create custom function.
     *
     * @param name The name of the sqlite3 function.
     * @param numArgs The number of arguments for the function, or -1 to
     * support any number of arguments.
     * @param callback The callback to invoke when the function is executed.
     */
    SQLiteCustomFunction(
        /* [in] */ const String& name,
        /* [in] */ Int32 numArgs,
        /* [in] */ ICustomFunction* callBack);

    // Called from native.
    //@SuppressWarnings("unused")
    CARAPI_(void) DispatchCallback(
        /* [in] */ ArrayOf<String>* args);

public:
    String mName;
    Int32 mNumArgs;
    AutoPtr<ICustomFunction> mCallback;
};

} //Sqlite
} //Database
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_DATABASE_SQLITE_SQLITECUSTOMFUNCTION_H__
