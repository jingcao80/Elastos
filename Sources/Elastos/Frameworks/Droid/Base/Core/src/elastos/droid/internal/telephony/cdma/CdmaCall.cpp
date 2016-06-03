#include "Elastos.Droid.Internal.h"

#include "elastos/droid/internal/telephony/cdma/CdmaCall.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                               CdmaCall
//=====================================================================
CAR_INTERFACE_IMPL(CdmaCall, Call, ICdmaCall);

CdmaCall::CdmaCall(
    /* [in] */ ICdmaCallTracker* owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode CdmaCall::Dispose()
{
    assert(0);
    return NOERROR;
}

ECode CdmaCall::GetConnections(
    /* [out] */ IList** result)// IConnection
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // // FIXME should return Collections.unmodifiableList();
    // return mConnections;
    assert(0);
    return NOERROR;
}

ECode CdmaCall::GetPhone(
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mOwner.mPhone;
    assert(0);
    return NOERROR;
}

ECode CdmaCall::IsMultiparty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mConnections.size() > 1;
    assert(0);
    return NOERROR;
}

ECode CdmaCall::Hangup()
{
    // ==================before translated======================
    // mOwner.hangup(this);
    assert(0);
    return NOERROR;
}

ECode CdmaCall::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mState.toString();
    assert(0);
    return NOERROR;
}

ECode CdmaCall::Attach(
    /* [in] */ IConnection* conn,
    /* [in] */ IDriverCall* dc)
{
    // ==================before translated======================
    // mConnections.add(conn);
    //
    // mState = stateFromDCState (dc.state);
    assert(0);
    return NOERROR;
}

ECode CdmaCall::AttachFake(
    /* [in] */ IConnection* conn,
    /* [in] */ ICallState state)
{
    // ==================before translated======================
    // mConnections.add(conn);
    //
    // mState = state;
    assert(0);
    return NOERROR;
}

ECode CdmaCall::ConnectionDisconnected(
    /* [in] */ ICdmaConnection* conn,
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
    //         if (mConnections.get(i).getState()
    //             != State.DISCONNECTED
    //         ) {
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

ECode CdmaCall::Detach(
    /* [in] */ ICdmaConnection* conn)
{
    // ==================before translated======================
    // mConnections.remove(conn);
    //
    // if (mConnections.size() == 0) {
    //     mState = State.IDLE;
    // }
    assert(0);
    return NOERROR;
}

ECode CdmaCall::Update(
    /* [in] */ ICdmaConnection* conn,
    /* [in] */ IDriverCall* dc,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // State newState;
    // boolean changed = false;
    //
    // newState = stateFromDCState(dc.state);
    //
    // if (newState != mState) {
    //     mState = newState;
    //     changed = true;
    // }
    //
    // return changed;
    assert(0);
    return NOERROR;
}

ECode CdmaCall::IsFull(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mConnections.size() == CdmaCallTracker.MAX_CONNECTIONS_PER_CALL;
    assert(0);
    return NOERROR;
}

ECode CdmaCall::OnHangupLocal()
{
    // ==================before translated======================
    // for (int i = 0, s = mConnections.size(); i < s; i++) {
    //     CdmaConnection cn = (CdmaConnection)mConnections.get(i);
    //
    //     cn.onHangupLocal();
    // }
    // mState = State.DISCONNECTING;
    assert(0);
    return NOERROR;
}

ECode CdmaCall::ClearDisconnected()
{
    // ==================before translated======================
    // for (int i = mConnections.size() - 1 ; i >= 0 ; i--) {
    // CdmaConnection cn = (CdmaConnection)mConnections.get(i);
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

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
