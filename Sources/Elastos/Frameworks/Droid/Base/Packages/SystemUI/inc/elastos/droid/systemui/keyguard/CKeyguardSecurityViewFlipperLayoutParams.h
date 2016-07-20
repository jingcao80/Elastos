
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARD_SECURITY_VIEWFLIPPER_LAYOUT_PARAMS_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARD_SECURITY_VIEWFLIPPER_LAYOUT_PARAMS_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardSecurityViewFlipperLayoutParams.h"
#include <elastos/droid/systemui/keyguard/CKeyguardSecurityViewFlipper.h>

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CKeyguardSecurityViewFlipperLayoutParams)
    , public CKeyguardSecurityViewFlipper::LayoutParams
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARD_SECURITY_VIEWFLIPPER_LAYOUT_PARAMS_H__
