
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "elastos/droid/incallui/InCallPresenter.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::IIterator;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::Concurrent::CConcurrentHashMap;
using Elastos::Utility::Concurrent::CCopyOnWriteArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

//========================================================================
// InCallPresenter::PhoneListener
//========================================================================
ECode InCallPresenter::PhoneListener::OnBringToForeground(
    /* [in] */ IPhone* phone,
    /* [in] */ Boolean showDialpad)
{
    Logger::I("InCallPresenter", "Bringing UI to foreground.");
    mHost->BringToForeground(showDialpad);
    return NOERROR;
}

// @Override
ECode InCallPresenter::PhoneListener::OnCallAdded(
    /* [in] */ IPhone* phone,
    /* [in] */ Elastos::Droid::Telecom::ICall* call)
{
    call->AddListener(mHost->mCallListener);
    return NOERROR;
}

// @Override
ECode InCallPresenter::PhoneListener::OnCallRemoved(
    /* [in] */ IPhone* phone,
    /* [in] */ Elastos::Droid::Telecom::ICall* call)
{
    call->RemoveListener(mHost->mCallListener);
    return NOERROR;
}


//========================================================================
// InCallPresenter::CallListener
//========================================================================
ECode InCallPresenter::CallListener::OnPostDialWait(
    /* [in] */ Elastos::Droid::Telecom::ICall* call,
    /* [in] */ const String& remainingPostDialSequence)
{
    mHost->OnPostDialCharWait(
            CallList::GetInstance()->GetCallByTelecommCall(call)->GetId(),
            remainingPostDialSequence);
    return NOERROR;
}

// @Override
ECode InCallPresenter::CallListener::OnDetailsChanged(
    /* [in] */ Elastos::Droid::Telecom::ICall* call,
    /* [in] */ ICallDetails* details)
{
    AutoPtr<IIterator> it;
    mHost->mDetailsListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        IInCallDetailsListener* listener = IInCallDetailsListener::Probe(obj);
        listener->OnDetailsChanged(CallList::GetInstance()->GetCallByTelecommCall(call),
                details);
    }
    return NOERROR;
}

// @Override
ECode InCallPresenter::CallListener::OnConferenceableCallsChanged(
    /* [in] */ Elastos::Droid::Telecom::ICall* call,
    /* [in] */ IList* conferenceableCalls)
{
    Logger::I("InCallPresenter", "onConferenceableCallsChanged: %s", TO_CSTR(call));
    AutoPtr<IIterator> it;
    mHost->mDetailsListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        IInCallDetailsListener* listener = IInCallDetailsListener::Probe(obj);
        AutoPtr<ICallDetails> details;
        call->GetDetails((ICallDetails**)&details);
        listener->OnDetailsChanged(CallList::GetInstance()->GetCallByTelecommCall(call),
                details);
    }
    return NOERROR;
}


//========================================================================
// InCallPresenter
//========================================================================
const String InCallPresenter::EXTRA_FIRST_TIME_SHOWN("com.android.incallui.intent.extra.FIRST_TIME_SHOWN");
AutoPtr<InCallPresenter> InCallPresenter::sInCallPresenter;

CAR_INTERFACE_IMPL_2(InCallPresenter, Object, ICallListListener, IInCallPhoneListener)

InCallPresenter::InCallPresenter()
    : mInCallState(InCallState_NO_CALLS)
    , mServiceConnected(FALSE)
    , mAccountSelectionCancelled(FALSE)
    , mIsActivityPreviouslyStarted(FALSE)
{
    AutoPtr<IMap> map;
    CConcurrentHashMap::New(8, 0.9f, 1, (IMap**)&map);
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    collections->SetFromMap(map, (ISet**)&mListeners);
    CCopyOnWriteArrayList::New((IList**)&mIncomingCallListeners);
    map = NULL;
    CConcurrentHashMap::New(8, 0.9f, 1, (IMap**)&map);
    collections->SetFromMap(map, (ISet**)&mDetailsListeners);
    map = NULL;
    CConcurrentHashMap::New(8, 0.9f, 1, (IMap**)&map);
    collections->SetFromMap(map, (ISet**)&mOrientationListeners);
    map = NULL;
    CConcurrentHashMap::New(8, 0.9f, 1, (IMap**)&map);
    collections->SetFromMap(map, (ISet**)&mInCallEventListeners);

    mPhoneListener = new PhoneListener(this);
    mCallListener = new CallListener(this);
}

AutoPtr<InCallPresenter> InCallPresenter::GetInstance()
{
    if (sInCallPresenter == NULL) {
        sInCallPresenter = new InCallPresenter();
    }
    return sInCallPresenter;
}

ECode InCallPresenter::SetPhone(
    /* [in] */ IPhone* phone)
{
    mPhone = phone;
    mPhone->AddListener(mPhoneListener);
    return NOERROR;
}

ECode InCallPresenter::ClearPhone()
{
    mPhone->RemoveListener(mPhoneListener);
    mPhone = NULL;
    return NOERROR;
}

InCallState InCallPresenter::GetInCallState()
{
    return mInCallState;
}

AutoPtr<CallList> InCallPresenter::GetCallList()
{
    return mCallList;
}

void InCallPresenter::SetUp(
    /* [in] */ IContext* context,
    /* [in] */ CallList* callList,
    /* [in] */ AudioModeProvider* audioModeProvider)
{
    if (mServiceConnected) {
        Logger::I("InCallPresenter", "New service connection replacing existing one.");
        // retain the current resources, no need to create new ones.
        assert(context == mContext.Get());
        assert(callList == mCallList.Get());
        assert(audioModeProvider == mAudioModeProvider.Get());
        return;
    }

    assert(context != NULL);
    mContext = context;

    mContactInfoCache = ContactInfoCache::GetInstance(context);

    mStatusBarNotifier = new StatusBarNotifier(context, mContactInfoCache);
    AddListener(mStatusBarNotifier);

    mAudioModeProvider = audioModeProvider;

    mProximitySensor = new ProximitySensor(context, mAudioModeProvider);
    AddListener(mProximitySensor);

    mCallList = callList;

    // This only gets called by the service so this is okay.
    mServiceConnected = TRUE;

    // The final thing we do in this set up is add ourselves as a listener to CallList.  This
    // will kick off an update and the whole process can start.
    mCallList->AddListener(this);

    Logger::D("InCallPresenter", "Finished InCallPresenter.setUp");
}

void InCallPresenter::TearDown()
{
    Logger::D("InCallPresenter", "tearDown");
    mServiceConnected = FALSE;
    AttemptCleanup();
}

void InCallPresenter::AttemptFinishActivity()
{
    Boolean doFinish = (mInCallActivity != NULL && IsActivityStarted());
    Logger::I("InCallPresenter", "Hide in call UI: %d", doFinish);

    if (doFinish) {
        mInCallActivity->Finish();

        if (mAccountSelectionCancelled) {
            // This finish is a result of account selection cancellation
            // do not include activity ending transition
            mInCallActivity->OverridePendingTransition(0, 0);
        }
    }
}

void InCallPresenter::SetActivity(
    /* [in] */ CInCallActivity* inCallActivity)
{
    Boolean updateListeners = FALSE;
    Boolean doAttemptCleanup = FALSE;

    if (inCallActivity != NULL) {
        if (mInCallActivity == NULL) {
            updateListeners = TRUE;
            Logger::I("InCallPresenter", "UI Initialized");
        }
        else if (mInCallActivity.Get() != inCallActivity) {
            Logger::E("InCallPresenter", "Setting a second activity before destroying the first.");
        }
        else {
            // since setActivity is called onStart(), it can be called multiple times.
            // This is fine and ignorable, but we do not want to update the world every time
            // this happens (like going to/from background) so we do not set updateListeners.
        }

        mInCallActivity = inCallActivity;

        // By the time the UI finally comes up, the call may already be disconnected.
        // If that's the case, we may need to show an error dialog.
        if (mCallList != NULL && mCallList->GetDisconnectedCall() != NULL) {
            MaybeShowErrorDialogOnDisconnect(mCallList->GetDisconnectedCall());
        }

        // When the UI comes up, we need to first check the in-call state.
        // If we are showing NO_CALLS, that means that a call probably connected and
        // then immediately disconnected before the UI was able to come up.
        // If we dont have any calls, start tearing down the UI instead.
        // NOTE: This code relies on {@link #mInCallActivity} being set so we run it after
        // it has been set.
        if (mInCallState == InCallState_NO_CALLS) {
            Logger::I("InCallPresenter", "UI Intialized, but no calls left.  shut down.");
            AttemptFinishActivity();
            return;
        }
    }
    else {
        Logger::I("InCallPresenter", "UI Destroyed)");
        updateListeners = TRUE;
        mInCallActivity = NULL;

        // We attempt cleanup for the destroy case but only after we recalculate the state
        // to see if we need to come back up or stay shut down. This is why we do the cleanup
        // after the call to onCallListChange() instead of directly here.
        doAttemptCleanup = TRUE;
    }

    // Messages can come from the telephony layer while the activity is coming up
    // and while the activity is going down.  So in both cases we need to recalculate what
    // state we should be in after they complete.
    // Examples: (1) A new incoming call could come in and then get disconnected before
    //               the activity is created.
    //           (2) All calls could disconnect and then get a new incoming call before the
    //               activity is destroyed.
    //
    // b/1122139 - We previously had a check for mServiceConnected here as well, but there are
    // cases where we need to recalculate the current state even if the service in not
    // connected.  In particular the case where startOrFinish() is called while the app is
    // already finish()ing. In that case, we skip updating the state with the knowledge that
    // we will check again once the activity has finished. That means we have to recalculate the
    // state here even if the service is disconnected since we may not have finished a state
    // transition while finish()ing.
    if (updateListeners) {
        OnCallListChange(mCallList);
    }

    if (doAttemptCleanup) {
        AttemptCleanup();
    }
}

ECode InCallPresenter::OnCallListChange(
    /* [in] */ ICallList* _callList)
{
    if (_callList == NULL) {
        return NOERROR;
    }
    CallList* callList = (CallList*)_callList;
    InCallState newState = GetPotentialStateFromCallList(callList);
    InCallState oldState = mInCallState;
    newState = StartOrFinishUi(newState);

    // Set the new state before announcing it to the world
    Logger::I("InCallPresenter", "Phone switching state: %d -> %d", oldState, newState);
    mInCallState = newState;

    // notify listeners of new state
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> object;
        it->GetNext((IInterface**)&object);
        IInCallStateListener* listener = IInCallStateListener::Probe(it);
        Logger::D("InCallPresenter", "Notify %s of state %d", TO_CSTR(listener), mInCallState);
        listener->OnStateChange(oldState, mInCallState, callList);
    }

    if (IsActivityStarted()) {
        Boolean hasCall = callList->GetActiveOrBackgroundCall() != NULL ||
                callList->GetOutgoingCall() != NULL;
        mInCallActivity->DismissKeyguard(hasCall);
    }
    return NOERROR;
}

ECode InCallPresenter::OnIncomingCall(
    /* [in] */ ICall* call)
{
    InCallState newState = StartOrFinishUi(InCallState_INCOMING);
    InCallState oldState = mInCallState;

    Logger::I("InCallPresenter", "Phone switching state: %s -> %s", oldState, newState);
    mInCallState = newState;

    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> object;
        it->GetNext((IInterface**)&object);
        IIncomingCallListener* listener = IIncomingCallListener::Probe(it);
        listener->OnIncomingCall(oldState, mInCallState, call);
    }
    return NOERROR;
}

ECode InCallPresenter::OnDisconnect(
    /* [in] */ ICall* call)
{
    HideDialpadForDisconnect();
    MaybeShowErrorDialogOnDisconnect((Call*)call);

    // We need to do the run the same code as onCallListChange.
    OnCallListChange(CallList::GetInstance());

    if (IsActivityStarted()) {
        mInCallActivity->DismissKeyguard(FALSE);
    }
    return NOERROR;
}

InCallState InCallPresenter::GetPotentialStateFromCallList(
    /* [in] */ CallList* callList)
{
    InCallState newState = InCallState.NO_CALLS;

    if (callList == null) {
        return newState;
    }
    if (callList.getIncomingCall() != null) {
        newState = InCallState.INCOMING;
    } else if (callList.getWaitingForAccountCall() != null) {
        newState = InCallState.WAITING_FOR_ACCOUNT;
    } else if (callList.getPendingOutgoingCall() != null) {
        newState = InCallState.PENDING_OUTGOING;
    } else if (callList.getOutgoingCall() != null) {
        newState = InCallState.OUTGOING;
    } else if (callList.getActiveCall() != null ||
            callList.getBackgroundCall() != null ||
            callList.getDisconnectedCall() != null ||
            callList.getDisconnectingCall() != null) {
        newState = InCallState.INCALL;
    }

    return newState;
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
