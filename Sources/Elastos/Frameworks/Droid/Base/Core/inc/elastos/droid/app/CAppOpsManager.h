
#ifndef __ELASTOS_DROID_APP_CAPP_OPS_MANAGER_H__
#define __ELASTOS_DROID_APP_CAPP_OPS_MANAGER_H__

#include "_Elastos_Droid_App_CAppOpsManager.h"
#include "elastos/droid/app/AppOpsManager.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CAppOpsManager)
    , public AppOpsManager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CAPP_OPS_MANAGER_H__