
#ifndef  __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_CSTATUSBARICONVIEW_H__
#define  __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_CSTATUSBARICONVIEW_H__

#include "_Elastos_Droid_Packages_SystemUI_StatusBar_CStatusBarIconView.h"
#include "elastos/droid/packages/systemui/statusbar/StatusBarIconView.h"

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {
namespace StatusBar {

CarClass(CStatusBarIconView), public StatusBarIconView
{
public:
    CAR_OBJECT_DECL();
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Packages
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_CSTATUSBARICONVIEW_H__
