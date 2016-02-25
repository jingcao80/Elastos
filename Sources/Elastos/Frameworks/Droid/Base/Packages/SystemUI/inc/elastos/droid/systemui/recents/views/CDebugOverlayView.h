
#ifndef __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_CDEBUGOVERLAYVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_CDEBUGOVERLAYVIEW_H__

#include "_Elastos_Droid_SystemUI_Recents_Views_CDebugOverlayView.h"
#include "elastos/droid/systemui/recents/views/DebugOverlayView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

CarClass(CDebugOverlayView)
    , public DebugOverlayView
{
    CAR_OBJECT_DECL();
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_CDEBUGOVERLAYVIEW_H__
