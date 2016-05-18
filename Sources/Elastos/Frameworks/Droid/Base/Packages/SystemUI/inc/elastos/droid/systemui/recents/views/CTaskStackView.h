
#ifndef __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_CTASKSTACKVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_CTASKSTACKVIEW_H__

#include "_Elastos_Droid_SystemUI_Recents_Views_CTaskStackView.h"
#include "elastos/droid/systemui/recents/views/TaskStackView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

CarClass(CTaskStackView)
    , public TaskStackView
{
    CAR_OBJECT_DECL()
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_CTASKSTACKVIEW_H__
