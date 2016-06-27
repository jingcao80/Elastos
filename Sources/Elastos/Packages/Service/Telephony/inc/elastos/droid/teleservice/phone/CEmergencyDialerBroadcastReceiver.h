#ifndef  __ELASTOS_DROID_PHONE_CEMERGENCYDIALERBROADCASTRECEIVER_H__
#define  __ELASTOS_DROID_PHONE_CEMERGENCYDIALERBROADCASTRECEIVER_H__

#include "_Elastos_Droid_TeleService_Phone_CEmergencyDialerBroadcastReceiver.h"
#include "elastos/droid/teleservice/phone/CEmergencyDialer.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::TeleService::Phone::CEmergencyDialer;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CarClass(CEmergencyDialerBroadcastReceiver)
    , public CEmergencyDialer::MyBroadcastReceiver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CEMERGENCYDIALERBROADCASTRECEIVER_H__