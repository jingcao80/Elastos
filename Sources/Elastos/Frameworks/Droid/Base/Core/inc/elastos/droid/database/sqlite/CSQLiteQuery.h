#ifndef __ELASTOS_DROID_DATABASE_SQLITE_CSQLITEQUERY_H__
#define __ELASTOS_DROID_DATABASE_SQLITE_CSQLITEQUERY_H__

#include "_Elastos_Droid_Database_Sqlite_CSQLiteQuery.h"
#include "elastos/droid/database/sqlite/SQLiteProgram.h"

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

CarClass(CSQLiteQuery)
    , public SQLiteProgram
    , public ISQLiteQuery
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI FillWindow(
        /* [in] */ ICursorWindow* window,
        /* [in] */ Int32 startPos,
        /* [in] */ Int32 requiredPos,
        /* [in] */ Boolean countAllRows,
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* value);

    CARAPI constructor(
        /* [in] */ ISQLiteDatabase *db,
        /* [in] */ const String& query,
        /* [in] */ ICancellationSignal* cancellationSignal);

private:
    const static String TAG;

    AutoPtr<ICancellationSignal> mCancellationSignal;
};

} //Sqlite
} //Database
} //Droid
} //Elastos

#endif // __ELASTOS_DROID_DATABASE_SQLITE_CSQLITEQUERY_H__
