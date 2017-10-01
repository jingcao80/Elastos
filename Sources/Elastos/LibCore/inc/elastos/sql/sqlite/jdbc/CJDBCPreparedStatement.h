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

#ifndef __ELASTOS_SQL_SQLITE_JDBC_CJDBCPREPAREDSTATEMENT_H__
#define __ELASTOS_SQL_SQLITE_JDBC_CJDBCPREPAREDSTATEMENT_H__

#include "_Elastos_Sql_SQLite_JDBC_CJDBCPreparedStatement.h"
#include "JDBCStatement.h"

using Elastos::Net::IURL;
using Elastos::IO::IReader;
using Elastos::IO::IInputStream;
using Elastos::Math::IBigDecimal;
using Elastos::Utility::ICalendar;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Sql {
namespace SQLite {
namespace JDBC{

class BatchArg : public ElLightRefBase {
public:
    BatchArg(String iarg, Boolean iblob) {
        if (iarg.IsNull()) {
            arg = String(NULL);
        } else {
            arg = iarg;
        }
        blob = iblob;
    }

public:
    String arg;
    Boolean blob;
};

CarClass(CJDBCPreparedStatement)
    , public JDBCStatement
    , public IJDBCPreparedStatement
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    ~CJDBCPreparedStatement();

    CARAPI constructor(
        /* [in] */ IJDBCConnection * pConn,
        /* [in] */ const String& sql);

    using JDBCStatement::AddBatch;

    CARAPI AddBatch();

    CARAPI ExecuteBatch(
        /* [out] */ ArrayOf<Int32> ** outarray);

    CARAPI ClearBatch();

    CARAPI Close();

    CARAPI ClearParameters();

    using JDBCStatement::Execute;

    CARAPI Execute(
        /* [out] */ Boolean * value);

    using JDBCStatement::ExecuteQuery;

    CARAPI ExecuteQuery(
        /* [out] */ IResultSet ** resultset);

    using JDBCStatement::ExecuteUpdate;

    CARAPI ExecuteUpdate(
        /* [out] */ Int32 * value);

    CARAPI GetMetaData(
        /* [out] */ IResultSetMetaData ** resultsetmeta);

    CARAPI GetParameterMetaData(
        /* [out] */ IParameterMetaData ** parametermeta);

    CARAPI SetArray(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IArray * theArray);

    CARAPI SetAsciiStream(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IInputStream * theInputStream,
        /* [in] */ Int32 length);

    CARAPI SetBigDecimal(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IBigDecimal * theBigDecimal);

    CARAPI SetBinaryStream(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IInputStream * theInputStream,
        /* [in] */ Int32 length);

    CARAPI SetBlob(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ Elastos::Sql::IBlob * theBlob);

    CARAPI SetBoolean(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ Boolean theBoolean);

    CARAPI SetByte(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ Byte theByte);

    CARAPI SetBytes(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ const ArrayOf<Byte>& theBytes);

    CARAPI SetCharacterStream(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IReader * reader,
        /* [in] */ Int32 length);

    CARAPI SetClob(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IClob * theClob);

    CARAPI SetDate(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ ISQLDate * theDate);

    CARAPI SetDate(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ ISQLDate * theDate,
        /* [in] */ ICalendar * cal);

    CARAPI SetDouble(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ Double theDouble);

    CARAPI SetFloat(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ Float theFloat);

    CARAPI SetInt32(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ Int32 theInt);

    CARAPI SetInt64(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ Int64 theLong);

    CARAPI SetNull(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ Int32 sqlType);

    CARAPI SetNull(
        /* [in] */ Int32 paramIndex,
        /* [in] */ Int32 sqlType,
        /* [in] */ const String& typeName);

    CARAPI SetObject(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IObject * theObject);

    CARAPI SetObject(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IObject * theObject,
        /* [in] */ Int32 targetSqlType);

    CARAPI SetObject(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IObject * theObject,
        /* [in] */ Int32 targetSqlType,
        /* [in] */ Int32 scale);

    CARAPI SetRef(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IRef * theRef);

    CARAPI SetInt16(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ Int16 theShort);

    CARAPI SetString(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ const String& theString);

    CARAPI SetTime(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ ITime * theTime);

    CARAPI SetTime(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ ITime * theTime,
        /* [in] */ ICalendar * cal);

    CARAPI SetTimestamp(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ ITimestamp * theTimestamp);

    CARAPI SetTimestamp(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ ITimestamp * theTimestamp,
        /* [in] */ ICalendar * cal);

    CARAPI SetUnicodeStream(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IInputStream * theInputStream,
        /* [in] */ Int32 length);

    CARAPI SetURL(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IURL * theURL);

    CARAPI RegisterOutputParameter(
        /* [in] */ const String& parameterName,
        /* [in] */ Int32 sqlType);

    CARAPI RegisterOutputParameter(
        /* [in] */ const String& parameterName,
        /* [in] */ Int32 sqlType,
        /* [in] */ Int32 scale);

    CARAPI RegisterOutputParameter(
        /* [in] */ const String& parameterName,
        /* [in] */ Int32 sqlType,
        /* [in] */ const String& typeName);

    CARAPI GetURL(
        /* [in] */ Int32 parameterIndex,
        /* [out] */ IURL ** url);

    CARAPI SetNull(
        /* [in] */ const String& parameterName,
        /* [in] */ Int32 sqlType);

    CARAPI SetBoolean(
        /* [in] */ const String& parameterName,
        /* [in] */ Boolean val);

    CARAPI SetByte(
        /* [in] */ const String& parameterName,
        /* [in] */ Byte val);

    CARAPI SetInt16(
        /* [in] */ const String& parameterName,
        /* [in] */ Int16 val);

    CARAPI SetInt32(
        /* [in] */ const String& parameterName,
        /* [in] */ Int32 val);

    CARAPI SetInt64(
        /* [in] */ const String& parameterName,
        /* [in] */ Int64 val);

    CARAPI SetFloat(
        /* [in] */ const String& parameterName,
        /* [in] */ Float val);

    CARAPI SetDouble(
        /* [in] */ const String& parameterName,
        /* [in] */ Double val);

    CARAPI SetBigDecimal(
        /* [in] */ const String& parameterName,
        /* [in] */ IBigDecimal * val);

    CARAPI SetString(
        /* [in] */ const String& parameterName,
        /* [in] */ const String& val);

    CARAPI SetBytes(
        /* [in] */ const String& parameterName,
        /* [in] */ const ArrayOf<Byte>& val);

    CARAPI SetDate(
        /* [in] */ const String& parameterName,
        /* [in] */ ISQLDate * val);

    CARAPI SetTime(
        /* [in] */ const String& parameterName,
        /* [in] */ ITime * val);

    CARAPI SetTimestamp(
        /* [in] */ const String& parameterName,
        /* [in] */ ITimestamp * val);

    CARAPI SetAsciiStream(
        /* [in] */ const String& parameterName,
        /* [in] */ IInputStream * s,
        /* [in] */ Int32 length);

    CARAPI SetBinaryStream(
        /* [in] */ const String& parameterName,
        /* [in] */ IInputStream * s,
        /* [in] */ Int32 length);

    CARAPI SetObject(
        /* [in] */ const String& parameterName,
        /* [in] */ IObject * val,
        /* [in] */ Int32 tarGetSqlType,
        /* [in] */ Int32 scale);

    CARAPI SetObject(
        /* [in] */ const String& parameterName,
        /* [in] */ IObject * val,
        /* [in] */ Int32 tarGetSqlType);

    CARAPI SetObject(
        /* [in] */ const String& parameterName,
        /* [in] */ IObject * val);

    CARAPI SetCharacterStream(
        /* [in] */ const String& parameterName,
        /* [in] */ IReader * r,
        /* [in] */ Int32 length);

    CARAPI SetDate(
        /* [in] */ const String& parameterName,
        /* [in] */ ISQLDate * val,
        /* [in] */ ICalendar * cal);

    CARAPI SetTime(
        /* [in] */ const String& parameterName,
        /* [in] */ ITime * val,
        /* [in] */ ICalendar * cal);

    CARAPI SetTimestamp(
        /* [in] */ const String& parameterName,
        /* [in] */ ITimestamp * val,
        /* [in] */ ICalendar * cal);

    CARAPI SetNull(
        /* [in] */ const String& parameterName,
        /* [in] */ Int32 sqlType,
        /* [in] */ const String& typeName);

    CARAPI GetString(
        /* [in] */ const String& parameterName,
        /* [out] */ String * str);

    CARAPI GetBoolean(
        /* [in] */ const String& parameterName,
        /* [out] */ Boolean * value);

    CARAPI GetByte(
        /* [in] */ const String& parameterName,
        /* [out] */ Byte * outbyte);

    CARAPI GetInt16(
        /* [in] */ const String& parameterName,
        /* [out] */ Int16 * value);

    CARAPI GetInt32(
        /* [in] */ const String& parameterName,
        /* [out] */ Int32 * value);

    CARAPI GetInt64(
        /* [in] */ const String& parameterName,
        /* [out] */ Int64 * value);

    CARAPI GetFloat(
        /* [in] */ const String& parameterName,
        /* [out] */ Float * value);

    CARAPI GetDouble(
        /* [in] */ const String& parameterName,
        /* [out] */ Double * value);

    CARAPI GetBytes(
        /* [in] */ const String& parameterName,
        /* [out, callee] */ ArrayOf<Byte> ** outarray);

    CARAPI GetDate(
        /* [in] */ const String& parameterName,
        /* [out] */ ISQLDate ** thedate);

    CARAPI GetTime(
        /* [in] */ const String& parameterName,
        /* [out] */ ITime ** thetime);

    CARAPI GetTimestamp(
        /* [in] */ const String& parameterName,
        /* [out] */ ITimestamp ** timestamp);

    CARAPI GetObject(
        /* [in] */ const String& parameterName,
        /* [out] */ IObject ** theobject);

    CARAPI GetObject(
        /* [in] */ Int32 parameterIndex,
        /* [out] */ IObject ** theobject);

    CARAPI GetBigDecimal(
        /* [in] */ const String& parameterName,
        /* [out] */ IBigDecimal ** bigdecimal);

    CARAPI GetObject(
        /* [in] */ const String& parameterName,
        /* [in] */ IMap * map,
        /* [out] */ IObject ** theobject);

    CARAPI GetObject(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IMap * map,
        /* [out] */ IObject ** theobject);

    CARAPI GetRef(
        /* [in] */ Int32 parameterIndex,
        /* [out] */ IRef ** iref);

    CARAPI GetRef(
        /* [in] */ const String& parameterName,
        /* [out] */ IRef ** iref);

    CARAPI GetBlob(
        /* [in] */ const String& parameterName,
        /* [out] */ Elastos::Sql::IBlob ** blob);

    CARAPI GetBlob(
        /* [in] */ Int32 parameterIndex,
        /* [out] */ Elastos::Sql::IBlob ** blob);

    CARAPI GetClob(
        /* [in] */ const String& parameterName,
        /* [out] */ IClob ** clob);

    CARAPI GetClob(
        /* [in] */ Int32 parameterIndex,
        /* [out] */ IClob ** clob);

    CARAPI GetArray(
        /* [in] */ const String& parameterName,
        /* [out] */ IArray ** outarray);

    CARAPI GetArray(
        /* [in] */ Int32 parameterIndex,
        /* [out] */ IArray ** outarray);

    CARAPI GetDate(
        /* [in] */ const String& parameterName,
        /* [in] */ ICalendar * cal,
        /* [out] */ ISQLDate ** thedate);

    CARAPI GetDate(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ ICalendar * cal,
        /* [out] */ ISQLDate ** thedate);

    CARAPI GetTime(
        /* [in] */ const String& parameterName,
        /* [in] */ ICalendar * cal,
        /* [out] */ ITime ** thetime);

    CARAPI GetTime(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ ICalendar * cal,
        /* [out] */ ITime ** thetime);

    CARAPI GetTimestamp(
        /* [in] */ const String& parameterName,
        /* [in] */ ICalendar * cal,
        /* [out] */ ITimestamp ** timestamp);

    CARAPI GetTimestamp(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ ICalendar * cal,
        /* [out] */ ITimestamp ** timestamp);

    CARAPI GetURL(
        /* [in] */ const String& parameterName,
        /* [out] */ IURL ** url);

    CARAPI SetRowId(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IRowId * x);

    CARAPI SetRowId(
        /* [in] */ const String& parameterName,
        /* [in] */ IRowId * x);

    CARAPI SetNString(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ const String& value);

    CARAPI SetNString(
        /* [in] */ const String& parameterName,
        /* [in] */ const String& value);

    CARAPI SetNCharacterStream(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IReader * x,
        /* [in] */ Int64 len);

    CARAPI SetNCharacterStream(
        /* [in] */ const String& parameterName,
        /* [in] */ IReader * x,
        /* [in] */ Int64 len);

    CARAPI SetNClob(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ INClob * value);

    CARAPI SetNClob(
        /* [in] */ const String& parameterName,
        /* [in] */ INClob * value);

    CARAPI SetClob(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IReader * x,
        /* [in] */ Int64 len);

    CARAPI SetClob(
        /* [in] */ const String& parameterName,
        /* [in] */ IReader * x,
        /* [in] */ Int64 len);

    CARAPI SetBlob(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IInputStream * x,
        /* [in] */ Int64 len);

    CARAPI SetBlob(
        /* [in] */ const String& parameterName,
        /* [in] */ IInputStream * x,
        /* [in] */ Int64 len);

    CARAPI SetNClob(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IReader * x,
        /* [in] */ Int64 len);

    CARAPI SetNClob(
        /* [in] */ const String& parameterName,
        /* [in] */ IReader * x,
        /* [in] */ Int64 len);

    CARAPI SetSQLXML(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ ISQLXML * xml);

    CARAPI SetSQLXML(
        /* [in] */ const String& parameterName,
        /* [in] */ ISQLXML * xml);

    CARAPI SetAsciiStream(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IInputStream * x,
        /* [in] */ Int64 len);

    CARAPI SetAsciiStream(
        /* [in] */ const String& parameterName,
        /* [in] */ IInputStream * x,
        /* [in] */ Int64 len);

    CARAPI SetBinaryStream(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IInputStream * x,
        /* [in] */ Int64 len);

    CARAPI SetBinaryStream(
        /* [in] */ const String& parameterName,
        /* [in] */ IInputStream * x,
        /* [in] */ Int64 len);

    CARAPI SetCharacterStream(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IReader * x,
        /* [in] */ Int64 len);

    CARAPI SetCharacterStream(
        /* [in] */ const String& parameterName,
        /* [in] */ IReader * x,
        /* [in] */ Int64 len);

    CARAPI SetAsciiStream(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IInputStream * x);

    CARAPI SetAsciiStream(
        /* [in] */ const String& parameterName,
        /* [in] */ IInputStream * x);

    CARAPI SetBinaryStream(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IInputStream * x);

    CARAPI SetBinaryStream(
        /* [in] */ const String& parameterName,
        /* [in] */ IInputStream * x);

    CARAPI SetCharacterStream(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IReader * x);

    CARAPI SetCharacterStream(
        /* [in] */ const String& parameterName,
        /* [in] */ IReader * x);

    CARAPI SetNCharacterStream(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IReader * x);

    CARAPI SetNCharacterStream(
        /* [in] */ const String& parameterName,
        /* [in] */ IReader * x);

    CARAPI SetClob(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IReader * x);

    CARAPI SetClob(
        /* [in] */ const String& parameterName,
        /* [in] */ IReader * x);

    CARAPI SetBlob(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IInputStream * x);

    CARAPI SetBlob(
        /* [in] */ const String& parameterName,
        /* [in] */ IInputStream * x);

    CARAPI SetNClob(
        /* [in] */ Int32 parameterIndex,
        /* [in] */ IReader * x);

    CARAPI SetNClob(
        /* [in] */ const String& parameterName,
        /* [in] */ IReader * x);

    CARAPI_(void) Init(
        /* [in] */ IJDBCConnection * pConn,
        /* [in] */ const String& isql);

private:
    CARAPI_(String) Fixup(
        /* [in] */ const String& sql);

    CARAPI_(String) Fixup2(
        /* [in] */ const String& sql);

private:
    String sql;
    AutoPtr<ArrayOf<String> > mArgs;
    AutoPtr<ArrayOf<Boolean> > mBlobs;
    AutoPtr<List<AutoPtr<BatchArg> > > mBatch;
    const static Boolean mNullrepl;
};

} // namespace JDBC
} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_JDBC_CJDBCPREPAREDSTATEMENT_H__
