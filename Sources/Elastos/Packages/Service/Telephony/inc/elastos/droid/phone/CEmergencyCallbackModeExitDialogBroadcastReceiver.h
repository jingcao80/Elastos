#ifndef  __ELASTOS_DROID_PHONE_CEMERGENCYCALLBACKMODEEXITDIALOGBROADCASTRECEIVER_H__
#define  __ELASTOS_DROID_PHONE_CEMERGENCYCALLBACKMODEEXITDIALOGBROADCASTRECEIVER_H__

#include "elastos/droid/telephony/phone/CEmergencyCallbackModeExitDialog.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CarClass(CCEmergencyCallbackModeExitDialogBroadcastReceiver)
    , public CEmergencyCallbackModeExitDialog::MyBroadcastReceiver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CEMERGENCYCALLBACKMODEEXITDIALOGBROADCASTRECEIVER_H__