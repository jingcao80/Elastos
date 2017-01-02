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

#ifndef __ELASTOS_DROID_DATABASE_CDEFAULTDATABASEERRORHANDLER_H__
#define __ELASTOS_DROID_DATABASE_CDEFAULTDATABASEERRORHANDLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Database_CDefaultDatabaseErrorHandler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;

namespace Elastos {
namespace Droid {
namespace Database {
/**
 * Default class used to define the actions to take when the database corruption is reported
 * by sqlite.
 * <p>
 * An application can specify an implementation of {@link DatabaseErrorHandler} on the
 * following:
 * <ul>
 *   <li>{@link SQLiteDatabase#openOrCreateDatabase(String,
 *      android.database.sqlite.SQLiteDatabase.CursorFactory, DatabaseErrorHandler)}</li>
 *   <li>{@link SQLiteDatabase#openDatabase(String,
 *      android.database.sqlite.SQLiteDatabase.CursorFactory, int, DatabaseErrorHandler)}</li>
 * </ul>
 * The specified {@link DatabaseErrorHandler} is used to handle database corruption errors, if they
 * occur.
 * <p>
 * If null is specified for DatabaeErrorHandler param in the above calls, then this class is used
 * as the default {@link DatabaseErrorHandler}.
 */
CarClass(CDefaultDatabaseErrorHandler)
    , public Object
    , public IDatabaseErrorHandler
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * defines the default method to be invoked when database corruption is detected.
     * @param dbObj the {@link SQLiteDatabase} object representing the database on which corruption
     * is detected.
     */
    CARAPI OnCorruption(
        /* [in] */ ISQLiteDatabase* dbObj);

private:
    CARAPI_(void) DeleteDatabaseFile(
        /* [in] */ const String& fileName);

private:
    static const String TAG;
};

} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_CDEFAULTDATABASEERRORHANDLER_H__
