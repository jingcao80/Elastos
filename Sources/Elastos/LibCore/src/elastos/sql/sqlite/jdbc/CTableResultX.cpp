
#include "CTableResultX.h"
#include "CTableResult.h"
#include "CJDBCDatabaseMetaData.h"

namespace Elastos {
namespace Sql {
namespace SQLite {
namespace JDBC{

CAR_OBJECT_IMPL(CTableResultX)
CAR_INTERFACE_IMPL(CTableResultX, TableResult, ITableResultX);

ECode CTableResultX::constructor()
{
    FAIL_RETURN(TableResult::constructor())
    sql_type = ArrayOf<Int32>::Alloc(mNcolumns);
    for (Int32 i = 0; i < mNcolumns; i++) {
        (*sql_type)[i] = ITypes::VARCHAR;
    }
    return NOERROR;
}

ECode CTableResultX::constructor(
    /* [in] */ Int32 maxrows)
{
    FAIL_RETURN(TableResult::constructor(maxrows))
    sql_type = ArrayOf<Int32>::Alloc(mNcolumns);
    for (Int32 i = 0; i < mNcolumns; i++) {
        (*sql_type)[i] = ITypes::VARCHAR;
    }
    return NOERROR;
}

ECode CTableResultX::constructor(
    /* [in] */ Elastos::Sql::SQLite::ITableResult* tr)
{
    CTableResult* ctr = (CTableResult*)tr;
    mColumn = ctr->mColumn;
    mRows = ctr->mRows;
    mNcolumns = ctr->mNcolumns;
    mNrows = ctr->mNrows;
    mTypes = ctr->mTypes;
    mMaxRows = ctr->mMaxRows;
    sql_type = ArrayOf<Int32>::Alloc(mNcolumns);
    for (Int32 i = 0; i < mNcolumns; i++) {
        (*sql_type)[i] = ITypes::VARCHAR;
    }
    if (ctr->mTypes != NULL) {
        for (Int32 i = 0; i < ctr->mTypes->GetLength(); i++) {
            (*sql_type)[i] = CJDBCDatabaseMetaData::MapSqlType((*ctr->mTypes)[i]);
        }
    }
    return NOERROR;
}

void CTableResultX::Sql_types(
    /* [in] */ ArrayOf<Int32>* types)
{
    sql_type = types;
}

} // namespace JDBC
} // namespace SQLite
} // namespace Sql
} // namespace Elastos
