#ifndef __ELASTOS_DROID_DATABASE_SQLITE_SQLITECLOSABLE_H__
#define __ELASTOS_DROID_DATABASE_SQLITE_SQLITECLOSABLE_H__

#include "elastos/droid/ext/frameworkdef.h"
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Database.h"
#include <elastos/core/Object.h>

using Elastos::IO::ICloseable;
using Elastos::IO::EIID_ICloseable;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

/**
 * An object created from a SQLiteDatabase that can be closed.
 *
 * This class implements a primitive reference counting scheme for database objects.
 */
class SQLiteClosable
    : public Object
    , public ISQLiteClosable
    , public ICloseable
{
public:
    CAR_INTERFACE_DECL()

    SQLiteClosable();

    virtual CARAPI AcquireReference();

    virtual CARAPI ReleaseReference();

    virtual CARAPI ReleaseReferenceFromContainer();

    virtual CARAPI Close();

protected:
    virtual CARAPI_(void) OnAllReferencesReleased() = 0;

    virtual CARAPI_(void) OnAllReferencesReleasedFromContainer();

private:
    Int32 mReferenceCount;
};

} //Sqlite
} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_SQLITE_SQLITECLOSABLE_H__
