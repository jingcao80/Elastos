#ifndef __ELASTOS_DROID_DATABASE_SQLITE_CSQLITEQUERYBUILDER_H__
#define __ELASTOS_DROID_DATABASE_SQLITE_CSQLITEQUERYBUILDER_H__

#include "_Elastos_Droid_Database_Sqlite_CSQLiteQueryBuilder.h"
#include "elastos/droid/database/sqlite/SQLiteQueryBuilder.h"

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

CarClass(CSQLiteQueryBuilder) , public SQLiteQueryBuilder
{
public:
    CAR_OBJECT_DECL()
};

} //Sqlite
} //Database
} //Droid
} //Elastos

#endif // __ELASTOS_DROID_DATABASE_SQLITE_CSQLITEQUERYBUILDER_H__
