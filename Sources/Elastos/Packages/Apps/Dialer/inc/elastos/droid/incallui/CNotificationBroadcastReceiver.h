
#ifndef __ELASTOS_DROID_INCALLUI_CNOTIFICATIONBROADCASTRECEIVER_H__
#define __ELASTOS_DROID_INCALLUI_CNOTIFICATIONBROADCASTRECEIVER_H__

#include "_Elastos_Droid_InCallUI_CNotificationBroadcastReceiver.h"
#include <elastos/droid/incallui/InCallApp.h>

namespace Elastos {
namespace Droid {
namespace InCallUI {

CarClass(CNotificationBroadcastReceiver)
    , public InCallApp::NotificationBroadcastReceiver
{
public:
    CAR_OBJECT_DECL();
};

} // InCallUI
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_INCALLUI_CNOTIFICATIONBROADCASTRECEIVER_H__
