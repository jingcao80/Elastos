
#ifndef __ELASTOS_DROID_DATABASE_SQLITE_CSQLITEDATABASEHELPER_H__
#define __ELASTOS_DROID_DATABASE_SQLITE_CSQLITEDATABASEHELPER_H__

#include "_Elastos_Droid_Database_Sqlite_CSQLiteDatabaseHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

CarClass(CSQLiteDatabaseHelper)
    , public Singleton
    , public ISQLiteDatabaseHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ReleaseMemory(
        /* [out] */ Int32* result);

    CARAPI OpenDatabase(
        /* [in] */ const String& path,
        /* [in] */ ISQLiteDatabaseCursorFactory* factory,
        /* [in] */ Int32 flags,
        /* [out] */ ISQLiteDatabase** result);

    CARAPI OpenDatabase(
        /* [in] */ const String& path,
        /* [in] */ ISQLiteDatabaseCursorFactory* factory,
        /* [in] */ Int32 flags,
        /* [in] */ IDatabaseErrorHandler* errorHandler,
        /* [out] */ ISQLiteDatabase** result);

    CARAPI OpenOrCreateDatabase(
        /* [in] */ IFile* file,
        /* [in] */ ISQLiteDatabaseCursorFactory* factory,
        /* [out] */ ISQLiteDatabase** result);

    CARAPI OpenOrCreateDatabase(
        /* [in] */ const String& path,
        /* [in] */ ISQLiteDatabaseCursorFactory* factory,
        /* [out] */ ISQLiteDatabase** result);

    CARAPI OpenOrCreateDatabase(
        /* [in] */ const String& path,
        /* [in] */ ISQLiteDatabaseCursorFactory* factory,
        /* [in] */ IDatabaseErrorHandler* errorHandler,
        /* [out] */ ISQLiteDatabase** result);

    CARAPI DeleteDatabase(
        /* [in] */ IFile* file,
        /* [out] */ Boolean* result);

    CARAPI Create(
        /* [in] */ ISQLiteDatabaseCursorFactory* factory,
        /* [out] */ ISQLiteDatabase** result);

    CARAPI FindEditTable(
        /* [in] */ const String& tables,
        /* [out] */ String* str);
};

} //Sqlite
} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_SQLITE_CSQLITEDATABASEHELPER_H__
