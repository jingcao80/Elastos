#ifndef __ELASTOS_DROID_SERVER_AM_CACTIVITYMANAGER_PREBOOT_COMPLTETED_RECEIVER_H__
#define __ELASTOS_DROID_SERVER_AM_CACTIVITYMANAGER_PREBOOT_COMPLTETED_RECEIVER_H__

#include "_Elastos_Droid_Server_Am_CActivityManagerPreBootCompletedReceiver.h"
#include "elastos/droid/server/am/CActivityManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CarClass(CActivityManagerPreBootCompletedReceiver)
    , public CActivityManagerService::PreBootCompletedReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_AM_CACTIVITYMANAGER_PREBOOT_COMPLTETED_RECEIVER_H__
