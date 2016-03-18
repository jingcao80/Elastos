#ifndef __ELASTOS_DROID_SERVER_AM_CACTIVITYMANAGER_DISPATCH_USER_SWITCH_CALLBACK_H__
#define __ELASTOS_DROID_SERVER_AM_CACTIVITYMANAGER_DISPATCH_USER_SWITCH_CALLBACK_H__

#include "_Elastos_Droid_Server_Am_CActivityManagerDispatchUserSwitchCallback.h"
#include "elastos/droid/server/am/CActivityManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CarClass(CActivityManagerDispatchUserSwitchCallback)
    , public CActivityManagerService::DispatchUserSwitchCallback
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_AM_CACTIVITYMANAGER_DISPATCH_USER_SWITCH_CALLBACK_H__
