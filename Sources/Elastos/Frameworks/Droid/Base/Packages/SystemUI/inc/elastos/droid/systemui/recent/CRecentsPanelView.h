
#ifndef __ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTS_PANELVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTS_PANELVIEW_H__

#include "_Elastos_Droid_SystemUI_Recent_CRecentsPanelView.h"
#include "elastos/droid/systemui/recent/RecentsPanelView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

CarClass(CRecentsPanelView)
    , public RecentsPanelView
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Recent
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTS_PANELVIEW_H__