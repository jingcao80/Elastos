
#ifndef __ELASTOS_DROID_APP_CNOTIFICATIONMANAGER_HELPER_H__
#define __ELASTOS_DROID_APP_CNOTIFICATIONMANAGER_HELPER_H__

#include "_Elastos_Droid_App_CNotificationManagerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CNotificationManagerHelper)
    , public Object
    , public INotificationManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /** @hide */
    CARAPI GetService(
        /* [out] */ IINotificationManager** mgr);

    /** @hide */
    CARAPI From(
        /* [in] */ IContext* context,
        /* [out] */ INotificationManager** mgr);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CNOTIFICATIONMANAGER_HELPER_H__