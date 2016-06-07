
#ifndef __ELASTOS_DROID_SYSTEMUI_RECENTS_CRECENTACTIVITY_SERVICE_BROADCASTRECEIVER_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENTS_CRECENTACTIVITY_SERVICE_BROADCASTRECEIVER_H__

#include "_Elastos_Droid_SystemUI_Recents_CRecentsActivityServiceBroadcastReceiver.h"
#include "elastos/droid/systemui/recents/RecentsActivity.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {

CarClass(CRecentsActivityServiceBroadcastReceiver)
    , public RecentsActivity::ServiceBroadcastReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_CRECENTACTIVITY_SERVICE_BROADCASTRECEIVER_H__