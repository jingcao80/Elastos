
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/sip/SipCallBase.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Sip {

//=====================================================================
//                             SipCallBase
//=====================================================================
//CAR_INTERFACE_IMPL(SipCallBase, Call, ISipCallBase);

ECode SipCallBase::GetConnections(
    /* [out] */ IList/*<Connection>*/** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // FIXME should return Collections.unmodifiableList();
    // return mConnections;
    assert(0);
    return NOERROR;
}

ECode SipCallBase::IsMultiparty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mConnections.size() > 1;
    assert(0);
    return NOERROR;
}

ECode SipCallBase::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mState.toString() + ":" + super.toString();
    assert(0);
    return NOERROR;
}

ECode SipCallBase::ClearDisconnected()
{
    // ==================before translated======================
    // for (Iterator<Connection> it = mConnections.iterator(); it.hasNext(); ) {
    //     Connection c = it.next();
    //     if (c.getState() == State.DISCONNECTED) it.remove();
    // }
    //
    // if (mConnections.isEmpty()) setState(State.IDLE);
    assert(0);
    return NOERROR;
}

} // namespace Sip
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
