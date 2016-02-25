
#ifndef __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_CRECENTSVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_CRECENTSVIEW_H__

#include "_Elastos_Droid_SystemUI_Recents_Views_CRecentsView.h"
#include "elastos/droid/systemui/recents/views/RecentsView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

CarClass(CRecentsView)
    , public RecentsView
{
    CAR_OBJECT_DECL();
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_CRECENTSVIEW_H__
