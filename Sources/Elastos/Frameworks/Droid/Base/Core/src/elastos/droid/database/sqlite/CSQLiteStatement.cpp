
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