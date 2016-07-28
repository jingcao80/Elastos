
#include "elastos/droid/teleservice/services/telephony/EmergencyCallHelper.h"
#include "elastos/droid/os/AsyncResult.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Internal::Os::CSomeArgsHelper;
using Elastos::Droid::Internal::Os::ISomeArgsHelper;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState_OFFHOOK;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {

EmergencyCallHelper::MyHandler::MyHandler(
    /* [in] */ EmergencyCallHelper* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode EmergencyCallHelper::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_START_SEQUENCE:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);

            AutoPtr<IInterface> obj2;
            args->GetObjectArg(1, (IInterface**)&obj2);
            AutoPtr<IPhone> phone = IPhone::Probe(obj2);

            AutoPtr<IInterface> obj3;
            args->GetObjectArg(2, (IInterface**)&obj3);
            AutoPtr<IEmergencyCallHelperCallback> _callback =
                    IEmergencyCallHelperCallback::Probe(obj3);

            args->Recycle();

            mHost->StartSequenceInternal(phone, _callback);
            break;
        }
        case MSG_SERVICE_STATE_CHANGED:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<AsyncResult> args = (AsyncResult*)IAsyncResult::Probe(obj);

            mHost->OnServiceStateChanged(IServiceState::Probe(args->mResult));
            break;
        }
        case MSG_RETRY_TIMEOUT:
            mHost->OnRetryTimeout();
            break;
        default:
            Logger::W("EmergencyCallHelper", "handleMessage: unexpected message: %d.", what);
            break;
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(EmergencyCallHelper, Object, IEmergencyCallHelper)

EmergencyCallHelper::EmergencyCallHelper(
    /* [in] */ IContext* context)
    : mContext(context)
    , mNumRetriesSoFar(0)
{
    Logger::D("EmergencyCallHelper", "EmergencyCallHelper constructor.");
    mContext = context;

    mHandler = new MyHandler(this);
}

ECode EmergencyCallHelper::StartTurnOnRadioSequence(
    /* [in] */ IPhone* phone,
    /* [in] */ IEmergencyCallHelperCallback* _callback)
{
    Logger::D("EmergencyCallHelper", "startTurnOnRadioSequence");

    AutoPtr<ISomeArgsHelper> helper;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&helper);
    AutoPtr<ISomeArgs> args;
    helper->Obtain((ISomeArgs**)&args);
    args->SetObjectArg(1, TO_IINTERFACE(phone));
    args->SetObjectArg(2, TO_IINTERFACE(_callback));

    AutoPtr<IMessage> m;
    mHandler->ObtainMessage(MSG_START_SEQUENCE, args, (IMessage**)&m);
    return m->SendToTarget();
}

void EmergencyCallHelper::StartSequenceInternal(
    /* [in] */ IPhone* phone,
    /* [in] */ IEmergencyCallHelperCallback* _callback)
{
    Logger::D("EmergencyCallHelper", "startSequenceInternal()");

    // First of all, clean up any state left over from a prior emergency call sequence. This
    // ensures that we'll behave sanely if another startTurnOnRadioSequence() comes in while
    // we're already in the middle of the sequence.
    Cleanup();

    mPhone = phone;
    mCallback = _callback;


    // No need to check the current service state here, since the only reason to invoke this
    // method in the first place is if the radio is powered-off. So just go ahead and turn the
    // radio on.

    PowerOnRadio();  // We'll get an onServiceStateChanged() callback
                     // when the radio successfully comes up.

    // Next step: when the SERVICE_STATE_CHANGED event comes in, we'll retry the call; see
    // onServiceStateChanged(). But also, just in case, start a timer to make sure we'll retry
    // the call even if the SERVICE_STATE_CHANGED event never comes in for some reason.
    StartRetryTimer();
}

void EmergencyCallHelper::OnServiceStateChanged(
    /* [in] */ IServiceState* state)
{
    Logger::D("EmergencyCallHelper", "onServiceStateChanged(), new state = %s.", TO_CSTR(state));

    // Possible service states:
    // - STATE_IN_SERVICE        // Normal operation
    // - STATE_OUT_OF_SERVICE    // Still searching for an operator to register to,
    //                           // or no radio signal
    // - STATE_EMERGENCY_ONLY    // Phone is locked; only emergency numbers are allowed
    // - STATE_POWER_OFF         // Radio is explicitly powered off (airplane mode)

    Int32 state1, state2;
    state->GetState(&state1);
    mPhone->GetState(&state2);
    if (IsOkToCall(state1, state2)) {
        // Woo hoo!  It's OK to actually place the call.
        Logger::D("EmergencyCallHelper", "onServiceStateChanged: ok to call!");

        OnComplete(TRUE);
        Cleanup();
    }
    else {
        // The service state changed, but we're still not ready to call yet. (This probably was
        // the transition from STATE_POWER_OFF to STATE_OUT_OF_SERVICE, which happens
        // immediately after powering-on the radio.)
        //
        // So just keep waiting; we'll probably get to either STATE_IN_SERVICE or
        // STATE_EMERGENCY_ONLY very shortly. (Or even if that doesn't happen, we'll at least do
        // another retry when the RETRY_TIMEOUT event fires.)
        Logger::D("EmergencyCallHelper", "onServiceStateChanged: not ready to call yet, keep waiting.");
    }
}

Boolean EmergencyCallHelper::IsOkToCall(
    /* [in] */ Int32 serviceState,
    /* [in] */ PhoneConstantsState phoneState)
{
    // Once we reach either STATE_IN_SERVICE or STATE_EMERGENCY_ONLY, it's finally OK to place
    // the emergency call.
    return ((phoneState == PhoneConstantsState_OFFHOOK)
            || (serviceState == IServiceState::STATE_IN_SERVICE)
            || (serviceState == IServiceState::STATE_EMERGENCY_ONLY)) ||

            // Allow STATE_OUT_OF_SERVICE if we are at the max number of retries.
            (mNumRetriesSoFar == MAX_NUM_RETRIES &&
             serviceState == IServiceState::STATE_OUT_OF_SERVICE);
}

void EmergencyCallHelper::OnRetryTimeout()
{
    PhoneConstantsState phoneState;
    mPhone->GetState(&phoneState);

    AutoPtr<IServiceState> sState;
    mPhone->GetServiceState((IServiceState**)&sState);
    Int32 serviceState;
    sState->GetState(&serviceState);

    Logger::D("EmergencyCallHelper", "onRetryTimeout():  phone state = %s, service state = %d, retries = %d.",
           phoneState, serviceState, mNumRetriesSoFar);

    // - If we're actually in a call, we've succeeded.
    // - Otherwise, if the radio is now on, that means we successfully got out of airplane mode
    //   but somehow didn't get the service state change event.  In that case, try to place the
    //   call.
    // - If the radio is still powered off, try powering it on again.

    if (IsOkToCall(serviceState, phoneState)) {
        Logger::D("EmergencyCallHelper", "onRetryTimeout: Radio is on. Cleaning up.");

        // Woo hoo -- we successfully got out of airplane mode.
        OnComplete(TRUE);
        Cleanup();
    }
    else {
        // Uh oh; we've waited the full TIME_BETWEEN_RETRIES_MILLIS and the radio is still not
        // powered-on.  Try again.

        mNumRetriesSoFar++;
        Logger::D("EmergencyCallHelper", "mNumRetriesSoFar is now %d", mNumRetriesSoFar);

        if (mNumRetriesSoFar > MAX_NUM_RETRIES) {
            Logger::W("EmergencyCallHelper", "Hit MAX_NUM_RETRIES; giving up.");
            Cleanup();
        }
        else {
            Logger::D("EmergencyCallHelper", "Trying (again) to turn on the radio.");
            PowerOnRadio();  // Again, we'll (hopefully) get an onServiceStateChanged() callback
                             // when the radio successfully comes up.
            StartRetryTimer();
        }
    }
}

void EmergencyCallHelper::PowerOnRadio()
{
    Logger::D("EmergencyCallHelper", "powerOnRadio().");

    // We're about to turn on the radio, so arrange to be notified when the sequence is
    // complete.
    RegisterForServiceStateChanged();

    // If airplane mode is on, we turn it off the same way that the Settings activity turns it
    // off.
    AutoPtr<IContentResolver> contentResolver;
    mContext->GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<ISettingsGlobal> helper;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
    Int32 tmp;
    helper->GetInt32(contentResolver, ISettingsGlobal::AIRPLANE_MODE_ON, 0, &tmp);
    if (tmp > 0) {
        Logger::D("EmergencyCallHelper", "==> Turning off airplane mode.");

        // Change the system setting
        Boolean res;
        helper->PutInt32(contentResolver, ISettingsGlobal::AIRPLANE_MODE_ON, 0, &res);

        // Post the broadcast intend for change in airplane mode
        // TODO: We really should not be in charge of sending this broadcast.
        //     If changing the setting is sufficent to trigger all of the rest of the logic,
        //     then that should also trigger the broadcast intent.
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_AIRPLANE_MODE_CHANGED, (IIntent**)&intent);
        intent->PutExtra(String("state"), FALSE);

        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        AutoPtr<IUserHandle> handle;
        helper->GetALL((IUserHandle**)&handle);
        mContext->SendBroadcastAsUser(intent, handle);
    }
    else {
        // Otherwise, for some strange reason the radio is off (even though the Settings
        // database doesn't think we're in airplane mode.)  In this case just turn the radio
        // back on.
        Logger::D("EmergencyCallHelper", "==> (Apparently) not in airplane mode; manually powering radio on.");
        mPhone->SetRadioPower(TRUE);
    }
}

void EmergencyCallHelper::Cleanup()
{
    Logger::D("EmergencyCallHelper", "cleanup()");

    // This will send a failure call back if callback has yet to be invoked.  If the callback
    // was already invoked, it's a no-op.
    OnComplete(FALSE);

    UnregisterForServiceStateChanged();
    CancelRetryTimer();

    // Used for unregisterForServiceStateChanged() so we null it out here instead.
    mPhone = NULL;
    mNumRetriesSoFar = 0;
}

void EmergencyCallHelper::StartRetryTimer()
{
    CancelRetryTimer();
    Boolean res;
    mHandler->SendEmptyMessageDelayed(MSG_RETRY_TIMEOUT, TIME_BETWEEN_RETRIES_MILLIS,
            &res);
}

void EmergencyCallHelper::CancelRetryTimer()
{
    mHandler->RemoveMessages(MSG_RETRY_TIMEOUT);
}

void EmergencyCallHelper::RegisterForServiceStateChanged()
{
    // Unregister first, just to make sure we never register ourselves twice.  (We need this
    // because Phone.registerForServiceStateChanged() does not prevent multiple registration of
    // the same handler.)
    UnregisterForServiceStateChanged();
    mPhone->RegisterForServiceStateChanged(mHandler, MSG_SERVICE_STATE_CHANGED, NULL);
}

void EmergencyCallHelper::UnregisterForServiceStateChanged()
{
    // This method is safe to call even if we haven't set mPhone yet.
    if (mPhone != NULL) {
        mPhone->UnregisterForServiceStateChanged(mHandler);  // Safe even if unnecessary
    }
    mHandler->RemoveMessages(MSG_SERVICE_STATE_CHANGED);  // Clean up any pending messages too
}

void EmergencyCallHelper::OnComplete(
    /* [in] */ Boolean isRadioReady)
{
    if (mCallback != NULL) {
        AutoPtr<IEmergencyCallHelperCallback> tempCallback = mCallback;
        mCallback = NULL;
        tempCallback->OnComplete(isRadioReady);
    }
}

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos