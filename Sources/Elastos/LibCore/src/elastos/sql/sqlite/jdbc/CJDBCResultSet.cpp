
#include "CJDBCResultSet.h"
#include "Database.h"
#include "CJDBCResultSetMetaData.h"
#include "JDBCStatement.h"
#include "CJDBCConnection.h"
#include "CTime.h"
#include "CTimestamp.h"
#include "StringEncoder.h"
#include "CTableResultX.h"
#include "CShell.h"
#include "CDatabaseX.h"
#include <elastos/utility/etl/HashSet.h>
#include "StringBuffer.h"
#include <elastos/core/StringUtils.h>
#include "CURL.h"
#include "CByteArrayInputStream.h"
#include "CCharArrayReader.h"
#include "CSqlDate.h"

using Elastos::Utility::Etl::HashSet;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringUtils;
using Elastos::Net::CURL;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::ICharArrayReader;
using Elastos::IO::CCharArrayReader;
using Elastos::Sql::CSqlDate;
using Elastos::Sql::CTimestamp;

namespace Elastos {
namespace Sql {
namespace SQLite {
namespace JDBC{

const Boolean CJDBCResultSet::nullrepl =
    SQLite::Database::Version().Compare("2.5.0") < 0;

CAR_OBJECT_IMPL(CJDBCResultSet);

CAR_INTERFACE_IMPL_3(CJDBCResultSet, Object, IJDBCResultSet, IResultSet, IWrapper);

ECode CJDBCResultSet::Absolute(
    /* [in] */ Int32 row,
    /* [out] */ Boolean * value)
{
    if (tr == NULL) {
        *value = FALSE;
        return NOERROR;
    }
    if (row < 0) {
        row = tr->mNrows + 1 + row;
    }
    row--;
    if (row < 0 || row > tr->mNrows) {
        *value = FALSE;
        return NOERROR;
    }
    this->mRow = row;
    *value = TRUE;
    return NOERROR;
}

ECode CJDBCResultSet::AfterLast()
{
    if (tr == NULL) {
        return NOERROR;
    }
    mRow = tr->mNrows;
    return NOERROR;
}

ECode CJDBCResultSet::BeforeFirst()
{
    if (tr == NULL) {
        return NOERROR;
    }
    mRow = -1;
    return NOERROR;
}

ECode CJDBCResultSet::CancelRowUpdates()
{
    rowbuf = NULL;
    return NOERROR;
}

ECode CJDBCResultSet::ClearWarnings()
{

    return E_SQL_EXCEPTION;
}

ECode CJDBCResultSet::Close()
{
    s = NULL;
    tr = NULL;
    lastg = NULL;
    oninsrow = FALSE;
    rowbuf = NULL;
    mRow = -1;
    return NOERROR;
}

ECode CJDBCResultSet::DeleteRow()
{
    Boolean isupdate = FALSE;
    IsUpdatable(&isupdate);
    if (oninsrow) {
        return E_SQL_EXCEPTION;
    }
    if (updatable < UPD_INSUPDDEL) {
        return E_SQL_EXCEPTION;
    }
    FillRowbuf();
    StringBuffer sb;
    sb.Append("DELETE FROM ");
    sb.Append(SQLite::CShell::SqlQuoteDbl(uptable));
    sb.Append(" WHERE ");
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(pkcols->GetLength());
    for (Int32 i = 0; i < pkcols->GetLength(); i++) {
        sb.Append(SQLite::CShell::SqlQuoteDbl((*pkcols)[i]));
        sb.Append(String(" = ") + (nullrepl ? String("'%q'") : String("%Q")));
        if (i < pkcols->GetLength() - 1) {
        sb.Append(" AND ");
        }
        (*args)[i] = (*rowbuf)[(*pkcoli)[i]];
    }
    ECode ec = ((CDatabaseX *)((CJDBCConnection *)((JDBCStatement *)s.Get())->conn.Get())->mDb.Get())->Exec(sb.ToString(),NULL, args);
    if (ec != NOERROR)
    {
        return E_SQL_EXCEPTION;
    }
    rowbuf = NULL;
    return NOERROR;
}

ECode CJDBCResultSet::FindColumn(
    /* [in] */ const String& colName)
{

    return NOERROR;
}

ECode CJDBCResultSet::GetFirst(
    /* [out] */ Boolean *value)
{
    if (tr == NULL || tr->mNrows <= 0) {
        *value = FALSE;
    }
    mRow = 0;
    *value = TRUE;
    return NOERROR;
}

ECode CJDBCResultSet::GetAsciiStream(
    /* [in] */ Int32 colIndex,
    /* [out] */ IInputStream ** iinputstream)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCResultSet::GetAsciiStream(
    /* [in] */ const String& colName,
    /* [out] */ IInputStream ** iinputstream)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return GetAsciiStream(col,iinputstream);
}

ECode CJDBCResultSet::GetBigDecimal(
    /* [in] */ Int32 colIndex,
    /* [out] */ IBigDecimal ** bigdecimal)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::GetBigDecimal(
    /* [in] */ Int32 colIndex,
    /* [in] */ Int32 scale,
    /* [out] */ IBigDecimal ** bigdecimal)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::GetBigDecimal(
    /* [in] */ const String& colName,
    /* [out] */ IBigDecimal ** bigdecimal)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return GetBigDecimal(col,bigdecimal);
}

ECode CJDBCResultSet::GetBigDecimal(
    /* [in] */ const String& colName,
    /* [in] */ Int32 scale,
    /* [out] */ IBigDecimal ** bigdecimal)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return GetBigDecimal(col, scale,bigdecimal);
}

ECode CJDBCResultSet::GetBinaryStream(
    /* [in] */ Int32 colIndex,
    /* [out] */ IInputStream ** iinputstream)
{
    AutoPtr<ArrayOf<Byte> > data;
    GetBytes(colIndex,(ArrayOf<Byte> **)&data);
    if (data != NULL) {
        FAIL_RETURN(CByteArrayInputStream::New(data,(IByteArrayInputStream **)iinputstream));
    }
    *iinputstream = NULL;
    return NOERROR;
}

ECode CJDBCResultSet::GetBinaryStream(
    /* [in] */ const String& colName,
    /* [out] */ IInputStream ** iinputstream)
{
    AutoPtr<ArrayOf<Byte> > data;
    GetBytes(colName,(ArrayOf<Byte> **)&data);
    if (data != NULL) {
        FAIL_RETURN(CByteArrayInputStream::New(data,(IByteArrayInputStream **)iinputstream));
    }
    *iinputstream = NULL;
    return NOERROR;
}

ECode CJDBCResultSet::GetBlob(
    /* [in] */ Int32 colIndex,
    /* [out] */ Elastos::Sql::IBlob ** oblob)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::GetBlob(
    /* [in] */ const String& colName,
    /* [out] */ Elastos::Sql::IBlob ** oblob)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return GetBlob(col,oblob);
}

ECode CJDBCResultSet::GetBoolean(
    /* [in] */ Int32 colIndex,
    /* [out] */ Boolean * value)
{
    Int32 gint = 0;
    GetInt32(colIndex,&gint);
    String str = String(NULL);
    GetString(colIndex,&str);
    *value = gint == 1 || (StringUtils::ParseInt16(str) == 0 ? 0 : 1);
    return NOERROR;
}

ECode CJDBCResultSet::GetBoolean(
    /* [in] */ const String& colName,
    /* [out] */ Boolean * value)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    GetBoolean(col,value);
    return NOERROR;
}

ECode CJDBCResultSet::GetByte(
    /* [in] */ Int32 colIndex,
    /* [out] */ Byte * value)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCResultSet::GetByte(
    /* [in] */ const String& colName,
    /* [out] */ Byte * value)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return GetByte(col,value);
}

ECode CJDBCResultSet::GetBytes(
    /* [in] */ Int32 colIndex,
    /* [out, callee] */ ArrayOf<Byte> ** oarray)
{
    VALIDATE_NOT_NULL(oarray)
    *oarray = NULL;

    if (tr == NULL || colIndex < 1 || colIndex > tr->mNcolumns) {
        return E_SQL_EXCEPTION;
    }

    AutoPtr<ArrayOf<String> > rd = tr->mRows[mRow];
    lastg = (*rd)[colIndex - 1];
    if (lastg != NULL) {
        AutoPtr<ArrayOf<Byte> > array = SQLite::StringEncoder::Decode(lastg);
        *oarray = array;
        REFCOUNT_ADD(*oarray);
    }
    return NOERROR;
}

ECode CJDBCResultSet::GetBytes(
    /* [in] */ const String& colName,
    /* [out, callee] */ ArrayOf<Byte> ** oarray)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return GetBytes(col,oarray);
}

ECode CJDBCResultSet::GetCharacterStream(
    /* [in] */ Int32 colIndex,
    /* [out] */ IReader ** reader)
{
    VALIDATE_NOT_NULL(reader)
    *reader = NULL;

    String data = String(NULL);
    GetString(colIndex,&data);
    if (!data.IsNull()) {
        AutoPtr<ArrayOf<Char32> > cdata = data.GetChars();
        CCharArrayReader::New(cdata, (ICharArrayReader **)reader);
    }
    return NOERROR;
}

ECode CJDBCResultSet::GetCharacterStream(
    /* [in] */ const String& colName,
    /* [out] */ IReader ** reader)
{
    VALIDATE_NOT_NULL(reader)
    *reader = NULL;

    String data = String(NULL);
    GetString(colName,&data);
    if (data != NULL) {
        AutoPtr<ArrayOf<Char32> > cdata = data.GetChars();
        CCharArrayReader::New(cdata,(ICharArrayReader **)reader);
    }
    return NOERROR;
}

ECode CJDBCResultSet::GetClob(
    /* [in] */ Int32 colIndex,
    /* [out] */ IClob ** oclob)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::GetClob(
    /* [in] */ const String& colName,
    /* [out] */ IClob ** oclob)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return GetClob(col,oclob);
}

ECode CJDBCResultSet::GetConcurrency(
    /* [out] */ Int32 * mode)
{
    *mode = IResultSet::CONCUR_UPDATABLE;
    return NOERROR;
}

ECode CJDBCResultSet::GetCursorName(
    /* [out] */ String * name)
{
    *name = String(NULL);
    return NOERROR;
}

ECode CJDBCResultSet::GetDouble(
    /* [in] */ Int32 colIndex,
    /* [out] */ Double * value)
{
    AutoPtr<IDouble> d = InternalGetDouble(colIndex);
    if (d != NULL) {
        d->GetValue(value);
    }else{
        *value = 0;
    }
    return NOERROR;
}

ECode CJDBCResultSet::GetDouble(
    /* [in] */ const String& colName,
    /* [out] */ Double * value)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    GetDouble(col,value);
    return NOERROR;
}

ECode CJDBCResultSet::GetFetchDirection(
    /* [out] */ Int32 * value)
{
    *value = IResultSet::FETCH_FORWARD;
    return NOERROR;
}

ECode CJDBCResultSet::GetFetchSize(
    /* [out] */ Int32 * size)
{
    *size = 1;
    return NOERROR;
}

ECode CJDBCResultSet::GetFloat(
    /* [in] */ Int32 colIndex,
    /* [out] */ Float * value)
{
    AutoPtr<IFloat> f = InternalGetFloat(colIndex);
    if (f != NULL) {
        f->GetValue(value);
    }else{
        *value = 0;
    }
    return NOERROR;
}

ECode CJDBCResultSet::GetFloat(
    /* [in] */ const String& colName,
    /* [out] */ Float * value)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    GetFloat(col,value);
    return NOERROR;
}

ECode CJDBCResultSet::GetInt32(
    /* [in] */ Int32 colIndex,
    /* [out] */ Int32 * value)
{
    AutoPtr<IInteger32> i = InternalGetInt(colIndex);
    if (i != NULL) {
        i->GetValue(value);
    }
    else{
        *value = 0;
    }
    return NOERROR;
}

ECode CJDBCResultSet::GetInt32(
    /* [in] */ const String& colName,
    /* [out] */ Int32 * value)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    GetInt32(col,value);
    return NOERROR;
}

ECode CJDBCResultSet::GetInt64(
    /* [in] */ Int32 colIndex,
    /* [out] */ Int64 * value)
{
    AutoPtr<IInteger64> l = InternalGetLong(colIndex);
    if (l != NULL) {
        l->GetValue(value);
    }else{
        *value = 0;
    }
    return NOERROR;
}

ECode CJDBCResultSet::GetInt64(
    /* [in] */ const String& colName,
    /* [out] */ Int64 * value)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    GetInt64(col,value);
    return NOERROR;
}

ECode CJDBCResultSet::GetMetaData(
    /* [out] */ IResultSetMetaData ** metaData)
{
    if (md == NULL) {
        FAIL_RETURN(CJDBCResultSetMetaData::New((IJDBCResultSet *)this,(IJDBCResultSetMetaData **)&md));
    }
    *metaData = (IResultSetMetaData *)md.Get();
    REFCOUNT_ADD(*metaData);
    return NOERROR;
}

ECode CJDBCResultSet::GetObject(
    /* [in] */ Int32 colIndex,
    /* [out] */ IInterface ** obj)
{
    VALIDATE_NOT_NULL(obj);
    *obj = NULL;

    if (tr == NULL || colIndex < 1 || colIndex > tr->mNcolumns) {
        return E_SQL_EXCEPTION;
    }
    AutoPtr<ArrayOf<String> > rd = tr->mRows[mRow];
    lastg = (*rd)[colIndex - 1];
    assert(0 && "TODO error");
    AutoPtr<IInterface> temp = (IInterface *)&lastg;
    AutoPtr<ITableResultX > arr = (ITableResultX * )tr->Probe(EIID_ITableResultX);
    if (arr) {
            switch ((*((CTableResultX *)tr.Get())->sql_type)[colIndex - 1]) {
                case ITypes::SMALLINT:
                    temp = InternalGetShort(colIndex);
                    break;
                case ITypes::INTEGER:
                    temp = InternalGetInt(colIndex);
                    break;
                case ITypes::DOUBLE:
                    temp = InternalGetDouble(colIndex);
                    break;
                case ITypes::FLOAT:
                    temp = InternalGetFloat(colIndex);
                    break;
                case ITypes::BIGINT:
                    temp = InternalGetLong(colIndex);
                    break;
                case ITypes::BINARY:
                case ITypes::VARBINARY:
                case ITypes::LONGVARBINARY:
                    temp = NULL;
                    assert(0 && "TODO error");
                    // GetBytes(colIndex,(ArrayOf<Byte> **)temp);
                break;
                case ITypes::SQL_NULL:
                    temp = NULL;
                    break;
                /* defaults to String below */
        }
    }

    *obj = temp;
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode CJDBCResultSet::GetRef(
    /* [in] */ Int32 colIndex,
    /* [out] */ IRef ** ref)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::GetRef(
    /* [in] */ const String& colName,
    /* [out] */ IRef ** ref)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return GetRef(col,ref);
}

ECode CJDBCResultSet::GetRow(
    /* [out] */ Int32 * value)
{
    if (tr == NULL) {
        return E_SQL_EXCEPTION;
    }
    *value = mRow + 1;
    return NOERROR;
}

ECode CJDBCResultSet::GetInt16(
    /* [in] */ Int32 colIndex,
    /* [out] */ Int16 * value)
{
    AutoPtr<IInteger16> sh = InternalGetShort(colIndex);
    if (sh != NULL) {
        sh->GetValue(value);
    }
    *value = 0;
    return NOERROR;
}

ECode CJDBCResultSet::GetInt16(
    /* [in] */ const String& colName,
    /* [out] */ Int16 * value)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    GetInt16(col,value);
    return NOERROR;
}

ECode CJDBCResultSet::GetStatement(
    /* [out] */ IStatement ** statement)
{
    if (s == NULL) {
        return E_SQL_EXCEPTION;
    }
    *statement = IStatement::Probe(s);
    REFCOUNT_ADD(*statement);
    return NOERROR;
}

ECode CJDBCResultSet::GetString(
    /* [in] */ Int32 colIndex,
    /* [out] */ String * value)
{
    if (tr == NULL || colIndex < 1 || colIndex > tr->mNcolumns) {
        return E_SQL_EXCEPTION;
    }
    AutoPtr<ArrayOf<String> > rd = tr->mRows[mRow];
    lastg = (*rd)[colIndex - 1];
    *value = lastg;
    return NOERROR;
}

ECode CJDBCResultSet::GetString(
    /* [in] */ const String& colName,
    /* [out] */ String * value)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    GetString(col,value);
    return NOERROR;
}

ECode CJDBCResultSet::GetTime(
    /* [in] */ Int32 colIndex,
    /* [out] */ ITime ** sqltime)
{
    *sqltime = InternalGetTime(colIndex, NULL);
    return NOERROR;
}

ECode CJDBCResultSet::GetTime(
    /* [in] */ Int32 colIndex,
    /* [in] */ ICalendar * pCal,
    /* [out] */ ITime ** sqltime)
{
    *sqltime = InternalGetTime(colIndex, pCal);
    return NOERROR;
}

ECode CJDBCResultSet::GetTime(
    /* [in] */ const String& colName,
    /* [out] */ ITime ** sqltime)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    GetTime(col,sqltime);
    return NOERROR;
}

ECode CJDBCResultSet::GetTime(
    /* [in] */ const String& colName,
    /* [in] */ ICalendar * pCal,
    /* [out] */ ITime ** sqltime)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    GetTime(col, pCal,sqltime);
    return NOERROR;
}

ECode CJDBCResultSet::GetTimestamp(
    /* [in] */ Int32 colIndex,
    /* [out] */ ITimestamp ** timestamp)
{
    *timestamp = InternalGetTimestamp(colIndex, NULL);
    return NOERROR;
}

ECode CJDBCResultSet::GetTimestamp(
    /* [in] */ Int32 colIndex,
    /* [in] */ ICalendar * pCal,
    /* [out] */ ITimestamp ** timestamp)
{
    *timestamp = InternalGetTimestamp(colIndex, pCal);
    return NOERROR;
}

ECode CJDBCResultSet::GetTimestamp(
    /* [in] */ const String& colName,
    /* [out] */ ITimestamp ** timestamp)
{

    return NOERROR;
}

ECode CJDBCResultSet::GetTimestamp(
    /* [in] */ const String& colName,
    /* [in] */ ICalendar * pCal,
    /* [out] */ ITimestamp ** timestamp)
{

    return NOERROR;
}

ECode CJDBCResultSet::GetType(
    /* [out] */ Int32 * value)
{
    *value = IResultSet::TYPE_SCROLL_SENSITIVE;
    return NOERROR;
}

ECode CJDBCResultSet::GetUnicodeStream(
    /* [in] */ Int32 colIndex,
    /* [out] */ IInputStream ** iinputstream)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::GetUnicodeStream(
    /* [in] */ const String& colName,
    /* [out] */ IInputStream ** iinputstream)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return GetUnicodeStream(col,iinputstream);
}

ECode CJDBCResultSet::GetWarnings(
    /* [out] */ ISQLWarning ** sqlwarning)
{

    return E_SQL_EXCEPTION;
}

ECode CJDBCResultSet::InsertRow()
{
    Boolean isupdate = FALSE;
    IsUpdatable(&isupdate);
    if (!oninsrow || rowbuf == NULL) {
        return E_SQL_EXCEPTION;
    }
    AutoPtr<CJDBCResultSetMetaData> m;
    GetMetaData((IResultSetMetaData **)&m);
    StringBuffer sb;
    sb.Append("INSERT INTO ");
    sb.Append(SQLite::CShell::SqlQuoteDbl(uptable));
    sb.Append("(");
    for (Int32 i = 0; i < tr->mNcolumns; i++) {
        String str = String(NULL);
        m->GetColumnName(i + 1,&str);
        sb.Append(SQLite::CShell::SqlQuoteDbl(str));
        if (i < tr->mNcolumns - 1) {
            sb.Append(",");
        }
    }
    sb.Append(") VALUES(");
    for (Int32 i = 0; i < tr->mNcolumns; i++) {
        sb.Append(nullrepl ? "'%q'" : "%Q");
        if (i < tr->mNcolumns - 1) {
            sb.Append(",");
        }
    }
    sb.Append(")");
    ECode ec = ((CDatabaseX *)((CJDBCConnection *)((JDBCStatement *)s.Get())->conn.Get())->mDb.Get())->Exec(sb.ToString(),NULL, rowbuf);
    if (ec != NOERROR) {
        return E_SQL_EXCEPTION;
    }
    tr->Newrow(rowbuf, &isupdate);
    rowbuf = NULL;
    oninsrow = FALSE;
    GetLast(&isupdate);
    return NOERROR;
}

ECode CJDBCResultSet::IsAfterLast(
    /* [out] */ Boolean * value)
{
    if (tr == NULL || tr->mNrows <= 0) {
        *value = FALSE;
    }
    *value = mRow >= tr->mNrows;
    return NOERROR;
}

ECode CJDBCResultSet::IsBeforeFirst(
    /* [out] */ Boolean * value)
{
    if (tr == NULL || tr->mNrows <= 0) {
        *value = FALSE;
    }
    *value = mRow < 0;
    return NOERROR;
}

ECode CJDBCResultSet::IsFirst(
    /* [out] */ Boolean * value)
{
    if (tr == NULL) {
        *value = TRUE;
    }
    *value = mRow == 0;
    return NOERROR;
}

ECode CJDBCResultSet::IsLast(
    /* [out] */ Boolean * value)
{
    if (tr == NULL) {
        *value = TRUE;
    }
    *value = mRow == (tr->mNrows - 1);
    return NOERROR;
}

ECode CJDBCResultSet::GetLast(
    /* [out] */ Boolean * value)
{
    if (tr == NULL || tr->mNrows <= 0) {
        *value = FALSE;
    }
    mRow = tr->mNrows -1;
    *value = TRUE;
    return NOERROR;
}

ECode CJDBCResultSet::MoveToCurrentRow()
{
    if (oninsrow) {
        oninsrow = FALSE;
        rowbuf = NULL;
    }
    return NOERROR;
}

ECode CJDBCResultSet::MoveToInsertRow()
{
    Boolean isupdate = FALSE;
    IsUpdatable(&isupdate);
    if (!oninsrow) {
        oninsrow = TRUE;
        rowbuf = ArrayOf<String>::Alloc(tr->mNrows);
    }
    return NOERROR;
}

ECode CJDBCResultSet::GetNext(
    /* [out] */ Boolean * value)
{
    if (tr == NULL) {
        *value = FALSE;
    }
    mRow++;
    *value = mRow < tr->mNrows;
    return NOERROR;
}

ECode CJDBCResultSet::GetPrevious(
    /* [out] */ Boolean * value)
{
    if (tr == NULL) {
        return E_SQL_EXCEPTION;
    }
    if (mRow >= 0) {
        mRow--;
    }
    *value = mRow >= 0;
    return NOERROR;
}

ECode CJDBCResultSet::RefreshRow()
{
    Boolean isupdate = FALSE;
    IsUpdatable(&isupdate);
    if (oninsrow) {
        return E_SQL_EXCEPTION;
    }
    if (updatable < UPD_INSUPDDEL) {
        return E_SQL_EXCEPTION;
    }
    AutoPtr<CJDBCResultSetMetaData> m;
    GetMetaData((IResultSetMetaData **)&m);
    StringBuffer sb;
    AutoPtr<ArrayOf<String> > rd = tr->mRows[mRow];
    sb.Append("SELECT ");
    for (Int32 i = 0; i < tr->mNcolumns; i++) {
        String str = String(NULL);
        m->GetColumnName(i + 1,&str);
        sb.Append(SQLite::CShell::SqlQuoteDbl(str));
        if (i < tr->mNcolumns - 1) {
            sb.Append(",");
        }
    }
    sb.Append(" FROM ");
    sb.Append(SQLite::CShell::SqlQuoteDbl(uptable));
    sb.Append(" WHERE ");
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(pkcols->GetLength());
    for (Int32 i = 0; i < pkcols->GetLength(); i++) {
        sb.Append(SQLite::CShell::SqlQuoteDbl((*pkcols)[i]));
        sb.Append(String(" = ") + (nullrepl ? String("'%q'") : String("%Q")));
        if (i < pkcols->GetLength() - 1) {
            sb.Append(" AND ");
        }
        (*args)[i] = (*rd)[(*pkcoli)[i]];
    }
    AutoPtr<ITableResult> trnew ;
    ((CDatabaseX *)((CJDBCConnection *)((JDBCStatement *)s.Get())->conn.Get())->mDb.Get())->GetTable(sb.ToString(), args,(ITableResult **)&trnew);
    if (trnew == NULL) {
        return E_SQL_EXCEPTION;
    }
    if (((CTableResult *)trnew.Get())->mNrows != 1) {
        return E_SQL_EXCEPTION;
    }
    rowbuf = NULL;
    tr->mRows[mRow] = ((CTableResult *)trnew.Get())->mRows[0];
    return NOERROR;
}

ECode CJDBCResultSet::Relative(
    /* [in] */ Int32 rows,
    /* [out] */ Boolean * value)
{
    if (tr == NULL) {
        *value = FALSE;
        return NOERROR;
    }
    if (mRow + rows < 0 || mRow + rows >= tr->mNrows) {
        *value = FALSE;
        return NOERROR;
    }
    mRow += rows;
    *value = TRUE;
    return NOERROR;
}

ECode CJDBCResultSet::RowDeleted(
    /* [out] */ Boolean * value)
{
    *value = FALSE;
    return NOERROR;
}

ECode CJDBCResultSet::RowInserted(
    /* [out] */ Boolean * value)
{
    *value = FALSE;
    return NOERROR;
}

ECode CJDBCResultSet::RowUpdated(
    /* [out] */ Boolean * value)
{
    *value = FALSE;
    return NOERROR;
}

ECode CJDBCResultSet::SetFetchDirection(
    /* [in] */ Int32 direction)
{
    if (direction != IResultSet::FETCH_FORWARD) {
        return E_SQL_EXCEPTION;
    }
    return NOERROR;
}

ECode CJDBCResultSet::SetFetchSize(
    /* [in] */ Int32 rows)
{
    if (rows != 1) {
        return E_SQL_EXCEPTION;
    }
    return NOERROR;
}

ECode CJDBCResultSet::UpdateArray(
    /* [in] */ Int32 colIndex,
    /* [in] */ IArray * x)
{

    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateArray(
    /* [in] */ const String& colName,
    /* [in] */ IArray * x)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateArray(col, x);
}

ECode CJDBCResultSet::UpdateAsciiStream(
    /* [in] */ Int32 colIndex,
    /* [in] */ IInputStream * x,
    /* [in] */ Int32 length)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateAsciiStream(
    /* [in] */ const String& colName,
    /* [in] */ IInputStream * x,
    /* [in] */ Int32 length)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateAsciiStream(col,x, length);
}

ECode CJDBCResultSet::UpdateBigDecimal(
    /* [in] */ Int32 colIndex,
    /* [in] */ IBigDecimal * x)
{

    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateBigDecimal(
    /* [in] */ const String& colName,
    /* [in] */ IBigDecimal * x)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateBigDecimal(col, x);
}

ECode CJDBCResultSet::UpdateBinaryStream(
    /* [in] */ Int32 colIndex,
    /* [in] */ IInputStream * x,
    /* [in] */ Int32 length)
{

    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateBinaryStream(
    /* [in] */ const String& colName,
    /* [in] */ IInputStream * x,
    /* [in] */ Int32 length)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateBinaryStream(col,x, length);
}

ECode CJDBCResultSet::UpdateBlob(
    /* [in] */ Int32 colIndex,
    /* [in] */ Elastos::Sql::IBlob * x)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateBlob(
    /* [in] */ const String& colName,
    /* [in] */ Elastos::Sql::IBlob * x)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateBlob(col, x);
}

ECode CJDBCResultSet::UpdateBoolean(
    /* [in] */ Int32 colIndex,
    /* [in] */ Boolean x)
{
    return UpdateString(colIndex, x ? String("1") : String("0"));
}

ECode CJDBCResultSet::UpdateBoolean(
    /* [in] */ const String& colName,
    /* [in] */ Boolean x)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateBoolean(col, x);
}

ECode CJDBCResultSet::UpdateByte(
    /* [in] */ Int32 colIndex,
    /* [in] */ Byte x)
{

    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateByte(
    /* [in] */ const String& colName,
    /* [in] */ Byte x)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateByte(col, x);
}

ECode CJDBCResultSet::UpdateBytes(
    /* [in] */ Int32 colIndex,
    /* [in] */ const ArrayOf<Byte> & x)
{
    Boolean isupdate = FALSE;
    IsUpdatable(&isupdate);
    if (tr == NULL || colIndex < 1 || colIndex > tr->mNcolumns) {
        return E_SQL_EXCEPTION;
    }
    FillRowbuf();
    isupdate = FALSE;
    ((CJDBCConnection *)((JDBCStatement *)s.Get())->conn.Get())->mDb->Is3(&isupdate);
    if (isupdate) {
        (*rowbuf)[colIndex - 1] = SQLite::StringEncoder::EncodeX(const_cast<ArrayOf<Byte> *>(&x));
    } else {
        (*rowbuf)[colIndex - 1] = SQLite::StringEncoder::Encode(const_cast<ArrayOf<Byte> *>(&x));
    }
    return NOERROR;
}

ECode CJDBCResultSet::UpdateBytes(
    /* [in] */ const String& colName,
    /* [in] */ const ArrayOf<Byte> & x)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateBytes(col, x);
}

ECode CJDBCResultSet::UpdateCharacterStream(
    /* [in] */ Int32 colIndex,
    /* [in] */ IReader * x,
    /* [in] */ Int32 length)
{

    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateCharacterStream(
    /* [in] */ const String& colName,
    /* [in] */ IReader * reader,
    /* [in] */ Int32 length)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateCharacterStream(col,reader, length);
}

ECode CJDBCResultSet::UpdateClob(
    /* [in] */ Int32 colIndex,
    /* [in] */ IClob * x)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateClob(
    /* [in] */ const String& colName,
    /* [in] */ IClob * x)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateClob(col, x);
}

ECode CJDBCResultSet::UpdateDate(
    /* [in] */ Int32 colIndex,
    /* [in] */ ISQLDate * x)
{
    Boolean isupdate = FALSE;
    IsUpdatable(&isupdate);
    if (tr == NULL || colIndex < 1 || colIndex > tr->mNcolumns) {
        return E_SQL_EXCEPTION;
    }
    FillRowbuf();
    String str = String(NULL);
    IObject::Probe(x)->ToString(&str);
    (*rowbuf)[colIndex - 1] = str ;
    return NOERROR;
}

ECode CJDBCResultSet::UpdateDate(
    /* [in] */ const String& colName,
    /* [in] */ ISQLDate * x)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateDate(col, x);
}

ECode CJDBCResultSet::UpdateDouble(
    /* [in] */ Int32 colIndex,
    /* [in] */ Double x)
{
    Boolean isupdate = FALSE;
    IsUpdatable(&isupdate);
    if (tr == NULL || colIndex < 1 || colIndex > tr->mNcolumns) {
        return E_SQL_EXCEPTION;
    }
    FillRowbuf();
    (*rowbuf)[colIndex - 1] = StringUtils::ToString(x);
    return NOERROR;
}

ECode CJDBCResultSet::UpdateDouble(
    /* [in] */ const String& colName,
    /* [in] */ Double x)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateDouble(col, x);
}

ECode CJDBCResultSet::UpdateFloat(
    /* [in] */ Int32 colIndex,
    /* [in] */ Float x)
{
    Boolean isupdate = FALSE;
    IsUpdatable(&isupdate);
    if (tr == NULL || colIndex < 1 || colIndex > tr->mNcolumns) {
        return E_SQL_EXCEPTION;
    }
    FillRowbuf();
    (*rowbuf)[colIndex - 1] = StringUtils::ToString(x);
    return NOERROR;
}

ECode CJDBCResultSet::UpdateFloat(
    /* [in] */ const String& colName,
    /* [in] */ Float x)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateFloat(col, x);
}

ECode CJDBCResultSet::UpdateInt(
    /* [in] */ Int32 colIndex,
    /* [in] */ Int32 x)
{
    Boolean isupdate = FALSE;
    IsUpdatable(&isupdate);
    if (tr == NULL || colIndex < 1 || colIndex > tr->mNcolumns) {
        return E_SQL_EXCEPTION;
    }
    FillRowbuf();
    (*rowbuf)[colIndex - 1] = StringUtils::ToString(x);
    return NOERROR;
}

ECode CJDBCResultSet::UpdateInt(
    /* [in] */ const String& colName,
    /* [in] */ Int32 x)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateInt(col, x);
}

ECode CJDBCResultSet::UpdateLong(
    /* [in] */ Int32 colIndex,
    /* [in] */ Int64 x)
{
    Boolean isupdate = FALSE;
    IsUpdatable(&isupdate);
    if (tr == NULL || colIndex < 1 || colIndex > tr->mNcolumns) {
        return E_SQL_EXCEPTION;
    }
    FillRowbuf();
    (*rowbuf)[colIndex - 1] = StringUtils::ToString(x);
    return NOERROR;
}

ECode CJDBCResultSet::UpdateLong(
    /* [in] */ const String& colName,
    /* [in] */ Int64 x)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateLong(col, x);
}

ECode CJDBCResultSet::UpdateNull(
    /* [in] */ Int32 colIndex)
{
    Boolean isupdate = FALSE;
    IsUpdatable(&isupdate);
    if (tr == NULL || colIndex < 1 || colIndex > tr->mNcolumns) {
        return E_SQL_EXCEPTION;
    }
    FillRowbuf();
    (*rowbuf)[colIndex - 1] = String(NULL);
    return NOERROR;
}

ECode CJDBCResultSet::UpdateNull(
    /* [in] */ const String& colName)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateNull(col);
}

ECode CJDBCResultSet::UpdateObject(
    /* [in] */ Int32 colIndex,
    /* [in] */ IInterface * x)
{
    // TODO:
    // UpdateString(colIndex, ((StringBuffer *)x)->ToString());
    return NOERROR;
}

ECode CJDBCResultSet::UpdateObject(
    /* [in] */ Int32 colIndex,
    /* [in] */ IInterface * x,
    /* [in] */ Int32 scale)
{
    // TODO:
    // UpdateString(colIndex, ((StringBuffer *)x)->ToString());
    return NOERROR;
}

ECode CJDBCResultSet::UpdateObject(
    /* [in] */ const String& colName,
    /* [in] */ IInterface * x)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateObject(col,x);
}

ECode CJDBCResultSet::UpdateObject(
    /* [in] */ const String& colName,
    /* [in] */ IInterface * x,
    /* [in] */ Int32 scale)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateObject(col,x,scale);
}

ECode CJDBCResultSet::UpdateRef(
    /* [in] */ Int32 colIndex,
    /* [in] */ IRef * x)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateRef(
    /* [in] */ const String& colName,
    /* [in] */ IRef * x)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateRef(col, x);
}

ECode CJDBCResultSet::UpdateRow()
{
    Boolean isupdate = FALSE;
    IsUpdatable(&isupdate);
    if (rowbuf == NULL) {
        return E_SQL_EXCEPTION;
    }
    if (oninsrow) {
        return E_SQL_EXCEPTION;
    }
    if (updatable < UPD_INSUPDDEL) {
        return E_SQL_EXCEPTION;
    }
    AutoPtr<ArrayOf<String> > rd = tr->mRows[mRow];
    AutoPtr<CJDBCResultSetMetaData> m;
    GetMetaData((IResultSetMetaData **)&m);
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(tr->mNcolumns + pkcols->GetLength());
    StringBuffer sb;
    sb.Append("UPDATE ");
    sb.Append(SQLite::CShell::SqlQuoteDbl(uptable));
    sb.Append(" SET ");
    Int32 i = 0;
    for (i = 0; i < tr->mNcolumns; i++) {
        String str = String(NULL);
        m->GetColumnName(i + 1,&str);
        sb.Append(SQLite::CShell::SqlQuoteDbl(str));
        sb.Append(String(" = ") + (nullrepl ? String("'%q'") : String("%Q")));
        if (i < tr->mNcolumns - 1) {
            sb.Append(",");
        }
        (*args)[i] = (*rowbuf)[i];
    }
    sb.Append(" WHERE ");
    for (Int32 k = 0; k < pkcols->GetLength(); k++, i++) {
        sb.Append(SQLite::CShell::SqlQuoteDbl((*pkcols)[k]));
        sb.Append(String(" = ") + (nullrepl ? String("'%q'") : String("%Q")));
        if (k < pkcols->GetLength() - 1) {
        sb.Append(" AND ");
        }
        (*args)[i] = (*rd)[(*pkcoli)[k]];
    }
    ECode ec = ((CDatabaseX *)((CJDBCConnection *)((JDBCStatement *)s.Get())->conn.Get())->mDb.Get())->Exec(sb.ToString(),NULL, args);
    if (ec != NOERROR)
    {
        return E_SQL_EXCEPTION;
    }
    rd->Copy(rowbuf,0,rowbuf->GetLength());
    rowbuf = NULL;
    return NOERROR;
}

ECode CJDBCResultSet::UpdateShort(
    /* [in] */ Int32 colIndex,
    /* [in] */ Int16 x)
{
    Boolean isupdate = FALSE;
    IsUpdatable(&isupdate);
    if (tr == NULL || colIndex < 1 || colIndex > tr->mNcolumns) {
        return E_SQL_EXCEPTION;
    }
    FillRowbuf();
    (*rowbuf)[colIndex - 1] = StringUtils::ToString(x);
    return NOERROR;
}

ECode CJDBCResultSet::UpdateShort(
    /* [in] */ const String& colName,
    /* [in] */ Int16 x)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateShort(col, x);
}

ECode CJDBCResultSet::UpdateString(
    /* [in] */ Int32 colIndex,
    /* [in] */ const String& x)
{
    Boolean isupdate = FALSE;
    IsUpdatable(&isupdate);
    if (tr == NULL || colIndex < 1 || colIndex > tr->mNcolumns) {
        return E_SQL_EXCEPTION;
    }
    FillRowbuf();
    (*rowbuf)[colIndex - 1] = x;
    return NOERROR;
}

ECode CJDBCResultSet::UpdateString(
    /* [in] */ const String& colName,
    /* [in] */ const String& x)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateString(col, x);
}

ECode CJDBCResultSet::UpdateTime(
    /* [in] */ Int32 colIndex,
    /* [in] */ ITime * x)
{
    Boolean isupdate = FALSE;
    IsUpdatable(&isupdate);
    if (tr == NULL || colIndex < 1 || colIndex > tr->mNcolumns) {
        return E_SQL_EXCEPTION;
    }
    FillRowbuf();
    String str = String(NULL);
    IObject::Probe(x)->ToString(&str);
    (*rowbuf)[colIndex - 1] = str ;
    return NOERROR;
}

ECode CJDBCResultSet::UpdateTime(
    /* [in] */ const String& colName,
    /* [in] */ ITime * x)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateTime(col, x);
}

ECode CJDBCResultSet::UpdateTimestamp(
    /* [in] */ Int32 colIndex,
    /* [in] */ ITimestamp * x)
{
    Boolean isupdate = FALSE;
    IsUpdatable(&isupdate);
    if (tr == NULL || colIndex < 1 || colIndex > tr->mNcolumns) {
        return E_SQL_EXCEPTION;
    }
    FillRowbuf();
    String str = String(NULL);
    IObject::Probe(x)->ToString(&str);
    (*rowbuf)[colIndex - 1] = str ;
    return NOERROR;
}

ECode CJDBCResultSet::UpdateTimestamp(
    /* [in] */ const String& colName,
    /* [in] */ ITimestamp * x)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateTimestamp(col, x);
}

ECode CJDBCResultSet::WasNull(
    /* [out] */ Boolean * value)
{
    *value = lastg == NULL;
    return NOERROR;
}

ECode CJDBCResultSet::GetRowId(
    /* [in] */ Int32 colIndex,
    /* [out] */ IRowId ** rowid)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::GetRowId(
    /* [in] */ const String& colName,
    /* [out] */ IRowId ** rowid)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return GetRowId(col, rowid);
}

ECode CJDBCResultSet::UpdateRowId(
    /* [in] */ Int32 colIndex,
    /* [in] */ IRowId * value)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateRowId(
    /* [in] */ const String& colName,
    /* [in] */ IRowId * value)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateRowId(col, value);
}

ECode CJDBCResultSet::GetHoldability(
    /* [out] */ Int32 * value)
{
    *value = IResultSet::CLOSE_CURSORS_AT_COMMIT;
    return NOERROR;
}

ECode CJDBCResultSet::IsClosed(
    /* [out] */ Boolean * value)
{
    *value = tr == NULL;
    return NOERROR;
}

ECode CJDBCResultSet::UpdateNString(
    /* [in] */ Int32 colIndex,
    /* [in] */ const String& nString)
{

    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateNString(
    /* [in] */ const String& colName,
    /* [in] */ const String& nString)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateNString(col, nString);
}

ECode CJDBCResultSet::UpdateNClob(
    /* [in] */ Int32 colIndex,
    /* [in] */ INClob * nclob)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateNClob(
    /* [in] */ const String& colName,
    /* [in] */ INClob * nclob)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateNClob(col, nclob);
}

ECode CJDBCResultSet::GetNClob(
    /* [in] */ Int32 colIndex,
    /* [out] */ INClob ** nclob)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::GetNClob(
    /* [in] */ const String& colName,
    /* [out] */ INClob ** nclob)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return GetNClob(col, nclob);
}

ECode CJDBCResultSet::GetSQLXML(
    /* [in] */ Int32 colIndex,
    /* [out] */ ISQLXML ** sqlxml)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::GetSQLXML(
    /* [in] */ const String& colName,
    /* [out] */ ISQLXML ** sqlxml)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return GetSQLXML(col, sqlxml);
}

ECode CJDBCResultSet::UpdateSQLXML(
    /* [in] */ Int32 colIndex,
    /* [in] */ ISQLXML * xmlObject)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateSQLXML(
    /* [in] */ const String& colName,
    /* [in] */ ISQLXML * xmlObject)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateSQLXML(col, xmlObject);
}

ECode CJDBCResultSet::GetNString(
    /* [in] */ Int32 colIndex,
    /* [out] */ String * nstr)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::GetNString(
    /* [in] */ const String& colName,
    /* [out] */ String * nstr)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return GetNString(col, nstr);
}

ECode CJDBCResultSet::GetNCharacterStream(
    /* [in] */ Int32 colIndex,
    /* [out] */ IReader ** reader)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::GetNCharacterStream(
    /* [in] */ const String& colName,
    /* [out] */ IReader ** reader)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return GetNCharacterStream(col, reader);
}

ECode CJDBCResultSet::UpdateNCharacterStream(
    /* [in] */ Int32 colIndex,
    /* [in] */ IReader * x,
    /* [in] */ Int64 length)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateNCharacterStream(
    /* [in] */ const String& colName,
    /* [in] */ IReader * reader,
    /* [in] */ Int64 length)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateNCharacterStream(col,reader, length);
}

ECode CJDBCResultSet::UpdateAsciiStream(
    /* [in] */ Int32 colIndex,
    /* [in] */ IInputStream * x,
    /* [in] */ Int64 length)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateBinaryStream(
    /* [in] */ Int32 colIndex,
    /* [in] */ IInputStream * x,
    /* [in] */ Int64 length)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateCharacterStream(
    /* [in] */ Int32 colIndex,
    /* [in] */ IReader * x,
    /* [in] */ Int64 length)
{

    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateAsciiStream(
    /* [in] */ const String& colName,
    /* [in] */ IInputStream * x,
    /* [in] */ Int64 length)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateAsciiStream(col,x,length);
}

ECode CJDBCResultSet::UpdateBinaryStream(
    /* [in] */ const String& colName,
    /* [in] */ IInputStream * x,
    /* [in] */ Int64 length)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateBinaryStream(col,x,length);
}

ECode CJDBCResultSet::UpdateCharacterStream(
    /* [in] */ const String& colName,
    /* [in] */ IReader * reader,
    /* [in] */ Int64 length)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateCharacterStream(col,reader,length);
}

ECode CJDBCResultSet::UpdateBlob(
    /* [in] */ Int32 colIndex,
    /* [in] */ IInputStream * pInputStream,
    /* [in] */ Int64 length)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateBlob(
    /* [in] */ const String& colName,
    /* [in] */ IInputStream * pInputStream,
    /* [in] */ Int64 length)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateBlob(col,pInputStream,length);
}

ECode CJDBCResultSet::UpdateClob(
    /* [in] */ Int32 colIndex,
    /* [in] */ IReader * reader,
    /* [in] */ Int64 length)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateClob(
    /* [in] */ const String& colName,
    /* [in] */ IReader * reader,
    /* [in] */ Int64 length)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateClob(col,reader,length);
}

ECode CJDBCResultSet::UpdateNClob(
    /* [in] */ Int64 colIndex,
    /* [in] */ IReader * reader,
    /* [in] */ Int64 length)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateNClob(
    /* [in] */ const String& colName,
    /* [in] */ IReader * reader,
    /* [in] */ Int64 length)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateNClob(col,reader,length);
}

ECode CJDBCResultSet::UpdateNCharacterStream(
    /* [in] */ Int32 colIndex,
    /* [in] */ IReader * x)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateNCharacterStream(
    /* [in] */ const String& colName,
    /* [in] */ IReader * reader)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateNCharacterStream(col, reader);
}

ECode CJDBCResultSet::UpdateAsciiStream(
    /* [in] */ Int32 colIndex,
    /* [in] */ IInputStream * x)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateBinaryStream(
    /* [in] */ Int32 colIndex,
    /* [in] */ IInputStream * x)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateCharacterStream(
    /* [in] */ Int32 colIndex,
    /* [in] */ IReader * px)
{

    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateAsciiStream(
    /* [in] */ const String& colName,
    /* [in] */ IInputStream * x)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateAsciiStream(col,x);
}

ECode CJDBCResultSet::UpdateBinaryStream(
    /* [in] */ const String& colName,
    /* [in] */ IInputStream * x)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateBinaryStream(col,x);
}

ECode CJDBCResultSet::UpdateCharacterStream(
    /* [in] */ const String& colName,
    /* [in] */ IReader * reader)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateCharacterStream(col,reader);
}

ECode CJDBCResultSet::UpdateBlob(
    /* [in] */ Int32 colIndex,
    /* [in] */ IInputStream * pInputStream)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateBlob(
    /* [in] */ const String& colName,
    /* [in] */ IInputStream * pInputStream)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateBlob(col,pInputStream);
}

ECode CJDBCResultSet::UpdateClob(
    /* [in] */ Int32 colIndex,
    /* [in] */ IReader * reader)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateClob(
    /* [in] */ const String& colName,
    /* [in] */ IReader * reader)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateClob(col,reader);
}

ECode CJDBCResultSet::UpdateNClob(
    /* [in] */ Int32 colIndex,
    /* [in] */ IReader * reader)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::UpdateNClob(
    /* [in] */ const String& colName,
    /* [in] */ IReader * reader)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return UpdateNClob(col,reader);
}

ECode CJDBCResultSet::constructor(
    /* [in] */ ITableResult * pTr,
    /* [in] */ IJDBCStatement * pS)
{
    tr = (CTableResult *)pTr;
    s = pS;
    md = NULL;
    lastg = NULL;
    mRow = -1;
    updatable = UPD_UNKNOWN;
    oninsrow = FALSE;
    rowbuf = NULL;
    return NOERROR;
}

ECode CJDBCResultSet::IsWrapperFor(
    /* [in] */ IInterface * iface,
    /* [out] */ Boolean * value)
{
    return NOERROR;
}

ECode CJDBCResultSet::GetArray(
    /* [in] */ Int32 colIndex,
    /* [out] */ IArray ** iarray)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::GetArray(
    /* [in] */ const String& colName,
    /* [out] */ IArray ** iarray)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return GetArray(col,iarray);
}

ECode CJDBCResultSet::GetDate(
    /* [in] */ Int32 colIndex,
    /* [out] */ ISQLDate ** idate)
{
    *idate = InternalGetDate(colIndex, NULL);
    return NOERROR;
}

ECode CJDBCResultSet::GetDate(
    /* [in] */ Int32 colIndex,
    /* [in] */ ICalendar * cal,
    /* [out] */ ISQLDate ** idate)
{
    *idate = InternalGetDate(colIndex, cal);
    return NOERROR;
}

ECode CJDBCResultSet::GetDate(
    /* [in] */ const String& colName,
    /* [out] */ ISQLDate ** idate)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    GetDate(col,idate);
    return NOERROR;
}

ECode CJDBCResultSet::GetDate(
    /* [in] */ const String& colName,
    /* [in] */ ICalendar * cal,
    /* [out] */ ISQLDate ** idate)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    GetDate(col, cal,idate);
    return NOERROR;
}

ECode CJDBCResultSet::GetObject(
    /* [in] */ Int32 colIndex,
    /* [in] */ IMap * map,
    /* [out] */ IInterface ** obj)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCResultSet::GetObject(
    /* [in] */ const String& colName,
    /* [out] */ IInterface** obj)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return GetObject(col,obj);
}

ECode CJDBCResultSet::GetObject(
    /* [in] */ const String& colName,
    /* [in] */ IMap * map,
    /* [out] */ IInterface** obj)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return GetObject(col, map, obj);
}

ECode CJDBCResultSet::GetURL(
    /* [in] */ Int32 colIndex,
    /* [out] */ IURL** url)
{
    if (tr == NULL || colIndex < 1 || colIndex > tr->mNcolumns) {
        return E_SQL_EXCEPTION;
    }
    AutoPtr<ArrayOf<String> > rd = tr->mRows[mRow];
    lastg = (*rd)[colIndex - 1];
    if (lastg == NULL) {
        *url = NULL;
        return NOERROR;
    }
    //TODO
    assert(0);
    ECode ec = NOERROR;//CURL::New(lastg,url);
    if (FAILED(ec)) {
        *url = NULL;
    }
    return NOERROR;
}

ECode CJDBCResultSet::GetURL(
    /* [in] */ const String& colName,
    /* [out] */ IURL** url)
{
    Int32 col = 0;
    FindColumn(colName,&col);
    return GetURL(col,url);
}

ECode CJDBCResultSet::Unwrap(
    /* [in] */ PInterface iface,
    /* [out] */ IInterface ** oface)
{
    return NOERROR;
}

ECode CJDBCResultSet::IsUpdatable(
    /* [out] */ Boolean * value)
{
    if (updatable == UPD_UNKNOWN) {
        AutoPtr<CJDBCResultSetMetaData> m ;
        GetMetaData((IResultSetMetaData **)&m);
        HashSet<String> h;
        String lastt = String(NULL);
        for (Int32 i = 1; i <= tr->mNcolumns; i++) {
            m->GetTableName(i,&lastt);
            h.Insert(lastt);
        }
        if (h.GetSize() > 1 || lastt.IsNull()) {
            updatable = UPD_NO;
            return E_SQL_EXCEPTION;
        }
        updatable = UPD_INS;
        uptable = lastt;
        AutoPtr<IDatabaseMetaData> mdatabase;
        ECode ec1 = IConnection::Probe(((JDBCStatement *)s.Get())->conn)->GetMetaData((IDatabaseMetaData **)&mdatabase);
        AutoPtr<IResultSet> res;
        ECode ec2 = mdatabase->GetPrimaryKeys(String(NULL), String(NULL), uptable, (IResultSet**)&res);
        AutoPtr<CJDBCResultSet> pk = (CJDBCResultSet*)res.Get();
        if (pk->tr->mNrows > 0) {
            Boolean colnotfound = FALSE;
            pkcols = ArrayOf<String>::Alloc(pk->tr->mNrows);
            pkcoli = ArrayOf<Int32>::Alloc(pk->tr->mNrows);
            for (Int32 i = 0; i < pk->tr->mNrows; i++) {
                AutoPtr<ArrayOf<String> > rd = pk->tr->mRows[i];
                (*pkcols)[i] = (*rd)[3];
                Int32 midvalue = 0;
                ECode ec = FindColumn((*pkcols)[i],&midvalue);
                (*pkcoli)[i] = midvalue - 1;
                if ( ec != NOERROR)
                {
                    colnotfound = TRUE;
                }
            }
            if (!colnotfound) {
                updatable = UPD_INSUPDDEL;
            }
        }
        pk->Close();
        if (ec1 !=NOERROR || ec2 != NOERROR)
        {
            updatable = UPD_NO;
        }
    }
    if (updatable < UPD_INS) {
        return E_SQL_EXCEPTION;
    }
    *value = TRUE;
    return NOERROR;
}

ECode CJDBCResultSet::FillRowbuf()
{
    if (rowbuf == NULL) {
        if (mRow < 0) {
            return E_SQL_EXCEPTION;
        }
        rowbuf = ArrayOf<String>::Alloc(tr->mNcolumns);
        rowbuf->Copy(tr->mRows[mRow],tr->mNcolumns);
    }
    return NOERROR;
}

ECode CJDBCResultSet::FindColumn(
    /* [in] */ const String& colName,
    /* [out] */ Int32 * value)
{
    AutoPtr<IJDBCResultSetMetaData> m ;
    GetMetaData((IResultSetMetaData **)&m);
    m->FindColByName(colName,value);
    return NOERROR;
}

AutoPtr<IInteger32> CJDBCResultSet::InternalGetInt(
    /* [in] */ Int32 colIndex)
{
    if (tr == NULL || colIndex < 1 || colIndex > tr->mNcolumns) {
        return NULL;
    }
    AutoPtr<ArrayOf<String> > rd = tr->mRows[mRow];
    lastg = (*rd)[colIndex - 1];
    // try {
    // AutoPtr<IInteger32> out;
    //     return Integer.valueOf(lastg);
    // } catch (java.lang.Exception e) {
    //     lastg = null;
    // }
    return NULL;
}

AutoPtr<IInteger16> CJDBCResultSet::InternalGetShort(
    /* [in] */ Int32 colIndex)
{
    if (tr == NULL || colIndex < 1 || colIndex > tr->mNcolumns) {
        return NULL;
    }
    AutoPtr<ArrayOf<String> > rd = tr->mRows[mRow];
    lastg = (*rd)[colIndex - 1];
    // try {
    //     return Short.valueOf(lastg);
    // } catch (java.lang.Exception e) {
    //     lastg = null;
    // }
    return NULL;
}

AutoPtr<ITime> CJDBCResultSet::InternalGetTime(
    /* [in] */ Int32 colIndex,
    /* [in] */ ICalendar * cal)
{
    if (tr == NULL || colIndex < 1 || colIndex > tr->mNcolumns) {
        return NULL;
    }
    AutoPtr<ArrayOf<String> > rd = tr->mRows[mRow];
    AutoPtr<ITime> otime;
    ECode ec = NOERROR;
    lastg = (*rd)[colIndex - 1];

    if (((CJDBCConnection *)((JDBCStatement *)s.Get())->conn.Get())->mUseJulian) {
        ec = CTime::New(SQLite::Database::LongFromJulian(lastg),(ITime **)&otime);
        if (ec != NOERROR) {
            otime = NULL;
            CTime::ValueOf(lastg, (ITime**)&otime);
            return otime;
        }
    }
    else {
        CTime::ValueOf(lastg, (ITime**)&otime);
        if (otime != NULL) {
            return otime;
        }
        else{
            CTime::New(SQLite::Database::LongFromJulian(lastg),(ITime **)&otime);
            return otime;
        }
    }

    if (ec != NOERROR)
    {
        lastg = NULL;
    }
    return NULL;
}

AutoPtr<ITimestamp> CJDBCResultSet::InternalGetTimestamp(
    /* [in] */ Int32 colIndex,
    /* [in] */ ICalendar * cal)
{
    if (tr == NULL || colIndex < 1 || colIndex > tr->mNcolumns) {
        return NULL;
    }
    AutoPtr<ArrayOf<String> > rd = tr->mRows[mRow];
    lastg = (*rd)[colIndex - 1];
    AutoPtr<ITimestamp> otime;
    ECode ec = NOERROR;

    if (((CJDBCConnection *)((JDBCStatement *)s.Get())->conn.Get())->mUseJulian) {
        ec = CTimestamp::New(SQLite::Database::LongFromJulian(lastg),(ITimestamp ** )&otime);
        if (ec != NOERROR) {
            otime = NULL;
            CTimestamp::ValueOf(lastg, (ITimestamp**)&otime);
            return otime;
        }
    }
    else {
        CTimestamp::ValueOf(lastg, (ITimestamp**)&otime);
        if (otime) {
            return otime;
        }
        else{
            CTimestamp::New(SQLite::Database::LongFromJulian(lastg),(ITimestamp ** )&otime);
            return otime;
        }
    }
    if(ec != NOERROR) {
        lastg = String(NULL);
    }
    return NULL;
}

AutoPtr<ISQLDate> CJDBCResultSet::InternalGetDate(
    /* [in] */ Int32 colIndex,
    /* [in] */ ICalendar * cal)
{
    if (tr == NULL || colIndex < 1 || colIndex > tr->mNcolumns) {
        return NULL;
    }
    AutoPtr<ArrayOf<String> > rd = tr->mRows[mRow];
    lastg = (*rd)[colIndex - 1];
    AutoPtr<ISQLDate> otime;
    ECode ec = NOERROR;

    if (((CJDBCConnection *)((JDBCStatement *)s.Get())->conn.Get())->mUseJulian) {
        ec = CSqlDate::New(SQLite::Database::LongFromJulian(lastg), (ISQLDate **)&otime);
        if (ec != NOERROR) {
            otime = NULL;
            CSqlDate::ValueOf(lastg, (ISQLDate**)&otime);
            return otime;
        }
    }
    else {
            CSqlDate::ValueOf(lastg, (ISQLDate**)&otime);
            if (otime) {
                return otime;
            }
            else {
                CSqlDate::New(SQLite::Database::LongFromJulian(lastg),(ISQLDate **)&otime);
                return otime;
            }
    }

    if (ec != NOERROR) {
        lastg = NULL;
    }
    return NULL;
}

AutoPtr<IDouble> CJDBCResultSet::InternalGetDouble(
    /* [in] */ Int32 colIndex)
{
    if (tr == NULL || colIndex < 1 || colIndex > tr->mNcolumns) {
        return NULL;
    }
    AutoPtr<ArrayOf<String> > rd = tr->mRows[mRow];
    lastg = (*rd)[colIndex - 1];
    // try {
    //     return Double.valueOf(lastg);
    // } catch (java.lang.Exception e) {
    //     lastg = null;
    // }
    return NULL;
}

AutoPtr<IFloat> CJDBCResultSet::InternalGetFloat(
    /* [in] */ Int32 colIndex)
{
    if (tr == NULL || colIndex < 1 || colIndex > tr->mNcolumns) {
        return NULL;
    }
    AutoPtr<ArrayOf<String> > rd = tr->mRows[mRow];
    lastg = (*rd)[colIndex - 1];
    // try {
    //     return Float.valueOf(lastg);
    // } catch (java.lang.Exception e) {
    //     lastg = null;
    // }
    return NULL;
}

AutoPtr<IInteger64> CJDBCResultSet::InternalGetLong(
    /* [in] */ Int32 colIndex)
{
    if (tr == NULL || colIndex < 1 || colIndex > tr->mNcolumns) {
        return NULL;
    }
    AutoPtr<ArrayOf<String> > rd = tr->mRows[mRow];
    lastg = (*rd)[colIndex - 1];
    // try {
    //     return Long.valueOf(lastg);
    // } catch (java.lang.Exception e) {
    //     lastg = null;
    // }
    return NULL;
}

} // namespace JDBC
} // namespace SQLite
} // namespace Sql
} // namespace Elastos
