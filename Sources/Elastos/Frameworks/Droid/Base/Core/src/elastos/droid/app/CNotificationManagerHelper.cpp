#include "elastos/droid/app/CNotificationManagerHelper.h"
#include "elastos/droid/app/NotificationManager.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CNotificationManagerHelper, Object, INotificationManagerHelper)

CAR_SINGLETON_IMPL(CNotificationManagerHelper)

/** @hide */
ECode CNotificationManagerHelper::GetService(
    /* [out] */ IINotificationManager** mgr)
{
    VALIDATE_NOT_NULL(mgr)
    AutoPtr<IINotificationManager> service = NotificationManager::GetService();
    *mgr = service;
    REFCOUNT_ADD(*mgr)
    return NOERROR;
}

/** @hide */
ECode CNotificationManagerHelper::From(
    /* [in] */ IContext* context,
    /* [out] */ INotificationManager** mgr)
{
    VALIDATE_NOT_NULL(mgr)
    AutoPtr<INotificationManager> service = NotificationManager::From(context);
    *mgr = service;
    REFCOUNT_ADD(*mgr)
    return NOERROR;
}


} // namespace App
} // namespace Droid
} // namespace Elastos