
#ifndef __ELASTOS_DROID_SYSTEMUI_RECENTS_CRECENTACTIVITY_SYSTEM_BROADCASTRECEIVER_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENTS_CRECENTACTIVITY_SYSTEM_BROADCASTRECEIVER_H__

#include "_Elastos_Droid_SystemUI_Recents_CRecentsActivitySystemBroadcastReceiver.h"
#include "elastos/droid/systemui/recents/RecentsActivity.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {

CarClass(CRecentsActivitySystemBroadcastReceiver)
    , public RecentsActivity::SystemBroadcastReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_CRECENTACTIVITY_SYSTEM_BROADCASTRECEIVER_H__