#ifndef  __ELASTOS_DROID_PHONE_COUTGOINGCALLRECEIVER_H__
#define  __ELASTOS_DROID_PHONE_COUTGOINGCALLRECEIVER_H__

#include "elastos/droid/telephony/phone/COutgoingCallBroadcaster.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CarClass(COutgoingCallReceiver)
    , public COutgoingCallBroadcaster::OutgoingCallReceiver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_COUTGOINGCALLRECEIVER_H__