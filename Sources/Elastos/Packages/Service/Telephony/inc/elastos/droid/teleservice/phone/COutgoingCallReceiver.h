#ifndef  __ELASTOS_DROID_PHONE_COUTGOINGCALLRECEIVER_H__
#define  __ELASTOS_DROID_PHONE_COUTGOINGCALLRECEIVER_H__

#include "_Elastos_Droid_TeleService_Phone_COutgoingCallReceiver.h"
#include "elastos/droid/teleservice/phone/COutgoingCallBroadcaster.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::TeleService::Phone::COutgoingCallBroadcaster;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CarClass(COutgoingCallReceiver)
    , public COutgoingCallBroadcaster::OutgoingCallReceiver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_COUTGOINGCALLRECEIVER_H__