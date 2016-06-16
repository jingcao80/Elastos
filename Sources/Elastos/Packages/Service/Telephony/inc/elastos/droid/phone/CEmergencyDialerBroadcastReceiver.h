#ifndef  __ELASTOS_DROID_PHONE_CEMERGENCYDIALERBROADCASTRECEIVER_H__
#define  __ELASTOS_DROID_PHONE_CEMERGENCYDIALERBROADCASTRECEIVER_H__

#include "elastos/droid/telephony/phone/CEmergencyDialer.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CarClass(CEmergencyDialerBroadcastReceiver)
    , public CEmergencyDialer::MyBroadcastReceiver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CEMERGENCYDIALERBROADCASTRECEIVER_H__