
#ifndef __ELASTOS_DROID_APP_CLOCAL_ACTIVITY_MANAGER_H__
#define __ELASTOS_DROID_APP_CLOCAL_ACTIVITY_MANAGER_H__

#include "_Elastos_Droid_App_CLocalActivityManager.h"
#include "elastos/droid/app/LocalActivityManager.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CLocalActivityManager)
    , public LocalActivityManager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CLOCAL_ACTIVITY_MANAGER_H__