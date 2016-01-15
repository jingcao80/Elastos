#ifndef __ELASTOS_DROID_DATABASE_CMERGECURSOR_H__
#define __ELASTOS_DROID_DATABASE_CMERGECURSOR_H__

#include "_Elastos_Droid_Database_CMergeCursor.h"
#include "elastos/droid/database/MergeCursor.h"

namespace Elastos {
namespace Droid {
namespace Database {

CarClass(CMergeCursor) , public MergeCursor
{
public:
    CAR_OBJECT_DECL()
};

} //Database
} //Droid
} //Elastos

#endif // __ELASTOS_DROID_DATABASE_CMERGECURSOR_H__
