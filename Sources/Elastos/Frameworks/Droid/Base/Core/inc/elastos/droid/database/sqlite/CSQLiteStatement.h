#ifndef __ELASTOS_DROID_DATABASE_SQLITE_CSQLITESTATEMENT_H__
#define __ELASTOS_DROID_DATABASE_SQLITE_CSQLITESTATEMENT_H__

#include "_Elastos_Droid_Database_Sqlite_CSQLiteStatement.h"
#include "elastos/droid/database/sqlite/SQLiteProgram.h"

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

CarClass(CSQLiteStatement)
    , public SQLiteProgram
    , public ISQLiteStatement
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI Execute();

    CARAPI ExecuteUpdateDelete(
        /* [out] */ Int32* result);

    CARAPI ExecuteInsert(
        /* [out] */ Int64* value);

    CARAPI SimpleQueryForInt64(
        /* [out] */ Int64* value);

    CARAPI SimpleQueryForString(
        /* [out] */ String* value);

    CARAPI SimpleQueryForBlobFileDescriptor(
        /* [out] */ IParcelFileDescriptor** result);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI constructor(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& sql,
        /* [in] */ ArrayOf<IInterface*>* bindArgs);
};

} //Sqlite
} //Database
} //Droid
} //Elastos

#endif // __ELASTOS_DROID_DATABASE_SQLITE_CSQLITESTATEMENT_H__
