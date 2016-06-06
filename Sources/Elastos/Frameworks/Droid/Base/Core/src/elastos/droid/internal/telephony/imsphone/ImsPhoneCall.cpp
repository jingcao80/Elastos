
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneCall.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

//=====================================================================
//                             ImsPhoneCall
//=====================================================================
CAR_INTERFACE_IMPL(ImsPhoneCall, Call, IImsPhoneCall);

const String ImsPhoneCall::LOGTAG("ImsPhoneCall");
const Boolean ImsPhoneCall::DBG = FALSE;

ImsPhoneCall::ImsPhoneCall()
{
}

ImsPhoneCall::ImsPhoneCall(
    /* [in] */ IImsPhoneCallTracker* owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode ImsPhoneCall::Dispose()
{
    // ==================before translated======================
    // try {
    //     mOwner.hangup(this);
    // } catch (CallStateException ex) {
    //     //Rlog.e(LOGTAG, "dispose: unexpected error on hangup", ex);
    //     //while disposing, ignore the exception and clean the connections
    // } finally {
    //     for(int i = 0, s = mConnections.size(); i < s; i++) {
    //         ImsPhoneConnection c = (ImsPhoneConnection) mConnections.get(i);
    //         c.onDisconnect(DisconnectCause.LOST_SIGNAL);
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCall::GetConnections(
    /* [out] */ IList/*<Connection>*/** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mConnections;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCall::GetPhone(
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mOwner.mPhone;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCall::IsMultiparty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // ImsCall imsCall = getImsCall();
    // if (imsCall == null) {
    //     return false;
    // }
    //
    // return imsCall.isMultiparty();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCall::Hangup()
{
    // ==================before translated======================
    // mOwner.hangup(this);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCall::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mState.toString();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCall::GetExtras(
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // Bundle imsCallExtras = null;
    // ImsCall call = getImsCall();
    // ImsCallProfile callProfile;
    //
    // if (call != null) {
    //     callProfile = call.getCallProfile();
    //     if (callProfile != null) {
    //         imsCallExtras = callProfile.mCallExtras;
    //     }
    // }
    // if (imsCallExtras == null) {
    //     if (DBG) Rlog.d(LOGTAG, "ImsCall extras are null.");
    // }
    // return imsCallExtras;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCall::Attach(
    /* [in] */ IConnection* conn)
{
    // ==================before translated======================
    // clearDisconnected();
    // mConnections.add(conn);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCall::Attach(
    /* [in] */ IConnection* conn,
    /* [in] */ ICallState state)
{
    // ==================before translated======================
    // this.attach(conn);
    // mState = state;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCall::AttachFake(
    /* [in] */ IConnection* conn,
    /* [in] */ ICallState state)
{
    // ==================before translated======================
    // attach(conn, state);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCall::ConnectionDisconnected(
    /* [in] */ IImsPhoneConnection* conn,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mState != State.DISCONNECTED) {
    //     /* If only disconnected connections remain, we are disconnected*/
    //
    //     boolean hasOnlyDisconnectedConnections = true;
    //
    //     for (int i = 0, s = mConnections.size()  ; i < s; i ++) {
    //         if (mConnections.get(i).getState() != State.DISCONNECTED) {
    //             hasOnlyDisconnectedConnections = false;
    //             break;
    //         }
    //     }
    //
    //     if (hasOnlyDisconnectedConnections) {
    //         mState = State.DISCONNECTED;
    //         return true;
    //     }
    // }
    //
    // return false;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCall::Detach(
    /* [in] */ IImsPhoneConnection* conn)
{
    // ==================before translated======================
    // mConnections.remove(conn);
    // clearDisconnected();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCall::IsFull(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mConnections.size() == ImsPhoneCallTracker.MAX_CONNECTIONS_PER_CALL;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCall::OnHangupLocal()
{
    // ==================before translated======================
    // for (int i = 0, s = mConnections.size(); i < s; i++) {
    //     ImsPhoneConnection cn = (ImsPhoneConnection)mConnections.get(i);
    //     cn.onHangupLocal();
    // }
    // mState = State.DISCONNECTING;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCall::ClearDisconnected()
{
    // ==================before translated======================
    // for (int i = mConnections.size() - 1 ; i >= 0 ; i--) {
    //     ImsPhoneConnection cn = (ImsPhoneConnection)mConnections.get(i);
    //
    //     if (cn.getState() == State.DISCONNECTED) {
    //         mConnections.remove(i);
    //     }
    // }
    //
    // if (mConnections.size() == 0) {
    //     mState = State.IDLE;
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCall::GetFirstConnection(
    /* [out] */ IImsPhoneConnection** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mConnections.size() == 0) return null;
    //
    // for (int i = mConnections.size() - 1 ; i >= 0 ; i--) {
    //     ImsPhoneConnection cn = (ImsPhoneConnection)mConnections.get(i);
    //     if (cn.getState().isAlive()) {
    //         return (ImsPhoneConnection) mConnections.get(i);
    //     }
    // }
    //
    // return null;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCall::SetMute(
    /* [in] */ Boolean mute)
{
    // ==================before translated======================
    // ImsCall imsCall = getFirstConnection() == null ?
    //         null : getFirstConnection().getImsCall();
    // if (imsCall != null) {
    //     try {
    //         imsCall.setMute(mute);
    //     } catch (ImsException e) {
    //         Rlog.e(LOGTAG, "setMute failed : " + e.getMessage());
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCall::Merge(
    /* [in] */ IImsPhoneCall* that,
    /* [in] */ ICallState state)
{
    // ==================before translated======================
    // ImsPhoneConnection[] cc = that.mConnections.toArray(
    //         new ImsPhoneConnection[that.mConnections.size()]);
    // for (ImsPhoneConnection c : cc) {
    //     c.update(null, state);
    // }
    assert(0);
    return NOERROR;
}

AutoPtr</*TODO IImsCall*/IInterface> ImsPhoneCall::GetImsCall()
{
    // ==================before translated======================
    // return (getFirstConnection() == null) ? null : getFirstConnection().getImsCall();
    assert(0);
    AutoPtr<IInterface> empty;
    return empty;
}

Boolean ImsPhoneCall::IsLocalTone(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall)
{
    // ==================before translated======================
    // if ((imsCall == null) || (imsCall.getCallProfile() == null)
    //         || (imsCall.getCallProfile().mMediaProfile == null)) {
    //     return false;
    // }
    //
    // ImsStreamMediaProfile mediaProfile = imsCall.getCallProfile().mMediaProfile;
    //
    // return (mediaProfile.mAudioDirection == ImsStreamMediaProfile.DIRECTION_INACTIVE)
    //         ? true : false;
    assert(0);
    return FALSE;
}

ECode ImsPhoneCall::Update(
    /* [in] */ IImsPhoneConnection* conn,
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
    /* [in] */ ICallState state,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // State newState = state;
    // boolean changed = false;
    //
    // //ImsCall.Listener.onCallProgressing can be invoked several times
    // //and ringback tone mode can be changed during the call setup procedure
    // if (state == State.ALERTING) {
    //     if (mRingbackTonePlayed && !isLocalTone(imsCall)) {
    //         mOwner.mPhone.stopRingbackTone();
    //         mRingbackTonePlayed = false;
    //     } else if (!mRingbackTonePlayed && isLocalTone(imsCall)) {
    //         mOwner.mPhone.startRingbackTone();
    //         mRingbackTonePlayed = true;
    //     }
    // } else {
    //     if (mRingbackTonePlayed) {
    //         mOwner.mPhone.stopRingbackTone();
    //         mRingbackTonePlayed = false;
    //     }
    // }
    //
    // if ((newState != mState) && (state != State.DISCONNECTED)) {
    //     mState = newState;
    //     changed = true;
    // } else if (state == State.DISCONNECTED) {
    //     changed = true;
    // }
    //
    // return changed;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCall::GetHandoverConnection(
    /* [out] */ IImsPhoneConnection** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return (ImsPhoneConnection) getEarliestConnection();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCall::SwitchWith(
    /* [in] */ IImsPhoneCall* that)
{
    // ==================before translated======================
    // synchronized (ImsPhoneCall.class) {
    //     ImsPhoneCall tmp = new ImsPhoneCall();
    //     tmp.takeOver(this);
    //     this.takeOver(that);
    //     that.takeOver(tmp);
    // }
    assert(0);
    return NOERROR;
}

void ImsPhoneCall::TakeOver(
    /* [in] */ IImsPhoneCall* that)
{
    // ==================before translated======================
    // mConnections = that.mConnections;
    // mState = that.mState;
    // for (Connection c : mConnections) {
    //     ((ImsPhoneConnection) c).changeParent(this);
    // }
    assert(0);
}

} // namespace Imsphone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
