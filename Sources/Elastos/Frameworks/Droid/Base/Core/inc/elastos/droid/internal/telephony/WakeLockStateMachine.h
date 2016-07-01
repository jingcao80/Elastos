
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_WAKELOCKSTATEMACHINE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_WAKELOCKSTATEMACHINE_H__

#include "_Elastos.Droid.Internal.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/internal/utility/State.h"
#include "elastos/droid/internal/utility/StateMachine.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Internal::Utility::StateMachine;
using Elastos::Droid::Os::IPowerManagerWakeLock;

using Elastos::Core::IThrowable;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * Generic state machine for handling messages and waiting for ordered broadcasts to complete.
 * Subclasses implement {@link #handleSmsMessage}, which returns TRUE to transition into waiting
 * state, or FALSE to remain in idle state. The wakelock is acquired on exit from idle state,
 * and is released a few seconds after returning to idle state, or immediately upon calling
 * {@link #quit}.
 */
class WakeLockStateMachine
    : public StateMachine
    , public IWakeLockStateMachine
{
private:
    /**
     * This parent state throws an exception (for debug builds) or prints an error for unhandled
     * message types.
     */
    class DefaultState
        : public State
    {
    public:
        DefaultState(
            /* [in] */ WakeLockStateMachine* host);

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        WakeLockStateMachine* mHost;
    };

    /**
     * Idle state delivers Cell Broadcasts to receivers. It acquires the wakelock, which is
     * released when the broadcast completes.
     */
    class IdleState
        : public State
    {
    public:
        IdleState(
            /* [in] */ WakeLockStateMachine* host);

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI Exit();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        WakeLockStateMachine* mHost;
    };

    /**
     * Waiting state waits for the result receiver to be called for the current cell broadcast.
     * In this state, any new cell broadcasts are deferred until we return to Idle state.
     */
    class WaitingState
        : public State
    {
    public:
        WaitingState(
            /* [in] */ WakeLockStateMachine* host);

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        WakeLockStateMachine* mHost;
    };

    /**
     * BroadcastReceiver to send message to return to idle state.
     */
    class Receiver
        : public BroadcastReceiver
    {
    public:
        Receiver(
            /* [in] */ WakeLockStateMachine* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        WakeLockStateMachine* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI UpdatePhoneObject(
        /* [in] */ IPhoneBase* phone);

    /**
     * Tell the state machine to quit after processing all messages.
     */
    CARAPI Dispose();

    /**
     * Send a message with the specified object for {@link #handleSmsMessage}.
     * @param obj the object to pass in the msg.obj field
     */
    CARAPI DispatchSmsMessage(
        /* [in] */ IInterface* obj);

    /**
     * Implemented by subclass to handle messages in {@link IdleState}.
     * @param message the message to process
     * @return TRUE to transition to {@link WaitingState}; FALSE to stay in {@link IdleState}
     */
    virtual CARAPI HandleSmsMessage(
        /* [in] */ IMessage* message,
        /* [out] */ Boolean* result) = 0;

protected:
    WakeLockStateMachine();

    CARAPI constructor(
        /* [in] */ const String& debugTag,
        /* [in] */ IContext* context,
        /* [in] */ IPhoneBase* phone);

    // @Override
    CARAPI_(void) OnQuitting();

    /**
     * Log with debug level.
     * @param s the string to log
     */
    // @Override
    CARAPI_(void) Log(
        /* [in] */ const String& s);

    /**
     * Log with error level.
     * @param s the string to log
     */
    // @Override
    CARAPI_(void) Loge(
        /* [in] */ const String& s);

    /**
     * Log with error level.
     * @param s the string to log
     * @param e is a Throwable which logs additional information.
     */
    // @Override
    CARAPI_(void) Loge(
        /* [in] */ const String& s,
        /* [in] */ IThrowable* e);

public:
    /** New message to process. */
    static const Int32 EVENT_NEW_SMS_MESSAGE;

protected:
    static const Boolean DBG;    // TODO: change to FALSE

    /** Result receiver called for current cell broadcast. */
    static const Int32 EVENT_BROADCAST_COMPLETE;

    /** Release wakelock after a short timeout when returning to idle state. */
    static const Int32 EVENT_RELEASE_WAKE_LOCK;

    static const Int32 EVENT_UPDATE_PHONE_OBJECT;

    AutoPtr<IPhoneBase> mPhone;

    AutoPtr<IContext> mContext;

    AutoPtr<IBroadcastReceiver> mReceiver;

private:
    AutoPtr<IPowerManagerWakeLock> mWakeLock;

    /** Wakelock release delay when returning to idle state. */
    static const Int32 WAKE_LOCK_TIMEOUT;

    AutoPtr<DefaultState> mDefaultState;
    AutoPtr<IdleState> mIdleState;
    AutoPtr<WaitingState> mWaitingState;
};


} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_WAKELOCKSTATEMACHINE_H__
