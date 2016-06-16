#ifndef  __ELASTOS_DROID_PHONE_CEMERGENCYCALLBACKMODESERVICEBROADCASTRECEIVER_H__
#define  __ELASTOS_DROID_PHONE_CEMERGENCYCALLBACKMODESERVICEBROADCASTRECEIVER_H__

#include "elastos/droid/telephony/phone/CEmergencyCallbackModeService.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CarClass(CEmergencyCallbackModeServiceBroadcastReceiver)
    , public CEmergencyCallbackModeService::MyBroadcastReceiver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CEMERGENCYCALLBACKMODESERVICEBROADCASTRECEIVER_H__