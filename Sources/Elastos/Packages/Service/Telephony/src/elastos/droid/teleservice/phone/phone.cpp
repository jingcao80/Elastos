
#include "elastos/droid/teleservice/phone/CADNList.h"
#include "elastos/droid/teleservice/phone/COutgoingCallReceiver.h"
#include "elastos/droid/teleservice/phone/CEmergencyCallbackModeServiceBroadcastReceiver.h"
#include "elastos/droid/teleservice/phone/CEmergencyCallbackModeServiceLocalBinder.h"
#include "elastos/droid/teleservice/phone/CEmergencyCallbackModeExitDialogBroadcastReceiver.h"
#include "elastos/droid/teleservice/phone/CHfaLogicBroadcastReceiver.h"
#include "elastos/droid/teleservice/phone/CEmergencyDialerBroadcastReceiver.h"
#include "elastos/droid/teleservice/phone/CNetworkQueryServiceLocalBinder.h"
#include "elastos/droid/teleservice/phone/CNetworkQueryServiceNetworkQueryService.h"
#include "elastos/droid/teleservice/phone/CNetworkSettingNetworkQueryServiceCallback.h"
#include "elastos/droid/teleservice/phone/CPhoneGlobalsPhoneAppBroadcastReceiver.h"
#include "elastos/droid/teleservice/phone/CPhoneGlobalsNotificationBroadcastReceiver.h"
#include "elastos/droid/teleservice/phone/CPhoneGlobalsNotificationBroadcastReceiver.h"
#include "elastos/droid/teleservice/phone/CIccProvider.h"

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CAR_OBJECT_IMPL(CADNList)
CAR_OBJECT_IMPL(COutgoingCallReceiver)
CAR_OBJECT_IMPL(CEmergencyCallbackModeServiceBroadcastReceiver)
CAR_OBJECT_IMPL(CEmergencyCallbackModeServiceLocalBinder)
CAR_OBJECT_IMPL(CEmergencyCallbackModeExitDialogBroadcastReceiver)
CAR_OBJECT_IMPL(CHfaLogicBroadcastReceiver)
CAR_OBJECT_IMPL(CEmergencyDialerBroadcastReceiver)
CAR_OBJECT_IMPL(CNetworkQueryServiceLocalBinder)
CAR_OBJECT_IMPL(CNetworkQueryServiceNetworkQueryService)
CAR_OBJECT_IMPL(CNetworkSettingNetworkQueryServiceCallback)
CAR_OBJECT_IMPL(CPhoneGlobalsPhoneAppBroadcastReceiver)
CAR_OBJECT_IMPL(CPhoneGlobalsNotificationBroadcastReceiver)
CAR_OBJECT_IMPL(CIccProvider)

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos
