#ifndef  __ELASTOS_DROID_PHONE_PHONEGLOBALS_H__
#define  __ELASTOS_DROID_PHONE_PHONEGLOBALS_H__

#include "_Elastos.Droid.Server.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * Global state for the telephony subsystem when running in the primary
 * phone process.
 */
class PhoneGlobals
    : public ContextWrapper
{
public:
    /**
     * Accepts broadcast Intents which will be prepared by {@link NotificationMgr} and thus
     * sent from framework's notification mechanism (which is outside Phone context).
     * This should be visible from outside, but shouldn't be in "exported" state.
     *
     * TODO: If possible merge this into PhoneAppBroadcastReceiver.
     */
    class NotificationBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("PhoneGlobals::NotificationBroadcastReceiver")

        CARAPI constructor(
            /* [in] */ IPhoneGlobals* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        PhoneGlobals* mHost;
    };

private:
    /**
     * Receiver for misc intent broadcasts the Phone app cares about.
     */
    class PhoneAppBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("PhoneGlobals::PhoneAppBroadcastReceiver")

        CARAPI constructor(
            /* [in] */ IPhoneGlobals* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        PhoneGlobals* mHost;
    };

    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("PhoneGlobals::MyHandler")

        MyHandler(
            /* [in] */ PhoneGlobals* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        PhoneGlobals* mHost;
    };

public:
    TO_STRING_IMPL("PhoneGlobals")

    /**
     * Set the restore mute state flag. Used when we are setting the mute state
     * OUTSIDE of user interaction {@link PhoneUtils#startNewCall(Phone)}
     */
    CARAPI SetRestoreMuteOnInCallResume(
        /* [in] */ Boolean mode);

    PhoneGlobals(
        /* [in] */ IContext* context);

    CARAPI OnCreate();

    /**
     * Returns the singleton instance of the PhoneApp.
     */
    static CARAPI GetInstance(
        /* [out] */ PhoneGlobals** global);

    /**
     * Returns the singleton instance of the PhoneApp if running as the
     * primary user, otherwise null.
     */
    static CARAPI_(AutoPtr<PhoneGlobals>) GetInstanceIfPrimary();

    /**
     * Returns the Phone associated with this instance
     */
    static CARAPI_(AutoPtr<IPhone>) GetPhone();

    CARAPI GetBluetoothManager(
        /* [out] */ IBluetoothManager** manager);

    CARAPI GetCallManager(
        /* [out] */ ICallManager** manager);

    /**
     * Returns PendingIntent for hanging up ongoing phone call. This will typically be used from
     * Notification context.
     */
    static CARAPI_(AutoPtr<IPendingIntent>) CreateHangUpOngoingCallPendingIntent(
        /* [in] */ IContext* context);

    CARAPI IsSimPinEnabled(
        /* [out] */ Boolean* result);

    CARAPI AuthenticateAgainstCachedSimPin(
        /* [in] */ const String& pin,
        /* [out] */ Boolean* result);

    CARAPI SetCachedSimPin(
        /* [in] */ const String& pin);

    /**
     * Handles OTASP-related events from the telephony layer.
     *
     * While an OTASP call is active, the CallNotifier forwards
     * OTASP-related telephony events to this method.
     */
    CARAPI HandleOtaspEvent(
        /* [in] */ IMessage* msg);

    /**
     * Similarly, handle the disconnect event of an OTASP call
     * by forwarding it to the OtaUtils instance.
     */
    CARAPI HandleOtaspDisconnect();

    /**
     * Sets the activity responsible for un-PUK-blocking the device
     * so that we may close it when we receive a positive result.
     * mPUKEntryActivity is also used to indicate to the device that
     * we are trying to un-PUK-lock the phone. In other words, iff
     * it is NOT null, then we are trying to unlock and waiting for
     * the SIM to move to READY state.
     *
     * @param activity is the activity to close when PUK has
     * finished unlocking. Can be set to null to indicate the unlock
     * or SIM READYing process is over.
     */
    CARAPI SetPukEntryActivity(
        /* [in] */ IActivity* activity);

    CARAPI GetPUKEntryActivity(
        /* [out] */ IActivity** activity);

    /**
     * Sets the dialog responsible for notifying the user of un-PUK-
     * blocking - SIM READYing progress, so that we may dismiss it
     * when we receive a positive result.
     *
     * @param dialog indicates the progress dialog informing the user
     * of the state of the device.  Dismissed upon completion of
     * READYing process
     */
    CARAPI SetPukEntryProgressDialog(
        /* [in] */ IProgressDialog* dialog);

    CARAPI GetPUKEntryProgressDialog(
        /* [out] */ IProgressDialog** dialog);

    /**
     * Controls whether or not the screen is allowed to sleep.
     *
     * Once sleep is allowed (WakeState is SLEEP), it will rely on the
     * settings for the poke lock to determine when to timeout and let
     * the device sleep {@link PhoneGlobals#setScreenTimeout}.
     *
     * @param ws tells the device to how to wake.
     */
    CARAPI RequestWakeState(
        /* [in] */ WakeState ws);

    /**
     * If we are not currently keeping the screen on, then poke the power
     * manager to wake up the screen for the user activity timeout duration.
     */
    CARAPI WakeUpScreen();

    /**
     * Sets the wake state and screen timeout based on the current state
     * of the phone, and the current state of the in-call UI.
     *
     * This method is a "UI Policy" wrapper around
     * {@link PhoneGlobals#requestWakeState} and {@link PhoneGlobals#setScreenTimeout}.
     *
     * It's safe to call this method regardless of the state of the Phone
     * (e.g. whether or not it's idle), and regardless of the state of the
     * Phone UI (e.g. whether or not the InCallScreen is active.)
     */
    CARAPI UpdateWakeState();

    /**
     * Manually pokes the PowerManager's userActivity method.  Since we
     * set the {@link WindowManager.LayoutParams#INPUT_FEATURE_DISABLE_USER_ACTIVITY}
     * flag while the InCallScreen is active when there is no proximity sensor,
     * we need to do this for touch events that really do count as user activity
     * (like pressing any onscreen UI elements.)
     */
    CARAPI PokeUserActivity();

    /**
     * Notifies the phone app when the phone state changes.
     *
     * This method will updates various states inside Phone app (e.g. update-lock state, etc.)
     */
    CARAPI UpdatePhoneState(
        /* [in] */ IPhoneConstantsState state);

    CARAPI GetPhoneState(
        /* [out] */ IPhoneConstantsState* state);

    CARAPI GetKeyguardManager(
        /* [out] */ IKeyguardManager** manager);

    CARAPI IsOtaCallInActiveState(
        /* [out] */ Boolean* result);

    CARAPI IsOtaCallInEndState(
        /* [out] */ Boolean* result);

    // it is safe to call clearOtaState() even if the InCallScreen isn't active
    CARAPI ClearOtaState();

    // it is safe to call dismissOtaDialogs() even if the InCallScreen isn't active
    CARAPI DismissOtaDialogs();

private:
    CARAPI OnMMIComplete(
        /* [in] */ IAsyncResult* r);

    CARAPI_(void) InitForNewRadioTechnology();

    CARAPI_(void) HandleServiceStateChanged(
        /* [in] */ IIntent* intent);

public:
    /**
     * The singleton OtaUtils instance used for OTASP calls.
     *
     * The OtaUtils instance is created lazily the first time we need to
     * make an OTASP call, regardless of whether it's an interactive or
     * non-interactive OTASP call.
     */
    AutoPtr<OtaUtils> mOtaUtils;

    // Following are the CDMA OTA information Objects used during OTA Call.
    // cdmaOtaProvisionData object store static OTA information that needs
    // to be maintained even during Slider open/close scenarios.
    // cdmaOtaConfigData object stores configuration info to control visiblity
    // of each OTA Screens.
    // cdmaOtaScreenState object store OTA Screen State information.
    AutoPtr<OtaUtils::CdmaOtaProvisionData> mCdmaOtaProvisionData;
    AutoPtr<OtaUtils::CdmaOtaConfigData> mCdmaOtaConfigData;
    AutoPtr<OtaUtils::CdmaOtaScreenState> mCdmaOtaScreenState;
    AutoPtr<OtaUtils::CdmaOtaInCallScreenUiState> mCdmaOtaInCallScreenUiState;

private:
    static const Boolean DBG;
    static const Boolean VDBG;

    // Message codes; see mHandler below.
    static const Int32 EVENT_SIM_NETWORK_LOCKED;
    static const Int32 EVENT_SIM_STATE_CHANGED;
    static const Int32 EVENT_DATA_ROAMING_DISCONNECTED;
    static const Int32 EVENT_DATA_ROAMING_OK;
    static const Int32 EVENT_UNSOL_CDMA_INFO_RECORD;
    static const Int32 EVENT_DOCK_STATE_CHANGED;
    static const Int32 EVENT_START_SIP_SERVICE;

    static AutoPtr<PhoneGlobals> sMe;

    // A few important fields we expose to the rest of the package
    // directly (rather than thru set/get methods) for efficiency.
    AutoPtr<ICallController> mCallController;
    AutoPtr<ICallManager> mCM;
    AutoPtr<ICallNotifier> mNotifier;
    AutoPtr<ICallerInfoCache> mCallerInfoCache;
    AutoPtr<INotificationMgr> mNotificationMgr;
    AutoPtr<IPhone> mHhone;
    AutoPtr<IPhoneInterfaceManager> mPhoneMgr;

    AutoPtr<IBluetoothManager> mBluetoothManager;
    AutoPtr<ICallGatewayManager> mCallGatewayManager;
    AutoPtr<ICallStateMonitor> mCallStateMonitor;

    static Int32 mDockState;
    static Boolean sVoiceCapable;

    // Internal PhoneApp Call state tracker
    CdmaPhoneCallState mCdmaPhoneCallState;

    // The currently-active PUK entry activity and progress dialog.
    // Normally, these are the Emergency Dialer and the subsequent
    // progress dialog.  null if there is are no such objects in
    // the foreground.
    AutoPtr<IActivity> mPUKEntryActivity;
    AutoPtr<IProgressDialog> mPUKEntryProgressDialog;

    Boolean mIsSimPinEnabled;
    String mCachedSimPin;

    // True if we are beginning a call, but the phone state has not changed yet
    Boolean mBeginningCall;

    // Last phone state seen by updatePhoneState()
    PhoneConstantsState mLastPhoneState;

    PhoneGlobalsWakeState mWakeState;

    AutoPtr<IPowerManager> mPowerManager;
    AutoPtr<IIPowerManager> mPowerManagerService;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;
    AutoPtr<IPowerManagerWakeLock> mPartialWakeLock;
    AutoPtr<IKeyguardManager> mKeyguardManager;

    AutoPtr<IUpdateLock> mUpdateLock;

    // Broadcast receiver for various intent broadcasts (see onCreate())
    AutoPtr<IBroadcastReceiver> mReceiver;

    /** Boolean indicating restoring mute state on InCallScreen.onResume() */
    Boolean mShouldRestoreMuteOnInCallResume;

    AutoPtr<IHandler> mHandler;

    static const String DEFAULT_CALL_ORIGIN_PACKAGE;
    static const String ALLOWED_EXTRA_CALL_ORIGIN;
    /**
     * Used to determine if the preserved call origin is fresh enough.
     */
    static const Int64 CALL_ORIGIN_EXPIRATION_MILLIS;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_PHONEGLOBALS_H__