#ifndef  __ELASTOS_DROID_PHONE_OTAUTILS_H__
#define  __ELASTOS_DROID_PHONE_OTAUTILS_H__

#include "_Elastos.Droid.Server.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/AsyncResult.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnKeyListener;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IToggleButton;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * Handles all OTASP Call related logic and UI functionality.
 * The InCallScreen interacts with this class to perform an OTASP Call.
 *
 * OTASP is a CDMA-specific feature:
 *   OTA or OTASP == Over The Air service provisioning
 *   SPC == Service Programming Code
 *   TODO: Include pointer to more detailed documentation.
 *
 * TODO: This is Over The Air Service Provisioning (OTASP)
 *       A better name would be OtaspUtils.java.
 */
class OtaUtils
    : public Object
    , public IOtaUtils
{
public:
    /**
     * Defines OTA information that needs to be maintained during
     * an OTA call when display orientation changes.
     */
    class CdmaOtaProvisionData
        : public Object
    {
    public:
        TO_STRING_IMPL("OtaUtils::CdmaOtaProvisionData")

    public:
        Boolean mIsOtaCallCommitted;
        Boolean mIsOtaCallIntentProcessed;
        Boolean mInOtaSpcState;
        Int32 mActivationCount;
        Int64 mOtaSpcUptime;
    };

    /**
     * Defines OTA screen configuration items read from config.xml
     * and used to control OTA display.
     */
    class CdmaOtaConfigData
        : public Object
    {
    public:
        TO_STRING_IMPL("OtaUtils::CdmaOtaConfigData")

        CdmaOtaConfigData()
            : mOtaShowActivationScreen(OTA_SHOW_ACTIVATION_SCREEN_OFF)
            , mOtaShowListeningScreen(OTA_SHOW_LISTENING_SCREEN_OFF)
            , mOtaShowActivateFailTimes(OTA_SHOW_ACTIVATE_FAIL_COUNT_OFF)
            , mOtaPlaySuccessFailureTone(OTA_PLAY_SUCCESS_FAILURE_TONE_OFF)
            , mConfigComplete(FALSE)

        {
            if (DBG) Log(String("CdmaOtaConfigData constructor!"));
        }

    public:
        Int32 mOtaShowActivationScreen;
        Int32 mOtaShowListeningScreen;
        Int32 mOtaShowActivateFailTimes;
        Int32 mOtaPlaySuccessFailureTone;
        Boolean mConfigComplete;
    };

    /**
     * The state of the OTA InCallScreen UI.
     */
    class CdmaOtaInCallScreenUiState
        : public Object
    {
    public:
        TO_STRING_IMPL("OtaUtils::CdmaOtaInCallScreenUiState")

        CdmaOtaInCallScreenUiState()
            : mState(UNDEFINED)
        {
            if (DBG) Log(String("CdmaOtaInCallScreenState: constructor init to UNDEFINED"));
        }

    public:
        enum State {
            UNDEFINED,
            NORMAL,
            ENDED
        };

        State mState;
    };

    /**
     * The OTA screen state machine.
     */
    class CdmaOtaScreenState
        : public Object
    {
    public:
        TO_STRING_IMPL("OtaUtils::CdmaOtaScreenState")

        CdmaOtaScreenState()
            : mOtaScreenState(OTA_STATUS_UNDEFINED)
        {}

    public:
        enum OtaScreenState {
            OTA_STATUS_UNDEFINED,
            OTA_STATUS_ACTIVATION,
            OTA_STATUS_LISTENING,
            OTA_STATUS_PROGRESS,
            OTA_STATUS_SUCCESS_FAILURE_DLG
        };

        OtaScreenState mOtaScreenState;

        /**
         * {@link PendingIntent} used to report an OTASP result status code
         * back to our caller. Can be null.
         *
         * Our caller (presumably SetupWizard) may create this PendingIntent,
         * pointing back at itself, and passes it along as an extra with the
         * ACTION_PERFORM_CDMA_PROVISIONING intent.  Then, when there's an
         * OTASP result to report, we send that PendingIntent back, adding an
         * extra called EXTRA_OTASP_RESULT_CODE to indicate the result.
         *
         * Possible result values are the OTASP_RESULT_* constants.
         */
        AutoPtr<IPendingIntent> mOtaspResultCodePendingIntent;
    };

private:
    /**
     * OtaWidgetData class represent all OTA UI elements
     *
     * TODO(OTASP): It's really ugly for the OtaUtils object to reach into the
     *     InCallScreen like this and directly manipulate its widgets.
     *
     *     Instead, the model/view separation should be more clear: OtaUtils
     *     should only know about a higher-level abstraction of the
     *     OTASP-specific UI state (just like how the CallController uses the
     *     InCallUiState object), and the InCallScreen itself should translate
     *     that higher-level abstraction into actual onscreen views and widgets.
     */
    class OtaWidgetData
        : public Object
    {
    public:
        TO_STRING_IMPL("OtaUtils::OtaWidgetData")

    public:
        AutoPtr<IButton> mOtaEndButton;
        AutoPtr<IButton> mOtaActivateButton;
        AutoPtr<IButton> mOtaSkipButton;
        AutoPtr<IButton> mOtaNextButton;
        AutoPtr<IToggleButton> mOtaSpeakerButton;
        AutoPtr<IViewGroup> mOtaUpperWidgets;
        AutoPtr<IView> mCallCardOtaButtonsFailSuccess;
        AutoPtr<IProgressBar> mOtaTextProgressBar;
        AutoPtr<ITextView> mOtaTextSuccessFail;
        AutoPtr<IView> mCallCardOtaButtonsActivate;
        AutoPtr<IView> mCallCardOtaButtonsListenProgress;
        AutoPtr<ITextView> mOtaTextActivate;
        AutoPtr<ITextView> mOtaTextListenProgress;
        AutoPtr<IAlertDialog> mSpcErrorDialog;
        AutoPtr<IAlertDialog> mOtaFailureDialog;
        AutoPtr<IAlertDialog> mOtaSkipConfirmationDialog;
        AutoPtr<ITextView> mOtaTitle;
        AutoPtr<IButton> mOtaTryAgainButton;
    };

    class MyDialogInterfaceOnKeyListener
        : public Object
        , public IDialogInterfaceOnKeyListener
    {
    public:
        TO_STRING_IMPL("OtaUtils::MyDialogInterfaceOnKeyListener")

        CAR_INTERFACE_DECL()

        CARAPI OnKey(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);
    };

    class MyDialogInterfaceOnKeyListener2
        : public Object
        , public IDialogInterfaceOnKeyListener
    {
    public:
        TO_STRING_IMPL("OtaUtils::MyDialogInterfaceOnKeyListener2")

        CAR_INTERFACE_DECL()

        CARAPI OnKey(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);
    };

    class MyAlertDialogOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("OtaUtils::MyAlertDialogOnClickListener")

        CAR_INTERFACE_DECL()

        MyAlertDialogOnClickListener(
            /* [in] */ OtaUtils* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        OtaUtils* mHost;
    };

public:
    TO_STRING_IMPL("OtaUtils")

    CAR_INTERFACE_DECL()

    OtaUtils();

    /**
     * OtaUtils constructor.
     *
     * @param context the Context of the calling Activity or Application
     * @param interactive if true, use the InCallScreen to display the progress
     *                    and result of the OTASP call.  In practice this is
     *                    true IFF the current device is a voice-capable phone.
     *
     * Note if interactive is true, you must also call updateUiWidgets() as soon
     * as the InCallScreen instance is ready.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Boolean interactive,
        /* [in] */ IBluetoothManager* bluetoothManager);

    /**
     * Starts the OTA provisioning call.  If the MIN isn't available yet, it returns false and adds
     * an event to return the request to the calling app when it becomes available.
     *
     * @param context
     * @param handler
     * @param request
     * @return true if we were able to launch Ota activity or it's not required; false otherwise
     */
    CARAPI static MaybeDoOtaCall(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler,
        /* [in] */ Int32 request,
        /* [out] */ Boolean* result);

    /**
     * Starts a normal "interactive" OTASP call (i.e. CDMA activation
     * for regular voice-capable phone devices.)
     *
     * This method is called from the InCallScreenShowActivation activity when
     * handling the ACTION_PERFORM_CDMA_PROVISIONING intent.
     */
    CARAPI static StartInteractiveOtasp(
        /* [in] */ IContext* context);

    /**
     * Starts the OTASP call *without* involving the InCallScreen or
     * displaying any UI.
     *
     * This is used on data-only devices, which don't support any kind of
     * in-call phone UI.
     *
     * @return PhoneUtils.CALL_STATUS_DIALED if we successfully
     *         dialed the OTASP number, or one of the other
     *         CALL_STATUS_* constants if there was a failure.
     */
    CARAPI static StartNonInteractiveOtasp(
        /* [in] */ IContext* context,
        /* [out] */ Int32* result);

    /**
     * @return true if the specified Intent is a CALL action that's an attempt
     * to initate an OTASP call.
     *
     * OTASP is a CDMA-specific concept, so this method will always return false
     * on GSM phones.
     *
     * This code was originally part of the InCallScreen.checkIsOtaCall() method.
     */
    CARAPI static IsOtaspCallIntent(
        /* [in] */ IIntent* intent,
        /* [out] */ Boolean* result);

    /**
     * Set up for an OTASP call.
     *
     * This method is called as part of the CallController placeCall() sequence
     * before initiating an outgoing OTASP call.
     *
     * The purpose of this method is mainly to create and initialize the
     * OtaUtils instance, along with some other misc pre-OTASP cleanup.
     */
    CARAPI static SetupOtaspCall(
        /* [in] */ IIntent* intent);

    /**
     * Handles OTA Provision events from the telephony layer.
     * These events come in to this method whether or not
     * the InCallScreen is visible.
     *
     * Possible events are:
     * OTA Commit Event - OTA provisioning was successful
     * SPC retries exceeded - SPC failure retries has exceeded, and Phone needs to
     *    power down.
     */
    CARAPI OnOtaProvisionStatusChanged(
        /* [in] */ AsyncResult* r);

    /**
     * Handle a disconnect event from the OTASP call.
     */
    CARAPI OnOtaspDisconnect();

    /**
     * Show Activation Screen when phone powers up and OTA provision is
     * required. Also shown when activation fails and user needs
     * to re-attempt it. Contains ACTIVATE and SKIP buttons
     * which allow user to start OTA activation or skip the activation process.
     */
    CARAPI OtaShowActivateScreen();

    /**
     * Show either programming success dialog when OTA provisioning succeeds, or
     * programming failure dialog when it fails. See {@link #otaShowProgramFailure}
     * for more details.
     */
    CARAPI OtaShowSuccessFailure();

    /**
     * When SPC notice times out, force phone to power down.
     */
    CARAPI OnOtaCloseSpcNotice();

    /**
     * Handle OTA unsuccessful notice expiry. Dismisses the
     * two-second notice and shows the activation screen.
     */
    CARAPI OnOtaCloseFailureNotice();

    CARAPI HideOtaScreen();

    CARAPI IsDialerOpened(
        /* [out] */ Boolean* result);

    /**
     * Show the appropriate OTA screen based on the current state of OTA call.
     *
     * This is called from the InCallScreen when the screen needs to be
     * refreshed (and thus is only ever used in interactive mode.)
     *
     * Since this is called as part of the InCallScreen.updateScreen() sequence,
     * this method does *not* post an mInCallScreen.requestUpdateScreen()
     * request.
     */
    CARAPI OtaShowProperScreen();

    /**
     * Handle the click events for OTA buttons.
     */
    CARAPI OnClickHandler(
        /* [in] */ Int32 id);

    CARAPI DismissAllOtaDialogs();

    /**
     * Clear out all OTA UI widget elements. Needs to get called
     * when OTA call ends or InCallScreen is destroyed.
     * @param disableSpeaker parameter control whether Speaker should be turned off.
     */
    CARAPI CleanOtaScreen(
        /* [in] */ Boolean disableSpeaker);

    /**
     * Save the Ota InCallScreen UI state
     */
    CARAPI SetCdmaOtaInCallScreenUiState(
        /* [in] */ CdmaOtaInCallScreenUiState::State state);

    /**
     * Get the Ota InCallScreen UI state
     */
    CARAPI GetCdmaOtaInCallScreenUiState(
        /* [out] */ CdmaOtaInCallScreenUiState::State* state);

private:
    CARAPI_(void) SetSpeaker(
        /* [in] */ Boolean state);

    CARAPI_(void) OtaShowHome();

    CARAPI_(void) OtaSkipActivation();

    /**
     * Actually initiate the OTASP call.  This method is triggered by the
     * onscreen "Activate" button, and is only used in interactive mode.
     */
    CARAPI_(void) OtaPerformActivation();

    /**
     * Show "Listen for Instruction" screen during OTA call. Shown when OTA Call
     * is initiated and user needs to listen for network instructions and press
     * appropriate DTMF digits to proceed to the "Programming in Progress" phase.
     */
    CARAPI_(void) OtaShowListeningScreen();

    /**
     * Do any necessary updates (of onscreen UI, for example)
     * based on the latest status of the OTASP call.
     */
    CARAPI_(void) UpdateOtaspProgress();

    /**
     * When a non-interactive OTASP call completes, send a success or
     * failure indication back to our caller.
     *
     * This is basically the non-interactive equivalent of
     * otaShowSuccessFailure().
     */
    CARAPI_(void) UpdateNonInteractiveOtaSuccessFailure();

    /**
     * Sends the specified OTASP result code back to our caller (presumably
     * SetupWizard) via the PendingIntent that they originally sent along with
     * the ACTION_PERFORM_CDMA_PROVISIONING intent.
     */
    CARAPI_(void) SendOtaspResult(
        /* [in] */ Int32 resultCode);

    /**
     * Show "Programming In Progress" screen during OTA call. Shown when OTA
     * provisioning is in progress after user has selected an option.
     */
    CARAPI_(void) OtaShowInProgressScreen();

    /**
     * Show programming failure dialog when OTA provisioning fails.
     * If OTA provisioning attempts fail more than 3 times, then unsuccessful
     * dialog is shown. Otherwise a two-second notice is shown with unsuccessful
     * information. When notice expires, phone returns to activation screen.
     */
    CARAPI_(void) OtaShowProgramFailure(
        /* [in] */ Int32 length);

    /**
     * Show programming failure dialog when OTA provisioning fails more than 3
     * times.
     */
    CARAPI_(void) OtaShowProgramFailureDialog();

    /**
     * Show programming success dialog when OTA provisioning succeeds.
     */
    CARAPI_(void) OtaShowProgramSuccessDialog();

    /**
     * Show SPC failure notice when SPC attempts exceed 15 times.
     * During OTA provisioning, if SPC code is incorrect OTA provisioning will
     * fail. When SPC attempts are over 15, it shows SPC failure notice for one minute and
     * then phone will power down.
     */
    CARAPI_(void) OtaShowSpcErrorNotice(
        /* [in] */ Int32 length);

    /**
     * Show two-second notice when OTA provisioning fails and number of failed attempts
     * is less then 3.
     */
    CARAPI_(void) OtaShowProgramFailureNotice(
        /* [in] */ Int32 length);

    /**
     * Initialize all OTA UI elements to be gone. Also set inCallPanel,
     * callCard and the dialpad handle to be gone. This is called before any OTA screen
     * gets drawn.
     */
    CARAPI_(void) OtaScreenInitialize();

    /**
     * Read configuration values for each OTA screen from config.xml.
     * These configuration values control visibility of each screen.
     */
    CARAPI_(void) ReadXmlSettings();

    CARAPI_(void) OnClickOtaTryAgainButton();

    CARAPI_(void) OnClickOtaEndButton();

    CARAPI_(void) OnClickOtaSpeakerButton();

    CARAPI_(void) OnClickOtaActivateButton();

    CARAPI_(void) OnClickOtaActivateSkipButton();

    CARAPI_(void) OnClickOtaActivateNextButton();

    CARAPI_(Int32) GetOtaSpcDisplayTime();

    /**
     * Initialize the OTA widgets for all OTA screens.
     */
    CARAPI_(void) InitOtaInCallScreen();

    /** @see com.android.internal.telephony.Phone */
    static CARAPI_(String) OtaProvisionStatusToString(
        /* [in] */ Int32 status);

    static CARAPI_(Int32) GetLteOnCdmaMode(
        /* [in] */ IContext* context);

    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

    static CARAPI_(void) Loge(
        /* [in] */ const String& msg);

public:
    // SPC Timeout is 60 seconds
    const Int32 OTA_SPC_TIMEOUT;
    const Int32 OTA_FAILURE_DIALOG_TIMEOUT;

private:
    static const String TAG;
    static const Boolean DBG;

    // TODO: Distinguish between interactive and non-interactive success
    // and failure.  Then, have the PendingIntent be sent after
    // interactive OTASP as well (so the caller can find out definitively
    // when interactive OTASP completes.)

    static const String OTASP_NUMBER;
    static const String OTASP_NUMBER_NON_INTERACTIVE;

    AutoPtr<IContext> mContext;
    AutoPtr<IPhoneGlobals> mApplication;
    AutoPtr<OtaWidgetData> mOtaWidgetData;

    static Boolean sIsWizardMode;

    // How many times do we retry maybeDoOtaCall() if the LTE state is not known yet,
    // and how long do we wait between retries
    static const Int32 OTA_CALL_LTE_RETRIES_MAX;
    static const Int32 OTA_CALL_LTE_RETRY_PERIOD;
    static Int32 sOtaCallLteRetries;

    // In "interactive mode", the OtaUtils object is tied to an
    // InCallScreen instance, where we display a bunch of UI specific to
    // the OTASP call.  But on devices that are not "voice capable", the
    // OTASP call runs in a non-interactive mode, and we don't have
    // an InCallScreen or CallCard or any OTASP UI elements at all.
    Boolean mInteractive;

    // used when setting speakerphone
    AutoPtr<IBluetoothManager> mBluetoothManager;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_OTAUTILS_H__