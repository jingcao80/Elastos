
#include "Elastos.Droid.Os.h"
#include "elastos/droid/database/CDatabaseUtils.h"
#include "elastos/droid/database/DatabaseUtils.h"

namespace Elastos {
namespace Droid {
namespace Database {

CAR_INTERFACE_IMPL(CDatabaseUtils, Singleton, IDatabaseUtils)

CAR_SINGLETON_IMPL(CDatabaseUtils)

ECode CDatabaseUtils::WriteExceptionToParcel(
    /* [in] */ IParcel* reply,
    /* [in] */ ECode e)
{
    DatabaseUtils::WriteExceptionToParcel(reply, e);
    return NOERROR;
}

ECode CDatabaseUtils::ReadExceptionFromParcel(
    /* [in] */ IParcel* reply)
{
    DatabaseUtils::ReadExceptionFromParcel(reply);
    return NOERROR;
}

ECode CDatabaseUtils::ReadExceptionWithFileNotFoundExceptionFromParcel(
    /* [in] */ IParcel* reply)
{
    return DatabaseUtils::ReadExceptionWithFileNotFoundExceptionFromParcel(reply);
}

ECode CDatabaseUtils::ReadExceptionWithOperationApplicationExceptionFromParcel(
    /* [in] */ IParcel* reply)
{
    return DatabaseUtils::ReadExceptionWithOperationApplicationExceptionFromParcel(reply);
}

ECode CDatabaseUtils::BindObjectToProgram(
    /* [in] */ ISQLiteProgram* prog,
    /* [in] */ Int32 index,
    /* [in] */ IInterface* value)
{
    return DatabaseUtils::BindObjectToProgram(prog, index, value);
}

ECode CDatabaseUtils::AppendEscapedSQLString(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& sqlString)
{
    DatabaseUtils::AppendEscapedSQLString(sb, sqlString);
    return NOERROR;
}

ECode CDatabaseUtils::SqlEscapeString(
    /* [in] */ const String& value,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = DatabaseUtils::SqlEscapeString(value);
    return NOERROR;
}

ECode CDatabaseUtils::AppendValueToSql(
    /* [in] */ IStringBuilder* sql,
    /* [in] */ IInterface* value)
{
    DatabaseUtils::AppendValueToSql(sql, value);
    return NOERROR;
}

ECode CDatabaseUtils::ConcatenateWhere(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = DatabaseUtils::ConcatenateWhere(a, b);
    return NOERROR;
}

ECode CDatabaseUtils::GetCollationKey(
    /* [in] */ const String& name,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = DatabaseUtils::GetCollationKey(name);
    return NOERROR;
}

ECode CDatabaseUtils::GetHexCollationKey(
    /* [in] */ const String& name,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = DatabaseUtils::GetHexCollationKey(name);
    return NOERROR;
}

ECode CDatabaseUtils::DumpCursor(
    /* [in] */ ICursor* cursor)
{
    DatabaseUtils::DumpCursor(cursor);
    return NOERROR;
}

ECode CDatabaseUtils::DumpCursor(
    /* [in] */ ICursor* cursor,
    /* [in] */ IPrintStream* stream)
{
    DatabaseUtils::DumpCursor(cursor, stream);
    return NOERROR;
}

ECode CDatabaseUtils::DumpCursor(
    /* [in] */ ICursor* cursor,
    /* [in] */ IStringBuilder* sb)
{
    DatabaseUtils::DumpCursor(cursor, sb);
    return NOERROR;
}

ECode CDatabaseUtils::DumpCursorToString(
    /* [in] */ ICursor* cursor,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = DatabaseUtils::DumpCursorToString(cursor);
    return NOERROR;
}

ECode CDatabaseUtils::DumpCurrentRow(
    /* [in] */ ICursor* cursor)
{
    DatabaseUtils::DumpCurrentRow(cursor);
    return NOERROR;
}

ECode CDatabaseUtils::DumpCurrentRow(
    /* [in] */ ICursor* cursor,
    /* [in] */ IPrintStream* stream)
{
    DatabaseUtils::DumpCurrentRow(cursor, stream);
    return NOERROR;
}

ECode CDatabaseUtils::DumpCurrentRow(
    /* [in] */ ICursor* cursor,
    /* [in] */ IStringBuilder* sb)
{
    DatabaseUtils::DumpCurrentRow(cursor, sb);
    return NOERROR;
}

ECode CDatabaseUtils::DumpCurrentRowToString(
    /* [in] */ ICursor* cursor,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = DatabaseUtils::DumpCurrentRowToString(cursor);
    return NOERROR;
}

ECode CDatabaseUtils::CursorStringToContentValues(
    /* [in] */ ICursor* cursor,
    /* [in] */ const String& field,
    /* [in] */ IContentValues* values)
{
    DatabaseUtils::CursorStringToContentValues(cursor, field, values);
    return NOERROR;
}

ECode CDatabaseUtils::CursorStringToInsertHelper(
    /* [in] */ ICursor* cursor,
    /* [in] */ const String& field,
    /* [in] */ IDatabaseInsertHelper* inserter,
    /* [in] */ Int32 index)
{
    DatabaseUtils::CursorStringToInsertHelper(cursor, field, inserter, index);
    return NOERROR;
}

ECode CDatabaseUtils::CursorStringToContentValues(
    /* [in] */ ICursor* cursor,
    /* [in] */ const String& field,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& key)
{
    DatabaseUtils::CursorStringToContentValues(cursor, field, values, key);
    return NOERROR;
}

ECode CDatabaseUtils::CursorInt32ToContentValues(
    /* [in] */ ICursor* cursor,
    /* [in] */ const String& field,
    /* [in] */ IContentValues* values)
{
    DatabaseUtils::CursorInt32ToContentValues(cursor, field, values);
    return NOERROR;
}

ECode CDatabaseUtils::CursorInt32ToContentValues(
    /* [in] */ ICursor* cursor,
    /* [in] */ const String& field,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& key)
{
    DatabaseUtils::CursorInt32ToContentValues(cursor, field, values, key);
    return NOERROR;
}

ECode CDatabaseUtils::CursorInt64ToContentValues(
    /* [in] */ ICursor* cursor,
    /* [in] */ const String& field,
    /* [in] */ IContentValues* values)
{
    DatabaseUtils::CursorInt64ToContentValues(cursor, field, values);
    return NOERROR;
}

ECode CDatabaseUtils::CursorInt64ToContentValues(
    /* [in] */ ICursor* cursor,
    /* [in] */ const String& field,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& key)
{
    DatabaseUtils::CursorInt64ToContentValues(cursor, field, values, key);
    return NOERROR;
}

ECode CDatabaseUtils::CursorDoubleToCursorValues(
    /* [in] */ ICursor* cursor,
    /* [in] */ const String& field,
    /* [in] */ IContentValues* values)
{
    DatabaseUtils::CursorDoubleToCursorValues(cursor, field, values);
    return NOERROR;
}

ECode CDatabaseUtils::CursorDoubleToContentValues(
    /* [in] */ ICursor* cursor,
    /* [in] */ const String& field,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& key)
{
    DatabaseUtils::CursorDoubleToContentValues(cursor, field, values, key);
    return NOERROR;
}

ECode CDatabaseUtils::CursorRowToContentValues(
    /* [in] */ ICursor* cursor,
    /* [in] */ IContentValues* values)
{
    DatabaseUtils::CursorRowToContentValues(cursor, values);
    return NOERROR;
}

ECode CDatabaseUtils::QueryNumEntries(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& table,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = DatabaseUtils::QueryNumEntries(db, table);
    return NOERROR;
}

ECode CDatabaseUtils::QueryNumEntries(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& table,
    /* [in] */ const String& selection,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = DatabaseUtils::QueryNumEntries(db, table, selection);
    return NOERROR;
}

ECode CDatabaseUtils::QueryNumEntries(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& table,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = DatabaseUtils::QueryNumEntries(db, table, selection, selectionArgs);
    return NOERROR;
}

ECode CDatabaseUtils::Int64ForQuery(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& query,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = DatabaseUtils::Int64ForQuery(db, query, selectionArgs);
    return NOERROR;
}

ECode CDatabaseUtils::QueryIsEmpty(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& table,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = DatabaseUtils::QueryIsEmpty(db, table);
    return NOERROR;
}

ECode CDatabaseUtils::Int64ForQuery(
    /* [in] */ ISQLiteStatement* prog,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = DatabaseUtils::Int64ForQuery(prog, selectionArgs);
    return NOERROR;
}

ECode CDatabaseUtils::StringForQuery(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& query,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = DatabaseUtils::StringForQuery(db, query, selectionArgs);
    return NOERROR;
}

ECode CDatabaseUtils::StringForQuery(
    /* [in] */ ISQLiteStatement* prog,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = DatabaseUtils::StringForQuery(prog, selectionArgs);
    return NOERROR;
}

ECode CDatabaseUtils::BlobFileDescriptorForQuery(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& query,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ IParcelFileDescriptor** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IParcelFileDescriptor> desc = DatabaseUtils::BlobFileDescriptorForQuery(db, query, selectionArgs);
    *result = desc;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDatabaseUtils::BlobFileDescriptorForQuery(
    /* [in] */ ISQLiteStatement* prog,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ IParcelFileDescriptor** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IParcelFileDescriptor> desc = DatabaseUtils::BlobFileDescriptorForQuery(prog, selectionArgs);
    *result = desc;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDatabaseUtils::CursorStringToContentValuesIfPresent(
    /* [in] */ ICursor* cursor,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& column)
{
    DatabaseUtils::CursorStringToContentValuesIfPresent(cursor, values, column);
    return NOERROR;
}

ECode CDatabaseUtils::CursorInt64ToContentValuesIfPresent(
    /* [in] */ ICursor* cursor,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& column)
{
    DatabaseUtils::CursorInt64ToContentValuesIfPresent(cursor, values, column);
    return NOERROR;
}

ECode CDatabaseUtils::CursorInt16ToContentValuesIfPresent(
    /* [in] */ ICursor* cursor,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& column)
{
    DatabaseUtils::CursorInt16ToContentValuesIfPresent(cursor, values, column);
    return NOERROR;
}

ECode CDatabaseUtils::CursorInt32ToContentValuesIfPresent(
    /* [in] */ ICursor* cursor,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& column)
{
    DatabaseUtils::CursorInt32ToContentValuesIfPresent(cursor, values, column);
    return NOERROR;
}

ECode CDatabaseUtils::CursorFloatToContentValuesIfPresent(
    /* [in] */ ICursor* cursor,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& column)
{
    DatabaseUtils::CursorFloatToContentValuesIfPresent(cursor, values, column);
    return NOERROR;
}

ECode CDatabaseUtils::CursorDoubleToContentValuesIfPresent(
    /* [in] */ ICursor* cursor,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& column)
{
    DatabaseUtils::CursorDoubleToContentValuesIfPresent(cursor, values, column);
    return NOERROR;
}

ECode CDatabaseUtils::CreateDbFromSqlStatements(
    /* [in] */ IContext* context,
    /* [in] */ const String& dbName,
    /* [in] */ Int32 dbVersion,
    /* [in] */ const String& sqlStatements)
{
    DatabaseUtils::CreateDbFromSqlStatements(context, dbName, dbVersion, sqlStatements);
    return NOERROR;
}

ECode CDatabaseUtils::GetSqlStatementType(
    /* [in] */ const String& sql,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    *type = DatabaseUtils::GetSqlStatementType(sql);
    return NOERROR;
}

ECode CDatabaseUtils::AppendSelectionArgs(
    /* [in] */ ArrayOf<String>* originalValues,
    /* [in] */ ArrayOf<String>* newValues,
    /* [out,callee] */ ArrayOf<String>** args)
{
    VALIDATE_NOT_NULL(args)
    AutoPtr< ArrayOf<String> > array = DatabaseUtils::AppendSelectionArgs(originalValues, newValues);
    *args = array;
    REFCOUNT_ADD(*args)
    return NOERROR;
}

} //Database
} //Droid
} //Elastos
