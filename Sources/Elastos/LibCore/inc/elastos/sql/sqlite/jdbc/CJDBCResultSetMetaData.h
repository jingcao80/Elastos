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

#ifndef __ELASTOS_SQL_SQLITE_JDBC_CJDBCRESULTSETMETADATA_H__
#define __ELASTOS_SQL_SQLITE_JDBC_CJDBCRESULTSETMETADATA_H__

#include <elastos/core/Object.h>
#include "_Elastos_Sql_SQLite_JDBC_CJDBCResultSetMetaData.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Sql {
namespace SQLite {
namespace JDBC{

CarClass(CJDBCResultSetMetaData)
    , public Object
    , public IJDBCResultSetMetaData
    , public IResultSetMetaData
    , public IWrapper
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCatalogName(
        /* [in] */ Int32 column,
        /* [out] */ String * str);

    CARAPI GetColumnClassName(
        /* [in] */ Int32 column,
        /* [out] */ String * str);

    CARAPI GetColumnCount(
        /* [out] */ Int32 * value);

    CARAPI GetColumnDisplaySize(
        /* [in] */ Int32 column,
        /* [out] */ Int32 * value);

    CARAPI GetColumnLabel(
        /* [in] */ Int32 column,
        /* [out] */ String * str);

    CARAPI GetColumnName(
        /* [in] */ Int32 column,
        /* [out] */ String * str);

    CARAPI GetColumnType(
        /* [in] */ Int32 column,
        /* [out] */ Int32 * value);

    CARAPI GetColumnTypeName(
        /* [in] */ Int32 column,
        /* [out] */ String * str);

    CARAPI GetPrecision(
        /* [in] */ Int32 column,
        /* [out] */ Int32 * value);

    CARAPI GetScale(
        /* [in] */ Int32 column,
        /* [out] */ Int32 * value);

    CARAPI GetSchemaName(
        /* [in] */ Int32 column,
        /* [out] */ String * str);

    CARAPI GetTableName(
        /* [in] */ Int32 column,
        /* [out] */ String * str);

    CARAPI IsAutoIncrement(
        /* [in] */ Int32 column,
        /* [out] */ Boolean * value);

    CARAPI IsCaseSensitive(
        /* [in] */ Int32 column,
        /* [out] */ Boolean * value);

    CARAPI IsCurrency(
        /* [in] */ Int32 column,
        /* [out] */ Boolean * value);

    CARAPI IsDefinitelyWritable(
        /* [in] */ Int32 column,
        /* [out] */ Boolean * value);

    CARAPI IsNullable(
        /* [in] */ Int32 column,
        /* [out] */ Int32 * value);

    CARAPI IsReadOnly(
        /* [in] */ Int32 column,
        /* [out] */ Boolean * value);

    CARAPI IsSearchable(
        /* [in] */ Int32 column,
        /* [out] */ Boolean * value);

    CARAPI IsSigned(
        /* [in] */ Int32 column,
        /* [out] */ Boolean * value);

    CARAPI IsWritable(
        /* [in] */ Int32 column,
        /* [out] */ Boolean * value);
    CARAPI IsWrapperFor(
        /* [in] */ IInterface * iface,
        /* [out] */ Boolean * value);

    CARAPI constructor(
        /* [in] */ IJDBCResultSet * ir);

    CARAPI Unwrap(
        /* [in] */ PInterface iface,
        /* [out] */ IInterface ** oface);

    CARAPI FindColByName(
        /* [in] */ const String& columnName,
        /* [out] */ Int32 * value);

private:
    AutoPtr<IJDBCResultSet> r;
};

} // namespace JDBC
} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_JDBC_CJDBCRESULTSETMETADATA_H__
