#ifndef __ELASTOS_DROID_SERVER_CBATTERY_BINDER_SERVICE_H__
#define __ELASTOS_DROID_SERVER_CBATTERY_BINDER_SERVICE_H__

#include "_Elastos_Droid_Server_CBatteryBinderService.h"
#include "elastos/droid/server/CBatteryService.h"

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CBatteryBinderService)
    , public CBatteryService::BinderService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CBATTERY_BINDER_SERVICE_H__
