
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDWIDGITPAPER_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDWIDGITPAPER_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardWidgetPager.h"
#include "elastos/droid/systemui/keyguard/KeyguardWidgetPager.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CKeyguardWidgetPager)
    , public KeyguardWidgetPager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CKEYGUARDWIDGITPAPER_H__
