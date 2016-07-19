
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDUPDATEMONITORBROADCASTRECEIVER2_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDUPDATEMONITORBROADCASTRECEIVER2_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardUpdateMonitorBroadcastReceiver2.h"
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitor.h"

using Elastos::Droid::SystemUI::Keyguard::KeyguardUpdateMonitor;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CKeyguardUpdateMonitorBroadcastReceiver2)
    , public KeyguardUpdateMonitor::MyBroadcastReceiver2
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDUPDATEMONITORBROADCASTRECEIVER2_H__
