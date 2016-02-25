
#ifndef __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_CTASKVIEWTHUMBNAIL_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_CTASKVIEWTHUMBNAIL_H__

#include "_Elastos_Droid_SystemUI_Recents_Views_CTaskViewThumbnail.h"
#include "elastos/droid/systemui/recents/views/TaskViewThumbnail.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

CarClass(CTaskViewThumbnail)
    , public TaskViewThumbnail
{
    CAR_OBJECT_DECL();
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_CTASKVIEWTHUMBNAIL_H__
