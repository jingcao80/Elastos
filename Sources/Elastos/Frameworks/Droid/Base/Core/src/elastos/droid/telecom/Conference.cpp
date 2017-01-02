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

#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/Conference.h"
#include "elastos/droid/telecom/CDisconnectCause.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Concurrent::CCopyOnWriteArraySet;
using Elastos::Utility::Concurrent::CCopyOnWriteArrayList;

namespace Elastos {
namespace Droid {
namespace Telecom {

//===============================================================
// Conference::ConnectionListener::
//===============================================================
Conference::ConnectionListener::ConnectionListener(
    /* [in] */ Conference* host)
    : mHost(host)
{}

ECode Conference::ConnectionListener::OnDestroyed(
    /* [in] */ IConnection* c)
{
    Boolean bRm = FALSE;
    mHost->mConferenceableConnections->Remove(c, &bRm);
    if (bRm) {
        mHost->FireOnConferenceableConnectionsChanged();
    }
    return NOERROR;
}

//===============================================================
// Conference::
//===============================================================
CAR_INTERFACE_IMPL(Conference, Object, IConference)

Conference::Conference()
{
    CCopyOnWriteArraySet::New((ISet**)&mListeners);
    CCopyOnWriteArrayList::New((IList**)&mChildConnections);
    AutoPtr<ICollections> cls;
    CCollections::AcquireSingleton((ICollections**)&cls);
    cls->UnmodifiableList(mChildConnections, (IList**)&mUnmodifiableChildConnections);
    CArrayList::New((IList**)&mConferenceableConnections);
    cls->UnmodifiableList(mConferenceableConnections, (IList**)&mUnmodifiableConferenceableConnections);

    mState = IConnection::STATE_NEW;

    mConnectionDeathListener = new ConnectionListener(this);
}

ECode Conference::constructor(
    /* [in] */ IPhoneAccountHandle* phoneAccount)
{
    mPhoneAccount = phoneAccount;
    return NOERROR;
}

ECode Conference::GetPhoneAccountHandle(
    /* [out] */ IPhoneAccountHandle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPhoneAccount;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Conference::GetConnections(
    /* [out] */ IList** res)
{
    VALIDATE_NOT_NULL(res)
    *res = mUnmodifiableChildConnections;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode Conference::GetState(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    *res = mState;
    return NOERROR;
}

ECode Conference::GetCapabilities(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    *res = mCapabilities;
    return NOERROR;
}

ECode Conference::GetAudioState(
    /* [out] */ IAudioState** res)
{
    VALIDATE_NOT_NULL(res)
    *res = mAudioState;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode Conference::OnDisconnect()
{
    return NOERROR;
}

ECode Conference::OnSeparate(
    /* [in] */ IConnection* connection)
{
    return NOERROR;
}

ECode Conference::OnMerge(
    /* [in] */ IConnection* connection)
{
    return NOERROR;
}

ECode Conference::OnHold()
{
    return NOERROR;
}

ECode Conference::OnUnhold()
{
    return NOERROR;
}

ECode Conference::OnMerge()
{
    return NOERROR;
}

ECode Conference::OnSwap()
{
    return NOERROR;
}

ECode Conference::OnPlayDtmfTone(
    /* [in] */ Char32 c)
{
    return NOERROR;
}

ECode Conference::OnStopDtmfTone()
{
    return NOERROR;
}

ECode Conference::OnAudioStateChanged(
    /* [in] */ IAudioState* state)
{
    return NOERROR;
}

ECode Conference::SetOnHold()
{
    SetState(IConnection::STATE_HOLDING);
    return NOERROR;
}

ECode Conference::SetActive()
{
    SetState(IConnection::STATE_ACTIVE);
    return NOERROR;
}

ECode Conference::SetDisconnected(
    /* [in] */ IDisconnectCause* disconnectCause)
{
    mDisconnectCause = disconnectCause;;
    SetState(IConnection::STATE_DISCONNECTED);
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IConferenceListener> l = IConferenceListener::Probe(p);
        l->OnDisconnected(this, mDisconnectCause);
    }
    return NOERROR;
}

ECode Conference::SetCapabilities(
    /* [in] */ Int32 capabilities)
{
    if (capabilities != mCapabilities) {
        mCapabilities = capabilities;

        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<IConferenceListener> l = IConferenceListener::Probe(p);
            l->OnCapabilitiesChanged(this, mCapabilities);
        }
    }
    return NOERROR;
}

ECode Conference::AddConnection(
    /* [in] */ IConnection* connection,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Boolean bContain = FALSE;
    if (connection != NULL && !(mChildConnections->Contains(connection, &bContain), bContain)) {
        Boolean bSet = FALSE;
        if ((connection->SetConference(this, &bSet), bSet)) {
            mChildConnections->Add(connection);
            AutoPtr<IIterator> it;
            mListeners->GetIterator((IIterator**)&it);
            Boolean bHasNxt = FALSE;
            while ((it->HasNext(&bHasNxt), bHasNxt)) {
                AutoPtr<IInterface> p;
                it->GetNext((IInterface**)&p);
                AutoPtr<IConferenceListener> l = IConferenceListener::Probe(p);
                l->OnConnectionAdded(this, connection);
            }
            *res = TRUE;
            return NOERROR;
        }
    }
    *res = FALSE;
    return NOERROR;
}

ECode Conference::RemoveConnection(
    /* [in] */ IConnection* connection)
{
    Logger::D("Conference", "removing %p from %p", connection, mChildConnections.Get());
    if (connection != NULL && mChildConnections->Remove(connection)) {
        connection->ResetConference();
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<IConferenceListener> l = IConferenceListener::Probe(p);
            l->OnConnectionRemoved(this, connection);
        }
    }
    return NOERROR;
}

ECode Conference::SetConferenceableConnections(
    /* [in] */ IList* conferenceableConnections)
{
    ClearConferenceableList();
    AutoPtr<IIterator> it;
    conferenceableConnections->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IConnection> c = IConnection::Probe(p);
        // If statement checks for duplicates in input. It makes it N^2 but we're dealing with a
        // small amount of items here.
        Boolean bContain = FALSE;
        if (!(mConferenceableConnections->Contains(c, &bContain), bContain)) {
            c->AddConnectionListener(mConnectionDeathListener);
            mConferenceableConnections->Add(c);
        }
    }
    FireOnConferenceableConnectionsChanged();
    return NOERROR;
}

void Conference::FireOnConferenceableConnectionsChanged()
{
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IConferenceListener> l = IConferenceListener::Probe(p);
        AutoPtr<IList> list;
        GetConferenceableConnections((IList**)&list);
        l->OnConferenceableConnectionsChanged(this, list);
    }
}

ECode Conference::GetConferenceableConnections(
    /* [out] */ IList** res)
{
    VALIDATE_NOT_NULL(res)
    *res = mUnmodifiableConferenceableConnections;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode Conference::Destroy()
{
    Logger::D("Conference", "destroying conference : %p", this);
    // Tear down the children.
    AutoPtr<IIterator> itConnection;
    mChildConnections->GetIterator((IIterator**)&itConnection);
    Boolean bHasNxt = FALSE;
    while ((itConnection->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        itConnection->GetNext((IInterface**)&p);
        AutoPtr<IConnection> connection = IConnection::Probe(p);
        Logger::D("Conference", "removing connection %p", connection.Get());
        RemoveConnection(connection);
    }

    // If not yet disconnected, set the conference call as disconnected first.
    if (mState != IConnection::STATE_DISCONNECTED) {
        Logger::D("Conference", "setting to disconnected");
        assert(0 && "TODO");
        AutoPtr<IDisconnectCause> dc;
        CDisconnectCause::New(IDisconnectCause::LOCAL, (IDisconnectCause**)&dc);
        SetDisconnected(dc);
    }

    // ...and notify.
    AutoPtr<IIterator> itListener;
    mListeners->GetIterator((IIterator**)&itListener);
    bHasNxt = FALSE;
    while ((itListener->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        itListener->GetNext((IInterface**)&p);
        AutoPtr<IConferenceListener> l = IConferenceListener::Probe(p);
        l->OnDestroyed(this);
    }
    return NOERROR;
}

ECode Conference::AddListener(
    /* [in] */ IConferenceListener* listener)
{
    mListeners->Add(listener);
    return NOERROR;
}

ECode Conference::RemoveListener(
    /* [in] */ IConferenceListener* listener)
{
    mListeners->Remove(listener);
    return NOERROR;
}

ECode Conference::SetAudioState(
    /* [in] */ IAudioState* state)
{
    Logger::D("Conference", "setAudioState %p", state);
    mAudioState = state;
    OnAudioStateChanged(state);
    return NOERROR;
}

void Conference::SetState(
    /* [in] */ Int32 newState)
{
    if (newState != IConnection::STATE_ACTIVE &&
            newState != IConnection::STATE_HOLDING &&
            newState != IConnection::STATE_DISCONNECTED) {
        // Logger::W(this, "Unsupported state transition for Conference call.",
        //         Connection.stateToString(newState));
        return;
    }

    if (mState != newState) {
        Int32 oldState = mState;
        mState = newState;
        AutoPtr<IIterator> itListener;
        mListeners->GetIterator((IIterator**)&itListener);
        Boolean bHasNxt = FALSE;
        while ((itListener->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            itListener->GetNext((IInterface**)&p);
            AutoPtr<IConferenceListener> l = IConferenceListener::Probe(p);
            l->OnStateChanged(this, oldState, newState);
        }
    }
}

void Conference::ClearConferenceableList()
{
    AutoPtr<IIterator> it;
    mConferenceableConnections->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IConnection> c = IConnection::Probe(p);
        c->RemoveConnectionListener(mConnectionDeathListener);
    }
    mConferenceableConnections->Clear();
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos