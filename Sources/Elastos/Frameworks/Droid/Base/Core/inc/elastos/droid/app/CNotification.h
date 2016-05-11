
#ifndef __ELASTOS_DROID_APP_CNOTIFICATION_H__
#define __ELASTOS_DROID_APP_CNOTIFICATION_H__

#include "Elastos.Droid.App.h"
#include "elastos/droid/widget/RemoteViews.h"
#include "_Elastos_Droid_App_CNotification.h"

using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::RemoteViews;
using Elastos::Droid::Widget::IRemoteViews;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Core::Object;
using Elastos::Core::ICharSequence;
using Elastos::Text::INumberFormat;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * A class that represents how a persistent notification is to be presented to
 * the user using the {@link android.app.NotificationManager}.
 *
 * <p>The {@link Notification.Builder Notification.Builder} has been added to make it
 * easier to construct Notifications.</p>
 *
 * <div class="special reference">
 * <h3>Developer Guides</h3>
 * <p>For a guide to creating notifications, read the
 * <a href="{@docRoot}guide/topics/ui/notifiers/notifications.html">Status Bar Notifications</a>
 * developer guide.</p>
 * </div>
 */
CarClass(CNotification)
    , public Object
    , public INotification
    , public IParcelable
{
public:
    class BuilderRemoteViews
        : public RemoteViews
        , public INotificationBuilderRemoteViews
    {
    public:
        CAR_INTERFACE_DECL()

        BuilderRemoteViews();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IApplicationInfo* appInfo,
            /* [in] */ Int32 layoutId);

        // @Override
        CARAPI Clone(
            /* [out] */ IRemoteViews** obj);

    protected:
        CARAPI CloneImpl(
            /* [in] */ INotificationBuilderRemoteViews* views);
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CNotification();

    virtual ~CNotification();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 icon,
        /* [in] */ ICharSequence* tickerText,
        /* [in] */ Int64 when,
        /* [in] */ ICharSequence* contentTitle,
        /* [in] */ ICharSequence* contentText,
        /* [in] */ IIntent* contentIntent);

    CARAPI constructor(
        /* [in] */ Int32 icon,
        /* [in] */ ICharSequence* tickerText,
        /* [in] */ Int64 when);

    CARAPI GetWhen(
        /* [out] */ Int64* when);

    CARAPI SetWhen(
        /* [in] */ Int64 when);

    CARAPI GetIcon(
        /* [out] */ Int32* icon);

    CARAPI SetIcon(
        /* [in] */ Int32 icon);

    CARAPI GetIconLevel(
        /* [out] */ Int32* iconLevel);

    CARAPI SetIconLevel(
        /* [in] */ Int32 iconLevel);

    CARAPI GetNumber(
        /* [out] */ Int32* number);

    CARAPI SetNumber(
        /* [in] */ Int32 number);

    CARAPI GetContentIntent(
        /* [out] */ IPendingIntent** intent);

    CARAPI SetContentIntent(
        /* [in] */ IPendingIntent* intent);

    CARAPI GetDeleteIntent(
        /* [out] */ IPendingIntent** intent);

    CARAPI SetDeleteIntent(
        /* [in] */ IPendingIntent* intent);

    CARAPI GetFullScreenIntent(
        /* [out] */ IPendingIntent** intent);

    CARAPI SetFullScreenIntent(
        /* [in] */ IPendingIntent* intent);

    CARAPI GetTickerText(
        /* [out] */ ICharSequence** text);

    CARAPI SetTickerText(
        /* [in] */ ICharSequence* text);

    CARAPI GetTickerView(
        /* [out] */ IRemoteViews** view);

    CARAPI SetTickerView(
        /* [in] */ IRemoteViews* view);

    CARAPI GetContentView(
        /* [out] */ IRemoteViews** view);

    CARAPI SetContentView(
        /* [in] */ IRemoteViews* view);

    CARAPI GetBigContentView(
        /* [out] */ IRemoteViews** view);

    CARAPI SetBigContentView(
        /* [in] */ IRemoteViews* view);

    CARAPI GetLargeIcon(
        /* [out] */ IBitmap** bitmap);

    CARAPI SetLargeIcon(
        /* [in] */ IBitmap* bitmap);

    CARAPI GetSound(
        /* [out] */ IUri** sound);

    CARAPI SetSound(
        /* [in] */ IUri* sound);

    CARAPI GetAudioStreamType(
        /* [out] */ Int32* type);

    CARAPI SetAudioStreamType(
        /* [in] */ Int32 type);

    CARAPI GetAudioAttributes (
        /* [out] */ IAudioAttributes** attrs);

    CARAPI SetAudioAttributes (
        /* [in] */ IAudioAttributes* attrs);

    CARAPI GetVibrate(
        /* [out, callee] */ ArrayOf<Int64>** vibrate);

    CARAPI SetVibrate(
        /* [in] */ ArrayOf<Int64>* vibrate);

    CARAPI GetLedARGB(
        /* [out] */ Int32* argb);

    CARAPI SetLedARGB(
        /* [in] */ Int32 argb);

    CARAPI GetLedOnMS(
        /* [out] */ Int32* onMS);

    CARAPI SetLedOnMS(
        /* [in] */ Int32 onMS);

    CARAPI GetLedOffMS(
        /* [out] */ Int32* offMS);

    CARAPI SetLedOffMS(
        /* [in] */ Int32 offMS);

    CARAPI GetDefaults(
        /* [out] */ Int32* defaults);

    CARAPI SetDefaults(
        /* [in] */ Int32 defaults);

    CARAPI GetFlags(
        /* [out] */ Int32* flags);

    CARAPI SetFlags(
        /* [in] */ Int32 flags);

    CARAPI GetPriority(
        /* [out] */ Int32* priority);

    CARAPI SetPriority(
        /* [in] */ Int32 priority);

    /**
    * Get the key used to group this notification into a cluster or stack
    * with other notifications on devices which support such rendering.
     */
    CARAPI GetGroup(
        /* [out] */ String* group);

    CARAPI SetGroup(
        /* [in] */ const String& group);

    /**
    * Get a sort key that orders this notification among other notifications from the
    * same package. This can be useful if an external sort was already applied and an app
    * would like to preserve this. Notifications will be sorted lexicographically using this
    * value, although providing different priorities in addition to providing sort key may
    * cause this value to be ignored.
     *
    * <p>This sort key can also be used to order members of a notification group. See
    * {@link Builder#setGroup}.
     *
    * @see String#compareTo(String)
     */
    CARAPI GetSortKey(
        /* [out] */ String* sortKey);

    CARAPI SetSortKey(
        /* [in] */ const String& sortKey);

    CARAPI GetKind(
        /* [out, callee] */ ArrayOf<String>** kind);

    CARAPI SetKind(
        /* [in] */ ArrayOf<String>* kind);

    CARAPI GetExtras(
        /* [out, callee] */ IBundle** extras);

    CARAPI SetExtras(
        /* [in] */ IBundle* extras);

    CARAPI GetActions(
        /* [out, callee] */ ArrayOf<INotificationAction*>** actions);

    CARAPI SetActions(
        /* [in] */ ArrayOf<INotificationAction*>* actions);

    CARAPI GetHeadsUpContentView(
        /* [out] */ IRemoteViews** view);

    CARAPI SetHeadsUpContentView(
        /* [in] */ IRemoteViews* view);

    CARAPI GetVisibility(
        /* [out] */ Int32* result);

    CARAPI SetVisibility(
        /* [in] */ Int32 color);

    CARAPI GetColor(
        /* [out] */ Int32* result);

    CARAPI SetColor(
        /* [in] */ Int32 color);

    CARAPI GetCategory(
        /* [out] */ String* value);

    CARAPI SetCategory(
        /* [in] */ const String& result);

    CARAPI SetPublicVersion(
        /* [in] */ INotification* value);

    CARAPI GetPublicVersion(
        /* [out] */ INotification** result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //@Override
    CARAPI Clone(
        /* [out] */ IInterface** itfc);

    /**
    * Copy all (or if heavy is false, all except Bitmaps and RemoteViews) members
    * of this into that.
    * @hide
     */
    CARAPI CloneInto(
        /* [in] */ INotification* that,
        /* [in] */ Boolean heavy);

    /**
    * Removes heavyweight parts of the Notification object for archival or for sending to
    * listeners when the full contents are not necessary.
    * @hide
     */
    CARAPI LightenPayload();

    /**
    * Make sure this CharSequence is safe to put into a bundle, which basically
    * means it had better not be some custom Parcelable implementation.
    * @hide
     */
    static AutoPtr<ICharSequence> SafeCharSequence(
        /* [in] */ ICharSequence* cs);

    /**
    * Sets the {@link #contentView} field to be a view with the standard "Latest Event"
    * layout.
     *
    * <p>Uses the {@link #icon} and {@link #when} fields to set the icon and time fields
    * in the view.</p>
    * @param context       The context for your application / activity.
    * @param contentTitle The title that goes in the expanded entry.
    * @param contentText  The text that goes in the expanded entry.
    * @param contentIntent The intent to launch when the user clicks the expanded notification.
    * If this is an activity, it must include the
    * {@link android.content.Intent#FLAG_ACTIVITY_NEW_TASK} flag, which requires
    * that you take care of task management as described in the
    * <a href="{@docRoot}guide/topics/fundamentals/tasks-and-back-stack.html">Tasks and Back
    * Stack</a> document.
     *
    * @deprecated Use {@link Builder} instead.
     */
    //@Deprecated
    CARAPI SetLatestEventInfo(
        /* [in] */ IContext* context,
        /* [in] */ ICharSequence* contentTitle,
        /* [in] */ ICharSequence* contentText,
        /* [in] */ IPendingIntent* contentIntent);

    CARAPI ToString(
        /* [out] */ String* info);

    /**
    * {@hide}
     */
    static String VisibilityToString(
        /* [in] */ Int32 vis);

    /**
    * @hide
     */
    Boolean IsValid();

    CARAPI IsValid(
        /* [out] */ Boolean* result);

    /**
    * @hide
     */
    Boolean IsGroupSummary();

    CARAPI IsGroupSummary(
        /* [out] */ Boolean* result);

    /**
    * @hide
     */
    Boolean IsGroupChild();

    CARAPI IsGroupChild(
        /* [out] */ Boolean* result);

public:
    static const String TAG;// = "Notification";

    /**
    * Maximum length of CharSequences accepted by Builder and friends.
     *
    * <p>
    * Avoids spamming the system with overly large strings such as full e-mails.
     */
    static const Int32 MAX_CHARSEQUENCE_LENGTH; // = 5* 1024;

    /**
    * The timestamp for the notification.  The icons and expanded views
    * are sorted by this key.
     */
    Int64 mWhen;

    /**
    * The resource id of a drawable to use as the icon in the status bar.
     */
    Int32 mIcon;

    /**
    * If the icon in the status bar is to have more than one level, you can set this.  Otherwise,
    * leave it at its default value of 0.
     *
    * @see android.widget.ImageView#setImageLevel
    * @see android.graphics.drawable#setLevel
     */
    Int32 mIconLevel;

    /**
    * The number of events that this notification represents.  For example, in a new mail
    * notification, this could be the number of unread messages.  This number is superimposed over
    * the icon in the status bar.  If the number is 0 or negative, it is not shown in the status
    * bar.
     */
    Int32 mNumber;

    /**
    * The intent to execute when the expanded status entry is clicked.  If
    * this is an activity, it must include the
    * {@link android.content.Intent#FLAG_ACTIVITY_NEW_TASK} flag, which requires
    * that you take care of task management as described in the <em>Activities and Tasks</em>
    * section of the <a href="{@docRoot}guide/topics/fundamentals.html#acttask">Application
    * Fundamentals</a> document.
     */
    AutoPtr<IPendingIntent> mContentIntent;

    /**
    * The intent to execute when the status entry is deleted by the user
    * with the "Clear All Notifications" button. This probably shouldn't
    * be launching an activity since several of those will be sent at the
    * same time.
     */
    AutoPtr<IPendingIntent> mDeleteIntent;

    /**
    * An intent to launch instead of posting the notification to the status bar.
     *
    * @see Notification.Builder#setFullScreenIntent
     */
    AutoPtr<IPendingIntent> mFullScreenIntent;

    /**
    * Text to scroll across the screen when this item is added to
    * the status bar.
     */
    AutoPtr<ICharSequence> mTickerText;

    /**
    * The view to show as the ticker in the status bar when the notification
    * is posted.
     */
    AutoPtr<IRemoteViews> mTickerView;

    /**
    * The view that will represent this notification in the expanded status bar.
     */
    AutoPtr<IRemoteViews> mContentView;

    /**
    * A large-format version of {@link #contentView}, giving the Notification an
    * opportunity to show more detail. The system UI may choose to show this
    * instead of the normal content view at its discretion.
     */
    AutoPtr<IRemoteViews> mBigContentView;

    /**
    * A medium-format version of {@link #contentView}, providing the Notification an
    * opportunity to add action buttons to contentView. At its discretion, the system UI may
    * choose to show this as a heads-up notification, which will pop up so the user can see
    * it without leaving their current activity.
     */
    AutoPtr<IRemoteViews> mHeadsUpContentView;

    /**
    * The bitmap that may escape the bounds of the panel and bar.
     */
    AutoPtr<IBitmap> mLargeIcon;

    /**
    * The sound to play.
     *
    * <p>
    * To play the default notification sound, see {@link #defaults}.
    * </p>
     */
    AutoPtr<IUri> mSound;

    /**
    * The audio stream type to use when playing the sound.
    * Should be one of the STREAM_ constants from
    * {@link android.media.AudioManager}.
     */
    Int32 mAudioStreamType;

    /**
    * The default value of {@link #audioAttributes}.
     */
    static const AutoPtr<IAudioAttributes> AUDIO_ATTRIBUTES_DEFAULT;
         // = new AudioAttributes.Builder()
         //    .setContentType(AudioAttributes.CONTENT_TYPE_SONIFICATION)
         //    .setUsage(AudioAttributes.USAGE_NOTIFICATION)
         //    .build();

    /**
    * The {@link AudioAttributes audio attributes} to use when playing the sound.
     */
    AutoPtr<IAudioAttributes> mAudioAttributes;// = AUDIO_ATTRIBUTES_DEFAULT;

    /**
    * The pattern with which to vibrate.
     *
    * <p>
    * To vibrate the default pattern, see {@link #defaults}.
    * </p>
     *
    * @see android.os.Vibrator#vibrate(long[],int)
     */
    AutoPtr< ArrayOf<Int64> > mVibrate;

    /**
   * The color of the led.  The hardware will do its best approximation.
    *
   * @see #FLAG_SHOW_LIGHTS
   * @see #flags
    */
    Int32 mLedARGB;

    /**
    * The number of milliseconds for the LED to be on while it's flashing.
    * The hardware will do its best approximation.
     *
    * @see #FLAG_SHOW_LIGHTS
    * @see #flags
     */
    Int32 mLedOnMS;

    /**
    * The number of milliseconds for the LED to be off while it's flashing.
    * The hardware will do its best approximation.
     *
    * @see #FLAG_SHOW_LIGHTS
    * @see #flags
     */
    Int32 mLedOffMS;

    /**
    * Specifies which values should be taken from the defaults.
    * <p>
    * To set, OR the desired from {@link #DEFAULT_SOUND},
    * {@link #DEFAULT_VIBRATE}, {@link #DEFAULT_LIGHTS}. For all default
    * values, use {@link #DEFAULT_ALL}.
    * </p>
     */
    Int32 mDefaults;

    Int32 mFlags;

    /**
    * Relative priority for this notification.
     *
    * Priority is an indication of how much of the user's valuable attention should be consumed by
    * this notification. Low-priority notifications may be hidden from the user in certain
    * situations, while the user might be interrupted for a higher-priority notification. The
    * system will make a determination about how to interpret this priority when presenting
    * the notification
     */
    Int32 mPriority;

    /**
    * Accent color (an ARGB integer like the constants in {@link android.graphics.Color})
    * to be applied by the standard Style templates when presenting this notification.
     *
    * The current template design constructs a colorful header image by overlaying the
    * {@link #icon} image (stenciled in white) atop a field of this color. Alpha components are
    * ignored.
     */
    Int32 mColor;// = COLOR_DEFAULT;

    /**
    * Sphere of visibility of this notification, which affects how and when the SystemUI reveals
    * the notification's presence and contents in untrusted situations (namely, on the secure
    * lockscreen).
     *
    * The default level, {@link #VISIBILITY_PRIVATE}, behaves exactly as notifications have always
    * done on Android: The notification's {@link #icon} and {@link #tickerText} (if available) are
    * shown in all situations, but the contents are only available if the device is unlocked for
    * the appropriate user.
     *
    * A more permissive policy can be expressed by {@link #VISIBILITY_PUBLIC}; such a notification
    * can be read even in an "insecure" context (that is, above a secure lockscreen).
    * To modify the public version of this notification—for example, to redact some portions—see
    * {@link Builder#setPublicVersion(Notification)}.
     *
    * Finally, a notification can be made {@link #VISIBILITY_SECRET}, which will suppress its icon
    * and ticker until the user has bypassed the lockscreen.
     */
    Int32 mVisibility;

    /**
    * One of the predefined notification categories (see the <code>CATEGORY_*</code> constants)
    * that best describes this Notification.  May be used by the system for ranking and filtering.
     */
    String mCategory;

    String mGroupKey;

    String mSortKey;

    /**
    * @hide
    * If this notification matches of one or more special types (see the <code>KIND_*</code>
    * constants), add them here, best match first.
     */
    AutoPtr<ArrayOf<String> > mKind;

    /**
    * Additional semantic data to be carried around with this Notification.
    * <p>
    * The extras keys defined here are intended to capture the original inputs to {@link Builder}
    * APIs, and are intended to be used by
    * {@link android.service.notification.NotificationListenerService} implementations to extract
    * detailed information from notification objects.
     */
    AutoPtr<IBundle> mExtras;

    /**
    * Array of all {@link Action} structures attached to this notification by
    * {@link Builder#addAction(int, CharSequence, PendingIntent)}. Mostly useful for instances of
    * {@link android.service.notification.NotificationListenerService} that provide an alternative
    * interface for invoking actions.
     */
    AutoPtr<ArrayOf<INotificationAction*> > mActions;

    /**
    * Replacement version of this notification whose content will be shown
    * in an insecure context such as atop a secure keyguard. See {@link #visibility}
    * and {@link #VISIBILITY_PUBLIC}.
     */
    AutoPtr<INotification> mPublicVersion;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CNOTIFICATION_H__
