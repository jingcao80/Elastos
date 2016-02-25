
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CSTATUSBARICONVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CSTATUSBARICONVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CStatusBarIconView.h"
#include "elastos/droid/systemui/statusbar/StatusBarIconView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CarClass(CStatusBarIconView), public StatusBarIconView
{
public:
    CAR_OBJECT_DECL();
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CSTATUSBARICONVIEW_H__
