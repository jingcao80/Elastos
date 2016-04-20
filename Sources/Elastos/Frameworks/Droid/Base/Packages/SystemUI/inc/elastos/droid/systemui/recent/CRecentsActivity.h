
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTSACTIVITY_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTSACTIVITY_H__

#include "_Elastos_Droid_SystemUI_Recent_CRecentsActivity.h"
#include "elastos/droid/systemui/recent/RecentsActivity.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

CarClass(CRecentsActivity)
    , public RecentsActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Recent
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENT_CRECENTSACTIVITY_H__