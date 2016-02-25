
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CSIGNALCLUSTERVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CSIGNALCLUSTERVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CSignalClusterView.h"
#include "elastos/droid/systemui/statusbar/SignalClusterView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CarClass(CSignalClusterView), public SignalClusterView
{
public:
    CAR_OBJECT_DECL();
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CSIGNALCLUSTERVIEW_H__
