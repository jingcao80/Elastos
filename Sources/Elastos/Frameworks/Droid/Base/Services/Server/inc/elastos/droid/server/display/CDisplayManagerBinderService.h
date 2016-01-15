
#ifndef __ELASTOS_DROID_SERVER_DISPLAY_CDISPLAY_MANAGER_BINDER_SERVICE_H__
#define __ELASTOS_DROID_SERVER_DISPLAY_CDISPLAY_MANAGER_BINDER_SERVICE_H__

#include "_Elastos_Droid_Server_Display_CDisplayManagerBinderService.h"
#include "elastos/droid/server/display/CDisplayManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

CarClass(CDisplayManagerBinderService)
    , public CDisplayManagerService::BinderService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_SERVER_DISPLAY_CDISPLAY_MANAGER_BINDER_SERVICE_H__
