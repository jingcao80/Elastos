
#ifndef __ELASTOS_SQL_SQLITE_CTABLERESULT_H__
#define __ELASTOS_SQL_SQLITE_CTABLERESULT_H__

#include "_Elastos_Sql_SQLite_CTableResult.h"
#include "TableResult.h"
#include <elstring.h>

namespace Elastos {
namespace Sql {
namespace SQLite {

CarClass(CTableResult) , public TableResult
{
public:
    CAR_OBJECT_DECL();
};

} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_CTABLERESULT_H__
