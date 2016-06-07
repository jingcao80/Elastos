
#ifndef __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_CANIMATEABLE_VIEWBOUNDS_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_CANIMATEABLE_VIEWBOUNDS_H__

#include "_Elastos_Droid_SystemUI_Recents_Views_CAnimateableViewBounds.h"
#include "elastos/droid/systemui/recents/views/AnimateableViewBounds.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

CarClass(CAnimateableViewBounds)
    , public AnimateableViewBounds
{
    CAR_OBJECT_DECL()
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_CANIMATEABLE_VIEWBOUNDS_H__
