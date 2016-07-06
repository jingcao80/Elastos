
#include "elastos/droid/teleservice/phone/CallController.h"
#include "elastos/droid/teleservice/phone/CallGatewayManager.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "elastos/droid/teleservice/phone/PhoneUtils.h"
#include "elastos/droid/teleservice/phone/CdmaPhoneCallState.h"
#include "R.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::ITelephonyCapabilities;
using Elastos::Droid::Internal::Telephony::CTelephonyCapabilities;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Provider::ICalls;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::TeleService::R;
using Elastos::Core::AutoLock;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

const String CallController::TAG("CallController");

static Boolean initDBG()
{
    AutoPtr<ISystemProperties> helper;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&helper);
    Int32 value;
    helper->GetInt32(String("ro.debuggable"), 0, &value);

    return (IPhoneGlobals::DBG_LEVEL >= 1) && value == 1;
}

const Boolean CallController::DBG = initDBG();

const Boolean CallController::VDBG = FALSE;

AutoPtr<CallController> CallController::sInstance;

const Int32 CallController::THREEWAY_CALLERINFO_DISPLAY_DONE = 1;

const Int32 CallController::THREEWAY_CALLERINFO_DISPLAY_TIME = 3000; // msec
Object CallController::THIS;

ECode CallController::Init(
    /* [in] */ IPhoneGlobals* app,
    /* [in] */ CallLogger* callLogger,
    /* [in] */ CallGatewayManager* callGatewayManager,
    /* [out] */ CallController** controller)
{
    VALIDATE_NOT_NULL(controller)

    {
        AutoLock syncLock(THIS);
        if (sInstance == NULL) {
            sInstance = new CallController(app, callLogger, callGatewayManager);
        }
        else {
            Logger::W(TAG, "init() called multiple times!  sInstance = %s", TO_CSTR(sInstance));
        }
        *controller = sInstance;
        REFCOUNT_ADD(*controller)
        return NOERROR;
    }
}

CallController::CallController(
    /* [in] */ IPhoneGlobals* app,
    /* [in] */ CallLogger* callLogger,
    /* [in] */ CallGatewayManager* callGatewayManager)
    : mApp(app)
    , mCallLogger(callLogger)
    , mCallGatewayManager(callGatewayManager)
{
    Handler::constructor();

    if (DBG) {
        StringBuilder sb;
        sb += "CallController constructor: app = ";
        sb += TO_CSTR(app);
        Log(sb.ToString());
    }
    AutoPtr<PhoneGlobals> global = (PhoneGlobals*)app;
    mCM = global->mCM;
}

ECode CallController::HandleMessage(
    /* [in] */ IMessage* msg)
{
    if (VDBG) {
        StringBuilder sb;
        sb += "handleMessage: ";
        sb += msg;
        Log(sb.ToString());
    }

    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case THREEWAY_CALLERINFO_DISPLAY_DONE:
        {
            if (DBG) Log(String("THREEWAY_CALLERINFO_DISPLAY_DONE..."));

            AutoPtr<PhoneGlobals> global = (PhoneGlobals*)mApp.Get();
            CdmaPhoneCallState::PhoneCallState state;
            if ((global->mCdmaPhoneCallState->GetCurrentCallState(&state), state)
                == CdmaPhoneCallState::THRWAY_ACTIVE) {
                // Reset the mThreeWayCallOrigStateDialing state
                global->mCdmaPhoneCallState->SetThreeWayCallOrigState(FALSE);

                // TODO: Remove this code.
                //global.getCallModeler().setCdmaOutgoing3WayCall(null);
            }
            break;
        }
        default:
        {
            Logger::W(TAG, "handleMessage: unexpected code: %s", TO_CSTR(msg));
            break;
        }
    }
    return NOERROR;
}

ECode CallController::PlaceCall(
    /* [in] */ IIntent* intent)
{
    Log(String("placeCall()...  intent = ") + TO_CSTR(intent));
    if (VDBG) {
        AutoPtr<IBundle> obj;
        intent->GetExtras((IBundle**)&obj);
        Log(String("                extras = ") + TO_CSTR(obj));
    }

    // TODO: Do we need to hold a wake lock while this method runs?
    //       Or did we already acquire one somewhere earlier
    //       in this sequence (like when we first received the CALL intent?)

    if (intent == NULL) {
        //throw new IllegalArgumentException("placeCall: called with null intent");
        Logger::E(TAG, "placeCall: called with null intent");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String action;
    intent->GetAction(&action);
    AutoPtr<IUri> uri;
    intent->GetData((IUri**)&uri);
    if (uri == NULL) {
        //throw new IllegalArgumentException("placeCall: intent had no data");
        Logger::E(TAG, "placeCall: intent had no data");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String scheme;
    uri->GetScheme(&scheme);
    AutoPtr<IPhoneNumberUtils> helper;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&helper);
    String number;
    helper->GetNumberFromIntent(intent, IContext::Probe(mApp), &number);
    if (VDBG) {
        Log(String("- action: ") + action);
        Log(String("- uri: ") + TO_CSTR(uri));
        Log(String("- scheme: ") + scheme);
        Log(String("- number: ") + number);
    }

    // This method should only be used with the various flavors of CALL
    // intents.  (It doesn't make sense for any other action to trigger an
    // outgoing call!)
    if (!(IIntent::ACTION_CALL.Equals(action)
          || IIntent::ACTION_CALL_EMERGENCY.Equals(action)
          || IIntent::ACTION_CALL_PRIVILEGED.Equals(action))) {
        //throw new IllegalArgumentException("Unexpected action: " + action);
        Logger::E(TAG, "placeCall: unexpected intent action %s", action.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Check to see if this is an OTASP call (the "activation" call
    // used to provision CDMA devices), and if so, do some
    // OTASP-specific setup.
    AutoPtr<PhoneGlobals> global = (PhoneGlobals*)mApp.Get();
    AutoPtr<IPhone> phone;
    global->mCM->GetDefaultPhone((IPhone**)&phone);
    AutoPtr<ITelephonyCapabilities> helper2;
    CTelephonyCapabilities::AcquireSingleton((ITelephonyCapabilities**)&helper2);
    Boolean res;
    if (helper2->SupportsOtasp(phone, &res), res) {
        CheckForOtaspCall(intent);
    }

    // Clear out the "restore mute state" flag since we're
    // initiating a brand-new call.
    //
    // (This call to setRestoreMuteOnInCallResume(false) informs the
    // phone app that we're dealing with a new connection
    // (i.e. placing an outgoing call, and NOT handling an aborted
    // "Add Call" request), so we should let the mute state be handled
    // by the PhoneUtils phone state change handler.)
    global->SetRestoreMuteOnInCallResume(FALSE);

    ConstantsCallStatusCode status = PlaceCallInternal(intent);

    switch (status) {
        // Call was placed successfully:
        case Constants_SUCCESS:
        case Constants_EXITED_ECM:
            if (DBG) Log(String("==> placeCall(): success from placeCallInternal(): ")
                    + StringUtils::ToString(status));
            break;

        default:
            // Any other status code is a failure.
            Log(String("==> placeCall(): failure code from placeCallInternal(): ")
                    + StringUtils::ToString(status));
            // Handle the various error conditions that can occur when
            // initiating an outgoing call, typically by directing the
            // InCallScreen to display a diagnostic message (via the
            // "pending call status code" flag.)
            HandleOutgoingCallError(status);
            break;
    }

    // Finally, regardless of whether we successfully initiated the
    // outgoing call or not, force the InCallScreen to come to the
    // foreground.
    //
    // (For successful calls the the user will just see the normal
    // in-call UI.  Or if there was an error, the InCallScreen will
    // notice the InCallUiState pending call status code flag and display an
    // error indication instead.)
    return NOERROR;
}

ConstantsCallStatusCode CallController::PlaceCallInternal(
    /* [in] */ IIntent* intent)
{
    if (DBG) Log(String("placeCallInternal()...  intent = ") + TO_CSTR(intent));

    // TODO: This method is too long.  Break it down into more
    // manageable chunks.

    AutoPtr<IUri> uri;
    intent->GetData((IUri**)&uri);
    String scheme;
    if (uri != NULL) {
        uri->GetScheme(&scheme);
    }

    String number;
    AutoPtr<IPhone> phone;

    // Check the current ServiceState to make sure it's OK
    // to even try making a call.
    Int32 serviceState;
    mCM->GetServiceState(&serviceState);
    ConstantsCallStatusCode okToCallStatus;
    CheckIfOkToInitiateOutgoingCall(serviceState, &okToCallStatus);

    // TODO: Streamline the logic here.  Currently, the code is
    // unchanged from its original form in InCallScreen.java.  But we
    // should fix a couple of things:
    // - Don't call checkIfOkToInitiateOutgoingCall() more than once
    // - Wrap the try/catch for VoiceMailNumberMissingException
    //   around *only* the call that can throw that exception.

    //try
    ECode ec = NOERROR;
    {
        FAIL_GOTO(ec = PhoneUtils::GetInitialNumber(intent, &number), ERROR)
        if (VDBG) {
            StringBuilder sb;
            sb += "- actual number to dial: '";
            sb += number;
            sb += "'";
            Log(sb.ToString());
        }

        // find the phone first
        // TODO Need a way to determine which phone to place the call
        // It could be determined by SIP setting, i.e. always,
        // or by number, i.e. for international,
        // or by user selection, i.e., dialog query,
        // or any of combinations
        String sipPhoneUri;
        intent->GetStringExtra(
                IOutgoingCallBroadcaster::EXTRA_SIP_PHONE_URI, &sipPhoneUri);

        AutoPtr<IParcelable> obj;
        intent->GetParcelableExtra(
                IOutgoingCallBroadcaster::EXTRA_THIRD_PARTY_CALL_COMPONENT, (IParcelable**)&obj);
        AutoPtr<IComponentName> thirdPartyCallComponent = IComponentName::Probe(obj);
        phone = PhoneUtils::PickPhoneBasedOnNumber(mCM, scheme, number, sipPhoneUri, thirdPartyCallComponent);
        if (VDBG) {
            StringBuilder sb;
            sb += "- got Phone instance: ";
            sb += phone;
            sb += ", class = ";
            // sb += phone.getClass();
            Log(sb.ToString());
        }

        // update okToCallStatus based on new phone
        AutoPtr<IServiceState> sstate;
        phone->GetServiceState((IServiceState**)&sstate);
        Int32 state;
        sstate->GetState(&state);
        CheckIfOkToInitiateOutgoingCall(state, &okToCallStatus);
    }
ERROR:
    //catch (PhoneUtils.VoiceMailNumberMissingException ex) {
        // If the call status is NOT in an acceptable state, it
        // may effect the way the voicemail number is being
        // retrieved.  Mask the VoiceMailNumberMissingException
        // with the underlying issue of the phone state.
    assert(0 && "TODO: Need PhoneUtils");
    // if (ec == (ECode)PhoneUtils.VoiceMailNumberMissingException) {
    //     if (okToCallStatus != Constants_SUCCESS) {
    //         if (DBG) Log(String("Voicemail number not reachable in current SIM card state."));
    //         return okToCallStatus;
    //     }
    //     if (DBG) Log(String("VoiceMailNumberMissingException from getInitialNumber()"));
    //     return Constants_SUCCESS_VOICEMAIL_NUMBER_MISSING;
    // }
    //}

    if (number.IsNull()) {
        Logger::W(TAG, "placeCall: couldn't get a phone number from Intent %s", TO_CSTR(intent));
        return Constants_NO_PHONE_NUMBER_SUPPLIED;
    }

    // Sanity-check that ACTION_CALL_EMERGENCY is used if and only if
    // this is a call to an emergency number
    // (This is just a sanity-check; this policy *should* really be
    // enforced in OutgoingCallBroadcaster.onCreate(), which is the
    // main entry point for the CALL and CALL_* intents.)
    AutoPtr<IPhoneNumberUtils> helper;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&helper);
    Boolean isEmergencyNumber;
    helper->IsLocalEmergencyNumber(IContext::Probe(mApp), number, &isEmergencyNumber);
    Boolean isPotentialEmergencyNumber;
    helper->IsPotentialLocalEmergencyNumber(IContext::Probe(mApp), number, &isPotentialEmergencyNumber);
    String action;
    intent->GetAction(&action);
    Boolean isEmergencyIntent = IIntent::ACTION_CALL_EMERGENCY.Equals(action);

    if (isPotentialEmergencyNumber && !isEmergencyIntent) {
        StringBuilder sb;
        sb += "Non-CALL_EMERGENCY Intent ";
        sb += TO_CSTR(intent);
        sb += " attempted to call potential emergency number ";
        sb += number;
        sb += ".";
        Logger::E(TAG, sb.ToString());
        return Constants_CALL_FAILED;
    }
    else if (!isPotentialEmergencyNumber && isEmergencyIntent) {
        StringBuilder sb;
        sb += "Received CALL_EMERGENCY Intent ";
        sb += TO_CSTR(intent);
        sb += " with non-potential-emergency number ";
        sb += number;
        sb += " -- failing call.";
        Logger::E(TAG, sb.ToString());
        return Constants_CALL_FAILED;
    }

    // If we're trying to call an emergency number, then it's OK to
    // proceed in certain states where we'd otherwise bring up
    // an error dialog:
    // - If we're in EMERGENCY_ONLY mode, then (obviously) you're allowed
    //   to dial emergency numbers.
    // - If we're OUT_OF_SERVICE, we still attempt to make a call,
    //   since the radio will register to any available network.

    if (isEmergencyNumber
        && ((okToCallStatus == Constants_EMERGENCY_ONLY)
            || (okToCallStatus == Constants_OUT_OF_SERVICE))) {
        if (DBG) Log(String("placeCall: Emergency number detected with status = ")
                + StringUtils::ToString(okToCallStatus));
        okToCallStatus = Constants_SUCCESS;
        if (DBG) Log(String("==> UPDATING status to: ")
                + StringUtils::ToString(okToCallStatus));
    }

    if (okToCallStatus != Constants_SUCCESS) {
        // If this is an emergency call, launch the EmergencyCallHelperService
        // to turn on the radio and retry the call.
        if (isEmergencyNumber && (okToCallStatus == Constants_POWER_OFF)) {
            Logger::I(TAG, "placeCall: Trying to make emergency call while POWER_OFF!");

            // If needed, lazily instantiate an EmergencyCallHelper instance.
            {
                AutoLock syncLock(this);
                if (mEmergencyCallHelper == NULL) {
                    mEmergencyCallHelper = new EmergencyCallHelper();
                    mEmergencyCallHelper->constructor(this);
                }
            }

            // ...and kick off the "emergency call from airplane mode" sequence.
            mEmergencyCallHelper->StartEmergencyCallFromAirplaneModeSequence(number);

            // Finally, return CallStatusCode.SUCCESS right now so
            // that the in-call UI will remain visible (in order to
            // display the progress indication.)
            // TODO: or maybe it would be more clear to return a whole
            // new CallStatusCode called "TURNING_ON_RADIO" here.
            // That way, we'd update inCallUiState.progressIndication from
            // the handleOutgoingCallError() method, rather than here.
            return Constants_SUCCESS;
        }
        else {
            // Otherwise, just return the (non-SUCCESS) status code
            // back to our caller.
            if (DBG) Log(String("==> placeCallInternal(): non-success status: ")
                + StringUtils::ToString(okToCallStatus));

            // Log failed call.
            // Note: Normally, many of these values we gather from the Connection object but
            // since no such object is created for unconnected calls, we have to build them
            // manually.
            // TODO: Try to restructure code so that we can handle failure-
            // condition call logging in a single place (placeCall()) that also has access to
            // the number we attempted to dial (not placeCall()).
            Int64 currentTimeMillis = 0;
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            system->GetCurrentTimeMillis(&currentTimeMillis);
            mCallLogger->LogCall(NULL /* callerInfo */, number, 0 /* presentation */,
                    ICalls::OUTGOING_TYPE, currentTimeMillis, 0 /* duration */);

            return okToCallStatus;
        }
    }

    // We have a valid number, so try to actually place a call:
    // make sure we pass along the intent's URI which is a
    // reference to the contact. We may have a provider gateway
    // phone number to use for the outgoing call.
    AutoPtr<IUri> contactUri;
    intent->GetData((IUri**)&contactUri);

    // If a gateway is used, extract the data here and pass that into placeCall.
    AutoPtr<CallGatewayManager::RawGatewayInfo> rawGatewayInfo;
    rawGatewayInfo = mCallGatewayManager->GetRawGatewayInfo(intent, number);

    // Watch out: PhoneUtils.placeCall() returns one of the
    // CALL_STATUS_* constants, not a CallStatusCode enum value.
    assert(0 && "TODO: Need IPhoneGlobals");
    // Int32 callStatus = 0;
    // callStatus = PhoneUtils::PlaceCall(mApp, phone, number, contactUri, (isEmergencyNumber || isEmergencyIntent),
    //       rawGatewayInfo, mCallGatewayManager);

    // switch (callStatus) {
    //     case PhoneUtils::CALL_STATUS_DIALED: {
    //         if (VDBG) {
    //             StringBuilder sb;
    //             sb += "placeCall: PhoneUtils.placeCall() succeeded for regular call '";
    //             sb += number;
    //             sb += "'.";
    //             Log(sb.ToString());
    //         }


    //         // TODO(OTASP): still need more cleanup to simplify the mApp.cdma*State objects:
    //         // - Rather than checking inCallUiState.inCallScreenMode, the
    //         //   code here could also check for
    //         //   app.getCdmaOtaInCallScreenUiState() returning NORMAL.
    //         // - But overall, app.inCallUiState.inCallScreenMode and
    //         //   app.cdmaOtaInCallScreenUiState.state are redundant.
    //         //   Combine them.

    //         Boolean voicemailUriSpecified = !scheme.IsNull() &&
    //             scheme.Equals(IPhoneAccount::SCHEME_VOICEMAIL);
    //         // Check for an obscure ECM-related scenario: If the phone
    //         // is currently in ECM (Emergency callback mode) and we
    //         // dial a non-emergency number, that automatically
    //         // *cancels* ECM.  So warn the user about it.
    //         // (See InCallScreen.showExitingECMDialog() for more info.)
    //         Boolean exitedEcm = FALSE;
    //         if (PhoneUtils::IsPhoneInEcm(phone) && !isEmergencyNumber) {
    //             Logger::I(TAG, "About to exit ECM because of an outgoing non-emergency call");
    //             exitedEcm = TRUE;  // this will cause us to return EXITED_ECM from this method
    //         }

    //         Int32 type;
    //         phone->GetPhoneType(&type);
    //         if (type == IPhoneConstants::PHONE_TYPE_CDMA) {
    //             // Start the timer for 3 Way CallerInfo
    //             Int32 state;
    //             assert(0 && "TODO: Need IPhoneGlobals");
    //             // mApp->mCdmaPhoneCallState->GetCurrentCallState(&state);
    //             // if (state == CdmaPhoneCallState_THRWAY_ACTIVE) {

    //             //     // This is a "CDMA 3-way call", which means that you're dialing a
    //             //     // 2nd outgoing call while a previous call is already in progress.
    //             //     //
    //             //     // Due to the limitations of CDMA this call doesn't actually go
    //             //     // through the DIALING/ALERTING states, so we can't tell for sure
    //             //     // when (or if) it's actually answered.  But we want to show
    //             //     // *some* indication of what's going on in the UI, so we "fake it"
    //             //     // by displaying the "Dialing" state for 3 seconds.

    //             //     // Set the mThreeWayCallOrigStateDialing state to true
    //             //     mApp->mCdmaPhoneCallState->SetThreeWayCallOrigState(TRUE);

    //             //     // Schedule the "Dialing" indication to be taken down in 3 seconds:
    //             //     SendEmptyMessageDelayed(THREEWAY_CALLERINFO_DISPLAY_DONE,
    //             //                             THREEWAY_CALLERINFO_DISPLAY_TIME);
    //             // }
    //         }

    //         // Success!
    //         if (exitedEcm) {
    //             return Constants_EXITED_ECM;
    //         }
    //         else {
    //             return Constants_SUCCESS;
    //         }
    //     }
    //     case PhoneUtils::CALL_STATUS_DIALED_MMI:
    //         if (DBG) {
    //             StringBuilder sb;
    //             sb += "placeCall: specified number was an MMI code: '";
    //             sb += number;
    //             sb += "'.";
    //             Log(sb.ToString());
    //         }
    //         // The passed-in number was an MMI code, not a regular phone number!
    //         // This isn't really a failure; the Dialer may have deliberately
    //         // fired an ACTION_CALL intent to dial an MMI code, like for a
    //         // USSD call.
    //         //
    //         // Presumably an MMI_INITIATE message will come in shortly
    //         // (and we'll bring up the "MMI Started" dialog), or else
    //         // an MMI_COMPLETE will come in (which will take us to a
    //         // different Activity; see PhoneUtils.displayMMIComplete()).
    //         return Constants_DIALED_MMI;

    //     case PhoneUtils::CALL_STATUS_FAILED:
    //         Logger::W(TAG, "placeCall: PhoneUtils.placeCall() FAILED for number '%s '.", number.tostring());
    //         // We couldn't successfully place the call; there was some
    //         // failure in the telephony layer.

    //         // Log failed call.
    //         mCallLogger->LogCall(NULL /* callerInfo */, number, 0 /* presentation */,
    //                 ICalls::OUTGOING_TYPE, System::GetCurrentTimeMillis(), 0 /* duration */);

    //         return Constants_CALL_FAILED;

    //     default: {
    //         StringBuilder sb;
    //         sb += "placeCall: unknown callStatus ";
    //         sb += callStatus;
    //         sb += " from PhoneUtils.placeCall() for number '";
    //         sb += number;
    //         sb += "'.";
    //         Logger::W(TAG, sb.ToString());
    //         return Constants_SUCCESS;  // Try to continue anyway...
    //     }
    // }
}

ECode CallController::CheckIfOkToInitiateOutgoingCall(
    /* [in] */ Int32 state,
    /* [out] */ ConstantsCallStatusCode* result)
{
    VALIDATE_NOT_NULL(result)

    if (VDBG) Log(String("checkIfOkToInitiateOutgoingCall: ServiceState = ") + StringUtils::ToString(state));

    switch (state) {
        case IServiceState::STATE_IN_SERVICE:
            // Normal operation.  It's OK to make outgoing calls.
            *result = Constants_SUCCESS;
            return NOERROR;

        case IServiceState::STATE_POWER_OFF:
            // Radio is explictly powered off.
            *result = Constants_POWER_OFF;
            return NOERROR;

        case IServiceState::STATE_EMERGENCY_ONLY:
            // The phone is registered, but locked. Only emergency
            // numbers are allowed.
            // Note that as of Android 2.0 at least, the telephony layer
            // does not actually use ServiceState.STATE_EMERGENCY_ONLY,
            // mainly since there's no guarantee that the radio/RIL can
            // make this distinction.  So in practice the
            // CallStatusCode.EMERGENCY_ONLY state and the string
            // "incall_error_emergency_only" are totally unused.
            *result = Constants_EMERGENCY_ONLY;
            return NOERROR;

        case IServiceState::STATE_OUT_OF_SERVICE:
            // No network connection.
            *result = Constants_OUT_OF_SERVICE;
            return NOERROR;

        default:
            //throw new IllegalStateException("Unexpected ServiceState: " + state);
            Logger::E(TAG, "Unexpected ServiceState: %d", state);
            return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

void CallController::HandleOutgoingCallError(
    /* [in] */ ConstantsCallStatusCode status)
{
    if (DBG) Log(String("handleOutgoingCallError(): status = ") + StringUtils::ToString(status));
    AutoPtr<IIntent> intent;
    CIntent::New(IContext::Probe(mApp), ECLSID_CErrorDialogActivity, (IIntent**)&intent);
    Int32 errorMessageId = -1;
    switch (status) {
        case Constants_SUCCESS:
            // This case shouldn't happen; you're only supposed to call
            // handleOutgoingCallError() if there was actually an error!
            Logger::W(TAG, "handleOutgoingCallError: SUCCESS isn't an error");
            break;

        case Constants_CALL_FAILED:
            // We couldn't successfully place the call; there was some
            // failure in the telephony layer.
            // TODO: Need UI spec for this failure case; for now just
            // show a generic error.
            errorMessageId = R::string::incall_error_call_failed;
            break;
        case Constants_POWER_OFF:
            // Radio is explictly powered off, presumably because the
            // device is in airplane mode.
            //
            // TODO: For now this UI is ultra-simple: we simply display
            // a message telling the user to turn off airplane mode.
            // But it might be nicer for the dialog to offer the option
            // to turn the radio on right there (and automatically retry
            // the call once network registration is complete.)
            errorMessageId = R::string::incall_error_power_off;
            break;
        case Constants_EMERGENCY_ONLY:
            // Only emergency numbers are allowed, but we tried to dial
            // a non-emergency number.
            // (This state is currently unused; see comments above.)
            errorMessageId = R::string::incall_error_emergency_only;
            break;
        case Constants_OUT_OF_SERVICE:
            // No network connection.
            errorMessageId = R::string::incall_error_out_of_service;
            break;
        case Constants_NO_PHONE_NUMBER_SUPPLIED:
            // The supplied Intent didn't contain a valid phone number.
            // (This is rare and should only ever happen with broken
            // 3rd-party apps.) For now just show a generic error.
            errorMessageId = R::string::incall_error_no_phone_number_supplied;
            break;

        case Constants_VOICEMAIL_NUMBER_MISSING:
            // Bring up the "Missing Voicemail Number" dialog, which
            // will ultimately take us to some other Activity (or else
            // just bail out of this activity.)

            // Send a request to the InCallScreen to display the
            // "voicemail missing" dialog when it (the InCallScreen)
            // comes to the foreground.
            intent->PutExtra(IErrorDialogActivity::SHOW_MISSING_VOICEMAIL_NO_DIALOG_EXTRA, TRUE);
            break;

        case Constants_DIALED_MMI: {
            // Our initial phone number was actually an MMI sequence.
            // There's no real "error" here, but we do bring up the
            // a Toast (as requested of the New UI paradigm).
            //
            // In-call MMIs do not trigger the normal MMI Initiate
            // Notifications, so we should notify the user here.
            // Otherwise, the code in PhoneUtils.java should handle
            // user notifications in the form of Toasts or Dialogs.
            //
            // TODO: Rather than launching a toast from here, it would
            // be cleaner to just set a pending call status code here,
            // and then let the InCallScreen display the toast...
            AutoPtr<IIntent> mmiIntent;
            CIntent::New(IContext::Probe(mApp), ECLSID_CMMIDialogActivity, (IIntent**)&mmiIntent);
            mmiIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK |
                    IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
            IContext::Probe(mApp)->StartActivity(mmiIntent);
            return;
        }
        default:
            Logger::D(TAG, "handleOutgoingCallError: unexpected status code %d" , status);
            // Show a generic "call failed" error.
            errorMessageId = R::string::incall_error_call_failed;
            break;
    }
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK | IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
    if (errorMessageId != -1) {
        intent->PutExtra(IErrorDialogActivity::ERROR_MESSAGE_ID_EXTRA, errorMessageId);
    }
    IContext::Probe(mApp)->StartActivity(intent);
}

void CallController::CheckForOtaspCall(
    /* [in] */ IIntent* intent)
{
    Boolean res;
    if (OtaUtils::IsOtaspCallIntent(intent, &res), res) {
        Logger::I(TAG, "checkForOtaspCall: handling OTASP intent! %s", TO_CSTR(intent));

        // ("OTASP-specific setup" basically means creating and initializing
        // the OtaUtils instance.  Note that this setup needs to be here in
        // the CallController.placeCall() sequence, *not* in
        // OtaUtils.startInteractiveOtasp(), since it's also possible to
        // start an OTASP call by manually dialing "*228" (in which case
        // OtaUtils.startInteractiveOtasp() never gets run at all.)
        OtaUtils::SetupOtaspCall(intent);
    }
    else {
        if (DBG) Log(String("checkForOtaspCall: not an OTASP call."));
    }
}

void CallController::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, msg);
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos