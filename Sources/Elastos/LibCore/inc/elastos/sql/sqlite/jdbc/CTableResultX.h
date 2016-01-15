
#ifndef __ELASTOS_SQL_SQLITE_JDBC_CTABLERESULTX_H__
#define __ELASTOS_SQL_SQLITE_JDBC_CTABLERESULTX_H__

#include "_Elastos_Sql_SQLite_JDBC_CTableResultX.h"
#include "TableResult.h"

namespace Elastos {
namespace Sql {
namespace SQLite {
namespace JDBC{

CarClass(CTableResultX)
    , public TableResult
    , public ITableResultX
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 maxrows);

    CARAPI constructor(
        /* [in] */ Elastos::Sql::SQLite::ITableResult* tr);

    CARAPI_(void) Sql_types(
        /* [in] */ ArrayOf<Int32>* types);

public:
    AutoPtr<ArrayOf<Int32> > sql_type;
};

} // namespace JDBC
} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_JDBC_CTABLERESULTX_H__
