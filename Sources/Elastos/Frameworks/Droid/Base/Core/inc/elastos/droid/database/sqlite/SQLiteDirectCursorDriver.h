#ifndef __ELASTOS_DROID_DATABASE_SQLITE_SQLITEDIRECTCURSORDRIVER_H__
#define __ELASTOS_DROID_DATABASE_SQLITE_SQLITEDIRECTCURSORDRIVER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Database.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::ICancellationSignal;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {
/**
 * A cursor driver that uses the given query directly.
 *
 * @hide
 */
class SQLiteDirectCursorDriver
    : public Object
    , public ISQLiteCursorDriver
{
public:
    CAR_INTERFACE_DECL()

    SQLiteDirectCursorDriver(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& sql,
        /* [in] */ const String& editTable,
        /* [in] */ ICancellationSignal* cancellationSignal);

    CARAPI Query(
        /* [in] */ ISQLiteDatabaseCursorFactory* factory,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ ICursor** cursor);

    CARAPI CursorClosed();

    CARAPI SetBindArguments(
        /* [in] */ ArrayOf<String>* bindArgs);

    CARAPI CursorDeactivated();

    CARAPI CursorRequeried(
        /* [in] */ ICursor* cursor);

    CARAPI_(String) ToString();

private:
    AutoPtr<ISQLiteDatabase> mDatabase;
    String mEditTable;
    String mSql;
    AutoPtr<ICancellationSignal> mCancellationSignal;
    AutoPtr<ISQLiteQuery> mQuery;
};

} //Sqlite
} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_SQLITE_SQLITEDIRECTCURSORDRIVER_H__
