
#include "elastos/droid/internal/telephony/Call.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::CArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const String Call::TAG("Call");
CAR_INTERFACE_IMPL(Call, Object, ICall)
Call::Call()
    : mState(ICallState_IDLE)
    , mIsGeneric(FALSE)
{
    CArrayList::New((IArrayList**)&mConnections);
}

Call::~Call()
{}

ICallState Call::StateFromDCState(
    /* [in] */ IDriverCallState dcState)
{
    switch (dcState) {
        case DriverCallState_ACTIVE:        return ICallState_ACTIVE;
        case DriverCallState_HOLDING:       return ICallState_HOLDING;
        case DriverCallState_DIALING:       return ICallState_DIALING;
        case DriverCallState_ALERTING:      return ICallState_ALERTING;
        case DriverCallState_INCOMING:      return ICallState_INCOMING;
        case DriverCallState_WAITING:       return ICallState_WAITING;
        default: {
            assert(0 && "illegal call state");
            // throw new RuntimeException ("illegal call state:" + dcState);
        }
    }
}

/* Instance Methods */

/**
 * hasConnection
 *
 * @param c a Connection object
 * @return true if the call contains the connection object passed in
 */
ECode Call::HasConnection(
    /* [in] */ IConnection* c,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICall> call;
    c->GetCall((ICall**)&call);
    *result = call.Get() == this;
    return NOERROR;
}

/**
 * hasConnections
 * @return true if the call contains one or more connections
 */
ECode Call::HasConnections(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IList> connections;
    GetConnections((IList**)&connections);

    if (connections == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 size = 0;
    connections->GetSize(&size);
    *result = size > 0;
    return NOERROR;
}

/**
 * getState
 * @return state of class call
 */
ECode Call::GetState(
    /* [out] */ ICallState* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mState;
    return NOERROR;
}

/**
* getExtras
* @return Call Extras. Subclasses of Call that support call extras need
*         to override this method to return the extras.
*/
ECode Call::GetExtras(
    /* [out] */ IBundle** extras)
{
    VALIDATE_NOT_NULL(extras);
    *extras = NULL;
    return NOERROR;
}

/**
 * isIdle
 *
 * FIXME rename
 * @return true if the call contains only disconnected connections (if any)
 */
ECode Call::IsIdle(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    ICallState state;
    GetState(&state);
    *result = (state == ICallState_IDLE || state == ICallState_DISCONNECTED || state == ICallState_DISCONNECTING);
    return NOERROR;
}

/**
 * Returns the Connection associated with this Call that was created
 * first, or NULL if there are no Connections in this Call
 */
ECode Call::GetEarliestConnection(
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IList> l;
    Int64 time = Elastos::Core::Math::INT64_MAX_VALUE;
    AutoPtr<IConnection> c;
    AutoPtr<IConnection> earliest;

    GetConnections((IList**)&l);

    Int32 size = 0;
    l->GetSize(&size);
    if (size == 0) {
        *result = NULL;
        return NOERROR;
    }

    for (Int32 i = 0, s = size ; i < s ; i++) {
        AutoPtr<IInterface> o;
        l->Get(i, (IInterface**)&o);
        c = IConnection::Probe(o);
        Int64 t = 0;

        c->GetCreateTime(&t);

        if (t < time) {
            earliest = c;
            time = t;
        }
    }

    *result = earliest;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode Call::GetEarliestCreateTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IList> l;
    Int64 time = Elastos::Core::Math::INT64_MAX_VALUE;

    GetConnections((IList**)&l);

    Int32 size = 0;
    l->GetSize(&size);
    if (size == 0) {
        *result = 0;
        return NOERROR;
    }

    for (Int32 i = 0, s = size ; i < s ; i++) {
        AutoPtr<IInterface> o;
        l->Get(i, (IInterface**)&o);
        AutoPtr<IConnection> c = IConnection::Probe(o);
        Int64 t = 0;

        c->GetCreateTime(&t);

        time = t < time ? t : time;
    }

    *result = time;
    return NOERROR;
}

ECode Call::GetEarliestConnectTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    Int64 time = Elastos::Core::Math::INT64_MAX_VALUE;
    AutoPtr<IList> l;
    GetConnections((IList**)&l);

    Int32 size = 0;
    l->GetSize(&size);
    if (size == 0) {
        *result = 0;
        return NOERROR;
    }

    for (Int32 i = 0, s = size ; i < s ; i++) {
        AutoPtr<IInterface> o;
        l->Get(i, (IInterface**)&o);
        AutoPtr<IConnection> c = IConnection::Probe(o);
        Int64 t = 0;

        c->GetConnectTime(&t);

        time = t < time ? t : time;
    }

    *result = time;
    return NOERROR;
}


ECode Call::IsDialingOrAlerting(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    ICallState state;
    GetState(&state);
    *result = (state == ICallState_DIALING || state == ICallState_ALERTING);
    return NOERROR;
}

ECode Call::IsRinging(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    ICallState state;
    GetState(&state);
    *result = (state == ICallState_DIALING || state == ICallState_ALERTING);
    return NOERROR;
}

/**
 * Returns the Connection associated with this Call that was created
 * last, or NULL if there are no Connections in this Call
 */
ECode Call::GetLatestConnection(
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IList> l;
    GetConnections((IList**)&l);
    Int32 size = 0;
    l->GetSize(&size);
    if (size == 0) {
        *result = NULL;
        return NOERROR;
    }

    Int64 time = 0;
    AutoPtr<IConnection> latest;
    for (Int32 i = 0, s = size ; i < s ; i++) {
        AutoPtr<IInterface> o;
        l->Get(i, (IInterface**)&o);
        AutoPtr<IConnection> c = IConnection::Probe(o);
        Int64 t = 0;
        c->GetCreateTime(&t);

        if (t > time) {
            latest = c;
            time = t;
        }
    }

    *result = latest;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/**
 * To indicate if the connection information is accurate
 * or not. FALSE means accurate. Only used for CDMA.
 */
ECode Call::IsGeneric(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mIsGeneric;
}

/**
 * Set the generic instance variable
 */
ECode Call::SetGeneric(
    /* [in] */ Boolean generic)
{
    mIsGeneric = generic;
    return NOERROR;
}

/**
 * Hangup call if it is alive
 */
ECode Call::HangupIfAlive()
{
    ICallState state;
    GetState(&state);
    Boolean isAlive = !(state == ICallState_IDLE || state == ICallState_DISCONNECTED || state == ICallState_DISCONNECTING);
    if (isAlive) {
        // try {
        if (FAILED(Hangup())) {
            Logger::W(TAG, " hangupIfActive: caught ");
        }
        // } catch (CallStateException ex) {
        //     Rlog.w(TAG, " hangupIfActive: caught " + ex);
        // }
    }
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
