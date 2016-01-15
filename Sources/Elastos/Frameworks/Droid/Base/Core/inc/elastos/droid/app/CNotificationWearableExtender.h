
#ifndef __ELASTOS_DROID_APP_CNOTIFICATION_WEARABLE_EXTENDER_H__
#define __ELASTOS_DROID_APP_CNOTIFICATION_WEARABLE_EXTENDER_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Graphics.h"
#include "_Elastos_Droid_App_CNotificationWearableExtender.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Core::ICloneable;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Helper class to add wearable extensions to notifications.
 * <p class="note"> See
 * <a href="{@docRoot}wear/notifications/creating.html">Creating Notifications
 * for Android Wear</a> for more information on how to use this class.
 * <p>
 * To create a notification with wearable extensions:
 * <ol>
 *   <li>Create a {@link android.app.INotificationBuilder}, setting any desired
 *   properties.
 *   <li>Create a {@link android.app.Notification.WearableExtender}.
 *   <li>Set wearable-specific properties using the
 *   {@code add} and {@code set} methods of {@link android.app.Notification.WearableExtender}.
 *   <li>Call {@link android.app.INotificationBuilder#extend} to apply the extensions to a
 *   notification.
 *   <li>Post the notification to the notification system with the
 *   {@code NotificationManager.notify(...)} methods.
 * </ol>
 *
 * <pre class="prettyprint">
 * Notification notif = new INotificationBuilder(mContext)
 *         .setContentTitle(&quot;New mail from &quot; + sender.toString())
 *         .setContentText(subject)
 *         .setSmallIcon(R.drawable.new_mail)
 *         .extend(new Notification.WearableExtender()
 *                 .setContentIcon(R.drawable.new_mail))
 *         .build();
 * NotificationManager notificationManger =
 *         (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
 * notificationManger.notify(0, notif);</pre>
 *
 * <p>Wearable extensions can be accessed on an existing notification by using the
 * {@code WearableExtender(Notification)} constructor,
 * and then using the {@code get} methods to access values.
 *
 * <pre class="prettyprint">
 * Notification.WearableExtender wearableExtender = new Notification.WearableExtender(
 *         notification);
 * List&lt;Notification&gt; pages = wearableExtender.getPages();</pre>
 */
CarClass(CNotificationWearableExtender)
    , public Object
    , public INotificationWearableExtender
    , public INotificationExtender
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Create a {@link android.app.Notification.WearableExtender} with default
     * options.
     */
    CNotificationWearableExtender();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ INotification* notif);

    /**
     * Apply wearable extensions to a notification that is being built. This is typically
     * called by the {@link android.app.INotificationBuilder#extend} method of
     * {@link android.app.INotificationBuilder}.
     */
    //@Override
    CARAPI Extend(
        /* [in] */ INotificationBuilder* builder);

    //@Override
    CARAPI Clone(
        /* [out] */ IInterface** obj);

    /**
     * Add a wearable action to this notification.
     *
     * <p>When wearable actions are added using this method, the set of actions that
     * show on a wearable device splits from devices that only show actions added
     * using {@link android.app.INotificationBuilder#addAction}. This allows for customization
     * of which actions display on different devices.
     *
     * @param action the action to add to this notification
     * @return this object for method chaining
     * @see android.app.Notification.Action
     */
    CARAPI AddAction(
        /* [in] */ INotificationAction* action);

    /**
     * Adds wearable actions to this notification.
     *
     * <p>When wearable actions are added using this method, the set of actions that
     * show on a wearable device splits from devices that only show actions added
     * using {@link android.app.INotificationBuilder#addAction}. This allows for customization
     * of which actions display on different devices.
     *
     * @param actions the actions to add to this notification
     * @return this object for method chaining
     * @see android.app.Notification.Action
     */
    CARAPI AddActions(
        /* [in] */ IList* actions);

    /**
     * Clear all wearable actions present on this builder.
     * @return this object for method chaining.
     * @see #addAction
     */
    CARAPI ClearActions();

    /**
     * Get the wearable actions present on this notification.
     */
    CARAPI GetActions(
        /* [out] */ IList** actions);

    /**
     * Set an intent to launch inside of an activity view when displaying
     * this notification. The {@link PendingIntent} provided should be for an activity.
     *
     * <pre class="prettyprint">
     * Intent displayIntent = new Intent(context, MyDisplayActivity.class);
     * PendingIntent displayPendingIntent = PendingIntent.getActivity(context,
     *         0, displayIntent, PendingIntent.FLAG_UPDATE_CURRENT);
     * Notification notif = new INotificationBuilder(context)
     *         .extend(new Notification.WearableExtender()
     *                 .setDisplayIntent(displayPendingIntent)
     *                 .setCustomSizePreset(Notification.WearableExtender.SIZE_MEDIUM))
     *         .build();</pre>
     *
     * <p>The activity to launch needs to allow embedding, must be exported, and
     * should have an empty task affinity. It is also recommended to use the device
     * default light theme.
     *
     * <p>Example AndroidManifest.xml entry:
     * <pre class="prettyprint">
     * &lt;activity android:name=&quot;com.example.MyDisplayActivity&quot;
     *     android:exported=&quot;true&quot;
     *     android:allowEmbedded=&quot;true&quot;
     *     android:taskAffinity=&quot;&quot;
     *     android:theme=&quot;@android:style/Theme.DeviceDefault.Light&quot; /&gt;</pre>
     *
     * @param intent the {@link PendingIntent} for an activity
     * @return this object for method chaining
     * @see android.app.Notification.WearableExtender#getDisplayIntent
     */
    CARAPI SetDisplayIntent(
        /* [in] */ IPendingIntent* intent);

    /**
     * Get the intent to launch inside of an activity view when displaying this
     * notification. This {@code PendingIntent} should be for an activity.
     */
    CARAPI GetDisplayIntent(
        /* [out] */ IPendingIntent** intent);

    /**
     * Add an additional page of content to display with this notification. The current
     * notification forms the first page, and pages added using this function form
     * subsequent pages. This field can be used to separate a notification into multiple
     * sections.
     *
     * @param page the notification to add as another page
     * @return this object for method chaining
     * @see android.app.Notification.WearableExtender#getPages
     */
    CARAPI AddPage(
        /* [in] */ INotification* page);

    /**
     * Add additional pages of content to display with this notification. The current
     * notification forms the first page, and pages added using this function form
     * subsequent pages. This field can be used to separate a notification into multiple
     * sections.
     *
     * @param pages a list of notifications
     * @return this object for method chaining
     * @see android.app.Notification.WearableExtender#getPages
     */
    CARAPI AddPages(
        /* [in] */ IList* pages);

    /**
     * Clear all additional pages present on this builder.
     * @return this object for method chaining.
     * @see #addPage
     */
    CARAPI ClearPages();

    /**
     * Get the array of additional pages of content for displaying this notification. The
     * current notification forms the first page, and elements within this array form
     * subsequent pages. This field can be used to separate a notification into multiple
     * sections.
     * @return the pages for this notification
     */
    CARAPI GetPages(
        /* [out] */ IList** pages);

    /**
     * Set a background image to be displayed behind the notification content.
     * Contrary to the {@link android.app.Notification.BigPictureStyle}, this background
     * will work with any notification style.
     *
     * @param background the background bitmap
     * @return this object for method chaining
     * @see android.app.Notification.WearableExtender#getBackground
     */
    CARAPI SetBackground(
        /* [in] */ IBitmap* background);

    /**
     * Get a background image to be displayed behind the notification content.
     * Contrary to the {@link android.app.Notification.BigPictureStyle}, this background
     * will work with any notification style.
     *
     * @return the background image
     * @see android.app.Notification.WearableExtender#setBackground
     */
    CARAPI GetBackground(
        /* [out] */ IBitmap** background);

    /**
     * Set an icon that goes with the content of this notification.
     */
    CARAPI SetContentIcon(
        /* [in] */ Int32 icon);

    /**
     * Get an icon that goes with the content of this notification.
     */
    CARAPI GetContentIcon(
        /* [out] */ Int32* icon);

    /**
     * Set the gravity that the content icon should have within the notification display.
     * Supported values include {@link android.view.Gravity#START} and
     * {@link android.view.Gravity#END}. The default value is {@link android.view.Gravity#END}.
     * @see #setContentIcon
     */
    CARAPI SetContentIconGravity(
        /* [in] */ Int32 contentIconGravity);

    /**
     * Get the gravity that the content icon should have within the notification display.
     * Supported values include {@link android.view.Gravity#START} and
     * {@link android.view.Gravity#END}. The default value is {@link android.view.Gravity#END}.
     * @see #getContentIcon
     */
    CARAPI GetContentIconGravity(
        /* [out] */ Int32* contentIconGravity);

    /**
     * Set an action from this notification's actions to be clickable with the content of
     * this notification. This action will no longer display separately from the
     * notification's content.
     *
     * <p>For notifications with multiple pages, child pages can also have content actions
     * set, although the list of available actions comes from the main notification and not
     * from the child page's notification.
     *
     * @param actionIndex The index of the action to hoist onto the current notification page.
     *                    If wearable actions were added to the main notification, this index
     *                    will apply to that list, otherwise it will apply to the regular
     *                    actions list.
     */
    CARAPI SetContentAction(
        /* [in] */ Int32 actionIndex);

    /**
     * Get the index of the notification action, if any, that was specified as being clickable
     * with the content of this notification. This action will no longer display separately
     * from the notification's content.
     *
     * <p>For notifications with multiple pages, child pages can also have content actions
     * set, although the list of available actions comes from the main notification and not
     * from the child page's notification.
     *
     * <p>If wearable specific actions were added to the main notification, this index will
     * apply to that list, otherwise it will apply to the regular actions list.
     *
     * @return the action index or {@link #UNSET_ACTION_INDEX} if no action was selected.
     */
    CARAPI GetContentAction(
        /* [out] */ Int32* actionIndex);

    /**
     * Set the gravity that this notification should have within the available viewport space.
     * Supported values include {@link android.view.Gravity#TOP},
     * {@link android.view.Gravity#CENTER_VERTICAL} and {@link android.view.Gravity#BOTTOM}.
     * The default value is {@link android.view.Gravity#BOTTOM}.
     */
    CARAPI SetGravity(
        /* [in] */ Int32 gravity);

    /**
     * Get the gravity that this notification should have within the available viewport space.
     * Supported values include {@link android.view.Gravity#TOP},
     * {@link android.view.Gravity#CENTER_VERTICAL} and {@link android.view.Gravity#BOTTOM}.
     * The default value is {@link android.view.Gravity#BOTTOM}.
     */
    CARAPI GetGravity(
        /* [out] */ Int32* gravity);

    /**
     * Set the custom size preset for the display of this notification out of the available
     * presets found in {@link android.app.Notification.WearableExtender}, e.g.
     * {@link #SIZE_LARGE}.
     * <p>Some custom size presets are only applicable for custom display notifications created
     * using {@link android.app.Notification.WearableExtender#setDisplayIntent}. Check the
     * documentation for the preset in question. See also
     * {@link #setCustomContentHeight} and {@link #getCustomSizePreset}.
     */
    CARAPI SetCustomSizePreset(
        /* [in] */ Int32 sizePreset);

    /**
     * Get the custom size preset for the display of this notification out of the available
     * presets found in {@link android.app.Notification.WearableExtender}, e.g.
     * {@link #SIZE_LARGE}.
     * <p>Some custom size presets are only applicable for custom display notifications created
     * using {@link #setDisplayIntent}. Check the documentation for the preset in question.
     * See also {@link #setCustomContentHeight} and {@link #setCustomSizePreset}.
     */
    CARAPI GetCustomSizePreset(
        /* [out] */ Int32* sizePreset);

    /**
     * Set the custom height in pixels for the display of this notification's content.
     * <p>This option is only available for custom display notifications created
     * using {@link android.app.Notification.WearableExtender#setDisplayIntent}. See also
     * {@link android.app.Notification.WearableExtender#setCustomSizePreset} and
     * {@link #getCustomContentHeight}.
     */
    CARAPI SetCustomContentHeight(
        /* [in] */ Int32 height);

    /**
     * Get the custom height in pixels for the display of this notification's content.
     * <p>This option is only available for custom display notifications created
     * using {@link #setDisplayIntent}. See also {@link #setCustomSizePreset} and
     * {@link #setCustomContentHeight}.
     */
    CARAPI GetCustomContentHeight(
        /* [out] */ Int32* height);

    /**
     * Set whether the scrolling position for the contents of this notification should start
     * at the bottom of the contents instead of the top when the contents are too long to
     * display within the screen.  Default is false (start scroll at the top).
     */
    CARAPI SetStartScrollBottom(
        /* [in] */ Boolean startScrollBottom);

    /**
     * Get whether the scrolling position for the contents of this notification should start
     * at the bottom of the contents instead of the top when the contents are too long to
     * display within the screen. Default is false (start scroll at the top).
     */
    CARAPI GetStartScrollBottom(
        /* [out] */ Boolean* startScrollBottom);

    /**
     * Set whether the content intent is available when the wearable device is not connected
     * to a companion device.  The user can still trigger this intent when the wearable device
     * is offline, but a visual hint will indicate that the content intent may not be available.
     * Defaults to true.
     */
    CARAPI SetContentIntentAvailableOffline(
        /* [in] */ Boolean contentIntentAvailableOffline);

    /**
     * Get whether the content intent is available when the wearable device is not connected
     * to a companion device.  The user can still trigger this intent when the wearable device
     * is offline, but a visual hint will indicate that the content intent may not be available.
     * Defaults to true.
     */
    CARAPI GetContentIntentAvailableOffline(
        /* [out] */ Boolean* contentIntentAvailableOffline);

    /**
     * Set a hint that this notification's icon should not be displayed.
     * @param hintHideIcon {@code true} to hide the icon, {@code false} otherwise.
     * @return this object for method chaining
     */
    CARAPI SetHintHideIcon(
        /* [in] */ Boolean hintHideIcon);

    /**
     * Get a hint that this notification's icon should not be displayed.
     * @return {@code true} if this icon should not be displayed, false otherwise.
     * The default value is {@code false} if this was never set.
     */
    CARAPI GetHintHideIcon(
        /* [out] */ Boolean* hintHideIcon);

    /**
     * Set a visual hint that only the background image of this notification should be
     * displayed, and other semantic content should be hidden. This hint is only applicable
     * to sub-pages added using {@link #addPage}.
     */
    CARAPI SetHintShowBackgroundOnly(
        /* [in] */ Boolean hintShowBackgroundOnly);

    /**
     * Get a visual hint that only the background image of this notification should be
     * displayed, and other semantic content should be hidden. This hint is only applicable
     * to sub-pages added using {@link android.app.Notification.WearableExtender#addPage}.
     */
    CARAPI GetHintShowBackgroundOnly(
        /* [out] */ Boolean* result);

private:
    void SetFlag(
        /* [in] */ Int32 mask,
        /* [in] */ Boolean value);

private:

    /** Notification extra which contains wearable extensions */
    static const String EXTRA_WEARABLE_EXTENSIONS;

    // Keys within EXTRA_WEARABLE_OPTIONS for wearable options.
    static const String KEY_ACTIONS;
    static const String KEY_FLAGS;
    static const String KEY_DISPLAY_INTENT;
    static const String KEY_PAGES;
    static const String KEY_BACKGROUND;
    static const String KEY_CONTENT_ICON;
    static const String KEY_CONTENT_ICON_GRAVITY;
    static const String KEY_CONTENT_ACTION_INDEX;
    static const String KEY_CUSTOM_SIZE_PRESET;
    static const String KEY_CUSTOM_CONTENT_HEIGHT;
    static const String KEY_GRAVITY;

    // Flags bitwise-ored to mFlags
    static const Int32 FLAG_CONTENT_INTENT_AVAILABLE_OFFLINE;
    static const Int32 FLAG_HINT_HIDE_ICON;
    static const Int32 FLAG_HINT_SHOW_BACKGROUND_ONLY;
    static const Int32 FLAG_START_SCROLL_BOTTOM;

    // Default value for flags integer
    static const Int32 DEFAULT_FLAGS;

    static const Int32 DEFAULT_CONTENT_ICON_GRAVITY;
    static const Int32 DEFAULT_GRAVITY;

    AutoPtr<IArrayList> mActions;// = new ArrayList<Action>();
    Int32 mFlags;
    AutoPtr<IPendingIntent> mDisplayIntent;
    AutoPtr<IArrayList> mPages;// = new ArrayList<Notification>();
    AutoPtr<IBitmap> mBackground;
    Int32 mContentIcon;
    Int32 mContentIconGravity;
    Int32 mContentActionIndex;
    Int32 mCustomSizePreset;
    Int32 mCustomContentHeight;
    Int32 mGravity;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CNOTIFICATION_WEARABLE_EXTENDER_H__
