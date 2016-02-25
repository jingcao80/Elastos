
#ifndef __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_CTASKVIEWFOOTER_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_CTASKVIEWFOOTER_H__

#include "_Elastos_Droid_SystemUI_Recents_Views_CTaskViewFooter.h"
#include "elastos/droid/systemui/recents/views/TaskViewFooter.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

CarClass(CTaskViewFooter)
    , public TaskViewFooter
{
    CAR_OBJECT_DECL();
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_CTASKVIEWFOOTER_H__
