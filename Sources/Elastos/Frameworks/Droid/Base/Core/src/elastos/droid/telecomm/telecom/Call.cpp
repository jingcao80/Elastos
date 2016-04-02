
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/Call.h"

using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::IIterator;
using Elastos::Utility::Concurrent::CCopyOnWriteArrayList;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

//===============================================================
// Call::Details::
//===============================================================
CAR_INTERFACE_IMPL(Call::Details, Object, ICallDetails)

Call::Details::Details(
    /* [in] */ IUri* handle,
    /* [in] */ Int32 handlePresentation,
    /* [in] */ const String& callerDisplayName,
    /* [in] */ Int32 callerDisplayNamePresentation,
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [in] */ Int32 capabilities,
    /* [in] */ Int32 properties,
    /* [in] */ IDisconnectCause* disconnectCause,
    /* [in] */ Int64 connectTimeMillis,
    /* [in] */ IGatewayInfo* gatewayInfo,
    /* [in] */ Int32 videoState,
    /* [in] */ IStatusHints* statusHints,
    /* [in] */ IBundle* extras)
    : mHandle(handle)
    , mHandlePresentation(handlePresentation)
    , mCallerDisplayName(callerDisplayName)
    , mCallerDisplayNamePresentation(callerDisplayNamePresentation)
    , mAccountHandle(accountHandle)
    , mCallCapabilities(capabilities)
    , mCallProperties(properties)
    , mDisconnectCause(disconnectCause)
    , mConnectTimeMillis(connectTimeMillis)
    , mGatewayInfo(gatewayInfo)
    , mVideoState(videoState)
    , mStatusHints(statusHints)
    , mExtras(extras)
{
}

ECode Call::Details::GetHandle(
    /* [out] */ IUri** handle)
{
    VALIDATE_NOT_NULL(handle)
    *handle = mHandle;
    REFCOUNT_ADD(*handle)
    return NOERROR;
}

ECode Call::Details::GetHandlePresentation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHandlePresentation;
    return NOERROR;
}

ECode Call::Details::GetCallerDisplayName(
    /* [out] */ String* disName)
{
    VALIDATE_NOT_NULL(disName)
    *disName = mCallerDisplayName;
    return NOERROR;
}

ECode Call::Details::GetCallerDisplayNamePresentation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCallerDisplayNamePresentation;
    return NOERROR;
}

ECode Call::Details::GetAccountHandle(
    /* [out] */ IPhoneAccountHandle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAccountHandle;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::Details::GetCallCapabilities(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCallCapabilities;
    return NOERROR;
}

ECode Call::Details::GetCallProperties(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCallProperties;
    return NOERROR;
}

ECode Call::Details::GetDisconnectCause(
    /* [out] */ IDisconnectCause** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDisconnectCause;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::Details::GetConnectTimeMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mConnectTimeMillis;
    return NOERROR;
}

ECode Call::Details::GetGatewayInfo(
    /* [out] */ IGatewayInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mGatewayInfo;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::Details::GetVideoState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mVideoState;
    return NOERROR;
}

ECode Call::Details::GetStatusHints(
    /* [out] */ IStatusHints** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStatusHints;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::Details::GetExtras(
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mExtras;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::Details::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (ICallDetails::Probe(o) != NULL) {
        AutoPtr<ICallDetails> d = ICallDetails::Probe(o);
        AutoPtr<IUri> handle;
        d->GetHandle((IUri**)&handle);
        Int32 handlePresentation = 0;
        d->GetHandlePresentation(&handlePresentation);
        String callerDisplayName;
        d->GetCallerDisplayName(&callerDisplayName);
        Int32 callerDisplayNamePresentation = 0;
        d->GetCallerDisplayNamePresentation(&callerDisplayNamePresentation);
        AutoPtr<IPhoneAccountHandle> accountHandle;
        d->GetAccountHandle((IPhoneAccountHandle**)&accountHandle);
        Int32 callCapabilities = 0;
        d->GetCallCapabilities(&callCapabilities);
        Int32 callProperties = 0;
        d->GetCallProperties(&callProperties);
        AutoPtr<IDisconnectCause> disconnectCause;
        d->GetDisconnectCause((IDisconnectCause**)&disconnectCause);
        Int64 connectTimeMillis = 0;
        d->GetConnectTimeMillis(&connectTimeMillis);
        AutoPtr<IGatewayInfo> gatewayInfo;
        d->GetGatewayInfo((IGatewayInfo**)&gatewayInfo);
        Int32 videoState = 0;
        d->GetVideoState(&videoState);
        AutoPtr<IStatusHints> statusHints;
        d->GetStatusHints((IStatusHints**)&statusHints);
        AutoPtr<IBundle> extras;
        d->GetExtras((IBundle**)&extras);
        *result = Object::Equals(mHandle, handle) &&
                (mHandlePresentation == handlePresentation) &&
                mCallerDisplayName.Equals(callerDisplayName) &&
                (mCallerDisplayNamePresentation == callerDisplayNamePresentation) &&
                Object::Equals(mAccountHandle, accountHandle) &&
                (mCallCapabilities == callCapabilities) &&
                (mCallProperties == callProperties) &&
                Object::Equals(mDisconnectCause, disconnectCause) &&
                (mConnectTimeMillis == connectTimeMillis) &&
                Object::Equals(mGatewayInfo, gatewayInfo) &&
                (mVideoState == videoState) &&
                Object::Equals(mStatusHints, statusHints) &&
                Object::Equals(mExtras, extras);
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode Call::Details::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 hc = 0, res = 0;
    IObject::Probe(mHandle)->GetHashCode(&hc);
    res += hc;
    res += mHandlePresentation;
    res += mCallerDisplayName.GetHashCode();
    res += mCallerDisplayNamePresentation;
    hc = 0;
    IObject::Probe(mAccountHandle)->GetHashCode(&hc);
    res += hc;
    res += mCallCapabilities;
    res += mCallProperties;
    hc = 0;
    IObject::Probe(mDisconnectCause)->GetHashCode(&hc);
    res += hc;
    hc = 0;
    IObject::Probe(mDisconnectCause)->GetHashCode(&hc);
    res += hc;
    res += mConnectTimeMillis;
    hc = 0;
    IObject::Probe(mGatewayInfo)->GetHashCode(&hc);
    res += hc;
    res += mVideoState;
    hc = 0;
    IObject::Probe(mStatusHints)->GetHashCode(&hc);
    res += hc;
    hc = 0;
    IObject::Probe(mExtras)->GetHashCode(&hc);
    res += hc;
    *result = res;
    return NOERROR;
}

//===============================================================
// Call::
//===============================================================
CAR_INTERFACE_IMPL(Call, Object, ICall)

Call::Call()
    : mChildrenCached(FALSE)
    , mParentId(String(NULL))
    , mState(0)
{
    CArrayList::New((IList**)&mChildrenIds);
    CArrayList::New((IList**)&mChildren);
    AutoPtr<ICollections> cls;
    CCollections::AcquireSingleton((ICollections**)&cls);
    cls->UnmodifiableList(mChildren, (IList**)&mUnmodifiableChildren);
    CCopyOnWriteArrayList::New((IList**)&mListeners);
    CArrayList::New((IList**)&mConferenceableCalls);
    cls->UnmodifiableList(mConferenceableCalls, (IList**)&mUnmodifiableConferenceableCalls);
}

ECode Call::GetRemainingPostDialSequence(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRemainingPostDialSequence;
    return NOERROR;
}

ECode Call::Answer(
    /* [in] */ Int32 videoState)
{
    return mInCallAdapter->AnswerCall(mTelecomCallId, videoState);
}

ECode Call::Reject(
    /* [in] */ Boolean rejectWithMessage,
    /* [in] */ const String& textMessage)
{
    return mInCallAdapter->RejectCall(mTelecomCallId, rejectWithMessage, textMessage);
}

ECode Call::Disconnect()
{
    return mInCallAdapter->DisconnectCall(mTelecomCallId);
}

ECode Call::Hold()
{
    return mInCallAdapter->HoldCall(mTelecomCallId);
}

ECode Call::Unhold()
{
    return mInCallAdapter->UnholdCall(mTelecomCallId);
}

ECode Call::PlayDtmfTone(
    /* [in] */ Char32 digit)
{
    return mInCallAdapter->PlayDtmfTone(mTelecomCallId, digit);
}

ECode Call::StopDtmfTone()
{
    return mInCallAdapter->StopDtmfTone(mTelecomCallId);
}

ECode Call::PostDialContinue(
    /* [in] */ Boolean proceed)
{
    return mInCallAdapter->PostDialContinue(mTelecomCallId, proceed);
}

ECode Call::PhoneAccountSelected(
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    return mInCallAdapter->PhoneAccountSelected(mTelecomCallId, accountHandle);

}

ECode Call::Conference(
    /* [in] */ ICall* callToConferenceWith)
{
    if (callToConferenceWith != NULL) {
        String callId;
        callToConferenceWith->InternalGetCallId(&callId);
        mInCallAdapter->Conference(mTelecomCallId, callId);
    }
    return NOERROR;
}

ECode Call::SplitFromConference()
{
    return mInCallAdapter->SplitFromConference(mTelecomCallId);
}

ECode Call::MergeConference()
{
    return mInCallAdapter->MergeConference(mTelecomCallId);
}

ECode Call::SwapConference()
{
    return mInCallAdapter->SwapConference(mTelecomCallId);
}

ECode Call::GetParent(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)
    if (mParentId != NULL) {
        return mPhone->InternalGetCallByTelecomId(mParentId, result);
    }
    *result = NULL;
    return NOERROR;
}

ECode Call::GetChildren(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    if (!mChildrenCached) {
        mChildrenCached = TRUE;
        mChildren->Clear();

        AutoPtr<IIterator> it;
        mChildrenIds->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<ICharSequence> pId = ICharSequence::Probe(p);
            String id;
            pId->ToString(&id);
            AutoPtr<ICall> call;
            mPhone->InternalGetCallByTelecomId(id, (ICall**)&call);
            if (call == NULL) {
                // At least one child was still not found, so do not save true for "cached"
                mChildrenCached = FALSE;
            }
            else {
                mChildren->Add(call);
            }
        }
    }
    *result = mUnmodifiableChildren;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::GetConferenceableCalls(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUnmodifiableConferenceableCalls;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::GetState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mState;
    return NOERROR;
}

ECode Call::GetCannedTextResponses(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCannedTextResponses;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::GetVideoCall(
    /* [out] */ IInCallServiceVideoCall** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mVideoCall;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::GetDetails(
    /* [out] */ ICallDetails** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDetails;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Call::AddListener(
    /* [in] */ ICallListener* listener)
{
    return mListeners->Add(listener);
}

ECode Call::RemoveListener(
    /* [in] */ ICallListener* listener)
{
    if (listener != NULL) {
        mListeners->Remove(listener);
    }
    return NOERROR;
}

ECode Call::constructor(
    /* [in] */ IPhone* phone,
    /* [in] */ const String& telecomCallId,
    /* [in] */ IInCallAdapter* inCallAdapter)
{
    mPhone = phone;
    mTelecomCallId = telecomCallId;
    mInCallAdapter = inCallAdapter;
    mState = STATE_NEW;
    return NOERROR;
}

ECode Call::InternalGetCallId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTelecomCallId;
    return NOERROR;
}

ECode Call::InternalUpdate(
    /* [in] */ IParcelableCall* parcelableCall,
    /* [in] */ IMap* callIdMap)
{
    // First, we update the internal state as far as possible before firing any updates.
    AutoPtr<IUri> uri;
    parcelableCall->GetHandle((IUri**)&uri);
    Int32 presentation = 0;
    parcelableCall->GetHandlePresentation(&presentation);
    String disName;
    parcelableCall->GetCallerDisplayName(&disName);
    Int32 disNamePresentation = 0;
    parcelableCall->GetCallerDisplayNamePresentation(&disNamePresentation);
    AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
    parcelableCall->GetAccountHandle((IPhoneAccountHandle**)&phoneAccountHandle);
    Int32 capabilities = 0;
    parcelableCall->GetCapabilities(&capabilities);
    Int32 properties = 0;
    parcelableCall->GetProperties(&properties);
    AutoPtr<IDisconnectCause> disconnectCause;
    parcelableCall->GetDisconnectCause((IDisconnectCause**)&disconnectCause);
    Int64 connectTimeMillis = 0;
    parcelableCall->GetConnectTimeMillis(&connectTimeMillis);
    AutoPtr<IGatewayInfo> gatewayInfo;
    parcelableCall->GetGatewayInfo((IGatewayInfo**)&gatewayInfo);
    Int32 videoState = 0;
    parcelableCall->GetVideoState(&videoState);
    AutoPtr<IStatusHints> statusHints;
    parcelableCall->GetStatusHints((IStatusHints**)&statusHints);
    AutoPtr<IBundle> extras;
    parcelableCall->GetExtras((IBundle**)&extras);
    AutoPtr<ICallDetails> details = new Details(
            uri,
            presentation,
            disName,
            disNamePresentation,
            phoneAccountHandle,
            capabilities,
            properties,
            disconnectCause,
            connectTimeMillis,
            gatewayInfo,
            videoState,
            statusHints,
            extras);
    Boolean detailsChanged = !Object::Equals(mDetails, details);
    if (detailsChanged) {
        mDetails = details;
    }

    Boolean cannedTextResponsesChanged = FALSE;
    if (mCannedTextResponses == NULL) {
        AutoPtr<IList> cannedSmsResponses;
        parcelableCall->GetCannedSmsResponses((IList**)&cannedSmsResponses);
        if (cannedSmsResponses != NULL) {
            Boolean bEmp = FALSE;
            cannedSmsResponses->IsEmpty(&bEmp);
            if (!bEmp) {
                AutoPtr<ICollections> cls;
                CCollections::AcquireSingleton((ICollections**)&cls);
                cls->UnmodifiableList(cannedSmsResponses, (IList**)&mCannedTextResponses);
            }
        }
    }

    AutoPtr<IInCallServiceVideoCall> videoCall;
    parcelableCall->GetVideoCall((IInCallServiceVideoCall**)&videoCall);
    Boolean videoCallChanged = !Object::Equals(mVideoCall, videoCall);
    if (videoCallChanged) {
        parcelableCall->GetVideoCall((IInCallServiceVideoCall**)&mVideoCall);
    }

    Int32 parState = 0;
    parcelableCall->GetState(&parState);
    Int32 state = StateFromParcelableCallState(parState);
    Boolean stateChanged = mState != state;
    if (stateChanged) {
        mState = state;
    }

    String parentId;
    parcelableCall->GetParentCallId(&parentId);
    Boolean parentChanged = !mParentId.Equals(parentId);
    if (parentChanged) {
        mParentId = parentId;
    }

    AutoPtr<IList> childCallIds;
    parcelableCall->GetChildCallIds((IList**)&childCallIds);
    Boolean childrenChanged = !Object::Equals(childCallIds, mChildrenIds);
    if (childrenChanged) {
        mChildrenIds->Clear();
        mChildrenIds->AddAll(ICollection::Probe(childCallIds));
        mChildrenCached = FALSE;
    }

    AutoPtr<IList> conferenceableCallIds;
    parcelableCall->GetConferenceableCallIds((IList**)&conferenceableCallIds);
    Int32 size = 0;
    conferenceableCallIds->GetSize(&size);
    AutoPtr<IList> conferenceableCalls;
    CArrayList::New(size, (IList**)&conferenceableCalls);
    AutoPtr<IIterator> it;
    conferenceableCallIds->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        Boolean bContain = FALSE;
        if ((callIdMap->ContainsKey(p, &bContain), bContain)) {
            AutoPtr<IInterface> val;
            callIdMap->Get(p, (IInterface**)&val);
            conferenceableCalls->Add(val);
        }
    }
    if (!Object::Equals(mConferenceableCalls, conferenceableCalls)) {
        mConferenceableCalls->Clear();
        mConferenceableCalls->AddAll(ICollection::Probe(conferenceableCalls));
        FireConferenceableCallsChanged();
    }

    // Now we fire updates, ensuring that any client who listens to any of these notifications
    // gets the most up-to-date state.

    if (stateChanged) {
        FireStateChanged(mState);
    }
    if (detailsChanged) {
        FireDetailsChanged(mDetails);
    }
    if (cannedTextResponsesChanged) {
        FireCannedTextResponsesLoaded(mCannedTextResponses);
    }
    if (videoCallChanged) {
        FireVideoCallChanged(mVideoCall);
    }
    if (parentChanged) {
        AutoPtr<ICall> cl;
        GetParent((ICall**)&cl);
        FireParentChanged(cl);
    }
    if (childrenChanged) {
        AutoPtr<IList> child;
        GetChildren((IList**)&child);
        FireChildrenChanged(child);
    }

    // If we have transitioned to DISCONNECTED, that means we need to notify clients and
    // remove ourselves from the Phone. Note that we do this after completing all state updates
    // so a client can cleanly transition all their UI to the state appropriate for a
    // DISCONNECTED Call while still relying on the existence of that Call in the Phone's list.
    if (mState == STATE_DISCONNECTED) {
        FireCallDestroyed();
        mPhone->InternalRemoveCall(this);
    }
    return NOERROR;
}

ECode Call::InternalSetPostDialWait(
    /* [in] */ const String& remaining)
{
    mRemainingPostDialSequence = remaining;
    FirePostDialWait(mRemainingPostDialSequence);
    return NOERROR;
}

ECode Call::InternalSetDisconnected()
{
    if (mState != ICall::STATE_DISCONNECTED) {
        mState = ICall::STATE_DISCONNECTED;
        FireStateChanged(mState);
        FireCallDestroyed();
        mPhone->InternalRemoveCall(this);
    }
    return NOERROR;
}

void Call::FireStateChanged(
    /* [in] */ Int32 newState)
{
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<ICallListener> listener = ICallListener::Probe(p);
        listener->OnStateChanged(this, newState);
    }
}

void Call::FireParentChanged(
    /* [in] */ ICall* newParent)
{
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<ICallListener> listener = ICallListener::Probe(p);
        listener->OnParentChanged(this, newParent);
    }
}

void Call::FireChildrenChanged(
    /* [in] */ IList* children)
{
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<ICallListener> listener = ICallListener::Probe(p);
        listener->OnChildrenChanged(this, children);
    }
}

void Call::FireDetailsChanged(
    /* [in] */ ICallDetails* details)
{
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<ICallListener> listener = ICallListener::Probe(p);
        listener->OnDetailsChanged(this, details);
    }
}

void Call::FireCannedTextResponsesLoaded(
    /* [in] */ IList* cannedTextResponses)
{
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<ICallListener> listener = ICallListener::Probe(p);
        listener->OnCannedTextResponsesLoaded(this, cannedTextResponses);
    }
}

void Call::FireVideoCallChanged(
    /* [in] */ IInCallServiceVideoCall* videoCall)
{
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<ICallListener> listener = ICallListener::Probe(p);
        listener->OnVideoCallChanged(this, videoCall);
    }
}

void Call::FirePostDialWait(
    /* [in] */ const String& remainingPostDialSequence)
{
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<ICallListener> listener = ICallListener::Probe(p);
        listener->OnPostDialWait(this, remainingPostDialSequence);
    }
}

void Call::FireCallDestroyed()
{
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<ICallListener> listener = ICallListener::Probe(p);
        listener->OnCallDestroyed(this);
    }
}

void Call::FireConferenceableCallsChanged()
{
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<ICallListener> listener = ICallListener::Probe(p);
        listener->OnConferenceableCallsChanged(this, mUnmodifiableConferenceableCalls);
    }
}

Int32 Call::StateFromParcelableCallState(
    /* [in] */ Int32 parcelableCallState)
{
    switch (parcelableCallState) {
        case ICallState::NEW:
            return STATE_NEW;
        case ICallState::CONNECTING:
            return STATE_CONNECTING;
        case ICallState::PRE_DIAL_WAIT:
            return STATE_PRE_DIAL_WAIT;
        case ICallState::DIALING:
            return STATE_DIALING;
        case ICallState::RINGING:
            return STATE_RINGING;
        case ICallState::ACTIVE:
            return STATE_ACTIVE;
        case ICallState::ON_HOLD:
            return STATE_HOLDING;
        case ICallState::DISCONNECTED:
            return STATE_DISCONNECTED;
        case ICallState::ABORTED:
            return STATE_DISCONNECTED;
        case ICallState::DISCONNECTING:
            return STATE_DISCONNECTING;
        default:
            // Logger::wtf(this, "Unrecognized CallState %s", parcelableCallState);
            return STATE_NEW;
    }
}

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos