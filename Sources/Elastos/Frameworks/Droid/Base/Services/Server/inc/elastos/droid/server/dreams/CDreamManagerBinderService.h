
#ifndef __ELASTOS_DROID_SERVER_DREAMS_CDREAM_MANAGER_BINDER_SERVICE_H__
#define __ELASTOS_DROID_SERVER_DREAMS_CDREAM_MANAGER_BINDER_SERVICE_H__

#include "_Elastos_Droid_Server_Dreams_CDreamManagerBinderService.h"
#include "elastos/droid/server/dreams/CDreamManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Dreams {

CarClass(CDreamManagerBinderService)
    , public CDreamManagerService::BinderService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Dreams
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_SERVER_DREAMS_CDREAM_MANAGER_BINDER_SERVICE_H__
