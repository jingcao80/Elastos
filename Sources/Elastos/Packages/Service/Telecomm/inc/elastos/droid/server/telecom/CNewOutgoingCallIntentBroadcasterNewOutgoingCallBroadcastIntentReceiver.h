
#ifndef __ELASTOS_DROID_SERVER_TELECOM_CNEWOUTGOINGCALLINTENTBROADCASTERNEWOUTGOINGCALLBROADCASTINTENTRECEIVER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CNEWOUTGOINGCALLINTENTBROADCASTERNEWOUTGOINGCALLBROADCASTINTENTRECEIVER_H__

#include "_Elastos_Droid_Server_Telecom_CNewOutgoingCallIntentBroadcasterNewOutgoingCallBroadcastIntentReceiver.h"
#include "elastos/droid/server/telecom/NewOutgoingCallIntentBroadcaster.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Processes the result of the outgoing call broadcast intent, and performs callbacks to
 * the OutgoingCallIntentBroadcasterListener as necessary.
 */
CarClass(CNewOutgoingCallIntentBroadcasterNewOutgoingCallBroadcastIntentReceiver)
    , public NewOutgoingCallIntentBroadcaster::NewOutgoingCallBroadcastIntentReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CNEWOUTGOINGCALLINTENTBROADCASTERNEWOUTGOINGCALLBROADCASTINTENTRECEIVER_H__
