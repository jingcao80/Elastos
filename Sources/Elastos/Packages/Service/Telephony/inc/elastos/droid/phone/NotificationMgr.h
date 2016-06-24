#ifndef  __ELASTOS_DROID_PHONE_NOTIFICATIONMGR_H__
#define  __ELASTOS_DROID_PHONE_NOTIFICATIONMGR_H__

#include "_Elastos.Droid.Server.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/Object.h>
#include <Elastos.CoreLibrary.Core.h>

using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Widget::IToast;
using Elastos::Core::Object;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * NotificationManager-related utility code for the Phone app.
 *
 * This is a singleton object which acts as the interface to the
 * framework's NotificationManager, and is used to display status bar
 * icons and control other status bar-related behavior.
 *
 * @see PhoneGlobals.notificationMgr
 */
class NotificationMgr
    : public Object
{
public:
    /**
     * Helper class that's a wrapper around the framework's
     * StatusBarManager.disable() API.
     *
     * This class is used to control features like:
     *
     *   - Disabling the status bar "notification windowshade"
     *     while the in-call UI is up
     *
     *   - Disabling notification alerts (audible or vibrating)
     *     while a phone call is active
     *
     *   - Disabling navigation via the system bar (the "soft buttons" at
     *     the bottom of the screen on devices with no hard buttons)
     *
     * We control these features through a single point of control to make
     * sure that the various StatusBarManager.disable() calls don't
     * interfere with each other.
     */
    class StatusBarHelper
        : public Object
    {
    public:
        /**
         * Enables or disables auditory / vibrational alerts.
         *
         * (We disable these any time a voice call is active, regardless
         * of whether or not the in-call UI is visible.)
         */
        CARAPI EnableNotificationAlerts(
            /* [in] */ Boolean enable);

        /**
         * Enables or disables the expanded view of the status bar
         * (i.e. the ability to pull down the "notification windowshade").
         *
         * (This feature is disabled by the InCallScreen while the in-call
         * UI is active.)
         */
        CARAPI EnableExpandedView(
            /* [in] */ Boolean enable);

        /**
         * Enables or disables the navigation via the system bar (the
         * "soft buttons" at the bottom of the screen)
         *
         * (This feature is disabled while an incoming call is ringing,
         * because it's easy to accidentally touch the system bar while
         * pulling the phone out of your pocket.)
         */
        CARAPI EnableSystemBarNavigation(
            /* [in] */ Boolean enable);

    private:
        StatusBarHelper(
            /* [in] */ NotificationMgr* host);

        /**
         * Updates the status bar to reflect the current desired state.
         */
        CARAPI_(void) UpdateStatusBar();

    private:
        // Current desired state of status bar / system bar behavior
        Boolean mIsNotificationEnabled;
        Boolean mIsExpandedViewEnabled;
        Boolean mIsSystemBarNavigationEnabled;

        NotificationMgr* mHost;
    };

public:
    TO_STRING_IMPL("NotificationMgr")

    /**
     * Initialize the singleton NotificationMgr instance.
     *
     * This is only done once, at startup, from PhoneApp.onCreate().
     * From then on, the NotificationMgr instance is available via the
     * PhoneApp's public "notificationMgr" field, which is why there's no
     * getInstance() method here.
     */
    // static CARAPI_(AutoPtr<NotificationMgr>) Init(
    //     /* [in] */ PhoneGlobals* app);

    /**
     * Updates the message waiting indicator (voicemail) notification.
     *
     * @param visible true if there are messages waiting
     */
    CARAPI UpdateMwi(
        /* [in] */ Boolean visible);

    /**
     * Updates the message call forwarding indicator notification.
     *
     * @param visible true if there are messages waiting
     */
    CARAPI UpdateCfi(
        /* [in] */ Boolean visible);

    /**
     * Shows the "data disconnected due to roaming" notification, which
     * appears when you lose data connectivity because you're roaming and
     * you have the "data roaming" feature turned off.
     */
    CARAPI ShowDataDisconnectedRoaming();

    /**
     * Turns off the "data disconnected due to roaming" notification.
     */
    CARAPI HideDataDisconnectedRoaming();

    /**
     * Update notification about no service of user selected operator
     *
     * @param serviceState Phone service state
     */
    CARAPI UpdateNetworkSelection(
        /* [in] */ Int32 serviceState);

    CARAPI PostTransientNotification(
        /* [in] */ Int32 notifyId,
        /* [in] */ ICharSequence* msg);

private:
    //NotificationMgr(
    //    /* [in] */ PhoneGlobals* app);

    /**
     * Display the network selection "no service" notification
     * @param operator is the numeric operator number
     */
    CARAPI_(void) ShowNetworkSelection(
        /* [in] */ const String& _operator);

    /**
     * Turn off the network selection "no service" notification
     */
    CARAPI_(void) CancelNetworkSelection();

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

public:
    AutoPtr<StatusBarHelper> mStatusBarHelper;

private:
    friend class CNetworkSetting;

    static const String TAG;
    static const Boolean DBG;
    // Do not check in with VDBG = true, since that may write PII to the system log.
    static const Boolean VDBG;

    // notification types
    static const Int32 MMI_NOTIFICATION;
    static const Int32 NETWORK_SELECTION_NOTIFICATION;
    static const Int32 VOICEMAIL_NOTIFICATION;
    static const Int32 CALL_FORWARD_NOTIFICATION;
    static const Int32 DATA_DISCONNECTED_ROAMING_NOTIFICATION;
    static const Int32 SELECTED_OPERATOR_FAIL_NOTIFICATION;

    /** The singleton NotificationMgr instance. */
    static AutoPtr<NotificationMgr> sInstance;

    //AutoPtr<PhoneGlobals> mApp;
    AutoPtr<IPhone> mPhone;

    AutoPtr<IContext> mContext;
    AutoPtr<INotificationManager> mNotificationManager;
    AutoPtr<IStatusBarManager> mStatusBarManager;
    AutoPtr<IUserManager> mUserManager;
    AutoPtr<IToast> mToast;

    // used to track the notification of selected network unavailable
    Boolean mSelectedUnavailableNotify;

    // Retry params for the getVoiceMailNumber() call; see updateMwi().
    static const Int32 MAX_VM_NUMBER_RETRIES;
    static const Int32 VM_NUMBER_RETRY_DELAY_MILLIS;
    Int32 mVmNumberRetriesRemaining;

    /** The projection to use when querying the phones table */
    static const AutoPtr<ArrayOf<String> > PHONES_PROJECTION;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_NOTIFICATIONMGR_H__