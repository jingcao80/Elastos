
#ifndef __ELASTOS_SQL_SQLITE_JDBC_CJDBCRESULTSET_H__
#define __ELASTOS_SQL_SQLITE_JDBC_CJDBCRESULTSET_H__

#include <elastos/core/Object.h>
#include "_Elastos_Sql_SQLite_JDBC_CJDBCResultSet.h"
#include "CTableResult.h"

using Elastos::IO::IInputStream;
using Elastos::IO::IReader;
using Elastos::Math::IBigDecimal;
using Elastos::Net::IURL;
using Elastos::Utility::ICalendar;
using Elastos::Utility::IMap;
using Elastos::Sql::ISQLWarning;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger16;
using Elastos::Core::IInteger64;
using Elastos::Core::IDouble;
using Elastos::Core::IFloat;

namespace Elastos {
namespace Sql {
namespace SQLite {
namespace JDBC{

CarClass(CJDBCResultSet)
    , public Object
    , public IJDBCResultSet
    , public IResultSet
    , public IWrapper
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI Absolute(
        /* [in] */ Int32 row,
        /* [out] */ Boolean * value);

    CARAPI AfterLast();

    CARAPI BeforeFirst();

    CARAPI CancelRowUpdates();

    CARAPI ClearWarnings();

    CARAPI Close();

    CARAPI DeleteRow();

    CARAPI FindColumn(
        /* [in] */ const String& colName);

    CARAPI GetFirst(
        /* [out] */ Boolean *value);

    CARAPI GetAsciiStream(
        /* [in] */ Int32 colIndex,
        /* [out] */ IInputStream ** iinputstream);

    CARAPI GetAsciiStream(
        /* [in] */ const String& colName,
        /* [out] */ IInputStream ** iinputstream);

    CARAPI GetBigDecimal(
        /* [in] */ Int32 colIndex,
        /* [out] */ IBigDecimal ** bigdecimal);

    CARAPI GetBigDecimal(
        /* [in] */ Int32 colIndex,
        /* [in] */ Int32 scale,
        /* [out] */ IBigDecimal ** bigdecimal);

    CARAPI GetBigDecimal(
        /* [in] */ const String& colName,
        /* [out] */ IBigDecimal ** bigdecimal);

    CARAPI GetBigDecimal(
        /* [in] */ const String& colName,
        /* [in] */ Int32 scale,
        /* [out] */ IBigDecimal ** bigdecimal);

    CARAPI GetBinaryStream(
        /* [in] */ Int32 colIndex,
        /* [out] */ IInputStream ** iinputstream);

    CARAPI GetBinaryStream(
        /* [in] */ const String& colName,
        /* [out] */ IInputStream ** iinputstream);

    CARAPI GetBlob(
        /* [in] */ Int32 colIndex,
        /* [out] */ Elastos::Sql::IBlob ** oblob);

    CARAPI GetBlob(
        /* [in] */ const String& colName,
        /* [out] */ Elastos::Sql::IBlob ** oblob);

    CARAPI GetBoolean(
        /* [in] */ Int32 colIndex,
        /* [out] */ Boolean * value);

    CARAPI GetBoolean(
        /* [in] */ const String& colName,
        /* [out] */ Boolean * value);

    CARAPI GetByte(
        /* [in] */ Int32 colIndex,
        /* [out] */ Byte * value);

    CARAPI GetByte(
        /* [in] */ const String& colName,
        /* [out] */ Byte * value);

    CARAPI GetBytes(
        /* [in] */ Int32 colIndex,
        /* [out, callee] */ ArrayOf<Byte> ** oarray);

    CARAPI GetBytes(
        /* [in] */ const String& colName,
        /* [out, callee] */ ArrayOf<Byte> ** oarray);

    CARAPI GetCharacterStream(
        /* [in] */ Int32 colIndex,
        /* [out] */ IReader ** reader);

    CARAPI GetCharacterStream(
        /* [in] */ const String& colName,
        /* [out] */ IReader ** reader);

    CARAPI GetClob(
        /* [in] */ Int32 colIndex,
        /* [out] */ IClob ** oclob);

    CARAPI GetClob(
        /* [in] */ const String& colName,
        /* [out] */ IClob ** oclob);

    CARAPI GetConcurrency(
        /* [out] */ Int32 * mode);

    CARAPI GetCursorName(
        /* [out] */ String * name);

    CARAPI GetDouble(
        /* [in] */ Int32 colIndex,
        /* [out] */ Double * value);

    CARAPI GetDouble(
        /* [in] */ const String& colName,
        /* [out] */ Double * value);

    CARAPI GetFetchDirection(
        /* [out] */ Int32 * value);

    CARAPI GetFetchSize(
        /* [out] */ Int32 * size);

    CARAPI GetFloat(
        /* [in] */ Int32 colIndex,
        /* [out] */ Float * value);

    CARAPI GetFloat(
        /* [in] */ const String& colName,
        /* [out] */ Float * value);

    CARAPI GetInt32(
        /* [in] */ Int32 colIndex,
        /* [out] */ Int32 * value);

    CARAPI GetInt32(
        /* [in] */ const String& colName,
        /* [out] */ Int32 * value);

    CARAPI GetInt64(
        /* [in] */ Int32 colIndex,
        /* [out] */ Int64 * value);

    CARAPI GetInt64(
        /* [in] */ const String& colName,
        /* [out] */ Int64 * value);

    CARAPI GetMetaData(
        /* [out] */ IResultSetMetaData ** metaData);

    CARAPI GetObject(
        /* [in] */ Int32 colIndex,
        /* [out] */ IInterface ** obj);

    CARAPI GetRef(
        /* [in] */ Int32 colIndex,
        /* [out] */ IRef ** ref);

    CARAPI GetRef(
        /* [in] */ const String& colName,
        /* [out] */ IRef ** ref);

    CARAPI GetRow(
        /* [out] */ Int32 * value);

    CARAPI GetInt16(
        /* [in] */ Int32 colIndex,
        /* [out] */ Int16 * value);

    CARAPI GetInt16(
        /* [in] */ const String& colName,
        /* [out] */ Int16 * value);

    CARAPI GetStatement(
        /* [out] */ IStatement ** ppStatement);

    CARAPI GetString(
        /* [in] */ Int32 colIndex,
        /* [out] */ String * value);

    CARAPI GetString(
        /* [in] */ const String& colName,
        /* [out] */ String * value);

    CARAPI GetTime(
        /* [in] */ Int32 colIndex,
        /* [out] */ ITime ** sqltime);

    CARAPI GetTime(
        /* [in] */ Int32 colIndex,
        /* [in] */ ICalendar * pCal,
        /* [out] */ ITime ** sqltime);

    CARAPI GetTime(
        /* [in] */ const String& colName,
        /* [out] */ ITime ** sqltime);

    CARAPI GetTime(
        /* [in] */ const String& colName,
        /* [in] */ ICalendar * pCal,
        /* [out] */ ITime ** sqltime);

    CARAPI GetTimestamp(
        /* [in] */ Int32 colIndex,
        /* [out] */ ITimestamp ** timemestamp);

    CARAPI GetTimestamp(
        /* [in] */ Int32 colIndex,
        /* [in] */ ICalendar * pCal,
        /* [out] */ ITimestamp ** timemestamp);

    CARAPI GetTimestamp(
        /* [in] */ const String& colName,
        /* [out] */ ITimestamp ** timemestamp);

    CARAPI GetTimestamp(
        /* [in] */ const String& colName,
        /* [in] */ ICalendar * pCal,
        /* [out] */ ITimestamp ** timemestamp);

    CARAPI GetType(
        /* [out] */ Int32 * value);

    CARAPI GetUnicodeStream(
        /* [in] */ Int32 colIndex,
        /* [out] */ IInputStream ** iinputstream);

    CARAPI GetUnicodeStream(
        /* [in] */ const String& colName,
        /* [out] */ IInputStream ** iinputstream);

    CARAPI GetWarnings(
        /* [out] */ ISQLWarning ** sqlwarning);

    CARAPI InsertRow();

    CARAPI IsAfterLast(
        /* [out] */ Boolean * value);

    CARAPI IsBeforeFirst(
        /* [out] */ Boolean * value);

    CARAPI IsFirst(
        /* [out] */ Boolean * value);

    CARAPI IsLast(
        /* [out] */ Boolean * value);

    CARAPI GetLast(
        /* [out] */ Boolean * value);

    CARAPI MoveToCurrentRow();

    CARAPI MoveToInsertRow();

    CARAPI GetNext(
        /* [out] */ Boolean * value);

    CARAPI GetPrevious(
        /* [out] */ Boolean * value);

    CARAPI RefreshRow();

    CARAPI Relative(
        /* [in] */ Int32 rows,
        /* [out] */ Boolean * value);

    CARAPI RowDeleted(
        /* [out] */ Boolean * value);

    CARAPI RowInserted(
        /* [out] */ Boolean * value);

    CARAPI RowUpdated(
        /* [out] */ Boolean * value);

    CARAPI SetFetchDirection(
        /* [in] */ Int32 direction);

    CARAPI SetFetchSize(
        /* [in] */ Int32 rows);

    CARAPI UpdateArray(
        /* [in] */ Int32 colIndex,
        /* [in] */ IArray * x);

    CARAPI UpdateArray(
        /* [in] */ const String& colName,
        /* [in] */ IArray * x);

    CARAPI UpdateAsciiStream(
        /* [in] */ Int32 colIndex,
        /* [in] */ IInputStream * x,
        /* [in] */ Int32 length);

    CARAPI UpdateAsciiStream(
        /* [in] */ const String& colName,
        /* [in] */ IInputStream * x,
        /* [in] */ Int32 length);

    CARAPI UpdateBigDecimal(
        /* [in] */ Int32 colIndex,
        /* [in] */ IBigDecimal * x);

    CARAPI UpdateBigDecimal(
        /* [in] */ const String& colName,
        /* [in] */ IBigDecimal * x);

    CARAPI UpdateBinaryStream(
        /* [in] */ Int32 colIndex,
        /* [in] */ IInputStream * x,
        /* [in] */ Int32 length);

    CARAPI UpdateBinaryStream(
        /* [in] */ const String& colName,
        /* [in] */ IInputStream * x,
        /* [in] */ Int32 length);

    CARAPI UpdateBlob(
        /* [in] */ Int32 colIndex,
        /* [in] */ Elastos::Sql::IBlob * x);

    CARAPI UpdateBlob(
        /* [in] */ const String& colName,
        /* [in] */ Elastos::Sql::IBlob * x);

    CARAPI UpdateBoolean(
        /* [in] */ Int32 colIndex,
        /* [in] */ Boolean x);

    CARAPI UpdateBoolean(
        /* [in] */ const String& colName,
        /* [in] */ Boolean x);

    CARAPI UpdateByte(
        /* [in] */ Int32 colIndex,
        /* [in] */ Byte x);

    CARAPI UpdateByte(
        /* [in] */ const String& colName,
        /* [in] */ Byte x);

    CARAPI UpdateBytes(
        /* [in] */ Int32 colIndex,
        /* [in] */ const ArrayOf<Byte> & x);

    CARAPI UpdateBytes(
        /* [in] */ const String& colName,
        /* [in] */ const ArrayOf<Byte> & x);

    CARAPI UpdateCharacterStream(
        /* [in] */ Int32 colIndex,
        /* [in] */ IReader * x,
        /* [in] */ Int32 length);

    CARAPI UpdateCharacterStream(
        /* [in] */ const String& colName,
        /* [in] */ IReader * reader,
        /* [in] */ Int32 length);

    CARAPI UpdateClob(
        /* [in] */ Int32 colIndex,
        /* [in] */ IClob * x);

    CARAPI UpdateClob(
        /* [in] */ const String& colName,
        /* [in] */ IClob * x);

    CARAPI UpdateDate(
        /* [in] */ Int32 colIndex,
        /* [in] */ ISQLDate * x);

    CARAPI UpdateDate(
        /* [in] */ const String& colName,
        /* [in] */ ISQLDate * x);

    CARAPI UpdateDouble(
        /* [in] */ Int32 colIndex,
        /* [in] */ Double x);

    CARAPI UpdateDouble(
        /* [in] */ const String& colName,
        /* [in] */ Double x);

    CARAPI UpdateFloat(
        /* [in] */ Int32 colIndex,
        /* [in] */ Float x);

    CARAPI UpdateFloat(
        /* [in] */ const String& colName,
        /* [in] */ Float x);

    CARAPI UpdateInt(
        /* [in] */ Int32 colIndex,
        /* [in] */ Int32 x);

    CARAPI UpdateInt(
        /* [in] */ const String& colName,
        /* [in] */ Int32 x);

    CARAPI UpdateLong(
        /* [in] */ Int32 colIndex,
        /* [in] */ Int64 x);

    CARAPI UpdateLong(
        /* [in] */ const String& colName,
        /* [in] */ Int64 x);

    CARAPI UpdateNull(
        /* [in] */ Int32 colIndex);

    CARAPI UpdateNull(
        /* [in] */ const String& colName);

    CARAPI UpdateObject(
        /* [in] */ Int32 colIndex,
        /* [in] */ IInterface * x);

    CARAPI UpdateObject(
        /* [in] */ Int32 colIndex,
        /* [in] */ IInterface * x,
        /* [in] */ Int32 scale);

    CARAPI UpdateObject(
        /* [in] */ const String& colName,
        /* [in] */ IInterface * x);

    CARAPI UpdateObject(
        /* [in] */ const String& colName,
        /* [in] */ IInterface * x,
        /* [in] */ Int32 scale);

    CARAPI UpdateRef(
        /* [in] */ Int32 colIndex,
        /* [in] */ IRef * x);

    CARAPI UpdateRef(
        /* [in] */ const String& colName,
        /* [in] */ IRef * x);

    CARAPI UpdateRow();

    CARAPI UpdateShort(
        /* [in] */ Int32 colIndex,
        /* [in] */ Int16 x);

    CARAPI UpdateShort(
        /* [in] */ const String& colName,
        /* [in] */ Int16 x);

    CARAPI UpdateString(
        /* [in] */ Int32 colIndex,
        /* [in] */ const String& x);

    CARAPI UpdateString(
        /* [in] */ const String& colName,
        /* [in] */ const String& x);

    CARAPI UpdateTime(
        /* [in] */ Int32 colIndex,
        /* [in] */ ITime * x);

    CARAPI UpdateTime(
        /* [in] */ const String& colName,
        /* [in] */ ITime * x);

    CARAPI UpdateTimestamp(
        /* [in] */ Int32 colIndex,
        /* [in] */ ITimestamp * x);

    CARAPI UpdateTimestamp(
        /* [in] */ const String& colName,
        /* [in] */ ITimestamp * x);

    CARAPI WasNull(
        /* [out] */ Boolean * pIsNull);

    CARAPI GetRowId(
        /* [in] */ Int32 colIndex,
        /* [out] */ IRowId ** rowid);

    CARAPI GetRowId(
        /* [in] */ const String& colName,
        /* [out] */ IRowId ** rowid);

    CARAPI UpdateRowId(
        /* [in] */ Int32 colIndex,
        /* [in] */ IRowId * value);

    CARAPI UpdateRowId(
        /* [in] */ const String& colName,
        /* [in] */ IRowId * value);

    CARAPI GetHoldability(
        /* [out] */ Int32 * value);

    CARAPI IsClosed(
        /* [out] */ Boolean * value);

    CARAPI UpdateNString(
        /* [in] */ Int32 colIndex,
        /* [in] */ const String& nString);

    CARAPI UpdateNString(
        /* [in] */ const String& colName,
        /* [in] */ const String& nString);

    CARAPI UpdateNClob(
        /* [in] */ Int32 colIndex,
        /* [in] */ INClob * nclob);

    CARAPI UpdateNClob(
        /* [in] */ const String& colName,
        /* [in] */ INClob * nclob);

    CARAPI GetNClob(
        /* [in] */ Int32 colIndex,
        /* [out] */ INClob ** nclob);

    CARAPI GetNClob(
        /* [in] */ const String& colName,
        /* [out] */ INClob ** nclob);

    CARAPI GetSQLXML(
        /* [in] */ Int32 colIndex,
        /* [out] */ ISQLXML ** sqlxml);

    CARAPI GetSQLXML(
        /* [in] */ const String& colName,
        /* [out] */ ISQLXML ** sqlxml);

    CARAPI UpdateSQLXML(
        /* [in] */ Int32 colIndex,
        /* [in] */ ISQLXML * xmlObject);

    CARAPI UpdateSQLXML(
        /* [in] */ const String& colName,
        /* [in] */ ISQLXML * xmlObject);

    CARAPI GetNString(
        /* [in] */ Int32 colIndex,
        /* [out] */ String * nstr);

    CARAPI GetNString(
        /* [in] */ const String& colName,
        /* [out] */ String * nstr);

    CARAPI GetNCharacterStream(
        /* [in] */ Int32 colIndex,
        /* [out] */ IReader ** reader);

    CARAPI GetNCharacterStream(
        /* [in] */ const String& colName,
        /* [out] */ IReader ** reader);

    CARAPI UpdateNCharacterStream(
        /* [in] */ Int32 colIndex,
        /* [in] */ IReader * x,
        /* [in] */ Int64 length);

    CARAPI UpdateNCharacterStream(
        /* [in] */ const String& colName,
        /* [in] */ IReader * reader,
        /* [in] */ Int64 length);

    CARAPI UpdateAsciiStream(
        /* [in] */ Int32 colIndex,
        /* [in] */ IInputStream * x,
        /* [in] */ Int64 length);

    CARAPI UpdateBinaryStream(
        /* [in] */ Int32 colIndex,
        /* [in] */ IInputStream * x,
        /* [in] */ Int64 length);

    CARAPI UpdateCharacterStream(
        /* [in] */ Int32 colIndex,
        /* [in] */ IReader * x,
        /* [in] */ Int64 length);

    CARAPI UpdateAsciiStream(
        /* [in] */ const String& colName,
        /* [in] */ IInputStream * x,
        /* [in] */ Int64 length);

    CARAPI UpdateBinaryStream(
        /* [in] */ const String& colName,
        /* [in] */ IInputStream * x,
        /* [in] */ Int64 length);

    CARAPI UpdateCharacterStream(
        /* [in] */ const String& colName,
        /* [in] */ IReader * reader,
        /* [in] */ Int64 length);

    CARAPI UpdateBlob(
        /* [in] */ Int32 colIndex,
        /* [in] */ IInputStream * pInputStream,
        /* [in] */ Int64 length);

    CARAPI UpdateBlob(
        /* [in] */ const String& colName,
        /* [in] */ IInputStream * pInputStream,
        /* [in] */ Int64 length);

    CARAPI UpdateClob(
        /* [in] */ Int32 colIndex,
        /* [in] */ IReader * reader,
        /* [in] */ Int64 length);

    CARAPI UpdateClob(
        /* [in] */ const String& colName,
        /* [in] */ IReader * reader,
        /* [in] */ Int64 length);

    CARAPI UpdateNClob(
        /* [in] */ Int64 colIndex,
        /* [in] */ IReader * reader,
        /* [in] */ Int64 length);

    CARAPI UpdateNClob(
        /* [in] */ const String& colName,
        /* [in] */ IReader * reader,
        /* [in] */ Int64 length);

    CARAPI UpdateNCharacterStream(
        /* [in] */ Int32 colIndex,
        /* [in] */ IReader * x);

    CARAPI UpdateNCharacterStream(
        /* [in] */ const String& colName,
        /* [in] */ IReader * reader);

    CARAPI UpdateAsciiStream(
        /* [in] */ Int32 colIndex,
        /* [in] */ IInputStream * x);

    CARAPI UpdateBinaryStream(
        /* [in] */ Int32 colIndex,
        /* [in] */ IInputStream * x);

    CARAPI UpdateCharacterStream(
        /* [in] */ Int32 colIndex,
        /* [in] */ IReader * px);

    CARAPI UpdateAsciiStream(
        /* [in] */ const String& colName,
        /* [in] */ IInputStream * x);

    CARAPI UpdateBinaryStream(
        /* [in] */ const String& colName,
        /* [in] */ IInputStream * x);

    CARAPI UpdateCharacterStream(
        /* [in] */ const String& colName,
        /* [in] */ IReader * reader);

    CARAPI UpdateBlob(
        /* [in] */ Int32 colIndex,
        /* [in] */ IInputStream * pInputStream);

    CARAPI UpdateBlob(
        /* [in] */ const String& colName,
        /* [in] */ IInputStream * pInputStream);

    CARAPI UpdateClob(
        /* [in] */ Int32 colIndex,
        /* [in] */ IReader * reader);

    CARAPI UpdateClob(
        /* [in] */ const String& colName,
        /* [in] */ IReader * reader);

    CARAPI UpdateNClob(
        /* [in] */ Int32 colIndex,
        /* [in] */ IReader * reader);

    CARAPI UpdateNClob(
        /* [in] */ const String& colName,
        /* [in] */ IReader * reader);

    CARAPI constructor(
        /* [in] */ ITableResult * pTr,
        /* [in] */ IJDBCStatement * pS);

    CARAPI IsWrapperFor(
        /* [in] */ IInterface * iface,
        /* [out] */ Boolean * value);

    CARAPI GetArray(
        /* [in] */ Int32 colIndex,
        /* [out] */ IArray ** iarray);

    CARAPI GetArray(
        /* [in] */ const String& colName,
        /* [out] */ IArray ** iarray);

    CARAPI GetDate(
        /* [in] */ Int32 colIndex,
        /* [out] */ ISQLDate ** idate);

    CARAPI GetDate(
        /* [in] */ Int32 colIndex,
        /* [in] */ ICalendar * cal,
        /* [out] */ ISQLDate ** idate);

    CARAPI GetDate(
        /* [in] */ const String& colName,
        /* [out] */ ISQLDate ** idate);

    CARAPI GetDate(
        /* [in] */ const String& colName,
        /* [in] */ ICalendar * cal,
        /* [out] */ ISQLDate ** idate);

    CARAPI GetObject(
        /* [in] */ Int32 colIndex,
        /* [in] */ IMap * map,
        /* [out] */ IInterface ** obj);

    CARAPI GetObject(
        /* [in] */ const String& colName,
        /* [out] */ IInterface** obj);

    CARAPI GetObject(
        /* [in] */ const String& colName,
        /* [in] */ IMap * map,
        /* [out] */ IInterface** obj);

    CARAPI GetURL(
        /* [in] */ Int32 colIndex,
        /* [out] */ IURL** url);

    CARAPI GetURL(
        /* [in] */ const String& colName,
        /* [out] */ IURL** url);

    CARAPI Unwrap(
        /* [in] */ PInterface iface,
        /* [out] */ IInterface ** oface);

    CARAPI IsUpdatable(
        /* [out] */ Boolean * value);

    CARAPI FillRowbuf();

    CARAPI FindColumn(
        /* [in] */ const String& colName,
        /* [out] */ Int32 * value);

private:
    CARAPI_(AutoPtr<IInteger32>) InternalGetInt(
        /* [in] */ Int32 colIndex);

    CARAPI_(AutoPtr<IInteger16>) InternalGetShort(
        /* [in] */ Int32 colIndex);

    CARAPI_(AutoPtr<ITime>) InternalGetTime(
        /* [in] */ Int32 colIndex,
        /* [in] */ ICalendar * cal);

    CARAPI_(AutoPtr<ITimestamp>) InternalGetTimestamp(
        /* [in] */ Int32 colIndex,
        /* [in] */ ICalendar * cal);

    CARAPI_(AutoPtr<ISQLDate>) InternalGetDate(
        /* [in] */ Int32 colIndex,
        /* [in] */ ICalendar * cal);

    CARAPI_(AutoPtr<IDouble>) InternalGetDouble(
        /* [in] */ Int32 colIndex);

    CARAPI_(AutoPtr<IFloat>) InternalGetFloat(
        /* [in] */ Int32 colIndex);

    CARAPI_(AutoPtr<IInteger64>) InternalGetLong(
        /* [in] */ Int32 colIndex);

public:
    AutoPtr<CTableResult> tr;

private:
    Int32 mRow;
    AutoPtr<IJDBCStatement> s;
    AutoPtr<IJDBCResultSetMetaData> md;
    String lastg;
    Int32 updatable;
    String uptable;
    AutoPtr<ArrayOf<String> > pkcols;
    AutoPtr<ArrayOf<Int32> > pkcoli;
    const static Int32 UPD_UNKNOWN = -1;
    const static Int32 UPD_NO = 0;
    const static Int32 UPD_INS = 1;
    const static Int32 UPD_INSUPDDEL = 2;
    Boolean oninsrow;
    AutoPtr<ArrayOf<String> > rowbuf;
    const static Boolean nullrepl;
};

} // namespace JDBC
} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_JDBC_CJDBCRESULTSET_H__
