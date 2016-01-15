
#ifndef __ELASTOS_DROID_APP_CNOTIFICATIONMEDIASTYLE_H__
#define __ELASTOS_DROID_APP_CNOTIFICATIONMEDIASTYLE_H__

#include "_Elastos_Droid_App_CNotificationMediaStyle.h"
#include "elastos/droid/app/NotificationStyle.h"

using Elastos::Droid::Media::Session::IMediaSessionToken;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Notification style for media playback notifications.
 *
 * In the expanded form, {@link Notification#bigContentView}, up to 5
 * {@link Notification.Action}s specified with
 * {@link Notification.Builder#addAction(int, CharSequence, PendingIntent) addAction} will be
 * shown as icon-only pushbuttons, suitable for transport controls. The Bitmap given to
 * {@link Notification.Builder#setLargeIcon(android.graphics.Bitmap) setLargeIcon()} will be
 * treated as album artwork.
 *
 * Unlike the other styles provided here, MediaStyle can also modify the standard-size
 * {@link Notification#contentView}; by providing action indices to
 * {@link #setShowActionsInCompactView(int...)} you can promote up to 2 actions to be displayed
 * in the standard view alongside the usual content.
 *
 * Notifications created with MediaStyle will have their category set to
 * {@link Notification#CATEGORY_TRANSPORT CATEGORY_TRANSPORT} unless you set a different
 * category using {@link Notification.Builder#setCategory(String) setCategory()}.
 *
 * Finally, if you attach a {@link android.media.session.MediaSession.Token} using
 * {@link android.app.Notification.MediaStyle#setMediaSession(MediaSession.Token)},
 * the System UI can identify this as a notification representing an active media session
 * and respond accordingly (by showing album artwork in the lockscreen, for example).
 *
 * To use this style with your Notification, feed it to
 * {@link Notification.Builder#setStyle(android.app.Notification.Style)} like so:
 * <pre class="prettyprint">
 * Notification noti = new Notification.Builder()
 *     .setSmallIcon(R.drawable.ic_stat_player)
 *     .setContentTitle(&quot;Track title&quot;)     // these three lines are optional
 *     .setContentText(&quot;Artist - Album&quot;)   // if you use
 *     .setLargeIcon(albumArtBitmap))      // setMediaSession(token)
 *     .setStyle(<b>new Notification.MediaStyle()</b>
 *         .setMediaSession(mySession))
 *     .build();
 * </pre>
 *
 * @see Notification#bigContentView
 */
CarClass(CNotificationMediaStyle)
    , public NotificationStyle
    , public INotificationMediaStyle
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CNotificationMediaStyle();

    virtual ~CNotificationMediaStyle();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ INotificationBuilder* builder);

    /**
     * Request up to 3 actions (by index in the order of addition) to be shown in the compact
     * notification view.
     *
     * @param actions the indices of the actions to show in the compact notification view
     */
    CARAPI SetShowActionsInCompactView(
        /* [in] */ ArrayOf<Int32>* actions);

    /**
     * Attach a {@link android.media.session.MediaSession.Token} to this Notification
     * to provide additional playback information and control to the SystemUI.
     */
    CARAPI SetMediaSession(
        /* [in] */ IMediaSessionToken* token);

    /**
     * @hide
     */
    //@Override
    CARAPI BuildStyled(
        /* [in] */ INotification* wip);

    /**
     * @hide
     */
    //@Override
    CARAPI PopulateContentView(
        /* [in] */ INotification* wip);

    /**
     * @hide
     */
    //@Override
    CARAPI PopulateBigContentView(
        /* [in] */ INotification* wip);

    /** @hide */
    //@Override
    CARAPI AddExtras(
        /* [in] */ IBundle* extras);

protected:
    /**
     * @hide
     */
    //@Override
    CARAPI RestoreFromExtras(
        /* [in] */ IBundle* extras);

    /**
     * @hide
     */
    //@Override
    Boolean HasProgress();

private:
    AutoPtr<IRemoteViews> GenerateMediaActionButton(
        /* [in] */ INotificationAction* action);

    AutoPtr<IRemoteViews> MakeMediaContentView();

    AutoPtr<IRemoteViews> MakeMediaBigContentView();

    Int32 GetBigLayoutResource(
        /* [in] */ Int32 actionCount);

    void HideRightIcon(
        /* [in] */ IRemoteViews* contentView);

    /**
     * Applies the special text colors for media notifications to all text views.
     */
    void StyleText(
        /* [in] */ IRemoteViews* contentView);

private:
    static const Int32 MAX_MEDIA_BUTTONS_IN_COMPACT;
    static const Int32 MAX_MEDIA_BUTTONS;

    AutoPtr<ArrayOf<Int32> > mActionsToShowInCompact;
    AutoPtr<IMediaSessionToken> mToken;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CNOTIFICATIONMEDIASTYLE_H__

