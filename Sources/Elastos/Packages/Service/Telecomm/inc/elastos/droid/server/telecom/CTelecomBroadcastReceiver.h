
#ifndef __ELASTOS_DROID_SERVER_TELECOM_CTELECOMBROADCASTRECEIVER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CTELECOMBROADCASTRECEIVER_H__

#include "_Elastos_Droid_Server_Telecom_CTelecomBroadcastReceiver.h"
#include "elastos/droid/server/telecom/TelecomBroadcastReceiver.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Handles miscellaneous Telecom broadcast intents. This should be visible from outside, but
 * should not be in the "exported" state.
 */
CarClass(CTelecomBroadcastReceiver)
    , public TelecomBroadcastReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CTELECOMBROADCASTRECEIVER_H__
