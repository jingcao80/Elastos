#ifndef  __ELASTOS_DROID_PHONE_PHONEGLOBALS_H__
#define  __ELASTOS_DROID_PHONE_PHONEGLOBALS_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/teleservice/phone/CallController.h"
#include "elastos/droid/teleservice/phone/CallerInfoCache.h"
#include "elastos/droid/teleservice/phone/CallNotifier.h"
#include "elastos/droid/teleservice/phone/CallGatewayManager.h"
#include "elastos/droid/teleservice/phone/CdmaPhoneCallState.h"
#include "elastos/droid/teleservice/phone/CPhoneInterfaceManager.h"
#include "elastos/droid/teleservice/phone/NotificationMgr.h"
#include "elastos/droid/teleservice/phone/OtaUtils.h"
#include "elastos/droid/teleservice/phone/CallStateMonitor.h"
#include "elastos/droid/content/ContextWrapper.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/AsyncResult.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IKeyguardManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::ContextWrapper;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IIPowerManager;
using Elastos::Droid::Os::IUpdateLock;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::ICallManager;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Global state for the telephony subsystem when running in the primary
 * phone process.
 */
class PhoneGlobals
    : public ContextWrapper
    , public IPhoneGlobals
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

private:
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

    CAR_INTERFACE_DECL()

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
        /* [in] */ IPhoneGlobalsWakeState ws);

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
        /* [in] */ PhoneConstantsState state);

    CARAPI GetPhoneState(
        /* [out] */ PhoneConstantsState* state);

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
        /* [in] */ AsyncResult* r);

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
    friend class SpecialCharSequenceMgr;
    friend class OtaUtils;
    friend class PhoneUtils;
    friend class CallController;
    friend class CPhoneInterfaceManager;
    friend class COtaStartupReceiver;
    friend class COutgoingCallBroadcaster;
    friend class CNetworkSetting;
    friend class CallNotifier;
    friend class EmergencyCallHelper;
    friend class NotificationMgr;

    static const Boolean DBG;
    static const Boolean VDBG;

    // Message codes; see mHandler below.
    static const Int32 EVENT_SIM_NETWORK_LOCKED = 3;
    static const Int32 EVENT_SIM_STATE_CHANGED = 8;
    static const Int32 EVENT_DATA_ROAMING_DISCONNECTED = 10;
    static const Int32 EVENT_DATA_ROAMING_OK = 11;
    static const Int32 EVENT_UNSOL_CDMA_INFO_RECORD = 12;
    static const Int32 EVENT_DOCK_STATE_CHANGED = 13;
    static const Int32 EVENT_START_SIP_SERVICE = 14;

    static AutoPtr<PhoneGlobals> sMe;

    // A few important fields we expose to the rest of the package
    // directly (rather than thru set/get methods) for efficiency.
    AutoPtr<CallController> mCallController;
    AutoPtr<ICallManager> mCM;
    AutoPtr<CallNotifier> mNotifier;
    AutoPtr<CallerInfoCache> mCallerInfoCache;
    AutoPtr<NotificationMgr> mNotificationMgr;
    AutoPtr<IPhone> mPhone;
    AutoPtr<CPhoneInterfaceManager> mPhoneMgr;

    AutoPtr<IBluetoothManager> mBluetoothManager;
    AutoPtr<CallGatewayManager> mCallGatewayManager;
    AutoPtr<CallStateMonitor> mCallStateMonitor;

    static Int32 mDockState;
    static Boolean sVoiceCapable;

    // Internal PhoneApp Call state tracker
    AutoPtr<CdmaPhoneCallState> mCdmaPhoneCallState;

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

    IPhoneGlobalsWakeState mWakeState;

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
} // namespace TeleService
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_PHONEGLOBALS_H__
