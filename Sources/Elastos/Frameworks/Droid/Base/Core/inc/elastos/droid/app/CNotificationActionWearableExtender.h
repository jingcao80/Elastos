
#ifndef __ELASTOS_DROID_APP_CNOTIFICATIONACTIONWEARABLEEXTENDER_H__
#define __ELASTOS_DROID_APP_CNOTIFICATIONACTIONWEARABLEEXTENDER_H__

#include "_Elastos_Droid_App_CNotificationActionWearableExtender.h"
#include "elastos/droid/app/NotificationActionWearableExtender.h"

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Wearable extender for notification actions. To add extensions to an action,
 * create a new {@link android.app.Notification.Action.WearableExtender} object using
 * the {@code WearableExtender()} constructor and apply it to a
 * {@link android.app.Notification.Action.Builder} using
 * {@link android.app.Notification.Action.Builder#extend}.
 *
 * <pre class="prettyprint">
 * Notification.Action action = new Notification.Action.Builder(
 *         R.drawable.archive_all, "Archive all", actionIntent)
 *         .extend(new Notification.Action.WearableExtender()
 *                 .setAvailableOffline(false))
 *         .build();</pre>
 */
CarClass(CNotificationActionWearableExtender)
    , public NotificationActionWearableExtender
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CNOTIFICATIONACTIONWEARABLEEXTENDER_H__
