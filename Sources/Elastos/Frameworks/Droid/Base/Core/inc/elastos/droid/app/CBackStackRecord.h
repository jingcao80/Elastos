
#ifndef __ELASTOS_DROID_APP_CBACKSTACKRECORD_H__
#define __ELASTOS_DROID_APP_CBACKSTACKRECORD_H__

#include "_Elastos_Droid_App_CBackStackRecord.h"
#include "elastos/droid/app/CBackStackState.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CBackStackRecord)
    , public BackStackRecord
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CBACKSTACKRECORD_H__