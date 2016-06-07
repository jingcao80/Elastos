
#ifndef __ELASTOS_DROID_SERVER_TELECOM_CWIREDHEADSETMANAGERWIREDHEADSETBROADCASTRECEIVER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CWIREDHEADSETMANAGERWIREDHEADSETBROADCASTRECEIVER_H__

#include "_Elastos_Droid_Server_Telecom_CWiredHeadsetManagerWiredHeadsetBroadcastReceiver.h"
#include "elastos/droid/server/telecom/WiredHeadsetManager.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/** Receiver for wired headset plugged and unplugged events. */
CarClass(CWiredHeadsetManagerWiredHeadsetBroadcastReceiver)
    , public WiredHeadsetManager::WiredHeadsetBroadcastReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CWIREDHEADSETMANAGERWIREDHEADSETBROADCASTRECEIVER_H__
