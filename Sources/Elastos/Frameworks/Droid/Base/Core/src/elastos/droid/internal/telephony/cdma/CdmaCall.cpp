#include "Elastos.Droid.Internal.h"

#include "elastos/droid/internal/telephony/cdma/CdmaCall.h"
#include "elastos/droid/internal/telephony/cdma/CdmaCallTracker.h"
#include "elastos/droid/internal/telephony/cdma/CdmaConnection.h"
#include "elastos/droid/internal/telephony/DriverCall.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Internal::Telephony::DriverCall;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                               CdmaCall
//=====================================================================
CAR_INTERFACE_IMPL(CdmaCall, Call, ICdmaCall);

CdmaCall::CdmaCall()
{
}

ECode CdmaCall::constructor(
    /* [in] */ ICdmaCallTracker* owner)
{
    mOwner = owner;
    return NOERROR;
}

ECode CdmaCall::Dispose()
{
    return NOERROR;
}

ECode CdmaCall::GetConnections(
    /* [out] */ IList** result)// IConnection
{
    VALIDATE_NOT_NULL(result);
    // FIXME should return Collections.unmodifiableList();
    *result = IList::Probe(mConnections);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CdmaCall::GetPhone(
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result);
    *result = IPhone::Probe(((CdmaCallTracker*)mOwner.Get())->mPhone);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CdmaCall::IsMultiparty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 size;
    mConnections->GetSize(&size);
    *result = size > 1;
    return NOERROR;
}

ECode CdmaCall::Hangup()
{
    return ((CdmaCallTracker*)mOwner.Get())->Hangup(this);
}

ECode CdmaCall::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = StringUtils::ToString(mState);
    return NOERROR;
}

ECode CdmaCall::Attach(
    /* [in] */ IConnection* conn,
    /* [in] */ IDriverCall* dc)
{
    mConnections->Add(conn);

    mState = StateFromDCState(((DriverCall*)(dc))->mState);
    return NOERROR;
}

ECode CdmaCall::AttachFake(
    /* [in] */ IConnection* conn,
    /* [in] */ ICallState state)
{
    mConnections->Add(conn);

    mState = state;
    return NOERROR;
}

ECode CdmaCall::ConnectionDisconnected(
    /* [in] */ ICdmaConnection* conn,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mState != ICallState_DISCONNECTED) {
        /* If only disconnected connections remain, we are disconnected*/

        Boolean hasOnlyDisconnectedConnections = TRUE;

        Int32 size;
        mConnections->GetSize(&size);
        for (Int32 i = 0; i < size; i ++) {
            AutoPtr<IInterface> obj;
            mConnections->Get(i, (IInterface**)&obj);
            ICallState state;
            IConnection::Probe(obj)->GetState(&state);
            if (state != ICallState_DISCONNECTED) {
                hasOnlyDisconnectedConnections = FALSE;
                break;
            }
        }

        if (hasOnlyDisconnectedConnections) {
            mState = ICallState_DISCONNECTED;
            *result = TRUE;
        }
    }

    *result = FALSE;
    return NOERROR;
}

ECode CdmaCall::Detach(
    /* [in] */ ICdmaConnection* conn)
{
    mConnections->Remove(conn);

    Int32 size;
    mConnections->GetSize(&size);
    if (size == 0) {
        mState = ICallState_IDLE;
    }
    return NOERROR;
}

ECode CdmaCall::Update(
    /* [in] */ ICdmaConnection* conn,
    /* [in] */ IDriverCall* dc,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    ICallState newState;
    Boolean changed = FALSE;

    newState = StateFromDCState(((DriverCall*)(dc))->mState);

    if (newState != mState) {
        mState = newState;
        changed = TRUE;
    }

    *result = changed;
    return NOERROR;
}

ECode CdmaCall::IsFull(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 size;
    mConnections->GetSize(&size);
    *result = size == CdmaCallTracker::MAX_CONNECTIONS_PER_CALL;
    return NOERROR;
}

ECode CdmaCall::OnHangupLocal()
{
    Int32 size;
    mConnections->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mConnections->Get(i, (IInterface**)&obj);
        AutoPtr<ICdmaConnection> cn = ICdmaConnection::Probe(obj);

        ((CdmaConnection*)cn.Get())->OnHangupLocal();
    }
    mState = ICallState_DISCONNECTING;
    return NOERROR;
}

ECode CdmaCall::ClearDisconnected()
{
    Int32 size;
    mConnections->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mConnections->Get(i, (IInterface**)&obj);
        AutoPtr<ICdmaConnection> cn = ICdmaConnection::Probe(obj);

        ICallState state;
        IConnection::Probe(cn)->GetState(&state);
        if (state == ICallState_DISCONNECTED) {
            mConnections->Remove(i);
        }
    }

    if (size == 0) {
        mState = ICallState_IDLE;
    }
    return NOERROR;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
