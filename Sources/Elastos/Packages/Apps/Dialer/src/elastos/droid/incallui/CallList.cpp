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

#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "elastos/droid/incallui/CallList.h"
#include "elastos/droid/incallui/Call.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::InCallUI::EIID_ICallList;
using Elastos::Droid::InCallUI::EIID_IInCallPhoneListener;
using Elastos::Droid::Telecom::CDisconnectCause;
using Elastos::Utility::IIterator;
using Elastos::Utility::Concurrent::CCopyOnWriteArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

//================================================================
// CallList::PhoneListener
//================================================================

ECode CallList::PhoneListener::OnCallAdded(
    /* [in] */ IPhone* phone,
    /* [in] */ Elastos::Droid::Telecom::ICall* telecommCall)
{
    AutoPtr<Call> call = new Call(telecommCall);
    if (call->GetState() == Call::State::INCOMING) {
        mHost->OnIncoming(call, call->GetCannedSmsResponses());
    }
    else {
        mHost->OnUpdate(call);
    }
    return NOERROR;
}

ECode CallList::PhoneListener::OnCallRemoved(
    /* [in] */ IPhone* phone,
    /* [in] */ Elastos::Droid::Telecom::ICall* telecommCall)
{
    HashMap<AutoPtr<Elastos::Droid::Telecom::ICall>, AutoPtr<Call> >::Iterator it
            = mHost->mCallByTelecommCall.Find(telecommCall);
    if (it != mHost->mCallByTelecommCall.End()) {
        AutoPtr<Call> call = it->mSecond;
        call->SetState(Call::State::DISCONNECTED);
        AutoPtr<IDisconnectCause> cause;
        CDisconnectCause::New(IDisconnectCause::UNKNOWN, (IDisconnectCause**)&cause);
        call->SetDisconnectCause(cause);
        if (mHost->UpdateCallInMap(call)) {
            Logger::W("CallList", "Removing call not previously disconnected %s", call->GetId().string());
        }
        mHost->UpdateCallTextMap(call, NULL);
    }
    return NOERROR;
}


//================================================================
// CallList::MyHandler
//================================================================

ECode CallList::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_DISCONNECTED_TIMEOUT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            Logger::D("CallList", "EVENT_DISCONNECTED_TIMEOUT %s", TO_CSTR(obj));
            mHost->FinishDisconnectedCall((Call*)ICall::Probe(obj));
            break;
        }
        default:
            Logger::D("CallList", "Message not expected: %d", what);
            break;
    }
    return NOERROR;
}


//================================================================
// CallList
//================================================================

const Int32 CallList::DISCONNECTED_CALL_SHORT_TIMEOUT_MS;
const Int32 CallList::DISCONNECTED_CALL_MEDIUM_TIMEOUT_MS;
const Int32 CallList::DISCONNECTED_CALL_LONG_TIMEOUT_MS;
const Int32 CallList::EVENT_DISCONNECTED_TIMEOUT;

AutoPtr<CallList> CallList::sInstance = new CallList();

CallList::CallList()
{
    mPhoneListener = new PhoneListener(this);
    mHandler = new MyHandler(this);
}

CAR_INTERFACE_IMPL_2(CallList, Object, ICallList, IInCallPhoneListener);

AutoPtr<CallList> CallList::GetInstance()
{
    return sInstance;
}

ECode CallList::SetPhone(
    /* [in] */ IPhone* phone)
{
    mPhone = phone;
    mPhone->AddListener(mPhoneListener);
    return NOERROR;
}

ECode CallList::ClearPhone()
{
    mPhone->RemoveListener(mPhoneListener);
    mPhone = NULL;
    return NOERROR;
}

void CallList::OnDisconnect(
    /* [in] */ Call* call)
{
    if (UpdateCallInMap(call)) {
        Logger::I("CallList", "onDisconnect: %s", TO_CSTR(call));
        // notify those listening for changes on this specific change
        NotifyCallUpdateListeners(call);
        // notify those listening for all disconnects
        NotifyListenersOfDisconnect(call);
    }
}

ECode CallList::OnIncoming(
    /* [in] */ Call* call,
    /* [in] */ IList* textMessages)
{
    if (UpdateCallInMap(call)) {
        Logger::I("CallList", "onIncoming - %s", TO_CSTR(call));
    }
    UpdateCallTextMap(call, textMessages);

    Set<AutoPtr<ICallListListener> >::Iterator it = mListeners.Begin();
    for (; it != mListeners.End(); ++it) {
        (*it)->OnIncomingCall((ICall*)call);
    }
    return NOERROR;
}

void CallList::OnUpdate(
    /* [in] */ Call* call)
{
    OnUpdateCall(call);
    NotifyGenericListeners();
}

void CallList::NotifyCallUpdateListeners(
    /* [in] */ Call* call)
{
    HashMap<String, AutoPtr<IList> >::Iterator it
            = mCallUpdateListenerMap.Find(call->GetId());
    AutoPtr<IList> listeners;
    if (it != mCallUpdateListenerMap.End()) {
        listeners = it->mSecond;
    }
    if (listeners != NULL) {
        AutoPtr<IIterator> iter;
        listeners->GetIterator((IIterator**)&iter);
        Boolean hasNext;
        while (iter->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            iter->GetNext((IInterface**)&obj);
            ICallUpdateListener* listener = ICallUpdateListener::Probe(obj);
            listener->OnCallChanged((ICall*)call);
        }
    }
}

void CallList::AddCallUpdateListener(
    /* [in] */ const String& callId,
    /* [in] */ ICallUpdateListener* listener)
{
    HashMap<String, AutoPtr<IList> >::Iterator it
            = mCallUpdateListenerMap.Find(callId);
    AutoPtr<IList> listeners;
    if (it != mCallUpdateListenerMap.End()) {
        listeners = it->mSecond;
    }
    if (listeners == NULL) {
        CCopyOnWriteArrayList::New((IList**)&listeners);
        mCallUpdateListenerMap[callId] = listeners;
    }
    listeners->Add(listener);
}

void CallList::RemoveCallUpdateListener(
    /* [in] */ const String& callId,
    /* [in] */ ICallUpdateListener* listener)
{
    HashMap<String, AutoPtr<IList> >::Iterator it
            = mCallUpdateListenerMap.Find(callId);
    AutoPtr<IList> listeners;
    if (it != mCallUpdateListenerMap.End()) {
        listeners = it->mSecond;
    }
    if (listeners != NULL) {
        listeners->Remove(listener);
    }
}

ECode CallList::AddListener(
    /* [in] */ ICallListListener* listener)
{
    assert(listener != NULL);

    mListeners.Insert(listener);

    // Let the listener know about the active calls immediately.
    listener->OnCallListChange((ICallList*)this);
    return NOERROR;
}

void CallList::RemoveListener(
    /* [in] */ ICallListListener* listener)
{
    if (listener != NULL) {
        mListeners.Erase(listener);
    }
}

AutoPtr<Call> CallList::GetIncomingOrActive()
{
    AutoPtr<Call> retval = GetIncomingCall();
    if (retval == NULL) {
        retval = GetActiveCall();
    }
    return retval;
}

AutoPtr<Call> CallList::GetOutgoingOrActive()
{
    AutoPtr<Call> retval = GetOutgoingCall();
    if (retval == NULL) {
        retval = GetActiveCall();
    }
    return retval;
}

AutoPtr<Call> CallList::GetWaitingForAccountCall()
{
    return GetFirstCallWithState(Call::State::PRE_DIAL_WAIT);
}

AutoPtr<Call> CallList::GetPendingOutgoingCall()
{
    return GetFirstCallWithState(Call::State::CONNECTING);
}

AutoPtr<Call> CallList::GetOutgoingCall()
{
    AutoPtr<Call> call = GetFirstCallWithState(Call::State::DIALING);
    if (call == NULL) {
        call = GetFirstCallWithState(Call::State::REDIALING);
    }
    return call;
}

AutoPtr<Call> CallList::GetActiveCall()
{
    return GetFirstCallWithState(Call::State::ACTIVE);
}

AutoPtr<Call> CallList::GetBackgroundCall()
{
    return GetFirstCallWithState(Call::State::ONHOLD);
}

AutoPtr<Call> CallList::GetDisconnectedCall()
{
    return GetFirstCallWithState(Call::State::DISCONNECTED);
}

AutoPtr<Call> CallList::GetDisconnectingCall()
{
    return GetFirstCallWithState(Call::State::DISCONNECTING);
}

AutoPtr<Call> CallList::GetSecondBackgroundCall()
{
    return GetCallWithState(Call::State::ONHOLD, 1);
}

AutoPtr<Call> CallList::GetActiveOrBackgroundCall()
{
    AutoPtr<Call> call = GetActiveCall();
    if (call == NULL) {
        call = GetBackgroundCall();
    }
    return call;
}

AutoPtr<Call> CallList::GetIncomingCall()
{
    AutoPtr<Call> call = GetFirstCallWithState(Call::State::INCOMING);
    if (call == NULL) {
        call = GetFirstCallWithState(Call::State::CALL_WAITING);
    }

    return call;
}

AutoPtr<Call> CallList::GetFirstCall()
{
    AutoPtr<Call> result = GetIncomingCall();
    if (result == NULL) {
        result = GetPendingOutgoingCall();
    }
    if (result == NULL) {
        result = GetOutgoingCall();
    }
    if (result == NULL) {
        result = GetFirstCallWithState(Call::State::ACTIVE);
    }
    if (result == NULL) {
        result = GetDisconnectingCall();
    }
    if (result == NULL) {
        result = GetDisconnectedCall();
    }
    return result;
}

Boolean CallList::HasLiveCall()
{
    AutoPtr<Call> call = GetFirstCall();
    if (call == NULL) {
        return FALSE;
    }
    return call != GetDisconnectingCall() && call != GetDisconnectedCall();
}

AutoPtr<Call> CallList::GetVideoUpgradeRequestCall()
{
    HashMap<String, AutoPtr<Call> >::Iterator it = mCallById.Begin();
    for (; it != mCallById.End(); ++it) {
        AutoPtr<Call> call = it->mSecond;
        if (call->GetSessionModificationState() ==
                Call::SessionModificationState::RECEIVED_UPGRADE_TO_VIDEO_REQUEST) {
            return call;
        }
    }
    return NULL;
}

AutoPtr<Call> CallList::GetCallById(
    /* [in] */ const String& callId)
{
    AutoPtr<Call> call;
    HashMap<String, AutoPtr<Call> >::Iterator it = mCallById.Find(callId);
    if (it != mCallById.End()) call = it->mSecond;
    return call;
}

AutoPtr<Call> CallList::GetCallByTelecommCall(
    /* [in] */ Elastos::Droid::Telecom::ICall* telecommCall)
{
    AutoPtr<Call> call;
    HashMap<AutoPtr<Elastos::Droid::Telecom::ICall>, AutoPtr<Call> >::Iterator it
            = mCallByTelecommCall.Find(telecommCall);
    if (it != mCallByTelecommCall.End()) call = it->mSecond;
    return call;
}

AutoPtr<IList> CallList::GetTextResponses(
    /* [in] */ const String& callId)
{
    AutoPtr<IList> responses;
    HashMap<String, AutoPtr<IList> >::Iterator it = mCallTextReponsesMap.Find(callId);
    if (it != mCallTextReponsesMap.End()) responses = it->mSecond;
    return responses;
}

AutoPtr<Call> CallList::GetFirstCallWithState(
    /* [in] */ Int32 state)
{
    return GetCallWithState(state, 0);
}

AutoPtr<Call> CallList::GetCallWithState(
    /* [in] */ Int32 state,
    /* [in] */ Int32 positionToFind)
{
    AutoPtr<Call> retval;
    Int32 position = 0;
    HashMap<String, AutoPtr<Call> >::Iterator it = mCallById.Begin();
    for (; it != mCallById.End(); ++it) {
        AutoPtr<Call> call = it->mSecond;
        if (call->GetState() == state) {
            if (position >= positionToFind) {
                retval = call;
                break;
            }
            else {
                position++;
            }
        }
    }

    return retval;
}

void CallList::ClearOnDisconnect()
{
    HashMap<String, AutoPtr<Call> >::Iterator it = mCallById.Begin();
    for (; it != mCallById.End(); ++it) {
        AutoPtr<Call> call = it->mSecond;
        Int32 state = call->GetState();
        if (state != Call::State::IDLE &&
                state != Call::State::INVALID &&
                state != Call::State::DISCONNECTED) {
            call->SetState(Call::State::DISCONNECTED);
            AutoPtr<IDisconnectCause> cause;
            CDisconnectCause::New(IDisconnectCause::UNKNOWN, (IDisconnectCause**)&cause);
            call->SetDisconnectCause(cause);
            UpdateCallInMap(call);
        }
    }
    NotifyGenericListeners();
}

ECode CallList::OnUpdateCall(
    /* [in] */ Call* call)
{
    Logger::D("CallList", "\t%s", TO_CSTR(call));
    if (UpdateCallInMap(call)) {
        Logger::I("CallList", "onUpdate - %s", TO_CSTR(call));
    }
    UpdateCallTextMap(call, call->GetCannedSmsResponses());
    NotifyCallUpdateListeners(call);
    return NOERROR;
}

void CallList::NotifyGenericListeners()
{
    Set<AutoPtr<ICallListListener> >::Iterator it = mListeners.Begin();
    for (; it != mListeners.End(); ++it) {
        (*it)->OnCallListChange((ICallList*)this);
    }
}

void CallList::NotifyListenersOfDisconnect(
    /* [in] */ Call* call)
{
    Set<AutoPtr<ICallListListener> >::Iterator it = mListeners.Begin();
    for (; it != mListeners.End(); ++it) {
        (*it)->OnDisconnect((ICall*)call);
    }
}

Boolean CallList::UpdateCallInMap(
    /* [in] */ Call* call)
{
    assert(call != NULL);

    Boolean updated = FALSE;

    if (call->GetState() == Call::State::DISCONNECTED) {
        // update existing (but do not add!!) disconnected calls
        HashMap<String, AutoPtr<Call> >::Iterator it = mCallById.Find(call->GetId());
        if (it != mCallById.End()) {

            // For disconnected calls, we want to keep them alive for a few seconds so that the
            // UI has a chance to display anything it needs when a call is disconnected.

            // Set up a timer to destroy the call after X seconds.
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(EVENT_DISCONNECTED_TIMEOUT, (ICall*)call, (IMessage**)&msg);
            Boolean result;
            mHandler->SendMessageDelayed(msg, GetDelayForDisconnect(call), &result);

            mCallById[call->GetId()] = call;
            AutoPtr<Elastos::Droid::Telecom::ICall> telecommCall = call->GetTelecommCall();
            mCallByTelecommCall[telecommCall] = call;
            updated = TRUE;
        }
    }
    else if (!IsCallDead(call)) {
        mCallById[call->GetId()] = call;
        AutoPtr<Elastos::Droid::Telecom::ICall> telecommCall = call->GetTelecommCall();
        mCallByTelecommCall[telecommCall] = call;
        updated = TRUE;
    }
    else if (mCallById.Find(call->GetId()) != mCallById.End()) {
        mCallById.Erase(call->GetId());
        AutoPtr<Elastos::Droid::Telecom::ICall> telecommCall = call->GetTelecommCall();
        mCallByTelecommCall.Erase(telecommCall);
        updated = TRUE;
    }

    return updated;
}

Int32 CallList::GetDelayForDisconnect(
    /* [in] */ Call* call)
{
    assert(call->GetState() == Call::State::DISCONNECTED);

    Int32 cause;
    call->GetDisconnectCause()->GetCode(&cause);
    Int32 delay;
    switch (cause) {
        case IDisconnectCause::LOCAL:
            delay = DISCONNECTED_CALL_SHORT_TIMEOUT_MS;
            break;
        case IDisconnectCause::REMOTE:
            delay = DISCONNECTED_CALL_MEDIUM_TIMEOUT_MS;
            break;
        case IDisconnectCause::REJECTED:
        case IDisconnectCause::MISSED:
        case IDisconnectCause::CANCELED:
            // no delay for missed/rejected incoming calls and canceled outgoing calls.
            delay = 0;
            break;
        default:
            delay = DISCONNECTED_CALL_LONG_TIMEOUT_MS;
            break;
    }

    return delay;
}

void CallList::UpdateCallTextMap(
    /* [in] */ Call* call,
    /* [in] */ IList* textResponses)
{
    assert(call != NULL);

    if (!IsCallDead(call)) {
        if (textResponses != NULL) {
            mCallTextReponsesMap[call->GetId()] = textResponses;
        }
    }
    else if (mCallById.Find(call->GetId()) != mCallById.End()) {
        mCallTextReponsesMap.Erase(call->GetId());
    }
}

Boolean CallList::IsCallDead(
    /* [in] */ Call* call)
{
    Int32 state = call->GetState();
    return Call::State::IDLE == state || Call::State::INVALID == state;
}

void CallList::FinishDisconnectedCall(
    /* [in] */ Call* call)
{
    call->SetState(Call::State::IDLE);
    UpdateCallInMap(call);
    NotifyGenericListeners();
}

void CallList::NotifyCallsOfDeviceRotation(
    /* [in] */ Int32 rotation)
{
    HashMap<String, AutoPtr<Call> >::Iterator it = mCallById.Begin();
    for (; it != mCallById.End(); ++it) {
        AutoPtr<Call> call = it->mSecond;
        if (call->GetVideoCall() != NULL) {
            call->GetVideoCall()->SetDeviceOrientation(rotation);
        }
    }
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
