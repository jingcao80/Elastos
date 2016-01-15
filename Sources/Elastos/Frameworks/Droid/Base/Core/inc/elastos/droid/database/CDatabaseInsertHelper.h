#ifndef __ELASTOS_DROID_DATABASE_CDATABASEINSERTHELPER_H__
#define __ELASTOS_DROID_DATABASE_CDATABASEINSERTHELPER_H__

#include "_Elastos_Droid_Database_CDatabaseInsertHelper.h"
#include "elastos/droid/database/DatabaseUtils.h"

namespace Elastos {
namespace Droid {
namespace Database {

CarClass(CDatabaseInsertHelper) , public DatabaseUtils::InsertHelper
{
public:
    CAR_OBJECT_DECL()
};

} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_CDATABASEINSERTHELPER_H__
