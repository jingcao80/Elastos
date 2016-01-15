
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

ECode CSQLiteStatement::AcquireReference()
{
    return SQLiteProgram::AcquireReference();
}

ECode CSQLiteStatement::ReleaseReference()
{
    return SQLiteProgram::ReleaseReference();
}

ECode CSQLiteStatement::ReleaseReferenceFromContainer()
{
    return SQLiteProgram::ReleaseReferenceFromContainer();
}

ECode CSQLiteStatement::Close()
{
    return SQLiteProgram::Close();
}

ECode CSQLiteStatement::GetUniqueId(
    /* [out] */ Int32 *value)
{
    VALIDATE_NOT_NULL(value);
    return SQLiteProgram::GetUniqueId(value);
}

ECode CSQLiteStatement::BindNull(
    /* [in] */ Int32 index)
{
    return SQLiteProgram::BindNull(index);
}

ECode CSQLiteStatement::BindInt64(
    /* [in] */ Int32 index,
    /* [in] */ Int64 value)
{
    return SQLiteProgram::BindInt64(index, value);
}

ECode CSQLiteStatement::BindDouble(
    /* [in] */ Int32 index,
    /* [in] */ Double value)
{
    return SQLiteProgram::BindDouble(index, value);
}

ECode CSQLiteStatement::BindString(
    /* [in] */ Int32 index,
    /* [in] */ const String& value)
{
    return SQLiteProgram::BindString(index, value);
}

ECode CSQLiteStatement::BindBlob(
    /* [in] */ Int32 index,
    /* [in] */ const ArrayOf<Byte>& value)
{
    return SQLiteProgram::BindBlob(index, value);
}

ECode CSQLiteStatement::ClearBindings()
{
    return SQLiteProgram::ClearBindings();
}

ECode CSQLiteStatement::BindAllArgsAsStrings(
    /* [in] */ ArrayOf<String>* bindArgs)
{
    return SQLiteProgram::BindAllArgsAsStrings(bindArgs);
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
    return SQLiteProgram::Init(db, sql, bindArgs, NULL);
}

} //Sqlite
} //Database
} //Droid
} //Elastos