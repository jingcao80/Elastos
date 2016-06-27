#ifndef  __ELASTOS_DROID_PHONE_CEMERGENCYCALLBACKMODEEXITDIALOGBROADCASTRECEIVER_H__
#define  __ELASTOS_DROID_PHONE_CEMERGENCYCALLBACKMODEEXITDIALOGBROADCASTRECEIVER_H__

#include "_Elastos_Droid_TeleService_Phone_CEmergencyCallbackModeExitDialogBroadcastReceiver.h"
#include "elastos/droid/teleservice/phone/CEmergencyCallbackModeExitDialog.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::TeleService::Phone::CEmergencyCallbackModeExitDialog;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CarClass(CEmergencyCallbackModeExitDialogBroadcastReceiver)
    , public CEmergencyCallbackModeExitDialog::MyBroadcastReceiver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CEMERGENCYCALLBACKMODEEXITDIALOGBROADCASTRECEIVER_H__