#ifndef __ELASTOS_DROID_SERVER_AM_CACTIVITYMANAGER_SWITCH_USER_RECEIVER_H__
#define __ELASTOS_DROID_SERVER_AM_CACTIVITYMANAGER_SWITCH_USER_RECEIVER_H__

#include "_Elastos_Droid_Server_Am_CActivityManagerSwitchUserReceiver.h"
#include "elastos/droid/server/am/CActivityManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CarClass(CActivityManagerSwitchUserReceiver)
    , public CActivityManagerService::SwitchUserReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_AM_CACTIVITYMANAGER_SWITCH_USER_RECEIVER_H__
