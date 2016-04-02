
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/Phone.h"
#include "elastos/droid/telecomm/telecom/CCall.h"
#include "elastos/droid/utility/CArrayMap.h"

using Elastos::Droid::Utility::CArrayMap;
using Elastos::Core::CString;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::Concurrent::CCopyOnWriteArrayList;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

//===============================================================
// Phone::Listener::
//===============================================================
CAR_INTERFACE_IMPL(Phone::Listener, Object, IPhoneListener)

ECode Phone::Listener::OnAudioStateChanged(
    /* [in] */ IPhone* phone,
    /* [in] */ IAudioState* audioState)
{
    return NOERROR;
}

ECode Phone::Listener::OnBringToForeground(
    /* [in] */ IPhone* phone,
    /* [in] */ Boolean showDialpad)
{
    return NOERROR;
}

ECode Phone::Listener::OnCallAdded(
    /* [in] */ IPhone* phone,
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode Phone::Listener::OnCallRemoved(
    /* [in] */ IPhone* phone,
    /* [in] */ ICall* call)
{
    return NOERROR;
}

//===============================================================
// Phone::
//===============================================================
CAR_INTERFACE_IMPL(Phone, Object, IPhone)

Phone::Phone()
{
    CArrayMap::New((IMap**)&mCallByTelecomCallId);

    CCopyOnWriteArrayList::New((IList**)&mCalls);

    AutoPtr<ICollections> cls;
    CCollections::AcquireSingleton((ICollections**)&cls);
    cls->UnmodifiableList(mCalls, (IList**)&mUnmodifiableCalls);

    CCopyOnWriteArrayList::New((IList**)&mListeners);
}

ECode Phone::constructor(
    /* [in] */ IInCallAdapter* adapter)
{
    mInCallAdapter = adapter;
    return NOERROR;
}

ECode Phone::InternalAddCall(
    /* [in] */ IParcelableCall* parcelableCall)
{
    String id;
    parcelableCall->GetId(&id);
    AutoPtr<ICall> call;
    CCall::New(this, id, mInCallAdapter, (ICall**)&call);
    AutoPtr<ICharSequence> pId;
    CString::New(id, (ICharSequence**)&pId);
    mCallByTelecomCallId->Put(pId, call);
    mCalls->Add(call);
    CheckCallTree(parcelableCall);
    call->InternalUpdate(parcelableCall, mCallByTelecomCallId);
    FireCallAdded(call);
    return NOERROR;
 }

ECode Phone::InternalRemoveCall(
    /* [in] */ ICall* call)
{
    String callId;
    call->InternalGetCallId(&callId);
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    mCallByTelecomCallId->Remove(pId);
    mCalls->Remove(call);
    FireCallRemoved(call);
    return NOERROR;
}

ECode Phone::InternalUpdateCall(
    /* [in] */ IParcelableCall* parcelableCall)
{
    String id;
    parcelableCall->GetId(&id);
    AutoPtr<ICharSequence> pId;
    CString::New(id, (ICharSequence**)&pId);
    AutoPtr<IInterface> p;
    mCallByTelecomCallId->Get(pId, (IInterface**)&p);
    AutoPtr<ICall> call = ICall::Probe(p);
    if (call != NULL) {
        CheckCallTree(parcelableCall);
        call->InternalUpdate(parcelableCall, mCallByTelecomCallId);
    }
    return NOERROR;
 }

ECode Phone::InternalSetPostDialWait(
    /* [in] */ const String& telecomId,
    /* [in] */ const String& remaining)
{
    AutoPtr<ICharSequence> pId;
    CString::New(telecomId, (ICharSequence**)&pId);
    AutoPtr<IInterface> p;
    mCallByTelecomCallId->Get(pId, (IInterface**)&p);
    AutoPtr<ICall> call = ICall::Probe(p);
    if (call != NULL) {
        call->InternalSetPostDialWait(remaining);
    }
    return NOERROR;
}

ECode Phone::InternalAudioStateChanged(
    /* [in] */ IAudioState* audioState)
{
    if (!Object::Equals(mAudioState, audioState)) {
        mAudioState = audioState;
        FireAudioStateChanged(audioState);
    }
    return NOERROR;
}

ECode Phone::InternalGetCallByTelecomId(
    /* [in] */ const String& telecomId,
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICharSequence> pId;
    CString::New(telecomId, (ICharSequence**)&pId);
    AutoPtr<IInterface> p;
    mCallByTelecomCallId->Get(pId, (IInterface**)&p);
    *result = ICall::Probe(p);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Phone::InternalBringToForeground(
    /* [in] */ Boolean showDialpad)
{
    FireBringToForeground(showDialpad);
    return NOERROR;
}

ECode Phone::Destroy()
{
    AutoPtr<IIterator> it;
    mCalls->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<ICall> call = ICall::Probe(p);
        Int32 stat = 0;
        call->GetState(&stat);
        if (stat != ICall::STATE_DISCONNECTED) {
            call->InternalSetDisconnected();
        }
    }
    return NOERROR;
}

ECode Phone::AddListener(
    /* [in] */ IPhoneListener* listener)
{
    mListeners->Add(listener);
    return NOERROR;
}

ECode Phone::RemoveListener(
    /* [in] */ IPhoneListener* listener)
{
    if (listener != NULL) {
        mListeners->Remove(listener);
    }
    return NOERROR;
}

ECode Phone::GetCalls(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUnmodifiableCalls;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Phone::SetMuted(
    /* [in] */ Boolean state)
{
    mInCallAdapter->Mute(state);
    return NOERROR;
}

ECode Phone::SetAudioRoute(
    /* [in] */ Int32 route)
{
    mInCallAdapter->SetAudioRoute(route);
    return NOERROR;
}

ECode Phone::SetProximitySensorOn()
{
    mInCallAdapter->TurnProximitySensorOn();
    return NOERROR;
}

ECode Phone::SetProximitySensorOff(
    /* [in] */ Boolean screenOnImmediately)
{
    mInCallAdapter->TurnProximitySensorOff(screenOnImmediately);
    return NOERROR;
}

ECode Phone::GetAudioState(
    /* [out] */ IAudioState** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAudioState;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

void Phone::FireCallAdded(
    /* [in] */ ICall* call)
{
    AutoPtr<IIterator>it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IPhoneListener> listener = IPhoneListener::Probe(p);
        listener->OnCallAdded(this, call);
    }
}

void Phone::FireCallRemoved(
    /* [in] */ ICall* call)
{
    AutoPtr<IIterator>it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IPhoneListener> listener = IPhoneListener::Probe(p);
        listener->OnCallRemoved(this, call);
    }
}

void Phone::FireAudioStateChanged(
    /* [in] */ IAudioState* audioState)
{
    AutoPtr<IIterator>it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IPhoneListener> listener = IPhoneListener::Probe(p);
        listener->OnAudioStateChanged(this, audioState);
    }
}

void Phone::FireBringToForeground(
    /* [in] */ Boolean showDialpad)
{
    AutoPtr<IIterator>it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IPhoneListener> listener = IPhoneListener::Probe(p);
        listener->OnBringToForeground(this, showDialpad);
    }
}

void Phone::CheckCallTree(
    /* [in] */ IParcelableCall* parcelableCall)
{
    String callId;
    parcelableCall->GetParentCallId(&callId);
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    Boolean bContain = FALSE;
    if (!callId.IsNull() &&
            !(mCallByTelecomCallId->ContainsKey(pId, &bContain), bContain)) {
        String id;
        parcelableCall->GetId(&id);
        // Log.wtf(this, "ParcelableCall %s has nonexistent parent %s",
        //         (const char*)id, (const char*)callId);
    }

    AutoPtr<IList> lst;
    parcelableCall->GetChildCallIds((IList**)&lst);
    if (lst != NULL) {
        Int32 s = 0;
        lst->GetSize(&s);
        for (Int32 i = 0; i < s; i++) {
            AutoPtr<IInterface> p;
            lst->Get(i, (IInterface**)&p);
            Boolean bContainK = FALSE;
            if (!(mCallByTelecomCallId->ContainsKey(p, &bContainK), bContainK)) {
                String id;
                parcelableCall->GetId(&id);
                AutoPtr<IInterface> node;
                lst->Get(i, (IInterface**)&node);
                // Log.wtf(this, "ParcelableCall %s has nonexistent child %s",
                //         id, node);
            }
        }
    }
}

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos