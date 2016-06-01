
#include "elastos/droid/internal/telephony/WakeLockStateMachine.h"
#include "elastos/droid/os/Build.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const Int32 WakeLockStateMachine::EVENT_NEW_SMS_MESSAGE = 1;
const Boolean WakeLockStateMachine::DBG = TRUE;    // TODO: change to FALSE
/** Result receiver called for current cell broadcast. */
const Int32 WakeLockStateMachine::EVENT_BROADCAST_COMPLETE = 2;
/** Release wakelock after a short timeout when returning to idle state. */
const Int32 WakeLockStateMachine::EVENT_RELEASE_WAKE_LOCK = 3;
const Int32 WakeLockStateMachine::EVENT_UPDATE_PHONE_OBJECT = 4;
/** Wakelock release delay when returning to idle state. */
const Int32 WakeLockStateMachine::WAKE_LOCK_TIMEOUT = 3000;

WakeLockStateMachine::DefaultState::DefaultState(
    /* [in] */ WakeLockStateMachine* host)
    : mHost(host)
{}

ECode WakeLockStateMachine::DefaultState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 what = 0;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_UPDATE_PHONE_OBJECT: {
            AutoPtr<IInterface> obj;
            mHost->mPhone = IPhoneBase::Probe(obj);
            mHost->Log(String("updatePhoneObject: phone=") + TO_CSTR(mHost->mPhone)/*.getClass().getSimpleName()*/);
            break;
        }
        default: {
            String errorText = String("processMessage: unhandled message type ") + StringUtils::ToString(what);
            if (Build::IS_DEBUGGABLE) {
                // throw new RuntimeException(errorText);
                *result = FALSE;
                return E_RUNTIME_EXCEPTION;
            }
            else {
                mHost->Loge(errorText);
            }
            break;
        }
    }
    *result = HANDLED;
    return NOERROR;
}

String WakeLockStateMachine::DefaultState::GetName()
{
    return String("Elastos.Droid.Internal.Telephony.WakeLockStateMachine.DefaultState");
}

WakeLockStateMachine::IdleState::IdleState(
    /* [in] */ WakeLockStateMachine* host)
    : mHost(host)
{}

ECode WakeLockStateMachine::IdleState::Enter()
{
    mHost->SendMessageDelayed(EVENT_RELEASE_WAKE_LOCK, WAKE_LOCK_TIMEOUT);
    return NOERROR;
}

ECode WakeLockStateMachine::IdleState::Exit()
{
    mHost->mWakeLock->AcquireLock();
    if (DBG) mHost->Log(String("acquired wakelock, leaving Idle state"));
    return NOERROR;
}

ECode WakeLockStateMachine::IdleState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 what = 0;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_NEW_SMS_MESSAGE: {
            // transition to waiting state if we sent a broadcast
            Boolean tmp = FALSE;
            if (mHost->HandleSmsMessage(msg, &tmp), tmp) {
                mHost->TransitionTo(mHost->mWaitingState);
            }
            *result = HANDLED;
            return NOERROR;
        }

        case EVENT_RELEASE_WAKE_LOCK: {
            mHost->mWakeLock->ReleaseLock();
            if (DBG) {
                Boolean tmp = FALSE;
                if (mHost->mWakeLock->IsHeld(&tmp), tmp) {
                    // this is okay as long as we call release() for every acquire()
                    mHost->Log(String("mWakeLock is still held after release"));
                }
                else {
                    mHost->Log(String("mWakeLock released"));
                }
            }
            *result = HANDLED;
            return NOERROR;
        }

        default: {
            *result = NOT_HANDLED;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

String WakeLockStateMachine::IdleState::GetName()
{
    return String("Elastos.Droid.Internal.Telephony.WakeLockStateMachine.IdleState");
}

WakeLockStateMachine::WaitingState::WaitingState(
    /* [in] */ WakeLockStateMachine* host)
    : mHost(host)
{}

ECode WakeLockStateMachine::WaitingState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 what = 0;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_NEW_SMS_MESSAGE: {
            mHost->Log(String("deferring message until return to idle"));
            mHost->DeferMessage(msg);
            *result = HANDLED;
            return NOERROR;
        }

        case EVENT_BROADCAST_COMPLETE: {
            mHost->Log(String("broadcast complete, returning to idle"));
            mHost->TransitionTo(mHost->mIdleState);
            *result = HANDLED;
            return NOERROR;
        }

        case EVENT_RELEASE_WAKE_LOCK: {
            mHost->mWakeLock->ReleaseLock();    // decrement wakelock from previous entry to Idle
            Boolean tmp = FALSE;
            if (mHost->mWakeLock->IsHeld(&tmp), !tmp) {
                // wakelock should still be held until 3 seconds after we enter Idle
                mHost->Loge(String("mWakeLock released while still in WaitingState!"));
            }
            *result = HANDLED;
            return NOERROR;
        }

        default: {
            *result = NOT_HANDLED;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

String WakeLockStateMachine::WaitingState::GetName()
{
    return String("Elastos.Droid.Internal.Telephony.WakeLockStateMachine.WaitingState");
}

WakeLockStateMachine::Receiver::Receiver(
    /* [in] */ WakeLockStateMachine* host)
    : mHost(host)
{
    BroadcastReceiver::constructor();
}

ECode WakeLockStateMachine::Receiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->SendMessage(EVENT_BROADCAST_COMPLETE);
    return NOERROR;
}

CAR_INTERFACE_IMPL(WakeLockStateMachine, StateMachine, IWakeLockStateMachine)
WakeLockStateMachine::WakeLockStateMachine()
{}

ECode WakeLockStateMachine::constructor(
    /* [in] */ const String& debugTag,
    /* [in] */ IContext* context,
    /* [in] */ IPhoneBase* phone)
{
    StateMachine::constructor(debugTag);
    mDefaultState = new DefaultState(this);
    mIdleState = new IdleState(this);
    mWaitingState = new WaitingState(this);
    mReceiver = new Receiver(this);
    mContext = context;
    mPhone = phone;

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(obj);
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, debugTag, (IPowerManagerWakeLock**)&mWakeLock);
    mWakeLock->AcquireLock();    // wake lock released after we enter idle state

    AddState(mDefaultState);
    AddState(mIdleState, mDefaultState);
    AddState(mWaitingState, mDefaultState);
    SetInitialState(mIdleState);
    return NOERROR;
}

ECode WakeLockStateMachine::UpdatePhoneObject(
    /* [in] */ IPhoneBase* phone)
{
    SendMessage(EVENT_UPDATE_PHONE_OBJECT, phone);
    return NOERROR;
}

/**
 * Tell the state machine to quit after processing all messages.
 */
ECode WakeLockStateMachine::Dispose()
{
    Quit();
    return NOERROR;
}

void WakeLockStateMachine::OnQuitting()
{
    // fully release the wakelock
    Boolean tmp = FALSE;
    while (mWakeLock->IsHeld(&tmp), tmp) {
        mWakeLock->ReleaseLock();
    }
}

/**
 * Send a message with the specified object for {@link #handleSmsMessage}.
 * @param obj the object to pass in the msg.obj field
 */
ECode WakeLockStateMachine::DispatchSmsMessage(
    /* [in] */ IInterface* obj)
{
    SendMessage(EVENT_NEW_SMS_MESSAGE, obj);
    return NOERROR;
}

void WakeLockStateMachine::Log(
    /* [in] */ const String& s)
{
    String n;
    GetName(&n);
    Logger::D(n, s);
}

/**
 * Log with error level.
 * @param s the string to log
 */
void WakeLockStateMachine::Loge(
    /* [in] */ const String& s)
{
    String n;
    GetName(&n);
    Logger::E(n, s);
}

/**
 * Log with error level.
 * @param s the string to log
 * @param e is a Throwable which logs additional information.
 */
void WakeLockStateMachine::Loge(
    /* [in] */ const String& s,
    /* [in] */ IThrowable* e)
{
    String n;
    GetName(&n);
    Logger::E(n, "%s, %s", s.string(), TO_CSTR(e));
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
