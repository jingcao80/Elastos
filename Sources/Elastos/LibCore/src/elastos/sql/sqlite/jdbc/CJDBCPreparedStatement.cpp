
#include "Elastos.CoreLibrary.Utility.h"
#include "CJDBCPreparedStatement.h"
#include "Database.h"
#include "CJDBCConnection.h"
#include "CDatabaseX.h"
#include "StringEncoder.h"
#include "Database.h"
#include "StringBuffer.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuffer;
using Elastos::Core::StringUtils;
using Elastos::Utility::IDate;

namespace Elastos {
namespace Sql {
namespace SQLite {
namespace JDBC{

const Boolean CJDBCPreparedStatement::mNullrepl =
    SQLite::Database::Version().Compare("2.5.0") < 0;

CAR_OBJECT_IMPL(CJDBCPreparedStatement)

CAR_INTERFACE_IMPL(CJDBCPreparedStatement, JDBCStatement, IJDBCPreparedStatement);

ECode CJDBCPreparedStatement::constructor(
    /* [in] */ IJDBCConnection * pConn,
    /* [in] */ const String& sql)
{
    Init(pConn,sql);
    return NOERROR;
}

CJDBCPreparedStatement::~CJDBCPreparedStatement()
{
    ClearBatch();
}

void CJDBCPreparedStatement::Init(
    /* [in] */ IJDBCConnection * pConn,
    /* [in] */ const String& isql)
{
    conn = pConn;
    mArgs = NULL;
    mBlobs = NULL;
    sql = Fixup(isql);
}

String CJDBCPreparedStatement::Fixup(
    /* [in] */ const String& sql)
{
    StringBuffer sb;
    Boolean inq = FALSE;
    Int32 nparm = 0;
    Char32 c, nextChar;
    AutoPtr<ArrayOf<Char32> > charArray = sql.GetChars();
    for (Int32 i = 0; i < charArray->GetLength(); i++) {
        c = (*charArray)[i];
        if (c == '\'') {
            if (inq) {
                nextChar = 0;
                if (i + 1 < charArray->GetLength()) {
                    nextChar = (*charArray)[i + 1];
                }
                if (nextChar == '\'') {
                    sb.AppendChar(c);
                    sb.AppendChar(nextChar);
                    i++;
                } else {
                    inq = FALSE;
                    sb.AppendChar(c);
                }
            } else {
                inq = TRUE;
                sb.AppendChar(c);
            }
        }
        else if (c == '?') {
            if (inq) {
                sb.AppendChar(c);
            } else {
                ++nparm;
                sb.Append(mNullrepl ? "'%q'" : "%Q");
            }
        } else if (c == ';') {
            if (!inq) {
                break;
            }
            sb.AppendChar(c);
        }
        else if (c == '%') {
            sb.Append("%%");
        }
        else {
            sb.AppendChar(c);
        }
    }
    mArgs = ArrayOf<String>::Alloc(nparm);
    mBlobs = ArrayOf<Boolean>::Alloc(nparm);
    ClearParameters();

    return sb.ToString();
}

String CJDBCPreparedStatement::Fixup2(
    /* [in] */ const String& sql)
{
    Boolean dbflag = FALSE;
    ((CDatabaseX* )((CJDBCConnection* )conn.Get())->mDb.Get())->Is3(&dbflag);
    if (!dbflag) {
        return sql;
    }
    StringBuffer sb;
    Int32 parm = -1;
    Char32 c;
    AutoPtr<ArrayOf<Char32> > charArray = sql.GetChars();
    for (Int32 i = 0; i < charArray->GetLength(); i++) {
        c = (*charArray)[i];
        if (c == '%') {
            sb.AppendChar(c);
            ++i;
            c = (*charArray)[i];
            if (c == 'Q') {
                parm++;
                if ((*mBlobs)[parm]) {
                    c = 's';
                }
            }
        }
        sb.AppendChar(c);
    }
    return sb.ToString();
}

ECode CJDBCPreparedStatement::AddBatch()
{
    if (mBatch == NULL) {
        mBatch = new List<AutoPtr<BatchArg> >(mArgs->GetLength());
    }
    for (Int32 i = 0; i < mArgs->GetLength(); i++) {
        AutoPtr<BatchArg> arg = new BatchArg((*mArgs)[i], (*mBlobs)[i]);
        mBatch->PushBack(arg);
    }
    return NOERROR;
}

ECode CJDBCPreparedStatement::ExecuteBatch(
    /* [out] */ ArrayOf<Int32> ** outarray)
{
    if (mBatch == NULL) {
        *outarray = ArrayOf<Int32>::Alloc(0);
        REFCOUNT_ADD(*outarray);
        return NOERROR;
    }

    AutoPtr<ArrayOf<Int32> > ret =
            ArrayOf<Int32>::Alloc(mBatch->GetSize() / mArgs->GetLength());
    for (Int32 i = 0; i < ret->GetLength(); i++) {
        (*ret)[i] = IStatement::EXECUTE_FAILED;
    }
    Int32 errs = 0;
    Int32 index = 0;
    for (Int32 i = 0; i < ret->GetLength(); i++) {
        for (Int32 k = 0; k < mArgs->GetLength(); k++) {
            AutoPtr<BatchArg> b = (*mBatch)[index++];
            (*mArgs)[k] = b->arg;
            (*mBlobs)[k] = b->blob;
        }
        ECode ec = ExecuteUpdate(&((*ret)[i]));
        if (ec != NOERROR)
        {
            ++errs;
        }
    }
    if (errs > 0) {
        return E_SQL_SQLITE_JDBC_BATCHUPDATE_EXCEPTION;
    }
    *outarray = ret;
    REFCOUNT_ADD(*outarray)
    return NOERROR;
}

ECode CJDBCPreparedStatement::ClearBatch()
{
    if (mBatch) {
        mBatch = NULL;
    }
    return NOERROR;
}

ECode CJDBCPreparedStatement::Close()
{
    ClearBatch();
    JDBCStatement::Close();
    return NOERROR;
}

ECode CJDBCPreparedStatement::ClearParameters()
{
    for (Int32 i = 0; i < mArgs->GetLength(); i++) {
        (*mArgs)[i] = mNullrepl ? String("") : String(NULL);
        (*mBlobs)[i] = FALSE;
    }
    return NOERROR;
}

ECode CJDBCPreparedStatement::Execute(
    /* [out] */ Boolean * value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<IResultSet> set;
    JDBCStatement::ExecuteQuery(Fixup2(sql), mArgs, FALSE, (IResultSet**)&set);
    *value = set != NULL;
    return NOERROR;
}

ECode CJDBCPreparedStatement::ExecuteQuery(
    /* [out] */ IResultSet ** resultset)
{
    return JDBCStatement::ExecuteQuery(Fixup2(sql), mArgs, FALSE, resultset);
}

ECode CJDBCPreparedStatement::ExecuteUpdate(
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<IResultSet> set;
    ECode ec = JDBCStatement::ExecuteQuery(Fixup2(sql), mArgs, TRUE, (IResultSet**)&set);
    *value = updcnt;
    return ec;
}
ECode CJDBCPreparedStatement::GetMetaData(
    /* [out] */ IResultSetMetaData ** resultsetmeta)
{
    VALIDATE_NOT_NULL(*resultsetmeta);
    return IResultSet::Probe(rs)->GetMetaData(resultsetmeta);
}

ECode CJDBCPreparedStatement::GetParameterMetaData(
    /* [out] */ IParameterMetaData ** parametermeta)
{
    VALIDATE_NOT_NULL(*parametermeta);
    *parametermeta = NULL;
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetArray(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IArray * theArray)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetAsciiStream(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IInputStream * theInputStream,
    /* [in] */ Int32 length)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetBigDecimal(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IBigDecimal * theBigDecimal)
{
    if (parameterIndex < 1 || parameterIndex > mArgs->GetLength()) {
        return E_SQL_EXCEPTION;
    }
    if (theBigDecimal == NULL) {
        (*mArgs)[parameterIndex - 1] = mNullrepl ? String("") : String(NULL);
    } else {
        String  outstr;
        IObject::Probe(theBigDecimal)->ToString(&outstr);
        (*mArgs)[parameterIndex - 1] = String("") + outstr;
    }
    (*mBlobs)[parameterIndex - 1] = FALSE;
    return NOERROR;
}

ECode CJDBCPreparedStatement::SetBinaryStream(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IInputStream * theInputStream,
    /* [in] */ Int32 length)
{

    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(length);
    Int32 value = 0;
    ECode ec = theInputStream->Read(data, 0, length,&value);
    if (ec !=NOERROR)
    {
        return E_SQL_EXCEPTION;
    }
    ec = SetBytes(parameterIndex, *data);
    if (ec !=NOERROR)
    {
        return E_SQL_EXCEPTION;
    }
    return NOERROR;
}

ECode CJDBCPreparedStatement::SetBlob(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ Elastos::Sql::IBlob * theBlob)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetBoolean(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ Boolean theBoolean)
{
    if (parameterIndex < 1 || parameterIndex > mArgs->GetLength()) {
        return E_SQL_EXCEPTION;
    }
    (*mArgs)[parameterIndex - 1] = theBoolean ? String("1") : String("0");
    (*mBlobs)[parameterIndex - 1] = FALSE;
    return NOERROR;
}

ECode CJDBCPreparedStatement::SetByte(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ Byte theByte)
{
    if (parameterIndex < 1 || parameterIndex > mArgs->GetLength()) {
        return E_SQL_EXCEPTION;
    }
    String str("");
    str += theByte;
    (*mArgs)[parameterIndex - 1] = str;
    (*mBlobs)[parameterIndex - 1] = FALSE;
    return NOERROR;
}

ECode CJDBCPreparedStatement::SetBytes(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ const ArrayOf<Byte>& theBytes)
{
    if (parameterIndex < 1 || parameterIndex > mArgs->GetLength()) {
        return E_SQL_EXCEPTION;
    }
    (*mBlobs)[parameterIndex - 1] = FALSE;
    if (theBytes.GetLength()) {
        (*mArgs)[parameterIndex - 1] = mNullrepl ? String("") : String(NULL);
    }
    else {
        Boolean dbflag = FALSE;
        ((CDatabaseX* )((CJDBCConnection* )conn.Get())->mDb.Get())->Is3(&dbflag);
        if (dbflag) {
            (*mArgs)[parameterIndex - 1] = SQLite::StringEncoder::EncodeX((ArrayOf<Byte> *)&theBytes);
            (*mBlobs)[parameterIndex - 1] = TRUE;
        }
        else {
            (*mArgs)[parameterIndex - 1] = SQLite::StringEncoder::Encode((ArrayOf<Byte> *)&theBytes);
        }
    }
    return NOERROR;
}

ECode CJDBCPreparedStatement::SetCharacterStream(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IReader * reader,
    /* [in] */ Int32 length)
{
    AutoPtr<ArrayOf<Char32> > data = ArrayOf<Char32>::Alloc(length);
    Int32 outlen = 0;
    ECode ec = reader->Read(data,&outlen);
    if (ec != NOERROR)
    {
        return E_SQL_EXCEPTION;
    }
    ec = SetString(parameterIndex, String(*data, 0, outlen));
    if (ec != NOERROR)
    {
        return E_SQL_EXCEPTION;
    }
    return NOERROR;
}

ECode CJDBCPreparedStatement::SetClob(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IClob * theClob)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetDate(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ ISQLDate * theDate)
{
    if (parameterIndex < 1 || parameterIndex > mArgs->GetLength()) {
        return E_SQL_EXCEPTION;
    }
    if (theDate == NULL) {
        (*mArgs)[parameterIndex - 1] = mNullrepl ? String("") : String(NULL);
    } else {
        if (((CJDBCConnection* )conn.Get())->mUseJulian) {
            Int64 outtime = 0;
            IDate::Probe(theDate)->GetTime(&outtime);
            (*mArgs)[parameterIndex - 1] = StringUtils::ToString(SQLite::Database::JulianFromLong(outtime));
        } else {
            String outstr;
            IObject::Probe(theDate)->ToString(&outstr);
            (*mArgs)[parameterIndex - 1] = outstr;
        }
    }
    (*mBlobs)[parameterIndex - 1] = FALSE;
    return NOERROR;
}

ECode CJDBCPreparedStatement::SetDate(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ ISQLDate * theDate,
    /* [in] */ ICalendar * cal)
{
    return SetDate(parameterIndex, theDate);
}

ECode CJDBCPreparedStatement::SetDouble(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ Double theDouble)
{
    if (parameterIndex < 1 || parameterIndex > mArgs->GetLength()) {
        return E_SQL_EXCEPTION;
    }
    (*mArgs)[parameterIndex - 1] = String("") + StringUtils::ToString(theDouble);
    (*mBlobs)[parameterIndex - 1] = FALSE;
    return NOERROR;
}

ECode CJDBCPreparedStatement::SetFloat(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ Float theFloat)
{
    if (parameterIndex < 1 || parameterIndex > mArgs->GetLength()) {
        return E_SQL_EXCEPTION;
    }
    (*mArgs)[parameterIndex - 1] = String("") + StringUtils::ToString(theFloat);
    (*mBlobs)[parameterIndex - 1] = FALSE;
    return NOERROR;
}

ECode CJDBCPreparedStatement::SetInt32(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ Int32 theInt)
{
    if (parameterIndex < 1 || parameterIndex > mArgs->GetLength()) {
        return E_SQL_EXCEPTION;
    }
    (*mArgs)[parameterIndex - 1] = String("") + StringUtils::ToString(theInt);
    (*mBlobs)[parameterIndex - 1] = FALSE;
    return NOERROR;
}

ECode CJDBCPreparedStatement::SetInt64(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ Int64 theLong)
{
    if (parameterIndex < 1 || parameterIndex > mArgs->GetLength()) {
        return E_SQL_EXCEPTION;
    }
    (*mArgs)[parameterIndex - 1] = String("") + StringUtils::ToString(theLong);
    (*mBlobs)[parameterIndex - 1] = FALSE;
    return NOERROR;
}

ECode CJDBCPreparedStatement::SetNull(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ Int32 sqlType)
{
    if (parameterIndex < 1 || parameterIndex > mArgs->GetLength()) {
        return E_SQL_EXCEPTION;
    }
    (*mArgs)[parameterIndex - 1] = mNullrepl ? String("") : String(NULL);
    (*mBlobs)[parameterIndex - 1] = FALSE;
    return NOERROR;
}

ECode CJDBCPreparedStatement::SetNull(
    /* [in] */ Int32 paramIndex,
    /* [in] */ Int32 sqlType,
    /* [in] */ const String& typeName)
{
    return SetNull(paramIndex, sqlType);
}

ECode CJDBCPreparedStatement::SetObject(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IObject * theObject)
{
    if (parameterIndex < 1 || parameterIndex > mArgs->GetLength()) {
        return E_SQL_EXCEPTION;
    }
    if (theObject == NULL) {
        (*mArgs)[parameterIndex - 1] = mNullrepl ? String("") : String(NULL);
    } else {
        AutoPtr<Elastos::Core::IArrayOf > arr =
            (Elastos::Core::IArrayOf* )theObject->Probe(Elastos::Core::EIID_IArrayOf);
        if (arr) {
            AutoPtr<ArrayOf<Byte> > bx = (ArrayOf<Byte> *)theObject;
            Boolean dbflag = FALSE;
            ((CDatabaseX* )((CJDBCConnection* )conn.Get())->mDb.Get())->Is3(&dbflag);
            if (dbflag) {
                (*mArgs)[parameterIndex - 1] = SQLite::StringEncoder::EncodeX(bx);
                (*mBlobs)[parameterIndex - 1] = TRUE;
                return  NOERROR;
            }
            (*mArgs)[parameterIndex - 1] = SQLite::StringEncoder::Encode(bx);
        } else {
            String str;
            theObject->ToString(&str);
            (*mArgs)[parameterIndex - 1] = str;
        }
    }
    (*mBlobs)[parameterIndex - 1] = FALSE;
    return NOERROR;
}

ECode CJDBCPreparedStatement::SetObject(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IObject * theObject,
    /* [in] */ Int32 targetSqlType)
{
    if (parameterIndex < 1 || parameterIndex > mArgs->GetLength()) {
        return E_SQL_EXCEPTION;
    }
    if (theObject == NULL) {
        (*mArgs)[parameterIndex - 1] = mNullrepl ? String("") : String(NULL);
    } else {
        AutoPtr<Elastos::Core::IArrayOf > arr =
            (Elastos::Core::IArrayOf* )theObject->Probe(Elastos::Core::EIID_IArrayOf);
        if (arr) {
            AutoPtr<ArrayOf<Byte> > bx = (ArrayOf<Byte> *)theObject;
            Boolean dbflag = FALSE;
            if (dbflag) {
            ((CDatabaseX* )((CJDBCConnection* )conn.Get())->mDb.Get())->Is3(&dbflag);
                (*mArgs)[parameterIndex - 1] = SQLite::StringEncoder::EncodeX(bx);
                (*mBlobs)[parameterIndex - 1] = TRUE;
                return  NOERROR;
            }
            (*mArgs)[parameterIndex - 1] = SQLite::StringEncoder::Encode(bx);
        } else {
            String str;
            theObject->ToString(&str);
            (*mArgs)[parameterIndex - 1] = str;
        }
    }
    (*mBlobs)[parameterIndex - 1] = FALSE;
    return NOERROR;
}

ECode CJDBCPreparedStatement::SetObject(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IObject * theObject,
    /* [in] */ Int32 targetSqlType,
    /* [in] */ Int32 scale)
{
    if (parameterIndex < 1 || parameterIndex > mArgs->GetLength()) {
        return E_SQL_EXCEPTION;
    }
    if (theObject == NULL) {
        (*mArgs)[parameterIndex - 1] = mNullrepl ? String("") : String(NULL);
    } else {
        AutoPtr<Elastos::Core::IArrayOf > arr =
            (Elastos::Core::IArrayOf* )theObject->Probe(Elastos::Core::EIID_IArrayOf);
        if (arr) {
            AutoPtr<ArrayOf<Byte> > bx = (ArrayOf<Byte> *)theObject;
            Boolean dbflag = FALSE;
            ((CDatabaseX* )((CJDBCConnection* )conn.Get())->mDb.Get())->Is3(&dbflag);
            if (dbflag) {
                (*mArgs)[parameterIndex - 1] = SQLite::StringEncoder::EncodeX(bx);
                (*mBlobs)[parameterIndex - 1] = TRUE;
                return  NOERROR;
            }
            (*mArgs)[parameterIndex - 1] = SQLite::StringEncoder::Encode(bx);
        } else {
            String str;
            theObject->ToString(&str);
            (*mArgs)[parameterIndex - 1] = str;
        }
    }
    (*mBlobs)[parameterIndex - 1] = FALSE;
    return NOERROR;
}

ECode CJDBCPreparedStatement::SetRef(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IRef * theRef)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetInt16(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ Int16 theShort)
{
    if (parameterIndex < 1 || parameterIndex > mArgs->GetLength()) {
        return E_SQL_EXCEPTION;
    }
    (*mArgs)[parameterIndex - 1] = String("") + StringUtils::ToString(theShort);
    (*mBlobs)[parameterIndex - 1] = FALSE;
    return NOERROR;
}

ECode CJDBCPreparedStatement::SetString(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ const String& theString)
{
    if (parameterIndex < 1 || parameterIndex > mArgs->GetLength()) {
        return E_SQL_EXCEPTION;
    }
    if (theString.IsNull()) {
        (*mArgs)[parameterIndex - 1] = mNullrepl ? String("") : String(NULL);
    } else {
        (*mArgs)[parameterIndex - 1] = theString;
    }
    (*mBlobs)[parameterIndex - 1] = FALSE;
    return NOERROR;
}

ECode CJDBCPreparedStatement::SetTime(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ ITime * theTime)
{
    if (parameterIndex < 1 || parameterIndex > mArgs->GetLength()) {
        return E_SQL_EXCEPTION;
    }
    if (theTime == NULL) {
        (*mArgs)[parameterIndex - 1] = mNullrepl ? String("") : String(NULL);
    } else {
        if (((CJDBCConnection* )conn.Get())->mUseJulian) {
            Int64 outtime = 0;
            IDate::Probe(theTime)->GetTime(&outtime);
            (*mArgs)[parameterIndex - 1] = StringUtils::ToString(SQLite::Database::JulianFromLong(outtime));
        } else {
            String outstr;
            IObject::Probe(theTime)->ToString(&outstr);
            (*mArgs)[parameterIndex - 1] = outstr;
        }
    }
    (*mBlobs)[parameterIndex - 1] = FALSE;
    return NOERROR;
}

ECode CJDBCPreparedStatement::SetTime(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ ITime * theTime,
    /* [in] */ ICalendar * cal)
{
    SetTime(parameterIndex, theTime);
    return NOERROR;
}

ECode CJDBCPreparedStatement::SetTimestamp(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ ITimestamp * theTimestamp)
{
    if (parameterIndex < 1 || parameterIndex > mArgs->GetLength()) {
        return E_SQL_EXCEPTION;
    }
    if (theTimestamp == NULL) {
        (*mArgs)[parameterIndex - 1] = mNullrepl ? String("") : String(NULL);
    } else {
        if (((CJDBCConnection* )conn.Get())->mUseJulian) {
            Int64 outtime = 0;
            IDate::Probe(theTimestamp)->GetTime(&outtime);
            (*mArgs)[parameterIndex - 1] = StringUtils::ToString(SQLite::Database::JulianFromLong(outtime));
        } else {
            String outstr;
            IObject::Probe(theTimestamp)->ToString(&outstr);
            (*mArgs)[parameterIndex - 1] = outstr;
        }
    }
    (*mBlobs)[parameterIndex - 1] = FALSE;
    return NOERROR;
}

ECode CJDBCPreparedStatement::SetTimestamp(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ ITimestamp * theTimestamp,
    /* [in] */ ICalendar * cal)
{
    return SetTimestamp(parameterIndex, theTimestamp);
}

ECode CJDBCPreparedStatement::SetUnicodeStream(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IInputStream * theInputStream,
    /* [in] */ Int32 length)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetURL(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IURL * theURL)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetRowId(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IRowId * theRowId)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetRowId(
    /* [in] */ const String& parameterName,
    /* [in] */ IRowId * theRowId)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetNString(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ const String& theString)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetNString(
    /* [in] */ const String& parameterName,
    /* [in] */ const String& theString)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetNCharacterStream(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IReader * reader,
    /* [in] */ Int64 length)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetNCharacterStream(
    /* [in] */ const String& parameterName,
    /* [in] */ IReader * reader,
    /* [in] */ Int64 length)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetNClob(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ INClob * value)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetNClob(
    /* [in] */ const String& parameterName,
    /* [in] */ INClob * value)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetClob(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IReader * reader,
    /* [in] */ Int64 length)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetClob(
    /* [in] */ const String& parameterName,
    /* [in] */ IReader * reader,
    /* [in] */ Int64 length)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetBlob(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IInputStream * inputStream,
    /* [in] */ Int64 length)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetBlob(
    /* [in] */ const String& parameterName,
    /* [in] */ IInputStream * inputStream,
    /* [in] */ Int64 length)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetNClob(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IReader * reader,
    /* [in] */ Int64 length)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetNClob(
    /* [in] */ const String& parameterName,
    /* [in] */ IReader * reader,
    /* [in] */ Int64 length)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetSQLXML(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ ISQLXML * xmlObject)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetSQLXML(
    /* [in] */ const String& parameterName,
    /* [in] */ ISQLXML * xmlObject)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetAsciiStream(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IInputStream * inputStream,
    /* [in] */ Int64 length)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetAsciiStream(
    /* [in] */ const String& parameterName,
    /* [in] */ IInputStream * inputStream,
    /* [in] */ Int64 length)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetBinaryStream(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IInputStream * inputStream,
    /* [in] */ Int64 length)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetBinaryStream(
    /* [in] */ const String& parameterName,
    /* [in] */ IInputStream * inputStream,
    /* [in] */ Int64 length)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetCharacterStream(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IReader * reader,
    /* [in] */ Int64 length)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetCharacterStream(
    /* [in] */ const String& parameterName,
    /* [in] */ IReader * reader,
    /* [in] */ Int64 length)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetAsciiStream(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IInputStream * inputStream)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetAsciiStream(
    /* [in] */ const String& parameterName,
    /* [in] */ IInputStream * inputStream)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetBinaryStream(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IInputStream * inputStream)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetBinaryStream(
    /* [in] */ const String& parameterName,
    /* [in] */ IInputStream * inputStream)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetCharacterStream(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IReader * reader)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetCharacterStream(
    /* [in] */ const String& parameterName,
    /* [in] */ IReader * reader)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetNCharacterStream(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IReader * reader)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetNCharacterStream(
    /* [in] */ const String& parameterName,
    /* [in] */ IReader * reader)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetClob(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IReader * reader)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetClob(
    /* [in] */ const String& parameterName,
    /* [in] */ IReader * reader)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetBlob(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IInputStream * inputStream)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetBlob(
    /* [in] */ const String& parameterName,
    /* [in] */ IInputStream * inputStream)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetNClob(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IReader * reader)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetNClob(
    /* [in] */ const String& parameterName,
    /* [in] */ IReader * reader)
{
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
}

ECode CJDBCPreparedStatement::RegisterOutputParameter(
    /* [in] */ const String& parameterName,
    /* [in] */ Int32 sqlType)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::RegisterOutputParameter(
    /* [in] */ const String& parameterName,
    /* [in] */ Int32 sqlType,
    /* [in] */ Int32 scale)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::RegisterOutputParameter(
    /* [in] */ const String& parameterName,
    /* [in] */ Int32 sqlType,
    /* [in] */ const String& typeName)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetURL(
    /* [in] */ Int32 parameterIndex,
    /* [out] */ IURL ** url)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetNull(
    /* [in] */ const String& parameterName,
    /* [in] */ Int32 sqlType)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetBoolean(
    /* [in] */ const String& parameterName,
    /* [in] */ Boolean val)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetByte(
    /* [in] */ const String& parameterName,
    /* [in] */ Byte val)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetInt16(
    /* [in] */ const String& parameterName,
    /* [in] */ Int16 val)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetInt32(
    /* [in] */ const String& parameterName,
    /* [in] */ Int32 val)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetInt64(
    /* [in] */ const String& parameterName,
    /* [in] */ Int64 val)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetFloat(
    /* [in] */ const String& parameterName,
    /* [in] */ Float val)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetDouble(
    /* [in] */ const String& parameterName,
    /* [in] */ Double val)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetBigDecimal(
    /* [in] */ const String& parameterName,
    /* [in] */ IBigDecimal * val)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetString(
    /* [in] */ const String& parameterName,
    /* [in] */ const String& val)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetBytes(
    /* [in] */ const String& parameterName,
    /* [in] */ const ArrayOf<Byte>& val)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetDate(
    /* [in] */ const String& parameterName,
    /* [in] */ ISQLDate * val)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetTime(
    /* [in] */ const String& parameterName,
    /* [in] */ ITime * val)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetTimestamp(
    /* [in] */ const String& parameterName,
    /* [in] */ ITimestamp * val)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetAsciiStream(
    /* [in] */ const String& parameterName,
    /* [in] */ IInputStream * s,
    /* [in] */ Int32 length)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetBinaryStream(
    /* [in] */ const String& parameterName,
    /* [in] */ IInputStream * s,
    /* [in] */ Int32 length)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetObject(
    /* [in] */ const String& parameterName,
    /* [in] */ IObject * val,
    /* [in] */ Int32 tarGetSqlType,
    /* [in] */ Int32 scale)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetObject(
    /* [in] */ const String& parameterName,
    /* [in] */ IObject * val,
    /* [in] */ Int32 tarGetSqlType)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetObject(
    /* [in] */ const String& parameterName,
    /* [in] */ IObject * val)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetCharacterStream(
    /* [in] */ const String& parameterName,
    /* [in] */ IReader * r,
    /* [in] */ Int32 length)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetDate(
    /* [in] */ const String& parameterName,
    /* [in] */ ISQLDate * val,
    /* [in] */ ICalendar * cal)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetTime(
    /* [in] */ const String& parameterName,
    /* [in] */ ITime * val,
    /* [in] */ ICalendar * cal)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetTimestamp(
    /* [in] */ const String& parameterName,
    /* [in] */ ITimestamp * val,
    /* [in] */ ICalendar * cal)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::SetNull(
    /* [in] */ const String& parameterName,
    /* [in] */ Int32 sqlType,
    /* [in] */ const String& typeName)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetString(
    /* [in] */ const String& parameterName,
    /* [out] */ String * str)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetBoolean(
    /* [in] */ const String& parameterName,
    /* [out] */ Boolean * value)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetByte(
    /* [in] */ const String& parameterName,
    /* [out] */ Byte * outbyte)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetInt16(
    /* [in] */ const String& parameterName,
    /* [out] */ Int16 * value)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetInt32(
    /* [in] */ const String& parameterName,
    /* [out] */ Int32 * value)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetInt64(
    /* [in] */ const String& parameterName,
    /* [out] */ Int64 * value)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetFloat(
    /* [in] */ const String& parameterName,
    /* [out] */ Float * value)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetDouble(
    /* [in] */ const String& parameterName,
    /* [out] */ Double * value)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetBytes(
    /* [in] */ const String& parameterName,
        /* [out, callee] */ ArrayOf<Byte> ** outarray)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetDate(
    /* [in] */ const String& parameterName,
    /* [out] */ ISQLDate ** thedate)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetTime(
    /* [in] */ const String& parameterName,
    /* [out] */ ITime ** thetime)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetTimestamp(
    /* [in] */ const String& parameterName,
    /* [out] */ ITimestamp ** timestamp)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetObject(
    /* [in] */ const String& parameterName,
    /* [out] */ IObject ** theobject)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetObject(
    /* [in] */ Int32 parameterIndex,
    /* [out] */ IObject ** theobject)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetBigDecimal(
    /* [in] */ const String& parameterName,
    /* [out] */ IBigDecimal ** bigdecimal)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetObject(
    /* [in] */ const String& parameterName,
    /* [in] */ IMap * map,
    /* [out] */ IObject ** theobject)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetObject(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ IMap * map,
    /* [out] */ IObject ** theobject)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetRef(
    /* [in] */ Int32 parameterIndex,
    /* [out] */ IRef ** iref)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetRef(
    /* [in] */ const String& parameterName,
    /* [out] */ IRef ** iref)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetBlob(
    /* [in] */ const String& parameterName,
    /* [out] */ Elastos::Sql::IBlob ** blob)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetBlob(
    /* [in] */ Int32 parameterIndex,
    /* [out] */ Elastos::Sql::IBlob ** blob)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetClob(
    /* [in] */ const String& parameterName,
    /* [out] */ IClob ** clob)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetClob(
    /* [in] */ Int32 parameterIndex,
    /* [out] */ IClob ** clob)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetArray(
    /* [in] */ const String& parameterName,
    /* [out] */ IArray ** outarray)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetArray(
    /* [in] */ Int32 parameterIndex,
    /* [out] */ IArray ** outarray)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetDate(
    /* [in] */ const String& parameterName,
    /* [in] */ ICalendar * cal,
    /* [out] */ ISQLDate ** thedate)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetDate(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ ICalendar * cal,
    /* [out] */ ISQLDate ** thedate)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetTime(
    /* [in] */ const String& parameterName,
    /* [in] */ ICalendar * cal,
    /* [out] */ ITime ** thetime)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetTime(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ ICalendar * cal,
    /* [out] */ ITime ** thetime)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetTimestamp(
    /* [in] */ const String& parameterName,
    /* [in] */ ICalendar * cal,
    /* [out] */ ITimestamp ** timestamp)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetTimestamp(
    /* [in] */ Int32 parameterIndex,
    /* [in] */ ICalendar * cal,
    /* [out] */ ITimestamp ** timestamp)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCPreparedStatement::GetURL(
    /* [in] */ const String& parameterName,
    /* [out] */ IURL ** url)
{
    return E_SQL_EXCEPTION;
}

} // namespace JDBC
} // namespace SQLite
} // namespace Sql
} // namespace Elastos
