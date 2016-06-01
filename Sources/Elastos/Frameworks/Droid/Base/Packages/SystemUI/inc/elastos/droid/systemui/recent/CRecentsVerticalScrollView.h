
#ifndef __ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTS_VERTICAL_SCROLLVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTS_VERTICAL_SCROLLVIEW_H__

#include "_Elastos_Droid_SystemUI_Recent_CRecentsVerticalScrollView.h"
#include "elastos/droid/systemui/recent/RecentsVerticalScrollView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

CarClass(CRecentsVerticalScrollView)
    , public RecentsVerticalScrollView
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Recent
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTS_VERTICAL_SCROLLVIEW_H__