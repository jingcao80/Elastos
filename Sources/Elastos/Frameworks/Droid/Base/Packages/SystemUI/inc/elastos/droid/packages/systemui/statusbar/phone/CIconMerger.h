#ifndef  __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_PHONE_CICONMERGER_H__
#define  __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_PHONE_CICONMERGER_H__

#include "_Elastos_Droid_Packages_SystemUI_StatusBar_Phone_CIconMerger.h"
#include "elastos/droid/packages/systemui/statusbar/phone/IconMerger.h"

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CarClass(CIconMerger), public IconMerger
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Packages
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_PHONE_CICONMERGER_H__
