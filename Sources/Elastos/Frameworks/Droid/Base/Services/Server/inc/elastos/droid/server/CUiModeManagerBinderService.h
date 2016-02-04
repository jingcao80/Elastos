
#ifndef __ELASTOS_DROID_SERVER_CUIMODEMANAGER_BINDER_SERVICE_H__
#define __ELASTOS_DROID_SERVER_CUIMODEMANAGER_BINDER_SERVICE_H__

#include "_Elastos_Droid_Server_CUiModeManagerBinderService.h"
#include "elastos/droid/server/UiModeManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CUiModeManagerBinderService)
    , public UiModeManagerService::BinderService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CUIMODEMANAGER_BINDER_SERVICE_H__
