
#ifndef __ELASTOS_DROID_DATABASE_SQLITE_SQLITECUSTOMFUNCTION_H__
#define __ELASTOS_DROID_DATABASE_SQLITE_SQLITECUSTOMFUNCTION_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/database/sqlite/SQLiteDatabase.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

/**
 * Describes a custom SQL function.
 *
 * @hide
 */
class SQLiteCustomFunction : public Object
{
public:
    /**
     * Create custom function.
     *
     * @param name The name of the sqlite3 function.
     * @param numArgs The number of arguments for the function, or -1 to
     * support any number of arguments.
     * @param callback The callback to invoke when the function is executed.
     */
    SQLiteCustomFunction(
        /* [in] */ const String& name,
        /* [in] */ Int32 numArgs,
        /* [in] */ ICustomFunction* callBack);

    // Called from native.
    //@SuppressWarnings("unused")
    CARAPI_(void) DispatchCallback(
        /* [in] */ ArrayOf<String>* args);

public:
    String mName;
    Int32 mNumArgs;
    AutoPtr<ICustomFunction> mCallback;
};

} //Sqlite
} //Database
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_DATABASE_SQLITE_SQLITECUSTOMFUNCTION_H__
