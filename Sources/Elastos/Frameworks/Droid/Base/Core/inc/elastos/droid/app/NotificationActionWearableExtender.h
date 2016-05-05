
#ifndef __ELASTOS_DROID_APP_NOTIFICATIONACTIONWEARABLEEXTENDER_H__
#define __ELASTOS_DROID_APP_NOTIFICATIONACTIONWEARABLEEXTENDER_H__

#include "Elastos.Droid.App.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

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
class NotificationActionWearableExtender
    : public Object
    , public INotificationActionExtender
    , public INotificationActionWearableExtender
{
public:
    CAR_INTERFACE_DECL()

    NotificationActionWearableExtender();

    /**
     * Create a {@link android.app.Notification.Action.WearableExtender} with default
     * options.
     */
    CARAPI constructor();

    /**
     * Create a {@link android.app.Notification.Action.WearableExtender} by reading
     * wearable options present in an existing notification action.
     * @param action the notification action to inspect.
     */
    CARAPI constructor(
        /* [in] */ INotificationAction* action);

    /**
     * Apply wearable extensions to a notification action that is being built. This is
     * typically called by the {@link android.app.Notification.Action.Builder#extend}
     * method of {@link android.app.Notification.Action.Builder}.
     */
    // @Override
    CARAPI Extend(
        /* [in] */ INotificationActionBuilder* builder);

    // @Override
    CARAPI Clone(
        /* [out] */ INotificationActionWearableExtender** result);

    /**
     * Set whether this action is available when the wearable device is not connected to
     * a companion device. The user can still trigger this action when the wearable device is
     * offline, but a visual hint will indicate that the action may not be available.
     * Defaults to true.
     */
    CARAPI SetAvailableOffline(
        /* [in] */ Boolean availableOffline);

    /**
     * Get whether this action is available when the wearable device is not connected to
     * a companion device. The user can still trigger this action when the wearable device is
     * offline, but a visual hint will indicate that the action may not be available.
     * Defaults to true.
     */
    CARAPI IsAvailableOffline(
        /* [out] */ Boolean* result);

private:
    CARAPI_(void) SetFlag(
        /* [in] */ Int32 mask,
        /* [in] */ Boolean value);

private:
    /** Notification action extra which contains wearable extensions */
    static const String EXTRA_WEARABLE_EXTENSIONS;

    static const String KEY_FLAGS;

    // Flags bitwise-ored to mFlags
    static const Int32 FLAG_AVAILABLE_OFFLINE;

    // Default value for flags integer
    static const Int32 DEFAULT_FLAGS;

    Int32 mFlags;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_NOTIFICATIONACTIONWEARABLEEXTENDER_H__
