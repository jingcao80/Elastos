
#ifndef __ELASTOS_DROID_SERVER_CUIMODEMANAGER_SERVICE_H__
#define __ELASTOS_DROID_SERVER_CUIMODEMANAGER_SERVICE_H__

#include "_Elastos_Droid_Server_CUiModeManagerService.h"
#include "elastos/droid/server/UiModeManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CUiModeManagerService)
    , public UiModeManagerService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CUIMODEMANAGER_SERVICE_H__
