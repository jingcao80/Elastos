#ifndef __ELASTOS_DROID_DATABASE_CCURSORWRAPPER_H__
#define __ELASTOS_DROID_DATABASE_CCURSORWRAPPER_H__

#include "_Elastos_Droid_Database_CCursorWrapper.h"
#include "elastos/droid/database/CursorWrapper.h"

namespace Elastos {
namespace Droid {
namespace Database {

CarClass(CCursorWrapper) , public CursorWrapper
{
public:
    CAR_OBJECT_DECL()
};

} //Database
} //Droid
} //Elastos

#endif // __ELASTOS_DROID_DATABASE_CCURSORWRAPPER_H__
