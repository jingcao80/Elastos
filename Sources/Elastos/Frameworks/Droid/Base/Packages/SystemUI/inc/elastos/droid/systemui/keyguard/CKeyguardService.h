#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSERVICE_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSERVICE_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardService.h"
#include "elastos/droid/systemui/keyguard/KeyguardService.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CKeyguardService)
    , public KeyguardService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDSERVICE_H__