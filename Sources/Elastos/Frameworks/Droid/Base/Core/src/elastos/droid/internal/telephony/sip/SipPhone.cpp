
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/sip/SipPhone.h"

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
    // ==================before translated======================
    // if (SC_DBG) log("reset");
    // mConnections.clear();
    // setState(Call.State.IDLE);
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipCall::SwitchWith(
    /* [in] */ SipCall* that)
{
    // ==================before translated======================
    // if (SC_DBG) log("switchWith");
    // synchronized (SipPhone.class) {
    //     SipCall tmp = new SipCall();
    //     tmp.takeOver(this);
    //     this.takeOver(that);
    //     that.takeOver(tmp);
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipCall::GetPhone(
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return SipPhone.this;
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipCall::GetConnections(
    /* [out] */ IList/*<Connection>*/** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (SC_VDBG) log("getConnections");
    // synchronized (SipPhone.class) {
    //     // FIXME should return Collections.unmodifiableList();
    //     return mConnections;
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipCall::Dial(
    /* [in] */ const String& originalNumber,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (SC_DBG) log("dial: num=" + (SC_VDBG ? originalNumber : "xxx"));
    // // TODO: Should this be synchronized?
    // String calleeSipUri = originalNumber;
    // if (!calleeSipUri.contains("@")) {
    //     String replaceStr = Pattern.quote(mProfile.getUserName() + "@");
    //     calleeSipUri = mProfile.getUriString().replaceFirst(replaceStr,
    //             calleeSipUri + "@");
    // }
    // try {
    //     SipProfile callee =
    //             new SipProfile.Builder(calleeSipUri).build();
    //     SipConnection c = new SipConnection(this, callee,
    //             originalNumber);
    //     c.dial();
    //     mConnections.add(c);
    //     setState(Call.State.DIALING);
    //     return c;
    // } catch (ParseException e) {
    //     throw new SipException("dial", e);
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipCall::Hangup()
{
    // ==================before translated======================
    // synchronized (SipPhone.class) {
    //     if (mState.isAlive()) {
    //         if (SC_DBG) log("hangup: call " + getState()
    //                 + ": " + this + " on phone " + getPhone());
    //         setState(State.DISCONNECTING);
    //         CallStateException excp = null;
    //         for (Connection c : mConnections) {
    //             try {
    //                 c.hangup();
    //             } catch (CallStateException e) {
    //                 excp = e;
    //             }
    //         }
    //         if (excp != null) throw excp;
    //     } else {
    //         if (SC_DBG) log("hangup: dead call " + getState()
    //                 + ": " + this + " on phone " + getPhone());
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipCall::InitIncomingCall(
    /* [in] */ /*TODO ISipAudioCall*/IInterface* sipAudioCall,
    /* [in] */ Boolean makeCallWait,
    /* [out] */ SipConnection** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // SipProfile callee = sipAudioCall.getPeerProfile();
    // SipConnection c = new SipConnection(this, callee);
    // mConnections.add(c);
    //
    // Call.State newState = makeCallWait ? State.WAITING : State.INCOMING;
    // c.initIncomingCall(sipAudioCall, newState);
    //
    // setState(newState);
    // notifyNewRingingConnectionP(c);
    // return c;
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipCall::RejectCall()
{
    // ==================before translated======================
    // if (SC_DBG) log("rejectCall:");
    // hangup();
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipCall::AcceptCall()
{
    // ==================before translated======================
    // if (SC_DBG) log("acceptCall: accepting");
    // if (this != mRingingCall) {
    //     throw new CallStateException("acceptCall() in a non-ringing call");
    // }
    // if (mConnections.size() != 1) {
    //     throw new CallStateException("acceptCall() in a conf call");
    // }
    // ((SipConnection) mConnections.get(0)).acceptCall();
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipCall::SetAudioGroupMode()
{
    // ==================before translated======================
    // AudioGroup audioGroup = getAudioGroup();
    // if (audioGroup == null) {
    //     if (SC_DBG) log("setAudioGroupMode: audioGroup == null ignore");
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
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipCall::Hold()
{
    // ==================before translated======================
    // if (SC_DBG) log("hold:");
    // setState(State.HOLDING);
    // for (Connection c : mConnections) ((SipConnection) c).hold();
    // setAudioGroupMode();
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipCall::Unhold()
{
    // ==================before translated======================
    // if (SC_DBG) log("unhold:");
    // setState(State.ACTIVE);
    // AudioGroup audioGroup = new AudioGroup();
    // for (Connection c : mConnections) {
    //     ((SipConnection) c).unhold(audioGroup);
    // }
    // setAudioGroupMode();
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipCall::SetMute(
    /* [in] */ Boolean muted)
{
    // ==================before translated======================
    // if (SC_DBG) log("setMute: muted=" + muted);
    // for (Connection c : mConnections) {
    //     ((SipConnection) c).setMute(muted);
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipCall::GetMute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean ret = mConnections.isEmpty()
    //         ? false
    //         : ((SipConnection) mConnections.get(0)).getMute();
    // if (SC_DBG) log("getMute: ret=" + ret);
    // return ret;
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipCall::Merge(
    /* [in] */ SipCall* that)
{
    // ==================before translated======================
    // if (SC_DBG) log("merge:");
    // AudioGroup audioGroup = getAudioGroup();
    //
    // // copy to an array to avoid concurrent modification as connections
    // // in that.connections will be removed in add(SipConnection).
    // Connection[] cc = that.mConnections.toArray(
    //         new Connection[that.mConnections.size()]);
    // for (Connection c : cc) {
    //     SipConnection conn = (SipConnection) c;
    //     add(conn);
    //     if (conn.getState() == Call.State.HOLDING) {
    //         conn.unhold(audioGroup);
    //     }
    // }
    // that.setState(Call.State.IDLE);
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipCall::SendDtmf(
    /* [in] */ Char16 c)
{
    // ==================before translated======================
    // if (SC_DBG) log("sendDtmf: c=" + c);
    // AudioGroup audioGroup = getAudioGroup();
    // if (audioGroup == null) {
    //     if (SC_DBG) log("sendDtmf: audioGroup == null, ignore c=" + c);
    //     return;
    // }
    // audioGroup.sendDtmf(convertDtmf(c));
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipCall::OnConnectionStateChanged(
    /* [in] */ SipConnection* conn)
{
    // ==================before translated======================
    // // this can be called back when a conf call is formed
    // if (SC_DBG) log("onConnectionStateChanged: conn=" + conn);
    // if (mState != State.ACTIVE) {
    //     setState(conn.getState());
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipCall::OnConnectionEnded(
    /* [in] */ SipConnection* conn)
{
    // ==================before translated======================
    // // set state to DISCONNECTED only when all conns are disconnected
    // if (SC_DBG) log("onConnectionEnded: conn=" + conn);
    // if (mState != State.DISCONNECTED) {
    //     boolean allConnectionsDisconnected = true;
    //     if (SC_DBG) log("---check connections: "
    //             + mConnections.size());
    //     for (Connection c : mConnections) {
    //         if (SC_DBG) log("   state=" + c.getState() + ": "
    //                 + c);
    //         if (c.getState() != State.DISCONNECTED) {
    //             allConnectionsDisconnected = false;
    //             break;
    //         }
    //     }
    //     if (allConnectionsDisconnected) setState(State.DISCONNECTED);
    // }
    // notifyDisconnectP(conn);
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipCall::SetState(
    /* [in] */ ICallState newState)
{
    // ==================before translated======================
    // if (mState != newState) {
    //     if (SC_DBG) log("setState: cur state" + mState
    //             + " --> " + newState + ": " + this + ": on phone "
    //             + getPhone() + " " + mConnections.size());
    //
    //     if (newState == Call.State.ALERTING) {
    //         mState = newState; // need in ALERTING to enable ringback
    //         startRingbackTone();
    //     } else if (mState == Call.State.ALERTING) {
    //         stopRingbackTone();
    //     }
    //     mState = newState;
    //     updatePhoneState();
    //     notifyPreciseCallStateChanged();
    // }
    assert(0);
    return NOERROR;
}

void SipPhone::SipCall::TakeOver(
    /* [in] */ SipCall* that)
{
    // ==================before translated======================
    // if (SC_DBG) log("takeOver");
    // mConnections = that.mConnections;
    // mState = that.mState;
    // for (Connection c : mConnections) {
    //     ((SipConnection) c).changeOwner(this);
    // }
    assert(0);
}

Boolean SipPhone::SipCall::IsSpeakerOn()
{
    // ==================before translated======================
    // Boolean ret = ((AudioManager) mContext.getSystemService(Context.AUDIO_SERVICE))
    //         .isSpeakerphoneOn();
    // if (SC_VDBG) log("isSpeakerOn: ret=" + ret);
    // return ret;
    assert(0);
    return FALSE;
}

void SipPhone::SipCall::Add(
    /* [in] */ SipConnection* conn)
{
    // ==================before translated======================
    // if (SC_DBG) log("add:");
    // SipCall call = conn.getCall();
    // if (call == this) return;
    // if (call != null) call.mConnections.remove(conn);
    //
    // mConnections.add(conn);
    // conn.changeOwner(this);
    assert(0);
}

Int32 SipPhone::SipCall::ConvertDtmf(
    /* [in] */ Char16 c)
{
    // ==================before translated======================
    // int code = c - '0';
    // if ((code < 0) || (code > 9)) {
    //     switch (c) {
    //         case '*': return 10;
    //         case '#': return 11;
    //         case 'A': return 12;
    //         case 'B': return 13;
    //         case 'C': return 14;
    //         case 'D': return 15;
    //         default:
    //             throw new IllegalArgumentException(
    //                     "invalid DTMF char: " + (int) c);
    //     }
    // }
    // return code;
    assert(0);
    return 0;
}

AutoPtr</*TODO IAudioGroup*/IInterface> SipPhone::SipCall::GetAudioGroup()
{
    // ==================before translated======================
    // if (mConnections.isEmpty()) return null;
    // return ((SipConnection) mConnections.get(0)).getAudioGroup();
    assert(0);
    AutoPtr</*TODO IAudioGroup*/IInterface> empty;
    return empty;
}

void SipPhone::SipCall::Log(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // Rlog.d(SC_TAG, s);
    assert(0);
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
    : SipConnectionBase(originalNumber)
    , mHost(host)
{
    // ==================before translated======================
    // super(originalNumber);
    // mOwner = owner;
    // mPeer = callee;
    // mOriginalNumber = originalNumber;
}

SipPhone::SipConnection::SipConnection(
    /* [in] */ SipPhone* host,
    /* [in] */ SipCall* owner,
    /* [in] */ /*TODO ISipProfile*/IInterface* callee)
    : SipConnection(host, owner, callee, host->GetUriString(callee))
{
    // ==================before translated======================
    // this(owner, callee, getUriString(callee));
    mHost = host;
}

ECode SipPhone::SipConnection::GetCnapName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // String displayName = mPeer.getDisplayName();
    // return TextUtils.isEmpty(displayName) ? null
    //                                       : displayName;
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipConnection::GetNumberPresentation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return PhoneConstants.PRESENTATION_ALLOWED;
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipConnection::InitIncomingCall(
    /* [in] */ /*TODO ISipAudioCall*/IInterface* sipAudioCall,
    /* [in] */ ICallState newState)
{
    // ==================before translated======================
    // setState(newState);
    // mSipAudioCall = sipAudioCall;
    // sipAudioCall.setListener(mAdapter); // call back to set state
    // mIncoming = true;
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipConnection::AcceptCall()
{
    // ==================before translated======================
    // try {
    //     mSipAudioCall.answerCall(TIMEOUT_ANSWER_CALL);
    // } catch (SipException e) {
    //     throw new CallStateException("acceptCall(): " + e);
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipConnection::ChangeOwner(
    /* [in] */ SipCall* owner)
{
    // ==================before translated======================
    // mOwner = owner;
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipConnection::GetAudioGroup(
    /* [out] */ /*TODO IAudioGroup*/IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mSipAudioCall == null) return null;
    // return mSipAudioCall.getAudioGroup();
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipConnection::Dial()
{
    // ==================before translated======================
    // setState(Call.State.DIALING);
    // mSipAudioCall = mSipManager.makeAudioCall(mProfile, mPeer, null,
    //         TIMEOUT_MAKE_CALL);
    // mSipAudioCall.setListener(mAdapter);
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipConnection::Hold()
{
    // ==================before translated======================
    // setState(Call.State.HOLDING);
    // try {
    //     mSipAudioCall.holdCall(TIMEOUT_HOLD_CALL);
    // } catch (SipException e) {
    //     throw new CallStateException("hold(): " + e);
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipConnection::Unhold(
    /* [in] */ /*TODO IAudioGroup*/IInterface* audioGroup)
{
    // ==================before translated======================
    // mSipAudioCall.setAudioGroup(audioGroup);
    // setState(Call.State.ACTIVE);
    // try {
    //     mSipAudioCall.continueCall(TIMEOUT_HOLD_CALL);
    // } catch (SipException e) {
    //     throw new CallStateException("unhold(): " + e);
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipConnection::SetMute(
    /* [in] */ Boolean muted)
{
    // ==================before translated======================
    // if ((mSipAudioCall != null) && (muted != mSipAudioCall.isMuted())) {
    //     if (SCN_DBG) log("setState: prev muted=" + !muted + " new muted=" + muted);
    //     mSipAudioCall.toggleMute();
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipConnection::GetMute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return (mSipAudioCall == null) ? false
    //                                : mSipAudioCall.isMuted();
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipConnection::GetState(
    /* [out] */ ICallState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mState;
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipConnection::IsIncoming(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mIncoming;
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipConnection::GetAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // Phone app uses this to query caller ID. Return the original dial
    // // number (which may be a PSTN number) instead of the peer's SIP
    // // URI.
    // return mOriginalNumber;
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipConnection::GetCall(
    /* [out] */ SipCall** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mOwner;
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipConnection::Hangup()
{
    // ==================before translated======================
    // synchronized (SipPhone.class) {
    //     if (SCN_DBG) log("hangup: conn=" + mPeer.getUriString()
    //             + ": " + mState + ": on phone "
    //             + getPhone().getPhoneName());
    //     if (!mState.isAlive()) return;
    //     try {
    //         SipAudioCall sipAudioCall = mSipAudioCall;
    //         if (sipAudioCall != null) {
    //             sipAudioCall.setListener(null);
    //             sipAudioCall.endCall();
    //         }
    //     } catch (SipException e) {
    //         throw new CallStateException("hangup(): " + e);
    //     } finally {
    //         mAdapter.onCallEnded(((mState == Call.State.INCOMING)
    //                 || (mState == Call.State.WAITING))
    //                 ? DisconnectCause.INCOMING_REJECTED
    //                 : DisconnectCause.LOCAL);
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipConnection::Separate()
{
    // ==================before translated======================
    // synchronized (SipPhone.class) {
    //     SipCall call = (getPhone() == SipPhone.this)
    //             ? (SipCall) getBackgroundCall()
    //             : (SipCall) getForegroundCall();
    //     if (call.getState() != Call.State.IDLE) {
    //         throw new CallStateException(
    //                 "cannot put conn back to a call in non-idle state: "
    //                 + call.getState());
    //     }
    //     if (SCN_DBG) log("separate: conn="
    //             + mPeer.getUriString() + " from " + mOwner + " back to "
    //             + call);
    //
    //     // separate the AudioGroup and connection from the original call
    //     Phone originalPhone = getPhone();
    //     AudioGroup audioGroup = call.getAudioGroup(); // may be null
    //     call.add(this);
    //     mSipAudioCall.setAudioGroup(audioGroup);
    //
    //     // put the original call to bg; and the separated call becomes
    //     // fg if it was in bg
    //     originalPhone.switchHoldingAndActive();
    //
    //     // start audio and notify the phone app of the state change
    //     call = (SipCall) getForegroundCall();
    //     mSipAudioCall.startAudio();
    //     call.onConnectionStateChanged(this);
    // }
    assert(0);
    return NOERROR;
}

void SipPhone::SipConnection::SetState(
    /* [in] */ ICallState state)
{
    // ==================before translated======================
    // if (state == mState) return;
    // super.setState(state);
    // mState = state;
    assert(0);
}

ECode SipPhone::SipConnection::GetPhone(
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mOwner.getPhone();
    assert(0);
    return NOERROR;
}

void SipPhone::SipConnection::Log(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // Rlog.d(SCN_TAG, s);
    assert(0);
}

//=====================================================================
//                  SipPhone::InnerSipAudioCallAdapter
//=====================================================================
SipPhone::InnerSipAudioCallAdapter::InnerSipAudioCallAdapter(
    /* [in] */ SipConnection* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode SipPhone::InnerSipAudioCallAdapter::OnCallEstablished(
    /* [in] */ /*TODO ISipAudioCall*/IInterface* call)
{
    // ==================before translated======================
    // onChanged(call);
    // // Race onChanged synchronized this isn't
    // if (mState == Call.State.ACTIVE) call.startAudio();
    assert(0);
    return NOERROR;
}

ECode SipPhone::InnerSipAudioCallAdapter::OnCallHeld(
    /* [in] */ /*TODO ISipAudioCall*/IInterface* call)
{
    // ==================before translated======================
    // onChanged(call);
    // // Race onChanged synchronized this isn't
    // if (mState == Call.State.HOLDING) call.startAudio();
    assert(0);
    return NOERROR;
}

ECode SipPhone::InnerSipAudioCallAdapter::OnChanged(
    /* [in] */ /*TODO ISipAudioCall*/IInterface* call)
{
    // ==================before translated======================
    // synchronized (SipPhone.class) {
    //     Call.State newState = getCallStateFrom(call);
    //     if (mState == newState) return;
    //     if (newState == Call.State.INCOMING) {
    //         setState(mOwner.getState()); // INCOMING or WAITING
    //     } else {
    //         if (mOwner == mRingingCall) {
    //             if (mRingingCall.getState() == Call.State.WAITING) {
    //                 try {
    //                     switchHoldingAndActive();
    //                 } catch (CallStateException e) {
    //                     // disconnect the call.
    //                     onCallEnded(DisconnectCause.LOCAL);
    //                     return;
    //                 }
    //             }
    //             mForegroundCall.switchWith(mRingingCall);
    //         }
    //         setState(newState);
    //     }
    //     mOwner.onConnectionStateChanged(SipConnection.this);
    //     if (SCN_DBG) log("onChanged: "
    //             + mPeer.getUriString() + ": " + mState
    //             + " on phone " + getPhone());
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::InnerSipAudioCallAdapter::OnCallEnded(
    /* [in] */ Int32 cause)
{
    // ==================before translated======================
    // if (getDisconnectCause() != DisconnectCause.LOCAL) {
    //     setDisconnectCause(cause);
    // }
    // synchronized (SipPhone.class) {
    //     setState(Call.State.DISCONNECTED);
    //     SipAudioCall sipAudioCall = mSipAudioCall;
    //     // FIXME: This goes null and is synchronized, but many uses aren't sync'd
    //     mSipAudioCall = null;
    //     String sessionState = (sipAudioCall == null)
    //             ? ""
    //             : (sipAudioCall.getState() + ", ");
    //     if (SCN_DBG) log("[SipAudioCallAdapter] onCallEnded: "
    //             + mPeer.getUriString() + ": " + sessionState
    //             + "cause: " + getDisconnectCause() + ", on phone "
    //             + getPhone());
    //     if (sipAudioCall != null) {
    //         sipAudioCall.setListener(null);
    //         sipAudioCall.close();
    //     }
    //     mOwner.onConnectionEnded(SipConnection.this);
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::InnerSipAudioCallAdapter::OnError(
    /* [in] */ Int32 cause)
{
    // ==================before translated======================
    // if (SCN_DBG) log("onError: " + cause);
    // onCallEnded(cause);
    assert(0);
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
    // ==================before translated======================
    // if (SACA_DBG) log("onCallEnded: call=" + call);
    // onCallEnded(call.isInCall()
    //         ? DisconnectCause.NORMAL
    //         : DisconnectCause.INCOMING_MISSED);
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipAudioCallAdapter::OnCallBusy(
    /* [in] */ /*TODO ISipAudioCall*/IInterface* call)
{
    // ==================before translated======================
    // if (SACA_DBG) log("onCallBusy: call=" + call);
    // onCallEnded(DisconnectCause.BUSY);
    assert(0);
    return NOERROR;
}

ECode SipPhone::SipAudioCallAdapter::OnError(
    /* [in] */ /*TODO ISipAudioCall*/IInterface* call,
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& errorMessage)
{
    // ==================before translated======================
    // if (SACA_DBG) {
    //     log("onError: call=" + call + " code="+ SipErrorCode.toString(errorCode)
    //         + ": " + errorMessage);
    // }
    // switch (errorCode) {
    //     case SipErrorCode.SERVER_UNREACHABLE:
    //         onError(DisconnectCause.SERVER_UNREACHABLE);
    //         break;
    //     case SipErrorCode.PEER_NOT_REACHABLE:
    //         onError(DisconnectCause.NUMBER_UNREACHABLE);
    //         break;
    //     case SipErrorCode.INVALID_REMOTE_URI:
    //         onError(DisconnectCause.INVALID_NUMBER);
    //         break;
    //     case SipErrorCode.TIME_OUT:
    //     case SipErrorCode.TRANSACTION_TERMINTED:
    //         onError(DisconnectCause.TIMED_OUT);
    //         break;
    //     case SipErrorCode.DATA_CONNECTION_LOST:
    //         onError(DisconnectCause.LOST_SIGNAL);
    //         break;
    //     case SipErrorCode.INVALID_CREDENTIALS:
    //         onError(DisconnectCause.INVALID_CREDENTIALS);
    //         break;
    //     case SipErrorCode.CROSS_DOMAIN_AUTHENTICATION:
    //         onError(DisconnectCause.OUT_OF_NETWORK);
    //         break;
    //     case SipErrorCode.SERVER_ERROR:
    //         onError(DisconnectCause.SERVER_ERROR);
    //         break;
    //     case SipErrorCode.SOCKET_ERROR:
    //     case SipErrorCode.CLIENT_ERROR:
    //     default:
    //         onError(DisconnectCause.ERROR_UNSPECIFIED);
    // }
    assert(0);
    return NOERROR;
}

void SipPhone::SipAudioCallAdapter::Log(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // Rlog.d(SACA_TAG, s);
    assert(0);
}

//=====================================================================
//                               SipPhone
//=====================================================================
const String SipPhone::LOGTAG("SipPhone");
const Boolean SipPhone::DBG = TRUE;
const Boolean SipPhone::VDBG = FALSE;
const Int32 SipPhone::TIMEOUT_MAKE_CALL;
const Int32 SipPhone::TIMEOUT_ANSWER_CALL;
const Int32 SipPhone::TIMEOUT_HOLD_CALL;

SipPhone::SipPhone(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneNotifier* notifier,
    /* [in] */ /*TODO ISipProfile*/IInterface* profile)
    : SipPhoneBase(String(""), context, notifier)//TODO the string value is not correct
{
    // ==================before translated======================
    // super("SIP:" + profile.getUriString(), context, notifier);

    //
    // if (DBG) log("new SipPhone: " + profile.getUriString());
    // mRingingCall = new SipCall();
    // mForegroundCall = new SipCall();
    // mBackgroundCall = new SipCall();
    // mProfile = profile;
    // mSipManager = SipManager.newInstance(context);
}

ECode SipPhone::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (o == this) return true;
    // if (!(o instanceof SipPhone)) return false;
    // SipPhone that = (SipPhone) o;
    // return mProfile.getUriString().equals(that.mProfile.getUriString());
    assert(0);
    return NOERROR;
}

ECode SipPhone::GetSipUri(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mProfile.getUriString();
    assert(0);
    return NOERROR;
}

ECode SipPhone::Equals(
    /* [in] */ SipPhone* phone,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return getSipUri().equals(phone.getSipUri());
    assert(0);
    return NOERROR;
}

ECode SipPhone::TakeIncomingCall(
    /* [in] */ IInterface* incomingCall,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // FIXME: Is synchronizing on the class necessary, should we use a mLockObj?
    // // Also there are many things not synchronized, of course
    // // this may be true of CdmaPhone and GsmPhone too!!!
    // synchronized (SipPhone.class) {
    //     if (!(incomingCall instanceof SipAudioCall)) {
    //         if (DBG) log("takeIncomingCall: ret=null, not a SipAudioCall");
    //         return null;
    //     }
    //     if (mRingingCall.getState().isAlive()) {
    //         if (DBG) log("takeIncomingCall: ret=null, ringingCall not alive");
    //         return null;
    //     }
    //
    //     // FIXME: is it true that we cannot take any incoming call if
    //     // both foreground and background are active
    //     if (mForegroundCall.getState().isAlive()
    //             && mBackgroundCall.getState().isAlive()) {
    //         if (DBG) {
    //             log("takeIncomingCall: ret=null," + " foreground and background both alive");
    //         }
    //         return null;
    //     }
    //
    //     try {
    //         SipAudioCall sipAudioCall = (SipAudioCall) incomingCall;
    //         if (DBG) log("takeIncomingCall: taking call from: "
    //                 + sipAudioCall.getPeerProfile().getUriString());
    //         String localUri = sipAudioCall.getLocalProfile().getUriString();
    //         if (localUri.equals(mProfile.getUriString())) {
    //             boolean makeCallWait = mForegroundCall.getState().isAlive();
    //             SipConnection connection = mRingingCall.initIncomingCall(sipAudioCall,
    //                     makeCallWait);
    //             if (sipAudioCall.getState() != SipSession.State.INCOMING_CALL) {
    //                 // Peer cancelled the call!
    //                 if (DBG) log("    takeIncomingCall: call cancelled !!");
    //                 mRingingCall.reset();
    //                 connection = null;
    //             }
    //             return connection;
    //         }
    //     } catch (Exception e) {
    //         // Peer may cancel the call at any time during the time we hook
    //         // up ringingCall with sipAudioCall. Clean up ringingCall when
    //         // that happens.
    //         if (DBG) log("    takeIncomingCall: exception e=" + e);
    //         mRingingCall.reset();
    //     }
    //     if (DBG) log("takeIncomingCall: NOT taking !!");
    //     return null;
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::AcceptCall(
    /* [in] */ Int32 videoState)
{
    // ==================before translated======================
    // synchronized (SipPhone.class) {
    //     if ((mRingingCall.getState() == Call.State.INCOMING) ||
    //             (mRingingCall.getState() == Call.State.WAITING)) {
    //         if (DBG) log("acceptCall: accepting");
    //         // Always unmute when answering a new call
    //         mRingingCall.setMute(false);
    //         mRingingCall.acceptCall();
    //     } else {
    //         if (DBG) {
    //             log("acceptCall:" +
    //                 " throw CallStateException(\"phone not ringing\")");
    //         }
    //         throw new CallStateException("phone not ringing");
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::RejectCall()
{
    // ==================before translated======================
    // synchronized (SipPhone.class) {
    //     if (mRingingCall.getState().isRinging()) {
    //         if (DBG) log("rejectCall: rejecting");
    //         mRingingCall.rejectCall();
    //     } else {
    //         if (DBG) {
    //             log("rejectCall:" +
    //                 " throw CallStateException(\"phone not ringing\")");
    //         }
    //         throw new CallStateException("phone not ringing");
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (SipPhone.class) {
    //     return dialInternal(dialString, videoState);
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::SwitchHoldingAndActive()
{
    // ==================before translated======================
    // if (DBG) log("dialInternal: switch fg and bg");
    // synchronized (SipPhone.class) {
    //     mForegroundCall.switchWith(mBackgroundCall);
    //     if (mBackgroundCall.getState().isAlive()) mBackgroundCall.hold();
    //     if (mForegroundCall.getState().isAlive()) mForegroundCall.unhold();
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::CanConference(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("canConference: ret=true");
    // return true;
    assert(0);
    return NOERROR;
}

ECode SipPhone::Conference()
{
    // ==================before translated======================
    // synchronized (SipPhone.class) {
    //     if ((mForegroundCall.getState() != SipCall.State.ACTIVE)
    //             || (mForegroundCall.getState() != SipCall.State.ACTIVE)) {
    //         throw new CallStateException("wrong state to merge calls: fg="
    //                 + mForegroundCall.getState() + ", bg="
    //                 + mBackgroundCall.getState());
    //     }
    //     if (DBG) log("conference: merge fg & bg");
    //     mForegroundCall.merge(mBackgroundCall);
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::Conference(
    /* [in] */ ICall* that)
{
    // ==================before translated======================
    // synchronized (SipPhone.class) {
    //     if (!(that instanceof SipCall)) {
    //         throw new CallStateException("expect " + SipCall.class
    //                 + ", cannot merge with " + that.getClass());
    //     }
    //     mForegroundCall.merge((SipCall) that);
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::CanTransfer(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;
    assert(0);
    return NOERROR;
}

ECode SipPhone::ExplicitCallTransfer()
{
    // ==================before translated======================
    // //mCT.explicitCallTransfer();
    assert(0);
    return NOERROR;
}

ECode SipPhone::ClearDisconnected()
{
    // ==================before translated======================
    // synchronized (SipPhone.class) {
    //     mRingingCall.clearDisconnected();
    //     mForegroundCall.clearDisconnected();
    //     mBackgroundCall.clearDisconnected();
    //
    //     updatePhoneState();
    //     notifyPreciseCallStateChanged();
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::SendDtmf(
    /* [in] */ Char16 c)
{
    // ==================before translated======================
    // if (!PhoneNumberUtils.is12Key(c)) {
    //     loge("sendDtmf called with invalid character '" + c + "'");
    // } else if (mForegroundCall.getState().isAlive()) {
    //     synchronized (SipPhone.class) {
    //         mForegroundCall.sendDtmf(c);
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::StartDtmf(
    /* [in] */ Char16 c)
{
    // ==================before translated======================
    // if (!PhoneNumberUtils.is12Key(c)) {
    //     loge("startDtmf called with invalid character '" + c + "'");
    // } else {
    //     sendDtmf(c);
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::StopDtmf()
{
    // ==================before translated======================
    // // no op
    assert(0);
    return NOERROR;
}

ECode SipPhone::SendBurstDtmf(
    /* [in] */ const String& dtmfString)
{
    // ==================before translated======================
    // loge("sendBurstDtmf() is a CDMA method");
    assert(0);
    return NOERROR;
}

ECode SipPhone::GetOutgoingCallerIdDisplay(
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // // FIXME: what to reply?
    // AsyncResult.forMessage(onComplete, null, null);
    // onComplete.sendToTarget();
    assert(0);
    return NOERROR;
}

ECode SipPhone::SetOutgoingCallerIdDisplay(
    /* [in] */ Int32 commandInterfaceCLIRMode,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // // FIXME: what's this for SIP?
    // AsyncResult.forMessage(onComplete, null, null);
    // onComplete.sendToTarget();
    assert(0);
    return NOERROR;
}

ECode SipPhone::GetCallWaiting(
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // // FIXME: what to reply?
    // AsyncResult.forMessage(onComplete, null, null);
    // onComplete.sendToTarget();
    assert(0);
    return NOERROR;
}

ECode SipPhone::SetCallWaiting(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // // FIXME: what to reply?
    // loge("call waiting not supported");
    assert(0);
    return NOERROR;
}

ECode SipPhone::SetEchoSuppressionEnabled()
{
    // ==================before translated======================
    // // Echo suppression may not be available on every device. So, check
    // // whether it is supported
    // synchronized (SipPhone.class) {
    //     AudioManager audioManager = (AudioManager) mContext.getSystemService(Context.AUDIO_SERVICE);
    //     String echoSuppression = audioManager.getParameters("ec_supported");
    //     if (echoSuppression.contains("off")) {
    //         mForegroundCall.setAudioGroupMode();
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::SetMute(
    /* [in] */ Boolean muted)
{
    // ==================before translated======================
    // synchronized (SipPhone.class) {
    //     mForegroundCall.setMute(muted);
    // }
    assert(0);
    return NOERROR;
}

ECode SipPhone::GetMute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return (mForegroundCall.getState().isAlive()
    //         ? mForegroundCall.getMute()
    //         : mBackgroundCall.getMute());
    assert(0);
    return NOERROR;
}

ECode SipPhone::GetForegroundCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mForegroundCall;
    assert(0);
    return NOERROR;
}

ECode SipPhone::GetBackgroundCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mBackgroundCall;
    assert(0);
    return NOERROR;
}

ECode SipPhone::GetRingingCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mRingingCall;
    assert(0);
    return NOERROR;
}

ECode SipPhone::GetServiceState(
    /* [out] */ IServiceState** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // FIXME: we may need to provide this when data connectivity is lost
    // // or when server is down
    // return super.getServiceState();
    assert(0);
    return NOERROR;
}

ECode SipPhone::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [in] */ IBundle* extras,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // NOTE: Add implementation if call extras are needed to be
    // //       passed through SIP phone.
    // return null;
    assert(0);
    return NOERROR;
}

AutoPtr<IConnection> SipPhone::DialInternal(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState)
{
    // ==================before translated======================
    // if (DBG) log("dialInternal: dialString=" + (VDBG ? dialString : "xxxxxx"));
    // clearDisconnected();
    //
    // if (!canDial()) {
    //     throw new CallStateException("dialInternal: cannot dial in current state");
    // }
    // if (mForegroundCall.getState() == SipCall.State.ACTIVE) {
    //     switchHoldingAndActive();
    // }
    // if (mForegroundCall.getState() != SipCall.State.IDLE) {
    //     //we should have failed in !canDial() above before we get here
    //     throw new CallStateException("cannot dial in current state");
    // }
    //
    // mForegroundCall.setMute(false);
    // try {
    //     Connection c = mForegroundCall.dial(dialString);
    //     return c;
    // } catch (SipException e) {
    //     loge("dialInternal: ", e);
    //     throw new CallStateException("dial error: " + e);
    // }
    assert(0);
    AutoPtr<IConnection> empty;
    return empty;
}

String SipPhone::GetUriString(
    /* [in] */ /*TODO ISipProfile*/IInterface* p)
{
    // ==================before translated======================
    // // SipProfile.getUriString() may contain "SIP:" and port
    // return p.getUserName() + "@" + getSipDomain(p);
    assert(0);
    return String("");
}

String SipPhone::GetSipDomain(
    /* [in] */ /*TODO ISipProfile*/IInterface* p)
{
    // ==================before translated======================
    // String domain = p.getSipDomain();
    // // TODO: move this to SipProfile
    // if (domain.endsWith(":5060")) {
    //     return domain.substring(0, domain.length() - 5);
    // } else {
    //     return domain;
    // }
    assert(0);
    return String("");
}

ICallState SipPhone::GetCallStateFrom(
    /* [in] */ /*TODO ISipAudioCall*/IInterface* sipAudioCall)
{
    // ==================before translated======================
    // if (sipAudioCall.isOnHold()) return Call.State.HOLDING;
    // int sessionState = sipAudioCall.getState();
    // switch (sessionState) {
    //     case SipSession.State.READY_TO_CALL:            return Call.State.IDLE;
    //     case SipSession.State.INCOMING_CALL:
    //     case SipSession.State.INCOMING_CALL_ANSWERING:  return Call.State.INCOMING;
    //     case SipSession.State.OUTGOING_CALL:            return Call.State.DIALING;
    //     case SipSession.State.OUTGOING_CALL_RING_BACK:  return Call.State.ALERTING;
    //     case SipSession.State.OUTGOING_CALL_CANCELING:  return Call.State.DISCONNECTING;
    //     case SipSession.State.IN_CALL:                  return Call.State.ACTIVE;
    //     default:
    //         slog("illegal connection state: " + sessionState);
    //         return Call.State.DISCONNECTED;
    // }
    assert(0);
    return 0;
}

void SipPhone::Log(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, s);
    assert(0);
}

void SipPhone::Slog(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, s);
    assert(0);
}

void SipPhone::Loge(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, s);
    assert(0);
}

//void SipPhone::Loge(
//    /* [in] */ const String& s,
//    /* [in] */ Exception* e)
//{
//    // ==================before translated======================
//    // Rlog.e(LOGTAG, s, e);
//    assert(0);
//}

} // namespace Sip
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
