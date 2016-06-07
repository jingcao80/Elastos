
#ifndef __ELASTOS_DROID_SERVER_TELECOM_CTTYMANAGERTTYBROADCASTRECEIVER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CTTYMANAGERTTYBROADCASTRECEIVER_H__

#include "_Elastos_Droid_Server_Telecom_CTtyManagerTtyBroadcastReceiver.h"
#include "elastos/droid/server/telecom/TtyManager.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

CarClass(CTtyManagerTtyBroadcastReceiver)
    , public TtyManager::TtyBroadcastReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CTTYMANAGERTTYBROADCASTRECEIVER_H__
