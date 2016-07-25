
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Telecom.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "elastos/droid/incallui/InCallPresenter.h"
#include "elastos/droid/incallui/TelecomAdapter.h"
#include "elastos/droid/incalluibind/ObjectFactory.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::InCallUIBind::ObjectFactory;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Telecom::IPhoneCapabilities;
using Elastos::Droid::Telecom::IVideoProfile;
using Elastos::Droid::Telecom::CVideoProfile;
using Elastos::Droid::Telecom::IVideoProfileVideoState;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::ISurface;
using Elastos::Core::CoreUtils;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocale;
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
    InCallState newState = InCallState_NO_CALLS;

    if (callList == NULL) {
        return newState;
    }
    if (callList->GetIncomingCall() != NULL) {
        newState = InCallState_INCOMING;
    }
    else if (callList->GetWaitingForAccountCall() != NULL) {
        newState = InCallState_WAITING_FOR_ACCOUNT;
    }
    else if (callList->GetPendingOutgoingCall() != NULL) {
        newState = InCallState_PENDING_OUTGOING;
    }
    else if (callList->GetOutgoingCall() != NULL) {
        newState = InCallState_OUTGOING;
    }
    else if (callList->GetActiveCall() != NULL ||
            callList->GetBackgroundCall() != NULL ||
            callList->GetDisconnectedCall() != NULL ||
            callList->GetDisconnectingCall() != NULL) {
        newState = InCallState_INCALL;
    }

    return newState;
}

void InCallPresenter::AddIncomingCallListener(
    /* [in] */ IIncomingCallListener* listener)
{
    assert(listener != NULL);
    mIncomingCallListeners->Add(listener);
}

void InCallPresenter::RemoveIncomingCallListener(
    /* [in] */ IIncomingCallListener* listener)
{
    if (listener != NULL) {
        mIncomingCallListeners->Remove(listener);
    }
}

void InCallPresenter::AddListener(
    /* [in] */ IInCallStateListener* listener)
{
    assert(listener != NULL);
    mListeners->Add(listener);
}

void InCallPresenter::RemoveListener(
    /* [in] */ IInCallStateListener* listener)
{
    if (listener != NULL) {
        mListeners->Remove(listener);
    }
}

void InCallPresenter::AddDetailsListener(
    /* [in] */ IInCallDetailsListener* listener)
{
    assert(listener != NULL);
    mDetailsListeners->Add(listener);
}

void InCallPresenter::RemoveDetailsListener(
    /* [in] */ IInCallDetailsListener* listener)
{
    if (listener != NULL) {
        mDetailsListeners->Remove(listener);
    }
}

void InCallPresenter::AddOrientationListener(
    /* [in] */ IInCallOrientationListener* listener)
{
    assert(listener != NULL);
    mOrientationListeners->Add(listener);
}

void InCallPresenter::RemoveOrientationListener(
    /* [in] */ IInCallOrientationListener* listener)
{
    if (listener != NULL) {
        mOrientationListeners->Remove(listener);
    }
}

void InCallPresenter::AddInCallEventListener(
    /* [in] */ IInCallEventListener* listener)
{
    assert(listener != NULL);
    mInCallEventListeners->Add(listener);
}

void InCallPresenter::RemoveInCallEventListener(
    /* [in] */ IInCallEventListener* listener)
{
    if (listener != NULL) {
        mInCallEventListeners->Remove(listener);
    }
}

AutoPtr<ProximitySensor> InCallPresenter::GetProximitySensor()
{
    return mProximitySensor;
}

void InCallPresenter::HandleAccountSelection(
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    AutoPtr<Call> call = mCallList->GetWaitingForAccountCall();
    if (call != NULL) {
        String callId = call->GetId();
        TelecomAdapter::GetInstance()->PhoneAccountSelected(callId, accountHandle);
    }
}

void InCallPresenter::CancelAccountSelection()
{
    mAccountSelectionCancelled = TRUE;
    AutoPtr<Call> call = mCallList->GetWaitingForAccountCall();
    if (call != NULL) {
        String callId = call->GetId();
        TelecomAdapter::GetInstance()->DisconnectCall(callId);
    }
}

void InCallPresenter::HangUpOngoingCall(
    /* [in] */ IContext* context)
{
    // By the time we receive this intent, we could be shut down and call list
    // could be null.  Bail in those cases.
    if (mCallList == NULL) {
        if (mStatusBarNotifier == NULL) {
            // The In Call UI has crashed but the notification still stayed up. We should not
            // come to this stage.
            StatusBarNotifier::ClearInCallNotification(context);
        }
        return;
    }

    AutoPtr<Call> call = mCallList->GetOutgoingCall();
    if (call == NULL) {
        call = mCallList->GetActiveOrBackgroundCall();
    }

    if (call != NULL) {
        TelecomAdapter::GetInstance()->DisconnectCall(call->GetId());
        call->SetState(Call::State::DISCONNECTING);
        mCallList->OnUpdate(call);
    }
}

void InCallPresenter::AnswerIncomingCall(
    /* [in] */ IContext* context,
    /* [in] */ Int32 videoState)
{
    // By the time we receive this intent, we could be shut down and call list
    // could be null.  Bail in those cases.
    if (mCallList == NULL) {
        StatusBarNotifier::ClearInCallNotification(context);
        return;
    }

    AutoPtr<Call> call = mCallList->GetIncomingCall();
    if (call != NULL) {
        TelecomAdapter::GetInstance()->AnswerCall(call->GetId(), videoState);
        ShowInCall(FALSE, FALSE/* newOutgoingCall */);
    }
}

void InCallPresenter::DeclineIncomingCall(
    /* [in] */ IContext* context)
{
    // By the time we receive this intent, we could be shut down and call list
    // could be null.  Bail in those cases.
    if (mCallList == NULL) {
        StatusBarNotifier::ClearInCallNotification(context);
        return;
    }

    AutoPtr<Call> call = mCallList->GetIncomingCall();
    if (call != NULL) {
        TelecomAdapter::GetInstance()->RejectCall(call->GetId(), FALSE, String(NULL));
    }
}

void InCallPresenter::AcceptUpgradeRequest(
    /* [in] */ IContext* context)
{
    // Bail if we have been shut down and the call list is null.
    if (mCallList == NULL) {
        StatusBarNotifier::ClearInCallNotification(context);
        return;
    }

    AutoPtr<Call> call = mCallList->GetVideoUpgradeRequestCall();
    if (call != NULL) {
        AutoPtr<IVideoProfile> videoProfile;
        CVideoProfile::New(IVideoProfileVideoState::BIDIRECTIONAL, (IVideoProfile**)&videoProfile);
        call->GetVideoCall()->SendSessionModifyResponse(videoProfile);
        call->SetSessionModificationState(Call::SessionModificationState::NO_REQUEST);
    }
}

void InCallPresenter::DeclineUpgradeRequest(
    /* [in] */ IContext* context)
{
    // Bail if we have been shut down and the call list is null.
    if (mCallList == NULL) {
        StatusBarNotifier::ClearInCallNotification(context);
        return;
    }

    AutoPtr<Call> call = mCallList->GetVideoUpgradeRequestCall();
    if (call != NULL) {
        AutoPtr<IVideoProfile> videoProfile;
        CVideoProfile::New(IVideoProfileVideoState::AUDIO_ONLY, (IVideoProfile**)&videoProfile);
        call->GetVideoCall()->SendSessionModifyResponse(videoProfile);
        call->SetSessionModificationState(Call::SessionModificationState::NO_REQUEST);
    }
}

Boolean InCallPresenter::IsShowingInCallUi()
{
    return (IsActivityStarted() && mInCallActivity->IsForegroundActivity());
}

Boolean InCallPresenter::IsActivityStarted()
{
    Boolean isDestroyed, isFinishing;
    return (mInCallActivity != NULL &&
            (mInCallActivity->IsDestroyed(&isDestroyed), !isDestroyed) &&
            (mInCallActivity->IsFinishing(&isFinishing), !isFinishing));
}

Boolean InCallPresenter::IsActivityPreviouslyStarted()
{
    return mIsActivityPreviouslyStarted;
}

void InCallPresenter::OnUiShowing(
    /* [in]*/ Boolean showing)
{
    // We need to update the notification bar when we leave the UI because that
    // could trigger it to show again.
    if (mStatusBarNotifier != NULL) {
        mStatusBarNotifier->UpdateNotification(mInCallState, mCallList);
    }

    if (mProximitySensor != NULL) {
        mProximitySensor->OnInCallShowing(showing);
    }

    AutoPtr<IIntent> broadcastIntent = ObjectFactory::GetUiReadyBroadcastIntent(mContext);
    if (broadcastIntent != NULL) {
        broadcastIntent->PutBooleanExtra(EXTRA_FIRST_TIME_SHOWN, !mIsActivityPreviouslyStarted);

        if (showing) {
            Logger::D("InCallPresenter", "Sending sticky broadcast: %s", TO_CSTR(broadcastIntent));
            mContext->SendStickyBroadcast(broadcastIntent);
        }
        else {
            Logger::D("InCallPresenter", "Removing sticky broadcast: %s", TO_CSTR(broadcastIntent));
            mContext->RemoveStickyBroadcast(broadcastIntent);
        }
    }

    if (showing) {
        mIsActivityPreviouslyStarted = TRUE;
    }
}

void InCallPresenter::BringToForeground(
    /* [in] */ Boolean showDialpad)
{
    // Before we bring the incall UI to the foreground, we check to see if:
    // 1. It is not currently in the foreground
    // 2. We are in a state where we want to show the incall ui (i.e. there are calls to
    // be displayed)
    // If the activity hadn't actually been started previously, yet there are still calls
    // present (e.g. a call was accepted by a bluetooth or wired headset), we want to
    // bring it up the UI regardless.
    if (!IsShowingInCallUi() && mInCallState != InCallState_NO_CALLS) {
        ShowInCall(showDialpad, FALSE /* newOutgoingCall */);
    }
}

void InCallPresenter::OnPostDialCharWait(
    /* [in] */ const String& callId,
    /* [in] */ const String& chars)
{
    if (IsActivityStarted()) {
        mInCallActivity->ShowPostCharWaitDialog(callId, chars);
    }
}

Boolean InCallPresenter::HandleCallKey()
{
    Logger::V("InCallPresenter", "handleCallKey");

    // The green CALL button means either "Answer", "Unhold", or
    // "Swap calls", or can be a no-op, depending on the current state
    // of the Phone.

    /**
     * INCOMING CALL
     */
    AutoPtr<CallList> calls = CallList::GetInstance();
    AutoPtr<Call> incomingCall = calls->GetIncomingCall();
    Logger::V("InCallPresenter", "incomingCall: %s", TO_CSTR(incomingCall));

    // (1) Attempt to answer a call
    if (incomingCall != NULL) {
        TelecomAdapter::GetInstance()->AnswerCall(
                incomingCall->GetId(), IVideoProfileVideoState::AUDIO_ONLY);
        return TRUE;
    }

    /**
     * STATE_ACTIVE CALL
     */
    AutoPtr<Call> activeCall = calls->GetActiveCall();
    if (activeCall != NULL) {
        // TODO: This logic is repeated from CallButtonPresenter.java. We should
        // consolidate this logic.
        Boolean canMerge = activeCall->Can(IPhoneCapabilities::MERGE_CONFERENCE);
        Boolean canSwap = activeCall->Can(IPhoneCapabilities::SWAP_CONFERENCE);

        Logger::V("InCallPresenter", "activeCall: %s, canMerge: %d, canSwap: %d", TO_CSTR(activeCall),
                canMerge, canSwap);

        // (2) Attempt actions on conference calls
        if (canMerge) {
            TelecomAdapter::GetInstance()->Merge(activeCall->GetId());
            return TRUE;
        }
        else if (canSwap) {
            TelecomAdapter::GetInstance()->Swap(activeCall->GetId());
            return TRUE;
        }
    }

    /**
     * BACKGROUND CALL
     */
    AutoPtr<Call> heldCall = calls->GetBackgroundCall();
    if (heldCall != NULL) {
        // We have a hold call so presumeable it will always support HOLD...but
        // there is no harm in double checking.
        Boolean canHold = heldCall->Can(IPhoneCapabilities::HOLD);

        Logger::V("InCallPresenter", "heldCall: %s, canHold: %d", TO_CSTR(heldCall), canHold);

        // (4) unhold call
        if (heldCall->GetState() == Call::State::ONHOLD && canHold) {
            TelecomAdapter::GetInstance()->UnholdCall(heldCall->GetId());
            return TRUE;
        }
    }

    // Always consume hard keys
    return TRUE;
}

void InCallPresenter::OnDismissDialog()
{
    Logger::I("InCallPresenter", "Dialog dismissed");
    if (mInCallState == InCallState_NO_CALLS) {
        AttemptFinishActivity();
        AttemptCleanup();
    }
}

void InCallPresenter::SetFullScreenVideoState(
    /* [in] */ Boolean isFullScreenVideo)
{
    AutoPtr<IIterator> it;
    mInCallEventListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        IInCallEventListener* listener = IInCallEventListener::Probe(obj);
        listener->OnFullScreenVideoStateChanged(isFullScreenVideo);
    }
}

void InCallPresenter::MaybeShowErrorDialogOnDisconnect(
    /* [in] */ Call* call)
{
    // For newly disconnected calls, we may want to show a dialog on specific error conditions
    if (IsActivityStarted() && call->GetState() == Call::State::DISCONNECTED) {
        if (call->GetAccountHandle() == NULL && !call->IsConferenceCall()) {
            SetDisconnectCauseForMissingAccounts(call);
        }
        mInCallActivity->MaybeShowErrorDialogOnDisconnect(call->GetDisconnectCause());
    }
}

void InCallPresenter::HideDialpadForDisconnect()
{
    if (IsActivityStarted()) {
        mInCallActivity->HideDialpadForDisconnect();
    }
}

InCallState InCallPresenter::StartOrFinishUi(
    /* [in] */ InCallState newState)
{
    Logger::D("InCallPresenter", "startOrFinishUi: %d -> %d", mInCallState, newState);

    // TODO: Consider a proper state machine implementation

    // If the state isn't changing or if we're transitioning from pending outgoing to actual
    // outgoing, we have already done any starting/stopping of activities in a previous pass
    // ...so lets cut out early
    Boolean alreadyOutgoing = mInCallState == InCallState_PENDING_OUTGOING &&
            newState == InCallState_OUTGOING;
    if (newState == mInCallState || alreadyOutgoing) {
        return newState;
    }

    // A new Incoming call means that the user needs to be notified of the the call (since
    // it wasn't them who initiated it).  We do this through full screen notifications and
    // happens indirectly through {@link StatusBarNotifier}.
    //
    // The process for incoming calls is as follows:
    //
    // 1) CallList          - Announces existence of new INCOMING call
    // 2) InCallPresenter   - Gets announcement and calculates that the new InCallState
    //                      - should be set to INCOMING.
    // 3) InCallPresenter   - This method is called to see if we need to start or finish
    //                        the app given the new state.
    // 4) StatusBarNotifier - Listens to InCallState changes. InCallPresenter calls
    //                        StatusBarNotifier explicitly to issue a FullScreen Notification
    //                        that will either start the InCallActivity or show the user a
    //                        top-level notification dialog if the user is in an immersive app.
    //                        That notification can also start the InCallActivity.
    // 5) InCallActivity    - Main activity starts up and at the end of its onCreate will
    //                        call InCallPresenter::setActivity() to let the presenter
    //                        know that start-up is complete.
    //
    //          [ AND NOW YOU'RE IN THE CALL. voila! ]
    //
    // Our app is started using a fullScreen notification.  We need to do this whenever
    // we get an incoming call.
    Boolean startStartupSequence = (InCallState_INCOMING == newState);

    // A dialog to show on top of the InCallUI to select a PhoneAccount
    Boolean showAccountPicker = (InCallState_WAITING_FOR_ACCOUNT == newState);

    // A new outgoing call indicates that the user just now dialed a number and when that
    // happens we need to display the screen immediately or show an account picker dialog if
    // no default is set. However, if the main InCallUI is already visible, we do not want to
    // re-initiate the start-up animation, so we do not need to do anything here.
    //
    // It is also possible to go into an intermediate state where the call has been initiated
    // but Telecomm has not yet returned with the details of the call (handle, gateway, etc.).
    // This pending outgoing state can also launch the call screen.
    //
    // This is different from the incoming call sequence because we do not need to shock the
    // user with a top-level notification.  Just show the call UI normally.
    Boolean mainUiNotVisible = !IsShowingInCallUi() || !GetCallCardFragmentVisible();
    Boolean showCallUi = ((InCallState_PENDING_OUTGOING == newState ||
            InCallState_OUTGOING == newState) && mainUiNotVisible);

    // TODO: Can we be suddenly in a call without it having been in the outgoing or incoming
    // state?  I havent seen that but if it can happen, the code below should be enabled.
    // showCallUi |= (InCallState.INCALL && !isActivityStarted());

    // The only time that we have an instance of mInCallActivity and it isn't started is
    // when it is being destroyed.  In that case, lets avoid bringing up another instance of
    // the activity.  When it is finally destroyed, we double check if we should bring it back
    // up so we aren't going to lose anything by avoiding a second startup here.
    Boolean activityIsFinishing = mInCallActivity != NULL && !IsActivityStarted();
    if (activityIsFinishing) {
        Logger::I("InCallPresenter", "Undo the state change: %d -> %d", newState, mInCallState);
        return mInCallState;
    }

    if (showCallUi || showAccountPicker) {
        Logger::I("InCallPresenter", "Start in call UI");
        ShowInCall(FALSE /* showDialpad */, !showAccountPicker /* newOutgoingCall */);
    }
    else if (startStartupSequence) {
        Logger::I("InCallPresenter", "Start Full Screen in call UI");

        // We're about the bring up the in-call UI for an incoming call. If we still have
        // dialogs up, we need to clear them out before showing incoming screen.
        if (IsActivityStarted()) {
            mInCallActivity->DismissPendingDialogs();
        }
        if (!StartUi(newState)) {
            // startUI refused to start the UI. This indicates that it needed to restart the
            // activity.  When it finally restarts, it will call us back, so we do not actually
            // change the state yet (we return mInCallState instead of newState).
            return mInCallState;
        }
    }
    else if (newState == InCallState_NO_CALLS) {
        // The new state is the no calls state.  Tear everything down.
        AttemptFinishActivity();
        AttemptCleanup();
    }

    return newState;
}

void InCallPresenter::SetDisconnectCauseForMissingAccounts(
    /* [in] */ Call* call)
{
    AutoPtr<Elastos::Droid::Telecom::ICall> telecomCall = call->GetTelecommCall();

    AutoPtr<ICallDetails> details;
    telecomCall->GetDetails((ICallDetails**)&details);
    AutoPtr<IBundle> extras;
    details->GetExtras((IBundle**)&extras);
    // Initialize the extras bundle to avoid NPE
    if (extras == NULL) {
        CBundle::New((IBundle**)&extras);
    }

    AutoPtr<IArrayList> phoneAccountHandles;
    extras->GetParcelableArrayList(Elastos::Droid::Telecom::ICall::AVAILABLE_PHONE_ACCOUNTS,
            (IArrayList**)&phoneAccountHandles);

    Boolean isEmpty;
    if (phoneAccountHandles == NULL || (phoneAccountHandles->IsEmpty(&isEmpty), isEmpty)) {
        details = NULL;
        telecomCall->GetDetails((ICallDetails**)&details);
        AutoPtr<IUri> handle;
        details->GetHandle((IUri**)&handle);
        String scheme;
        handle->GetScheme(&scheme);
        String errorMsg;
        if (IPhoneAccount::SCHEME_TEL.Equals(scheme)) {
            mContext->GetString(R::string::callFailed_simError, &errorMsg);
        }
        else {
            mContext->GetString(R::string::incall_error_supp_service_unknown, &errorMsg);
        }
        AutoPtr<IDisconnectCause> disconnectCause;
        CDisconnectCause::New(IDisconnectCause::ERROR, NULL, CoreUtils::Convert(errorMsg), errorMsg,
                (IDisconnectCause**)&disconnectCause);
        call->SetDisconnectCause(disconnectCause);
    }
}

Boolean InCallPresenter::StartUi(
    /* [in] */ InCallState inCallState)
{
    Boolean isCallWaiting = mCallList->GetActiveCall() != NULL &&
            mCallList->GetIncomingCall() != NULL;

    // If the screen is off, we need to make sure it gets turned on for incoming calls.
    // This normally works just fine thanks to FLAG_TURN_SCREEN_ON but that only works
    // when the activity is first created. Therefore, to ensure the screen is turned on
    // for the call waiting case, we finish() the current activity and start a new one.
    // There should be no jank from this since the screen is already off and will remain so
    // until our new activity is up.

    if (isCallWaiting) {
        if (mProximitySensor->IsScreenReallyOff() && IsActivityStarted()) {
            mInCallActivity->Finish();
            // When the activity actually finishes, we will start it again if there are
            // any active calls, so we do not need to start it explicitly here. Note, we
            // actually get called back on this function to restart it.

            // We return false to indicate that we did not actually start the UI.
            return FALSE;
        }
        else {
            ShowInCall(FALSE, FALSE);
        }
    }
    else {
        mStatusBarNotifier->UpdateNotification(inCallState, mCallList);
    }
    return TRUE;
}

void InCallPresenter::AttemptCleanup()
{
    Boolean shouldCleanup = (mInCallActivity == NULL && !mServiceConnected &&
            mInCallState == InCallState_NO_CALLS);
    Logger::I("InCallPresenter", "attemptCleanup? %d", shouldCleanup);

    if (shouldCleanup) {
        mIsActivityPreviouslyStarted = FALSE;

        // blow away stale contact info so that we get fresh data on
        // the next set of calls
        if (mContactInfoCache != NULL) {
            mContactInfoCache->ClearCache();
        }
        mContactInfoCache = NULL;

        if (mProximitySensor != NULL) {
            RemoveListener(mProximitySensor);
            mProximitySensor->TearDown();
        }
        mProximitySensor = NULL;

        mAudioModeProvider = NULL;

        if (mStatusBarNotifier != NULL) {
            RemoveListener(mStatusBarNotifier);
        }
        mStatusBarNotifier = NULL;

        if (mCallList != NULL) {
            mCallList->RemoveListener(this);
        }
        mCallList = NULL;

        mContext = NULL;
        mInCallActivity = NULL;

        mListeners->Clear();
        mIncomingCallListeners->Clear();

        Logger::D("InCallPresenter", "Finished InCallPresenter.CleanUp");
    }
}

void InCallPresenter::ShowInCall(
    /* [in] */ Boolean showDialpad,
    /* [in] */ Boolean newOutgoingCall)
{
    mContext->StartActivity(GetInCallIntent(showDialpad, newOutgoingCall));
}

AutoPtr<IIntent> InCallPresenter::GetInCallIntent(
    /* [in] */ Boolean showDialpad,
    /* [in] */ Boolean newOutgoingCall)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_MAIN, NULL, (IIntent**)&intent);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
            | IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS
            | IIntent::FLAG_ACTIVITY_NO_USER_ACTION);
    intent->SetClass(mContext, ECLSID_CInCallActivity);
    if (showDialpad) {
        intent->PutBooleanExtra(CInCallActivity::SHOW_DIALPAD_EXTRA, TRUE);
    }

    intent->PutBooleanExtra(CInCallActivity::NEW_OUTGOING_CALL, newOutgoingCall);
    return intent;
}

AutoPtr<InCallCameraManager> InCallPresenter::GetInCallCameraManager()
{
    {
        AutoLock lock(this);
        if (mInCallCameraManager == NULL) {
            mInCallCameraManager = new InCallCameraManager();
            mInCallCameraManager->constructor(mContext);
        }

        return mInCallCameraManager;
    }
}

void InCallPresenter::OnDeviceRotationChange(
    /* [in] */ Int32 rotation)
{
    // First translate to rotation in degrees.
    Int32 rotationAngle;
    switch (rotation) {
        case ISurface::ROTATION_0:
            rotationAngle = 0;
            break;
        case ISurface::ROTATION_90:
            rotationAngle = 90;
            break;
        case ISurface::ROTATION_180:
            rotationAngle = 180;
            break;
        case ISurface::ROTATION_270:
            rotationAngle = 270;
            break;
        default:
            rotationAngle = 0;
    }

    mCallList->NotifyCallsOfDeviceRotation(rotationAngle);
}

void InCallPresenter::OnDeviceOrientationChange(
    /* [in] */ Int32 orientation)
{
    AutoPtr<IIterator> it;
    mOrientationListeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        IInCallOrientationListener* listener = IInCallOrientationListener::Probe(obj);
        listener->OnDeviceOrientationChanged(orientation);
    }
}

void InCallPresenter::SetInCallAllowsOrientationChange(
    /* [in] */ Boolean allowOrientationChange)
{
    if (!allowOrientationChange) {
        mInCallActivity->SetRequestedOrientation(IActivityInfo::SCREEN_ORIENTATION_NOSENSOR);
    }
    else {
        mInCallActivity->SetRequestedOrientation(IActivityInfo::SCREEN_ORIENTATION_SENSOR);
    }
}

Float InCallPresenter::GetSpaceBesideCallCard()
{
    return mInCallActivity->GetCallCardFragment()->GetSpaceBesideCallCard();
}

Boolean InCallPresenter::GetCallCardFragmentVisible()
{
    if (mInCallActivity != NULL) {
        Boolean isVisible;
        mInCallActivity->GetCallCardFragment()->IsVisible(&isVisible);
        return isVisible;
    }
    return FALSE;
}

Boolean InCallPresenter::IsRtl()
{
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> defaultLocale;
    helper->GetDefault((ILocale**)&defaultLocale);
    return TextUtils::GetLayoutDirectionFromLocale(defaultLocale) ==
            IView::LAYOUT_DIRECTION_RTL;
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
