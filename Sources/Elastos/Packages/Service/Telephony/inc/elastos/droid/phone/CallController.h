#ifndef  __ELASTOS_DROID_PHONE_CALLCONTROLLER_H__
#define  __ELASTOS_DROID_PHONE_CALLCONTROLLER_H__

#include "_Elastos.Droid.Server.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * Phone app module in charge of "call control".
 *
 * This is a singleton object which acts as the interface to the telephony layer
 * (and other parts of the Android framework) for all user-initiated telephony
 * functionality, like making outgoing calls.
 *
 * This functionality includes things like:
 *   - actually running the placeCall() method and handling errors or retries
 *   - running the whole "emergency call in airplane mode" sequence
 *   - running the state machine of MMI sequences
 *   - restoring/resetting mute and speaker state when a new call starts
 *   - updating the prox sensor wake lock state
 *   - resolving what the voicemail: intent should mean (and making the call)
 *
 * The single CallController instance stays around forever; it's not tied
 * to the lifecycle of any particular Activity (like the InCallScreen).
 * There's also no implementation of onscreen UI here (that's all in InCallScreen).
 *
 * Note that this class does not handle asynchronous events from the telephony
 * layer, like reacting to an incoming call; see CallNotifier for that.  This
 * class purely handles actions initiated by the user, like outgoing calls.
 */
class CallController
    : public Handler
    , public ICallController
{
public:
    TO_STRING_IMPL("CallController")

    CAR_INTERFACE_DECL()

    /**
     * Initialize the singleton CallController instance.
     *
     * This is only done once, at startup, from PhoneApp.onCreate().
     * From then on, the CallController instance is available via the
     * PhoneApp's public "callController" field, which is why there's no
     * getInstance() method here.
     */
    static CARAPI Init(
        /* [in] */ IPhoneGlobals* app,
        /* [in] */ ICallLogger* callLogger,
        /* [in] */ ICallGatewayManager* callGatewayManager,
        /* [out] */ ICallController** controller);

    //@Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    //
    // Outgoing call sequence
    //

    /**
     * Initiate an outgoing call.
     *
     * Here's the most typical outgoing call sequence:
     *
     *  (1) OutgoingCallBroadcaster receives a CALL intent and sends the
     *      NEW_OUTGOING_CALL broadcast
     *
     *  (2) The broadcast finally reaches OutgoingCallReceiver, which stashes
     *      away a copy of the original CALL intent and launches
     *      SipCallOptionHandler
     *
     *  (3) SipCallOptionHandler decides whether this is a PSTN or SIP call (and
     *      in some cases brings up a dialog to let the user choose), and
     *      ultimately calls CallController.placeCall() (from the
     *      setResultAndFinish() method) with the stashed-away intent from step
     *      (2) as the "intent" parameter.
     *
     *  (4) Here in CallController.placeCall() we read the phone number or SIP
     *      address out of the intent and actually initiate the call, and
     *      simultaneously launch the InCallScreen to display the in-call UI.
     *
     *  (5) We handle various errors by directing the InCallScreen to
     *      display error messages or dialogs (via the InCallUiState
     *      "pending call status code" flag), and in some cases we also
     *      sometimes continue working in the background to resolve the
     *      problem (like in the case of an emergency call while in
     *      airplane mode).  Any time that some onscreen indication to the
     *      user needs to change, we update the "status dialog" info in
     *      the inCallUiState and (re)launch the InCallScreen to make sure
     *      it's visible.
     */
    CARAPI PlaceCall(
        /* [in] */ IIntent* intent);

private:
    /**
     * Private constructor (this is a singleton).
     * @see init()
     */
    CallController(
        /* [in] */ IPhoneGlobals* app,
        /* [in] */ ICallLogger* callLogger,
        /* [in] */ ICallGatewayManager* callGatewayManager);

    /**
     * Actually make a call to whomever the intent tells us to.
     *
     * Note that there's no need to explicitly update (or refresh) the
     * in-call UI at any point in this method, since a fresh InCallScreen
     * instance will be launched automatically after we return (see
     * placeCall() above.)
     *
     * @param intent the CALL intent describing whom to call
     * @return CallStatusCode.SUCCESS if we successfully initiated an
     *    outgoing call.  If there was some kind of failure, return one of
     *    the other CallStatusCode codes indicating what went wrong.
     */
    CARAPI_(ConstantsCallStatusCode) PlaceCallInternal(
        /* [in] */ IIntent* intent);

    /**
     * Checks the current ServiceState to make sure it's OK
     * to try making an outgoing call to the specified number.
     *
     * @return CallStatusCode.SUCCESS if it's OK to try calling the specified
     *    number.  If not, like if the radio is powered off or we have no
     *    signal, return one of the other CallStatusCode codes indicating what
     *    the problem is.
     */
    CARAPI_(ConstantsCallStatusCode) CheckIfOkToInitiateOutgoingCall(
        /* [in] */ Int32 state);

    /**
     * Handles the various error conditions that can occur when initiating
     * an outgoing call.
     *
     * Most error conditions are "handled" by simply displaying an error
     * message to the user.
     *
     * @param status one of the CallStatusCode error codes.
     */
    CARAPI_(void) HandleOutgoingCallError(
        /* [in] */ ConstantsCallStatusCode status);

    /**
     * Checks the current outgoing call to see if it's an OTASP call (the
     * "activation" call used to provision CDMA devices).  If so, do any
     * necessary OTASP-specific setup before actually placing the call.
     */
    CARAPI_(void) CheckForOtaspCall(
        /* [in] */ IIntent* intent);

    //
    // Debugging
    //

    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String TAG;
    static const Boolean DBG;
    // Do not check in with VDBG = true, since that may write PII to the system log.
    static const Boolean VDBG;

    /** The singleton CallController instance. */
    static AutoPtr<ICallController> sInstance;

    AutoPtr<PhoneGlobals> mApp;
    AutoPtr<ICallManager> mCM;
    AutoPtr<ICallLogger> mCallLogger;
    AutoPtr<ICallGatewayManager> mCallGatewayManager;

    /** Helper object for emergency calls in some rare use cases.  Created lazily. */
    AutoPtr<EmergencyCallHelper> mEmergencyCallHelper;

    //
    // Message codes; see handleMessage().
    //

    static const Int32 THREEWAY_CALLERINFO_DISPLAY_DONE;


    //
    // Misc constants.
    //

    // Amount of time the UI should display "Dialing" when initiating a CDMA
    // 3way call.  (See comments on the THRWAY_ACTIVE case in
    // placeCallInternal() for more info.)
    static const Int32 THREEWAY_CALLERINFO_DISPLAY_TIME; // msec
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CALLCONTROLLER_H__