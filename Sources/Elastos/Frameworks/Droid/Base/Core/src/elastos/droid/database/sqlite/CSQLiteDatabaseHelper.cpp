
#include "elastos/droid/database/sqlite/CSQLiteDatabaseHelper.h"
#include "elastos/droid/database/sqlite/SQLiteDatabase.h"

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

CAR_INTERFACE_IMPL(CSQLiteDatabaseHelper, Singleton, ISQLiteDatabaseHelper)

CAR_SINGLETON_IMPL(CSQLiteDatabaseHelper)

ECode CSQLiteDatabaseHelper::ReleaseMemory(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = SQLiteDatabase::ReleaseMemory();
    return NOERROR;
}

ECode CSQLiteDatabaseHelper::OpenDatabase(
    /* [in] */ const String& path,
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [in] */ Int32 flags,
    /* [out] */ ISQLiteDatabase** result)
{
    VALIDATE_NOT_NULL(result)
    return SQLiteDatabase::OpenDatabase(path, factory, flags, result);
}

ECode CSQLiteDatabaseHelper::OpenDatabase(
    /* [in] */ const String& path,
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [in] */ Int32 flags,
    /* [in] */ IDatabaseErrorHandler* errorHandler,
    /* [out] */ ISQLiteDatabase** result)
{
    VALIDATE_NOT_NULL(result)
    return SQLiteDatabase::OpenDatabase(path, factory, flags, errorHandler, result);
}

ECode CSQLiteDatabaseHelper::OpenOrCreateDatabase(
    /* [in] */ IFile* file,
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [out] */ ISQLiteDatabase** result)
{
    VALIDATE_NOT_NULL(result)
    return SQLiteDatabase::OpenOrCreateDatabase(file, factory, result);
}

ECode CSQLiteDatabaseHelper::OpenOrCreateDatabase(
    /* [in] */ const String& path,
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [out] */ ISQLiteDatabase** result)
{
    VALIDATE_NOT_NULL(result)
    return SQLiteDatabase::OpenOrCreateDatabase(path, factory, result);
}

ECode CSQLiteDatabaseHelper::OpenOrCreateDatabase(
    /* [in] */ const String& path,
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [in] */ IDatabaseErrorHandler* errorHandler,
    /* [out] */ ISQLiteDatabase** result)
{
    VALIDATE_NOT_NULL(result)
    return SQLiteDatabase::OpenOrCreateDatabase(path, factory, errorHandler, result);
}

ECode CSQLiteDatabaseHelper::DeleteDatabase(
    /* [in] */ IFile* file,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return SQLiteDatabase::DeleteDatabase(file, result);
}

ECode CSQLiteDatabaseHelper::Create(
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [out] */ ISQLiteDatabase** result)
{
    VALIDATE_NOT_NULL(result)
    return SQLiteDatabase::Create(factory, result);
}

ECode CSQLiteDatabaseHelper::FindEditTable(
    /* [in] */ const String& tables,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return SQLiteDatabase::FindEditTable(tables, str);
}

} //Sqlite
} //Database
} //Droid
} //Elastos