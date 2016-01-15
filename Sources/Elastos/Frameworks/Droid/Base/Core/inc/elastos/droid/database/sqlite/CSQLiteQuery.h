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

    CARAPI GetUniqueId(
        /* [out] */ Int32 *value);

    CARAPI BindNull(
        /* [in] */ Int32 index);

    CARAPI BindInt64(
        /* [in] */ Int32 index,
        /* [in] */ Int64 value);

    CARAPI BindDouble(
        /* [in] */ Int32 index,
        /* [in] */ Double value);

    CARAPI BindString(
        /* [in] */ Int32 index,
        /* [in] */ const String& value);

    CARAPI BindBlob(
        /* [in] */ Int32 index,
        /* [in] */ const ArrayOf<Byte>& value);

    CARAPI ClearBindings();

    CARAPI BindAllArgsAsStrings(
        /* [in] */ ArrayOf<String>* bindArgs);

    CARAPI AcquireReference();

    CARAPI ReleaseReference();

    CARAPI ReleaseReferenceFromContainer();

    CARAPI Close();

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
