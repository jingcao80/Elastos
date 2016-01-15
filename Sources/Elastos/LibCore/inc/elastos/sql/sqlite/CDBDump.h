
#ifndef __ELASTOS_SQL_SQLITE_CDBDUMP_H__
#define __ELASTOS_SQL_SQLITE_CDBDUMP_H__

#include "_Elastos_Sql_SQLite_CDBDump.h"
#include "CShell.h"

namespace Elastos {
namespace Sql {
namespace SQLite {

CarClass(CDBDump)
    , public Object
    , public IDBDump
    , public ICallback
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI Columns(
        /* [in] */ ArrayOf<String> * coldata);

    CARAPI Types(
        /* [in] */ ArrayOf<String> * types);

    CARAPI Newrow(
        /* [in] */ ArrayOf<String> * args,
        /* [out] */ Boolean * value);

    CARAPI constructor(
        /* [in] */ IShell * ms,
        /* [in] */ ArrayOf<String> * tables);

private:
    AutoPtr<CShell> mShell;
};

} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_CDBDUMP_H__
