#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDWIDGETFRAME_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDWIDGETFRAME_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardWidgetFrame.h"
#include "elastos/droid/systemui/keyguard/KeyguardWidgetFrame.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CKeyguardWidgetFrame)
    , public KeyguardWidgetFrame
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDWIDGETFRAME_H__