#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSERVICEBINDER_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSERVICEBINDER_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardServiceBinder.h"
#include "elastos/droid/systemui/keyguard/KeyguardService.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CKeyguardServiceBinder)
    , public KeyguardService::KeyguardServiceBinder
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSERVICEBINDER_H__