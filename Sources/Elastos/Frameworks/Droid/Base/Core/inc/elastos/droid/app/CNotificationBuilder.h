//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_APP_CNOTIFICATIONBUILDER_H__
#define __ELASTOS_DROID_APP_CNOTIFICATIONBUILDER_H__

#include "_Elastos_Droid_App_CNotificationBuilder.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IRemoteViews;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Internal::Utility::INotificationColorUtil;
using Elastos::Utility::Etl::List;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Text::INumberFormat;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CNotificationBuilder)
    , public Object
    , public INotificationBuilder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CNotificationBuilder();

    virtual ~CNotificationBuilder();

    CARAPI constructor(
        /* [in] */ IContext * context);

    /**
     * Creates a Builder for rebuilding the given Notification.
     * <p>
     * Call {@link #rebuild()} to retrieve the rebuilt version of 'n'.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ INotification* n);

    /**
     * Add a timestamp pertaining to the notification (usually the time the event occurred).
     * It will be shown in the notification content view by default; use
     * {@link Builder#setShowWhen(boolean) setShowWhen} to control this.
     *
     * @see Notification#when
     */
    CARAPI SetWhen(
        /* [in] */ Int64 when);

    /**
     * Control whether the timestamp set with {@link Builder#setWhen(long) setWhen} is shown
     * in the content view.
     */
    CARAPI SetShowWhen(
        /* [in] */ Boolean show);

    /**
     * Show the {@link Notification#when} field as a stopwatch.
     *
     * Instead of presenting <code>when</code> as a timestamp, the notification will show an
     * automatically updating display of the minutes and seconds since <code>when</code>.
     *
     * Useful when showing an elapsed time (like an ongoing phone call).
     *
     * @see android.widget.Chronometer
     * @see Notification#when
     */
    CARAPI SetUsesChronometer(
        /* [in] */ Boolean b);

    /**
     * Set the small icon resource, which will be used to represent the notification in the
     * status bar.
     *

     * The platform template for the expanded view will draw this icon in the left, unless a
     * {@link #setLargeIcon(Bitmap) large icon} has also been specified, in which case the small
     * icon will be moved to the right-hand side.
     *

     * @param icon
     *            A resource ID in the application's package of the drawable to use.
     * @see Notification#icon
     */
    CARAPI SetSmallIcon(
        /* [in] */ Int32 icon);

    /**
     * A variant of {@link #setSmallIcon(int) setSmallIcon(int)} that takes an additional
     * level parameter for when the icon is a {@link android.graphics.drawable.LevelListDrawable
     * LevelListDrawable}.
     *
     * @param icon A resource ID in the application's package of the drawable to use.
     * @param level The level to use for the icon.
     *
     * @see Notification#icon
     * @see Notification#iconLevel
     */
    CARAPI SetSmallIcon(
        /* [in] */ Int32 icon,
        /* [in] */ Int32 level);

    /**
     * Set the first line of text in the platform notification template.
     */
    CARAPI SetContentTitle(
        /* [in] */ ICharSequence * title);

    /**
     * Set the second line of text in the platform notification template.
     */
    CARAPI SetContentText(
        /* [in] */ ICharSequence * text);

    /**
     * Set the third line of text in the platform notification template.
     * Don't use if you're also using {@link #setProgress(int, int, boolean)}; they occupy the same location in the standard template.
     */
    CARAPI SetSubText(
        /* [in] */ ICharSequence * text);

    /**
     * Set the large number at the right-hand side of the notification.  This is
     * equivalent to setContentInfo, although it might show the number in a different
     * font size for readability.
     */
    CARAPI SetNumber(
        /* [in] */ Int32 number);

    /**
     * A small piece of additional information pertaining to this notification.
     *
     * The platform template will draw this on the last line of the notification, at the far
     * right (to the right of a smallIcon if it has been placed there).
     */
    CARAPI SetContentInfo(
        /* [in] */ ICharSequence * info);

    /**
     * Set the progress this notification represents.
     *
     * The platform template will represent this using a {@link ProgressBar}.
     */
    CARAPI SetProgress(
        /* [in] */ Int32 max,
        /* [in] */ Int32 progress,
        /* [in] */ Boolean indeterminate);

    /**
     * Supply a custom RemoteViews to use instead of the platform template.
     *
     * @see Notification#contentView
     */
    CARAPI SetContent(
        /* [in] */ IRemoteViews * views);

    /**
     * Supply a {@link PendingIntent} to be sent when the notification is clicked.
     *
     * As of {@link android.os.Build.VERSION_CODES#HONEYCOMB}, if this field is unset and you
     * have specified a custom RemoteViews with {@link #setContent(RemoteViews)}, you can use
     * {@link RemoteViews#setOnClickPendingIntent RemoteViews.setOnClickPendingIntent(int,PendingIntent)}
     * to assign PendingIntents to individual views in that custom layout (i.e., to create
     * clickable buttons inside the notification view).
     *
     * @see Notification#contentIntent Notification.contentIntent
     */
    CARAPI SetContentIntent(
        /* [in] */ IPendingIntent * intent);

    /**
     * Supply a {@link PendingIntent} to send when the notification is cleared explicitly by the user.
     *
     * @see Notification#deleteIntent
     */
    CARAPI SetDeleteIntent(
        /* [in] */ IPendingIntent * intent);

    /**
     * An intent to launch instead of posting the notification to the status bar.
     * Only for use with extremely high-priority notifications demanding the user's
     * <strong>immediate</strong> attention, such as an incoming phone call or
     * alarm clock that the user has explicitly set to a particular time.
     * If this facility is used for something else, please give the user an option
     * to turn it off and use a normal notification, as this can be extremely
     * disruptive.
     *
     * @param intent The pending intent to launch.
     * @param highPriority Passing true will cause this notification to be sent
     *          even if other notifications are suppressed.
     *
     * @see Notification#fullScreenIntent
     */
    CARAPI SetFullScreenIntent(
        /* [in] */ IPendingIntent * intent,
        /* [in] */ Boolean highPriority);

    /**
     * Set the "ticker" text which is displayed in the status bar when the notification first
     * arrives.
     *
     * @see Notification#tickerText
     */
    CARAPI SetTicker(
        /* [in] */ ICharSequence * tickerText);

    /**
     * Set the text that is displayed in the status bar when the notification first
     * arrives, and also a RemoteViews object that may be displayed instead on some
     * devices.
     *
     * @see Notification#tickerText
     * @see Notification#tickerView
     */
    CARAPI SetTicker(
        /* [in] */ ICharSequence * tickerText,
        /* [in] */ IRemoteViews * views);

    /**
     * Add a large icon to the notification (and the ticker on some devices).
     *
     * In the platform template, this image will be shown on the left of the notification view
     * in place of the {@link #setSmallIcon(int) small icon} (which will move to the right side).
     *
     * @see Notification#largeIcon
     */
    CARAPI SetLargeIcon(
        /* [in] */ IBitmap * icon);

    /**
     * Set the sound to play.
     *
     * It will be played on the {@link #STREAM_DEFAULT default stream} for notifications.
     *
     * @see Notification#sound
     */
    CARAPI SetSound(
        /* [in] */ IUri * sound);

    /**
     * Set the sound to play, along with a specific stream on which to play it.
     *
     * See {@link android.media.AudioManager} for the <code>STREAM_</code> constants.
     *
     * @see Notification#sound
     */
    CARAPI SetSound(
        /* [in] */ IUri * sound,
        /* [in] */ Int32 streamType);

    /**
     * Set the sound to play, along with specific {@link AudioAttributes audio attributes} to
     * use during playback.
     *
     * <p>
     * A notification that is noisy is more likely to be presented as a heads-up notification.
     * </p>
     *
     * @see Notification#sound
     */
    CARAPI SetSound(
    /* [in] */ IUri *sound,
    /* [in] */ IAudioAttributes* audioAttributes);

    /**
     * Set the vibration pattern to use.
     *

     * See {@link android.os.Vibrator#vibrate(long[], int)} for a discussion of the
     * <code>pattern</code> parameter.
     *

     * @see Notification#vibrate
     */
    CARAPI SetVibrate(
        /* [in] */ ArrayOf<Int64> *pattern);

    /**
     * Set the desired color for the indicator LED on the device, as well as the
     * blink duty cycle (specified in milliseconds).
     *

     * Not all devices will honor all (or even any) of these values.
     *

     * @see Notification#ledARGB
     * @see Notification#ledOnMS
     * @see Notification#ledOffMS
     */
    CARAPI SetLights(
        /* [in] */ Int32 argb,
        /* [in] */ Int32 onMs,
        /* [in] */ Int32 offMs);

    /**
     * Set whether this is an "ongoing" notification.
     *

     * Ongoing notifications cannot be dismissed by the user, so your application or service
     * must take care of canceling them.
     *

     * They are typically used to indicate a background task that the user is actively engaged
     * with (e.g., playing music) or is pending in some way and therefore occupying the device
     * (e.g., a file download, sync operation, active network connection).
     *

     * @see Notification#FLAG_ONGOING_EVENT
     * @see Service#setForeground(boolean)
     */
    CARAPI SetOngoing(
        /* [in] */ Boolean ongoing);

    /**
     * Set this flag if you would only like the sound, vibrate
     * and ticker to be played if the notification is not already showing.
     *
     * @see Notification#FLAG_ONLY_ALERT_ONCE
     */
    CARAPI SetOnlyAlertOnce(
        /* [in] */ Boolean onlyAlertOnce);

    /**
     * Make this notification automatically dismissed when the user touches it. The
     * PendingIntent set with {@link #setDeleteIntent} will be sent when this happens.
     *
     * @see Notification#FLAG_AUTO_CANCEL
     */
    CARAPI SetAutoCancel(
        /* [in] */ Boolean autoCancel);

    /**
     * Set whether or not this notification should not bridge to other devices.
     *
     * <p>Some notifications can be bridged to other devices for remote display.
     * This hint can be set to recommend this notification not be bridged.
     */
    CARAPI SetLocalOnly(
        /* [in] */ Boolean localOnly);

    /**
     * Set which notification properties will be inherited from system defaults.
     * <p>
     * The value should be one or more of the following fields combined with
     * bitwise-or:
     * {@link #DEFAULT_SOUND}, {@link #DEFAULT_VIBRATE}, {@link #DEFAULT_LIGHTS}.
     * <p>
     * For all default values, use {@link #DEFAULT_ALL}.
     */
    CARAPI SetDefaults(
        /* [in] */ Int32 defaults);

    /**
     * Set the priority of this notification.
     *
     * @see Notification#priority
     */
    CARAPI SetPriority(
        /* [in] */ Int32 pri);

    /**
     * Set the notification category.
     *
     * @see Notification#category
     */
    CARAPI SetCategory(
        /* [in] */ const String& category);

    /**
     * Add a person that is relevant to this notification.
     *
     * <P>
     * Depending on user preferences, this annotation may allow the notification to pass
     * through interruption filters, and to appear more prominently in the user interface.
     * </P>
     *
     * <P>
     * The person should be specified by the {@code String} representation of a
     * {@link android.provider.ContactsContract.Contacts#CONTENT_LOOKUP_URI}.
     * </P>
     *
     * <P>The system will also attempt to resolve {@code mailto:} and {@code tel:} schema
     * URIs.  The path part of these URIs must exist in the contacts database, in the
     * appropriate column, or the reference will be discarded as invalid. Telephone schema
     * URIs will be resolved by {@link android.provider.ContactsContract.PhoneLookup}.
     * </P>
     *
     * @param uri A URI for the person.
     * @see Notification#EXTRA_PEOPLE
     */
    CARAPI AddPerson(
        /* [in] */ const String& person);

    /**
     * Set this notification to be part of a group of notifications sharing the same key.
     * Grouped notifications may display in a cluster or stack on devices which
     * support such rendering.
     *
     * <p>To make this notification the summary for its group, also call
     * {@link #setGroupSummary}. A sort order can be specified for group members by using
     * {@link #setSortKey}.
     * @param groupKey The group key of the group.
     * @return this object for method chaining
     */
    CARAPI SetGroup(
        /* [in] */ const String& groupKey);

    /**
     * Set this notification to be the group summary for a group of notifications.
     * Grouped notifications may display in a cluster or stack on devices which
     * support such rendering. Requires a group key also be set using {@link #setGroup}.
     * @param isGroupSummary Whether this notification should be a group summary.
     * @return this object for method chaining
     */
    CARAPI SetGroupSummary(
        /* [in] */ Boolean isGroupSummary);

    /**
     * Set a sort key that orders this notification among other notifications from the
     * same package. This can be useful if an external sort was already applied and an app
     * would like to preserve this. Notifications will be sorted lexicographically using this
     * value, although providing different priorities in addition to providing sort key may
     * cause this value to be ignored.
     *
     * <p>This sort key can also be used to order members of a notification group. See
     * {@link #setGroup}.
     *
     * @see String#compareTo(String)
     */
     CARAPI SetSortKey(
        /* [in] */ const String& groupKey);

    /**
     * Merge additional metadata into this notification.
     *
     * <p>Values within the Bundle will replace existing extras values in this Builder.
     *
     * @see Notification#extras
     */
    CARAPI AddExtras(
        /* [in] */ IBundle* extras);

    /**
     * Add metadata to this notification.
     *
     * A reference to the Bundle is held for the lifetime of this Builder, and the Bundle's
     * current contents are copied into the Notification each time {@link #build()} is
     * called.
     *
     * @see Notification#extras
     * @hide
     */
    CARAPI SetExtras(
        /* [in] */ IBundle* extras);

    /**
     * Get the current metadata Bundle used by this notification Builder.
     *
     * <p>The returned Bundle is shared with this Builder.
     *
     * <p>The current contents of this Bundle are copied into the Notification each time
     * {@link #build()} is called.
     *
     * @see Notification#extras
     */
    CARAPI GetExtras(
        /* [out] */ IBundle** extras);

    /**
     * Add an action to this notification. Actions are typically displayed by
     * the system as a button adjacent to the notification content.
     *
     * @param icon Resource ID of a drawable that represents the action.
     * @param title Text describing the action.
     * @param intent PendingIntent to be fired when the action is invoked.
     */
    CARAPI AddAction(
        /* [in] */ Int32 icon,
        /* [in] */ ICharSequence * title,
        /* [in] */ IPendingIntent * intent);

    /**
     * Add an action to this notification. Actions are typically displayed by
     * the system as a button adjacent to the notification content.
     * <p>
     * Every action must have an icon (32dp square and matching the
     * <a href="{@docRoot}design/style/iconography.html#action-bar">Holo
     * Dark action bar</a> visual style), a textual label, and a {@link PendingIntent}.
     * <p>
     * A notification in its expanded form can display up to 3 actions, from left to right in
     * the order they were added. Actions will not be displayed when the notification is
     * collapsed, however, so be sure that any essential functions may be accessed by the user
     * in some other way (for example, in the Activity pointed to by {@link #contentIntent}).
     *
     * @param action The action to add.
     */
    CARAPI AddAction(
        /* [in] */ INotificationAction* action);

    /**
     * Add a rich notification style to be applied at build time.
     *
     * @param style Object responsible for modifying the notification style.
     */
    CARAPI SetStyle(
        /* [in] */ INotificationStyle * style);

    /**
     * Specify the value of {@link #visibility}.
     *
     * @param visibility One of {@link #VISIBILITY_PRIVATE} (the default),
     * {@link #VISIBILITY_SECRET}, or {@link #VISIBILITY_PUBLIC}.
     *
     * @return The same Builder.
     */
    CARAPI SetVisibility(
        /* [in] */ Int32 visibility);

    /**
     * Supply a replacement Notification whose contents should be shown in insecure contexts
     * (i.e. atop the secure lockscreen). See {@link #visibility} and {@link #VISIBILITY_PUBLIC}.
     * @param n A replacement notification, presumably with some or all info redacted.
     * @return The same Builder.
     */
    CARAPI SetPublicVersion(
        /* [in] */ INotification* n);

    /**
     * Apply an extender to this notification builder. Extenders may be used to add
     * metadata or change options on this builder.
     */
    CARAPI Extend(
        /* [in] */ INotificationExtender* extender);

    /**
     * Sets {@link Notification#color}.
     *
     * @param argb The accent color to use
     *
     * @return The same Builder.
     */
    CARAPI SetColor(
        /* [in] */ Int32 argb);

    static Int32 CalculateTopPadding(
        /* [in] */ IContext* ctx,
        /* [in] */ Boolean hasThreeLines,
        /* [in] */ Float fontScale);

    /**
     * @deprecated Use {@link #build()} instead.
     */
    //@Deprecated
    CARAPI GetNotification(
        /* [out] */ INotification **notification);

    /**
     * Combine all of the options that have been set and return a new {@link Notification}
     * object.
     */
    CARAPI Build(
        /* [out] */ INotification **notification);

    /**
     * Apply the unstyled operations and return a new {@link Notification} object.
     */
    CARAPI BuildUnstyled(
        /* [out] */ INotification **notification);

    /**
     * Capture, in the provided bundle, semantic information used in the construction of
     * this Notification object.
     * @hide
     */
    CARAPI PopulateExtras(
        /* [in] */ IBundle* extras);

    /**
     * @hide
     */
    static CARAPI StripForDelivery(
        /* [in] */ INotification* n);

    /**
     * @hide
     */
    static AutoPtr<INotification> Rebuild(
        /* [in] */ IContext* context,
        /* [in] */ INotification* n);

    /**
     * Rebuilds the notification passed in to the rebuild-constructor
     * {@link #Builder(Context, Notification)}.
     *
     * <p>
     * Throws IllegalStateException when invoked on a Builder that isn't in rebuild mode.
     *
     * @hide
     */
    CARAPI Rebuild(
        /* [out] */ INotification** result);

    static AutoPtr<IClassInfo> GetNotificationStyleClass(
        /* [in] */ const String& templateClass);

    CARAPI SetBuilderContentView(
        /* [in] */ INotification* n,
        /* [in] */ IRemoteViews* contentView);

    CARAPI SetBuilderBigContentView(
        /* [in] */ INotification* n,
        /* [in] */ IRemoteViews* bigContentView);

    CARAPI SetBuilderHeadsUpContentView(
        /* [in] */ INotification* n,
        /* [in] */ IRemoteViews* headsUpContentView);

    CARAPI RestoreFromNotification(
        /* [in] */ INotification* n);

    CARAPI GetSubText(
        /* [out] */ ICharSequence** text);

    CARAPI GetContentText(
        /* [out] */ ICharSequence** text);

    CARAPI ApplyStandardTemplateWithActions(
        /* [in] */ Int32 layoutId,
        /* [out] */ IRemoteViews** remoteViews);

    /**
     * Apply this Builder to an existing {@link Notification} object.
     *
     * @hide
     */
    CARAPI BuildInto(
        /* [in] */ INotification* n);

    CARAPI_(AutoPtr<ICharSequence>) ProcessLegacyText(
        /* [in] */ ICharSequence* charSequence);

    Int32 GetBaseLayoutResource();

    Int32 GetBigBaseLayoutResource();

    Int32 GetBigPictureLayoutResource();

    Int32 GetBigTextLayoutResource();

    Int32 GetInboxLayoutResource();

    Int32 GetActionLayoutResource();

    Int32 GetActionTombstoneLayoutResource();

    CARAPI_(Boolean) AddProfileBadge(
        /* [in] */ IRemoteViews* contentView,
        /* [in] */ Int32 resId);

    CARAPI ShrinkLine3Text(
        /* [in] */ IRemoteViews* contentView);

    CARAPI_(AutoPtr<IRemoteViews>) ApplyStandardTemplate(
        /* [in] */ Int32 resId);

    CARAPI_(AutoPtr<IRemoteViews>) ApplyStandardTemplate(
        /* [in] */ Int32 resId,
        /* [in] */ Boolean hasProgress);

    /**
     * @return true if the built notification will show the time or the chronometer; false
     *         otherwise
     */
    CARAPI_(Boolean) ShowsTimeOrChronometer();

private:
    CARAPI_(void) SetFlag(
        /* [in] */ Int32 mask,
        /* [in] */ Boolean value);

    CARAPI_(AutoPtr<IDrawable>) GetProfileBadgeDrawable();

    CARAPI_(AutoPtr<IBitmap>) GetProfileBadge();

    CARAPI UnshrinkLine3Text(
        /* [in] */ IRemoteViews* contentView);

    CARAPI ResetStandardTemplate(
        /* [in] */ IRemoteViews* contentView);

    /**
     * Logic to find out whether the notification is going to have three lines in the contracted
     * layout. This is used to adjust the top padding.
     *
     * @return true if the notification is going to have three lines; false if the notification
     *         is going to have one or two lines
     */
    CARAPI_(Boolean) HasThreeLines();

    CARAPI ResetStandardTemplateWithActions(
        /* [in] */ IRemoteViews* big);

    CARAPI_(AutoPtr<IRemoteViews>) MakeContentView();

    CARAPI_(AutoPtr<IRemoteViews>) MakeTickerView();

    CARAPI_(AutoPtr<IRemoteViews>) MakeBigContentView();

    CARAPI_(AutoPtr<IRemoteViews>) MakeHeadsUpContentView();

    CARAPI_(AutoPtr<IRemoteViews>) GenerateActionButton(
        /* [in] */ INotificationAction * action);

    /**
     * @return Whether we are currently building a notification from a legacy (an app that
     *         doesn't create material notifications by itself) app.
     */
    CARAPI_(Boolean) IsLegacy();

    CARAPI ProcessLegacyAction(
        /* [in] */ INotificationAction* action,
        /* [in] */ IRemoteViews* button);

    /**
     * Apply any necessary background to smallIcons being used in the largeIcon spot.
     */
    CARAPI ProcessSmallIconAsLarge(
        /* [in] */ Int32 largeIconId,
        /* [in] */ IRemoteViews* contentView);

    /**
     * Apply any necessary background to a largeIcon if it's a fake smallIcon (that is,
     * if it's grayscale).
     */
    // TODO: also check bounds, transparency, that sort of thing.
    CARAPI ProcessLargeLegacyIcon(
        /* [in] */ IBitmap* largeIcon,
        /* [in] */ IRemoteViews* contentView);

    /**
     * Add a colored circle behind the largeIcon slot.
     */
    CARAPI ApplyLargeIconBackground(
        /* [in] */ IRemoteViews* contentView);

    CARAPI RemoveLargeIconBackground(
        /* [in] */ IRemoteViews* contentView);

    /**
     * Recolor small icons when used in the R.id.right_icon slot.
     */
    CARAPI ProcessSmallRightIcon(
        /* [in] */ Int32 smallIconDrawableId,
        /* [in] */ IRemoteViews* contentView);

    CARAPI_(Int32) SanitizeColor();

    CARAPI_(Int32) ResolveColor();

public:
    const static Int32 MAX_ACTION_BUTTONS;
    const static Float LARGE_TEXT_SCALE;// = 1.3f;
    const static Boolean DBG;

    /**
     * The ApplicationInfo of the package that created the notification, used to create
     * a context to rebuild the notification via a Builder.
     * @hide
     */
    static const String EXTRA_REBUILD_CONTEXT_APPLICATION_INFO;
        // = "android.rebuild.applicationInfo";

    // Whether to enable stripping (at post time) & rebuilding (at listener receive time) of
    // memory intensive resources.
    const static Boolean STRIP_AND_REBUILD;// = true;

    AutoPtr<IContext> mContext;

    Int64 mWhen;
    Int32 mSmallIcon;
    Int32 mSmallIconLevel;
    Int32 mNumber;
    AutoPtr<ICharSequence> mContentTitle;
    AutoPtr<ICharSequence> mContentText;
    AutoPtr<ICharSequence> mContentInfo;
    AutoPtr<ICharSequence> mSubText;
    AutoPtr<IPendingIntent> mContentIntent;
    AutoPtr<IRemoteViews> mContentView;
    AutoPtr<IPendingIntent> mDeleteIntent;
    AutoPtr<IPendingIntent> mFullScreenIntent;
    AutoPtr<ICharSequence> mTickerText;
    AutoPtr<IRemoteViews> mTickerView;
    AutoPtr<IBitmap> mLargeIcon;
    AutoPtr<IUri> mSound;
    Int32 mAudioStreamType;
    AutoPtr<IAudioAttributes> mAudioAttributes;
    AutoPtr<ArrayOf<Int64> > mVibrate;
    Int32 mLedArgb;
    Int32 mLedOnMs;
    Int32 mLedOffMs;
    Int32 mDefaults;
    Int32 mFlags;
    Int32 mProgressMax;
    Int32 mProgress;
    Boolean mProgressIndeterminate;
    String mCategory;
    String mGroupKey;
    String mSortKey;
    AutoPtr<IBundle> mExtras;
    Int32 mPriority;
    List<AutoPtr<INotificationAction> > mActions;
    Boolean mUseChronometer;
    AutoPtr<INotificationStyle> mStyle;
    Boolean mShowWhen;

    Int32 mVisibility;// = VISIBILITY_PRIVATE;
    AutoPtr<INotification> mPublicVersion;// = null;
    AutoPtr<INotificationColorUtil> mColorUtil;
    List<String> mPeople;   // ArrayList<String>
    Int32 mColor;// = COLOR_DEFAULT;

    /**
     * The user that built the notification originally.
     */
    Int32 mOriginatingUserId;

    /**
     * Contains extras related to rebuilding during the build phase.
     */
    AutoPtr<IBundle> mRebuildBundle;// = new Bundle();
    /**
     * Contains the notification to rebuild when this Builder is in "rebuild" mode.
     * Null otherwise.
     */
    AutoPtr<INotification> mRebuildNotification;// = null;

    /**
     * Whether the build notification has three lines. This is used to make the top padding for
     * both the contracted and expanded layout consistent.
     *
     * <p>
     * This field is only valid during the build phase.
     */
    Boolean mHasThreeLines;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CNOTIFICATIONBUILDER_H__
