#ifndef  __ELASTOS_DROID_PHONE_CEMERGENCYCALLBACKMODESERVICEBROADCASTRECEIVER_H__
#define  __ELASTOS_DROID_PHONE_CEMERGENCYCALLBACKMODESERVICEBROADCASTRECEIVER_H__

#include "_Elastos_Droid_Phone_CEmergencyCallbackModeServiceBroadcastReceiver.h"
#include "elastos/droid/phone/CEmergencyCallbackModeService.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Phone::CEmergencyCallbackModeService;

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