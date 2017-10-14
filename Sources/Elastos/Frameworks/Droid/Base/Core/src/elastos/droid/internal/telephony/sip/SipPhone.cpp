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

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Media.h"
#include "elastos/droid/internal/telephony/sip/SipPhone.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/regex/Pattern.h>

using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Telephony::IDisconnectCause;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Regex::Pattern;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Sip {

//=====================================================================
//                          SipPhone::SipCall
//=====================================================================

const String SipPhone::SipCall::SC_TAG("SipCall");
const Boolean SipPhone::SipCall::SC_DBG = TRUE;
const Boolean SipPhone::SipCall::SC_VDBG = FALSE;

ECode SipPhone::SipCall::Reset()
{
    if (SC_DBG) Log(String("reset"));
    mConnections->Clear();
    SetState(ICallState_IDLE);
    return NOERROR;
}

ECode SipPhone::SipCall::SwitchWith(
    /* [in] */ SipCall* that)
{
    if (SC_DBG) Log(String("switchWith"));
    AutoLock lock(mHost);    // synchronized (SipPhone.class)
    AutoPtr<SipCall> tmp = new SipCall(mHost);
    tmp->TakeOver(this);
    this->TakeOver(that);
    that->TakeOver(tmp);
    return NOERROR;
}

ECode SipPhone::SipCall::GetPhone(
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHost;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SipPhone::SipCall::GetConnections(
    /* [out] */ IList/*<Connection>*/** result)
{
    VALIDATE_NOT_NULL(result);
    if (SC_VDBG) Log(String("getConnections"));
    AutoLock lock(mHost);    // synchronized (SipPhone.class)
    // FIXME should return Collections.unmodifiableList();
    *result = IList::Probe(mConnections);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SipPhone::SipCall::Dial(
    /* [in] */ const String& originalNumber,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    if (SC_DBG) Log(String("dial: num=") + (SC_VDBG ? originalNumber : String("xxx")));
    // TODO: Should this be synchronized?
    String calleeSipUri = originalNumber;
    if (!calleeSipUri.Contains("@")) {
        String name;
// TODO: Need ISipProfile
        // mHost->mProfile->GetUserName(&name);
        String replaceStr = Pattern::Quote(name + "@");
// TODO: Need ISipProfile
        // calleeSipUri = mHost->mProfile->GetUriString().ReplaceFirst(replaceStr,
        //         calleeSipUri + "@");
    }
// TODO: Need ISipProfile, ISipConnection
    // // try {
    // AutoPtr<ISipProfile> callee =
    //         new SipProfile.Builder(calleeSipUri).build();
    // AutoPtr<ISipConnection> c = new SipConnection(this, callee,
    //         originalNumber);
    // c->Dial();
    // mConnections->Add(c);
    // SetState(ICallState_DIALING);
    // *result = c;
    // REFCOUNT_ADD(*result)
    // // } catch (ParseException e) {
    // //     throw new SipException("dial", e);
    // // }
    return NOERROR;
}

ECode SipPhone::SipCall::Hangup()
{
    AutoLock lock(mHost);    // synchronized (SipPhone.class)
// TODO: Need ICallState::IsAlive
    // if (mState->IsAlive()) {
    //     // if (SC_DBG) Log("hangup: call " + getState()
    //     //         + ": " + this + " on phone " + getPhone());
    //     SetState(ICallState_DISCONNECTING);
    //     // CallStateException excp = NULL;
    //     for (Connection c : mConnections) {
    //         // try {
    //         c->Hangup();
    //         // } catch (CallStateException e) {
    //         //     excp = e;
    //         // }
    //     }
    //     if (excp != NULL) {
    //         return E_CALL_STATE_EXCEPTION; // throw excp;
    //     }
    // }
    // else {
    //     // if (SC_DBG) Log("hangup: dead call " + getState()
    //     //         + ": " + this + " on phone " + getPhone());
    // }
    return NOERROR;
}

ECode SipPhone::SipCall::InitIncomingCall(
    /* [in] */ /*TODO ISipAudioCall*/IInterface* sipAudioCall,
    /* [in] */ Boolean makeCallWait,
    /* [out] */ SipConnection** result)
{
    VALIDATE_NOT_NULL(result);
// TODO: Need ISipProfile
    AutoPtr</*ISipProfile*/IInterface> callee; // = sipAudioCall->GetPeerProfile();
    AutoPtr<SipConnection> c = new SipConnection(mHost, this, callee);
    mConnections->Add((IInterface*)(IObject*)c.Get());

    ICallState newState = makeCallWait ? ICallState_WAITING : ICallState_INCOMING;
    c->InitIncomingCall(sipAudioCall, newState);

    SetState(newState);
    mHost->NotifyNewRingingConnectionP(c);
    *result = c;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SipPhone::SipCall::RejectCall()
{
    if (SC_DBG) Log(String("rejectCall:"));
    Hangup();
    return NOERROR;
}

ECode SipPhone::SipCall::AcceptCall()
{
    if (SC_DBG) Log(String("acceptCall: accepting"));
    if (this != mHost->mRingingCall) {
        // throw new CallStateException("acceptCall() in a non-ringing call");
        return E_CALL_STATE_EXCEPTION;
    }
    Int32 size;
    mConnections->GetSize(&size);
    if (size != 1) {
        // throw new CallStateException("acceptCall() in a conf call");
        return E_CALL_STATE_EXCEPTION;
    }
    AutoPtr<IInterface> obj;
    mConnections->Get(0, (IInterface**)&obj);
    ((SipConnection*)(IObject*)obj.Get())->AcceptCall();
    return NOERROR;
}

ECode SipPhone::SipCall::SetAudioGroupMode()
{
// TODO: Need AudioGroup
    // AudioGroup audioGroup = getAudioGroup();
    // if (audioGroup == NULL) {
    //     if (SC_DBG) Log("setAudioGroupMode: audioGroup == NULL ignore");
    //     return;
    // }
    // int mode = audioGroup.getMode();
    // if (mState == State.HOLDING) {
    //     audioGroup.setMode(AudioGroup.MODE_ON_HOLD);
    // } else if (getMute()) {
    //     audioGroup.setMode(AudioGroup.MODE_MUTED);
    // } else if (isSpeakerOn()) {
    //     audioGroup.setMode(AudioGroup.MODE_ECHO_SUPPRESSION);
    // } else {
    //     audioGroup.setMode(AudioGroup.MODE_NORMAL);
    // }
    // if (SC_DBG) log(String.format(
    //         "setAudioGroupMode change: %d --> %d", mode,
    //         audioGroup.getMode()));
    return NOERROR;
}

ECode SipPhone::SipCall::Hold()
{
    if (SC_DBG) Log(String("hold:"));
    SetState(ICallState_HOLDING);

    Int32 size;
    mConnections->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mConnections->Get(i, (IInterface**)&obj);
        ((SipConnection*)(IObject*)obj.Get())->Hold();
    }
    SetAudioGroupMode();
    return NOERROR;
}

ECode SipPhone::SipCall::Unhold()
{
    if (SC_DBG) Log(String("unhold:"));
    SetState(ICallState_ACTIVE);
// TODO: Need AudioGroup
    // AudioGroup audioGroup = new AudioGroup();
    // for (Connection c : mConnections) {
    //     ((SipConnection) c).unhold(audioGroup);
    // }
    SetAudioGroupMode();
    return NOERROR;
}

ECode SipPhone::SipCall::SetMute(
    /* [in] */ Boolean muted)
{
    if (SC_DBG) Log(String("setMute: muted=") + StringUtils::ToString(muted));

    Int32 size;
    mConnections->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mConnections->Get(i, (IInterface**)&obj);
        ((SipConnection*)(IObject*)obj.Get())->SetMute(muted);
    }
    return NOERROR;
}

ECode SipPhone::SipCall::GetMute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean b;
    mConnections->IsEmpty(&b);

    Boolean ret;
    if (b) {
        ret = FALSE;
    }
    else {
        AutoPtr<IInterface> obj;
        mConnections->Get(0, (IInterface**)&obj);
        ((SipConnection*)(IObject*)obj.Get())->GetMute(&ret);
    }

    if (SC_DBG) Log(String("getMute: ret=") + StringUtils::ToString(ret));
    *result = ret;
    return NOERROR;
}

ECode SipPhone::SipCall::Merge(
    /* [in] */ SipCall* that)
{
    if (SC_DBG) Log(String("merge:"));
    AutoPtr</*TODO IAudioGroup*/IInterface> audioGroup = GetAudioGroup();

    // // copy to an array to avoid concurrent modification as connections
    // // in that.connections will be removed in add(SipConnection).
    // Connection[] cc = that.mConnections.toArray(
    //         new Connection[that.mConnections.size()]);
    // for (Connection c : cc) {
    //     SipConnection conn = (SipConnection) c;
    //     add(conn);
    //     if (conn.getState() == ICallState_HOLDING) {
    //         conn.unhold(audioGroup);
    //     }
    // }
    that->SetState(ICallState_IDLE);
    return NOERROR;
}

ECode SipPhone::SipCall::SendDtmf(
    /* [in] */ Char16 c)
{
    if (SC_DBG) Logger::E("SipPhone", "sendDtmf: c='%c'", c);
    AutoPtr</*TODO IAudioGroup*/IInterface> audioGroup = GetAudioGroup();
    if (audioGroup == NULL) {
        if (SC_DBG) Logger::E("SipPhone", "sendDtmf: audioGroup == NULL, ignore c='%c'", c);
        return NOERROR;
    }
    Int32 val;
    ConvertDtmf(c, &val);
// TODO: Need IAudioGroup
    // audioGroup->SendDtmf(val);
    return NOERROR;
}

ECode SipPhone::SipCall::OnConnectionStateChanged(
    /* [in] */ SipConnection* conn)
{
    // this can be called back when a conf call is formed
    // if (SC_DBG) Log("onConnectionStateChanged: conn=" + conn);
    if (mState != ICallState_ACTIVE) {
        ICallState state;
        conn->GetState(&state);
        SetState(state);
    }
    return NOERROR;
}

ECode SipPhone::SipCall::OnConnectionEnded(
    /* [in] */ SipConnection* conn)
{
    // set state to DISCONNECTED only when all conns are disconnected
    // if (SC_DBG) Log("onConnectionEnded: conn=" + conn);
    if (mState != ICallState_DISCONNECTED) {
        Boolean allConnectionsDisconnected = TRUE;
        Int32 size;
        mConnections->GetSize(&size);
        if (SC_DBG) Log(String("---check connections: ") + StringUtils::ToString(size));

        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mConnections->Get(i, (IInterface**)&obj);
            AutoPtr<IConnection> c = IConnection::Probe(obj);
            ICallState state;
            c->GetState(&state);
            if (state != ICallState_DISCONNECTED) {
                allConnectionsDisconnected = FALSE;
                break;
            }
        }

        if (allConnectionsDisconnected) SetState(ICallState_DISCONNECTED);
    }

    mHost->NotifyDisconnectP(conn);
    return NOERROR;
}

void SipPhone::SipCall::SetState(
    /* [in] */ ICallState newState)
{
    if (mState != newState) {
        // if (SC_DBG) Log(String("setState: cur state") + mState
        //         + " --> " + newState + ": " + this + ": on phone "
        //         + getPhone() + " " + mConnections.size());

        if (newState == ICallState_ALERTING) {
            mState = newState; // need in ALERTING to enable ringback
            mHost->StartRingbackTone();
        }
        else if (mState == ICallState_ALERTING) {
            mHost->StopRingbackTone();
        }
        mState = newState;
        mHost->UpdatePhoneState();
        mHost->NotifyPreciseCallStateChanged();
    }
}

void SipPhone::SipCall::TakeOver(
    /* [in] */ SipCall* that)
{
    if (SC_DBG) Log(String("takeOver"));
    mConnections = that->mConnections;
    mState = that->mState;

    Int32 size;
    mConnections->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mConnections->Get(i, (IInterface**)&obj);
        AutoPtr<IConnection> c = IConnection::Probe(obj);
        ((SipConnection*)c.Get())->ChangeOwner(this);
    }
}

Boolean SipPhone::SipCall::IsSpeakerOn()
{
    AutoPtr<IInterface> obj;
    mHost->mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    AutoPtr<IAudioManager> am =  IAudioManager::Probe(obj);
    Boolean ret;
    am->IsSpeakerphoneOn(&ret);
    if (SC_VDBG) Log(String("isSpeakerOn: ret=") + StringUtils::BooleanToString(ret));
    return ret;
}

void SipPhone::SipCall::Add(
    /* [in] */ SipConnection* conn)
{
    if (SC_DBG) Log(String("add:"));
    AutoPtr<ICall> c;
    conn->GetCall((ICall**)&c);
    AutoPtr<SipCall> call = (SipCall*)c.Get();
    if (call.Get() == this) return;
    if (call != NULL) call->mConnections->Remove((IInterface*)(IObject*)conn);

    mConnections->Add((IInterface*)(IObject*)conn);
    conn->ChangeOwner(this);
}

ECode SipPhone::SipCall::ConvertDtmf(
    /* [in] */ Char16 c,
    /* [out] */ Int32* result)
{
    *result = 0;

    Int32 code = c - '0';
    if ((code < 0) || (code > 9)) {
        switch (c) {
            case '*':
                *result = 10;
                return NOERROR;
            case '#':
                *result = 11;
                return NOERROR;
            case 'A':
                *result = 12;
                return NOERROR;
            case 'B':
                *result = 13;
                return NOERROR;
            case 'C':
                *result = 14;
                return NOERROR;
            case 'D':
                *result = 15;
                return NOERROR;
            default:
                // throw new IllegalArgumentException(
                //         "invalid DTMF char: " + (int) c);
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    *result = code;
    return NOERROR;
}

AutoPtr</*TODO IAudioGroup*/IInterface> SipPhone::SipCall::GetAudioGroup()
{
    Boolean b;
    if (mConnections->IsEmpty(&b), b) return NULL;
    AutoPtr<IInterface> obj;
    mConnections->Get(0, (IInterface**)&obj);
    AutoPtr<SipConnection> conn = (SipConnection*)(IObject*)obj.Get();
    AutoPtr</*TODO IAudioGroup*/IInterface> ag;
    conn->GetAudioGroup((/*TODO IAudioGroup*/IInterface**)&ag);
    return ag;
}

void SipPhone::SipCall::Log(
    /* [in] */ const String& s)
{
    Logger::D(SC_TAG, s);
}

//=====================================================================
//                       SipPhone::SipConnection
//=====================================================================
const String SipPhone::SipConnection::SCN_TAG("SipConnection");
const Boolean SipPhone::SipConnection::SCN_DBG = TRUE;

SipPhone::SipConnection::SipConnection(
    /* [in] */ SipPhone* host,
    /* [in] */ SipCall* owner,
    /* [in] */ /*TODO ISipProfile*/IInterface* callee,
    /* [in] */ const String& originalNumber)
    : mOwner(owner)
    , mPeer(callee)
    , mOriginalNumber(originalNumber)
    , mHost(host)
{
    SipConnectionBase::constructor(originalNumber);
}

SipPhone::SipConnection::SipConnection(
    /* [in] */ SipPhone* host,
    /* [in] */ SipCall* owner,
    /* [in] */ /*TODO ISipProfile*/IInterface* callee)
    : mOwner(owner)
    , mPeer(callee)
    , mOriginalNumber(host->GetUriString(callee))
    , mHost(host)
{
    SipConnectionBase::constructor(mOriginalNumber);
}

ECode SipPhone::SipConnection::GetCnapName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String displayName;
// TODO: Need ISipProfile
    // mPeer->GetDisplayName(&displayName);
    *result = displayName.IsEmpty() ? String(NULL) : displayName;
    return NOERROR;
}

ECode SipPhone::SipConnection::GetNumberPresentation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IPhoneConstants::PRESENTATION_ALLOWED;
    return NOERROR;
}

ECode SipPhone::SipConnection::InitIncomingCall(
    /* [in] */ /*TODO ISipAudioCall*/IInterface* sipAudioCall,
    /* [in] */ ICallState newState)
{
    SetState(newState);
    mSipAudioCall = sipAudioCall;
// TODO: Need ISipAudioCall
    // sipAudioCall->SetListener(mAdapter); // call back to set state
    mIncoming = TRUE;
    return NOERROR;
}

ECode SipPhone::SipConnection::AcceptCall()
{
    // try {
// TODO: Need ISipAudioCall
    // mSipAudioCall->AnswerCall(TIMEOUT_ANSWER_CALL);
    // } catch (SipException e) {
    //     throw new CallStateException("acceptCall(): " + e);
    // }
    return NOERROR;
}

ECode SipPhone::SipConnection::ChangeOwner(
    /* [in] */ SipCall* owner)
{
    mOwner = owner;
    return NOERROR;
}

ECode SipPhone::SipConnection::GetAudioGroup(
    /* [out] */ /*TODO IAudioGroup*/IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    if (mSipAudioCall == NULL) {
        *result = NULL;
        return NOERROR;
    }
// TODO: Need ISipAudioCall
    // return mSipAudioCall->GetAudioGroup(result);
    return NOERROR;
}

ECode SipPhone::SipConnection::Dial()
{
    SetState(ICallState_DIALING);
// TODO: Need ISipManager, ISipAudioCall
    // mSipAudioCall = mSipManager->MakeAudioCall(mProfile, mPeer, NULL,
    //         TIMEOUT_MAKE_CALL);
    // mSipAudioCall->SetListener(mAdapter);
    return NOERROR;
}

ECode SipPhone::SipConnection::Hold()
{
    SetState(ICallState_HOLDING);
    // try {
// TODO: Need ISipAudioCall
    // mSipAudioCall->HoldCall(TIMEOUT_HOLD_CALL);
    // } catch (SipException e) {
    //     throw new CallStateException("hold(): " + e);
    // }
    return NOERROR;
}

ECode SipPhone::SipConnection::Unhold(
    /* [in] */ /*TODO IAudioGroup*/IInterface* audioGroup)
{
// TODO: Need ISipAudioCall
    // mSipAudioCall->SetAudioGroup(audioGroup);
    SetState(ICallState_ACTIVE);
    // try {
// TODO: Need ISipAudioCall
    // mSipAudioCall->ContinueCall(TIMEOUT_HOLD_CALL);
    // } catch (SipException e) {
    //     throw new CallStateException("unhold(): " + e);
    // }
    return NOERROR;
}

ECode SipPhone::SipConnection::SetMute(
    /* [in] */ Boolean muted)
{
// TODO: Need ISipAudioCall
    // if ((mSipAudioCall != NULL) && (muted != mSipAudioCall.isMuted())) {
    //     if (SCN_DBG) Log("setState: prev muted=" + !muted + " new muted=" + muted);
    //     mSipAudioCall.toggleMute();
    // }
    return NOERROR;
}

ECode SipPhone::SipConnection::GetMute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
// TODO: Need ISipAudioCall
    // return (mSipAudioCall == NULL) ? FALSE
    //                                : mSipAudioCall.isMuted();
    return NOERROR;
}

ECode SipPhone::SipConnection::GetState(
    /* [out] */ ICallState* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mState;
    return NOERROR;
}

ECode SipPhone::SipConnection::IsIncoming(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIncoming;
    return NOERROR;
}

ECode SipPhone::SipConnection::GetAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // Phone app uses this to query caller ID. Return the original dial
    // number (which may be a PSTN number) instead of the peer's SIP
    // URI.
    *result = mOriginalNumber;
    return NOERROR;
}

ECode SipPhone::SipConnection::GetCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    *result = ICall::Probe(mOwner);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SipPhone::SipConnection::Hangup()
{
    AutoLock lock(mHost);    // synchronized (SipPhone.class)
    // if (SCN_DBG) Log("hangup: conn=" + mPeer.getUriString()
    //         + ": " + mState + ": on phone "
    //         + getPhone().getPhoneName());
// TODO; Need ICallState::IsAlive
    // if (!mState->IsAlive()) return;
    // try {
// TODO: Need ISipAudioCall
    // AutoPtr<ISipAudioCall> sipAudioCall = mSipAudioCall;
    // if (sipAudioCall != NULL) {
    //     sipAudioCall.setListener(NULL);
    //     sipAudioCall.endCall();
    // }
    // } catch (SipException e) {
    //     throw new CallStateException("hangup(): " + e);
    // } finally {
    mAdapter->OnCallEnded(((mState == ICallState_INCOMING)
            || (mState == ICallState_WAITING))
            ? IDisconnectCause::INCOMING_REJECTED
            : IDisconnectCause::LOCAL);
    // }
    return NOERROR;
}

ECode SipPhone::SipConnection::Separate()
{
    AutoLock lock(mHost);    // synchronized (SipPhone.class)
    AutoPtr<IPhone> originalPhone = GetPhone();
    AutoPtr<SipCall> call;
    if (originalPhone == mHost) {
        AutoPtr<ICall> c;
        mHost->GetBackgroundCall((ICall**)&c);
        call = (SipCall*)c.Get();
    }
    else {
        AutoPtr<ICall> c;
        mHost->GetForegroundCall((ICall**)&c);
        call = (SipCall*)c.Get();
    }
    ICallState state;
    call->GetState(&state);
    if (state != ICallState_IDLE) {
        // throw new CallStateException(
        //         "cannot put conn back to a call in non-idle state: "
        //         + call.getState());
        return E_CALL_STATE_EXCEPTION;
    }
    // if (SCN_DBG) Log("separate: conn="
    //         + mPeer.getUriString() + " from " + mOwner + " back to "
    //         + call);

    // separate the AudioGroup and connection from the original call
// TODO: Need IAudioGroup
    // AutoPtr<IAudioGroup> audioGroup = call->GetAudioGroup(); // may be NULL
    // call->Add(this);
// TODO: Need ISipAudioCall
    // mSipAudioCall->SetAudioGroup(audioGroup);

    // put the original call to bg; and the separated call becomes
    // fg if it was in bg
    originalPhone->SwitchHoldingAndActive();

    // start audio and notify the phone app of the state change
    AutoPtr<ICall> c;
    mHost->GetForegroundCall((ICall**)&c);
    call = (SipCall*)c.Get();

// TODO: Need ISipAudioCall
    // mSipAudioCall->StartAudio();
    call->OnConnectionStateChanged(this);

    return NOERROR;
}

void SipPhone::SipConnection::SetState(
    /* [in] */ ICallState state)
{
    if (state == mState) return;
    SipConnectionBase::SetState(state);
    mState = state;
}

AutoPtr<IPhone> SipPhone::SipConnection::GetPhone()
{
    AutoPtr<IPhone> phone;
    mOwner->GetPhone((IPhone**)&phone);
    return phone;
}

void SipPhone::SipConnection::Log(
    /* [in] */ const String& s)
{
    Logger::D(SCN_TAG, s);
}

//=====================================================================
//                  SipPhone::InnerSipAudioCallAdapter
//=====================================================================
SipPhone::InnerSipAudioCallAdapter::InnerSipAudioCallAdapter(
    /* [in] */ SipConnection* owner)
    : mOwner(owner)
{
}

ECode SipPhone::InnerSipAudioCallAdapter::OnCallEstablished(
    /* [in] */ /*TODO ISipAudioCall*/IInterface* call)
{
    OnChanged(call);
    // Race onChanged synchronized this isn't
    if (mOwner->mState == ICallState_ACTIVE) {
// TODO: Need ISipAudioCall
        // call->StartAudio();
    }
    return NOERROR;
}

ECode SipPhone::InnerSipAudioCallAdapter::OnCallHeld(
    /* [in] */ /*TODO ISipAudioCall*/IInterface* call)
{
    OnChanged(call);
    // Race onChanged synchronized this isn't
    if (mOwner->mState == ICallState_HOLDING) {
// TODO: Need ISipAudioCall
        // call->StartAudio();
    }
    return NOERROR;
}

ECode SipPhone::InnerSipAudioCallAdapter::OnChanged(
    /* [in] */ /*TODO ISipAudioCall*/IInterface* call)
{
    AutoLock lock(mOwner->mHost);    // synchronized (SipPhone.class)
    ICallState newState = GetCallStateFrom(call);
    if (mOwner->mState == newState) {
        return NOERROR;
    }
    if (newState == ICallState_INCOMING) {
        ICallState state;
        mOwner->GetState(&state);
        mOwner->SetState(state); // INCOMING or WAITING
    }
    else {
        if (mOwner->mOwner == mOwner->mHost->mRingingCall) {
            ICallState state;
            mOwner->mHost->mRingingCall->GetState(&state);
            if (state == ICallState_WAITING) {
                // try {
                mOwner->mHost->SwitchHoldingAndActive();
                // } catch (CallStateException e) {
                //     // disconnect the call.
                //     onCallEnded(DisconnectCause.LOCAL);
                //     return;
                // }
            }
            mOwner->mHost->mForegroundCall->SwitchWith(mOwner->mHost->mRingingCall);
        }
        mOwner->SetState(newState);
    }
    mOwner->mOwner->OnConnectionStateChanged(/*SipConnection.this*/mOwner);
    // if (SCN_DBG) Log("onChanged: "
    //         + mPeer.getUriString() + ": " + mState
    //         + " on phone " + getPhone());

    return NOERROR;
}

ECode SipPhone::InnerSipAudioCallAdapter::OnCallEnded(
    /* [in] */ Int32 cause)
{
    Int32 val;
    mOwner->GetDisconnectCause(&val);
    if (val != IDisconnectCause::LOCAL) {
        mOwner->SetDisconnectCause(cause);
    }

    AutoLock lock(mOwner->mHost);    // synchronized (SipPhone.class)
    mOwner->SetState(ICallState_DISCONNECTED);
    AutoPtr</*TODO ISipAudioCall*/IInterface> sipAudioCall = mOwner->mSipAudioCall;
    // FIXME: This goes NULL and is synchronized, but many uses aren't sync'd
    mOwner->mSipAudioCall = NULL;
// TODO: Need ISipAudioCall
    // String sessionState = (sipAudioCall == NULL)
    //         ? ""
    //         : (sipAudioCall->GetState() + ", ");
    // // if (SCN_DBG) Log("[SipAudioCallAdapter] onCallEnded: "
    // //         + mPeer.getUriString() + ": " + sessionState
    // //         + "cause: " + getDisconnectCause() + ", on phone "
    // //         + getPhone());
    // if (sipAudioCall != NULL) {
    //     sipAudioCall->SetListener(NULL);
    //     sipAudioCall->Close();
    // }
    mOwner->mOwner->OnConnectionEnded(/*SipConnection.this*/mOwner);

    return NOERROR;
}

ECode SipPhone::InnerSipAudioCallAdapter::OnError(
    /* [in] */ Int32 cause)
{
    // if (SCN_DBG) Log(String("onError: ") + StringUtils::ToString(cause));
    OnCallEnded(cause);
    return NOERROR;
}

//=====================================================================
//                    SipPhone::SipAudioCallAdapter
//=====================================================================
const String SipPhone::SipAudioCallAdapter::SACA_TAG("SipAudioCallAdapter");
const Boolean SipPhone::SipAudioCallAdapter::SACA_DBG = TRUE;

ECode SipPhone::SipAudioCallAdapter::OnCallEnded(
    /* [in] */ /*TODO ISipAudioCall*/IInterface* call)
{
    // if (SACA_DBG) Log("onCallEnded: call=" + call);
    Boolean b = FALSE;
// TODO: Need ISipAudioCall
    // call->IsInCall(&b);
    OnCallEnded(b
            ? IDisconnectCause::NORMAL
            : IDisconnectCause::INCOMING_MISSED);
    return NOERROR;
}

ECode SipPhone::SipAudioCallAdapter::OnCallBusy(
    /* [in] */ /*TODO ISipAudioCall*/IInterface* call)
{
    // if (SACA_DBG) Log(String("onCallBusy: call=") + call);
    OnCallEnded(IDisconnectCause::BUSY);
    return NOERROR;
}

ECode SipPhone::SipAudioCallAdapter::OnError(
    /* [in] */ /*TODO ISipAudioCall*/IInterface* call,
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& errorMessage)
{
    if (SACA_DBG) {
        // Log("onError: call=" + call + " code="+ SipErrorCode.toString(errorCode)
        //     + ": " + errorMessage);
    }
    switch (errorCode) {
// TODO: Need ISipErrorCode
        // case ISipErrorCode::SERVER_UNREACHABLE:
        //     OnError(IDisconnectCause::SERVER_UNREACHABLE);
        //     break;
        // case ISipErrorCode::PEER_NOT_REACHABLE:
        //     OnError(IDisconnectCause::NUMBER_UNREACHABLE);
        //     break;
        // case ISipErrorCode::INVALID_REMOTE_URI:
        //     OnError(IDisconnectCause::INVALID_NUMBER);
        //     break;
        // case ISipErrorCode::TIME_OUT:
        // case ISipErrorCode::TRANSACTION_TERMINTED:
        //     OnError(IDisconnectCause::TIMED_OUT);
        //     break;
        // case ISipErrorCode::DATA_CONNECTION_LOST:
        //     OnError(IDisconnectCause::LOST_SIGNAL);
        //     break;
        // case ISipErrorCode::INVALID_CREDENTIALS:
        //     OnError(IDisconnectCause::INVALID_CREDENTIALS);
        //     break;
        // case ISipErrorCode::CROSS_DOMAIN_AUTHENTICATION:
        //     OnError(IDisconnectCause::OUT_OF_NETWORK);
        //     break;
        // case ISipErrorCode::SERVER_ERROR:
        //     OnError(IDisconnectCause::SERVER_ERROR);
        //     break;
        // case ISipErrorCode::SOCKET_ERROR:
        // case ISipErrorCode::CLIENT_ERROR:
        default:
            OnError(IDisconnectCause::ERROR_UNSPECIFIED);
    }
    return NOERROR;
}

void SipPhone::SipAudioCallAdapter::Log(
    /* [in] */ const String& s)
{
    Logger::D(SACA_TAG, s);
}

//=====================================================================
//                               SipPhone
//=====================================================================
CAR_INTERFACE_IMPL(SipPhone, SipPhoneBase, ISipPhone);

const String SipPhone::LOGTAG("SipPhone");
const Boolean SipPhone::DBG = TRUE;
const Boolean SipPhone::VDBG = FALSE;
const Int32 SipPhone::TIMEOUT_MAKE_CALL;
const Int32 SipPhone::TIMEOUT_ANSWER_CALL;
const Int32 SipPhone::TIMEOUT_HOLD_CALL;

SipPhone::SipPhone()
{
}

ECode SipPhone::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneNotifier* notifier,
    /* [in] */ /*TODO ISipProfile*/IInterface* profile)
{
// TODO: Need ISipProfile
    // SipPhoneBase::constructor(String("SIP:") + profile->GetUriString(), context, notifier);
    // if (DBG) Log(("new SipPhone: ") + profile->GetUriString());
    mRingingCall = new SipCall(this);
    mForegroundCall = new SipCall(this);
    mBackgroundCall = new SipCall(this);
    mProfile = profile;
// TODO: Need SipManager
    // mSipManager = SipManager::NewInstance(context);
    return NOERROR;
}

ECode SipPhone::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (o == TO_IINTERFACE(this)) {
        *result = TRUE;
        return NOERROR;
    }
    if (ISipPhone::Probe(o) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<SipPhone> that = (SipPhone*)(IObject*)o;
// TODO: Need ISipProfile
    // *result = mProfile.getUriString().Equals(that.mProfile.getUriString());
    return NOERROR;
}

ECode SipPhone::GetSipUri(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
// TODO: Need ISipProfile
    // return mProfile.getUriString();
    return NOERROR;
}

ECode SipPhone::Equals(
    /* [in] */ ISipPhone* phone,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    String str1;
    GetSipUri(&str1);
    String str2;
    phone->GetSipUri(&str2);
    *result = str1.Equals(str2);
    return NOERROR;
}

ECode SipPhone::TakeIncomingCall(
    /* [in] */ IInterface* incomingCall,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    // FIXME: Is synchronizing on the class necessary, should we use a mLockObj?
    // Also there are many things not synchronized, of course
    // this may be TRUE of CdmaPhone and GsmPhone too!!!
    AutoLock lock(this); // synchronized (SipPhone.class)

// TODO: Need ISipAudioCall
    // if (ISipAudioCall::Probe(incomingCall) == NULL) {
    //     if (DBG) Log(String("takeIncomingCall: ret=NULL, not a SipAudioCall"));
    //     *result = NULL;
    //     return NOERROR;
    // }
// TODO: Need ICallState::IsAlive
    // if (mRingingCall.getState().isAlive()) {
    //     if (DBG) Log("takeIncomingCall: ret=NULL, ringingCall not alive");
    //     return NULL;
    // }

    // FIXME: is it TRUE that we cannot take any incoming call if
    // both foreground and background are active
// TODO: Need ICallState::IsAlive
    // if (mForegroundCall.getState().isAlive()
    //         && mBackgroundCall.getState().isAlive()) {
    //     if (DBG) {
    //         Log("takeIncomingCall: ret=NULL," + " foreground and background both alive");
    //     }
    //     return NULL;
    // }

    // try {
// TODO: Need ISipAudioCall
    // SipAudioCall sipAudioCall = (SipAudioCall) incomingCall;
    // if (DBG) Log("takeIncomingCall: taking call from: "
    //         + sipAudioCall.getPeerProfile().getUriString());
    // String localUri = sipAudioCall.getLocalProfile().getUriString();
    // if (localUri.equals(mProfile.getUriString())) {
    //     boolean makeCallWait = mForegroundCall.getState().isAlive();
    //     SipConnection connection = mRingingCall.initIncomingCall(sipAudioCall,
    //             makeCallWait);
    //     if (sipAudioCall.getState() != SipSession.State.INCOMING_CALL) {
    //         // Peer cancelled the call!
    //         if (DBG) Log("    takeIncomingCall: call cancelled !!");
    //         mRingingCall.reset();
    //         connection = NULL;
    //     }
    //     return connection;
    // }
    // } catch (Exception e) {
    //     // Peer may cancel the call at any time during the time we hook
    //     // up ringingCall with sipAudioCall. Clean up ringingCall when
    //     // that happens.
    //     if (DBG) Log("    takeIncomingCall: exception e=" + e);
    //     mRingingCall.reset();
    // }
    if (DBG) Log(String("takeIncomingCall: NOT taking !!"));
    *result = NULL;
    return NOERROR;
}

ECode SipPhone::AcceptCall(
    /* [in] */ Int32 videoState)
{
    AutoLock lock(this);    // synchronized (SipPhone.class)
    ICallState state;
    mRingingCall->GetState(&state);
    if ((state == ICallState_INCOMING) ||
            (state == ICallState_WAITING)) {
        if (DBG) Log(String("acceptCall: accepting"));
        // Always unmute when answering a new call
        mRingingCall->SetMute(FALSE);
        mRingingCall->AcceptCall();
    }
    else {
        if (DBG) {
            Log(String("acceptCall: throw CallStateException(\"phone not ringing\")"));
        }
        // throw new CallStateException("phone not ringing");
        return E_CALL_STATE_EXCEPTION;
    }

    return NOERROR;
}

ECode SipPhone::RejectCall()
{
    AutoLock lock(this);    // synchronized (SipPhone.class)
    ICallState state;
    mRingingCall->GetState(&state);
    Boolean b = FALSE;
// TODO: Need ICallState::IsRinging
    // state->IsRinging(&b);
    if (b) {
        if (DBG) Log(String("rejectCall: rejecting"));
        mRingingCall->RejectCall();
    }
    else {
        if (DBG) {
            Log(String("rejectCall: throw CallStateException(\"phone not ringing\")"));
        }
        // throw new CallStateException("phone not ringing");
        return E_CALL_STATE_EXCEPTION;
    }

    return NOERROR;
}

ECode SipPhone::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(this);    // synchronized (SipPhone.class)
    return DialInternal(dialString, videoState, result);

    return NOERROR;
}

ECode SipPhone::SwitchHoldingAndActive()
{
    if (DBG) Log(String("dialInternal: switch fg and bg"));
    AutoLock lock(this);    // synchronized (SipPhone.class)
    mForegroundCall->SwitchWith(mBackgroundCall);
    ICallState state;
    mBackgroundCall->GetState(&state);
// TODO: Need ICallState::IsAlive
    // if (state->IsAlive()) mBackgroundCall->Hold();

    mForegroundCall->GetState(&state);
// TODO: Need ICallState::IsAlive
    // if (state->IsAlive()) mForegroundCall->Unhold();

    return NOERROR;
}

ECode SipPhone::CanConference(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Log(String("canConference: ret=TRUE"));
    *result = TRUE;
    return NOERROR;
}

ECode SipPhone::Conference()
{
    AutoLock lock(this);    // synchronized (SipPhone.class)
    ICallState state1, state2;
    mForegroundCall->GetState(&state1);
    mBackgroundCall->GetState(&state2);
    if ((state1 != ICallState_ACTIVE)
            || (state2 != ICallState_ACTIVE)) {
        // throw new CallStateException("wrong state to merge calls: fg="
        //         + mForegroundCall.getState() + ", bg="
        //         + mBackgroundCall.getState());
        return E_CALL_STATE_EXCEPTION;
    }
    if (DBG) Log(String("conference: merge fg & bg"));
    mForegroundCall->Merge(mBackgroundCall);

    return NOERROR;
}

ECode SipPhone::Conference(
    /* [in] */ ICall* that)
{
    AutoLock lock(this);    // synchronized (SipPhone.class)
    if ((SipCall*)that == NULL) {
        // throw new CallStateException("expect " + SipCall.class
        //         + ", cannot merge with " + that.getClass());
        return E_CALL_STATE_EXCEPTION;
    }
    mForegroundCall->Merge((SipCall*)that);

    return NOERROR;
}

ECode SipPhone::CanTransfer(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode SipPhone::ExplicitCallTransfer()
{
    //mCT.explicitCallTransfer();
    return NOERROR;
}

ECode SipPhone::ClearDisconnected()
{
    AutoLock lock(this);    // synchronized (SipPhone.class)
    mRingingCall->ClearDisconnected();
    mForegroundCall->ClearDisconnected();
    mBackgroundCall->ClearDisconnected();

    UpdatePhoneState();
    NotifyPreciseCallStateChanged();

    return NOERROR;
}

ECode SipPhone::SendDtmf(
    /* [in] */ Char32 c)
{
    ICallState state;
    mBackgroundCall->GetState(&state);

    Boolean b;
    if (PhoneNumberUtils::Is12Key(c, &b), !b) {
        Logger::E("SipPhone", "startDtmf called with invalid character '%c'", c);
    }
// TODO: Need ICallState::IsAlive
    // else if (state->IsAlive()) {
    //     AutoLock lock(this);    // synchronized (SipPhone.class)
    //     mForegroundCall->SendDtmf(c);
    // }
    return NOERROR;
}

ECode SipPhone::StartDtmf(
    /* [in] */ Char32 c)
{
    Boolean b;
    if (PhoneNumberUtils::Is12Key(c, &b), !b) {
        Logger::E("SipPhone", "startDtmf called with invalid character '%c'", c);
    }
    else {
        SendDtmf(c);
    }
    return NOERROR;
}

ECode SipPhone::StopDtmf()
{
    // no op
    return NOERROR;
}

ECode SipPhone::SendBurstDtmf(
    /* [in] */ const String& dtmfString)
{
    Loge(String("sendBurstDtmf() is a CDMA method"));
    return NOERROR;
}

ECode SipPhone::GetOutgoingCallerIdDisplay(
    /* [in] */ IMessage* onComplete)
{
    // FIXME: what to reply?
    AsyncResult::ForMessage(onComplete, NULL, NULL);
    onComplete->SendToTarget();
    return NOERROR;
}

ECode SipPhone::SetOutgoingCallerIdDisplay(
    /* [in] */ Int32 commandInterfaceCLIRMode,
    /* [in] */ IMessage* onComplete)
{
    // FIXME: what's this for SIP?
    AsyncResult::ForMessage(onComplete, NULL, NULL);
    onComplete->SendToTarget();
    return NOERROR;
}

ECode SipPhone::GetCallWaiting(
    /* [in] */ IMessage* onComplete)
{
    // FIXME: what to reply?
    AsyncResult::ForMessage(onComplete, NULL, NULL);
    onComplete->SendToTarget();
    return NOERROR;
}

ECode SipPhone::SetCallWaiting(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* onComplete)
{
    // FIXME: what to reply?
    Loge(String("call waiting not supported"));
    return NOERROR;
}

ECode SipPhone::SetEchoSuppressionEnabled()
{
    // Echo suppression may not be available on every device. So, check
    // whether it is supported
    AutoLock lock(this);    // synchronized (SipPhone.class)
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    AutoPtr<IAudioManager> audioManager = IAudioManager::Probe(obj);
    String echoSuppression;
    audioManager->GetParameters(String("ec_supported"), &echoSuppression);
    if (echoSuppression.Contains("off")) {
        mForegroundCall->SetAudioGroupMode();
    }

    return NOERROR;
}

ECode SipPhone::SetMute(
    /* [in] */ Boolean muted)
{
    AutoLock lock(this);    // synchronized (SipPhone.class)
    mForegroundCall->SetMute(muted);

    return NOERROR;
}

ECode SipPhone::GetMute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    ICallState state;
    mForegroundCall->GetState(&state);

// TODO: Need ICallState::IsAlive
    // return (state->IsAlive()
    //         ? mForegroundCall->GetMute()
    //         : mBackgroundCall->GetMute());
    return NOERROR;
}

ECode SipPhone::GetForegroundCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mForegroundCall;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SipPhone::GetBackgroundCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBackgroundCall;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SipPhone::GetRingingCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRingingCall;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SipPhone::GetServiceState(
    /* [out] */ IServiceState** result)
{
    VALIDATE_NOT_NULL(result);
    // FIXME: we may need to provide this when data connectivity is lost
    // or when server is down
    return SipPhoneBase::GetServiceState(result);
}

ECode SipPhone::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [in] */ IBundle* extras,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    // NOTE: Add implementation if call extras are needed to be
    //       passed through SIP phone.
    *result = NULL;
    return NOERROR;
}

ECode SipPhone::DialInternal(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result)

    if (DBG) Log(String("dialInternal: dialString=") + (VDBG ? dialString : String("xxxxxx")));
    ClearDisconnected();

    Boolean b;
    if (CanDial(&b), !b) {
        // throw new CallStateException("dialInternal: cannot dial in current state");
        return E_CALL_STATE_EXCEPTION;
    }

    ICallState state;
    mForegroundCall->GetState(&state);
    if (state == ICallState_ACTIVE) {
        SwitchHoldingAndActive();
    }
    if (state != ICallState_IDLE) {
        //we should have failed in !canDial() above before we get here
        // throw new CallStateException("cannot dial in current state");
        return E_CALL_STATE_EXCEPTION;
    }

    mForegroundCall->SetMute(FALSE);
    // try {
    AutoPtr<IConnection> c;
    mForegroundCall->Dial(dialString, (IConnection**)&c);
    *result = c;
    REFCOUNT_ADD(*result)
    return NOERROR;
    // } catch (SipException e) {
    //     Loge("dialInternal: ", e);
    //     throw new CallStateException("dial error: " + e);
    // }
}

String SipPhone::GetUriString(
    /* [in] */ /*TODO ISipProfile*/IInterface* p)
{
    // SipProfile.getUriString() may contain "SIP:" and port
    String domain;
// TODO: Need ISipProfile
    // p->GetSipDomain(&domain);
    return domain + "@" + GetSipDomain(p);
}

String SipPhone::GetSipDomain(
    /* [in] */ /*TODO ISipProfile*/IInterface* p)
{
    String domain;
// TODO: Need ISipProfile
    // p->GetSipDomain(&domain);
    // TODO: move this to SipProfile
    if (domain.EndWith(":5060")) {
        return domain.Substring(0, domain.GetLength() - 5);
    }
    else {
        return domain;
    }
}

ICallState SipPhone::GetCallStateFrom(
    /* [in] */ /*TODO ISipAudioCall*/IInterface* sipAudioCall)
{
// TODO: Need ISipAudioCall
    // if (sipAudioCall->IsOnHold()) return ICallState_HOLDING;
    Int32 sessionState = 0;
    // sipAudioCall->GetState(&sessionState);
    switch (sessionState) {
// TODO: Need SipSession
        // case SipSession.State.READY_TO_CALL:            return ICallState_IDLE;
        // case SipSession.State.INCOMING_CALL:
        // case SipSession.State.INCOMING_CALL_ANSWERING:  return ICallState_INCOMING;
        // case SipSession.State.OUTGOING_CALL:            return ICallState_DIALING;
        // case SipSession.State.OUTGOING_CALL_RING_BACK:  return ICallState_ALERTING;
        // case SipSession.State.OUTGOING_CALL_CANCELING:  return ICallState_DISCONNECTING;
        // case SipSession.State.IN_CALL:                  return ICallState_ACTIVE;
        default:
            Slog(String("illegal connection state: ") + StringUtils::ToString(sessionState));
            return ICallState_DISCONNECTED;
    }
    return 0;
}

void SipPhone::Log(
    /* [in] */ const String& s)
{
    Logger::D(LOGTAG, s);
}

void SipPhone::Slog(
    /* [in] */ const String& s)
{
    Logger::D(LOGTAG, s);
}

void SipPhone::Loge(
    /* [in] */ const String& s)
{
    Logger::E(LOGTAG, s);
}

//void SipPhone::Loge(
//    /* [in] */ const String& s,
//    /* [in] */ Exception* e)
//{
//    // Rlog.e(LOGTAG, s, e);
//}

} // namespace Sip
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
