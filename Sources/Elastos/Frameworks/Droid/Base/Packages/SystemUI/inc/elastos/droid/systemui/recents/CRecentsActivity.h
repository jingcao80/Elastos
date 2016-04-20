
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_CRECENTSACTIVITY_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_CRECENTSACTIVITY_H__

#include "_Elastos_Droid_SystemUI_Recents_CRecentsActivity.h"
#include "elastos/droid/systemui/recents/RecentsActivity.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {

CarClass(CRecentsActivity)
    , public RecentsActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_CRECENTSACTIVITY_H__
