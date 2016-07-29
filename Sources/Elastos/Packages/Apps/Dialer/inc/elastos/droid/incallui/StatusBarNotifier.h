
#ifndef __ELASTOS_DROID_INCALLUI_STATUSBARNOTIFIER_H__
#define __ELASTOS_DROID_INCALLUI_STATUSBARNOTIFIER_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/incallui/Call.h"
#include "elastos/droid/incallui/CallList.h"
#include "elastos/droid/incallui/ContactInfoCache.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::INotificationBuilder;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Os::IHandlerCallback;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InCallUI {

/**
 * This class adds Notifications to the status bar for the in-call experience.
 */
class StatusBarNotifier
    : public Object
    , public IInCallStateListener
{
private:
    class NotificationTimer : public Object
    {
    public:
        enum State
        {
            SCHEDULED,
            FIRED,
            CLEAR,
        };

        virtual CARAPI_(State) GetState() = 0;

        virtual CARAPI_(void) Schedule() = 0;

        virtual CARAPI_(void) Clear() = 0;
    };

    class MyNotificationTimer : public NotificationTimer
    {
    private:
        class MyHandlerCallback
            : public Object
            , public IHandlerCallback
        {
        public:
            MyHandlerCallback(
                /* [in] */ MyNotificationTimer* host)
                : mHost(host)
            {}

            CAR_INTERFACE_DECL()

            CARAPI HandleMessage(
                /* [in] */ IMessage* msg,
                /* [out] */ Boolean* result);

        private:
            MyNotificationTimer* mHost;
        };

    public:
        MyNotificationTimer(
            /* [in] */ StatusBarNotifier* host);

        CARAPI_(State) GetState();

        CARAPI_(void) Schedule();

        CARAPI_(void) Clear();

    private:
        CARAPI_(void) Fire();

    private:
        AutoPtr<IHandler> mHandler;
        State mState;

        StatusBarNotifier* mHost;
    };

    class MyContactInfoCacheCallback
        : public Object
        , public IContactInfoCacheCallback
    {
    public:
        MyContactInfoCacheCallback(
            /* [in] */ StatusBarNotifier* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnContactInfoComplete(
            /* [in] */ const String& callId,
            /* [in] */ IContactCacheEntry* entry);

        CARAPI OnImageLoadComplete(
            /* [in] */ const String& callId,
            /* [in] */ IContactCacheEntry* entry);

    private:
        StatusBarNotifier* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    StatusBarNotifier(
        /* [in] */ IContext* context,
        /* [in] */ ContactInfoCache* contactInfoCache);

    /**
     * Creates notifications according to the state we receive from {@link InCallPresenter}.
     */
    // @Override
    CARAPI OnStateChange(
        /* [in] */ InCallState oldState,
        /* [in] */ InCallState newState,
        /* [in] */ ICallList* callList);

    /**
     * Updates the phone app's status bar notification *and* launches the
     * incoming call UI in response to a new incoming call.
     *
     * If an incoming call is ringing (or call-waiting), the notification
     * will also include a "fullScreenIntent" that will cause the
     * InCallScreen to be launched, unless the current foreground activity
     * is marked as "immersive".
     *
     * (This is the mechanism that actually brings up the incoming call UI
     * when we receive a "new ringing connection" event from the telephony
     * layer.)
     *
     * Also note that this method is safe to call even if the phone isn't
     * actually ringing (or, more likely, if an incoming call *was*
     * ringing briefly but then disconnected).  In that case, we'll simply
     * update or cancel the in-call notification based on the current
     * phone state.
     *
     * @see #updateInCallNotification(InCallState,CallList)
     */
    CARAPI_(void) UpdateNotification(
        /* [in] */ InCallState state,
        /* [in] */ CallList* callList);

    static CARAPI_(void) ClearInCallNotification(
        /* [in] */ IContext* backupContext);

private:
    /**
     * Take down the in-call notification.
     * @see #updateInCallNotification(InCallState,CallList)
     */
    CARAPI_(void) CancelInCall();

    /**
     * Helper method for updateInCallNotification() and
     * updateNotification(): Update the phone app's
     * status bar notification based on the current telephony state, or
     * cancels the notification if the phone is totally idle.
     */
    CARAPI_(void) UpdateInCallNotification(
        /* [in] */ InCallState state,
        /* [in] */ CallList* callList);

    CARAPI_(void) ShowNotification(
        /* [in] */ Call* call);

    /**
     * Sets up the main Ui for the notification
     */
    CARAPI_(void) BuildAndSendNotification(
        /* [in] */ Call* originalCall,
        /* [in] */ ContactInfoCache::ContactCacheEntry* contactInfo);

    CARAPI_(void) CreateIncomingCallNotification(
        /* [in] */ Call* call,
        /* [in] */ Int32 state,
        /* [in] */ INotificationBuilder* builder);

    /**
     * Checks the new notification data and compares it against any notification that we
     * are already displaying. If the data is exactly the same, we return false so that
     * we do not issue a new notification for the exact same data.
     */
    CARAPI_(Boolean) CheckForChangeAndSaveData(
        /* [in] */ Int32 icon,
        /* [in] */ Int32 content,
        /* [in] */ IBitmap* largeIcon,
        /* [in] */ const String& contentTitle,
        /* [in] */ Int32 state);

    /**
     * Returns the main string to use in the notification.
     */
    CARAPI_(String) GetContentTitle(
        /* [in] */ ContactInfoCache::ContactCacheEntry* contactInfo,
        /* [in] */ Boolean isConference);

    CARAPI_(void) AddPersonReference(
        /* [in] */ INotificationBuilder* builder,
        /* [in] */ ContactInfoCache::ContactCacheEntry* contactInfo,
        /* [in] */ Call* call);

    /**
     * Gets a large icon from the contact info object to display in the notification.
     */
    CARAPI_(AutoPtr<IBitmap>) GetLargeIconToDisplay(
        /* [in] */ ContactInfoCache::ContactCacheEntry* contactInfo,
        /* [in] */ Boolean isConference);

    /**
     * Returns the appropriate icon res Id to display based on the call for which
     * we want to display information.
     */
    CARAPI_(Int32) GetIconToDisplay(
        /* [in] */ Call* call);

    /**
     * Returns the message to use with the notification.
     */
    CARAPI_(Int32) GetContentString(
        /* [in] */ Call* call);

    /**
     * Gets the most relevant call to display in the notification.
     */
    CARAPI_(AutoPtr<Call>) GetCallToShow(
        /* [in] */ CallList* callList);

    CARAPI_(void) AddAnswerAction(
        /* [in] */ INotificationBuilder* builder);

    CARAPI_(void) AddDismissAction(
        /* [in] */ INotificationBuilder* builder);

    CARAPI_(void) AddHangupAction(
        /* [in] */ INotificationBuilder* builder);

    CARAPI_(void) AddVideoCallAction(
        /* [in] */ INotificationBuilder* builder);

    CARAPI_(void) AddVoiceAction(
        /* [in] */ INotificationBuilder* builder);

    CARAPI_(void) AddAcceptUpgradeRequestAction(
        /* [in] */ INotificationBuilder* builder);

    CARAPI_(void) AddDismissUpgradeRequestAction(
        /* [in] */ INotificationBuilder* builder);

    /**
     * Adds fullscreen intent to the builder.
     */
    CARAPI_(void) ConfigureFullScreenIntent(
        /* [in] */ INotificationBuilder* builder,
        /* [in] */ IPendingIntent* intent,
        /* [in] */ Call* call);

    CARAPI_(AutoPtr<INotificationBuilder>) GetNotificationBuilder();

    CARAPI_(AutoPtr<IPendingIntent>) CreateLaunchPendingIntent();

    /**
     * Returns PendingIntent for answering a phone call. This will typically be used from
     * Notification context.
     */
    static CARAPI_(AutoPtr<IPendingIntent>) CreateNotificationPendingIntent(
        /* [in] */ IContext* context,
        /* [in] */ const String& action);

private:
    // notification types
    static const Int32 IN_CALL_NOTIFICATION = 1;

    static const Int64 IN_CALL_TIMEOUT = 1000L;

    AutoPtr<MyNotificationTimer> mNotificationTimer;

    AutoPtr<IContext> mContext;
    AutoPtr<ContactInfoCache> mContactInfoCache;
    AutoPtr<INotificationManager> mNotificationManager;
    Boolean mIsShowingNotification;
    Int32 mCallState;
    Int32 mSavedIcon;
    Int32 mSavedContent;
    AutoPtr<IBitmap> mSavedLargeIcon;
    String mSavedContentTitle;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_STATUSBARNOTIFIER_H__
