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
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/RemoteConference.h"
#include "elastos/droid/telecom/Connection.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::IIterator;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::Concurrent::CCopyOnWriteArrayList;
using Elastos::Utility::Concurrent::CCopyOnWriteArraySet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Telecom {

//===============================================================
// RemoteConference::
//===============================================================
CAR_INTERFACE_IMPL(RemoteConference, Object, IRemoteConference)

RemoteConference::RemoteConference()
    : mState(IConnection::STATE_NEW)
    , mCallCapabilities(0)
{
    CCopyOnWriteArraySet::New((ISet**)&mCallbacks);
    CCopyOnWriteArrayList::New((IList**)&mChildConnections);

    AutoPtr<ICollections> cls;
    CCollections::AcquireSingleton((ICollections**)&cls);
    cls->UnmodifiableList(mChildConnections, (IList**)&mUnmodifiableChildConnections);
    CArrayList::New((IList**)&mConferenceableConnections);
    cls->UnmodifiableList(mConferenceableConnections, (IList**)&mUnmodifiableConferenceableConnections);
}

ECode RemoteConference::constructor(
    /* [in] */ const String& id,
    /* [in] */ IIConnectionService* connectionService)
{
    mId = id;
    mConnectionService = connectionService;
    return NOERROR;
}

ECode RemoteConference::GetId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mId;
    return NOERROR;
}

ECode RemoteConference::SetDestroyed()
{
    AutoPtr<IIterator> it;
    mChildConnections->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IRemoteConnection> connection = IRemoteConnection::Probe(p);
        connection->SetConference(NULL);
    }

    AutoPtr<IIterator> itCB;
    mCallbacks->GetIterator((IIterator**)&itCB);
    bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        itCB->GetNext((IInterface**)&p);
        AutoPtr<IRemoteConferenceCallback> c = IRemoteConferenceCallback::Probe(p);
        c->OnDestroyed(this);
    }
    return NOERROR;
}

ECode RemoteConference::SetState(
    /* [in] */ Int32 newState)
{
    if (newState != IConnection::STATE_ACTIVE &&
            newState != IConnection::STATE_HOLDING &&
            newState != IConnection::STATE_DISCONNECTED) {
        String stateStr;
        Connection::StateToString(newState, &stateStr);
        Logger::W("RemoteConference", "Unsupported state transition for Conference call.%s",
                (const char*)stateStr);
        return NOERROR;
    }

    if (mState != newState) {
        Int32 oldState = mState;
        mState = newState;
        AutoPtr<IIterator> it;
        mCallbacks->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<IRemoteConferenceCallback> c = IRemoteConferenceCallback::Probe(p);
            c->OnStateChanged(this, oldState, newState);
        }
    }
    return NOERROR;
}

ECode RemoteConference::AddConnection(
    /* [in] */ IRemoteConnection* connection)
{
    Boolean bContain = FALSE;
    if (!(mChildConnections->Contains(connection, &bContain), bContain)) {
        mChildConnections->Add(connection);
        connection->SetConference(this);
        AutoPtr<IIterator> it;
        mCallbacks->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<IRemoteConferenceCallback> c = IRemoteConferenceCallback::Probe(p);
            c->OnConnectionAdded(this, connection);
        }
    }
    return NOERROR;
}

ECode RemoteConference::RemoveConnection(
    /* [in] */ IRemoteConnection* connection)
{
    Boolean bContain = FALSE;
    if ((mChildConnections->Contains(connection, &bContain), bContain)) {
        mChildConnections->Remove(connection);
        connection->SetConference(NULL);
        AutoPtr<IIterator> it;
        mCallbacks->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<IRemoteConferenceCallback> c = IRemoteConferenceCallback::Probe(p);
            c->OnConnectionRemoved(this, connection);
        }
    }
    return NOERROR;
}

ECode RemoteConference::SetCallCapabilities(
    /* [in] */ Int32 capabilities)
{
    if (mCallCapabilities != capabilities) {
        mCallCapabilities = capabilities;
        AutoPtr<IIterator> it;
        mCallbacks->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<IRemoteConferenceCallback> c = IRemoteConferenceCallback::Probe(p);
            c->OnCapabilitiesChanged(this, mCallCapabilities);
        }
    }
    return NOERROR;
}

ECode RemoteConference::SetConferenceableConnections(
    /* [in] */ IList* conferenceableConnections)
{
    mConferenceableConnections->Clear();
    mConferenceableConnections->AddAll(ICollection::Probe(conferenceableConnections));
    AutoPtr<IIterator> it;
    mCallbacks->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IRemoteConferenceCallback> c = IRemoteConferenceCallback::Probe(p);
        c->OnConferenceableConnectionsChanged(this, mUnmodifiableConferenceableConnections);
    }
    return NOERROR;
}

ECode RemoteConference::SetDisconnected(
    /* [in] */ IDisconnectCause* disconnectCause)
{
    if (mState != IConnection::STATE_DISCONNECTED) {
        mDisconnectCause = disconnectCause;
        SetState(IConnection::STATE_DISCONNECTED);
        AutoPtr<IIterator> it;
        mCallbacks->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<IRemoteConferenceCallback> c = IRemoteConferenceCallback::Probe(p);
            c->OnDisconnected(this, disconnectCause);
        }
    }
    return NOERROR;
}

ECode RemoteConference::GetConnections(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUnmodifiableChildConnections;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode RemoteConference::GetState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mState;
    return NOERROR;
}

ECode RemoteConference::GetCallCapabilities(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCallCapabilities;
    return NOERROR;
}

ECode RemoteConference::Disconnect()
{
    mConnectionService->Disconnect(mId);
    return NOERROR;
}

ECode RemoteConference::Separate(
    /* [in] */ IRemoteConnection* connection)
{
    Boolean bContain = FALSE;
    if ((mChildConnections->Contains(connection, &bContain), bContain)) {
        String id;
        connection->GetId(&id);
        mConnectionService->SplitFromConference(id);
    }
    return NOERROR;
}

ECode RemoteConference::Merge()
{
    mConnectionService->MergeConference(mId);
    return NOERROR;
}

ECode RemoteConference::Swap()
{
    mConnectionService->SwapConference(mId);
    return NOERROR;
}

ECode RemoteConference::Hold()
{
    mConnectionService->Hold(mId);
    return NOERROR;

}

ECode RemoteConference::Unhold()
{
    mConnectionService->Unhold(mId);
    return NOERROR;
}

ECode RemoteConference::GetDisconnectCause(
    /* [out] */ IDisconnectCause** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDisconnectCause;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode RemoteConference::PlayDtmfTone(
    /* [in] */ Char32 digit)
{
    mConnectionService->PlayDtmfTone(mId, digit);
    return NOERROR;
}

ECode RemoteConference::StopDtmfTone()
{
    mConnectionService->StopDtmfTone(mId);
    return NOERROR;
}

ECode RemoteConference::SetAudioState(
    /* [in] */ IAudioState* state)
{
    mConnectionService->OnAudioStateChanged(mId, state);
    return NOERROR;
}

ECode RemoteConference::GetConferenceableConnections(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUnmodifiableConferenceableConnections;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode RemoteConference::RegisterCallback(
    /* [in] */ IRemoteConferenceCallback* clb)
{
    mCallbacks->Add(clb);
    return NOERROR;
}

ECode RemoteConference::UnregisterCallback(
    /* [in] */ IRemoteConferenceCallback* clb)
{
    mCallbacks->Remove(clb);
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos
