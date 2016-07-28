
#include "elastos/droid/teleservice/phone/EmergencyCallHelper.h"
#include "elastos/droid/teleservice/phone/CallController.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "elastos/droid/teleservice/phone/PhoneUtils.h"
#include "elastos/droid/os/AsyncResult.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState_OFFHOOK;
using Elastos::Droid::Telephony::CDisconnectCause;
using Elastos::Droid::Telephony::IDisconnectCause;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

const String EmergencyCallHelper::TAG("EmergencyCallHelper");
const Boolean EmergencyCallHelper::DBG = FALSE;

const Int32 EmergencyCallHelper::START_SEQUENCE = 1;
const Int32 EmergencyCallHelper::SERVICE_STATE_CHANGED = 2;
const Int32 EmergencyCallHelper::DISCONNECT = 3;
const Int32 EmergencyCallHelper::RETRY_TIMEOUT = 4;

CAR_INTERFACE_IMPL(EmergencyCallHelper, Handler, IEmergencyCallHelper)

EmergencyCallHelper::EmergencyCallHelper()
    : mNumber(NULL)
    , mNumRetriesSoFar(0)
{
}

ECode EmergencyCallHelper::constructor(
    /* [in] */ CallController* callController)
{
    Handler::constructor();

    if (DBG) Log(String("EmergencyCallHelper constructor..."));
    mCallController = callController;
    PhoneGlobals::GetInstance((PhoneGlobals**)&mApp);
    mCM = mApp->mCM;
    return NOERROR;
}

ECode EmergencyCallHelper::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case START_SEQUENCE:
            StartSequenceInternal(msg);
            break;
        case SERVICE_STATE_CHANGED:
            OnServiceStateChanged(msg);
            break;
        case DISCONNECT:
            OnDisconnect(msg);
            break;
        case RETRY_TIMEOUT:
            OnRetryTimeout();
            break;
        default:
            StringBuilder sb;
            sb += "handleMessage: unexpected message: ";
            sb += TO_CSTR(msg);
            Logger::W(TAG, sb.ToString());
            break;
    }
    return NOERROR;
}

ECode EmergencyCallHelper::StartEmergencyCallFromAirplaneModeSequence(
    /* [in] */ const String& number)
{
    if (DBG) {
        StringBuilder sb;
        sb += "startEmergencyCallFromAirplaneModeSequence('";
        sb += number;
        sb += "')...";
        Log(sb.ToString());
    }
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(number);
    AutoPtr<IMessage> msg;
    ObtainMessage(START_SEQUENCE, TO_IINTERFACE(cchar), (IMessage**)&msg);
    Boolean res;
    return SendMessage(msg, &res);
}

void EmergencyCallHelper::StartSequenceInternal(
    /* [in] */ IMessage* msg)
{
    if (DBG) {
        StringBuilder sb;
        sb += "startSequenceInternal(): msg = ";
        sb += TO_CSTR(msg);
        Log(sb.ToString());
    }

    // First of all, clean up any state (including mPartialWakeLock!)
    // left over from a prior emergency call sequence.
    // This ensures that we'll behave sanely if another
    // startEmergencyCallFromAirplaneModeSequence() comes in while
    // we're already in the middle of the sequence.
    Cleanup();

    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<ICharSequence> cchar = ICharSequence::Probe(obj);
    cchar->ToString(&mNumber);
    if (DBG) {
        StringBuilder sb;
        sb += "- startSequenceInternal: Got mNumber: '";
        sb += mNumber;
        sb += "'";
        Log(sb.ToString());
    }

    mNumRetriesSoFar = 0;

    // Wake lock to make sure the processor doesn't go to sleep midway
    // through the emergency call sequence.
    AutoPtr<IInterface> obj2;
    IContext::Probe(mApp)->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj2);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(obj2);
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, TAG, (IPowerManagerWakeLock**)&mPartialWakeLock);
    // Acquire with a timeout, just to be sure we won't hold the wake
    // lock forever even if a logic bug (in this class) causes us to
    // somehow never call cleanup().
    if (DBG) Log(String("- startSequenceInternal: acquiring wake lock"));
    mPartialWakeLock->AcquireLock(IEmergencyCallHelper::WAKE_LOCK_TIMEOUT);

    // No need to check the current service state here, since the only
    // reason the CallController would call this method in the first
    // place is if the radio is powered-off.
    //
    // So just go ahead and turn the radio on.

    PowerOnRadio();  // We'll get an onServiceStateChanged() callback
                     // when the radio successfully comes up.

    // Next step: when the SERVICE_STATE_CHANGED event comes in,
    // we'll retry the call; see placeEmergencyCall();
    // But also, just in case, start a timer to make sure we'll retry
    // the call even if the SERVICE_STATE_CHANGED event never comes in
    // for some reason.
    StartRetryTimer();

    // (Our caller is responsible for calling mApp.displayCallScreen().)
}

void EmergencyCallHelper::OnServiceStateChanged(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<AsyncResult> aResult = (AsyncResult*)IAsyncResult::Probe(obj);
    AutoPtr<IServiceState> state = IServiceState::Probe(aResult->mResult);
    if (DBG) {
        StringBuilder sb;
        sb += "onServiceStateChanged()...  new state = ";
        sb += TO_CSTR(state);
        Log(sb.ToString());
    }

    // Possible service states:
    // - STATE_IN_SERVICE        // Normal operation
    // - STATE_OUT_OF_SERVICE    // Still searching for an operator to register to,
    //                           // or no radio signal
    // - STATE_EMERGENCY_ONLY    // Phone is locked; only emergency numbers are allowed
    // - STATE_POWER_OFF         // Radio is explicitly powered off (airplane mode)

    // Once we reach either STATE_IN_SERVICE or STATE_EMERGENCY_ONLY,
    // it's finally OK to place the emergency call.
    Int32 _state;
    state->GetState(&_state);
    Boolean okToCall = (_state == IServiceState::STATE_IN_SERVICE)
            || (_state == IServiceState::STATE_EMERGENCY_ONLY);

    if (okToCall) {
        // Woo hoo!  It's OK to actually place the call.
        if (DBG) Log(String("onServiceStateChanged: ok to call!"));

        // Deregister for the service state change events.
        UnregisterForServiceStateChanged();

        PlaceEmergencyCall();
    }
    else {
        // The service state changed, but we're still not ready to call yet.
        // (This probably was the transition from STATE_POWER_OFF to
        // STATE_OUT_OF_SERVICE, which happens immediately after powering-on
        // the radio.)
        //
        // So just keep waiting; we'll probably get to either
        // STATE_IN_SERVICE or STATE_EMERGENCY_ONLY very shortly.
        // (Or even if that doesn't happen, we'll at least do another retry
        // when the RETRY_TIMEOUT event fires.)
        if (DBG) Log(String("onServiceStateChanged: not ready to call yet, keep waiting..."));
    }
}

void EmergencyCallHelper::OnDisconnect(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<AsyncResult> aResult = (AsyncResult*)IAsyncResult::Probe(obj);
    AutoPtr<Elastos::Droid::Internal::Telephony::IConnection> conn =
            Elastos::Droid::Internal::Telephony::IConnection::Probe(aResult->mResult);
    Int32 cause;
    conn->GetDisconnectCause(&cause);
    if (DBG) {
        StringBuilder sb;
        sb += "onDisconnect: connection '";
        sb += TO_CSTR(conn);
        sb += "', addr '";
        String address;
        conn->GetAddress(&address);
        sb += address;
        sb += "', cause = ";
        AutoPtr<IDisconnectCause> helper;
        CDisconnectCause::AcquireSingleton((IDisconnectCause**)&helper);
        String str;
        helper->ToString(cause, &str);
        sb += str;
        Log(sb.ToString());
    }

    if (cause == IDisconnectCause::OUT_OF_SERVICE) {
        // Wait a bit more and try again (or just bail out totally if
        // we've had too many failures.)
        if (DBG) Log(String("- onDisconnect: OUT_OF_SERVICE, need to retry..."));
        ScheduleRetryOrBailOut();
    }
    else {
        // Any other disconnect cause means we're done.
        // Either the emergency call succeeded *and* ended normally,
        // or else there was some error that we can't retry.  In either
        // case, just clean up our internal state.)

        if (DBG) Log(String("==> Disconnect event; clean up..."));
        Cleanup();

        // Nothing else to do here.  If the InCallScreen was visible,
        // it would have received this disconnect event too (so it'll
        // show the "Call ended" state and finish itself without any
        // help from us.)
    }
}

void EmergencyCallHelper::OnRetryTimeout()
{
    PhoneConstantsState phoneState;
    mCM->GetState(&phoneState);

    AutoPtr<IPhone> phone;
    mCM->GetDefaultPhone((IPhone**)&phone);
    AutoPtr<IServiceState> sState;
    phone->GetServiceState((IServiceState**)&sState);
    Int32 serviceState;
    sState->GetState(&serviceState);
    if (DBG) {
        StringBuilder sb;
        sb += "onRetryTimeout():  phone state ";
        sb += phoneState;
        sb += ", service state ";
        sb += serviceState;
        sb += ", mNumRetriesSoFar = ";
        sb += mNumRetriesSoFar;
        Log(sb.ToString());
    }

    // - If we're actually in a call, we've succeeded.
    //
    // - Otherwise, if the radio is now on, that means we successfully got
    //   out of airplane mode but somehow didn't get the service state
    //   change event.  In that case, try to place the call.
    //
    // - If the radio is still powered off, try powering it on again.

    if (phoneState == PhoneConstantsState_OFFHOOK) {
        if (DBG) Log(String("- onRetryTimeout: Call is active!  Cleaning up..."));
        Cleanup();
        return;
    }

    if (serviceState != IServiceState::STATE_POWER_OFF) {
        // Woo hoo -- we successfully got out of airplane mode.

        // Deregister for the service state change events; we don't need
        // these any more now that the radio is powered-on.
        UnregisterForServiceStateChanged();

        PlaceEmergencyCall();  // If the call fails, placeEmergencyCall()
                               // will schedule a retry.
    }
    else {
        // Uh oh; we've waited the full TIME_BETWEEN_RETRIES and the
        // radio is still not powered-on.  Try again...

        if (DBG) Log(String("- Trying (again) to turn on the radio..."));
        PowerOnRadio();  // Again, we'll (hopefully) get an onServiceStateChanged()
                         // callback when the radio successfully comes up.

        // ...and also set a fresh retry timer (or just bail out
        // totally if we've had too many failures.)
        ScheduleRetryOrBailOut();
    }
}

void EmergencyCallHelper::PowerOnRadio()
{
    if (DBG) Log(String("- powerOnRadio()..."));

    // We're about to turn on the radio, so arrange to be notified
    // when the sequence is complete.
    RegisterForServiceStateChanged();

    // If airplane mode is on, we turn it off the same way that the
    // Settings activity turns it off.
    AutoPtr<IContentResolver> contentResolver;
    IContext::Probe(mApp)->GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<ISettingsGlobal> helper;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
    Int32 value;
    helper->GetInt32(contentResolver, ISettingsGlobal::AIRPLANE_MODE_ON, 0, &value);
    if (value > 0) {
        if (DBG) Log(String("==> Turning off airplane mode..."));

        // Change the system setting
        Boolean res;
        helper->PutInt32(contentResolver, ISettingsGlobal::AIRPLANE_MODE_ON, 0, &res);

        // Post the intent
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_AIRPLANE_MODE_CHANGED, (IIntent**)&intent);
        intent->PutExtra(String("state"), FALSE);
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        AutoPtr<IUserHandle> allHandle;
        helper->GetALL((IUserHandle**)&allHandle);
        IContext::Probe(mApp)->SendBroadcastAsUser(intent, allHandle);
    }
    else {
        // Otherwise, for some strange reason the radio is off
        // (even though the Settings database doesn't think we're
        // in airplane mode.)  In this case just turn the radio
        // back on.
        if (DBG) Log(String("==> (Apparently) not in airplane mode; manually powering radio on..."));
        AutoPtr<IPhone> phone;
        mCM->GetDefaultPhone((IPhone**)&phone);
        phone->SetRadioPower(TRUE);
    }
}

void EmergencyCallHelper::PlaceEmergencyCall()
{
    if (DBG) Log(String("placeEmergencyCall()..."));

    // Place an outgoing call to mNumber.
    // Note we call PhoneUtils.placeCall() directly; we don't want any
    // of the behavior from CallController.placeCallInternal() here.
    // (Specifically, we don't want to start the "emergency call from
    // airplane mode" sequence from the beginning again!)

    RegisterForDisconnect();  // Get notified when this call disconnects

    if (DBG) {
        StringBuilder sb;
        sb += "- placing call to '";
        sb += mNumber;
        sb += "'...";
        Log(sb.ToString());
    }

    AutoPtr<IPhone> phone;
    mCM->GetDefaultPhone((IPhone**)&phone);
    Int32 callStatus = PhoneUtils::PlaceCall(mApp, phone, mNumber,
                NULL,  // contactUri
                TRUE); // isEmergencyCall

    if (DBG) {
        StringBuilder sb;
        sb += "- PhoneUtils.placeCall() returned status = ";
        sb += callStatus;
        Log(sb.ToString());
    }

    Boolean success;
    // Note PhoneUtils.placeCall() returns one of the CALL_STATUS_*
    // constants, not a CallStatusCode enum value.
    switch (callStatus) {
        case PhoneUtils::CALL_STATUS_DIALED:
            success = TRUE;
            break;

        case PhoneUtils::CALL_STATUS_DIALED_MMI:
        case PhoneUtils::CALL_STATUS_FAILED:
        default:
            // Anything else is a failure, and we'll need to retry.
            Logger::W(TAG, "placeEmergencyCall(): placeCall() failed: callStatus = %d", callStatus);
            success = FALSE;
            break;
    }

    if (success) {
        if (DBG) Log(String("==> Success from PhoneUtils.placeCall()!"));
        // Ok, the emergency call is (hopefully) under way.

        // We're not done yet, though, so don't call cleanup() here.
        // (It's still possible that this call will fail, and disconnect
        // with cause==OUT_OF_SERVICE.  If so, that will trigger a retry
        // from the onDisconnect() method.)
    }
    else {
        if (DBG) Log(String("==> Failure."));
        // Wait a bit more and try again (or just bail out totally if
        // we've had too many failures.)
        ScheduleRetryOrBailOut();
    }
}

void EmergencyCallHelper::ScheduleRetryOrBailOut()
{
    mNumRetriesSoFar++;
    if (DBG) {
        StringBuilder sb;
        sb += "scheduleRetryOrBailOut()...  mNumRetriesSoFar is now ";
        sb += mNumRetriesSoFar;
        Log(sb.ToString());
    }

    if (mNumRetriesSoFar > IEmergencyCallHelper::MAX_NUM_RETRIES) {
        Logger::W(TAG, "scheduleRetryOrBailOut: hit MAX_NUM_RETRIES; giving up...");
        Cleanup();
    }
    else {
        if (DBG) Log(String("- Scheduling another retry..."));
        StartRetryTimer();
    }
}

void EmergencyCallHelper::Cleanup()
{
    if (DBG) Log(String("cleanup()..."));

    UnregisterForServiceStateChanged();
    UnregisterForDisconnect();
    CancelRetryTimer();

    // Release / clean up the wake lock
    if (mPartialWakeLock != NULL) {
        Boolean res;
        if (mPartialWakeLock->IsHeld(&res), res) {
            if (DBG) Log(String("- releasing wake lock"));
            mPartialWakeLock->ReleaseLock();
        }
        mPartialWakeLock = NULL;
    }
}

void EmergencyCallHelper::StartRetryTimer()
{
    RemoveMessages(RETRY_TIMEOUT);
    Boolean res;
    SendEmptyMessageDelayed(RETRY_TIMEOUT, IEmergencyCallHelper::TIME_BETWEEN_RETRIES, &res);
}

void EmergencyCallHelper::CancelRetryTimer()
{
    RemoveMessages(RETRY_TIMEOUT);
}

void EmergencyCallHelper::RegisterForServiceStateChanged()
{
    // Unregister first, just to make sure we never register ourselves
    // twice.  (We need this because Phone.registerForServiceStateChanged()
    // does not prevent multiple registration of the same handler.)
    AutoPtr<IPhone> phone;
    mCM->GetDefaultPhone((IPhone**)&phone);
    phone->UnregisterForServiceStateChanged(this);  // Safe even if not currently registered
    phone->RegisterForServiceStateChanged(this, SERVICE_STATE_CHANGED, NULL);
}

void EmergencyCallHelper::UnregisterForServiceStateChanged()
{
    // This method is safe to call even if we haven't set mPhone yet.
    AutoPtr<IPhone> phone;
    mCM->GetDefaultPhone((IPhone**)&phone);
    if (phone != NULL) {
        phone->UnregisterForServiceStateChanged(this);  // Safe even if unnecessary
    }
    RemoveMessages(SERVICE_STATE_CHANGED);  // Clean up any pending messages too
}

void EmergencyCallHelper::RegisterForDisconnect()
{
    // Note: no need to unregister first, since
    // CallManager.registerForDisconnect() automatically prevents
    // multiple registration of the same handler.
    mCM->RegisterForDisconnect(this, DISCONNECT, NULL);
}

void EmergencyCallHelper::UnregisterForDisconnect()
{
    mCM->UnregisterForDisconnect(this);  // Safe even if not currently registered
    RemoveMessages(DISCONNECT);  // Clean up any pending messages too
}

void EmergencyCallHelper::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, msg);
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos