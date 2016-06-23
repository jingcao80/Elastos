#include "elastos/droid/internal/telephony/gsm/CGsmCall.h"
#include "elastos/droid/internal/telephony/gsm/CGsmCallTracker.h"
#include "elastos/droid/internal/telephony/gsm/CGsmConnection.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

CAR_INTERFACE_IMPL(CGsmCall, Call, IGsmCall)

CAR_OBJECT_IMPL(CGsmCall)

CGsmCall::CGsmCall()
{
}

ECode CGsmCall::constructor()
{
    return NOERROR;
}

ECode CGsmCall::constructor(
    /* [in] */ IGsmCallTracker* owner)
{
    mOwner = owner;
    return NOERROR;
}

ECode CGsmCall::Dispose()
{
    return NOERROR;
}

ECode CGsmCall::GetConnections(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    // FIXME should return Collections.unmodifiableList();
    *result = IList::Probe(mConnections);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CGsmCall::GetPhone(
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result)
    *result = IPhone::Probe(((CGsmCallTracker*)mOwner.Get())->mPhone);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CGsmCall::IsMultiparty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 size;
    mConnections->GetSize(&size);
    *result = size > 1;
    return NOERROR;
}
ECode CGsmCall::Hangup()
{
    return ((CGsmCallTracker*)mOwner.Get())->Hangup(this);
}

ECode CGsmCall::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // return mState.ToString(result);
    *result = StringUtils::ToString(mState);
    return NOERROR;
}

ECode CGsmCall::Attach(
    /* [in] */ IConnection* conn,
    /* [in] */ IDriverCall* dc)
{
    mConnections->Add(conn);
    mState = StateFromDCState(((DriverCall*)dc)->mState);
    return NOERROR;
}

ECode CGsmCall::AttachFake(
    /* [in] */ IConnection* conn,
    /* [in] */ ICallState state)
{
    mConnections->Add(conn);
    mState = state;
    return NOERROR;
}

ECode CGsmCall::ConnectionDisconnected(
    /* [in] */ IGsmConnection* conn,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mState != ICallState_DISCONNECTED) {
        /* If only disconnected connections remain, we are disconnected*/

        Boolean hasOnlyDisconnectedConnections = TRUE;

        Int32 size;
        mConnections->GetSize(&size);
        for (int i = 0, s = size; i < s; i ++) {
            AutoPtr<IInterface> obj;
            mConnections->Get(i, (IInterface**)&obj);
            AutoPtr<IGsmConnection> cn = IGsmConnection::Probe(obj);

            ICallState cs;
            ((CGsmConnection*)cn.Get())->GetState(&cs);

            if (cs != ICallState_DISCONNECTED) {
                hasOnlyDisconnectedConnections = FALSE;
                break;
            }
        }

        if (hasOnlyDisconnectedConnections) {
            mState = ICallState_DISCONNECTED;
            *result = TRUE;
            return NOERROR;
        }
    }

    *result = FALSE;
    return NOERROR;
}

ECode CGsmCall::Detach(
    /* [in] */ IGsmConnection* conn)
{
    mConnections->Remove(conn);

    Int32 size;
    mConnections->GetSize(&size);
    if (size == 0) {
        mState = ICallState_IDLE;
    }
    return NOERROR;
}

ECode CGsmCall::Update (
    /* [in] */ IGsmConnection* conn,
    /* [in] */ IDriverCall* dc,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    ICallState newState;
    Boolean changed = FALSE;

    newState = StateFromDCState(((DriverCall*)dc)->mState);

    if (newState != mState) {
        mState = newState;
        changed = TRUE;
    }

    *result = changed;
    return NOERROR;
}

ECode CGsmCall::IsFull(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 size;
    mConnections->GetSize(&size);
    *result = size == CGsmCallTracker::MAX_CONNECTIONS_PER_CALL;
    return NOERROR;
}

ECode CGsmCall::OnHangupLocal()
{
    Int32 size;
    mConnections->GetSize(&size);
    for (Int32 i = 0, s = size; i < s; i++) {
        AutoPtr<IInterface> obj;
        mConnections->Get(i, (IInterface**)&obj);
        AutoPtr<IGsmConnection> cn = IGsmConnection::Probe(obj);

        ((CGsmConnection*)cn.Get())->OnHangupLocal();
    }
    mState = ICallState_DISCONNECTING;
    return NOERROR;
}

ECode CGsmCall::ClearDisconnected()
{
    Int32 size;
    mConnections->GetSize(&size);
    for (Int32 i = size - 1 ; i >= 0 ; i--) {
        AutoPtr<IInterface> obj;
        mConnections->Get(i, (IInterface**)&obj);
        AutoPtr<IGsmConnection> cn = IGsmConnection::Probe(obj);

        ICallState cs;
        ((CGsmConnection*)cn.Get())->GetState(&cs);
        if (cs == ICallState_DISCONNECTED) {
            mConnections->Remove(i);
        }
    }

    mConnections->GetSize(&size);
    if (size == 0) {
        mState = ICallState_IDLE;
    }
    return NOERROR;
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos