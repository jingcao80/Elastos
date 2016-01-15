
#ifndef __ELASTOS_SQL_SQLITE_ELASTOS_SQL_SQLLITE_CBACKUP_H__
#define __ELASTOS_SQL_SQLITE_ELASTOS_SQL_SQLLITE_CBACKUP_H__

#include "_Elastos_Sql_SQLite_CBackup.h"
#include "Object.h"

namespace Elastos {
namespace Sql {
namespace SQLite {

CarClass(CBackup)
    , public Object
    , public IBackup
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI Finish();

    CARAPI Finalize();

    CARAPI NativeFinalize();

    CARAPI Step(
        /* [in] */ Int32 n,
        /* [out] */ Boolean* isCompleted);

    CARAPI Backup();

    CARAPI Remaining(
        /* [out] */ Int32* number);

    CARAPI Pagecount(
        /* [out] */ Int32* number);

    CBackup();

    virtual ~CBackup();

private:
    CARAPI NativeStep(
        /* [in] */ Int32 n,
        /* [out] */ Boolean* state);

    CARAPI_(Int32) NativeRemaining();

    CARAPI_(Int32) NativePagecount();

protected:
    Int64 mHandle;
};

} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_ELASTOS_SQL_SQLLITE_CBACKUP_H__
