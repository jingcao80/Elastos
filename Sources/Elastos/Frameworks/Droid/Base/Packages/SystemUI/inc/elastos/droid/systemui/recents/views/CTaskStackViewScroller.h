
#ifndef __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_CTASKSTACK_VIEWSCROLLER_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_CTASKSTACK_VIEWSCROLLER_H__

#include "_Elastos_Droid_SystemUI_Recents_Views_CTaskStackViewScroller.h"
#include "elastos/droid/systemui/recents/views/TaskStackViewScroller.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

CarClass(CTaskStackViewScroller)
    , public TaskStackViewScroller
{
    CAR_OBJECT_DECL()
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_CTASKSTACK_VIEWSCROLLER_H__
