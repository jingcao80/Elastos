#ifndef __ELASTOS_DROID_SERVER_CALRAMMANAGER_BINDER_SERVICE_H__
#define __ELASTOS_DROID_SERVER_CALRAMMANAGER_BINDER_SERVICE_H__

#include "_Elastos_Droid_Server_CAlarmManagerBinderService.h"
#include "elastos/droid/server/AlarmManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CAlarmManagerBinderService)
    , public AlarmManagerService::BinderService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CALRAMMANAGER_BINDER_SERVICE_H__
