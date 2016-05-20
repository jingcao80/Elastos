
#ifndef __ELASTOS_SQL_SQLITE_JDBC_CJDBCSTATEMENT_H__
#define __ELASTOS_SQL_SQLITE_JDBC_CJDBCSTATEMENT_H__

#include "_Elastos_Sql_SQLite_JDBC_CJDBCStatement.h"
#include "JDBCStatement.h"

namespace Elastos {
namespace Sql {
namespace SQLite {
namespace JDBC{

CarClass(CJDBCStatement) , public JDBCStatement
{
public:
    CAR_OBJECT_DECL()
};

} // namespace JDBC
} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_JDBC_CJDBCSTATEMENT_H__
