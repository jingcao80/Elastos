#ifndef __ELASTOS_DROID_DATABASE_SQLITE_CSQLITECURSOR_H__
#define __ELASTOS_DROID_DATABASE_SQLITE_CSQLITECURSOR_H__

#include "_Elastos_Droid_Database_Sqlite_CSQLiteCursor.h"
#include "elastos/droid/database/sqlite/SQLiteCursor.h"

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

CarClass(CSQLiteCursor) , public SQLiteCursor
{
public:
    CAR_OBJECT_DECL()
};

} //Sqlite
} //Database
} //Droid
} //Elastos

#endif // __ELASTOS_DROID_DATABASE_SQLITE_CSQLITECURSOR_H__
