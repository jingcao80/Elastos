
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDUPDATEMONITORUSERSWITCHOBSERVER_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDUPDATEMONITORUSERSWITCHOBSERVER_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardUpdateMonitorUserSwitchObserver.h"
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitor.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

class KeyguardUpdateMonitor;

CarClass(CKeyguardUpdateMonitorUserSwitchObserver)
    , public KeyguardUpdateMonitor::UserSwitchObserver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDUPDATEMONITORUSERSWITCHOBSERVER_H__
