#ifndef __ELASTOS_DROID_SERVER_AM_CACTIVITYMANAGER_SYSTEM_READY_RECEIVER_H__
#define __ELASTOS_DROID_SERVER_AM_CACTIVITYMANAGER_SYSTEM_READY_RECEIVER_H__

#include "_Elastos_Droid_Server_Am_CActivityManagerSystemReadyReceiver.h"
#include "elastos/droid/server/am/CActivityManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CarClass(CActivityManagerSystemReadyReceiver)
    , public CActivityManagerService::SystemReadyReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_AM_CACTIVITYMANAGER_SYSTEM_READY_RECEIVER_H__
