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
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/ConnectionService.h"
#include "elastos/droid/telecom/Connection.h"
#include "elastos/droid/telecom/CPhoneCapabilities.h"
#include "elastos/droid/telecom/CRemoteConnectionManager.h"
#include "elastos/droid/telecom/CConnectionServiceAdapter.h"
#include "elastos/droid/telecom/CConnectionServiceBinder.h"
#include "elastos/droid/telecom/CConnectionHelper.h"
#include "elastos/droid/telecom/CRemoteServiceCallback.h"
#include "elastos/droid/telecom/CParcelableConference.h"
#include "elastos/droid/telecom/CParcelableConnection.h"
#include "elastos/droid/telecom/CDisconnectCause.h"
#include "elastos/droid/os/CLooperHelper.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Internal::Telecom::IIConnectionServiceAdapter;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::IUUID;
using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::CUUIDHelper;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::Concurrent::CConcurrentHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Telecom {

const Boolean ConnectionService::PII_DEBUG = Logger::IsLoggable("ConnectionService", Logger::___DEBUG);

AutoPtr<IConnection> ConnectionService::sNullConnection;

//===============================================================
// ConnectionService::MyHandler::MyRunnable::
//===============================================================
CAR_INTERFACE_IMPL(ConnectionService::MyHandler::MyRunnable, Object, IRunnable)

ConnectionService::MyHandler::MyRunnable::MyRunnable(
    /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
    /* [in] */ const String& id,
    /* [in] */ IConnectionRequest* request,
    /* [in] */ Boolean isIncoming,
    /* [in] */ Boolean isUnknown,
    /* [in] */ MyHandler* host)
    : mConnectionManagerPhoneAccount(connectionManagerPhoneAccount)
    , mId(id)
    , mRequest(request)
    , mIsIncoming(isIncoming)
    , mIsUnknown(isUnknown)
    , mHost(host)
{}

ECode ConnectionService::MyHandler::MyRunnable::Run()
{
    mHost->mHost->CreateConnection(
        mConnectionManagerPhoneAccount,
        mId,
        mRequest,
        mIsIncoming,
        mIsUnknown);
    return NOERROR;
}

//===============================================================
// ConnectionService::MyHandler::
//===============================================================
ConnectionService::MyHandler::MyHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ ConnectionService* host)
    : Handler(looper)
    , mHost(host)
{}

ECode ConnectionService::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (what) {
        case IConnectionService::MSG_ADD_CONNECTION_SERVICE_ADAPTER: {
            AutoPtr<IIConnectionServiceAdapter> pObj = IIConnectionServiceAdapter::Probe(obj);
            mHost->mAdapter->AddAdapter(pObj);
            mHost->OnAdapterAttached();
        }
            break;
        case IConnectionService::MSG_REMOVE_CONNECTION_SERVICE_ADAPTER: {
            AutoPtr<IIConnectionServiceAdapter> pObj = IIConnectionServiceAdapter::Probe(obj);
            mHost->mAdapter->RemoveAdapter(pObj);
        }
            break;
        case IConnectionService::MSG_CREATE_CONNECTION: {
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);

            AutoPtr<IInterface> arg1;
            args->GetObjectArg(1, (IInterface**)&arg1);
            AutoPtr<IPhoneAccountHandle> connectionManagerPhoneAccount = IPhoneAccountHandle::Probe(arg1);
            AutoPtr<IInterface> arg2;
            args->GetObjectArg(2, (IInterface**)&arg2);
            AutoPtr<ICharSequence> pArg2 = ICharSequence::Probe(arg2);
            String id;
            pArg2->ToString(&id);
            AutoPtr<IInterface> arg3;
            args->GetObjectArg(3, (IInterface**)&arg3);
            AutoPtr<IConnectionRequest> request = IConnectionRequest::Probe(arg3);
            Int32 argi1 = 0;
            args->GetInt32Arg(1, &argi1);
            Boolean isIncoming = argi1 == 1;
            Int32 argi2 = 0;
            args->GetInt32Arg(1, &argi2);
            Boolean isUnknown = argi2 == 1;
            if (!mHost->mAreAccountsInitialized) {
                Logger::D("ConnectionService", "Enqueueing pre-init request %s", (const char*)id);
                AutoPtr<MyRunnable> run = new MyRunnable(
                                            connectionManagerPhoneAccount,
                                            id,
                                            request,
                                            isIncoming,
                                            isUnknown,
                                            this);
                mHost->mPreInitializationConnectionRequests->Add((IObject*)run.Get());
            }
            else {
                mHost->CreateConnection(
                    connectionManagerPhoneAccount,
                    id,
                    request,
                    isIncoming,
                    isUnknown);
            }
            args->Recycle();
            break;
        }
        case IConnectionService::MSG_ABORT: {
            AutoPtr<ICharSequence> pObj = ICharSequence::Probe(obj);
            String strObj;
            pObj->ToString(&strObj);
            mHost->Abort(strObj);
        }
            break;
        case IConnectionService::MSG_ANSWER: {
            AutoPtr<ICharSequence> pObj = ICharSequence::Probe(obj);
            String strObj;
            pObj->ToString(&strObj);
            mHost->Answer(strObj);
        }
            break;
        case IConnectionService::MSG_ANSWER_VIDEO: {
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);

            AutoPtr<IInterface> arg1;
            args->GetObjectArg(1, (IInterface**)&arg1);
            AutoPtr<ICharSequence> pArg1 = ICharSequence::Probe(arg1);
            String callId;
            pArg1->ToString(&callId);
            Int32 videoState = 0;
            args->GetInt32Arg(1, &videoState);
            mHost->AnswerVideo(callId, videoState);

            args->Recycle();
            break;
        }
        case IConnectionService::MSG_REJECT: {
            AutoPtr<ICharSequence> pObj = ICharSequence::Probe(obj);
            String strObj;
            pObj->ToString(&strObj);
            mHost->Reject(strObj);
        }
            break;
        case IConnectionService::MSG_DISCONNECT: {
            AutoPtr<ICharSequence> pObj = ICharSequence::Probe(obj);
            String strObj;
            pObj->ToString(&strObj);
            mHost->Disconnect(strObj);
        }
            break;
        case IConnectionService::MSG_HOLD: {
            AutoPtr<ICharSequence> pObj = ICharSequence::Probe(obj);
            String strObj;
            pObj->ToString(&strObj);
            mHost->Hold(strObj);
        }
            break;
        case IConnectionService::MSG_UNHOLD: {
            AutoPtr<ICharSequence> pObj = ICharSequence::Probe(obj);
            String strObj;
            pObj->ToString(&strObj);
            mHost->Unhold(strObj);
        }
            break;
        case IConnectionService::MSG_ON_AUDIO_STATE_CHANGED: {
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);

            AutoPtr<IInterface> arg1;
            args->GetObjectArg(1, (IInterface**)&arg1);
            AutoPtr<ICharSequence> pArg1 = ICharSequence::Probe(arg1);
            String callId;
            pArg1->ToString(&callId);
            AutoPtr<IInterface> arg2;
            args->GetObjectArg(2, (IInterface**)&arg2);
            AutoPtr<IAudioState> audioState = IAudioState::Probe(arg2);
            mHost->OnAudioStateChanged(callId, audioState);

            args->Recycle();
            break;
        }
        case IConnectionService::MSG_PLAY_DTMF_TONE: {
            AutoPtr<ICharSequence> pObj = ICharSequence::Probe(obj);
            String strObj;
            pObj->ToString(&strObj);

            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->PlayDtmfTone(strObj, arg1);
        }
            break;
        case IConnectionService::MSG_STOP_DTMF_TONE: {
            AutoPtr<ICharSequence> pObj = ICharSequence::Probe(obj);
            String strObj;
            pObj->ToString(&strObj);
            mHost->StopDtmfTone(strObj);
        }
            break;
        case IConnectionService::MSG_CONFERENCE: {
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);

            AutoPtr<IInterface> arg1;
            args->GetObjectArg(1, (IInterface**)&arg1);
            AutoPtr<ICharSequence> pArg1 = ICharSequence::Probe(arg1);
            String callId1;
            pArg1->ToString(&callId1);
            AutoPtr<IInterface> arg2;
            args->GetObjectArg(2, (IInterface**)&arg2);
            AutoPtr<ICharSequence> pArg2 = ICharSequence::Probe(arg2);
            String callId2;
            pArg2->ToString(&callId2);
            mHost->Conference(callId1, callId2);

            args->Recycle();
            break;
        }
        case IConnectionService::MSG_SPLIT_FROM_CONFERENCE: {
            AutoPtr<ICharSequence> pObj = ICharSequence::Probe(obj);
            String strObj;
            pObj->ToString(&strObj);
            mHost->SplitFromConference(strObj);
        }
            break;
        case IConnectionService::MSG_MERGE_CONFERENCE: {
            AutoPtr<ICharSequence> pObj = ICharSequence::Probe(obj);
            String strObj;
            pObj->ToString(&strObj);
            mHost->MergeConference(strObj);
        }
            break;
        case IConnectionService::MSG_SWAP_CONFERENCE: {
            AutoPtr<ICharSequence> pObj = ICharSequence::Probe(obj);
            String strObj;
            pObj->ToString(&strObj);
            mHost->SwapConference(strObj);
        }
            break;
        case IConnectionService::MSG_ON_POST_DIAL_CONTINUE: {
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);

            AutoPtr<IInterface> arg1;
            args->GetObjectArg(1, (IInterface**)&arg1);
            AutoPtr<ICharSequence> pArg1 = ICharSequence::Probe(arg1);
            String callId;
            pArg1->ToString(&callId);
            Int32 argi1 = 0;
            args->GetInt32Arg(1, &argi1);
            Boolean proceed = (argi1 == 1);
            mHost->OnPostDialContinue(callId, proceed);

            args->Recycle();
            break;
        }
        default:
            break;
    }
    return NOERROR;
}

//===============================================================
// ConnectionService::ConferenceListener::
//===============================================================
CAR_INTERFACE_IMPL(ConnectionService::ConferenceListener, Object, IConferenceListener)

ConnectionService::ConferenceListener::ConferenceListener(
    /* [in] */ ConnectionService* host)
    : mHost(host)
{}

ECode ConnectionService::ConferenceListener::OnStateChanged(
    /* [in] */ IConference* conference,
    /* [in] */ Int32 oldState,
    /* [in] */ Int32 newState)
{
    AutoPtr<IInterface> p;
    mHost->mIdByConference->Get(conference, (IInterface**)&p);
    AutoPtr<ICharSequence> pId = ICharSequence::Probe(p);
    String id;
    pId->ToString(&id);
    switch (newState) {
        case IConnection::STATE_ACTIVE:
            mHost->mAdapter->SetActive(id);
            break;
        case IConnection::STATE_HOLDING:
            mHost->mAdapter->SetOnHold(id);
            break;
        case IConnection::STATE_DISCONNECTED:
            // handled by onDisconnected
            break;
    }
    return NOERROR;
}

ECode ConnectionService::ConferenceListener::OnDisconnected(
    /* [in] */ IConference* conference,
    /* [in] */ IDisconnectCause* disconnectCause)
{
    AutoPtr<IInterface> p;
    mHost->mIdByConference->Get(conference, (IInterface**)&p);
    AutoPtr<ICharSequence> pId = ICharSequence::Probe(p);
    String id;
    pId->ToString(&id);
    mHost->mAdapter->SetDisconnected(id, disconnectCause);
    return NOERROR;
}

ECode ConnectionService::ConferenceListener::OnConnectionAdded(
    /* [in] */ IConference* conference,
    /* [in] */ IConnection* connection)
{
    return NOERROR;
}

ECode ConnectionService::ConferenceListener::OnConnectionRemoved(
    /* [in] */ IConference* conference,
    /* [in] */ IConnection* connection)
{
    return NOERROR;
}

ECode ConnectionService::ConferenceListener::OnConferenceableConnectionsChanged(
    /* [in] */ IConference* conference,
    /* [in] */ IList* conferenceableConnections)
{
    AutoPtr<IInterface> p;
    mHost->mIdByConference->Get(conference, (IInterface**)&p);
    AutoPtr<ICharSequence> pStr = ICharSequence::Probe(p);
    String str;
    pStr->ToString(&str);
    AutoPtr<IList> l = mHost->CreateConnectionIdList(conferenceableConnections);
    mHost->mAdapter->SetConferenceableConnections(str, l);
    return NOERROR;
}

ECode ConnectionService::ConferenceListener::OnDestroyed(
    /* [in] */ IConference* conference)
{
    mHost->RemoveConference(conference);
    return NOERROR;
}

ECode ConnectionService::ConferenceListener::OnCapabilitiesChanged(
    /* [in] */ IConference* conference,
    /* [in] */ Int32 capabilities)
{
    AutoPtr<IInterface> p;
    mHost->mIdByConference->Get(conference, (IInterface**)&p);
    AutoPtr<ICharSequence> pId = ICharSequence::Probe(p);
    String id;
    pId->ToString(&id);
    AutoPtr<IPhoneCapabilities> pc;
    CPhoneCapabilities::AcquireSingleton((IPhoneCapabilities**)&pc);
    String strCap;
    pc->ToString(capabilities, &strCap);
    Logger::D("ConnectionService", "call capabilities: conference: %s",
            (const char*)strCap);
    mHost->mAdapter->SetCallCapabilities(id, capabilities);
    return NOERROR;
}

//===============================================================
// ConnectionService::ConnectionListener::
//===============================================================
CAR_INTERFACE_IMPL(ConnectionService::ConnectionListener, Object, IConnectionListener)

ConnectionService::ConnectionListener::ConnectionListener(
    /* [in] */ ConnectionService* host)
    : mHost(host)
{}

ECode ConnectionService::ConnectionListener::OnStateChanged(
    /* [in] */ IConnection* c,
    /* [in] */ Int32 state)
{
    AutoPtr<IInterface> p;
    mHost->mIdByConnection->Get(c, (IInterface**)&p);
    AutoPtr<ICharSequence> pId = ICharSequence::Probe(p);
    String id;
    pId->ToString(&id);
    String stateString;
    Connection::StateToString(state, &stateString);
    Logger::D("ConnectionService", "Adapter set state %s %s", (const char*)id, (const char*)stateString);
    switch (state) {
        case IConnection::STATE_ACTIVE:
            mHost->mAdapter->SetActive(id);
            break;
        case IConnection::STATE_DIALING:
            mHost->mAdapter->SetDialing(id);
            break;
        case IConnection::STATE_DISCONNECTED:
            // Handled in onDisconnected()
            break;
        case IConnection::STATE_HOLDING:
            mHost->mAdapter->SetOnHold(id);
            break;
        case IConnection::STATE_NEW:
            // Nothing to tell Telecom
            break;
        case IConnection::STATE_RINGING:
            mHost->mAdapter->SetRinging(id);
            break;
    }
    return NOERROR;
}

ECode ConnectionService::ConnectionListener::OnDisconnected(
    /* [in] */ IConnection* c,
    /* [in] */ IDisconnectCause* disconnectCause)
{
    AutoPtr<IInterface> p;
    mHost->mIdByConnection->Get(c, (IInterface**)&p);
    AutoPtr<ICharSequence> pId = ICharSequence::Probe(p);
    String id;
    pId->ToString(&id);
    Logger::D("ConnectionService", "Adapter set disconnected %p", disconnectCause);
    mHost->mAdapter->SetDisconnected(id, disconnectCause);
    return NOERROR;
}

ECode ConnectionService::ConnectionListener::OnVideoStateChanged(
    /* [in] */ IConnection* c,
    /* [in] */ Int32 videoState)
{
    AutoPtr<IInterface> p;
    mHost->mIdByConnection->Get(c, (IInterface**)&p);
    AutoPtr<ICharSequence> pId = ICharSequence::Probe(p);
    String id;
    pId->ToString(&id);
    Logger::D("ConnectionService", "Adapter set video state %d", videoState);
    mHost->mAdapter->SetVideoState(id, videoState);
    return NOERROR;
}

ECode ConnectionService::ConnectionListener::OnAddressChanged(
    /* [in] */ IConnection* c,
    /* [in] */ IUri* address,
    /* [in] */ Int32 presentation)
{
    AutoPtr<IInterface> p;
    mHost->mIdByConnection->Get(c, (IInterface**)&p);
    AutoPtr<ICharSequence> pId = ICharSequence::Probe(p);
    String id;
    pId->ToString(&id);
    mHost->mAdapter->SetAddress(id, address, presentation);
    return NOERROR;
}

ECode ConnectionService::ConnectionListener::OnCallerDisplayNameChanged(
    /* [in] */ IConnection* c,
    /* [in] */ const String& callerDisplayName,
    /* [in] */ Int32 presentation)
{
    AutoPtr<IInterface> p;
    mHost->mIdByConnection->Get(c, (IInterface**)&p);
    AutoPtr<ICharSequence> pId = ICharSequence::Probe(p);
    String id;
    pId->ToString(&id);
    mHost->mAdapter->SetCallerDisplayName(id, callerDisplayName, presentation);
    return NOERROR;
}

ECode ConnectionService::ConnectionListener::OnDestroyed(
    /* [in] */ IConnection* c)
{
    mHost->RemoveConnection(c);
    return NOERROR;
}

ECode ConnectionService::ConnectionListener::OnPostDialWait(
    /* [in] */ IConnection* c,
    /* [in] */ const String& remaining)
{
    AutoPtr<IInterface> p;
    mHost->mIdByConnection->Get(c, (IInterface**)&p);
    AutoPtr<ICharSequence> pId = ICharSequence::Probe(p);
    String id;
    pId->ToString(&id);
    Logger::D("ConnectionService", "Adapter onPostDialWait %p, %s", c, (const char*)remaining);
    mHost->mAdapter->OnPostDialWait(id, remaining);
    return NOERROR;
}

ECode ConnectionService::ConnectionListener::OnRingbackRequested(
    /* [in] */ IConnection* c,
    /* [in] */ Boolean ringback)
{
    AutoPtr<IInterface> p;
    mHost->mIdByConnection->Get(c, (IInterface**)&p);
    AutoPtr<ICharSequence> pId = ICharSequence::Probe(p);
    String id;
    pId->ToString(&id);
    Logger::D("ConnectionService", "Adapter onRingback %d", ringback);
    mHost->mAdapter->SetRingbackRequested(id, ringback);
    return NOERROR;
}

ECode ConnectionService::ConnectionListener::OnCallCapabilitiesChanged(
    /* [in] */ IConnection* c,
    /* [in] */ Int32 capabilities)
{
    AutoPtr<IInterface> p;
    mHost->mIdByConnection->Get(c, (IInterface**)&p);
    AutoPtr<ICharSequence> pId = ICharSequence::Probe(p);
    String id;
    pId->ToString(&id);
    AutoPtr<IPhoneCapabilities> pc;
    CPhoneCapabilities::AcquireSingleton((IPhoneCapabilities**)&pc);
    String strCap;
    pc->ToString(capabilities, &strCap);
    Logger::D("ConnectionService", "capabilities: parcelableconnection: %s",
            (const char*)strCap);
    mHost->mAdapter->SetCallCapabilities(id, capabilities);
    return NOERROR;
}

ECode ConnectionService::ConnectionListener::OnVideoProviderChanged(
    /* [in] */ IConnection* c,
    /* [in] */ IConnectionVideoProvider* videoProvider)
{
    AutoPtr<IInterface> p;
    mHost->mIdByConnection->Get(c, (IInterface**)&p);
    AutoPtr<ICharSequence> pId = ICharSequence::Probe(p);
    String id;
    pId->ToString(&id);
    mHost->mAdapter->SetVideoProvider(id, videoProvider);
    return NOERROR;
}

ECode ConnectionService::ConnectionListener::OnAudioModeIsVoipChanged(
    /* [in] */ IConnection* c,
    /* [in] */ Boolean isVoip)
{
    AutoPtr<IInterface> p;
    mHost->mIdByConnection->Get(c, (IInterface**)&p);
    AutoPtr<ICharSequence> pId = ICharSequence::Probe(p);
    String id;
    pId->ToString(&id);
    mHost->mAdapter->SetIsVoipAudioMode(id, isVoip);
    return NOERROR;
}

ECode ConnectionService::ConnectionListener::OnStatusHintsChanged(
    /* [in] */ IConnection* c,
    /* [in] */ IStatusHints* statusHints)
{
    AutoPtr<IInterface> p;
    mHost->mIdByConnection->Get(c, (IInterface**)&p);
    AutoPtr<ICharSequence> pId = ICharSequence::Probe(p);
    String id;
    pId->ToString(&id);
    mHost->mAdapter->SetStatusHints(id, statusHints);
    return NOERROR;
}

ECode ConnectionService::ConnectionListener::OnConferenceableConnectionsChanged(
    /* [in] */ IConnection* connection,
    /* [in] */ IList* conferenceableConnections)
{
    AutoPtr<IInterface> p;
    mHost->mIdByConnection->Get(connection, (IInterface**)&p);
    AutoPtr<ICharSequence> pId = ICharSequence::Probe(p);
    String id;
    pId->ToString(&id);
    mHost->mAdapter->SetConferenceableConnections(
            id,
            mHost->CreateConnectionIdList(conferenceableConnections));
    return NOERROR;
}

ECode ConnectionService::ConnectionListener::OnConferenceChanged(
    /* [in] */ IConnection* connection,
    /* [in] */ IConference* conference)
{
    AutoPtr<IInterface> p;
    mHost->mIdByConnection->Get(connection, (IInterface**)&p);
    AutoPtr<ICharSequence> pId = ICharSequence::Probe(p);
    String id;
    pId->ToString(&id);
    if (!id.IsNull()) {
        String conferenceId(NULL);
        if (conference != NULL) {
            AutoPtr<IInterface> p;
            mHost->mIdByConference->Get(conference, (IInterface**)&p);
            AutoPtr<ICharSequence> pId = ICharSequence::Probe(p);
            String conferenceId;
            pId->ToString(&conferenceId);
        }
        mHost->mAdapter->SetIsConferenced(id, conferenceId);
    }
    return NOERROR;
}

//===============================================================
// ConnectionService::
//===============================================================
CAR_INTERFACE_IMPL(ConnectionService, Elastos::Droid::App::Service, IConnectionService);

ConnectionService::ConnectionService()
    : mAreAccountsInitialized(FALSE)
{
    CConcurrentHashMap::New((IMap**)&mConnectionById);
    CConcurrentHashMap::New((IMap**)&mIdByConnection);
    CConcurrentHashMap::New((IMap**)&mConferenceById);
    CConcurrentHashMap::New((IMap**)&mIdByConference);
    CRemoteConnectionManager::New(this, (IRemoteConnectionManager**)&mRemoteConnectionManager);
    CArrayList::New((IList**)&mPreInitializationConnectionRequests);

    CConnectionServiceAdapter::New((IConnectionServiceAdapter**)&mAdapter);

    AutoPtr<ILooperHelper> hlp;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&hlp);
    AutoPtr<ILooper> loop;
    hlp->GetMainLooper((ILooper**)&loop);
    mHandler = new MyHandler(loop, this);

    CConnectionServiceBinder::New(mHandler, (IBinder**)&mBinder);
    mConferenceListener = new ConferenceListener(this);

    mConnectionListener = new ConnectionListener(this);
}

ECode ConnectionService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBinder;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ConnectionService::OnUnbind(
    /* [in] */ IIntent* intent,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    EndAllConnections();
    return Elastos::Droid::App::Service::OnUnbind(intent, result);
}

void ConnectionService::CreateConnection(
    /* [in] */ IPhoneAccountHandle* callManagerAccount,
    /* [in] */ const String& callId,
    /* [in] */ IConnectionRequest* request,
    /* [in] */ Boolean isIncoming,
    /* [in] */ Boolean isUnknown)
{
    Logger::D("ConnectionService", "createConnection, callManagerAccount: %p, callId: %s, request: %p, "
            "isIncoming: %d, isUnknown: %d", callManagerAccount, (const char*)callId, request,
            isIncoming, isUnknown);

    AutoPtr<IConnection> connection;
    if (isUnknown) {
        OnCreateUnknownConnection(callManagerAccount, request, (IConnection**)&connection);
    }
    else if (isIncoming) {
        OnCreateIncomingConnection(callManagerAccount, request, (IConnection**)&connection);
    }
    else {
        OnCreateOutgoingConnection(callManagerAccount, request, (IConnection**)&connection);
    }
    Logger::D("ConnectionService", "createConnection, connection: %p", connection.Get());
    if (connection == NULL) {
        AutoPtr<IDisconnectCause> dc;
        CDisconnectCause::New(IDisconnectCause::ERROR, (IDisconnectCause**)&dc);
        AutoPtr<IConnectionHelper> hlp;
        CConnectionHelper::AcquireSingleton((IConnectionHelper**)&hlp);
        hlp->CreateFailedConnection(dc, (IConnection**)&connection);
    }

    Int32 state = 0;
    connection->GetState(&state);
    if (state != IConnection::STATE_DISCONNECTED) {
        AddConnection(callId, connection);
    }

    AutoPtr<IUri> address;
    connection->GetAddress((IUri**)&address);
    String number;
    if (address == NULL) {
        number = String("NULL");
    }
    else {
        address->GetSchemeSpecificPart(&number);
    }
    Int32 capabilities = 0;
    connection->GetCallCapabilities(&capabilities);
    String strNum;
    Connection::ToLogSafePhoneNumber(number, &strNum);
    String strState;
    Connection::StateToString(state, &strState);
    AutoPtr<IPhoneCapabilities> pc;
    CPhoneCapabilities::AcquireSingleton((IPhoneCapabilities**)&pc);
    String strCap;
    pc->ToString(capabilities, &strCap);
    Logger::V("ConnectionService", "createConnection, number: %s, state: %s, capabilities: %s",
            (const char*)strNum,
            (const char*)strState,
            (const char*)strCap);

    Logger::D("ConnectionService", "createConnection, calling handleCreateConnectionSuccessful %s", (const char*)callId);
    Int32 presentation = 0;
    connection->GetAddressPresentation(&presentation);
    String disName;
    connection->GetCallerDisplayName(&disName);
    Int32 dnPresentation = 0;
    connection->GetCallerDisplayNamePresentation(&dnPresentation);
    AutoPtr<IConnectionVideoProvider> videoProvider;
    connection->GetVideoProvider((IConnectionVideoProvider**)&videoProvider);
    Int32 videoState = 0;
    connection->GetVideoState(&videoState);
    Boolean isRingbackRequested = FALSE;
    connection->IsRingbackRequested(&isRingbackRequested);
    Boolean isVoip = FALSE;
    connection->GetAudioModeIsVoip(&isVoip);
    AutoPtr<IStatusHints> statusHints;
    connection->GetStatusHints((IStatusHints**)&statusHints);
    AutoPtr<IDisconnectCause> disconnectCause;
    connection->GetDisconnectCause((IDisconnectCause**)&disconnectCause);
    AutoPtr<IList> l;
    connection->GetConferenceableConnections((IList**)&l);
    AutoPtr<IIVideoProvider> iivp;
    AutoPtr<IPhoneAccountHandle> handle;
    request->GetAccountHandle((IPhoneAccountHandle**)&handle);

    AutoPtr<IIVideoProvider> vp;
    if (videoProvider != NULL) {
        videoProvider->GetInterface((IIVideoProvider**)&vp);
    }

    AutoPtr<IParcelableConnection> parcelableConnection;
    CParcelableConnection::New(
             handle,
             state,
             capabilities,
             address,
             presentation,
             disName,
             dnPresentation,
             vp,
             videoState,
             isRingbackRequested,
             isVoip,
             statusHints,
             disconnectCause,
             CreateConnectionIdList(l),
            (IParcelableConnection**)&parcelableConnection);

     mAdapter->HandleCreateConnectionComplete(callId, request, parcelableConnection);
}

void ConnectionService::Abort(
    /* [in] */ const String& callId)
{
    Logger::D("ConnectionService", "abort %s", (const char*)callId);
    AutoPtr<IConnection> c = FindConnectionForAction(callId, String("abort"));
    c->OnAbort();
}

void ConnectionService::AnswerVideo(
    /* [in] */ const String& callId,
    /* [in] */ Int32 videoState)
{
    Logger::D("ConnectionService", "answerVideo %s", (const char*)callId);
    AutoPtr<IConnection> c = FindConnectionForAction(callId, String("answer"));
    c->OnAnswer(videoState);
}

void ConnectionService::Answer(
    /* [in] */ const String& callId)
{
    Logger::D("ConnectionService", "answer %s", (const char*)callId);
    AutoPtr<IConnection> c = FindConnectionForAction(callId, String("answer"));
    c->OnAnswer();
}

void ConnectionService::Reject(
    /* [in] */ const String& callId)
{
    Logger::D("ConnectionService", "reject %s", (const char*)callId);
    AutoPtr<IConnection> c = FindConnectionForAction(callId, String("reject"));
    c->OnReject();
}

void ConnectionService::Disconnect(
    /* [in] */ const String& callId)
{
    Logger::D("ConnectionService", "disconnect %s", (const char*)callId);
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    Boolean bCK = FALSE;
    if ((mConnectionById->ContainsKey(pId, &bCK), bCK)) {
        AutoPtr<IConnection> c = FindConnectionForAction(callId, String("disconnect"));
        c->OnDisconnect();
    }
    else {
        AutoPtr<IConference> c = FindConferenceForAction(callId, String("disconnect"));
        c->OnDisconnect();
    }
}

void ConnectionService::Hold(
    /* [in] */ const String& callId)
{
    Logger::D("ConnectionService", "hold %s", (const char*)callId);
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    Boolean bCK = FALSE;
    if ((mConnectionById->ContainsKey(pId, &bCK), bCK)) {
        AutoPtr<IConnection> c = FindConnectionForAction(callId, String("hold"));
        c->OnHold();
    }
    else {
        AutoPtr<IConference> c = FindConferenceForAction(callId, String("hold"));
        c->OnHold();
    }
}

void ConnectionService::Unhold(
    /* [in] */ const String& callId)
{
    Logger::D("ConnectionService", "unhold %s", (const char*)callId);
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    Boolean bCK = FALSE;
    if ((mConnectionById->ContainsKey(pId, &bCK), bCK)) {
        AutoPtr<IConnection> c = FindConnectionForAction(callId, String("unhold"));
        c->OnUnhold();
    }
    else {
        AutoPtr<IConference> c = FindConferenceForAction(callId, String("unhold"));
        c->OnUnhold();
    }
}

void ConnectionService::OnAudioStateChanged(
    /* [in] */ const String& callId,
    /* [in] */ IAudioState* audioState)
{
    Logger::D("ConnectionService", "onAudioStateChanged %s %s", (const char*)callId, TO_CSTR(audioState));
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    Boolean bCK = FALSE;
    if ((mConnectionById->ContainsKey(pId, &bCK), bCK)) {
        AutoPtr<IConnection> c = FindConnectionForAction(callId, String("onAudioStateChanged"));
        c->SetAudioState(audioState);
    }
    else {
        AutoPtr<IConference> c = FindConferenceForAction(callId, String("onAudioStateChanged"));
        c->SetAudioState(audioState);
    }
}

void ConnectionService::PlayDtmfTone(
    /* [in] */ const String& callId,
    /* [in] */ Char32 digit)
{
    Logger::D("ConnectionService", "playDtmfTone %s %c", (const char*)callId, digit);
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    Boolean bCK = FALSE;
    if ((mConnectionById->ContainsKey(pId, &bCK), bCK)) {
        AutoPtr<IConnection> c = FindConnectionForAction(callId, String("playDtmfTone"));
        c->OnPlayDtmfTone(digit);
    }
    else {
        AutoPtr<IConference> c = FindConferenceForAction(callId, String("playDtmfTone"));
        c->OnPlayDtmfTone(digit);
    }
}

void ConnectionService::StopDtmfTone(
    /* [in] */ const String& callId)
{
    Logger::D("ConnectionService", "stopDtmfTone %s", (const char*)callId);
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    Boolean bCK = FALSE;
    if ((mConnectionById->ContainsKey(pId, &bCK), bCK)) {
        AutoPtr<IConnection> c = FindConnectionForAction(callId, String("stopDtmfTone"));
        c->OnStopDtmfTone();
    }
    else {
        AutoPtr<IConference> c = FindConferenceForAction(callId, String("stopDtmfTone"));
        c->OnStopDtmfTone();
    }
}

void ConnectionService::Conference(
    /* [in] */ const String& callId1,
    /* [in] */ const String& callId2)
{
    Logger::D("ConnectionService", "conference %s, %s", (const char*)callId1, (const char*)callId2);

    AutoPtr<IConnection> connection2 = FindConnectionForAction(callId2, String("conference"));
    if (connection2 == GetNullConnection()) {
        Logger::W("ConnectionService", "Connection2 missing in conference request %s.", (const char*)callId2);
        return;
    }

    AutoPtr<IConnection> connection1 = FindConnectionForAction(callId1, String("conference"));
    if (connection1 == GetNullConnection()) {
        AutoPtr<IConference> conference1 = FindConferenceForAction(callId1, String("addConnection"));
        if (conference1 == GetNullConference()) {
            Logger::W("ConnectionService",
                    "Connection1 or Conference1 missing in conference request %s.",
                    (const char*)callId1);
        }
        else {
            conference1->OnMerge(connection2);
        }
    }
    else {
        OnConference(connection1, connection2);
    }
}

void ConnectionService::SplitFromConference(
    /* [in] */ const String& callId)
{
    Logger::D("ConnectionService", "splitFromConference(%s)", (const char*)callId);

    AutoPtr<IConnection> connection = FindConnectionForAction(callId, String("splitFromConference"));
    if (connection == GetNullConnection()) {
        Logger::W("ConnectionService", "Connection missing in conference request %s.", (const char*)callId);
        return;
    }

    AutoPtr<IConference> conference;
    connection->GetConference((IConference**)&conference);
    if (conference != NULL) {
        conference->OnSeparate(connection);
    }
}

void ConnectionService::MergeConference(
    /* [in] */ const String& callId)
{
    Logger::D("ConnectionService", "mergeConference(%s)", (const char*)callId);
    AutoPtr<IConference> conference = FindConferenceForAction(callId, String("mergeConference"));
    if (conference != NULL) {
        conference->OnMerge();
    }
}

void ConnectionService::SwapConference(
    /* [in] */ const String& callId)
{
    Logger::D("ConnectionService", "swapConference(%s)", (const char*)callId);
    AutoPtr<IConference> conference = FindConferenceForAction(callId, String("swapConference"));
    if (conference != NULL) {
        conference->OnSwap();
    }
}

void ConnectionService::OnPostDialContinue(
    /* [in] */ const String& callId,
    /* [in] */ Boolean proceed)
{
    Logger::D("ConnectionService", "onPostDialContinue(%s)", (const char*)callId);
    AutoPtr<IConnection> c = FindConnectionForAction(callId, String("stopDtmfTone"));
    c->OnPostDialContinue(proceed);
}

void ConnectionService::OnAdapterAttached()
{
    if (mAreAccountsInitialized) {
        // No need to query again if we already did it.
        return;
    }

    AutoPtr<IRemoteServiceCallback> cb;
    CRemoteServiceCallback::New((IRemoteServiceCallback**)&cb);
    ((CRemoteServiceCallback*)cb.Get())->SetHost(this);
    mAdapter->QueryRemoteConnectionServices(cb);
}

ECode ConnectionService::CreateRemoteIncomingConnection(
    /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
    /* [in] */ IConnectionRequest* request,
    /* [out] */ IRemoteConnection** result)
{
    VALIDATE_NOT_NULL(result)
    return mRemoteConnectionManager->CreateRemoteConnection(
            connectionManagerPhoneAccount, request, TRUE, result);
}

ECode ConnectionService::CreateRemoteOutgoingConnection(
    /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
    /* [in] */ IConnectionRequest* request,
    /* [out] */ IRemoteConnection** result)
{
    VALIDATE_NOT_NULL(result)
    return mRemoteConnectionManager->CreateRemoteConnection(
            connectionManagerPhoneAccount, request, FALSE, result);
}

ECode ConnectionService::ConferenceRemoteConnections(
    /* [in] */ IRemoteConnection* a,
    /* [in] */ IRemoteConnection* b)
{
    return mRemoteConnectionManager->ConferenceRemoteConnections(a, b);
}

ECode ConnectionService::AddConference(
    /* [in] */ IConference* conference)
{
    String id = AddConferenceInternal(conference);
    if (!id.IsNull()) {
        AutoPtr<IList> connectionIds;
        CArrayList::New(2, (IList**)&connectionIds);
        AutoPtr<IList> l;
        conference->GetConnections((IList**)&l);
        AutoPtr<IIterator> it;
        l->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<IConnection> connection = IConnection::Probe(p);
            Boolean bContain = FALSE;
            if ((mIdByConnection->ContainsKey(connection, &bContain), bContain)) {
                AutoPtr<IInterface> id;
                mIdByConnection->Get(connection, (IInterface**)&id);
                connectionIds->Add(id);
            }
        }

        AutoPtr<IPhoneAccountHandle> handle;
        conference->GetPhoneAccountHandle((IPhoneAccountHandle**)&handle);
        Int32 state = 0;
        conference->GetState(&state);
        Int32 capabilities = 0;
        conference->GetCapabilities(&capabilities);
        AutoPtr<IParcelableConference> parcelableConference;
        CParcelableConference::New(
                            handle,
                            state,
                            capabilities,
                            connectionIds,
                            (IParcelableConference**)&parcelableConference);
        mAdapter->AddConferenceCall(id, parcelableConference);

        // Go through any child calls and set the parent.
        AutoPtr<IList> l2;
        conference->GetConnections((IList**)&l2);
        AutoPtr<IIterator> it2;
        l2->GetIterator((IIterator**)&it2);
        bHasNxt = FALSE;
        while ((it2->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it2->GetNext((IInterface**)&p);
            AutoPtr<IConnection> connection = IConnection::Probe(p);
            AutoPtr<IInterface> connId;
            mIdByConnection->Get(connection, (IInterface**)&connId);
            AutoPtr<ICharSequence> pId = ICharSequence::Probe(connId);
            String connectionId;
            pId->ToString(&connectionId);
            if (!connectionId.IsNull()) {
                mAdapter->SetIsConferenced(connectionId, id);
            }
        }
    }
    return NOERROR;
}

ECode ConnectionService::GetAllConnections(
    /* [out] */ ICollection** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICollection> col;
    mConnectionById->GetValues((ICollection**)&col);
    *result = col;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ConnectionService::OnCreateIncomingConnection(
    /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
    /* [in] */ IConnectionRequest* request,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    return NOERROR;
}

ECode ConnectionService::OnCreateOutgoingConnection(
    /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
    /* [in] */ IConnectionRequest* request,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    return NOERROR;
}

ECode ConnectionService::OnCreateUnknownConnection(
    /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
    /* [in] */ IConnectionRequest* request,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    return NOERROR;
}

ECode ConnectionService::OnConference(
    /* [in] */ IConnection* connection1,
    /* [in] */ IConnection* connection2)
{
    return NOERROR;
}

ECode ConnectionService::OnRemoteConferenceAdded(
    /* [in] */ IRemoteConference* conference)
{
    return NOERROR;
}

ECode ConnectionService::ContainsConference(
    /* [in] */ IConference* conference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mIdByConference->ContainsKey(conference, result);
}

ECode ConnectionService::AddRemoteConference(
    /* [in] */ IRemoteConference* remoteConference)
{
    return OnRemoteConferenceAdded(remoteConference);
}

void ConnectionService::OnAccountsInitialized()
{
    mAreAccountsInitialized = TRUE;
    AutoPtr<IIterator> it;
    mPreInitializationConnectionRequests->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IRunnable> r = IRunnable::Probe(p);
        r->Run();
    }
    mPreInitializationConnectionRequests->Clear();
}

void ConnectionService::AddConnection(
    /* [in] */ const String& callId,
    /* [in] */ IConnection* connection)
{
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    mConnectionById->Put(pId, connection);
    mIdByConnection->Put(connection, pId);
    connection->AddConnectionListener(mConnectionListener);
    connection->SetConnectionService(this);
}

void ConnectionService::RemoveConnection(
    /* [in] */ IConnection* connection)
{
    AutoPtr<IInterface> p;
    mIdByConnection->Get(connection, (IInterface**)&p);
    AutoPtr<ICharSequence> pId = ICharSequence::Probe(p);
    String id;
    pId->ToString(&id);
    connection->UnsetConnectionService(this);
    connection->RemoveConnectionListener(mConnectionListener);
    mConnectionById->Remove(p);
    mIdByConnection->Remove(connection);
    mAdapter->RemoveCall(id);
}

String ConnectionService::AddConferenceInternal(
    /* [in] */ IConference* conference)
{
    Boolean bCK = FALSE;
    if ((mIdByConference->ContainsKey(conference, &bCK), bCK)) {
        Logger::W("ConnectionService", "Re-adding an existing conference: %p.", conference);
    }
    else if (conference != NULL) {
        AutoPtr<IUUIDHelper> hlp;
        CUUIDHelper::AcquireSingleton((IUUIDHelper**)&hlp);
        AutoPtr<IUUID> uuid;
        hlp->RandomUUID((IUUID**)&uuid);
        String id;
        uuid->ToString(&id);
        AutoPtr<ICharSequence> pId;
        CString::New(id, (ICharSequence**)&pId);
        mConferenceById->Put(pId, conference);
        mIdByConference->Put(conference, pId);
        conference->AddListener(mConferenceListener);
        return id;
    }

    return String(NULL);
}

void ConnectionService::RemoveConference(
    /* [in] */ IConference* conference)
{
    Boolean bCK = FALSE;
    if ((mIdByConference->ContainsKey(conference, &bCK), bCK)) {
        conference->RemoveListener(mConferenceListener);

        AutoPtr<IInterface> p;
        mIdByConference->Get(conference, (IInterface**)&p);
        AutoPtr<ICharSequence> pId = ICharSequence::Probe(p);
        String id;
        pId->ToString(&id);
        mConferenceById->Remove(p);
        mIdByConference->Remove(conference);
        mAdapter->RemoveCall(id);
    }
}

AutoPtr<IConnection> ConnectionService::FindConnectionForAction(
    /* [in] */ const String& callId,
    /* [in] */ const String& action)
{
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    Boolean bCK = FALSE;
    if ((mConnectionById->ContainsKey(pId, &bCK), bCK)) {
        AutoPtr<IInterface> p;
        mConnectionById->Get(pId, (IInterface**)&p);
        return IConnection::Probe(p);
    }
    Logger::W("ConnectionService", "%s - Cannot find Connection %s", (const char*)action, (const char*)callId);
    return GetNullConnection();
}

AutoPtr<IConnection> ConnectionService::GetNullConnection()
{
    if (sNullConnection == NULL) {
        assert(0 && "TODO");
        // sNullConnection = new Connection() {};
    }
    return sNullConnection;
}

AutoPtr<IConference> ConnectionService::FindConferenceForAction(
    /* [in] */ const String& conferenceId,
    /* [in] */ const String& action)
{
    AutoPtr<ICharSequence> pId;
    CString::New(conferenceId, (ICharSequence**)&pId);
    Boolean bCK = FALSE;
    if ((mConferenceById->ContainsKey(pId, &bCK), bCK)) {
        AutoPtr<IInterface> p;
        mConferenceById->Get(pId, (IInterface**)&p);
        return IConference::Probe(p);
    }
    Logger::W("ConnectionService", "%s - Cannot find conference %s", (const char*)action, (const char*)conferenceId);
    return GetNullConference();
}

AutoPtr<IList> ConnectionService::CreateConnectionIdList(
    /* [in] */ IList* connections)
{
    AutoPtr<IList> ids;
    CArrayList::New((IList**)&ids);
    AutoPtr<IIterator> it;
    connections->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IConnection> c = IConnection::Probe(p);
        Boolean bCK = FALSE;
        if ((mIdByConnection->ContainsKey(c, &bCK), bCK)) {
            AutoPtr<IInterface> pId;
            mIdByConnection->Get(c, (IInterface**)&pId);
            ids->Add(pId);
        }
    }
    AutoPtr<ICollections> cls;
    CCollections::AcquireSingleton((ICollections**)&cls);
    cls->Sort(ids);
    return ids;
}

AutoPtr<IConference> ConnectionService::GetNullConference()
{
    if (sNullConference == NULL) {
        assert(0 && "TODO");
        // sNullConference = new Conference(NULL) {};
    }
    return sNullConference;
}

void ConnectionService::EndAllConnections()
{
    // Unbound from telecomm.  We should end all connections and conferences.
    AutoPtr<ISet> connectionSt;
    mIdByConnection->GetKeySet((ISet**)&connectionSt);
    AutoPtr<IIterator> connectionIt;
    connectionSt->GetIterator((IIterator**)&connectionIt);
    Boolean bHasNxt = FALSE;
    while ((connectionIt->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        connectionIt->GetNext((IInterface**)&p);
        AutoPtr<IConnection> connection = IConnection::Probe(p);
        // only operate on top-level calls. Conference calls will be removed on their own.
        AutoPtr<IConference> conference;
        connection->GetConference((IConference**)&conference);
        if (conference == NULL) {
            connection->OnDisconnect();
        }
    }
    AutoPtr<ISet> conferenceSt;
    mIdByConference->GetKeySet((ISet**)&conferenceSt);
    AutoPtr<IIterator> conferenceIt;
    conferenceSt->GetIterator((IIterator**)&conferenceIt);
    bHasNxt = FALSE;
    while ((conferenceIt->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        conferenceIt->GetNext((IInterface**)&p);
        AutoPtr<IConference> conference = IConference::Probe(p);
        conference->OnDisconnect();
    }
}

ECode ConnectionService::GetHandler(
    /* [out] */ IHandler** handler)
{
    VALIDATE_NOT_NULL(handler)
    *handler = mHandler;
    REFCOUNT_ADD(*handler)
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos
