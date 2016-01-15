
#include "elastos/droid/database/sqlite/CSQLiteQueryBuilderHelper.h"
#include "elastos/droid/database/sqlite/SQLiteQueryBuilder.h"

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

CAR_INTERFACE_IMPL(CSQLiteQueryBuilderHelper, Singleton, ISQLiteQueryBuilderHelper)

CAR_SINGLETON_IMPL(CSQLiteQueryBuilderHelper)

ECode CSQLiteQueryBuilderHelper::BuildQueryString(
    /* [in] */ Boolean distinct,
    /* [in] */ const String& tables,
    /* [in] */ ArrayOf<String>* columns,
    /* [in] */ const String& where,
    /* [in] */ const String& groupBy,
    /* [in] */ const String& having,
    /* [in] */ const String& orderBy,
    /* [in] */ const String& limit,
    /* [out] */ String* queryStr)
{
    VALIDATE_NOT_NULL(queryStr)
    return SQLiteQueryBuilder::BuildQueryString(distinct, tables, columns, where, groupBy,
        having, orderBy, limit, queryStr);
}

ECode CSQLiteQueryBuilderHelper::AppendColumns(
    /* [in] */ IStringBuilder* s,
    /* [in] */ const ArrayOf<String>& columns)
{
    return SQLiteQueryBuilder::AppendColumns(s, columns);
}


} //Sqlite
} //Database
} //Droid
} //Elastos