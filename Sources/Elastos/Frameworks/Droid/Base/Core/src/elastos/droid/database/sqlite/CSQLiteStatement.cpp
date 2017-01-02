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

#include "elastos/droid/database/sqlite/CSQLiteStatement.h"

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

CAR_INTERFACE_IMPL(CSQLiteStatement, SQLiteProgram, ISQLiteStatement);

CAR_OBJECT_IMPL(CSQLiteStatement)

ECode CSQLiteStatement::Execute()
{
    AcquireReference();
    // try {
    AutoPtr<SQLiteSession> session;
    GetSession((SQLiteSession**)&session);
    ECode ec = session->Execute(GetSql(), GetBindArgs(), GetConnectionFlags(), NULL);
    if (ec == (ECode)E_SQLITE_DATABASE_CORRUPT_EXCEPTION) {
        OnCorruption();
    }
    // } catch (SQLiteDatabaseCorruptException ex) {
    //     onCorruption();
    //     throw ex;
    // } finally {
    ReleaseReference();
    // }
    return ec;
}

ECode CSQLiteStatement::ExecuteUpdateDelete(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AcquireReference();
    // try {
    AutoPtr<SQLiteSession> session;
    GetSession((SQLiteSession**)&session);
    ECode ec = session->ExecuteForChangedRowCount(GetSql(), GetBindArgs(), GetConnectionFlags(), NULL, result);
    if (ec == (ECode)E_SQLITE_DATABASE_CORRUPT_EXCEPTION) {
        OnCorruption();
    }
    // } catch (SQLiteDatabaseCorruptException ex) {
    //     onCorruption();
    //     throw ex;
    // } finally {
    ReleaseReference();
    // }
    return ec;
}

ECode CSQLiteStatement::ExecuteInsert(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);

    AcquireReference();
    // try {
    AutoPtr<SQLiteSession> session;
    GetSession((SQLiteSession**)&session);
    ECode ec = session->ExecuteForLastInsertedRowId(GetSql(), GetBindArgs(), GetConnectionFlags(), NULL, value);
    if (ec == (ECode)E_SQLITE_DATABASE_CORRUPT_EXCEPTION) {
        OnCorruption();
    }
    // } catch (SQLiteDatabaseCorruptException ex) {
    //     onCorruption();
    //     throw ex;
    // } finally {
    ReleaseReference();
    // }
    return ec;
}

ECode CSQLiteStatement::SimpleQueryForInt64(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);

    AcquireReference();
    // try {
    AutoPtr<SQLiteSession> session;
    GetSession((SQLiteSession**)&session);
    ECode ec = session->ExecuteForInt64(GetSql(), GetBindArgs(), GetConnectionFlags(), NULL, value);
    if (ec == (ECode)E_SQLITE_DATABASE_CORRUPT_EXCEPTION) {
        OnCorruption();
    }
    // } catch (SQLiteDatabaseCorruptException ex) {
    //     onCorruption();
    //     throw ex;
    // } finally {
    ReleaseReference();
    // }
    return ec;
}

ECode CSQLiteStatement::SimpleQueryForString(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    AcquireReference();
    // try {
    AutoPtr<SQLiteSession> session;
    GetSession((SQLiteSession**)&session);
    ECode ec = session->ExecuteForString(GetSql(), GetBindArgs(), GetConnectionFlags(), NULL, value);
    if (ec == (ECode)E_SQLITE_DATABASE_CORRUPT_EXCEPTION) {
        OnCorruption();
    }
    // } catch (SQLiteDatabaseCorruptException ex) {
    //     onCorruption();
    //     throw ex;
    // } finally {
    ReleaseReference();
    // }
    return ec;
}

ECode CSQLiteStatement::SimpleQueryForBlobFileDescriptor(
    /* [out] */ IParcelFileDescriptor** result)
{
    VALIDATE_NOT_NULL(result);

    AcquireReference();
    // try {
    AutoPtr<SQLiteSession> session;
    GetSession((SQLiteSession**)&session);
    ECode ec = session->ExecuteForBlobFileDescriptor(GetSql(), GetBindArgs(), GetConnectionFlags(), NULL, result);
    if (ec == (ECode)E_SQLITE_DATABASE_CORRUPT_EXCEPTION) {
        OnCorruption();
    }
    // } catch (SQLiteDatabaseCorruptException ex) {
    //     onCorruption();
    //     throw ex;
    // } finally {
    ReleaseReference();
    // }
    return ec;
}

ECode CSQLiteStatement::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("SQLiteProgram: ") + GetSql();
    return NOERROR;
}

ECode CSQLiteStatement::constructor(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<IInterface*>* bindArgs)
{
    return SQLiteProgram::constructor(db, sql, bindArgs, NULL);
}

} //Sqlite
} //Database
} //Droid
} //Elastos