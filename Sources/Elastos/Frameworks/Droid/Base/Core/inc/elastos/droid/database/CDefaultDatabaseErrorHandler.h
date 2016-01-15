#ifndef __ELASTOS_DROID_DATABASE_CDEFAULTDATABASEERRORHANDLER_H__
#define __ELASTOS_DROID_DATABASE_CDEFAULTDATABASEERRORHANDLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Database_CDefaultDatabaseErrorHandler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;

namespace Elastos {
namespace Droid {
namespace Database {
/**
 * Default class used to define the actions to take when the database corruption is reported
 * by sqlite.
 * <p>
 * An application can specify an implementation of {@link DatabaseErrorHandler} on the
 * following:
 * <ul>
 *   <li>{@link SQLiteDatabase#openOrCreateDatabase(String,
 *      android.database.sqlite.SQLiteDatabase.CursorFactory, DatabaseErrorHandler)}</li>
 *   <li>{@link SQLiteDatabase#openDatabase(String,
 *      android.database.sqlite.SQLiteDatabase.CursorFactory, int, DatabaseErrorHandler)}</li>
 * </ul>
 * The specified {@link DatabaseErrorHandler} is used to handle database corruption errors, if they
 * occur.
 * <p>
 * If null is specified for DatabaeErrorHandler param in the above calls, then this class is used
 * as the default {@link DatabaseErrorHandler}.
 */
CarClass(CDefaultDatabaseErrorHandler)
    , public Object
    , public IDatabaseErrorHandler
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * defines the default method to be invoked when database corruption is detected.
     * @param dbObj the {@link SQLiteDatabase} object representing the database on which corruption
     * is detected.
     */
    CARAPI OnCorruption(
        /* [in] */ ISQLiteDatabase* dbObj);

private:
    CARAPI_(void) DeleteDatabaseFile(
        /* [in] */ const String& fileName);

private:
    static const String TAG;
};

} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_CDEFAULTDATABASEERRORHANDLER_H__
