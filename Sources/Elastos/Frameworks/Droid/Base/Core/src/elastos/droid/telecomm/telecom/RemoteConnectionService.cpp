
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/RemoteConnectionService.h"
#include "elastos/droid/telecomm/telecom/RemoteConnection.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Telecomm::Internal::EIID_IIConnectionServiceAdapter;
using Elastos::Core::CString;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollection;
using Elastos::Utility::IUUID;
using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::CUUIDHelper;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

//===============================================================
// RemoteConnectionService::MyConnectionServiceAdapter::RemoteConferenceCallback::
//===============================================================
CAR_INTERFACE_IMPL(RemoteConnectionService::MyConnectionServiceAdapter::RemoteConferenceCallback, Object, IRemoteConferenceCallback)

RemoteConnectionService::MyConnectionServiceAdapter::RemoteConferenceCallback::RemoteConferenceCallback(
    /* [in] */ const String& callId,
    /* [in] */ MyConnectionServiceAdapter* host)
    : mCallId(callId)
    , mHost(host)
{
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::RemoteConferenceCallback::OnStateChanged(
    /* [in] */ IRemoteConference* conference,
    /* [in] */ Int32 oldState,
    /* [in] */ Int32 newState)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::RemoteConferenceCallback::OnDisconnected(
    /* [in] */ IRemoteConference* conference,
    /* [in] */ IDisconnectCause* disconnectCause)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::RemoteConferenceCallback::OnConnectionAdded(
    /* [in] */ IRemoteConference* conference,
    /* [in] */ IRemoteConnection* connection)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::RemoteConferenceCallback::OnConnectionRemoved(
    /* [in] */ IRemoteConference* conference,
    /* [in] */ IRemoteConnection* connection)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::RemoteConferenceCallback::OnCapabilitiesChanged(
    /* [in] */ IRemoteConference* conference,
    /* [in] */ Int32 capabilities)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::RemoteConferenceCallback::OnConferenceableConnectionsChanged(
    /* [in] */ IRemoteConference* conference,
    /* [in] */ IList* conferenceableConnections)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::RemoteConferenceCallback::OnDestroyed(
    /* [in] */ IRemoteConference* conference)
{
    AutoPtr<ICharSequence> pId;
    CString::New(mCallId, (ICharSequence**)&pId);
    mHost->mHost->mConferenceById->Remove(pId);
    mHost->mHost->MaybeDisconnectAdapter();
    return NOERROR;
}

//===============================================================
// RemoteConnectionService::MyConnectionServiceAdapter::
//===============================================================
CAR_INTERFACE_IMPL(RemoteConnectionService::MyConnectionServiceAdapter, Object, IIConnectionServiceAdapter)

RemoteConnectionService::MyConnectionServiceAdapter::MyConnectionServiceAdapter(
    /* [in] */ RemoteConnectionService* host)
    : mHost(host)
{}

ECode RemoteConnectionService::MyConnectionServiceAdapter::HandleCreateConnectionComplete(
    /* [in] */ const String& id,
    /* [in] */ IConnectionRequest* request,
    /* [in] */ IParcelableConnection* parcel)
{
    AutoPtr<IRemoteConnection> connection =
            mHost->FindConnectionForAction(id, String("handleCreateConnectionSuccessful"));
    Boolean bContain = FALSE;
    if (connection != NULL_CONNECTION &&
        (mHost->mPendingConnections->Contains(connection, &bContain), bContain)) {
        mHost->mPendingConnections->Remove(connection);
        // Unconditionally initialize the connection ...
        Int32 capabilities = 0;
        parcel->GetCapabilities(&capabilities);
        connection->SetCallCapabilities(capabilities);
        AutoPtr<IUri> handle;
        parcel->GetHandle((IUri**)&handle);
        Int32 handlePresentation = 0;
        parcel->GetHandlePresentation(&handlePresentation);
        connection->SetAddress(handle, handlePresentation);
        String disName;
        parcel->GetCallerDisplayName(&disName);
        Int32 namePresentation = 0;
        parcel->GetCallerDisplayNamePresentation(&namePresentation);
        connection->SetCallerDisplayName(
                disName,
                namePresentation);
        // Set state after handle so that the client can identify the connection.
        Int32 state = 0;
        parcel->GetState(&state);
        if (state == IConnection::STATE_DISCONNECTED) {
            AutoPtr<IDisconnectCause> dc;
            parcel->GetDisconnectCause((IDisconnectCause**)&dc);
            connection->SetDisconnected(dc);
        }
        else {
            connection->SetState(state);
        }
        AutoPtr<IList> conferenceable;
        CArrayList::New((IList**)&conferenceable);
        AutoPtr<IList> ids;
        parcel->GetConferenceableConnectionIds((IList**)&ids);
        AutoPtr<IIterator> it;
        ids->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> confId;
            it->GetNext((IInterface**)&confId);
            Boolean bContain = FALSE;
            if ((mHost->mConnectionById->ContainsKey(confId, &bContain), bContain)) {
                AutoPtr<IInterface> p;
                mHost->mConnectionById->Get(confId, (IInterface**)&p);
                conferenceable->Add(p);
            }
        }
        connection->SetConferenceableConnections(conferenceable);
        Int32 videoState = 0;
        parcel->GetVideoState(&videoState);
        connection->SetVideoState(videoState);
        Int32 connState = 0;
        connection->GetState(&connState);
        if (connState == IConnection::STATE_DISCONNECTED) {
            // ... then, if it was created in a disconnected state, that indicates
            // failure on the providing end, so immediately mark it destroyed
            connection->SetDestroyed();
        }
    }
    return NOERROR;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::SetActive(
    /* [in] */ const String& callId)
{
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    Boolean bContain = FALSE;
    if ((mHost->mConnectionById->ContainsKey(pId, &bContain), bContain)) {
        AutoPtr<IRemoteConnection> conn = mHost->FindConnectionForAction(callId, String("setActive"));
        conn->SetState(IConnection::STATE_ACTIVE);
    }
    else {
        AutoPtr<IRemoteConference> conf = mHost->FindConferenceForAction(callId, String("setActive"));
        conf->SetState(IConnection::STATE_ACTIVE);
    }
    return NOERROR;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::SetRinging(
    /* [in] */ const String& callId)
{
    AutoPtr<IRemoteConnection> conn = mHost->FindConnectionForAction(callId, String("setRinging"));
    conn->SetState(IConnection::STATE_RINGING);
    return NOERROR;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::SetDialing(
    /* [in] */ const String& callId)
{
    AutoPtr<IRemoteConnection> conn = mHost->FindConnectionForAction(callId, String("setDialing"));
    conn->SetState(IConnection::STATE_DIALING);
    return NOERROR;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::SetDisconnected(
    /* [in] */ const String& callId,
    /* [in] */ IDisconnectCause* disconnectCause)
{
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    Boolean bContain = FALSE;
    if ((mHost->mConnectionById->ContainsKey(pId, &bContain), bContain)) {
        AutoPtr<IRemoteConnection> conn = mHost->FindConnectionForAction(callId, String("setDisconnected"));
        conn->SetDisconnected(disconnectCause);
    }
    else {
        AutoPtr<IRemoteConference> conf = mHost->FindConferenceForAction(callId, String("setDisconnected"));
        conf->SetDisconnected(disconnectCause);
    }
    return NOERROR;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::SetOnHold(
    /* [in] */ const String& callId)
{
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    Boolean bContain = FALSE;
    if ((mHost->mConnectionById->ContainsKey(pId, &bContain), bContain)) {
        AutoPtr<IRemoteConnection> conn = mHost->FindConnectionForAction(callId, String("setOnHold"));
        conn->SetState(IConnection::STATE_HOLDING);
    }
    else {
        AutoPtr<IRemoteConference> conf = mHost->FindConferenceForAction(callId, String("setOnHold"));
        conf->SetState(IConnection::STATE_HOLDING);
    }
    return NOERROR;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::SetRingbackRequested(
    /* [in] */ const String& callId,
    /* [in] */ Boolean ringing)
{
    AutoPtr<IRemoteConnection> conn = mHost->FindConnectionForAction(callId, String("setRingbackRequested"));
    conn->SetRingbackRequested(ringing);
    return NOERROR;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::SetCallCapabilities(
    /* [in] */ const String& callId,
    /* [in] */ Int32 callCapabilities)
{
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    Boolean bContain = FALSE;
    if ((mHost->mConnectionById->ContainsKey(pId, &bContain), bContain)) {
        AutoPtr<IRemoteConnection> conn = mHost->FindConnectionForAction(callId, String("setCallCapabilities"));
        conn->SetCallCapabilities(callCapabilities);
    }
    else {
        AutoPtr<IRemoteConference> conf = mHost->FindConferenceForAction(callId, String("setCallCapabilities"));
        conf->SetCallCapabilities(callCapabilities);
    }
    return NOERROR;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::SetIsConferenced(
    /* [in] */ const String& callId,
    /* [in] */ const String& conferenceCallId)
{
    // Note: callId should not be null; conferenceCallId may be null
    AutoPtr<IRemoteConnection> connection =
            mHost->FindConnectionForAction(callId, String("setIsConferenced"));
    if (connection != NULL_CONNECTION) {
        if (conferenceCallId == NULL) {
            // 'connection' is being split from its conference
            AutoPtr<IRemoteConference> conf;
            connection->GetConference((IRemoteConference**)&conf);
            if (conf != NULL) {
                conf->RemoveConnection(connection);
            }
        }
        else {
            AutoPtr<IRemoteConference> conference =
                    mHost->FindConferenceForAction(conferenceCallId, String("setIsConferenced"));
            if (conference != NULL_CONFERENCE) {
                conference->AddConnection(connection);
            }
        }
    }
    return NOERROR;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::AddConferenceCall(
    /* [in] */ const String& callId,
    /* [in] */ IParcelableConference* parcel)
{
    AutoPtr<IRemoteConference> conference;
    // CRemoteConference::New(callId,
    //         mOutgoingConnectionServiceRpc, (IRemoteConference**)&conference);

    AutoPtr<IList> l;
    parcel->GetConnectionIds((IList**)&l);
    AutoPtr<IIterator> it;
    l->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> id;
        it->GetNext((IInterface**)&id);
        AutoPtr<IInterface> p;
        mHost->mConnectionById->Get(id, (IInterface**)&p);
        AutoPtr<IRemoteConnection> c = IRemoteConnection::Probe(p);
        if (c != NULL) {
            conference->AddConnection(c);
        }
    }

    AutoPtr<IList> lConn;
    conference->GetConnections((IList**)&lConn);
    Int32 size = 0;
    lConn->GetSize(&size);
    if (size == 0) {
        // A conference was created, but none of its connections are ones that have been
        // created by, and therefore being tracked by, this remote connection service. It
        // is of no interest to us.
        return NOERROR;
    }

    Int32 state = 0;
    parcel->GetState(&state);
    conference->SetState(state);
    Int32 capabilities = 0;
    parcel->GetCapabilities(&capabilities);
    conference->SetCallCapabilities(capabilities);
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    mHost->mConferenceById->Put(pId, conference);
    AutoPtr<RemoteConferenceCallback> cb = new RemoteConferenceCallback(callId, this);
    conference->RegisterCallback(cb);

    mHost->mOurConnectionServiceImpl->AddRemoteConference(conference);
    return NOERROR;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::RemoveCall(
    /* [in] */ const String& callId)
{
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    Boolean bContain = FALSE;
    if ((mHost->mConnectionById->ContainsKey(pId, &bContain), bContain)) {
        AutoPtr<IRemoteConnection> conn = mHost->FindConnectionForAction(callId, String("removeCall"));
        conn->SetDestroyed();
    }
    else {
        AutoPtr<IRemoteConference> conf = mHost->FindConferenceForAction(callId, String("removeCall"));
        conf->SetDestroyed();
    }
    return NOERROR;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::OnPostDialWait(
    /* [in] */ const String& callId,
    /* [in] */ const String& remaining)
{
    AutoPtr<IRemoteConnection> conn = mHost->FindConnectionForAction(callId, String("onPostDialWait"));
    conn->SetPostDialWait(remaining);
    return NOERROR;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::QueryRemoteConnectionServices(
    /* [in] */ IRemoteServiceCallback* callback)
{
    // Not supported from remote connection service.
    return NOERROR;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::SetVideoProvider(
    /* [in] */ const String& callId,
    /* [in] */ IIVideoProvider* videoProvider)
{
    AutoPtr<IRemoteConnection> conn = mHost->FindConnectionForAction(callId, String("setVideoProvider"));
    AutoPtr<IRemoteConnectionVideoProvider> res = new RemoteConnection::VideoProvider(videoProvider);
    conn->SetVideoProvider(res);
    return NOERROR;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::SetVideoState(
    /* [in] */ const String& callId,
    /* [in] */ Int32 videoState)
{
    AutoPtr<IRemoteConnection> conn = mHost->FindConnectionForAction(callId, String("setVideoState"));
    conn->SetVideoState(videoState);
    return NOERROR;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::SetIsVoipAudioMode(
    /* [in] */ const String& callId,
    /* [in] */ Boolean isVoip)
{
    AutoPtr<IRemoteConnection> conn = mHost->FindConnectionForAction(callId, String("setIsVoipAudioMode"));
    conn->SetIsVoipAudioMode(isVoip);
    return NOERROR;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::SetStatusHints(
    /* [in] */ const String& callId,
    /* [in] */ IStatusHints* statusHints)
{
    AutoPtr<IRemoteConnection> conn = mHost->FindConnectionForAction(callId, String("setStatusHints"));
    conn->SetStatusHints(statusHints);
    return NOERROR;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::SetAddress(
    /* [in] */ const String& callId,
    /* [in] */ IUri* address,
    /* [in] */ Int32 presentation)
{
    AutoPtr<IRemoteConnection> conn = mHost->FindConnectionForAction(callId, String("setAddress"));
    conn->SetAddress(address, presentation);
    return NOERROR;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::SetCallerDisplayName(
    /* [in] */ const String& callId,
    /* [in] */ const String& callerDisplayName,
    /* [in] */ Int32 presentation)
{
    AutoPtr<IRemoteConnection> conn = mHost->FindConnectionForAction(callId, String("setCallerDisplayName"));
    conn->SetCallerDisplayName(callerDisplayName, presentation);
    return NOERROR;
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::AsBinder(
    /* [out] */ IBinder** result)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
    // throw new UnsupportedOperationException();
}

ECode RemoteConnectionService::MyConnectionServiceAdapter::SetConferenceableConnections(
    /* [in] */ const String& callId,
    /* [in] */ IList* conferenceableConnectionIds)
{
    AutoPtr<IList> conferenceable;
    CArrayList::New((IList**)&conferenceable);
    AutoPtr<IIterator> it;
    conferenceableConnectionIds->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> id;
        it->GetNext((IInterface**)&id);
        Boolean bContain = FALSE;
        if ((mHost->mConnectionById->ContainsKey(id, &bContain), bContain)) {
            AutoPtr<IInterface> p;
            mHost->mConnectionById->Get(id, (IInterface**)&p);
            conferenceable->Add(p);
        }
    }

    if (mHost->HasConnection(callId)) {
        AutoPtr<IRemoteConnection> conn = mHost->FindConnectionForAction(callId, String("setConferenceableConnections"));
        conn->SetConferenceableConnections(conferenceable);
    }
    else {
        AutoPtr<IRemoteConference> conf = mHost->FindConferenceForAction(callId, String("setConferenceableConnections"));
        conf->SetConferenceableConnections(conferenceable);
    }
    return NOERROR;
}

//===============================================================
// RemoteConnectionService::ProxyDeathRecipient::
//===============================================================
CAR_INTERFACE_IMPL(RemoteConnectionService::ProxyDeathRecipient, Object, IProxyDeathRecipient)

RemoteConnectionService::ProxyDeathRecipient::ProxyDeathRecipient(
    /* [in] */ RemoteConnectionService* host)
    : mHost(host)
{
}

ECode RemoteConnectionService::ProxyDeathRecipient::ProxyDied()
{
    AutoPtr<ICollection> connVals;
    mHost->mConnectionById->GetValues((ICollection**)&connVals);
    AutoPtr<IIterator> connIt;
    connVals->GetIterator((IIterator**)&connIt);
    Boolean bHasNxt = FALSE;
    while ((connIt->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        connIt->GetNext((IInterface**)&p);
        AutoPtr<IRemoteConnection> c = IRemoteConnection::Probe(p);
        c->SetDestroyed();
    }

    AutoPtr<ICollection> confVals;
    mHost->mConferenceById->GetValues((ICollection**)&confVals);
    AutoPtr<IIterator> confIt;
    confVals->GetIterator((IIterator**)&confIt);
    bHasNxt = FALSE;
    while ((confIt->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        confIt->GetNext((IInterface**)&p);
        AutoPtr<IRemoteConference> c = IRemoteConference::Probe(p);
        c->SetDestroyed();
    }

    mHost->mConnectionById->Clear();
    mHost->mConferenceById->Clear();
    mHost->mPendingConnections->Clear();
    AutoPtr<IProxy> prox = (IProxy*)mHost->mOutgoingConnectionServiceRpc->Probe(EIID_IProxy);
    Boolean bRes = FALSE;
    prox->UnlinkToDeath(mHost->mDeathRecipient, 0, &bRes);
    return NOERROR;
}

//===============================================================
// RemoteConnectionService::RemoteConnectionCallback::
//===============================================================
CAR_INTERFACE_IMPL(RemoteConnectionService::RemoteConnectionCallback, Object, IRemoteConnectionCallback)

RemoteConnectionService::RemoteConnectionCallback::RemoteConnectionCallback(
    /* [in] */ const String& id,
    /* [in] */ RemoteConnectionService* host)
    : mId(id)
    , mHost(host)
{}

ECode RemoteConnectionService::RemoteConnectionCallback::OnStateChanged(
    /* [in] */ IRemoteConnection* connection,
    /* [in] */ Int32 state)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode RemoteConnectionService::RemoteConnectionCallback::OnDisconnected(
    /* [in] */ IRemoteConnection* connection,
    /* [in] */ IDisconnectCause* disconnectCause)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode RemoteConnectionService::RemoteConnectionCallback::OnRingbackRequested(
    /* [in] */ IRemoteConnection* connection,
    /* [in] */ Boolean ringback)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode RemoteConnectionService::RemoteConnectionCallback::OnCallCapabilitiesChanged(
    /* [in] */ IRemoteConnection* connection,
    /* [in] */ Int32 callCapabilities)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode RemoteConnectionService::RemoteConnectionCallback::OnPostDialWait(
    /* [in] */ IRemoteConnection* connection,
    /* [in] */ const String& remainingPostDialSequence)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode RemoteConnectionService::RemoteConnectionCallback::OnVoipAudioChanged(
    /* [in] */ IRemoteConnection* connection,
    /* [in] */ Boolean isVoip)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode RemoteConnectionService::RemoteConnectionCallback::OnStatusHintsChanged(
    /* [in] */ IRemoteConnection* connection,
    /* [in] */ IStatusHints* statusHints)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode RemoteConnectionService::RemoteConnectionCallback::OnAddressChanged(
    /* [in] */ IRemoteConnection* connection,
    /* [in] */ IUri* address,
    /* [in] */ Int32 presentation)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode RemoteConnectionService::RemoteConnectionCallback::OnCallerDisplayNameChanged(
    /* [in] */ IRemoteConnection* connection,
    /* [in] */ const String& callerDisplayName,
    /* [in] */ Int32 presentation)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode RemoteConnectionService::RemoteConnectionCallback::OnVideoStateChanged(
    /* [in] */ IRemoteConnection* connection,
    /* [in] */ Int32 videoState)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode RemoteConnectionService::RemoteConnectionCallback::OnDestroyed(
    /* [in] */ IRemoteConnection* connection)
{
    AutoPtr<ICharSequence> pId;
    CString::New(mId, (ICharSequence**)&pId);
    mHost->mConnectionById->Remove(pId);
    mHost->MaybeDisconnectAdapter();
    return NOERROR;
}

ECode RemoteConnectionService::RemoteConnectionCallback::OnConferenceableConnectionsChanged(
    /* [in] */ IRemoteConnection* connection,
    /* [in] */ IList* conferenceableConnections)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode RemoteConnectionService::RemoteConnectionCallback::OnVideoProviderChanged(
    /* [in] */ IRemoteConnection* connection,
    /* [in] */ IRemoteConnectionVideoProvider* videoProvider)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode RemoteConnectionService::RemoteConnectionCallback::OnConferenceChanged(
    /* [in] */ IRemoteConnection* connection,
    /* [in] */ IRemoteConference* conference)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

//===============================================================
// RemoteConnectionService::
//===============================================================

static AutoPtr<IRemoteConnection> InitConn()
{
    AutoPtr<IRemoteConnection> conn;
    // CRemoteConnection::New(String("NULL"), NULL, NULL, (IRemoteConnection**)&conn);
    return conn;
}

AutoPtr<IRemoteConnection> RemoteConnectionService::NULL_CONNECTION = InitConn();

static AutoPtr<IRemoteConference> InitConf()
{
    AutoPtr<IRemoteConference> conf;
    // CRemoteConference::New(String("NULL"), NULL, (IRemoteConference**)&conf);
    return conf;
}

AutoPtr<IRemoteConference> RemoteConnectionService::NULL_CONFERENCE = InitConf();

RemoteConnectionService::RemoteConnectionService()
{
    mServantDelegate = new MyConnectionServiceAdapter(this);

    // CConnectionServiceAdapterServant::New(mServantDelegate, (IConnectionServiceAdapterServant**)&mServant);

    mDeathRecipient = new ProxyDeathRecipient(this);

    CHashMap::New((IMap**)&mConnectionById);
    CHashMap::New((IMap**)&mConferenceById);
    CHashSet::New((ISet**)&mPendingConnections);
}

ECode RemoteConnectionService::constructor(
    /* [in] */ IIConnectionService* outgoingConnectionServiceRpc,
    /* [in] */ IConnectionService* ourConnectionServiceImpl)
{
    mOutgoingConnectionServiceRpc = outgoingConnectionServiceRpc;
    AutoPtr<IProxy> prox = (IProxy*)mOutgoingConnectionServiceRpc->Probe(EIID_IProxy);
    prox->LinkToDeath(mDeathRecipient, 0);
    mOurConnectionServiceImpl = ourConnectionServiceImpl;
    return NOERROR;
}

ECode RemoteConnectionService::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // return "[RemoteCS - " + mOutgoingConnectionServiceRpc.asBinder().toString() + "]";
    return NOERROR;
}

ECode RemoteConnectionService::CreateRemoteConnection(
    /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
    /* [in] */ IConnectionRequest* request,
    /* [in] */ Boolean isIncoming,
    /* [out] */ IRemoteConnection** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IUUIDHelper> hlp;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&hlp);
    AutoPtr<IUUID> uid;
    hlp->RandomUUID((IUUID**)&uid);
    String id;
    uid->ToString(&id);
    AutoPtr<IPhoneAccountHandle> handle;
    request->GetAccountHandle((IPhoneAccountHandle**)&handle);
    AutoPtr<IUri> uri;
    request->GetAddress((IUri**)&uri);
    AutoPtr<IBundle> bundle;
    request->GetExtras((IBundle**)&bundle);
    Int32 state = 0;
    request->GetVideoState(&state);
    AutoPtr<IConnectionRequest> newRequest;
    // CConnectionRequest::New(
    //         handle,
    //         uri,
    //         bundle,
    //         state,
    //         (IConnectionRequest**)&newRequest);
    // try {
    Boolean bEmp = FALSE;
    if ((mConnectionById->IsEmpty(&bEmp), bEmp)) {
        AutoPtr<IIConnectionServiceAdapter> stub;
        mServant->GetStub((IIConnectionServiceAdapter**)&stub);
        mOutgoingConnectionServiceRpc->AddConnectionServiceAdapter(stub);
    }
    AutoPtr<IRemoteConnection> connection;
    // CRemoteConnection::New(id, mOutgoingConnectionServiceRpc, newRequest, (IRemoteConnection**)&connection);
    mPendingConnections->Add(connection);
    mConnectionById->Put(uid, connection);
    mOutgoingConnectionServiceRpc->CreateConnection(
            connectionManagerPhoneAccount,
            id,
            newRequest,
            isIncoming,
            FALSE /* isUnknownCall */);
    AutoPtr<RemoteConnectionCallback> cb = new RemoteConnectionCallback(id, this);
    connection->RegisterCallback(cb);
    *result = connection;
    REFCOUNT_ADD(*result)
    return NOERROR;
    // } catch (RemoteException e) {
    //     return RemoteConnection.failure(
    //             new DisconnectCause(DisconnectCause.ERROR, e.toString()));
    // }
}

Boolean RemoteConnectionService::HasConnection(
    /* [in] */ const String& callId)
{
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    Boolean bContain = FALSE;
    mConnectionById->ContainsKey(pId, &bContain);
    return bContain;
}

AutoPtr<IRemoteConnection> RemoteConnectionService::FindConnectionForAction(
    /* [in] */ const String& callId,
    /* [in] */ const String& action)
{
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    Boolean bContain = FALSE;
    if ((mConnectionById->ContainsKey(pId, &bContain), bContain)) {
        AutoPtr<IInterface> p;
        mConnectionById->Get(pId, (IInterface**)&p);
        return IRemoteConnection::Probe(p);
    }
    Logger::W("RemoteConnectionService", "%s - Cannot find Connection %s", (const char*)action, (const char*)callId);
    return NULL_CONNECTION;
}

AutoPtr<IRemoteConference> RemoteConnectionService::FindConferenceForAction(
    /* [in] */ const String& callId,
    /* [in] */ const String& action)
{
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    Boolean bContain = FALSE;
    if ((mConferenceById->ContainsKey(pId, &bContain), bContain)) {
        AutoPtr<IInterface> p;
        mConferenceById->Get(pId, (IInterface**)&p);
        return IRemoteConference::Probe(p);
    }
    Logger::W("RemoteConnectionService", "%s - Cannot find Conference %s", (const char*)action, (const char*)callId);
    return NULL_CONFERENCE;
}

void RemoteConnectionService::MaybeDisconnectAdapter()
{
    Boolean bEmpConn = FALSE, bEmpConf = FALSE;
    mConnectionById->IsEmpty(&bEmpConn);
    mConferenceById->IsEmpty(&bEmpConf);
    if (bEmpConn && bEmpConf) {
        AutoPtr<IIConnectionServiceAdapter> stub;
        mServant->GetStub((IIConnectionServiceAdapter**)&stub);
        mOutgoingConnectionServiceRpc->RemoveConnectionServiceAdapter(stub);
    }
}

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos