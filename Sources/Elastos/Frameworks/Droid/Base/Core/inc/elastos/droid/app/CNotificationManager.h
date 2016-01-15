
#ifndef __ELASTOS_DROID_APP_CNOTIFICATIONMANAGER_H__
#define __ELASTOS_DROID_APP_CNOTIFICATIONMANAGER_H__

#include "_Elastos_Droid_App_CNotificationManager.h"
#include "elastos/droid/app/NotificationManager.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CNotificationManager)
    , public NotificationManager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CNOTIFICATIONMANAGER_H__