
#ifndef __ELASTOS_SQL_SQLITE_JDBC_CDATABASEX_H__
#define __ELASTOS_SQL_SQLITE_JDBC_CDATABASEX_H__

#include "_Elastos_Sql_SQLite_JDBC_CDatabaseX.h"
#include "Database.h"

using Elastos::Sql::SQLite::Database;

namespace Elastos {
namespace Sql {
namespace SQLite {
namespace JDBC{

CarClass(CDatabaseX)
    , public Database
{
public:
    CAR_OBJECT_DECL()

    CDatabaseX();

    CARAPI Exec(
        /* [in] */ const String& sql,
        /* [in] */ ICallback * cb);

    CARAPI Exec(
        /* [in] */ const String& sql,
        /* [in] */ ICallback * cb,
        /* [in] */ ArrayOf<String> * args);

    CARAPI GetTable(
        /* [in] */ const String& sql,
        /* [in] */ ArrayOf<String> * args,
        /* [out] */ ITableResult ** tbl);

    CARAPI GetTable(
        /* [in] */ const String& sql,
        /* [in] */ ArrayOf<String> * args,
        /* [in] */ ITableResult * tbl);

protected:
    CARAPI_(void) Wait(Int32 ms);

private:
    static Object mLock;
};

} // namespace JDBC
} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_JDBC_CDATABASEX_H__
