
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDUPDATEMONITORBROADCASTRECEIVER1_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDUPDATEMONITORBROADCASTRECEIVER1_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardUpdateMonitorBroadcastReceiver1.h"
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitor.h"

using Elastos::Droid::SystemUI::Keyguard::KeyguardUpdateMonitor;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CKeyguardUpdateMonitorBroadcastReceiver1)
    , public KeyguardUpdateMonitor::MyBroadcastReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDUPDATEMONITORBROADCASTRECEIVER1_H__
