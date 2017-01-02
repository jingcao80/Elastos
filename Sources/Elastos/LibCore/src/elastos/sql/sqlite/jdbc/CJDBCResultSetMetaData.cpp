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

#include "CJDBCResultSetMetaData.h"
#include "CJDBCResultSet.h"
#include "CTableResultX.h"

namespace Elastos {
namespace Sql {
namespace SQLite {
namespace JDBC{

CAR_OBJECT_IMPL(CJDBCResultSetMetaData)

CAR_INTERFACE_IMPL_3(CJDBCResultSetMetaData, Object, IJDBCResultSetMetaData, IResultSetMetaData, IWrapper)

ECode CJDBCResultSetMetaData::GetCatalogName(
    /* [in] */ Int32 column,
    /* [out] */ String * str)
{
    *str = String(NULL);
    return NOERROR;
}

ECode CJDBCResultSetMetaData::GetColumnClassName(
    /* [in] */ Int32 column,
    /* [out] */ String * str)
{
    column--;
    if (r != NULL && ((CJDBCResultSet *)r.Get())->tr != NULL) {
        if (column < 0 || column >= ((CJDBCResultSet *)r.Get())->tr->mNcolumns) {
            *str = String(NULL);
            return NOERROR;
        }
        AutoPtr<ITableResultX > arr = (ITableResultX * )(((CJDBCResultSet *)r.Get())->tr)->Probe(EIID_ITableResultX);
        if (arr) {
            switch ((*((CTableResultX *) ((CJDBCResultSet *)r.Get())->tr.Get())->sql_type)[column]) {
                case ITypes::SMALLINT:
                    *str = String("java.lang.Short");
                case ITypes::INTEGER:
                    *str = String("java.lang.Integer");
                case ITypes::REAL:
                case ITypes::DOUBLE:
                    *str = String("java.lang.Double");
                case ITypes::FLOAT:
                    *str = String("java.lang.Float");
                case ITypes::BIGINT:
                    *str = String("java.lang.Long");
                case ITypes::DATE:
                    *str = String("java.sql.Date");
                case ITypes::TIME:
                    *str = String("java.sql.Time");
                case ITypes::TIMESTAMP:
                    *str = String("java.sql.Timestamp");
                case ITypes::BINARY:
                case ITypes::VARBINARY:
                    *str = String("[B");
                /* defaults to varchar below */
            }
        }else {
            *str = String("java.lang.String");
        }
    }else {
        *str = String(NULL);
    }
    return NOERROR;
}

ECode CJDBCResultSetMetaData::GetColumnCount(
    /* [out] */ Int32 * value)
{
    if (r != NULL && ((CJDBCResultSet *)r.Get())->tr != NULL) {
        *value = ((CJDBCResultSet *)r.Get())->tr->mNcolumns;
    }else {
        *value = 0;
    }
    return NOERROR;
}

ECode CJDBCResultSetMetaData::GetColumnDisplaySize(
    /* [in] */ Int32 column,
    /* [out] */ Int32 * value)
{
    *value = 0;
    return NOERROR;
}

ECode CJDBCResultSetMetaData::GetColumnLabel(
    /* [in] */ Int32 column,
    /* [out] */ String * str)
{
    column--;
    String c = String(NULL);
    if (r != NULL && ((CJDBCResultSet *)r.Get())->tr != NULL) {
        if (!(column < 0 || column >= ((CJDBCResultSet *)r.Get())->tr->mNcolumns)) {
            c = (*((CJDBCResultSet *)r.Get())->tr->mColumn)[column];
        }
    }
    *str = c;
    return NOERROR;
}

ECode CJDBCResultSetMetaData::GetColumnName(
    /* [in] */ Int32 column,
    /* [out] */ String * str)
{
    column--;
    String c = String(NULL);
    if (r != NULL && ((CJDBCResultSet *)r.Get())->tr != NULL) {
        if (!(column < 0 || column >= ((CJDBCResultSet *)r.Get())->tr->mNcolumns)) {
            c = (*((CJDBCResultSet *)r.Get())->tr->mColumn)[column];
            if (c != NULL) {
                Int32 i = c.IndexOf('.');
                if (i > 0) {
                    *str = c.Substring(i + 1);
                    return NOERROR;
                }
            }
        }
    }
    *str = c;
    return NOERROR;
}

ECode CJDBCResultSetMetaData::GetColumnType(
    /* [in] */ Int32 column,
    /* [out] */ Int32 * value)
{
    column--;
    if (r != NULL && ((CJDBCResultSet *)r.Get())->tr != NULL) {
        if (!(column < 0 || column >= ((CJDBCResultSet *)r.Get())->tr->mNcolumns)) {
            AutoPtr<ITableResultX > arr = (ITableResultX * )(((CJDBCResultSet *)r.Get())->tr)->Probe(EIID_ITableResultX);
            if (arr) {
                *value = (*((CTableResultX *) ((CJDBCResultSet *)r.Get())->tr.Get())->sql_type)[column];
            } else {
                *value = ITypes::VARCHAR;
            }
        }
    } else {
        return E_SQL_EXCEPTION;
    }
    return NOERROR;
}

ECode CJDBCResultSetMetaData::GetColumnTypeName(
    /* [in] */ Int32 column,
    /* [out] */ String * str)
{
    column--;
    if (r != NULL && ((CJDBCResultSet *)r.Get())->tr != NULL) {
        if (column < 0 || column < ((CJDBCResultSet *)r.Get())->tr->mNcolumns) {
        AutoPtr<ITableResultX > arr = (ITableResultX * )(((CJDBCResultSet *)r.Get())->tr)->Probe(EIID_ITableResultX);
        if (arr) {
            switch ((*((CTableResultX *) ((CJDBCResultSet *)r.Get())->tr.Get())->sql_type)[column]) {
                case ITypes::SMALLINT:
                    *str = String("smallint");
                case ITypes::INTEGER:
                    *str = String("integer");
                case ITypes::DOUBLE:
                    *str = String("double");
                case ITypes::FLOAT:
                    *str = String("float");
                case ITypes::BIGINT:
                    *str = String("bigint");
                case ITypes::DATE:
                    *str = String("date");
                case ITypes::TIME:
                    *str = String("time");
                case ITypes::TIMESTAMP:
                    *str = String("timestamp");
                case ITypes::BINARY:
                    *str = String("binary");
                case ITypes::VARBINARY:
                    *str = String("varbinary");
                case ITypes::REAL:
                    *str = String("real");
                /* defaults to varchar below */
                }
            } else {
                *str = String("varchar");
            }
        }
    } else {
        return E_SQL_EXCEPTION;
    }
    return NOERROR;
}

ECode CJDBCResultSetMetaData::GetPrecision(
    /* [in] */ Int32 column,
    /* [out] */ Int32 * value)
{
    *value = 0;
    return NOERROR;
}

ECode CJDBCResultSetMetaData::GetScale(
    /* [in] */ Int32 column,
    /* [out] */ Int32 * value)
{
    *value = 0;
    return NOERROR;
}

ECode CJDBCResultSetMetaData::GetSchemaName(
    /* [in] */ Int32 column,
    /* [out] */ String * str)
{
    *str = NULL;
    return NOERROR;
}

ECode CJDBCResultSetMetaData::GetTableName(
    /* [in] */ Int32 column,
    /* [out] */ String * str)
{
    column--;
    String c = String(NULL);
    if (r != NULL && ((CJDBCResultSet *)r.Get())->tr != NULL) {
        if (!(column < 0 || column >= ((CJDBCResultSet *)r.Get())->tr->mNcolumns)) {
            c = (*((CJDBCResultSet *)r.Get())->tr->mColumn)[column];
            if (c != NULL) {
                Int32 i = c.IndexOf('.');
                if (i > 0) {
                    *str = c.Substring(0,i);
                    return NOERROR;
                } else {
                    c = String(NULL);
                }
            }
        }
    }
    *str = c;
    return NOERROR;
}

ECode CJDBCResultSetMetaData::IsAutoIncrement(
    /* [in] */ Int32 column,
    /* [out] */ Boolean * value)
{
    *value = FALSE;
    return NOERROR;
}

ECode CJDBCResultSetMetaData::IsCaseSensitive(
    /* [in] */ Int32 column,
    /* [out] */ Boolean * value)
{
    *value = FALSE;
    return NOERROR;
}

ECode CJDBCResultSetMetaData::IsCurrency(
    /* [in] */ Int32 column,
    /* [out] */ Boolean * value)
{
    *value = FALSE;
    return NOERROR;
}

ECode CJDBCResultSetMetaData::IsDefinitelyWritable(
    /* [in] */ Int32 column,
    /* [out] */ Boolean * value)
{
    *value = TRUE;
    return NOERROR;
}

ECode CJDBCResultSetMetaData::IsNullable(
    /* [in] */ Int32 column,
    /* [out] */ Int32 * value)
{
    *value = IDatabaseMetaData::columnNullableUnknown;
    return NOERROR;
}

ECode CJDBCResultSetMetaData::IsReadOnly(
    /* [in] */ Int32 column,
    /* [out] */ Boolean * value)
{
    *value = FALSE;
    return NOERROR;
}

ECode CJDBCResultSetMetaData::IsSearchable(
    /* [in] */ Int32 column,
    /* [out] */ Boolean * value)
{
    *value = TRUE;
    return NOERROR;
}

ECode CJDBCResultSetMetaData::IsSigned(
    /* [in] */ Int32 column,
    /* [out] */ Boolean * value)
{
    *value = FALSE;
    return NOERROR;
}

ECode CJDBCResultSetMetaData::IsWritable(
    /* [in] */ Int32 column,
    /* [out] */ Boolean * value)
{
    *value = TRUE;
    return NOERROR;
}

ECode CJDBCResultSetMetaData::constructor(
    /* [in] */ IJDBCResultSet * ir)
{
    r = ir;
    return NOERROR;
}

ECode CJDBCResultSetMetaData::IsWrapperFor(
    /* [in] */ IInterface * iface,
    /* [out] */ Boolean * value)
{
    *value = FALSE;
    return NOERROR;
}

ECode CJDBCResultSetMetaData::Unwrap(
    /* [in] */ PInterface iface,
    /* [out] */ IInterface ** oface)
{
    return E_SQL_EXCEPTION;
}

ECode CJDBCResultSetMetaData::FindColByName(
    /* [in] */ const String& columnName,
    /* [out] */ Int32 * value)
{
    String c = String(NULL);
    if (r != NULL && ((CJDBCResultSet *)r.Get())->tr != NULL) {
        for (Int32 i = 0; i < ((CJDBCResultSet *)r.Get())->tr->mNcolumns; i++) {
            c = (*((CJDBCResultSet *)r.Get())->tr->mColumn)[i];
            if (c != NULL) {
                if (c.EqualsIgnoreCase(columnName) == 0) {
                    *value = i + 1;
                } else {
                    Int32 k = c.IndexOf('.');
                    if (k > 0) {
                        c = c.Substring(k + 1);
                        if (c.EqualsIgnoreCase(columnName) == 0) {
                            *value = i + 1;
                        }
                    }
                }
            }
        }
    } else {
        return E_SQL_EXCEPTION;
    }
    return NOERROR;
}

} // namespace JDBC
} // namespace SQLite
} // namespace Sql
} // namespace Elastos
