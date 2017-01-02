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
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/ims/CImsCallProfile.h"
#include "elastos/droid/ims/CImsStreamMediaProfile.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhone.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneCall.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneCallTracker.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneConnection.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Ims::CImsCallProfile;
using Elastos::Droid::Ims::CImsStreamMediaProfile;
using Elastos::Droid::Ims::IImsCallProfile;
using Elastos::Droid::Ims::IImsStreamMediaProfile;
using Elastos::Droid::Telephony::IDisconnectCause;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

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
    : mRingbackTonePlayed(FALSE)
{
}

ECode ImsPhoneCall::constructor()
{
    return NOERROR;
}

ECode ImsPhoneCall::constructor(
    /* [in] */ IImsPhoneCallTracker* owner)
{
    mOwner = owner;
    return NOERROR;
}

ECode ImsPhoneCall::Dispose()
{
    // try {
    ((ImsPhoneCallTracker*)mOwner.Get())->Hangup(this);
    // } catch (CallStateException ex) {
    //     //Logger::E(LOGTAG, "dispose: unexpected error on hangup", ex);
    //     //while disposing, ignore the exception and clean the connections
    // } finally {
    Int32 size;
    mConnections->GetSize(&size);
    for(Int32 i = 0, s = size; i < s; i++) {
        AutoPtr<IInterface> obj;
        mConnections->Get(i, (IInterface**)&obj);
        AutoPtr<IImsPhoneConnection> c = IImsPhoneConnection::Probe(obj);
        Boolean b;
        ((ImsPhoneConnection*)c.Get())->OnDisconnect(IDisconnectCause::LOST_SIGNAL, &b);
    }
    // }
    return NOERROR;
}

ECode ImsPhoneCall::GetConnections(
    /* [out] */ IList/*<Connection>*/** result)
{
    VALIDATE_NOT_NULL(result);
    *result = IList::Probe(mConnections);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhoneCall::GetPhone(
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result);
    *result = IPhone::Probe(((ImsPhoneCallTracker*)mOwner.Get())->mPhone);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhoneCall::IsMultiparty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr</*TODO IImsCall*/IInterface> imsCall;
    GetImsCall((/*TODO IImsCall*/IInterface**)&imsCall);

    if (imsCall == NULL) {
        *result = FALSE;
        return NOERROR;
    }

// TODO: Need IImsCall
    // return imsCall->IsMultiparty(result);
    *result = FALSE;
    return NOERROR;
}

ECode ImsPhoneCall::Hangup()
{
    return ((ImsPhoneCallTracker*)mOwner.Get())->Hangup(this);
}

ECode ImsPhoneCall::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = StringUtils::ToString(mState);
    return NOERROR;
}

ECode ImsPhoneCall::GetExtras(
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IBundle> imsCallExtras;

    AutoPtr</*TODO IImsCall*/IInterface> call;
    GetImsCall((/*TODO IImsCall*/IInterface**)&call);

    AutoPtr<IImsCallProfile> callProfile;

    if (call != NULL) {
// TODO: Need IImsCall
        // call->GetCallProfile((IImsCallProfile**)&callProfile);
        if (callProfile != NULL) {
            imsCallExtras = ((CImsCallProfile*)callProfile.Get())->mCallExtras;
        }
    }
    if (imsCallExtras == NULL) {
        if (DBG) Logger::D(LOGTAG, "ImsCall extras are NULL.");
    }
    *result = imsCallExtras;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhoneCall::Attach(
    /* [in] */ IConnection* conn)
{
    ClearDisconnected();
    mConnections->Add(conn);
    return NOERROR;
}

ECode ImsPhoneCall::Attach(
    /* [in] */ IConnection* conn,
    /* [in] */ ICallState state)
{
    this->Attach(conn);
    mState = state;
    return NOERROR;
}

ECode ImsPhoneCall::AttachFake(
    /* [in] */ IConnection* conn,
    /* [in] */ ICallState state)
{
    return Attach(conn, state);
}

ECode ImsPhoneCall::ConnectionDisconnected(
    /* [in] */ IImsPhoneConnection* conn,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mState != ICallState_DISCONNECTED) {
        /* If only disconnected connections remain, we are disconnected*/

        Boolean hasOnlyDisconnectedConnections = TRUE;

        Int32 size;
        mConnections->GetSize(&size);
        for (Int32 i = 0, s = size; i < s; i ++) {
            AutoPtr<IInterface> obj;
            mConnections->Get(i, (IInterface**)&obj);
            AutoPtr<IConnection> c = IConnection::Probe(obj);
            ICallState state;
            c->GetState(&state);
            if (state != ICallState_DISCONNECTED) {
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

ECode ImsPhoneCall::Detach(
    /* [in] */ IImsPhoneConnection* conn)
{
    mConnections->Remove(conn);
    ClearDisconnected();
    return NOERROR;
}

ECode ImsPhoneCall::IsFull(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 size;
    mConnections->GetSize(&size);
    *result = size == ImsPhoneCallTracker::MAX_CONNECTIONS_PER_CALL;
    return NOERROR;
}

ECode ImsPhoneCall::OnHangupLocal()
{
    Int32 size;
    mConnections->GetSize(&size);

    for (Int32 i = 0, s = size; i < s; i++) {
        AutoPtr<IInterface> obj;
        mConnections->Get(i, (IInterface**)&obj);
        AutoPtr<IImsPhoneConnection> cn = IImsPhoneConnection::Probe(obj);
        ((ImsPhoneConnection*)cn.Get())->OnHangupLocal();
    }
    mState = ICallState_DISCONNECTING;
    return NOERROR;
}

ECode ImsPhoneCall::ClearDisconnected()
{
    Int32 size;
    for (Int32 i = (mConnections->GetSize(&size), size) - 1 ; i >= 0 ; i--) {
        AutoPtr<IInterface> obj;
        mConnections->Get(i, (IInterface**)&obj);
        AutoPtr<IImsPhoneConnection> cn = IImsPhoneConnection::Probe(obj);

        ICallState state;
        IConnection::Probe(cn)->GetState(&state);
        if (state == ICallState_DISCONNECTED) {
            mConnections->Remove(i);
        }
    }

    mConnections->GetSize(&size);
    if (size == 0) {
        mState = ICallState_IDLE;
    }
    return NOERROR;
}

ECode ImsPhoneCall::GetFirstConnection(
    /* [out] */ IImsPhoneConnection** result)
{
    VALIDATE_NOT_NULL(result);
    Int32 size;
    mConnections->GetSize(&size);
    if (size == 0) {
        *result = NULL;
        return NOERROR;
    }

    for (Int32 i = size - 1 ; i >= 0 ; i--) {
        AutoPtr<IInterface> obj;
        mConnections->Get(i, (IInterface**)&obj);
        AutoPtr<IImsPhoneConnection> cn = IImsPhoneConnection::Probe(obj);

        ICallState state;
        IConnection::Probe(cn)->GetState(&state);
// TODO: Need ICallState::IsAlive
        // if (state->IsAlive()) {
        //     return (ImsPhoneConnection) mConnections.get(i);
        // }
    }

    *result = NULL;
    return NOERROR;
}

ECode ImsPhoneCall::SetMute(
    /* [in] */ Boolean mute)
{
    AutoPtr</*TODO IImsCall*/IInterface> imsCall;
    AutoPtr<IImsPhoneConnection> conn;
    GetFirstConnection((IImsPhoneConnection**)&conn);
    if ( conn == NULL) {
        imsCall = NULL;
    }
    else {
        ((ImsPhoneConnection*)conn.Get())->GetImsCall((/*TODO IImsCall*/IInterface**)&imsCall);
    }

    if (imsCall != NULL) {
        // try {
// TODO: Need IImsCall
        // imsCall->SetMute(mute);
        // } catch (ImsException e) {
        //     Logger::E(LOGTAG, "setMute failed : " + e.getMessage());
        // }
    }
    return NOERROR;
}

ECode ImsPhoneCall::Merge(
    /* [in] */ IImsPhoneCall* that,
    /* [in] */ ICallState state)
{
    AutoPtr<ArrayOf<IImsPhoneConnection*> > cc;
    Int32 size;
    ((ImsPhoneCall*)that)->mConnections->GetSize(&size);
    AutoPtr<ArrayOf<IInterface* > > array = ArrayOf<IInterface* >::Alloc(size);
    AutoPtr<ArrayOf<IInterface *> > outArray;
    ((ImsPhoneCall*)that)->mConnections->ToArray(array, (ArrayOf<IInterface *>**)&outArray);

    for (Int32 i = 0; i < outArray->GetLength(); i++) {
        AutoPtr<IImsPhoneConnection> c = IImsPhoneConnection::Probe((*outArray)[i]);
        Boolean b;
        ((ImsPhoneConnection*)c.Get())->Update(NULL, state, &b);
    }
    return NOERROR;
}

ECode ImsPhoneCall::GetImsCall(
    /* [out] */ /*TODO IImsCall*/IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IImsPhoneConnection> conn;
    GetFirstConnection((IImsPhoneConnection**)&conn);
    if (conn == NULL) {
        *result = NULL;
    }
    else {
        ((ImsPhoneConnection*)conn.Get())->GetImsCall(result);
    }
    return NOERROR;
}

Boolean ImsPhoneCall::IsLocalTone(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall)
{
    AutoPtr<IImsCallProfile> profile;
// TODO: Need IImsCall
    // imsCall->GetCallProfile((IImsCallProfile**)&profile);
    if ((imsCall == NULL) || (profile == NULL)
            || ((CImsCallProfile*)profile.Get())->mMediaProfile == NULL) {
        return FALSE;
    }

    AutoPtr<IImsStreamMediaProfile> mediaProfile = ((CImsCallProfile*)profile.Get())->mMediaProfile;

    return (((CImsStreamMediaProfile*)mediaProfile.Get())->mAudioDirection ==
            IImsStreamMediaProfile::DIRECTION_INACTIVE)
            ? TRUE : FALSE;
}

ECode ImsPhoneCall::Update(
    /* [in] */ IImsPhoneConnection* conn,
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
    /* [in] */ ICallState state,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    ICallState newState = state;
    Boolean changed = FALSE;

    //ImsCall.Listener.onCallProgressing can be invoked several times
    //and ringback tone mode can be changed during the call setup procedure
    if (state == ICallState_ALERTING) {
        if (mRingbackTonePlayed && !IsLocalTone(imsCall)) {
            ((ImsPhone*)((ImsPhoneCallTracker*)mOwner.Get())->mPhone.Get())->StopRingbackTone();
            mRingbackTonePlayed = FALSE;
        }
        else if (!mRingbackTonePlayed && IsLocalTone(imsCall)) {
            ((ImsPhone*)((ImsPhoneCallTracker*)mOwner.Get())->mPhone.Get())->StartRingbackTone();
            mRingbackTonePlayed = TRUE;
        }
    }
    else {
        if (mRingbackTonePlayed) {
            ((ImsPhone*)((ImsPhoneCallTracker*)mOwner.Get())->mPhone.Get())->StopRingbackTone();
            mRingbackTonePlayed = FALSE;
        }
    }

    if ((newState != mState) && (state != ICallState_DISCONNECTED)) {
        mState = newState;
        changed = TRUE;
    }
    else if (state == ICallState_DISCONNECTED) {
        changed = TRUE;
    }

    *result = changed;
    return NOERROR;
}

ECode ImsPhoneCall::GetHandoverConnection(
    /* [out] */ IImsPhoneConnection** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IConnection> c;
    GetEarliestConnection((IConnection**)&c);
    *result = IImsPhoneConnection::Probe(c);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ImsPhoneCall::SwitchWith(
    /* [in] */ IImsPhoneCall* that)
{
    AutoLock lock(this); // synchronized (ImsPhoneCall.class)
    AutoPtr<IImsPhoneCall> tmp = new ImsPhoneCall();
    ((ImsPhoneCall*)tmp.Get())->TakeOver(this);
    this->TakeOver(that);
    ((ImsPhoneCall*)that)->TakeOver(tmp);

    return NOERROR;
}

void ImsPhoneCall::TakeOver(
    /* [in] */ IImsPhoneCall* that)
{
    AutoPtr<ImsPhoneCall> call = (ImsPhoneCall*)that;
    mConnections = call->mConnections;
    mState = call->mState;

    Int32 size;
    mConnections->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mConnections->Get(i, (IInterface**)&obj);
        AutoPtr<IConnection> c = IConnection::Probe(obj);

        ((ImsPhoneConnection*)IImsPhoneConnection::Probe(c))->ChangeParent(this);
    }
}

} // namespace Imsphone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
