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

#include "elastos/droid/database/sqlite/SQLiteDirectCursorDriver.h"
#include "elastos/droid/database/sqlite/CSQLiteQuery.h"
#include "elastos/droid/database/sqlite/CSQLiteCursor.h"

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

CAR_INTERFACE_IMPL(SQLiteDirectCursorDriver, Object, ISQLiteCursorDriver)

SQLiteDirectCursorDriver::SQLiteDirectCursorDriver(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& sql,
    /* [in] */ const String& editTable,
    /* [in] */ ICancellationSignal* cancellationSignal)
    : mDatabase(db)
    , mEditTable(editTable)
    , mSql(sql)
    , mCancellationSignal(cancellationSignal)
{}

ECode SQLiteDirectCursorDriver::Query(
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ ICursor** cs)
{
    VALIDATE_NOT_NULL(cs)
    *cs = NULL;

    AutoPtr<ISQLiteQuery> query;
    FAIL_RETURN(CSQLiteQuery::New(mDatabase, mSql, NULL, (ISQLiteQuery**)&query));
    AutoPtr<ICursor> cursor;
    //try {
    ECode ec = ISQLiteProgram::Probe(query)->BindAllArgsAsStrings(selectionArgs);
    if (FAILED(ec)) {
        ICloseable::Probe(query)->Close();
        return ec;
    }

    if (factory == NULL) {
        ec = CSQLiteCursor::New(this, mEditTable, query, (ICursor**)&cursor);
    }
    else {
        ec = factory->NewCursor(mDatabase, this, mEditTable, query, (ICursor**)&cursor);
    }
    if (FAILED(ec)) {
        ICloseable::Probe(query)->Close();
        return ec;
    }
    // } catch (RuntimeException ex) {
    //     query.close();
    //     throw ex;
    // }
    mQuery = query;
    *cs = cursor;
    REFCOUNT_ADD(*cs)
    return NOERROR;
}

ECode SQLiteDirectCursorDriver::CursorClosed()
{
    // Do nothing
    return NOERROR;
}

ECode SQLiteDirectCursorDriver::SetBindArguments(
    /* [in] */ ArrayOf<String>* bindArgs)
{
    return ISQLiteProgram::Probe(mQuery)->BindAllArgsAsStrings(bindArgs);;
}

ECode SQLiteDirectCursorDriver::CursorDeactivated()
{
    // Do nothing
    return NOERROR;
}

ECode SQLiteDirectCursorDriver::CursorRequeried(
        /* [in] */ ICursor* cursor)
{
    // Do nothing
    return NOERROR;
}

String SQLiteDirectCursorDriver::ToString()
{
    return String("SQLiteDirectCursorDriver: ") + mSql;
}

} //Sqlite
} //Database
} //Droid
} //Elastos
