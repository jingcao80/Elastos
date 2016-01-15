#ifndef __ELASTOS_DROID_DATABASE_CCROSSPROCESSCURSORWRAPPER_H__
#define __ELASTOS_DROID_DATABASE_CCROSSPROCESSCURSORWRAPPER_H__

#include "_Elastos_Droid_Database_CCrossProcessCursorWrapper.h"
#include "elastos/droid/database/CrossProcessCursorWrapper.h"

namespace Elastos {
namespace Droid {
namespace Database {

CarClass(CCrossProcessCursorWrapper) , public CrossProcessCursorWrapper
{
public:
    CAR_OBJECT_DECL()
};

} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_CCROSSPROCESSCURSORWRAPPER_H__
