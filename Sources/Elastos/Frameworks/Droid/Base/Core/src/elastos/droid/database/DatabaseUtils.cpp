
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Text.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/database/DatabaseUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::Math;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::IInteger16;
using Elastos::Core::CInteger16;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::Core::IDouble;
using Elastos::Core::CDouble;
using Elastos::Core::IFloat;
using Elastos::Core::CFloat;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Core::INumber;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::EIID_IByte;
using Elastos::IO::ICloseable;
using Elastos::Text::ICollatorHelper;
//using Elastos::Text::CCollatorHelper;
using Elastos::Text::ICollationKey;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Database {

const Int32 DatabaseUtils::InsertHelper::TABLE_INFO_PRAGMA_COLUMNNAME_INDEX;
const Int32 DatabaseUtils::InsertHelper::TABLE_INFO_PRAGMA_DEFAULT_INDEX;

ECode DatabaseUtils::InsertHelper::constructor(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& tableName)
{
    mDb = db;
    mTableName = tableName;
    return NOERROR;
}

DatabaseUtils::InsertHelper::~InsertHelper()
{
    mColumns = NULL;
}

ECode DatabaseUtils::InsertHelper::BuildSQL()
{
    StringBuilder sb(128);
    sb += "INSERT INTO ";
    sb += mTableName;
    sb += " (";

    StringBuilder sbv(128);
    sbv += "VALUES (";

    Int32 i = 1;
    AutoPtr<ICursor> cur;
    //try {
    StringBuilder sql("PRAGMA table_info(");
    sql += mTableName;
    sql += ")";
    FAIL_RETURN(mDb->RawQuery(sql.ToString(), NULL, (ICursor**)&cur))
    Int32 count;
    cur->GetCount(&count);
    mColumns = new HashMap<String, Int32>(count);
    Boolean moveToNext;
    while (cur->MoveToNext(&moveToNext), moveToNext) {
        String columnName, defaultValue;
        ECode ec = cur->GetString(TABLE_INFO_PRAGMA_COLUMNNAME_INDEX, &columnName);
        if (FAILED(ec)) {
            ICloseable::Probe(cur)->Close();
            return ec;
        }
        ec = cur->GetString(TABLE_INFO_PRAGMA_DEFAULT_INDEX, &defaultValue);
        if (FAILED(ec)) {
            ICloseable::Probe(cur)->Close();
            return ec;
        }

        (*mColumns)[columnName] = i;
        sb += "'";
        sb += columnName;
        sb += "'";

        if (defaultValue.IsNull()) {
            sbv += "?";
        }
        else {
            sbv += "COALESCE(?, ";
            sbv += defaultValue;
            sbv += ")";
        }

        sb += (i == count ? ") " : ", ");
        sbv += (i == count ? ");" : ", ");
        ++i;
    }
    //} finally {
    if (cur != NULL) {
        FAIL_RETURN(ICloseable::Probe(cur)->Close())
    }
    //}

    sb += sbv;

    mInsertSQL = sb.ToString();
    if (DEBUG) {
        Slogger::V(TAG, "insert statement is %s", mInsertSQL.string());
    }
    return NOERROR;
}

AutoPtr<ISQLiteStatement> DatabaseUtils::InsertHelper::GetStatement(
    /* [in] */ Boolean allowReplace)
{
    if (allowReplace) {
        if (mReplaceStatement == NULL) {
            if (mInsertSQL == NULL) {
                BuildSQL();
            }
            // chop "INSERT" off the front and prepend "INSERT OR REPLACE" instead.
            String replaceSQL = String("INSERT OR REPLACE") + mInsertSQL.Substring(6);
            mDb->CompileStatement(replaceSQL, (ISQLiteStatement**)&mReplaceStatement);
        }
        return mReplaceStatement;
    }
    else {
        if (mInsertStatement == NULL) {
            if (mInsertSQL == NULL) {
                BuildSQL();
            }
            mDb->CompileStatement(mInsertSQL, (ISQLiteStatement**)&mInsertStatement);
        }
        return mInsertStatement;
    }
}

Int64 DatabaseUtils::InsertHelper::InsertInternal(
    /* [in] */ IContentValues* values,
    /* [in] */ Boolean allowReplace)
{
    // Start a transaction even though we don't really need one.
    // This is to help maintain compatibility with applications that
    // access InsertHelper from multiple threads even though they never should have.
    // The original code used to lock the InsertHelper itself which was prone
    // to deadlocks.  Starting a transaction achieves the same mutual exclusion
    // effect as grabbing a lock but without the potential for deadlocks.
    mDb->BeginTransactionNonExclusive();
    //try {
    AutoPtr<ISQLiteStatement> stmt = GetStatement(allowReplace);
    ISQLiteProgram::Probe(stmt)->ClearBindings();
    if (DEBUG) {
        Slogger::V(TAG, "--- inserting in table %s", mTableName.string());
    }

    AutoPtr<ISet> bsm;
    values->GetKeySet((ISet**)&bsm);
    AutoPtr<IIterator> it;
    bsm->GetIterator((IIterator**)&it);
    Boolean hasNext = FALSE;
    String key;
    Int32 i;
    AutoPtr<ISQLiteProgram> value;
    while ((it->HasNext(&hasNext), hasNext)) {
        AutoPtr<IInterface> outface;
        it->GetNext((IInterface**)&outface);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(outface);
        AutoPtr<IInterface> obj;
        entry->GetKey((IInterface**)&obj);
        assert(ICharSequence::Probe(obj) != NULL);
        ICharSequence::Probe(obj)->ToString(&key);

        value = NULL;
        values->Get(key, (IInterface**)&value);
        GetColumnIndex(key, &i);
        if (DatabaseUtils::BindObjectToProgram(ISQLiteProgram::Probe(stmt), i, value) == (ECode)E_SQLITE_EXCEPTION) {
            Slogger::E(TAG, "Error inserting %p into table  %s", values, mTableName.string());
            mDb->EndTransaction();
            return -1;
        }
        if (DEBUG) {
            Slogger::V(TAG, "binding %p to column %d (%s)", value.Get(), i, key.string());
        }
    }

    Int64 result;
    if (stmt->ExecuteInsert(&result) == (ECode)E_SQLITE_EXCEPTION) {
        Slogger::E(TAG, "Error inserting %p into table  %s", values, mTableName.string());
        mDb->EndTransaction();
        return -1;
    }
    mDb->SetTransactionSuccessful();
    //} catch (SQLException e) {
        // Log.e(TAG, "Error inserting " + values + " into table  " + mTableName, e);
        // return -1;
    //} finally {
    mDb->EndTransaction();
    //}
    return result;
}

ECode DatabaseUtils::InsertHelper::GetColumnIndex(
    /* [in] */ const String& key,
    /* [out] */ Int32* columnIndex)
{
    VALIDATE_NOT_NULL(columnIndex)

    AutoPtr<ISQLiteStatement> stmt = GetStatement(FALSE);
    HashMap<String, Int32>::Iterator it = mColumns->Find(key);
    if (it == mColumns->End()) {
        //throw new IllegalArgumentException("column '" + key + "' is invalid");
        Slogger::E(TAG, "column '%s' is invalid", key.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *columnIndex = it->mSecond;
    return NOERROR;
}

ECode DatabaseUtils::InsertHelper::BindDouble(
    /* [in] */ Int32 index,
    /* [in] */ Double value)
{
    return ISQLiteProgram::Probe(mPreparedStatement)->BindDouble(index, value);
}

ECode DatabaseUtils::InsertHelper::BindFloat(
    /* [in] */ Int32 index,
    /* [in] */ Float value)
{
    return ISQLiteProgram::Probe(mPreparedStatement)->BindDouble(index, value);
}

ECode DatabaseUtils::InsertHelper::BindInt64(
    /* [in] */ Int32 index,
    /* [in] */ Int64 value)
{
    return ISQLiteProgram::Probe(mPreparedStatement)->BindInt64(index, value);
}

ECode DatabaseUtils::InsertHelper::BindInt32(
    /* [in] */ Int32 index,
    /* [in] */ Int32 value)
{
    return ISQLiteProgram::Probe(mPreparedStatement)->BindInt64(index, value);
}

ECode DatabaseUtils::InsertHelper::BindBoolean(
    /* [in] */ Int32 index,
    /* [in] */ Boolean value)
{
    return ISQLiteProgram::Probe(mPreparedStatement)->BindInt64(index, value ? 1 : 0);
}

ECode DatabaseUtils::InsertHelper::BindNull(
    /* [in] */ Int32 index)
{
    return ISQLiteProgram::Probe(mPreparedStatement)->BindNull(index);
}

ECode DatabaseUtils::InsertHelper::BindBytes(
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Byte>* value)
{
    if (value == NULL) {
        return ISQLiteProgram::Probe(mPreparedStatement)->BindNull(index);
    }
    else {
        return ISQLiteProgram::Probe(mPreparedStatement)->BindBlob(index, *value);
    }
}

ECode DatabaseUtils::InsertHelper::BindString(
    /* [in] */ Int32 index,
    /* [in] */ const String& value)
{
    if (value.IsNull()) {
        return ISQLiteProgram::Probe(mPreparedStatement)->BindNull(index);
    }
    else {
        return ISQLiteProgram::Probe(mPreparedStatement)->BindString(index, value);
    }
}

ECode DatabaseUtils::InsertHelper::Insert(
    /* [in] */ IContentValues* values,
    /* [out] */ Int64* rowId)
{
    VALIDATE_NOT_NULL(rowId)
    *rowId = InsertInternal(values, FALSE);
    return NOERROR;
}

ECode DatabaseUtils::InsertHelper::Execute(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)

    if (mPreparedStatement == NULL) {
        //throw new IllegalStateException("you must prepare this inserter before calling " + "execute");
        Slogger::E(TAG, "you must prepare this inserter before calling execute");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    //try {
    if (DEBUG) {
        Slogger::V(TAG, "--- doing insert or replace in table %s", mTableName.string());
    }
    ECode ec = mPreparedStatement->ExecuteInsert(value);
    //} catch (SQLException e) {
    if(ec == (ECode)E_SQLITE_EXCEPTION) {
        Slogger::E(TAG, "Error executing InsertHelper with table %s, 0x%08x", mTableName.string(), ec);
        *value = -1;
    }
    //} finally {
    // you can only call this once per prepare
    mPreparedStatement = NULL;
    //}
    return ec;
}

ECode DatabaseUtils::InsertHelper::PrepareForInsert()
{
    mPreparedStatement = GetStatement(FALSE);
    return ISQLiteProgram::Probe(mPreparedStatement)->ClearBindings();
}

ECode DatabaseUtils::InsertHelper::PrepareForReplace()
{
    mPreparedStatement = GetStatement(TRUE);
    return ISQLiteProgram::Probe(mPreparedStatement)->ClearBindings();
}

ECode DatabaseUtils::InsertHelper::Replace(
    /* [in] */ IContentValues* values,
    /* [out] */ Int64* rowId)
{
    VALIDATE_NOT_NULL(rowId)
    *rowId = InsertInternal(values, TRUE);
    return NOERROR;
}

ECode DatabaseUtils::InsertHelper::Close()
{
    if (mInsertStatement != NULL) {
        ICloseable::Probe(mInsertStatement)->Close();
        mInsertStatement = NULL;
    }
    if (mReplaceStatement != NULL) {
        ICloseable::Probe(mReplaceStatement)->Close();
        mReplaceStatement = NULL;
    }
    mInsertSQL = NULL;

    if (mColumns != NULL) {
        mColumns = NULL;
    }
    return NOERROR;
}


const String DatabaseUtils::TAG("DatabaseUtils");
const Boolean DatabaseUtils::DEBUG;
AutoPtr<ICollator> DatabaseUtils::mColl;

void DatabaseUtils::WriteExceptionToParcel(
    /* [in] */ IParcel* reply,
    /* [in] */ ECode e)
{
    Int32 code = 0;
    Boolean logException = TRUE;
    if (e == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        code = 1;
        logException = FALSE;
    }
    else if (e == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
       code = 2;
    }
    else if (e == (ECode)E_UNSUPPORTED_OPERATION_EXCEPTION) {
       code = 3;
    }
    else if (e == (ECode)E_SQLITE_ABORT_EXCEPTION) {
       code = 4;
    }
    else if (e == (ECode)E_SQLITE_CONSTRAINT_EXCEPTION) {
        code = 5;
    }
    else if (e == (ECode)E_SQLITE_DATABASE_CORRUPT_EXCEPTION) {
        code = 6;
    }
    else if (e == (ECode)E_SQLITE_FULL_EXCEPTION) {
        code = 7;
    }
    else if (e == (ECode)E_SQLITE_DISK_IO_EXCEPTION) {
        code = 8;
    }
    else if (e == (ECode)E_SQLITE_EXCEPTION) {
        code = 9;
    }
    else if (e == (ECode)E_OPERATION_APPLICATION_EXCEPTION) {
        code = 10;
    }
    else {
        reply->WriteInt32((Int32)e);
        Slogger::E(TAG, "Writing exception to parcel, 0x%08x", e);
    }
    reply->WriteInt32(code);
    // reply.writeString(e.getMessage());

    if (logException) {
        Slogger::E(TAG, "Writing exception to parcel, 0x%08x", e);
    }
}

ECode DatabaseUtils::ReadExceptionFromParcel(
    /* [in] */ IParcel* reply)
{
    Int32 code;
    reply->ReadInt32(&code);
    if (code == 0) return NOERROR;
    String msg;
    reply->ReadString(&msg);
    return ReadExceptionFromParcel(reply, msg, code);
}

ECode DatabaseUtils::ReadExceptionWithFileNotFoundExceptionFromParcel(
    /* [in] */ IParcel* reply)
{
    Int32 code;
    reply->ReadInt32(&code);
    if (code == 0) return NOERROR;
    String msg;
    reply->ReadString(&msg);
    if (code == 1) {
        //throw new FileNotFoundException(msg);
        return E_FILE_NOT_FOUND_EXCEPTION;
    }
    else {
        return ReadExceptionFromParcel(reply, msg, code);
    }
}

ECode DatabaseUtils::ReadExceptionWithOperationApplicationExceptionFromParcel(
    /* [in] */ IParcel* reply)
{
    Int32 code;
    reply->ReadInt32(&code);
    if (code == 0) return NOERROR;
    String msg;
    reply->ReadString(&msg);
    if (code == 10) {
        //throw new OperationApplicationException(msg);
        return E_OPERATION_APPLICATION_EXCEPTION;
    }
    else {
        return ReadExceptionFromParcel(reply, msg, code);
    }
}

ECode DatabaseUtils::ReadExceptionFromParcel(
    /* [in] */ IParcel* reply,
    /* [in] */ const String& msg,
    /* [in] */ Int32 code)
{
    switch (code) {
        case 2:
            //throw new IllegalArgumentException(msg);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        case 3:
            //throw new UnsupportedOperationException(msg);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        case 4:
            //throw new SQLiteAbortException(msg);
            return E_SQLITE_ABORT_EXCEPTION;
        case 5:
            //throw new SQLiteConstraintException(msg);
            return E_SQLITE_CONSTRAINT_EXCEPTION;
        case 6:
            //throw new SQLiteDatabaseCorruptException(msg);
            return E_SQLITE_DATABASE_CORRUPT_EXCEPTION;
        case 7:
            //throw new SQLiteFullException(msg);
            return E_SQLITE_FULL_EXCEPTION;
        case 8:
            //throw new SQLiteDiskIOException(msg);
            return E_SQLITE_DISK_IO_EXCEPTION;
        case 9:
            //throw new SQLiteException(msg);
            return E_SQLITE_EXCEPTION;
        case 11:
            //throw new OperationCanceledException(msg);
            return E_OPERATION_CANCELED_EXCEPTION;
        default:
            // reply.readException(code, msg);
            Slogger::E(TAG, "TODO: readException has not been realized!!!!!!");
            return E_NOT_IMPLEMENTED;
    }
}

ECode DatabaseUtils::BindObjectToProgram(
    /* [in] */ ISQLiteProgram* prog,
    /* [in] */ Int32 index,
    /* [in] */ IInterface* value)
{
    if (value == NULL) {
        return prog->BindNull(index);
    }
    else if (IDouble::Probe(value) != NULL || IFloat::Probe(value) != NULL) {
        AutoPtr<INumber> inum = INumber::Probe(value);
        Double value;
        inum->DoubleValue(&value);
        prog->BindDouble(index, value);
    }
    else if (INumber::Probe(value) != NULL) {
        AutoPtr<INumber> inum = INumber::Probe(value);
        Int64 value;
        inum->Int64Value(&value);
        return prog->BindInt64(index, value);
    }
    else if (IBoolean::Probe(value) != NULL) {
        AutoPtr<IBoolean> ib = IBoolean::Probe(value);
        Boolean value;
        ib->GetValue(&value);
        if (value) {
            return prog->BindInt64(index, 1);
        }
        else {
            return prog->BindInt64(index, 0);
        }
    }
    else if (IArrayOf::Probe(value) != NULL){
        AutoPtr<IArrayOf> iarray = IArrayOf::Probe(value);
        Int32 len;
        iarray->GetLength(&len);
        AutoPtr< ArrayOf<Byte> > byteArray = ArrayOf<Byte>::Alloc(len);
        for (Int32 i = 0; i < len; ++i) {
            AutoPtr<IByte> b;
            iarray->Get(i, (IInterface**)&b);
            Byte value;
            b->GetValue(&value);
            (*byteArray)[i] = value;
        }
        return prog->BindBlob(index, *byteArray);
    }
    else {
        AutoPtr<ICharSequence> cs = ICharSequence::Probe(value);
        String value;
        cs->ToString(&value);
        return prog->BindString(index, value);
    }
    return NOERROR;
}

Int32 DatabaseUtils::GetTypeOfObject(
    /* [in] */ IInterface* obj)
{
    if (obj == NULL) {
        return ICursor::FIELD_TYPE_NULL;
    }
    else if (IArrayOf::Probe(obj) != NULL) {
        return ICursor::FIELD_TYPE_BLOB;
    }
    else if (IDouble::Probe(obj) != NULL || IFloat::Probe(obj) != NULL) {
        return ICursor::FIELD_TYPE_FLOAT;
    }
    else if (IInteger64::Probe(obj) != NULL || IInteger32::Probe(obj) != NULL
            || IInteger16::Probe(obj) != NULL || IByte::Probe(obj) != NULL) {
        return ICursor::FIELD_TYPE_INTEGER;
    }
    else {
        return ICursor::FIELD_TYPE_STRING;
    }
}

void DatabaseUtils::CursorFillWindow(
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 position,
    /* [in] */ ICursorWindow* window)
{
    Int32 count;
    cursor->GetCount(&count);
    if (position < 0 || position >= count) {
        return;
    }
    Int32 oldPos;
    cursor->GetPosition(&oldPos);
    Int32 numColumns;
    cursor->GetColumnCount(&numColumns);
    window->Clear();
    Boolean succeeded;
    window->SetStartPosition(position);
    window->SetNumColumns(numColumns, &succeeded);
    if (cursor->MoveToPosition(position, &succeeded), succeeded) {
        do {
            if (window->AllocRow(&succeeded), !succeeded) {
                break;
            }
            for (Int32 i = 0; i < numColumns; i++) {
                Int32 type;
                cursor->GetType(i, &type);
                Boolean success;
                switch (type) {
                    case ICursor::FIELD_TYPE_NULL:
                        window->PutNull(position, i, &success);
                        break;

                    case ICursor::FIELD_TYPE_INTEGER:
                        Int64 numInt64;
                        window->PutInt64((cursor->GetInt64(i, &numInt64),numInt64), position, i, &success);
                        break;

                    case ICursor::FIELD_TYPE_FLOAT:
                        Double numdouble;
                        window->PutDouble((cursor->GetDouble(i, &numdouble),numdouble), position, i, &success);
                        break;

                    case ICursor::FIELD_TYPE_BLOB: {
                        AutoPtr< ArrayOf<Byte> > value;
                        cursor->GetBlob(i, (ArrayOf<Byte>**)&value);
                        value != NULL ? window->PutBlob(*value, position, i, &success)
                                : window->PutNull(position, i, &success);
                        break;
                    }

                    default: // assume value is convertible to String
                    case ICursor::FIELD_TYPE_STRING: {
                        String value;
                        cursor->GetString(i, &value);
                        !value.IsNull() ? window->PutString(value, position, i, &success)
                                : window->PutNull(position, i, &success);
                        break;
                    }
                }
                if (!success) {
                    window->FreeLastRow();
                    break;
                }
            }
            position += 1;
        } while (cursor->MoveToNext(&succeeded), succeeded);
    }
    cursor->MoveToPosition(oldPos, &succeeded);
}

void DatabaseUtils::AppendEscapedSQLString(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& sqlString)
{
    sb->AppendChar('\'');
    if (sqlString.IndexOf('\'') != -1) {
        AutoPtr< ArrayOf<Char32> > chars = sqlString.GetChars();
        Char32 c;
        for (Int32 i = 0; i < (Int32)sqlString.GetLength(); i++) {
            c = (*chars)[i];
            if (c == '\'') {
                sb->AppendChar('\'');
            }
            sb->AppendChar(c);
        }
    }
    else
        sb->Append(sqlString);
    sb->AppendChar('\'');
}

String DatabaseUtils::SqlEscapeString(
    /* [in] */ const String& value)
{
    AutoPtr<IStringBuilder> escaper = (IStringBuilder*)new StringBuilder();
    AppendEscapedSQLString(escaper, value);
    String str;
    ICharSequence::Probe(escaper)->ToString(&str);
    return str;
}

void DatabaseUtils::AppendValueToSql(
    /* [in] */ IStringBuilder* sql,
    /* [in] */ IInterface* value)
{
    if (value == NULL) {
        sql->Append(String("NULL"));
    }
    else if (IBoolean::Probe(value) != NULL) {
        AutoPtr<IBoolean> ib = IBoolean::Probe(value);
        Boolean boolValue;
        ib->GetValue(&boolValue);
        if (boolValue) {
            sql->AppendChar('1');
        }
        else {
            sql->AppendChar('0');
        }
    }
    else {
        AutoPtr<ICharSequence> cs = ICharSequence::Probe(value);
        String str;
        cs->ToString(&str);
        AppendEscapedSQLString(sql, str);
    }
}

String DatabaseUtils::ConcatenateWhere(
    /* [in] */ const String& a,
    /* [in] */ const String& b)
{
    assert(0 && "TODO TextUtils::IsEmpty");
    // if (TextUtils::IsEmpty(a)) {
    //     return b;
    // }
    // if (TextUtils::IsEmpty(b)) {
    //     return a;
    // }

    StringBuilder sb("(");
    sb += a;
    sb += ") AND (";
    sb += b;
    sb += ")";
    return sb.ToString();
}

String DatabaseUtils::GetCollationKey(
    /* [in] */ const String& name)
{
    AutoPtr< ArrayOf<Byte> > arr = GetCollationKeyInBytes(name);
    // try {
    // return String(arr, 0, getKeyLen(arr), "ISO8859_1");
    return String((const char*)arr->GetPayload(), GetKeyLen(*arr));
    // } catch (Exception ex) {
    //     return "";
    // }
}

/**
 * Used building output as Hex
 */
static const Char32 DIGITS[] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
       '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

String DatabaseUtils::GetHexCollationKey(
    /* [in] */ const String& name)
{
    AutoPtr< ArrayOf<Byte> > arr = GetCollationKeyInBytes(name);
    Int32 l = arr->GetLength();

    AutoPtr< ArrayOf<Char32> > keys = ArrayOf<Char32>::Alloc(l << 1);

    // two characters form the hex value.
    for (Int32 i = 0, j = 0; i < l; i++) {
        (*keys)[j++] = DIGITS[((UInt32)(0xF0 & (*arr)[i])) >> 4 ];
        (*keys)[j++] = DIGITS[ 0x0F & (*arr)[i] ];
    }

    return String(*keys, 0, GetKeyLen(*arr) * 2);
}

Int32 DatabaseUtils::GetKeyLen(
    /* [in] */ const ArrayOf<Byte>& arr)
{
    Int32 size = arr.GetLength();
    if (arr[size - 1] != 0) {
        return size;
    }
    else {
        // remove zero "termination"
        return size - 1;
    }
}

AutoPtr< ArrayOf<Byte> > DatabaseUtils::GetCollationKeyInBytes(
    /* [in] */ const String& name)
{
    if (mColl == NULL) {
        AutoPtr<ICollatorHelper> helper;
        assert(0 && "TODO CCollatorHelper");
        //CCollatorHelper::AcquireSingleton((ICollatorHelper**)&helper);
        helper->GetInstance((ICollator**)&mColl);
        mColl->SetStrength(ICollator::PRIMARY);
    }
    AutoPtr<ICollationKey> key;
    mColl->GetCollationKey(name, (ICollationKey**)&key);
    AutoPtr< ArrayOf<Byte> > arr;
    key->ToByteArray((ArrayOf<Byte>**)&arr);
    return arr;
}

void DatabaseUtils::DumpCursor(
    /* [in] */ ICursor* cursor)
{
    assert(0);
    //DumpCursor(cursor, System.out);
}

void DatabaseUtils::DumpCursor(
    /* [in] */ ICursor* cursor,
    /* [in] */ IPrintStream* stream)
{
    stream->Println(String(">>>>> Dumping cursor "));
    stream->Println(cursor);
    if (cursor != NULL) {
        Int32 startPos;
        cursor->GetPosition(&startPos);

        Boolean succeeded;
        cursor->MoveToPosition(-1, &succeeded);
        while (cursor->MoveToNext(&succeeded), succeeded) {
            DumpCurrentRow(cursor, stream);
        }
        cursor->MoveToPosition(startPos, &succeeded);
    }
    stream->Println(String("<<<<<"));
}

void DatabaseUtils::DumpCursor(
    /* [in] */ ICursor* cursor,
    /* [in] */ IStringBuilder* sb)
{
    //sb += (">>>>> Dumping cursor " + cursor + "\n");
    sb->Append(String(">>>>> Dumping cursor  \n"));
    if (cursor != NULL) {
        Int32 startPos;
        cursor->GetPosition(&startPos);

        Boolean succeeded;
        cursor->MoveToPosition(-1, &succeeded);
        while (cursor->MoveToNext(&succeeded), succeeded) {
            DumpCurrentRow(cursor, sb);
        }
        cursor->MoveToPosition(startPos, &succeeded);
    }
    sb->Append(String("<<<<<\n"));
}

String DatabaseUtils::DumpCursorToString(
    /* [in] */ ICursor* cursor)
{
    AutoPtr<IStringBuilder> sb = (IStringBuilder*)new StringBuilder();
    DumpCursor(cursor, sb);
    String str;
    ICharSequence::Probe(sb)->ToString(&str);
    return str;
}

void DatabaseUtils::DumpCurrentRow(
    /* [in] */ ICursor* cursor)
{
    assert(0);
    //DumpCurrentRow(cursor, System.out);
}

void DatabaseUtils::DumpCurrentRow(
    /* [in] */ ICursor* cursor,
    /* [in] */ IPrintStream* stream)
{
    AutoPtr< ArrayOf<String> > cols;
    cursor->GetColumnNames((ArrayOf<String>**)&cols);
    Int32 position;
    cursor->GetPosition(&position);
    StringBuilder sb1("");
    sb1 += position;
    sb1 += " {";
    stream->Println(sb1.ToString());
    Int32 length = cols->GetLength();
    for (Int32 i = 0; i < length; i++) {
        String value;
        //try {
        ECode ec = cursor->GetString(i, &value);
        //} catch (SQLiteException e) {
        if(ec == (ECode)E_SQLITE_EXCEPTION) {
            // assume that if the getString threw this exception then the column is not
            // representable by a string, e.g. it is a BLOB.
            value = String("<unprintable>");
        }
        //}
        StringBuilder sb2("   ");
        sb2 += (*cols)[i];
        sb2 += '=';
        sb2 += value;
        stream->Println(sb2.ToString());
    }
    stream->Println(String("}"));
}

void DatabaseUtils::DumpCurrentRow(
    /* [in] */ ICursor* cursor,
    /* [in] */ IStringBuilder* sb)
{
    AutoPtr< ArrayOf<String> > cols;
    cursor->GetColumnNames((ArrayOf<String>**)&cols);
    sb->Append(String(""));
    Int32 pos;
    cursor->GetPosition(&pos);
    sb->Append(pos);
    sb->Append(String(" {\n"));
    Int32 length = cols->GetLength();
    for (Int32 i = 0; i < length; i++) {
        String value;
        //try {
        ECode ec = cursor->GetString(i, &value);
        //} catch (SQLiteException e) {
        if(ec == (ECode)E_SQLITE_EXCEPTION) {
            // assume that if the getString threw this exception then the column is not
            // representable by a string, e.g. it is a BLOB.
            value = String("<unprintable>");
        }
        //}
        sb->Append(String("   "));
        sb->Append((*cols)[i]);
        sb->AppendChar('=');
        sb->Append(value);
        sb->Append(String("\n"));
    }
    sb->Append(String("}\n"));
}

String DatabaseUtils::DumpCurrentRowToString(
    /* [in] */ ICursor* cursor)
{
    AutoPtr<IStringBuilder> sb = (IStringBuilder*)new StringBuilder();
    DumpCurrentRow(cursor, sb);
    String str;
    ICharSequence::Probe(sb)->ToString(&str);
    return str;
}

void DatabaseUtils::CursorStringToContentValues(
    /* [in] */ ICursor* cursor,
    /* [in] */ const String& field,
    /* [in] */ IContentValues* values)
{
    CursorStringToContentValues(cursor, field, values, field);
}

void DatabaseUtils::CursorStringToInsertHelper(
    /* [in] */ ICursor* cursor,
    /* [in] */ const String& field,
    /* [in] */ IDatabaseInsertHelper* inserter,
    /* [in] */ Int32 index)
{
    Int32 columnIndexOrThrow;
    cursor->GetColumnIndexOrThrow(field, &columnIndexOrThrow);
    String str;
    cursor->GetString(columnIndexOrThrow, &str);
    inserter->BindString(index, str);
}

void DatabaseUtils::CursorStringToContentValues(
    /* [in] */ ICursor* cursor,
    /* [in] */ const String& field,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& key)
{
    Int32 columnIndexOrThrow;
    cursor->GetColumnIndexOrThrow(field, &columnIndexOrThrow);
    String str;
    cursor->GetString(columnIndexOrThrow, &str);
    values->Put(key, str);
}

void DatabaseUtils::CursorInt32ToContentValues(
    /* [in] */ ICursor* cursor,
    /* [in] */ const String& field,
    /* [in] */ IContentValues* values)
{
    CursorInt32ToContentValues(cursor, field, values, field);
}

void DatabaseUtils::CursorInt32ToContentValues(
    /* [in] */ ICursor* cursor,
    /* [in] */ const String& field,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& key)
{
    Int32 colIndex;
    cursor->GetColumnIndex(field, &colIndex);
    Boolean isNull;
    if (cursor->IsNull(colIndex, &isNull), !isNull) {
        Int32 result;
        cursor->GetInt32(colIndex, &result);
        values->Put(key, result);
    }
    else {
        values->Put(key, (Int32)NULL);
    }
}

void DatabaseUtils::CursorInt64ToContentValues(
    /* [in] */ ICursor* cursor,
    /* [in] */ const String& field,
    /* [in] */ IContentValues* values)
{
    CursorInt64ToContentValues(cursor, field, values, field);
}

void DatabaseUtils::CursorInt64ToContentValues(
    /* [in] */ ICursor* cursor,
    /* [in] */ const String& field,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& key)
{
    Int32 colIndex;
    cursor->GetColumnIndex(field, &colIndex);
    Boolean isNUll;
    if (cursor->IsNull(colIndex, &isNUll), !isNUll) {
        Int64 value;
        cursor->GetInt64(colIndex, &value);
        values->Put(key, value);
    }
    else {
        values->Put(key, (Int64)NULL);
    }
}

void DatabaseUtils::CursorDoubleToCursorValues(
    /* [in] */ ICursor* cursor,
    /* [in] */ const String& field,
    /* [in] */ IContentValues* values)
{
    CursorDoubleToContentValues(cursor, field, values, field);
}

void DatabaseUtils::CursorDoubleToContentValues(
    /* [in] */ ICursor* cursor,
    /* [in] */ const String& field,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& key)
{
    Int32 colIndex;
    cursor->GetColumnIndex(field, &colIndex);
    Boolean isNUll;
    if (cursor->IsNull(colIndex, &isNUll), !isNUll) {
        Double result;
        cursor->GetDouble(colIndex, &result);
        values->Put(key, result);
    }
    else {
        values->Put(key, (Double)NULL);
    }
}

void DatabaseUtils::CursorRowToContentValues(
    /* [in] */ ICursor* cursor,
    /* [in] */ IContentValues* values)
{
    AutoPtr<IAbstractWindowedCursor> awc = (IAbstractWindowedCursor::Probe(cursor) != NULL) ?
            IAbstractWindowedCursor::Probe(cursor) : NULL;
    AutoPtr< ArrayOf<String> > columns;
    cursor->GetColumnNames((ArrayOf<String>**)&columns);
    Int32 length = columns->GetLength();
    Boolean isBlob;
    for (Int32 i = 0; i < length; i++) {
        if (awc != NULL && (awc->IsBlob(i, &isBlob), isBlob)) {
            AutoPtr< ArrayOf<Byte> > blob;
            cursor->GetBlob(i, (ArrayOf<Byte>**)&blob);
            values->Put((*columns)[i], blob);
        }
        else {
            String str;
            cursor->GetString(i, &str);
            values->Put((*columns)[i], str);
        }
    }
}

Int32 DatabaseUtils::CursorPickFillWindowStartPosition(
    /* [in] */ Int32 cursorPosition,
    /* [in] */ Int32 cursorWindowCapacity)
{
    return Elastos::Core::Math::Max(cursorPosition - cursorWindowCapacity / 3, 0);
}

Int64 DatabaseUtils::QueryNumEntries(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& table)
{
    return QueryNumEntries(db, table, String(NULL), NULL);
}

Int64 DatabaseUtils::QueryNumEntries(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& table,
    /* [in] */ const String& selection)
{
    return QueryNumEntries(db, table, selection, NULL);
}

Int64 DatabaseUtils::QueryNumEntries(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& table,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs)
{
    assert(0 && "TODO TextUtils::IsEmpty");
    String s ;//= (!TextUtils::IsEmpty(selection)) ? String(" where ") + selection : String("");
    StringBuilder sb("select count(*) from ");
    sb += table;
    sb += s;
    return Int64ForQuery(db, sb.ToString(), selectionArgs);
}

Boolean DatabaseUtils::QueryIsEmpty(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& table)
{
    Int64 isEmpty = Int64ForQuery(db, String("select exists(select 1 from ") + table + String(")"), NULL);
    return isEmpty == 0;
}

Int64 DatabaseUtils::Int64ForQuery(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& query,
    /* [in] */ ArrayOf<String>* selectionArgs)
{
    AutoPtr<ISQLiteStatement> prog;
    db->CompileStatement(query, (ISQLiteStatement**)&prog);
    //try {
    Int64 value = Int64ForQuery(prog, selectionArgs);
    //} finally {
    ICloseable::Probe(prog)->Close();
    //}
    return value;
}

Int64 DatabaseUtils::Int64ForQuery(
    /* [in] */ ISQLiteStatement* prog,
    /* [in] */ ArrayOf<String>* selectionArgs)
{
    ISQLiteProgram::Probe(prog)->BindAllArgsAsStrings(selectionArgs);
    Int64 value;
    prog->SimpleQueryForInt64(&value);
    return value;
}

String DatabaseUtils::StringForQuery(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& query,
    /* [in] */ ArrayOf<String>* selectionArgs)
{
    AutoPtr<ISQLiteStatement> prog;
    db->CompileStatement(query, (ISQLiteStatement**)&prog);
    //try {
    String str = StringForQuery(prog, selectionArgs);
    //} finally {
    ICloseable::Probe(prog)->Close();
    //}
    return str;
}

String DatabaseUtils::StringForQuery(
    /* [in] */ ISQLiteStatement* prog,
    /* [in] */ ArrayOf<String>* selectionArgs)
{
    ISQLiteProgram::Probe(prog)->BindAllArgsAsStrings(selectionArgs);
    String str;
    prog->SimpleQueryForString(&str);
    return str;
}

AutoPtr<IParcelFileDescriptor> DatabaseUtils::BlobFileDescriptorForQuery(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& query,
    /* [in] */ ArrayOf<String>* selectionArgs)
{
    AutoPtr<ISQLiteStatement> prog;
    db->CompileStatement(query, (ISQLiteStatement**)&prog);
    //try {
    AutoPtr<IParcelFileDescriptor> desc = BlobFileDescriptorForQuery(prog, selectionArgs);
    //} finally {
    ICloseable::Probe(prog)->Close();
    //}
    return desc;
}

AutoPtr<IParcelFileDescriptor> DatabaseUtils::BlobFileDescriptorForQuery(
    /* [in] */ ISQLiteStatement* prog,
    /* [in] */ ArrayOf<String>* selectionArgs)
{
    ISQLiteProgram::Probe(prog)->BindAllArgsAsStrings(selectionArgs);
    AutoPtr<IParcelFileDescriptor> desc;
    prog->SimpleQueryForBlobFileDescriptor((IParcelFileDescriptor**)&desc);
    return desc;
}

void DatabaseUtils::CursorStringToContentValuesIfPresent(
    /* [in] */ ICursor* cursor,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& column)
{
    Int32 index;
    cursor->GetColumnIndexOrThrow(column, &index);
    Boolean isNUll;
    if (index != -1 && (cursor->IsNull(index, &isNUll), !isNUll)) {
        String columnValue;
        cursor->GetString(index, &columnValue);
        values->Put(column, columnValue);
    }
}

void DatabaseUtils::CursorInt64ToContentValuesIfPresent(
    /* [in] */ ICursor* cursor,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& column)
{
    Int32 index;
    cursor->GetColumnIndexOrThrow(column, &index);
    Boolean isNull;
    if (index != -1 && (cursor->IsNull(index, &isNull), !isNull)) {
        Int64 columnValue;
        cursor->GetInt64(index, &columnValue);
        values->Put(column, columnValue);
    }
}

void DatabaseUtils::CursorInt16ToContentValuesIfPresent(
    /* [in] */ ICursor* cursor,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& column)
{
    Int32 index;
    cursor->GetColumnIndexOrThrow(column, &index);
    Boolean isNull;
    if (cursor->IsNull(index, &isNull), !isNull) {
        Int16 columnValue;
        cursor->GetInt16(index, &columnValue);
        values->Put(column, columnValue);
    }
}

void DatabaseUtils::CursorInt32ToContentValuesIfPresent(
    /* [in] */ ICursor* cursor,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& column)
{
    Int32 index;
    cursor->GetColumnIndexOrThrow(column, &index);
    Boolean isNull;
    if (cursor->IsNull(index, &isNull), !isNull) {
        Int32 columnValue;
        cursor->GetInt32(index, &columnValue);
        values->Put(column, columnValue);
    }
}

void DatabaseUtils::CursorFloatToContentValuesIfPresent(
    /* [in] */ ICursor* cursor,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& column)
{
    Int32 index;
    cursor->GetColumnIndexOrThrow(column, &index);
    Boolean isNull;
    if (cursor->IsNull(index, &isNull), !isNull) {
        Float columnValue;
        cursor->GetFloat(index, &columnValue);
        values->Put(column, columnValue);
    }
}

void DatabaseUtils::CursorDoubleToContentValuesIfPresent(
    /* [in] */ ICursor* cursor,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& column)
{
    Int32 index;
    cursor->GetColumnIndexOrThrow(column, &index);
    Boolean isNull;
    if (cursor->IsNull(index, &isNull), !isNull) {
        Double columnValue;
        cursor->GetDouble(index, &columnValue);
        values->Put(column, columnValue);
    }
}

void DatabaseUtils::CreateDbFromSqlStatements(
    /* [in] */ IContext* context,
    /* [in] */ const String& dbName,
    /* [in] */ Int32 dbVersion,
    /* [in] */ const String& sqlStatements)
{
    AutoPtr<ISQLiteDatabase> db;
    context->OpenOrCreateDatabase(dbName, 0, NULL, (ISQLiteDatabase**)&db);
    // TODO: this is not quite safe since it assumes that all semicolons at the end of a line
    // terminate statements. It is possible that a text field contains ;\n. We will have to fix
    // this if that turns out to be a problem.
    AutoPtr< ArrayOf<String> > statements;
    ASSERT_SUCCEEDED(StringUtils::Split(sqlStatements, String(";\n"), (ArrayOf<String>**)&statements))
    for (Int32 i = 0; i < statements->GetLength(); i++) {
        String statement = (*statements)[i];
        assert(0 && "TODO TextUtils::IsEmpty");
        //if (TextUtils::IsEmpty(statement)) continue;
        db->ExecSQL(statement);
    }

    db->SetVersion(dbVersion);
    ICloseable::Probe(db)->Close();
}

Int32 DatabaseUtils::GetSqlStatementType(
    /* [in] */ const String& sql)
{
    String sqlStr = sql.Trim();
    if (sqlStr.GetLength() < 3) {
        return DatabaseUtils_STATEMENT_OTHER;
    }

    String prefixSql = sqlStr.Substring(0, 3).ToUpperCase(/*Locale.ROOT*/);
    if (prefixSql.Equals("SEL")) {
        return DatabaseUtils_STATEMENT_SELECT;
    }
    else if (prefixSql.Equals("INS") ||
        prefixSql.Equals("UPD") ||
        prefixSql.Equals("REP") ||
        prefixSql.Equals("DEL")) {
        return DatabaseUtils_STATEMENT_UPDATE;
    }
    else if (prefixSql.Equals("ATT")) {
        return DatabaseUtils_STATEMENT_ATTACH;
    }
    else if (prefixSql.Equals("COM")) {
        return DatabaseUtils_STATEMENT_COMMIT;
    }
    else if (prefixSql.Equals("END")) {
        return DatabaseUtils_STATEMENT_COMMIT;
    }
    else if (prefixSql.Equals("ROL")) {
        return DatabaseUtils_STATEMENT_ABORT;
    }
    else if (prefixSql.Equals("BEG")) {
        return DatabaseUtils_STATEMENT_BEGIN;
    }
    else if (prefixSql.Equals("PRA")) {
        return DatabaseUtils_STATEMENT_PRAGMA;
    }
    else if (prefixSql.Equals("CRE") || prefixSql.Equals("DRO") ||
            prefixSql.Equals("ALT")) {
        return DatabaseUtils_STATEMENT_DDL;
    }
    else if (prefixSql.Equals("ANA") || prefixSql.Equals("DET")) {
        return DatabaseUtils_STATEMENT_UNPREPARED;
    }
    return DatabaseUtils_STATEMENT_OTHER;
}

AutoPtr< ArrayOf<String> > DatabaseUtils::AppendSelectionArgs(
    /* [in] */ ArrayOf<String>* originalValues,
    /* [in] */ ArrayOf<String>* newValues)
{
    if (originalValues == NULL || originalValues->GetLength() == 0) {
        return newValues;
    }
    Int32 len1 = originalValues->GetLength();
    Int32 len2 = newValues->GetLength();
    AutoPtr< ArrayOf<String> > appendresult = ArrayOf<String>::Alloc(len1 + len2);
    for(Int32 i = 0; i < len1; i++) {
        (*appendresult)[i] = (*originalValues)[i];
    }
    //System.arraycopy(newValues, 0, appendresult, originalValues.length, newValues.length);
    for(Int32 i = 0; i < len2; i++) {
        (*appendresult)[len1 + i] = (*newValues)[i];
    }
    return appendresult;
}

Int32 DatabaseUtils::FindRowIdColumnIndex(
    /* [in] */ ArrayOf<String>* columnNames)
{
    Int32 length = columnNames ? columnNames->GetLength() : 0;
    for (Int32 i = 0; i < length; i++) {
        if ((*columnNames)[i].Equals("_id")) {
            return i;
        }
    }
    return -1;
}

} //Database
} //Droid
} //Elastos
