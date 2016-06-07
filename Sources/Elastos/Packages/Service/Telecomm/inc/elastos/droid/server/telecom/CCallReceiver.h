
#ifndef __ELASTOS_DROID_SERVER_TELECOM_CCALLRECEIVER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CCALLRECEIVER_H__

#include "_Elastos_Droid_Server_Telecom_CCallReceiver.h"
#include "elastos/droid/server/telecom/CallReceiver.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Single point of entry for all outgoing and incoming calls. {@link CallActivity} serves as a
 * trampoline activity that captures call intents for individual users and forwards it to
 * the {@link CallReceiver} which interacts with the rest of Telecom, both of which run only as
 * the primary user.
 */
CarClass(CCallReceiver)
    , public CallReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CCALLRECEIVER_H__
