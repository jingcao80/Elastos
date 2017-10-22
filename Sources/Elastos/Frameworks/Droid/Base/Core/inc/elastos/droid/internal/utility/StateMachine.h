//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_STATEMACHINE_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_STATEMACHINE_H__

#include "elastos/droid/os/Handler.h"
#include "elastos/droid/internal/utility/State.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/Vector.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::ICollection;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::Vector;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

class ECO_PUBLIC StateMachine
    : public Object
    , public IStateMachine
{
public:
    /**
     * StateMachine logging record.
     * {@hide}
     */
    class LogRec
        : public Object
        , public IStateMachineLogRec
    {
    public:
        /**
         * Constructor
         *
         * @param msg
         * @param state that handled the message
         * @param orgState is the first state the received the message but
         * did not processes the message.
         */
        LogRec(
            /* [in] */ StateMachine* sm,
            /* [in] */ IMessage* msg,
            /* [in] */ const String& info,
            /* [in] */ IState* state,
            /* [in] */ IState* orgState,
            /* [in] */ IState* transToState);

        CAR_INTERFACE_DECL()

        /**
         * Update the information in the record.
         * @param state that handled the message
         * @param orgState is the first state the received the message but
         * did not processes the message.
         */
        CARAPI_(void) Update(
            /* [in] */ StateMachine* sm,
            /* [in] */ IMessage* msg,
            /* [in] */ const String& info,
            /* [in] */ IState* state,
            /* [in] */ IState* orgState,
            /* [in] */ IState* dstState);

        /**
         * @return time stamp
         */
        CARAPI_(Int64) GetTime();

        /**
         * @return msg.what
         */
        CARAPI_(Int64) GetWhat();

        /**
         * @return the command that was executing
         */
        CARAPI_(String) GetInfo();

        /**
         * @return the state that handled this message
         */
        CARAPI_(AutoPtr<IState>) GetState();

        /**
         * @return the state destination state if a transition is occurring or null if none.
         */
        CARAPI_(AutoPtr<IState>) GetDestState();

        /**
         * @return the original state that received the message.
         */
        CARAPI_(AutoPtr<IState>) GetOriginalState();

        /**
         * @return as string
         */
        CARAPI ToString(
            /* [out] */ String* str);

    private:
        AutoPtr<StateMachine> mSm;
        Int64 mTime;
        Int32 mWhat;
        String mInfo;
        AutoPtr<IState> mState;
        AutoPtr<IState> mOrgState;
        AutoPtr<IState> mDstState;
    };

private:
    /**
     * A list of log records including messages recently processed by the state machine.
     *
     * The class maintains a list of log records including messages
     * recently processed. The list is finite and may be set in the
     * constructor or by calling setSize. The public interface also
     * includes size which returns the number of recent records,
     * count which is the number of records processed since the
     * the last setSize, get which returns a record and
     * add which adds a record.
     */
    class LogRecords : public Object
    {
        friend class StateMachine;
    public:
        /**
         * private constructor use add
         */
        LogRecords();

        /**
         * Set size of messages to maintain and clears all current records.
         *
         * @param maxSize number of records to maintain at anyone time.
        */
        CARAPI_(void) SetSize(
            /* [in] */ Int32 maxSize);

        CARAPI_(void) SetLogOnlyTransitions(
            /* [in] */ Boolean enable);

        CARAPI_(Boolean) LogOnlyTransitions();

        /**
         * @return the number of recent records.
         */
        CARAPI_(Int32) GetSize();

        /**
         * @return the total number of records processed since size was set.
         */
        CARAPI_(Int32) Count();

        /**
         * Clear the list of records.
         */
        CARAPI_(void) Cleanup();

        /**
         * @return the information on a particular record. 0 is the oldest
         * record and size()-1 is the newest record. If the index is to
         * large null is returned.
         */
        CARAPI_(AutoPtr<LogRec>) Get(
            /* [in] */ Int32 index);

        /**
         * Add a processed message.
         *
         * @param msg
         * @param messageInfo to be stored
         * @param state that handled the message
         * @param orgState is the first state the received the message but
         * did not processes the message.
         */
        CARAPI_(void) Add(
            /* [in] */ StateMachine* sm,
            /* [in] */ IMessage* msg,
            /* [in] */ const String& messageInfo,
            /* [in] */ IState* state,
            /* [in] */ IState* orgState,
            /* [in] */ IState* transToState);

    private:
        static const Int32 DEFAULT_SIZE = 20;

        Vector< AutoPtr<LogRec> > mLogRecVector;
        Int32 mMaxSize;
        Int32 mOldestIndex;
        Int32 mCount;
        Boolean mLogOnlyTransitions;
    };

    class SmHandler
        : public Handler
    {
        friend class StateMachine;
    private:
        /**
         * Information about a state.
         * Used to maintain the hierarchy.
         */
        class StateInfo : public Object
        {
        public:
            StateInfo();

            using Object::ToString;

            /**
             * Convert StateInfo to string
             */
            CARAPI_(String) ToString();

        public:
            /** The state */
            AutoPtr<State> mState;

            /** The parent of this state, null if there is no parent */
            AutoPtr<StateInfo> mParentStateInfo;

            /** True when the state has been entered and on the stack */
            Boolean mActive;
        };

        /**
         * State entered when transitionToHaltingState is called.
         */
        class HaltingState : public State
        {
        public:
            HaltingState(
                /* [in] */ SmHandler* owner);

            // @Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* msg,
                /* [out] */ Boolean* result);

            CARAPI_(String) GetName()
            {
                return String("HaltingState");
            }

        private:
            SmHandler* mOwner;
        };

        /**
         * State entered when a valid quit message is handled.
         */
        class QuittingState : public State
        {
            // @Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* msg,
                /* [out] */ Boolean* result);

            CARAPI_(String) GetName()
            {
                return String("QuittingState");
            }
        };

    public:
        TO_STRING_IMPL("StateMachine::SmHandler")

        /**
         * Constructor
         *
         * @param looper for dispatching messages
         * @param sm the hierarchical state machine
         */
        SmHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ StateMachine* sm);

        /**
         * Handle messages sent to the state machine by calling
         * the current state's processMessage. It also handles
         * the enter/exit calls and placing any deferred messages
         * back onto the queue when transitioning to a new state.
         */
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        /**
         * Do any transitions
         */
        CARAPI_(void) PerformTransitions(
            /* [in] */ State* msgProcessedState,
            /* [in] */ IMessage* msg);

        /**
         * Cleanup all the static variables and the looper after the SM has been quit.
         */
        CARAPI_(void) CleanupAfterQuitting();

        /**
         * Complete the construction of the state machine.
         */
        CARAPI_(void) CompleteConstruction();

        /**
         * Process the message. If the current state doesn't handle
         * it, call the states parent and so on. If it is never handled then
         * call the state machines unhandledMessage method.
         */
        CARAPI_(AutoPtr<State>) ProcessMsg(
            /* [in] */ IMessage* msg);

        /**
         * Call the exit method for each state from the top of stack
         * up to the common ancestor state.
         */
        CARAPI_(void) InvokeExitMethods(
            /* [in] */ StateInfo* commonStateInfo);

        /**
         * Invoke the enter method starting at the entering index to top of state stack
         */
        CARAPI_(void) InvokeEnterMethods(
            /* [in] */ Int32 stateStackEnteringIndex);

        /**
         * Move the deferred message to the front of the message queue.
         */
        CARAPI_(void) MoveDeferredMessageAtFrontOfQueue();

        /**
         * Move the contents of the temporary stack to the state stack
         * reversing the order of the items on the temporary stack as
         * they are moved.
         *
         * @return index into mStateStack where entering needs to start
         */
        CARAPI_(Int32) MoveTempStateStackToStateStack();

        /**
         * Setup the mTempStateStack with the states we are going to enter.
         *
         * This is found by searching up the destState's ancestors for a
         * state that is already active i.e. StateInfo.active == true.
         * The destStae and all of its inactive parents will be on the
         * TempStateStack as the list of states to enter.
         *
         * @return StateInfo of the common ancestor for the destState and
         * current state or null if there is no common parent.
         */
        CARAPI_(AutoPtr<StateInfo>) SetupTempStateStackWithStatesToEnter(
            /* [in] */ State* destState);

        /**
         * Initialize StateStack to mInitialState.
         */
        CARAPI_(void) SetupInitialStateStack();

        /**
         * @return current message
         */
        CARAPI_(AutoPtr<IMessage>) GetCurrentMessage();

        /**
         * @return current state
         */
        CARAPI_(AutoPtr<IState>) GetCurrentState();

        /**
         * Add a new state to the state machine. Bottom up addition
         * of states is allowed but the same state may only exist
         * in one hierarchy.
         *
         * @param state the state to add
         * @param parent the parent of state
         * @return stateInfo for this state
         */
        CARAPI AddState(
            /* [in] */ State* state,
            /* [in] */ State* parent,
            /* [out] */ StateInfo** info);

        /** @see StateMachine#setInitialState(State) */
        CARAPI_(void) SetInitialState(
            /* [in] */ State* initialState);

        /** @see StateMachine#transitionTo(IState) */
        CARAPI_(void) TransitionTo(
            /* [in] */ IState* destState);

        /** @see StateMachine#deferMessage(Message) */
        CARAPI_(void) DeferMessage(
            /* [in] */ IMessage* msg);

        /** @see StateMachine#quit() */
        CARAPI_(void) Quit();

        /** @see StateMachine#quitNow() */
        CARAPI_(void) QuitNow();

        /** Validate that the message was sent by quit or abort. */
        CARAPI_(Boolean) IsQuit(
            /* [in] */ IMessage* msg);

        /** @see StateMachine#isDbg() */
        CARAPI_(Boolean) IsDbg();

        /** @see StateMachine#setDbg(boolean) */
        CARAPI_(void) SetDbg(
            /* [in] */ Boolean dbg);

    private:
        /** true if StateMachine has quit */
        Boolean mHasQuit;

        /** The debug flag */
        Boolean mDbg;

        /** The SmHandler object, identifies that message is internal */
        static const AutoPtr<IInterface> sSmHandlerObj;

        /** The current message */
        AutoPtr<IMessage> mMsg;

        /** A list of log records including messages this state machine has processed */
        AutoPtr<LogRecords> mLogRecords;

        /** true if construction of the state machine has not been completed */
        Boolean mIsConstructionCompleted;

        /** Stack used to manage the current hierarchy of states */
        AutoPtr< ArrayOf<StateInfo*> > mStateStack;

        /** Top of mStateStack */
        Int32 mStateStackTopIndex;

        /** A temporary stack used to manage the state stack */
        AutoPtr< ArrayOf<StateInfo*> > mTempStateStack;

        /** The top of the mTempStateStack */
        Int32 mTempStateStackCount;

        /** State used when state machine is halted */
        AutoPtr<HaltingState> mHaltingState;

        /** State used when state machine is quitting */
        AutoPtr<QuittingState> mQuittingState;

        /** Reference to the StateMachine */
        // TODO: StateMachine can't be delete before HandleMessage return
        //
        AutoPtr<StateMachine> mSm;

        /** The map of all of the states in the state machine */
        HashMap< AutoPtr<State>, AutoPtr<StateInfo> > mStateInfo;

        /** The initial state that will process the first message */
        AutoPtr<State> mInitialState;

        /** The destination state when transitionTo has been invoked */
        AutoPtr<State> mDestState;

        /** The list of deferred messages */
        List< AutoPtr<IMessage> > mDeferredMessages;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * @return the name
     */
    CARAPI GetName(
        /* [out] */ String* name);

    /**
     * Set number of log records to maintain and clears all current records.
     *
     * @param maxSize number of messages to maintain at anyone time.
     */
    CARAPI SetLogRecSize(
        /* [in] */ Int32 maxSize);

    /**
     * Set to log only messages that cause a state transition
     *
     * @param enable {@code true} to enable, {@code false} to disable
     */
    CARAPI SetLogOnlyTransitions(
        /* [in] */ Boolean enable);

    /**
     * @return number of log records
     */
    CARAPI GetLogRecSize(
        /* [out] */ Int32* size);

    /**
     * @return the total number of records processed
     */
    CARAPI GetLogRecCount(
        /* [out] */ Int32* count);

    /**
     * @return a log record
     */
    CARAPI GetLogRec(
        /* [in] */ Int32 index,
        /* [out] */ IStateMachineLogRec** logRec);

    /**
     * @return a copy of LogRecs as a collection
     */
    CARAPI CopyLogRecs(
        /* [out] */ ICollection** collection);

    /**
     * @return Handler
     */
    CARAPI GetHandler(
        /* [out] */ IHandler** handler);

    /**
     * Get a message and set Message.target state machine handler,
     * what, arg1, arg2 and obj
     *
     * Note: The handler can be null if the state machine has quit,
     * which means target will be null and may cause a AndroidRuntimeException
     * in MessageQueue#enqueMessage if sent directly or if sent using
     * StateMachine#sendMessage the message will just be ignored.
     *
     * @return  A Message object from the global pool
     */
    CARAPI ObtainMessage(
        /* [out] */ IMessage** msg);

    /**
     * Get a message and set Message.target state machine handler,
     * what, arg1, arg2 and obj
     *
     * Note: The handler can be null if the state machine has quit,
     * which means target will be null and may cause a AndroidRuntimeException
     * in MessageQueue#enqueMessage if sent directly or if sent using
     * StateMachine#sendMessage the message will just be ignored.
     *
     * @param what  is assigned to Message.what
     * @return  A Message object from the global pool
     */
    CARAPI ObtainMessage(
        /* [in] */ Int32 what,
        /* [out] */ IMessage** msg);

    /**
     * Get a message and set Message.target state machine handler,
     * what, arg1, arg2 and obj
     *
     * Note: The handler can be null if the state machine has quit,
     * which means target will be null and may cause a AndroidRuntimeException
     * in MessageQueue#enqueMessage if sent directly or if sent using
     * StateMachine#sendMessage the message will just be ignored.
     *
     * @param what  is assigned to Message.what
     * @param obj is assigned to Message.obj
     * @return  A Message object from the global pool
     */
    CARAPI ObtainMessage(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj,
        /* [out] */ IMessage** msg);

    /**
     * Get a message and set Message.target state machine handler,
     * what, arg1, arg2 and obj
     *
     * Note: The handler can be null if the state machine has quit,
     * which means target will be null and may cause a AndroidRuntimeException
     * in MessageQueue#enqueMessage if sent directly or if sent using
     * StateMachine#sendMessage the message will just be ignored.
     *
     * @param what  is assigned to Message.what
     * @param arg1  is assigned to Message.arg1
     * @return  A Message object from the global pool
     */
    CARAPI ObtainMessage(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [out] */ IMessage** msg);

    /**
     * Get a message and set Message.target state machine handler,
     * what, arg1, arg2 and obj
     *
     * Note: The handler can be null if the state machine has quit,
     * which means target will be null and may cause a AndroidRuntimeException
     * in MessageQueue#enqueMessage if sent directly or if sent using
     * StateMachine#sendMessage the message will just be ignored.
     *
     * @param what  is assigned to Message.what
     * @param arg1  is assigned to Message.arg1
     * @param arg2  is assigned to Message.arg2
     * @return  A Message object from the global pool
     */
    CARAPI ObtainMessage(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [out] */ IMessage** msg);

    /**
     * Get a message and set Message.target state machine handler,
     * what, arg1, arg2 and obj
     *
     * Note: The handler can be null if the state machine has quit,
     * which means target will be null and may cause a AndroidRuntimeException
     * in MessageQueue#enqueMessage if sent directly or if sent using
     * StateMachine#sendMessage the message will just be ignored.
     *
     * @param what  is assigned to Message.what
     * @param arg1  is assigned to Message.arg1
     * @param arg2  is assigned to Message.arg2
     * @param obj is assigned to Message.obj
     * @return  A Message object from the global pool
     */
    CARAPI ObtainMessage(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ IInterface* obj,
        /* [out] */ IMessage** msg);

    /**
     * Enqueue a message to this state machine.
     *
     * Message is ignored if state machine has quit.
     */
    CARAPI SendMessage(
        /* [in] */ Int32 what);

    /**
     * Enqueue a message to this state machine.
     *
     * Message is ignored if state machine has quit.
     */
    CARAPI SendMessage(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Enqueue a message to this state machine.
     *
     * Message is ignored if state machine has quit.
     */
    CARAPI SendMessage(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1);

    /**
     * Enqueue a message to this state machine.
     *
     * Message is ignored if state machine has quit.
     */
    CARAPI SendMessage(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2);

    /**
     * Enqueue a message to this state machine.
     *
     * Message is ignored if state machine has quit.
     */
    CARAPI SendMessage(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ IInterface* obj);

    /**
     * Enqueue a message to this state machine.
     *
     * Message is ignored if state machine has quit.
     */
    CARAPI SendMessage(
        /* [in] */ IMessage* msg);

    /**
     * Enqueue a message to this state machine after a delay.
     *
     * Message is ignored if state machine has quit.
     */
    CARAPI SendMessageDelayed(
        /* [in] */ Int32 what,
        /* [in] */ Int64 delayMillis);

    /**
     * Enqueue a message to this state machine after a delay.
     *
     * Message is ignored if state machine has quit.
     */
    CARAPI SendMessageDelayed(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj,
        /* [in] */ Int64 delayMillis);

    /**
     * Enqueue a message to this state machine after a delay.
     *
     * Message is ignored if state machine has quit.
     */
    CARAPI SendMessageDelayed(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int64 delayMillis);

    /**
     * Enqueue a message to this state machine after a delay.
     *
     * Message is ignored if state machine has quit.
     */
    CARAPI SendMessageDelayed(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ Int64 delayMillis);

    /**
     * Enqueue a message to this state machine after a delay.
     *
     * Message is ignored if state machine has quit.
     */
    CARAPI SendMessageDelayed(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ IInterface* obj,
        /* [in] */ Int64 delayMillis);

    /**
     * Enqueue a message to this state machine after a delay.
     *
     * Message is ignored if state machine has quit.
     */
    CARAPI SendMessageDelayed(
        /* [in] */ IMessage* msg,
        /* [in] */ Int64 delayMillis);

    /**
     * @return if debugging is enabled
     */
    CARAPI IsDbg(
        /* [out] */ Boolean* isDbg);

    /**
     * Set debug enable/disabled.
     *
     * @param dbg is true to enable debugging.
     */
    CARAPI SetDbg(
        /* [in] */ Boolean dbg);

    /**
     * Start the state machine.
     */
    CARAPI Start();

    /**
     * Dump the current state.
     *
     * @param fd
     * @param pw
     * @param args
     */
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

protected:
    StateMachine() {}

    /**
     * Constructor creates a StateMachine with its own thread.
     *
     * @param name of the state machine
     */
    CARAPI constructor(
        /* [in] */ const String& name);

    /**
     * Constructor creates a StateMachine using the looper.
     *
     * @param name of the state machine
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ ILooper* looper);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ IHandler* handler);

    /**
     * Initialize.
     *
     * @param looper for this state machine
     * @param name of the state machine
     */
    CARAPI_(void) InitStateMachine(
        /* [in] */ const String& name,
        /* [in] */ ILooper* looper);

    /**
     * Add a new state to the state machine
     * @param state the state to add
     * @param parent the parent of state
     */
    CARAPI AddState(
        /* [in] */ State* state,
        /* [in] */ State* parent);

    /**
     * Add a new state to the state machine, parent will be null
     * @param state to add
     */
    CARAPI AddState(
        /* [in] */ State* state);

    /**
     * Set the initial state. This must be invoked before
     * and messages are sent to the state machine.
     *
     * @param initialState is the state which will receive the first message.
     */
    CARAPI_(void) SetInitialState(
        /* [in] */ State* initialState);

    /**
     * @return current message
     */
    CARAPI_(AutoPtr<IMessage>) GetCurrentMessage();

    /**
     * @return current state
     */
    CARAPI_(AutoPtr<IState>) GetCurrentState();

    /**
     * transition to destination state. Upon returning
     * from processMessage the current state's exit will
     * be executed and upon the next message arriving
     * destState.enter will be invoked.
     *
     * this function can also be called inside the enter function of the
     * previous transition target, but the behavior is undefined when it is
     * called mid-way through a previous transition (for example, calling this
     * in the enter() routine of a intermediate node when the current transition
     * target is one of the nodes descendants).
     *
     * @param destState will be the state that receives the next message.
     */
    CARAPI_(void) TransitionTo(
        /* [in] */ IState* destState);

    /**
     * transition to halt state. Upon returning
     * from processMessage we will exit all current
     * states, execute the onHalting() method and then
     * for all subsequent messages haltedProcessMessage
     * will be called.
     */
    CARAPI_(void) TransitionToHaltingState();

    /**
     * Defer this message until next state transition.
     * Upon transitioning all deferred messages will be
     * placed on the queue and reprocessed in the original
     * order. (i.e. The next state the oldest messages will
     * be processed first)
     *
     * @param msg is deferred until the next transition.
     */
    CARAPI_(void) DeferMessage(
        /* [in] */ IMessage* msg);

    /**
     * Called when message wasn't handled
     *
     * @param msg that couldn't be handled.
     */
    virtual CARAPI_(void) UnhandledMessage(
        /* [in] */ IMessage* msg);

    /**
     * Called for any message that is received after
     * transitionToHalting is called.
     */
    virtual CARAPI_(void) HaltedProcessMessage(
        /* [in] */ IMessage* msg);

    /**
     * This will be called once after handling a message that called
     * transitionToHalting. All subsequent messages will invoke
     * {@link StateMachine#haltedProcessMessage(Message)}
     */
    virtual CARAPI_(void) OnHalting();

    /**
     * This will be called once after a quit message that was NOT handled by
     * the derived StateMachine. The StateMachine will stop and any subsequent messages will be
     * ignored. In addition, if this StateMachine created the thread, the thread will
     * be stopped after this method returns.
     */
    virtual CARAPI_(void) OnQuitting();

    /**
     * Add the string to LogRecords.
     *
     * @param string
     */
    virtual CARAPI_(void) AddLogRec(
        /* [in] */ const String& string);

    /**
     * @return true if msg should be saved in the log, default is true.
     */
    virtual CARAPI_(Boolean) RecordLogRec(
        /* [in] */ IMessage* msg);

    /**
     * Return a string to be logged by LogRec, default
     * is an empty string. Override if additional information is desired.
     *
     * @param msg that was processed
     * @return information to be logged as a String
     */
    virtual CARAPI_(String) GetLogRecString(
        /* [in] */ IMessage* msg);

    /**
     * @return the string for msg.what
     */
    virtual CARAPI_(String) GetWhatToString(
        /* [in] */ Int32 what);

    /**
     * Enqueue a message to the front of the queue for this state machine.
     * Protected, may only be called by instances of StateMachine.
     *
     * Message is ignored if state machine has quit.
     */
    CARAPI_(void) SendMessageAtFrontOfQueue(
        /* [in] */ Int32 what);

    /**
     * Enqueue a message to the front of the queue for this state machine.
     * Protected, may only be called by instances of StateMachine.
     *
     * Message is ignored if state machine has quit.
     */
    CARAPI_(void) SendMessageAtFrontOfQueue(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * Enqueue a message to the front of the queue for this state machine.
     * Protected, may only be called by instances of StateMachine.
     *
     * Message is ignored if state machine has quit.
     */
    CARAPI_(void) SendMessageAtFrontOfQueue(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1);

    /**
     * Enqueue a message to the front of the queue for this state machine.
     * Protected, may only be called by instances of StateMachine.
     *
     * Message is ignored if state machine has quit.
     */
    CARAPI_(void) SendMessageAtFrontOfQueue(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2);

    /**
     * Enqueue a message to the front of the queue for this state machine.
     * Protected, may only be called by instances of StateMachine.
     *
     * Message is ignored if state machine has quit.
     */
    CARAPI_(void) SendMessageAtFrontOfQueue(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ IInterface* obj);

    /**
     * Enqueue a message to the front of the queue for this state machine.
     * Protected, may only be called by instances of StateMachine.
     *
     * Message is ignored if state machine has quit.
     */
    CARAPI_(void) SendMessageAtFrontOfQueue(
        /* [in] */ IMessage* msg);

    /**
     * Removes a message from the message queue.
     * Protected, may only be called by instances of StateMachine.
     */
    CARAPI_(void) RemoveMessages(
        /* [in] */ Int32 what);

    /**
     * Validate that the message was sent by
     * {@link StateMachine#quit} or {@link StateMachine#quitNow}.
     * */
    CARAPI_(Boolean) IsQuit(
        /* [in] */ IMessage* msg);

    /**
     * Quit the state machine after all currently queued up messages are processed.
     */
    CARAPI_(void) Quit();

    /**
     * Quit the state machine immediately all currently queued messages will be discarded.
     */
    CARAPI_(void) QuitNow();

    /**
     * Log with debug and add to the LogRecords.
     *
     * @param s is string log
     */
    CARAPI_(void) LogAndAddLogRec(
        /* [in] */ const String& s);

    /**
     * Log with debug
     *
     * @param s is string log
     */
    CARAPI_(void) Log(
        /* [in] */ const String& s);
    /**
     * Log with debug attribute
     *
     * @param s is string log
     */
    CARAPI_(void) Logd(
        /* [in] */ const String& s);
    /**
     * Log with verbose attribute
     *
     * @param s is string log
     */
    CARAPI_(void) Logv(
        /* [in] */ const String& s);
    /**
     * Log with info attribute
     *
     * @param s is string log
     */
    CARAPI_(void) Logi(
        /* [in] */ const String& s);
    /**
     * Log with warning attribute
     *
     * @param s is string log
     */
    CARAPI_(void) Logw(
        /* [in] */ const String& s);
    /**
     * Log with error attribute
     *
     * @param s is string log
     */
    CARAPI_(void) Loge(
        /* [in] */ const String& s);

private:
    // Name of the state machine and used as logging tag
    String mName;

    /** Message.what value when quitting */
    ECO_LOCAL static const Int32 SM_QUIT_CMD = -1;

    /** Message.what value when initializing */
    ECO_LOCAL static const Int32 SM_INIT_CMD = -2;

    AutoPtr<SmHandler> mSmHandler;
    AutoPtr<IHandlerThread> mSmThread;
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

template <>
struct Conversion<Elastos::Droid::Internal::Utility::StateMachine::SmHandler::StateInfo*, IInterface*>
{
    enum { exists = TRUE, exists2Way = FALSE, sameType = FALSE };
};

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_STATEMACHINE_H__
