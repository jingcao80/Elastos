#ifndef __ELASTOS_DROID_SERVER_POWER_CPOWERMANAGERSERVICEBINDERSERVICE_H__
#define __ELASTOS_DROID_SERVER_POWER_CPOWERMANAGERSERVICEBINDERSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_Power_CPowerManagerServiceBinderService.h"
#include "elastos/droid/server/power/PowerManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Power {

CarClass(CPowerManagerServiceBinderService)
    , public PowerManagerService::BinderService
{
public:
    CAR_OBJECT_DECL();
};

} // Power
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_POWER_CPOWERMANAGERSERVICEBINDERSERVICE_H__
