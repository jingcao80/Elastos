
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/internal/utility/StateMachine.h"
#include "elastos/droid/os/CHandlerThread.h"
#include "elastos/droid/os/CMessageHelper.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Core::AutoLock;
using Elastos::Core::IThread;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::IFlushable;
using Elastos::Utility::ICalendar;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CVector;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

//====================================================
// StateMachine::LogRec
//====================================================

CAR_INTERFACE_IMPL(StateMachine::LogRec, Object, IStateMachineLogRec)

StateMachine::LogRec::LogRec(
    /* [in] */ StateMachine* sm,
    /* [in] */ IMessage* msg,
    /* [in] */ const String& info,
    /* [in] */ IState* state,
    /* [in] */ IState* orgState,
    /* [in] */ IState* transToState)
    : mTime(0)
    , mWhat(0)
{
    Update(sm, msg, info, state, orgState, transToState);
}

void StateMachine::LogRec::Update(
    /* [in] */ StateMachine* sm,
    /* [in] */ IMessage* msg,
    /* [in] */ const String& info,
    /* [in] */ IState* state,
    /* [in] */ IState* orgState,
    /* [in] */ IState* dstState)
{
    mSm = sm;
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mTime);
    if (msg != NULL)
        msg->GetWhat(&mWhat);
    mInfo = info;
    mState = state;
    mOrgState = orgState;
    mDstState = dstState;
}

Int64 StateMachine::LogRec::GetTime()
{
    return mTime;
}

Int64 StateMachine::LogRec::GetWhat()
{
    return mWhat;
}

String StateMachine::LogRec::GetInfo()
{
    return mInfo;
}

AutoPtr<IState> StateMachine::LogRec::GetState()
{
    return mState;
}

AutoPtr<IState> StateMachine::LogRec::GetDestState()
{
    return mDstState;
}

AutoPtr<IState> StateMachine::LogRec::GetOriginalState()
{
    return mOrgState;
}

ECode StateMachine::LogRec::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb;
    sb.Append("time=");
    AutoPtr<ICalendarHelper> ch;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&ch);
    AutoPtr<ICalendar> c;
    ch->GetInstance((ICalendar**)&c);
    c->SetTimeInMillis(mTime);
    // sb.Append(String.format("%tm-%td %tH:%tM:%tS.%tL", c, c, c, c, c, c));
    sb.Append(" processed=");
    sb.Append(mState == NULL ? "<null>" : ((State*)mState.Get())->GetName());
    sb.Append(" org=");
    sb.Append(mOrgState == NULL ? "<null>" : ((State*)mOrgState.Get())->GetName());
    sb.Append(" dest=");
    sb.Append(mDstState == NULL ? "<null>" : ((State*)mDstState.Get())->GetName());
    sb.Append(" what=");
    String what = mSm != NULL ? mSm->GetWhatToString(mWhat) : String("");
    if (what.IsNullOrEmpty()) {
        sb.Append(mWhat);
        sb.Append("(0x");
        sb.Append(StringUtils::ToString(mWhat, 16));
        sb.Append(")");
    }
    else {
        sb.Append(what);
    }
    if (!mInfo.IsNullOrEmpty()) {
        sb.Append(" ");
        sb.Append(mInfo);
    }
    *str = sb.ToString();
    return NOERROR;
}

//====================================================
// StateMachine::LogRecords
//====================================================

StateMachine::LogRecords::LogRecords()
    : mMaxSize(DEFAULT_SIZE)
    , mOldestIndex(0)
    , mCount(0)
    , mLogOnlyTransitions(FALSE)
{}

void StateMachine::LogRecords::SetSize(
    /* [in] */ Int32 maxSize)
{
    AutoLock lock(this);

    mMaxSize = maxSize;
    mCount = 0;
    mLogRecVector.Clear();
}

void StateMachine::LogRecords::SetLogOnlyTransitions(
    /* [in] */ Boolean enable)
{
    mLogOnlyTransitions = enable;
}

Boolean StateMachine::LogRecords::LogOnlyTransitions()
{
    return mLogOnlyTransitions;
}

Int32 StateMachine::LogRecords::GetSize()
{
    AutoLock lock(this);
    return mLogRecVector.GetSize();
}

Int32 StateMachine::LogRecords::Count()
{
    AutoLock lock(this);
    return mCount;
}

void StateMachine::LogRecords::Cleanup()
{
    AutoLock lock(this);
    mLogRecVector.Clear();
}

AutoPtr<StateMachine::LogRec> StateMachine::LogRecords::Get(
    /* [in] */ Int32 index)
{
    AutoLock lock(this);
    Int32 nextIndex = mOldestIndex + index;
    if (nextIndex >= mMaxSize) {
        nextIndex -= mMaxSize;
    }
    if (nextIndex >= GetSize()) {
        return NULL;
    }
    else {
        return mLogRecVector[nextIndex];
    }
}

void StateMachine::LogRecords::Add(
    /* [in] */ StateMachine* sm,
    /* [in] */ IMessage* msg,
    /* [in] */ const String& messageInfo,
    /* [in] */ IState* state,
    /* [in] */ IState* orgState,
    /* [in] */ IState* transToState)
{
    AutoLock lock(this);
    mCount += 1;
    if (mLogRecVector.GetSize() < mMaxSize) {
        AutoPtr<LogRec> pmi = new LogRec(sm, msg, messageInfo, state, orgState, transToState);
        mLogRecVector.PushBack(pmi);
    }
    else {
        AutoPtr<LogRec> pmi = mLogRecVector[mOldestIndex];
        mOldestIndex += 1;
        if (mOldestIndex >= mMaxSize) {
            mOldestIndex = 0;
        }
        pmi->Update(sm, msg, messageInfo, state, orgState, transToState);
    }
}


//====================================================
// StateMachine::SmHandler
//====================================================

StateMachine::SmHandler::StateInfo::StateInfo()
    : mActive(FALSE)
{}

/**
 * Convert StateInfo to string
 */
String StateMachine::SmHandler::StateInfo::ToString()
{
    StringBuilder sb("state=");
    sb += mState->GetName();
    sb += ",active=";
    sb += mActive;
    sb += ",parent=";
    sb += (mParentStateInfo == NULL) ? "null"
        : mParentStateInfo->mState->GetName();
    return sb.ToString();
}

StateMachine::SmHandler::HaltingState::HaltingState(
    /* [in] */ SmHandler* owner)
    : mOwner(owner)
{}

ECode StateMachine::SmHandler::HaltingState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mOwner->mSm->HaltedProcessMessage(msg);
    *result = TRUE;
    return NOERROR;
}

ECode StateMachine::SmHandler::QuittingState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = NOT_HANDLED;
    return NOERROR;
}

const AutoPtr<IInterface> StateMachine::SmHandler::sSmHandlerObj = (IObject*)new Object();

StateMachine::SmHandler::SmHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ StateMachine* sm)
    : Handler(looper)
    , mHasQuit(FALSE)
    , mDbg(FALSE)
    , mLogRecords(new LogRecords())
    , mIsConstructionCompleted(FALSE)
    , mStateStackTopIndex(-1)
    , mHaltingState(new HaltingState(this))
    , mQuittingState(new QuittingState())
    , mSm(sm)
{
    AutoPtr<StateInfo> stateInfo;
    AddState(mHaltingState, NULL, (StateInfo**)&stateInfo);
    stateInfo = NULL;
    AddState(mQuittingState, NULL, (StateInfo**)&stateInfo);
}

ECode StateMachine::SmHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    if (!mHasQuit) {
        /** Save the current message */
        mMsg = msg;

        Int32 what;
        mMsg->GetWhat(&what);
        if (mDbg) {
            StringBuilder sb("handleMessage: E msg.what=");
            sb += what;
            mSm->Log(sb.ToString());
        }

        AutoPtr<IInterface> obj;
        /** State that processed the message */
        AutoPtr<State> msgProcessedState;
        if (mIsConstructionCompleted) {
            /** Normal path */
            msgProcessedState = ProcessMsg(msg);
        }
        else if (!mIsConstructionCompleted && (what == SM_INIT_CMD) &&
            (mMsg->GetObj((IInterface**)&obj), obj == sSmHandlerObj)) {
            /** Initial one time path. */
            mIsConstructionCompleted = TRUE;
            InvokeEnterMethods(0);
        }
        else {
            // throw new RuntimeException("StateMachine.handleMessage: " +
            //             "The start method not called, received msg: " + msg);
            return E_RUNTIME_EXCEPTION;
        }
        PerformTransitions(msgProcessedState, msg);
        // We need to check if mSm == null here as we could be quitting.
        if (mDbg && mSm != NULL)
            mSm->Log(String("handleMessage: X"));
    }

    return NOERROR;
}

void StateMachine::SmHandler::PerformTransitions(
    /* [in] */ State* msgProcessedState,
    /* [in] */ IMessage* msg)
{
    /**
     * If transitionTo has been called, exit and then enter
     * the appropriate states. We loop on this to allow
     * enter and exit methods to use transitionTo.
     */
    AutoPtr<State> orgState = (*mStateStack)[mStateStackTopIndex]->mState;

    /**
     * Record whether message needs to be logged before we transition and
     * and we won't log special messages SM_INIT_CMD or SM_QUIT_CMD which
     * always set msg.obj to the handler.
     */
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    Boolean recordLogMsg = mSm->RecordLogRec(mMsg) && (obj != sSmHandlerObj);

    if (mLogRecords->LogOnlyTransitions()) {
        /** Record only if there is a transition */
        if (mDestState != NULL) {
            mLogRecords->Add(mSm, mMsg, mSm->GetLogRecString(mMsg), msgProcessedState,
                    orgState, mDestState);
        }
    }
    else if (recordLogMsg) {
        /** Record message */
        mLogRecords->Add(mSm, mMsg, mSm->GetLogRecString(mMsg), msgProcessedState, orgState,
                mDestState);
    }

    AutoPtr<State> destState = mDestState;
    if (destState != NULL) {
        /**
         * Process the transitions including transitions in the enter/exit methods
         */
        while (TRUE) {
            if (mDbg) mSm->Log(String("handleMessage: new destination call exit"));

            /**
             * Determine the states to exit and enter and return the
             * common ancestor state of the enter/exit states. Then
             * invoke the exit methods then the enter methods.
             */
            AutoPtr<StateInfo> commonStateInfo = SetupTempStateStackWithStatesToEnter(destState);
            InvokeExitMethods(commonStateInfo);
            Int32 stateStackEnteringIndex = MoveTempStateStackToStateStack();
            InvokeEnterMethods(stateStackEnteringIndex);

            /**
             * Since we have transitioned to a new state we need to have
             * any deferred messages moved to the front of the message queue
             * so they will be processed before any other messages in the
             * message queue.
             */
            MoveDeferredMessageAtFrontOfQueue();

            if (destState != mDestState) {
                // A new mDestState so continue looping
                destState = mDestState;
            }
            else {
                // No change in mDestState so we're done
                break;
            }
        }
        mDestState = NULL;
    }

    /**
     * After processing all transitions check and
     * see if the last transition was to quit or halt.
     */
    if (destState != NULL) {
        if (destState == mQuittingState) {
            /**
             * Call onQuitting to let subclasses cleanup.
             */
            mSm->OnQuitting();
            CleanupAfterQuitting();
        }
        else if (destState == mHaltingState) {
            /**
             * Call onHalting() if we've transitioned to the halting
             * state. All subsequent messages will be processed in
             * in the halting state which invokes haltedProcessMessage(msg);
             */
            mSm->OnHalting();
        }
    }
}

void StateMachine::SmHandler::CleanupAfterQuitting()
{
    if (mSm->mSmThread != NULL) {
        // If we made the thread then quit looper which stops the thread.
        AutoPtr<ILooper> looper;
        GetLooper((ILooper**)&looper);
        looper->Quit();
        mSm->mSmThread = NULL;
    }

    mSm->mSmHandler = NULL;
    mSm = NULL;
    mMsg = NULL;
    mLogRecords->Cleanup();
    mStateStack = NULL;
    mTempStateStack = NULL;
    mStateInfo.Clear();
    mInitialState = NULL;
    mDestState = NULL;
    mDeferredMessages.Clear();
    mHasQuit = TRUE;
}

void StateMachine::SmHandler::CompleteConstruction()
{
    if (mDbg) mSm->Log(String("completeConstruction: E"));

    /**
     * Determine the maximum depth of the state hierarchy
     * so we can allocate the state stacks.
     */
    Int32 maxDepth = 0;
    HashMap< AutoPtr<State>, AutoPtr<StateInfo> >::Iterator it;
    for (it = mStateInfo.Begin(); it != mStateInfo.End(); ++it) {
        AutoPtr<StateInfo> si = it->mSecond;
        Int32 depth = 0;
        for (AutoPtr<StateInfo> i = si; i != NULL; depth++) {
            i = i->mParentStateInfo;
        }
        if (maxDepth < depth) {
            maxDepth = depth;
        }
    }
    if (mDbg) {
        StringBuilder sb("completeConstruction: maxDepth=%d");
        sb += maxDepth;
        mSm->Log(sb.ToString());
    }

    mStateStack = ArrayOf<StateInfo*>::Alloc(maxDepth);
    mTempStateStack = ArrayOf<StateInfo*>::Alloc(maxDepth);
    SetupInitialStateStack();

    /** Sending SM_INIT_CMD message to invoke enter methods asynchronously */
    AutoPtr<IMessage> msg;
    mSm->ObtainMessage(SM_INIT_CMD, sSmHandlerObj, (IMessage**)&msg);
    mSm->SendMessageAtFrontOfQueue(msg);

    if (mDbg) mSm->Log(String("completeConstruction: X"));
}

AutoPtr<State> StateMachine::SmHandler::ProcessMsg(
    /* [in] */ IMessage* msg)
{
    AutoPtr<StateInfo> curStateInfo = (*mStateStack)[mStateStackTopIndex];
    if (mDbg) {
        mSm->Log(String("processMsg: ") + curStateInfo->mState->GetName());
    }

    if (IsQuit(msg)) {
        TransitionTo(mQuittingState);
    }
    else {
        Boolean result;
        while (curStateInfo->mState->ProcessMessage(msg, &result), !result) {
            /**
             * Not processed
             */
            curStateInfo = curStateInfo->mParentStateInfo;
            if (curStateInfo == NULL) {
                /**
                 * No parents left so it's not handled
                 */
                mSm->UnhandledMessage(msg);
                break;
            }
            if (mDbg) {
                mSm->Log(String("processMsg: ") + curStateInfo->mState->GetName());
            }
        }
    }
    return (curStateInfo != NULL) ? curStateInfo->mState : NULL;
}

void StateMachine::SmHandler::InvokeExitMethods(
    /* [in] */ StateInfo* commonStateInfo)
{
    while ((mStateStackTopIndex >= 0) &&
            ((*mStateStack)[mStateStackTopIndex] != commonStateInfo)) {
        AutoPtr<State> curState = (*mStateStack)[mStateStackTopIndex]->mState;
        if (mDbg) mSm->Log(String("invokeExitMethods: ") + curState->GetName());
        curState->Exit();
        (*mStateStack)[mStateStackTopIndex]->mActive = FALSE;
        mStateStackTopIndex -= 1;
    }
}

void StateMachine::SmHandler::InvokeEnterMethods(
    /* [in] */ Int32 stateStackEnteringIndex)
{
    for (Int32 i = stateStackEnteringIndex; i <= mStateStackTopIndex; i++) {
        if (mDbg) {
            mSm->Log(String("invokeEnterMethods: ") + (*mStateStack)[i]->mState->GetName());
        }
        (*mStateStack)[i]->mState->Enter();
        (*mStateStack)[i]->mActive = TRUE;
    }
}

void StateMachine::SmHandler::MoveDeferredMessageAtFrontOfQueue()
{
    /**
     * The oldest messages on the deferred list must be at
     * the front of the queue so start at the back, which
     * as the most resent message and end with the oldest
     * messages at the front of the queue.
     */
    List< AutoPtr<IMessage> >::ReverseIterator rit;
    for (rit = mDeferredMessages.RBegin(); rit != mDeferredMessages.REnd(); ++rit) {
        AutoPtr<IMessage> curMsg = *rit;
        if (mDbg) {
            Int32 what;
            curMsg->GetWhat(&what);
            StringBuilder sb("moveDeferredMessageAtFrontOfQueue; what=");
            sb += what;
            mSm->Log(sb.ToString());
        }
        mSm->SendMessageAtFrontOfQueue(curMsg);
    }
    mDeferredMessages.Clear();
}

Int32 StateMachine::SmHandler::MoveTempStateStackToStateStack()
{
    Int32 startingIndex = mStateStackTopIndex + 1;
    Int32 i = mTempStateStackCount - 1;
    Int32 j = startingIndex;
    while (i >= 0) {
        if (mDbg) {
            StringBuilder sb("moveTempStackToStateStack: i=");
            sb += i;
            sb += ",j=";
            sb += j;
            mSm->Log(sb.ToString());
        }
        mStateStack->Set(j, (*mTempStateStack)[i]);
        j += 1;
        i -= 1;
    }

    mStateStackTopIndex = j - 1;
    if (mDbg) {
        StringBuilder sb("moveTempStackToStateStack: X mStateStackTop=");
        sb += mStateStackTopIndex;
        sb += ",startingIndex=";
        sb += startingIndex;
        sb += ",Top=";
        sb += (*mStateStack)[mStateStackTopIndex]->mState->GetName();
        mSm->Log(sb.ToString());
    }
    return startingIndex;
}

AutoPtr<StateMachine::SmHandler::StateInfo>
StateMachine::SmHandler::SetupTempStateStackWithStatesToEnter(
    /* [in] */ State* destState)
{
    /**
     * Search up the parent list of the destination state for an active
     * state. Use a do while() loop as the destState must always be entered
     * even if it is active. This can happen if we are exiting/entering
     * the current state.
     */
    mTempStateStackCount = 0;
    AutoPtr<StateInfo> curStateInfo;
    HashMap< AutoPtr<State>, AutoPtr<StateInfo> >::Iterator it = mStateInfo.Find(destState);
    if (it != mStateInfo.End()) {
        curStateInfo = it->mSecond;
    }
    do {
        mTempStateStack->Set(mTempStateStackCount++, curStateInfo);
        curStateInfo = curStateInfo->mParentStateInfo;
    } while ((curStateInfo != NULL) && !curStateInfo->mActive);

    if (mDbg) {
        StringBuilder sb("setupTempStateStackWithStatesToEnter: X mTempStateStackCount=");
        sb += mTempStateStackCount;
        sb += ",curStateInfo ";
        sb += curStateInfo->ToString();
        mSm->Log(sb.ToString());
    }
    return curStateInfo;
}

void StateMachine::SmHandler::SetupInitialStateStack()
{
    if (mDbg) {
        mSm->Log(String("setupInitialStateStack: E mInitialState= ")
            + mInitialState->GetName());
    }

    AutoPtr<StateInfo> curStateInfo;
    HashMap< AutoPtr<State>, AutoPtr<StateInfo> >::Iterator it = mStateInfo.Find(mInitialState);
    if (it != mStateInfo.End()) {
        curStateInfo = it->mSecond;
    }
    for (mTempStateStackCount = 0; curStateInfo != NULL; mTempStateStackCount++) {
        mTempStateStack->Set(mTempStateStackCount, curStateInfo);
        curStateInfo = curStateInfo->mParentStateInfo;
    }

    // Empty the StateStack
    mStateStackTopIndex = -1;

    MoveTempStateStackToStateStack();
}

AutoPtr<IMessage> StateMachine::SmHandler::GetCurrentMessage()
{
    return mMsg;
}

AutoPtr<IState> StateMachine::SmHandler::GetCurrentState()
{
    return (*mStateStack)[mStateStackTopIndex]->mState;
}

ECode StateMachine::SmHandler::AddState(
    /* [in] */ State* state,
    /* [in] */ State* parent,
    /* [out] */ StateInfo** info)
{
    if (mDbg) {
        StringBuilder sb("addStateInternal: E state=");
        sb += state->GetName();
        sb += ",parent=";
        sb += (parent == NULL) ? "" : parent->GetName();
        mSm->Log(sb.ToString());
    }
    AutoPtr<StateInfo> parentStateInfo;
    if (parent != NULL) {
        HashMap< AutoPtr<State>, AutoPtr<StateInfo> >::Iterator it =
                mStateInfo.Find(parent);
        if (it != mStateInfo.End()) {
            parentStateInfo = it->mSecond;
        }
        if (parentStateInfo == NULL) {
            // Recursively add our parent as it's not been added yet.
            FAIL_RETURN(AddState(parent, NULL, (StateInfo**)&parentStateInfo));
        }
    }
    AutoPtr<StateInfo> stateInfo;
    HashMap< AutoPtr<State>, AutoPtr<StateInfo> >::Iterator it =
            mStateInfo.Find(state);
    if (it != mStateInfo.End()) {
        stateInfo = it->mSecond;
    }
    if (stateInfo == NULL) {
        stateInfo = new StateInfo();
        mStateInfo[state] = stateInfo;
    }

    // Validate that we aren't adding the same state in two different hierarchies.
    if ((stateInfo->mParentStateInfo != NULL) &&
            (stateInfo->mParentStateInfo != parentStateInfo)) {
        // throw new RuntimeException("state already added");
        return E_RUNTIME_EXCEPTION;
    }
    stateInfo->mState = state;
    stateInfo->mParentStateInfo = parentStateInfo;
    stateInfo->mActive = FALSE;
    if (mDbg) mSm->Log(String("addStateInternal: X stateInfo: ") + stateInfo->ToString());
    *info = stateInfo;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

void StateMachine::SmHandler::SetInitialState(
    /* [in] */ State* initialState)
{
    if (mDbg) mSm->Log(String("setInitialState: initialState=") + initialState->GetName());
    mInitialState = initialState;
}

void StateMachine::SmHandler::TransitionTo(
    /* [in] */ IState* destState)
{
    mDestState = (State*)destState;
    if (mDbg) mSm->Log(String("transitionTo: destState=") + mDestState->GetName());
}

void StateMachine::SmHandler::DeferMessage(
    /* [in] */ IMessage* msg)
{
    if (mDbg) {
        Int32 what;
        msg->GetWhat(&what);
        StringBuilder sb("deferMessage: msg=");
        sb += what;
        mSm->Log(sb.ToString());
    }

    /* Copy the "msg" to "newMsg" as "msg" will be recycled */
    AutoPtr<IMessage> newMsg;
    mSm->ObtainMessage((IMessage**)&newMsg);
    newMsg->CopyFrom(msg);

    mDeferredMessages.PushBack(newMsg);
}

void StateMachine::SmHandler::Quit()
{
    if (mDbg) mSm->Log(String("quit:"));
    AutoPtr<IMessage> msg;
    mSm->ObtainMessage(SM_QUIT_CMD, sSmHandlerObj, (IMessage**)&msg);
    mSm->SendMessage(msg);
}

void StateMachine::SmHandler::QuitNow()
{
    if (mDbg) mSm->Log(String("abort:"));
    AutoPtr<IMessage> msg;
    mSm->ObtainMessage(SM_QUIT_CMD, sSmHandlerObj, (IMessage**)&msg);
    mSm->SendMessageAtFrontOfQueue(msg);
}

Boolean StateMachine::SmHandler::IsQuit(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    AutoPtr<IInterface> obj;
    msg->GetWhat(&what);
    msg->GetObj((IInterface**)&obj);
    return (what == SM_QUIT_CMD) && (obj == sSmHandlerObj);
}

Boolean StateMachine::SmHandler::IsDbg()
{
    return mDbg;
}

void StateMachine::SmHandler::SetDbg(
    /* [in] */ Boolean dbg)
{
    mDbg = dbg;
}

//====================================================
// StateMachine
//====================================================

CAR_INTERFACE_IMPL(StateMachine, Object, IStateMachine)

StateMachine::StateMachine(
    /* [in] */ const String& name)
{
    CHandlerThread::New(name, (IHandlerThread**)&mSmThread);
    IThread::Probe(mSmThread)->Start();
    AutoPtr<ILooper> looper;
    mSmThread->GetLooper((ILooper**)&looper);

    InitStateMachine(name, looper);
}

StateMachine::StateMachine(
    /* [in] */ const String& name,
    /* [in] */ ILooper* looper)
{
    InitStateMachine(name, looper);
}

StateMachine::StateMachine(
    /* [in] */ const String& name,
    /* [in] */ IHandler* handler)
{
    AutoPtr<ILooper> looper;
    handler->GetLooper((ILooper**)&looper);
    InitStateMachine(name, looper);
}

void StateMachine::InitStateMachine(
    /* [in] */ const String& name,
    /* [in] */ ILooper* looper)
{
    mName = name;
    mSmHandler = new SmHandler(looper, this);
}

ECode StateMachine::AddState(
    /* [in] */ State* state,
    /* [in] */ State* parent)
{
    AutoPtr<SmHandler::StateInfo> sinfo;
    return mSmHandler->AddState(state, parent, (SmHandler::StateInfo**)&sinfo);
}

ECode StateMachine::AddState(
    /* [in] */ State* state)
{
    AutoPtr<SmHandler::StateInfo> sinfo;
    return mSmHandler->AddState(state, NULL, (SmHandler::StateInfo**)&sinfo);
}

void StateMachine::SetInitialState(
    /* [in] */ State* initialState)
{
    mSmHandler->SetInitialState(initialState);
}

AutoPtr<IMessage> StateMachine::GetCurrentMessage()
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL)
        return NULL;
    return smh->GetCurrentMessage();
}

AutoPtr<IState> StateMachine::GetCurrentState()
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL)
        return NULL;
    return smh->GetCurrentState();
}

void StateMachine::TransitionTo(
    /* [in] */ IState* destState)
{
    mSmHandler->TransitionTo(destState);
}

void StateMachine::TransitionToHaltingState()
{
    mSmHandler->TransitionTo(mSmHandler->mHaltingState);
}

void StateMachine::DeferMessage(
    /* [in] */ IMessage* msg)
{
    mSmHandler->DeferMessage(msg);
}

void StateMachine::UnhandledMessage(
    /* [in] */ IMessage* msg)
{
    if (mSmHandler->mDbg) {
        Int32 what;
        msg->GetWhat(&what);
        StringBuilder sb(" - unhandledMessage: msg.what=");
        sb += what;
        Loge(sb.ToString());
    }
}

void StateMachine::HaltedProcessMessage(
    /* [in] */ IMessage* msg)
{
}

void StateMachine::OnHalting()
{
}


void StateMachine::OnQuitting()
{
}

ECode StateMachine::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

ECode StateMachine::SetLogRecSize(
    /* [in] */ Int32 maxSize)
{
    mSmHandler->mLogRecords->SetSize(maxSize);
    return NOERROR;
}

ECode StateMachine::SetLogOnlyTransitions(
    /* [in] */ Boolean enable)
{
    mSmHandler->mLogRecords->SetLogOnlyTransitions(enable);
    return NOERROR;
}

ECode StateMachine::GetLogRecSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) {
        *size = 0;
        return NOERROR;
    }
    *size = smh->mLogRecords->GetSize();
    return NOERROR;
}

ECode StateMachine::GetLogRecCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) {
        *count = 0;
        return NOERROR;
    }
    *count = smh->mLogRecords->Count();
    return NOERROR;
}

ECode StateMachine::GetLogRec(
    /* [in] */ Int32 index,
    /* [out] */ IStateMachineLogRec** logRec)
{
    VALIDATE_NOT_NULL(logRec)
    *logRec = NULL;

    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) {
        return NOERROR;
    }
    AutoPtr<IStateMachineLogRec> temp = smh->mLogRecords->Get(index);
    *logRec = temp;
    REFCOUNT_ADD(*logRec)
    return NOERROR;
}

ECode StateMachine::CopyLogRecs(
    /* [out] */ ICollection** collection)
{
    VALIDATE_NOT_NULL(collection)
    CVector::New(collection);
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh != NULL) {
        Vector<AutoPtr<LogRec> >::Iterator iter = smh->mLogRecords->mLogRecVector.Begin();
        for (; iter != smh->mLogRecords->mLogRecVector.End(); ++iter) {
            (*collection)->Add(TO_IINTERFACE(*iter));
        }
    }
    return NOERROR;
}

void StateMachine::AddLogRec(
    /* [in] */ const String& string)
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return;
    smh->mLogRecords->Add(this, smh->GetCurrentMessage(), string, smh->GetCurrentState(),
        (*smh->mStateStack)[smh->mStateStackTopIndex]->mState, smh->mDestState);
}

Boolean StateMachine::RecordLogRec(
    /* [in] */ IMessage* msg)
{
    return TRUE;
}

String StateMachine::GetLogRecString(
    /* [in] */ IMessage* msg)
{
    return String("");
}

String StateMachine::GetWhatToString(
    /* [in] */ Int32 what)
{
    return String(NULL);
}

ECode StateMachine::GetHandler(
    /* [out] */ IHandler** handler)
{
    VALIDATE_NOT_NULL(handler)
    *handler = mSmHandler;
    REFCOUNT_ADD(*handler)
    return NOERROR;
}

ECode StateMachine::ObtainMessage(
    /* [out] */ IMessage** msg)
{
    return ObtainMessage(0, msg);
}

ECode StateMachine::ObtainMessage(
    /* [in] */ Int32 what,
    /* [out] */ IMessage** msg)
{
    return ObtainMessage(what, (IInterface*)NULL, msg);
}

ECode StateMachine::ObtainMessage(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj,
    /* [out] */ IMessage** msg)
{
    return ObtainMessage(what, 0, 0, obj, msg);
}

ECode StateMachine::ObtainMessage(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [out] */ IMessage** msg)
{
    return ObtainMessage(what, arg1, 0, NULL, msg);
}

ECode StateMachine::ObtainMessage(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [out] */ IMessage** msg)
{
    return ObtainMessage(what, arg1, arg2, NULL, msg);
}

ECode StateMachine::ObtainMessage(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj,
    /* [out] */ IMessage** msg)
{
    VALIDATE_NOT_NULL(msg)
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    return helper->Obtain(mSmHandler, what, arg1, arg2, obj, msg);
}

ECode StateMachine::SendMessage(
    /* [in] */ Int32 what)
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return NOERROR;

    AutoPtr<IMessage> msg;
    ObtainMessage(what, (IMessage**)&msg);
    Boolean result;
    return smh->SendMessage(msg, &result);
}

ECode StateMachine::SendMessage(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return NOERROR;

    AutoPtr<IMessage> msg;
    ObtainMessage(what, obj, (IMessage**)&msg);
    Boolean result;
    return smh->SendMessage(msg, &result);
}

ECode StateMachine::SendMessage(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1)
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return NOERROR;

    AutoPtr<IMessage> msg;
    ObtainMessage(what, arg1, (IMessage**)&msg);
    Boolean result;
    return smh->SendMessage(msg, &result);
}

ECode StateMachine::SendMessage(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2)
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return NOERROR;

    AutoPtr<IMessage> msg;
    ObtainMessage(what, arg1, arg2, (IMessage**)&msg);
    Boolean result;
    return smh->SendMessage(msg, &result);
}

ECode StateMachine::SendMessage(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj)
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return NOERROR;

    AutoPtr<IMessage> msg;
    ObtainMessage(what, arg1, arg2, obj, (IMessage**)&msg);
    Boolean result;
    return smh->SendMessage(msg, &result);
}

ECode StateMachine::SendMessage(
    /* [in] */ IMessage* msg)
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return NOERROR;

    Boolean result;
    return smh->SendMessage(msg, &result);
}

ECode StateMachine::SendMessageDelayed(
    /* [in] */ Int32 what,
    /* [in] */ Int64 delayMillis)
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return NOERROR;

    AutoPtr<IMessage> msg;
    smh->ObtainMessage(what, (IMessage**)&msg);
    Boolean result;
    return smh->SendMessageDelayed(msg, delayMillis, &result);
}

ECode StateMachine::SendMessageDelayed(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj,
    /* [in] */ Int64 delayMillis)
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return NOERROR;

    AutoPtr<IMessage> msg;
    smh->ObtainMessage(what, obj, (IMessage**)&msg);
    Boolean result;
    return smh->SendMessageDelayed(msg, delayMillis, &result);
}

ECode StateMachine::SendMessageDelayed(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int64 delayMillis)
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return NOERROR;

    AutoPtr<IMessage> msg;
    ObtainMessage(what, arg1, (IMessage**)&msg);
    Boolean result;
    return smh->SendMessageDelayed(msg, delayMillis, &result);
}

ECode StateMachine::SendMessageDelayed(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ Int64 delayMillis)
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return NOERROR;

    AutoPtr<IMessage> msg;
    ObtainMessage(what, arg1, arg2, (IMessage**)&msg);
    Boolean result;
    return smh->SendMessageDelayed(msg, delayMillis, &result);
}

ECode StateMachine::SendMessageDelayed(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj,
    /* [in] */ Int64 delayMillis)
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return NOERROR;

    AutoPtr<IMessage> msg;
    ObtainMessage(what, arg1, arg2, obj, (IMessage**)&msg);
    Boolean result;
    return smh->SendMessageDelayed(msg, delayMillis, &result);
}

ECode StateMachine::SendMessageDelayed(
    /* [in] */ IMessage* msg,
    /* [in] */ Int64 delayMillis)
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return NOERROR;

    Boolean result;
    return smh->SendMessageDelayed(msg, delayMillis, &result);
}

void StateMachine::SendMessageAtFrontOfQueue(
    /* [in] */ Int32 what)
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return;

    AutoPtr<IMessage> msg;
    smh->ObtainMessage(what, (IMessage**)&msg);
    Boolean result;
    smh->SendMessageAtFrontOfQueue(msg, &result);
}

void StateMachine::SendMessageAtFrontOfQueue(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return;

    AutoPtr<IMessage> msg;
    smh->ObtainMessage(what, obj, (IMessage**)&msg);
    Boolean result;
    smh->SendMessageAtFrontOfQueue(msg, &result);
}

void StateMachine::SendMessageAtFrontOfQueue(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1)
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return;

    AutoPtr<IMessage> msg;
    smh->ObtainMessage(what, arg1, 0, (IMessage**)&msg);
    Boolean result;
    smh->SendMessageAtFrontOfQueue(msg, &result);
}

void StateMachine::SendMessageAtFrontOfQueue(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2)
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return;

    AutoPtr<IMessage> msg;
    smh->ObtainMessage(what, arg1, arg2, (IMessage**)&msg);
    Boolean result;
    smh->SendMessageAtFrontOfQueue(msg, &result);
}

void StateMachine::SendMessageAtFrontOfQueue(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj)
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return;

    AutoPtr<IMessage> msg;
    smh->ObtainMessage(what, arg1, arg2, obj, (IMessage**)&msg);
    Boolean result;
    smh->SendMessageAtFrontOfQueue(msg, &result);
}

void StateMachine::SendMessageAtFrontOfQueue(
    /* [in] */ IMessage* msg)
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return;

    Boolean result;
    smh->SendMessageAtFrontOfQueue(msg, &result);
}

void StateMachine::RemoveMessages(
    /* [in] */ Int32 what)
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return;

    smh->RemoveMessages(what);
}

/**
 * Validate that the message was sent by
 * {@link StateMachine#quit} or {@link StateMachine#quitNow}.
 * */
Boolean StateMachine::IsQuit(
    /* [in] */ IMessage* msg)
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) {
        Int32 what;
        msg->GetWhat(&what);
        return what == SM_QUIT_CMD;
    }

    return smh->IsQuit(msg);
}

void StateMachine::Quit()
{
    // mSmHandler can be null if the state machine is already stopped.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return;

    smh->Quit();
}

void StateMachine::QuitNow()
{
    // mSmHandler can be null if the state machine is already stopped.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return;

    smh->QuitNow();
}

ECode StateMachine::IsDbg(
    /* [out] */ Boolean* isDbg)
{
    VALIDATE_NOT_NULL(isDbg)
    *isDbg = FALSE;
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return NOERROR;

    *isDbg = smh->IsDbg();
    return NOERROR;
}

ECode StateMachine::SetDbg(
    /* [in] */ Boolean dbg)
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return NOERROR;

    smh->SetDbg(dbg);
    return NOERROR;
}

ECode StateMachine::Start()
{
    // mSmHandler can be null if the state machine has quit.
    AutoPtr<SmHandler> smh = mSmHandler;
    if (smh == NULL) return NOERROR;

    /** Send the complete construction message */
    smh->CompleteConstruction();
    return NOERROR;
}

ECode StateMachine::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    String name;
    GetName(&name);
    pw->Println(name + ":");
    Int32 count;
    GetLogRecCount(&count);
    StringBuilder sb(" total records=");
    sb += count;
    pw->Println(sb.ToString());
    Int32 size;
    GetLogRecSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IStateMachineLogRec> logRec;
        String str, strLogRec;
        IObject::Probe(logRec)->ToString(&strLogRec);
        str.AppendFormat(" rec[%d]: %s\n", i, strLogRec.string());
        pw->Print(str);
        IFlushable::Probe(pw)->Flush();
    }
    pw->Println(String("curState=") + ((State*)GetCurrentState().Get())->GetName());
    return NOERROR;
}

void StateMachine::LogAndAddLogRec(
    /* [in] */ const String& s)
{
    AddLogRec(s);
    Log(s);
}

void StateMachine::Log(
    /* [in] */ const String& s)
{
    Logger::D(mName, s);
}

void StateMachine::Logd(
    /* [in] */ const String& s)
{
    Logger::D(mName, s);
}

void StateMachine::Logv(
    /* [in] */ const String& s)
{
    Logger::V(mName, s);
}

void StateMachine::Logi(
    /* [in] */ const String& s)
{
    Logger::I(mName, s);
}

void StateMachine::Logw(
    /* [in] */ const String& s)
{
    Logger::W(mName, s);
}

void StateMachine::Loge(
    /* [in] */ const String& s)
{
    Logger::E(mName, s);
}

/**
 * Log with error attribute
 *
 * @param s is string log
 * @param e is a Throwable which logs additional information.
 */
// void StateMachine::Loge(
//     /* [in] */ const String& s,
//     Throwable e) {
//     Log.e(mger::Ame, s, e);
// }

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
