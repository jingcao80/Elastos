
#include "elastos/droid/server/telecom/Call.h"
#include "elastos/droid/server/telecom/ConnectionServiceWrapper.h"
#include "elastos/droid/server/telecom/CallIdMapper.h"
#include "elastos/droid/server/telecom/Log.h"
#include <elastos/droid/internal/os/SomeArgs.h>
#include <elastos/droid/net/Uri.h>

using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Internal::Os::SomeArgs;
using Elastos::Droid::Internal::Utility::CPreconditions;
using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Telecomm::Internal::EIID_IIConnectionServiceAdapter;
using Elastos::Droid::Telecomm::Internal::IRemoteServiceCallback;
using Elastos::Droid::Telecomm::Telecom::CConnectionRequest;
using Elastos::Droid::Telecomm::Telecom::CDisconnectCause;
using Elastos::Droid::Telecomm::Telecom::IConnection;
using Elastos::Droid::Telecomm::Telecom::IConnectionRequest;
using Elastos::Droid::Telecomm::Telecom::IConnectionService;
using Elastos::Droid::Telecomm::Telecom::IGatewayInfo;
using Elastos::Droid::Telecomm::Telecom::IParcelableConference;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccount;
using Elastos::Droid::Telecomm::Telecom::ITelecomManager;
using Elastos::Droid::Telecomm::Telecom::IVideoProfileVideoState;
using Elastos::Core::ICloneable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Concurrent::CConcurrentHashMap;
using Elastos::Utility::Concurrent::IConcurrentHashMap;
using Elastos::Utility::ICollections;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// ConnectionServiceWrapper::Adapter
//=============================================================================
CAR_INTERFACE_IMPL_2(ConnectionServiceWrapper::Adapter, Object, IIConnectionServiceAdapter, IBinder)

ECode ConnectionServiceWrapper::Adapter::constructor(
    /* [in] */ IInterface* host)
{
    mHost = (ConnectionServiceWrapper*) IObject::Probe(host);
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::HandleCreateConnectionComplete(
    /* [in] */ const String& callId,
    /* [in] */ IConnectionRequest* request,
    /* [in] */ IParcelableConnection* connection)
{
    mHost->LogIncoming("handleCreateConnectionComplete %s", TO_CSTR(request));
    Boolean isValidCallId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<SomeArgs> args = SomeArgs::Obtain();
        args->mArg1 = StringUtils::ParseCharSequence(callId);
        args->mArg2 = request;
        args->mArg3 = connection;
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_HANDLE_CREATE_CONNECTION_COMPLETE, TO_IINTERFACE(args), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::SetActive(
    /* [in] */ const String& callId)
{
    mHost->LogIncoming("setActive %s", callId.string());
    Boolean isValidConferenceId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidConferenceId(callId, &isValidConferenceId);
    Boolean isValidCallId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId || isValidConferenceId) {
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_SET_ACTIVE, StringUtils::ParseCharSequence(callId), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::SetExtras(
    /* [in] */ const String& callId,
    /* [in] */ IBundle* extras)
{
    Int32 extrasSize;
    extras->GetSize(&extrasSize);
    mHost->LogIncoming("setExtras size= %d | callId= %s", extrasSize, callId.string());
    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArg1 = StringUtils::ParseCharSequence(callId);
    args->mArg2 = extras;
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(MSG_SET_EXTRAS, TO_IINTERFACE(args), (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::SetRinging(
    /* [in] */ const String& callId)
{
    mHost->LogIncoming("setRinging %s", callId.string());
    Boolean isValidCallId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_SET_RINGING, StringUtils::ParseCharSequence(callId), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::SetVideoProvider(
    /* [in] */ const String& callId,
    /* [in] */ IIVideoProvider* videoProvider)
{
    mHost->LogIncoming("setVideoProvider %s", callId.string());
    Boolean isValidCallId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<SomeArgs> args = SomeArgs::Obtain();
        args->mArg1 = StringUtils::ParseCharSequence(callId);
        args->mArg2 = videoProvider;
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_SET_VIDEO_PROVIDER, TO_IINTERFACE(args), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::SetDialing(
    /* [in] */ const String& callId)
{
    mHost->LogIncoming("setDialing %s", callId.string());
    Boolean isValidCallId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_SET_DIALING, StringUtils::ParseCharSequence(callId), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::SetDisconnected(
    /* [in] */ const String& callId,
    /* [in] */ IDisconnectCause* disconnectCause)
{
    mHost->LogIncoming("setDisconnected %s %s", callId.string(), disconnectCause);
    Boolean isValidConferenceId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidConferenceId(callId, &isValidConferenceId);
    Boolean isValidCallId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId || isValidConferenceId) {
        Log::D("ConnectionServiceWrapper", "disconnect call %s", callId.string());
        AutoPtr<SomeArgs> args = SomeArgs::Obtain();
        args->mArg1 = StringUtils::ParseCharSequence(callId);
        args->mArg2 = disconnectCause;
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_SET_DISCONNECTED, TO_IINTERFACE(args), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::SetOnHold(
    /* [in] */ const String& callId)
{
    mHost->LogIncoming("setOnHold %s", callId.string());
    Boolean isValidConferenceId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidConferenceId(callId, &isValidConferenceId);
    Boolean isValidCallId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId || isValidConferenceId) {
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_SET_ON_HOLD, StringUtils::ParseCharSequence(callId), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::SetRingbackRequested(
    /* [in] */ const String& callId,
    /* [in] */ Boolean ringback)
{
    mHost->LogIncoming("setRingbackRequested %s %d", callId.string(), ringback);
    Boolean isValidCallId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_SET_RINGBACK_REQUESTED, ringback ? 1 : 0, 0, StringUtils::ParseCharSequence(callId), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::RemoveCall(
    /* [in] */ const String& callId)
{
    mHost->LogIncoming("removeCall %s", callId.string());
    Boolean isValidConferenceId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidConferenceId(callId, &isValidConferenceId);
    Boolean isValidCallId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId || isValidConferenceId) {
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_REMOVE_CALL, StringUtils::ParseCharSequence(callId), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::SetCallCapabilities(
    /* [in] */ const String& callId,
    /* [in] */ Int32 callCapabilities)
{
    mHost->LogIncoming("setCallCapabilities %s %d", callId.string(), callCapabilities);
    Boolean isValidConferenceId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidConferenceId(callId, &isValidConferenceId);
    Boolean isValidCallId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId || isValidConferenceId) {
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_SET_CALL_CAPABILITIES, callCapabilities, 0, StringUtils::ParseCharSequence(callId), (IMessage**)&msg);
        msg->SendToTarget();
    } else {
        Log::W("ConnectionServiceWrapper", "ID not valid for setCallCapabilities");
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::SetCallProperties(
    /* [in] */ const String& callId,
    /* [in] */ Int32 callProperties)
{
    mHost->LogIncoming("setCallProperties %s %x", callId.string(), callProperties);
    Boolean isValidConferenceId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidConferenceId(callId, &isValidConferenceId);
    Boolean isValidCallId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId || isValidConferenceId) {
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_SET_CALL_PROPERTIES, callProperties, 0, StringUtils::ParseCharSequence(callId), (IMessage**)&msg);
        msg->SendToTarget();
    } else {
        Log::W("ConnectionServiceWrapper", "ID not valid for setCallProperties");
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::SetIsConferenced(
    /* [in] */ const String& callId,
    /* [in] */ const String& conferenceCallId)
{
    mHost->LogIncoming("setIsConferenced %s %s", callId.string(), conferenceCallId.string());
    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArg1 = StringUtils::ParseCharSequence(callId);
    args->mArg2 = StringUtils::ParseCharSequence(conferenceCallId);
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(MSG_SET_IS_CONFERENCED, TO_IINTERFACE(args), (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::AddConferenceCall(
    /* [in] */ const String& callId,
    /* [in] */ IParcelableConference* parcelableConference)
{
    mHost->LogIncoming("addConferenceCall %s %s", callId.string(), parcelableConference);
    // We do not check call Ids here because we do not yet know the call ID for new
    // conference calls.
    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArg1 = StringUtils::ParseCharSequence(callId);
    args->mArg2 = parcelableConference;
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(MSG_ADD_CONFERENCE_CALL, TO_IINTERFACE(args), (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::OnPostDialWait(
    /* [in] */ const String& callId,
    /* [in] */ const String& remaining)
{
    mHost->LogIncoming("onPostDialWait %s %s", callId.string(), remaining.string());
    Boolean isValidCallId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<SomeArgs> args = SomeArgs::Obtain();
        args->mArg1 = StringUtils::ParseCharSequence(callId);
        args->mArg2 = StringUtils::ParseCharSequence(remaining);
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_ON_POST_DIAL_WAIT, TO_IINTERFACE(args), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::QueryRemoteConnectionServices(
    /* [in] */ IRemoteServiceCallback* callback)
{
    mHost->LogIncoming("queryRemoteCSs");
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(MSG_QUERY_REMOTE_CALL_SERVICES, callback, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::SetVideoState(
    /* [in] */ const String& callId,
    /* [in] */ Int32 videoState)
{
    mHost->LogIncoming("setVideoState %s %d", callId.string(), videoState);
    Boolean isValidCallId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_SET_VIDEO_STATE, videoState, 0, StringUtils::ParseCharSequence(callId), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::SetIsVoipAudioMode(
    /* [in] */ const String& callId,
    /* [in] */ Boolean isVoip)
{
    mHost->LogIncoming("setIsVoipAudioMode %s %d", callId.string(), isVoip);
    Boolean isValidCallId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_SET_IS_VOIP_AUDIO_MODE, isVoip ? 1 : 0, 0,
                StringUtils::ParseCharSequence(callId), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::SetStatusHints(
    /* [in] */ const String& callId,
    /* [in] */ IStatusHints* statusHints)
{
    mHost->LogIncoming("setStatusHints %s %s", callId.string(), statusHints);
    Boolean isValidCallId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<SomeArgs> args = SomeArgs::Obtain();
        args->mArg1 = StringUtils::ParseCharSequence(callId);
        args->mArg2 = statusHints;
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_SET_STATUS_HINTS, TO_IINTERFACE(args), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::SetAddress(
    /* [in] */ const String& callId,
    /* [in] */ IUri* address,
    /* [in] */ Int32 presentation)
{
    mHost->LogIncoming("setAddress %s %s %d", callId.string(), address, presentation);
    Boolean isValidCallId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<SomeArgs> args = SomeArgs::Obtain();
        args->mArg1 = StringUtils::ParseCharSequence(callId);
        args->mArg2 = address;
        args->mArgi1 = presentation;
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_SET_ADDRESS, TO_IINTERFACE(args), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::SetCallerDisplayName(
    /* [in] */ const String& callId,
    /* [in] */ const String& callerDisplayName,
    /* [in] */ Int32 presentation)
{
    mHost->LogIncoming("setCallerDisplayName %s %s %d", callId.string(), callerDisplayName.string(), presentation);
    Boolean isValidCallId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<SomeArgs> args = SomeArgs::Obtain();
        args->mArg1 = StringUtils::ParseCharSequence(callId);
        args->mArg2 = StringUtils::ParseCharSequence(callerDisplayName);
        args->mArgi1 = presentation;
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_SET_CALLER_DISPLAY_NAME, TO_IINTERFACE(args), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::SetConferenceableConnections(
    /* [in] */ const String& callId,
    /* [in] */ IList* conferenceableCallIds)
{
    mHost->LogIncoming("setConferenceableConnections %s %s", callId.string(), conferenceableCallIds);
    Boolean isValidConferenceId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidConferenceId(callId, &isValidConferenceId);
    Boolean isValidCallId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId || isValidConferenceId) {
        AutoPtr<SomeArgs> args = SomeArgs::Obtain();
        args->mArg1 = StringUtils::ParseCharSequence(callId);
        args->mArg2 = conferenceableCallIds;
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_SET_CONFERENCEABLE_CONNECTIONS, TO_IINTERFACE(args), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::SetPhoneAccountHandle(
    /* [in] */ const String& callId,
    /* [in] */ IPhoneAccountHandle* pHandle)
{
    mHost->LogIncoming("setPhoneAccountHandle %s %s", callId.string(), pHandle);
    Boolean isValidCallId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<SomeArgs> args = SomeArgs::Obtain();
        args->mArg1 = StringUtils::ParseCharSequence(callId);
        args->mArg2 = pHandle;
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(MSG_SET_PHONE_ACCOUNT, TO_IINTERFACE(args), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::SetCallSubstate(
    /* [in] */ const String& callId,
    /* [in] */ Int32 callSubstate)
{
    mHost->LogIncoming("setCallSubstate %s %d", callId.string(), callSubstate);
    Boolean isValidCallId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->IsValidCallId(callId, &isValidCallId);
    if (isValidCallId) {
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(
            MSG_SET_CALL_SUBSTATE, callSubstate, 0, StringUtils::ParseCharSequence(callId), (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::AddExistingConnection(
    /* [in] */ const String& callId,
    /* [in] */ IParcelableConnection* connection)
{
    mHost->LogIncoming("addExistingConnection  %s %s", callId.string(), connection);
    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArg1 = StringUtils::ParseCharSequence(callId);
    args->mArg2 = connection;
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(MSG_ADD_EXISTING_CONNECTION, TO_IINTERFACE(args), (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode ConnectionServiceWrapper::Adapter::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("ConnectionServiceWrapper::Adapter");
    return NOERROR;
}

//=============================================================================
// ConnectionServiceWrapper::SubCreateBindCallback
//=============================================================================
CAR_INTERFACE_IMPL(ConnectionServiceWrapper::SubCreateBindCallback, Object, IServiceBinderBindCallback)

ConnectionServiceWrapper::SubCreateBindCallback::SubCreateBindCallback(
    /* [in] */ ConnectionServiceWrapper* host,
    /* [in] */ ICall* call,
    /* [in] */ ICreateConnectionResponse* response)
    : mHost(host)
    , mCall(call)
    , mResponse(response)
{}

ECode ConnectionServiceWrapper::SubCreateBindCallback::OnSuccess()
{
    String callId;
    ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCallId(mCall, &callId);
    mHost->mPendingResponses->Put(StringUtils::ParseCharSequence(callId), mResponse);
    AutoPtr<IGatewayInfo> gatewayInfo;
    ((Call*) mCall.Get())->GetGatewayInfo((IGatewayInfo**)&gatewayInfo);
    AutoPtr<IBundle> extras;
    ((Call*) mCall.Get())->GetExtras((IBundle**)&extras);
    String gatewayProviderPackageName;
    gatewayInfo->GetGatewayProviderPackageName(&gatewayProviderPackageName);
    AutoPtr<IUri> originalAddress;
    gatewayInfo->GetOriginalAddress((IUri**)&originalAddress);
    if (gatewayInfo != NULL && !gatewayProviderPackageName.IsNull() &&
            originalAddress != NULL) {
        AutoPtr<IInterface> obj;
        ICloneable::Probe(extras)->Clone((IInterface**)&obj);
        extras = IBundle::Probe(obj) ;
        extras->PutString(
                ITelecomManager::GATEWAY_PROVIDER_PACKAGE,
                gatewayProviderPackageName);
        extras->PutParcelable(
                ITelecomManager::GATEWAY_ORIGINAL_ADDRESS,
                IParcelable::Probe(originalAddress));
    }
    // try {
    ECode ec;
    do {
        Boolean isIncoming;
        ec = ((Call*) mCall.Get())->IsIncoming(&isIncoming);
        if (FAILED(ec)) break;
        Boolean isUnknown;
        ec = mCall->IsUnknown(&isUnknown);
        if (FAILED(ec)) break;
        AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
        ec = ((Call*) mCall.Get())->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
        if (FAILED(ec)) break;
        AutoPtr<IPhoneAccountHandle> connectionManagerPhoneAccount;
        ec = ((Call*) mCall.Get())->GetConnectionManagerPhoneAccount((IPhoneAccountHandle**)&connectionManagerPhoneAccount);
        if (FAILED(ec)) break;
        AutoPtr<IUri> callHandle;
        ec = ((Call*) mCall.Get())->GetHandle((IUri**)&callHandle);
        if (FAILED(ec)) break;
        Int32 videoState;
        ec = mCall->GetVideoState(&videoState);
        if (FAILED(ec)) break;
        AutoPtr<IConnectionRequest> connectionRequest;
        CConnectionRequest::New(phoneAccountHandle, callHandle, extras,
                videoState, (IConnectionRequest**)&connectionRequest);
        ec = mHost->mServiceInterface->CreateConnection(
                connectionManagerPhoneAccount,
                callId,
                connectionRequest,
                isIncoming,
                isUnknown);
    } while(FALSE);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            AutoPtr<IComponentName> componentName;
            mHost->GetComponentName((IComponentName**)&componentName);
            Log::E("ConnectionServiceWrapper", "Failure to createConnection -- %s %d", TO_CSTR(componentName), ec);
            AutoPtr<IDisconnectCause> disconnectCause;
            CDisconnectCause::New(IDisconnectCause::ERROR, StringUtils::ToString(ec), (IDisconnectCause**)&disconnectCause);
            AutoPtr<IInterface> obj;
            mHost->mPendingResponses->Remove(StringUtils::ParseCharSequence(callId), (IInterface**)&obj);
            ICreateConnectionResponse::Probe(obj)->HandleCreateConnectionFailure(disconnectCause);
        }
        else {
            return ec;
        }
    }
    // }
    return NOERROR;
}

ECode ConnectionServiceWrapper::SubCreateBindCallback::OnFailure()
{
    AutoPtr<IComponentName> componentName;
    mHost->GetComponentName((IComponentName**)&componentName);
    Log::E("ConnectionServiceWrapper", "Failure to call %s", TO_CSTR(componentName));
    AutoPtr<IDisconnectCause> disconnectCause;
    CDisconnectCause::New(IDisconnectCause::ERROR, (IDisconnectCause**)&disconnectCause);
    mResponse->HandleCreateConnectionFailure(disconnectCause);
    return NOERROR;
}

//=============================================================================
// ConnectionServiceWrapper::SubHandler
//=============================================================================
ConnectionServiceWrapper::SubHandler::SubHandler(
    /* [in] */ ConnectionServiceWrapper* host)
    : mHost(host)
{}

ECode ConnectionServiceWrapper::SubHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    Int32 msgArg1;
    msg->GetArg1(&msgArg1);
    AutoPtr<ICall> call;
    if (msgWhat == MSG_HANDLE_CREATE_CONNECTION_COMPLETE) {
        AutoPtr<SomeArgs> args = (SomeArgs*) ISomeArgs::Probe(msgObj);
        // try {
        String callId;
        IObject::Probe(args->mArg1)->ToString(&callId);
        AutoPtr<IConnectionRequest> request = IConnectionRequest::Probe(args->mArg2);
        AutoPtr<IParcelableConnection> connection = IParcelableConnection::Probe(args->mArg3);
        mHost->HandleCreateConnectionComplete(callId, request, connection);
        // } finally {
        args->Recycle();
        // }
    }
    else if (msgWhat == MSG_SET_ACTIVE) {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(msgObj, (ICall**)&call);
        if (call != NULL) {
            mHost->mCallsManager->MarkCallAsActive(call);
        } else {
            //Log.w(this, "setActive, unknown call id: %s", msg.obj);
        }
    }
    else if (msgWhat == MSG_SET_EXTRAS) {
        AutoPtr<SomeArgs> args = (SomeArgs*) ISomeArgs::Probe(msgObj);
        // try {
        String callId;
        IObject::Probe(args->mArg1)->ToString(&callId);
        AutoPtr<IBundle> extras = IBundle::Probe(args->mArg2);
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(StringUtils::ParseCharSequence(callId), (ICall**)&call);
        if (call != NULL) {
            mHost->mCallsManager->SetCallExtras(call, extras);
        }
        // } finally {
        args->Recycle();
        // }
    }
    else if (msgWhat == MSG_SET_RINGING) {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(msgObj, (ICall**)&call);
        if (call != NULL) {
            mHost->mCallsManager->MarkCallAsRinging(call);
        } else {
            //Log.w(this, "setRinging, unknown call id: %s", msg.obj);
        }
    }
    else if (msgWhat == MSG_SET_DIALING) {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(msgObj, (ICall**)&call);
        if (call != NULL) {
            mHost->mCallsManager->MarkCallAsDialing(call);
        } else {
            //Log.w(this, "setDialing, unknown call id: %s", msg.obj);
        }
    }
    else if (msgWhat == MSG_SET_DISCONNECTED) {
        AutoPtr<SomeArgs> args = (SomeArgs*) ISomeArgs::Probe(msgObj);
        // try {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(args->mArg1, (ICall**)&call);
        AutoPtr<IDisconnectCause> disconnectCause = IDisconnectCause::Probe(args->mArg2);
        Log::D("ConnectionServiceWrapper", "disconnect call %s %s", TO_CSTR(disconnectCause), TO_CSTR(call));
        if (call != NULL) {
            mHost->mCallsManager->MarkCallAsDisconnected(call, disconnectCause);
        } else {
            //Log.w(this, "setDisconnected, unknown call id: %s", args->mArg1);
        }
        // } finally {
        args->Recycle();
        // }
    }
    else if (msgWhat == MSG_SET_ON_HOLD) {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(msgObj, (ICall**)&call);
        if (call != NULL) {
            mHost->mCallsManager->MarkCallAsOnHold(call);
        } else {
            //Log.w(this, "setOnHold, unknown call id: %s", msg.obj);
        }
    }
    else if (msgWhat == MSG_SET_RINGBACK_REQUESTED) {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(msgObj, (ICall**)&call);
        if (call != NULL) {
            ((Call*) call.Get())->SetRingbackRequested(msgArg1 == 1);
        } else {
            //Log.w(this, "setRingback, unknown call id: %s", args->mArg1);
        }
    }
    else if (msgWhat == MSG_SET_CALL_CAPABILITIES) {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(msgObj, (ICall**)&call);
        if (call != NULL) {
            ((Call*) call.Get())->SetCallCapabilities(msgArg1);
        } else {
            //Log.w(ConnectionServiceWrapper.this,
            //      "setCallCapabilities, unknown call id: %s", msg.obj);
        }
    }
    else if (msgWhat == MSG_SET_CALL_PROPERTIES) {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(msgObj, (ICall**)&call);
        if (call != NULL) {
            ((Call*) call.Get())->SetCallProperties(msgArg1);
        } else {
            //Log.w(ConnectionServiceWrapper.this,
            //      "setCallProperties, unknown call id: %s", msg.obj);
        }
    }
    else if (msgWhat == MSG_SET_IS_CONFERENCED) {
        AutoPtr<SomeArgs> args = (SomeArgs*) ISomeArgs::Probe(msgObj);
        // try {
        AutoPtr<ICall> childCall;
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(args->mArg1, (ICall**)&childCall);
        Log::D("ConnectionServiceWrapper", "SET_IS_CONFERENCE: %s %s", TO_CSTR(args->mArg1), TO_CSTR(args->mArg2));
        if (childCall != NULL) {
            String conferenceCallId;
            IObject::Probe(args->mArg2)->ToString(&conferenceCallId);
            if (conferenceCallId == NULL) {
                Log::D("ConnectionServiceWrapper", "unsetting parent: %s", TO_CSTR(args->mArg1));
                ((Call*) childCall.Get())->SetParentCall(NULL);
            } else {
                AutoPtr<ICall> conferenceCall;
                ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(StringUtils::ParseCharSequence(conferenceCallId), (ICall**)&conferenceCall);
                ((Call*) childCall.Get())->SetParentCall(conferenceCall);
            }
        } else {
            //Log.w(this, "setIsConferenced, unknown call id: %s", args->mArg1);
        }
        // } finally {
        args->Recycle();
        // }
    }
    else if (msgWhat == MSG_ADD_CONFERENCE_CALL) {
        AutoPtr<SomeArgs> args = (SomeArgs*) ISomeArgs::Probe(msgObj);
        // try {
        String id;
        IObject::Probe(args->mArg1)->ToString(&id);
        AutoPtr<ICall> call;
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(StringUtils::ParseCharSequence(id), (ICall**)&call);
        if (call != NULL) {
            Log::W("ConnectionServiceWrapper", "Attempting to add a conference call using an existing "
                    "call id %s", id.string());
        }
        else {
            AutoPtr<IParcelableConference> parcelableConference = IParcelableConference::Probe(args->mArg2);
            // need to create a new Call
            AutoPtr<ICall> conferenceCall;
            mHost->mCallsManager->CreateConferenceCall(NULL, parcelableConference, (ICall**)&conferenceCall);
            ((CallIdMapper*) mHost->mCallIdMapper.Get())->AddCall(conferenceCall, id);
            ((Call*) conferenceCall.Get())->SetConnectionService(mHost);
            AutoPtr<IList> connectionIds;
            parcelableConference->GetConnectionIds((IList**)&connectionIds);
            Log::D("ConnectionServiceWrapper", "adding children to conference %s",
                    TO_CSTR(connectionIds));
            AutoPtr<IIterator> it;
            connectionIds->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                String callId;
                IObject::Probe(obj)->ToString(&callId);
                AutoPtr<ICall> childCall;
                ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(StringUtils::ParseCharSequence(callId), (ICall**)&childCall);
                Log::D("ConnectionServiceWrapper", "found child: %s", callId.string());
                AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
                ((Call*) conferenceCall.Get())->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
                if (phoneAccountHandle == NULL) {
                    AutoPtr<IPhoneAccountHandle> ph;
                    ((Call*) childCall.Get())->GetTargetPhoneAccount((IPhoneAccountHandle**)&ph);
                    ((Call*) conferenceCall.Get())->SetTargetPhoneAccount(ph);
                }
                if (childCall != NULL) {
                    ((Call*) childCall.Get())->SetParentCall(conferenceCall);
                }
            }
        }
        // } finally {
        args->Recycle();
        // }
    }
    else if (msgWhat == MSG_REMOVE_CALL) {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(msgObj, (ICall**)&call);
        if (call != NULL) {
            Boolean isActive;
            ((Call*) call.Get())->IsActive(&isActive);
            if (isActive) {
                AutoPtr<IDisconnectCause> disconnectCause;
                CDisconnectCause::New(IDisconnectCause::REMOTE, (IDisconnectCause**)&disconnectCause);
                mHost->mCallsManager->MarkCallAsDisconnected(call, disconnectCause);
            } else {
                mHost->mCallsManager->MarkCallAsRemoved(call);
            }
        }
    }
    else if (msgWhat == MSG_ON_POST_DIAL_WAIT) {
        AutoPtr<SomeArgs> args = (SomeArgs*) ISomeArgs::Probe(msgObj);
        // try {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(args->mArg1, (ICall**)&call);
        if (call != NULL) {
            String remaining;
            IObject::Probe(args->mArg2)->ToString(&remaining);
            ((Call*) call.Get())->OnPostDialWait(remaining);
        } else {
            //Log.w(this, "onPostDialWait, unknown call id: %s", args->mArg1);
        }
        // } finally {
        args->Recycle();
        // }
    }
    else if (msgWhat == MSG_QUERY_REMOTE_CALL_SERVICES) {
        mHost->QueryRemoteConnectionServices(IRemoteServiceCallback::Probe(msgObj));
    }
    else if (msgWhat == MSG_SET_VIDEO_PROVIDER) {
        AutoPtr<SomeArgs> args = (SomeArgs*) ISomeArgs::Probe(msgObj);
        // try {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(args->mArg1, (ICall**)&call);
        AutoPtr<IIVideoProvider> videoProvider = IIVideoProvider::Probe(args->mArg2);
        if (call != NULL) {
            call->SetVideoProvider(videoProvider);
        }
        // } finally {
        args->Recycle();
        // }
    }
    else if (msgWhat == MSG_SET_IS_VOIP_AUDIO_MODE) {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(msgObj, (ICall**)&call);
        if (call != NULL) {
            call->SetIsVoipAudioMode(msgArg1 == 1);
        }
    }
    else if (msgWhat == MSG_SET_STATUS_HINTS) {
        AutoPtr<SomeArgs> args = (SomeArgs*) ISomeArgs::Probe(msgObj);
        // try {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(args->mArg1, (ICall**)&call);
        AutoPtr<IStatusHints> statusHints = IStatusHints::Probe(args->mArg2);
        if (call != NULL) {
            call->SetStatusHints(statusHints);
        }
        // } finally {
        args->Recycle();
        // }
    }
    else if (msgWhat == MSG_SET_ADDRESS) {
        AutoPtr<SomeArgs> args = (SomeArgs*) ISomeArgs::Probe(msgObj);
        // try {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(args->mArg1, (ICall**)&call);
        if (call != NULL) {
            ((Call*) call.Get())->SetHandle(IUri::Probe(args->mArg2), args->mArgi1);
        }
        // } finally {
        args->Recycle();
        // }
    }
    else if (msgWhat == MSG_SET_CALLER_DISPLAY_NAME) {
        AutoPtr<SomeArgs> args = (SomeArgs*) ISomeArgs::Probe(msgObj);
        // try {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(args->mArg1, (ICall**)&call);
        if (call != NULL) {
            ((Call*) call.Get())->SetCallerDisplayName(TO_STR(args->mArg2), args->mArgi1);
        }
        // } finally {
        args->Recycle();
        // }
    }
    else if (msgWhat == MSG_SET_VIDEO_STATE) {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(msgObj, (ICall**)&call);
        if (call != NULL) {
            call->SetVideoState(msgArg1);
        }
    }
    else if (msgWhat == MSG_SET_CONFERENCEABLE_CONNECTIONS) {
        AutoPtr<SomeArgs> args = (SomeArgs*) ISomeArgs::Probe(msgObj);
        // try {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(args->mArg1, (ICall**)&call);
        if (call != NULL ){
            // @SuppressWarnings("unchecked")
            AutoPtr<IList> conferenceableIds = IList::Probe(args->mArg2);
            Int32 conferenceableIdsSize;
            conferenceableIds->GetSize(&conferenceableIdsSize);
            AutoPtr<IList> conferenceableCalls;
            CArrayList::New(conferenceableIdsSize, (IList**)&conferenceableCalls);
            AutoPtr<IIterator> it;
            IList::Probe(args->mArg2)->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                String otherId;
                IObject::Probe(obj)->ToString(&otherId);
                AutoPtr<ICall> otherCall;
                ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(StringUtils::ParseCharSequence(otherId), (ICall**)&otherCall);
                if (otherCall != NULL && otherCall != call) {
                    conferenceableCalls->Add(otherCall);
                }
            }
            ((Call*) call.Get())->SetConferenceableCalls(conferenceableCalls);
        }
        // } finally {
        args->Recycle();
        // }
    }
    else if (msgWhat == MSG_SET_PHONE_ACCOUNT) {
        AutoPtr<SomeArgs> args = (SomeArgs*) ISomeArgs::Probe(msgObj);
        // try {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(args->mArg1, (ICall**)&call);
        if (call != NULL) {
            AutoPtr<IPhoneAccountHandle> pHandle = IPhoneAccountHandle::Probe(args->mArg2);
            ((Call*) call.Get())->SetTargetPhoneAccount(pHandle);
        } else {
            Log::W("ConnectionServiceWrapper", "setPhoneAccountHandle, unknown call id: %s", TO_CSTR(args->mArg1));
        }
        // } finally {
        args->Recycle();
        // }
    }
    else if (msgWhat == MSG_SET_CALL_SUBSTATE) {
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->GetCall(msgObj, (ICall**)&call);
        if (call != NULL) {
            call->SetCallSubstate(msgArg1);
        }
    }
    else if (msgWhat == MSG_ADD_EXISTING_CONNECTION) {
        AutoPtr<SomeArgs> args = (SomeArgs*) ISomeArgs::Probe(msgObj);
        // try {
        String callId;
        IObject::Probe(args->mArg1)->ToString(&callId);
        AutoPtr<IParcelableConnection> connection = IParcelableConnection::Probe(args->mArg2);
        AutoPtr<ICall> existingCall;
        mHost->mCallsManager->CreateCallForExistingConnection(callId, connection, (ICall**)&existingCall);
        ((CallIdMapper*) mHost->mCallIdMapper.Get())->AddCall(existingCall, callId);
        ((Call*) existingCall.Get())->SetConnectionService(mHost);
        // } finally {
        args->Recycle();
        // }
    }
    return NOERROR;
}

//=============================================================================
// ConnectionServiceWrapper::SubQueryBindCallback
//=============================================================================
CAR_INTERFACE_IMPL(ConnectionServiceWrapper::SubQueryBindCallback, Object, IServiceBinderBindCallback)

ConnectionServiceWrapper::SubQueryBindCallback::SubQueryBindCallback(
    /* [in] */ ConnectionServiceWrapper* host,
    /* [in] */ IConnectionServiceWrapper* currentSimService,
    /* [in] */ IList* simServiceComponentNames,
    /* [in] */ IList* simServiceBinders,
    /* [in] */ IRemoteServiceCallback* callback,
    /* [in] */ ISet* simServices)
    : mHost(host)
    , mCurrentSimService(currentSimService)
    , mSimServiceComponentNames(simServiceComponentNames)
    , mSimServiceBinders(simServiceBinders)
    , mCallback(callback)
    , mSimServices(simServices)
{}

ECode ConnectionServiceWrapper::SubQueryBindCallback::OnSuccess()
{
    AutoPtr<IComponentName> componentName;
    ((ConnectionServiceWrapper*) mCurrentSimService.Get())->GetComponentName((IComponentName**)&componentName);
    Log::D("ConnectionServiceWrapper", "Adding simService %s", TO_CSTR(componentName));
    mSimServiceComponentNames->Add(componentName);
    mSimServiceBinders->Add(IBinder::Probe(((ConnectionServiceWrapper*) mCurrentSimService.Get())->mServiceInterface));
    MaybeComplete();
    return NOERROR;
}

ECode ConnectionServiceWrapper::SubQueryBindCallback::OnFailure()
{
    AutoPtr<IComponentName> componentName;
    ((ConnectionServiceWrapper*) mCurrentSimService.Get())->GetComponentName((IComponentName**)&componentName);
    Log::D("ConnectionServiceWrapper", "Failed simService %s", TO_CSTR(componentName));
    // We know maybeComplete() will always be a no-op from now on, so go ahead and
    // signal failure of the entire request
    mHost->NoRemoteServices(mCallback);
    return NOERROR;
}

ECode ConnectionServiceWrapper::SubQueryBindCallback::MaybeComplete()
{
    Int32 simServicesSize;
    mSimServices->GetSize(&simServicesSize);
    Int32 simServiceComponentNamesSize;
    mSimServiceComponentNames->GetSize(&simServiceComponentNamesSize);
    if (simServiceComponentNamesSize == simServicesSize) {
        mHost->SetRemoteServices(mCallback, mSimServiceComponentNames, mSimServiceBinders);
    }
    return NOERROR;
}

//=============================================================================
// ConnectionServiceWrapper
//=============================================================================
CAR_INTERFACE_IMPL(ConnectionServiceWrapper, ServiceBinder, IConnectionServiceWrapper)

const Int32 ConnectionServiceWrapper::MSG_HANDLE_CREATE_CONNECTION_COMPLETE = 1;
const Int32 ConnectionServiceWrapper::MSG_SET_ACTIVE = 2;
const Int32 ConnectionServiceWrapper::MSG_SET_RINGING = 3;
const Int32 ConnectionServiceWrapper::MSG_SET_DIALING = 4;
const Int32 ConnectionServiceWrapper::MSG_SET_DISCONNECTED = 5;
const Int32 ConnectionServiceWrapper::MSG_SET_ON_HOLD = 6;
const Int32 ConnectionServiceWrapper::MSG_SET_RINGBACK_REQUESTED = 7;
const Int32 ConnectionServiceWrapper::MSG_SET_CALL_CAPABILITIES = 8;
const Int32 ConnectionServiceWrapper::MSG_SET_IS_CONFERENCED = 9;
const Int32 ConnectionServiceWrapper::MSG_ADD_CONFERENCE_CALL = 10;
const Int32 ConnectionServiceWrapper::MSG_REMOVE_CALL = 11;
const Int32 ConnectionServiceWrapper::MSG_ON_POST_DIAL_WAIT = 12;
const Int32 ConnectionServiceWrapper::MSG_QUERY_REMOTE_CALL_SERVICES = 13;
const Int32 ConnectionServiceWrapper::MSG_SET_VIDEO_PROVIDER = 14;
const Int32 ConnectionServiceWrapper::MSG_SET_IS_VOIP_AUDIO_MODE = 15;
const Int32 ConnectionServiceWrapper::MSG_SET_STATUS_HINTS = 16;
const Int32 ConnectionServiceWrapper::MSG_SET_ADDRESS = 17;
const Int32 ConnectionServiceWrapper::MSG_SET_CALLER_DISPLAY_NAME = 18;
const Int32 ConnectionServiceWrapper::MSG_SET_VIDEO_STATE = 19;
const Int32 ConnectionServiceWrapper::MSG_SET_CONFERENCEABLE_CONNECTIONS = 20;
const Int32 ConnectionServiceWrapper::MSG_SET_EXTRAS = 21;
const Int32 ConnectionServiceWrapper::MSG_SET_DISCONNECTED_WITH_SUPP_NOTIFICATION = 22;
const Int32 ConnectionServiceWrapper::MSG_SET_PHONE_ACCOUNT = 23;
const Int32 ConnectionServiceWrapper::MSG_SET_CALL_SUBSTATE = 24;
const Int32 ConnectionServiceWrapper::MSG_ADD_EXISTING_CONNECTION = 25;
const Int32 ConnectionServiceWrapper::MSG_SET_CALL_PROPERTIES = 30;

ConnectionServiceWrapper::ConnectionServiceWrapper()
{
    mHandler = new SubHandler(this);
    mAdapter = new Adapter();
    mCallsManager = CallsManager::GetInstance();
    AutoPtr<IMap> map;
    CConcurrentHashMap::New(8, 0.9f, 1, (IMap**)&map);
    AutoPtr<ICollections> helper;
    helper->SetFromMap(map, (ISet**)&mPendingConferenceCalls);
    mCallIdMapper = new CallIdMapper();
    ((CallIdMapper*) mCallIdMapper.Get())->constructor(String("ConnectionService"));
    CHashMap::New((IMap**)&mPendingResponses);
    mBinder = new ServiceBinder::Binder(this);
}

ECode ConnectionServiceWrapper::constructor(
    /* [in] */ IComponentName* componentName,
    /* [in] */ ConnectionServiceRepository* connectionServiceRepository,
    /* [in] */ PhoneAccountRegistrar* phoneAccountRegistrar,
    /* [in] */ IContext* context)
{
    ServiceBinder::constructor(IConnectionService::SERVICE_INTERFACE, componentName, context);
    mConnectionServiceRepository = connectionServiceRepository;
    // TODO -- Upon changes to PhoneAccountRegistrar, need to re-wire connections
    // To do this, we must proxy remote ConnectionService objects
    phoneAccountRegistrar->AddListener(new PhoneAccountRegistrar::Listener());
    mPhoneAccountRegistrar = phoneAccountRegistrar;
    return NOERROR;
}

ECode ConnectionServiceWrapper::AddConnectionServiceAdapter(
    /* [in] */ IIConnectionServiceAdapter* adapter)
{
    Boolean isServiceValid;
    IsServiceValid(String("addConnectionServiceAdapter"), &isServiceValid);
    if (isServiceValid) {
        // try {
        LogOutgoing("addConnectionServiceAdapter %s", adapter);
        ECode ec = mServiceInterface->AddConnectionServiceAdapter(adapter);
        // } catch (RemoteException e) {
        if ((ECode) E_REMOTE_EXCEPTION != ec) {
            return ec;
        }
        // }
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::CreateConnection(
    /* [in] */ ICall* call,
    /* [in] */ ICreateConnectionResponse* response)
{
    AutoPtr<IComponentName> componentName;
    GetComponentName((IComponentName**)&componentName);
    Log::D("ConnectionServiceWrapper", "createConnection(%s) via %s.", TO_CSTR(call), TO_CSTR(componentName));
    AutoPtr<IServiceBinderBindCallback> callback = new SubCreateBindCallback(this, call, response);
    mBinder->Bind(callback);
    return NOERROR;
}

ECode ConnectionServiceWrapper::Abort(
    /* [in] */ ICall* call)
{
    // Clear out any pending outgoing call data
    String callId;
    ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(call, &callId);
    // If still bound, tell the connection service to abort.
    Boolean isServiceValid;
    IsServiceValid(String("abort"), &isServiceValid);
    if (callId != NULL && isServiceValid) {
        // try {
        LogOutgoing("abort %s", callId.string());
        ECode ec = mServiceInterface->Abort(callId);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION != ec) {
                return ec;
            }
        }
        // }
    }
    AutoPtr<IDisconnectCause> disconnectCause;
    CDisconnectCause::New(IDisconnectCause::LOCAL, (IDisconnectCause**)&disconnectCause);
    RemoveCall(call, disconnectCause);
    return NOERROR;
}

ECode ConnectionServiceWrapper::Hold(
    /* [in] */ ICall* call)
{
    String callId;
    ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(call, &callId);
    Boolean isServiceValid;
    IsServiceValid(String("hold"), &isServiceValid);
    if (callId != NULL && isServiceValid) {
        // try {
        LogOutgoing("hold %s", callId.string());
        ECode ec = mServiceInterface->Hold(callId);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION != ec) {
                return ec;
            }
        }
        // }
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Unhold(
    /* [in] */ ICall* call)
{
    String callId;
    ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(call, &callId);
    Boolean isServiceValid;
    IsServiceValid(String("unhold"), &isServiceValid);
    if (callId != NULL && isServiceValid) {
        // try {
        LogOutgoing("unhold %s", callId.string());
        ECode ec = mServiceInterface->Unhold(callId);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION != ec) {
                return ec;
            }
        }
        // }
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::OnAudioStateChanged(
    /* [in] */ ICall* activeCall,
    /* [in] */ IAudioState* audioState)
{
    String callId;
    ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(activeCall, &callId);
    Boolean isServiceValid;
    IsServiceValid(String("onAudioStateChanged"), &isServiceValid);
    if (callId != NULL && isServiceValid) {
        // try {
        LogOutgoing("onAudioStateChanged %s %s", callId.string(), audioState);
        ECode ec = mServiceInterface->OnAudioStateChanged(callId, audioState);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION != ec) {
                return ec;
            }
        }
        // }
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Disconnect(
    /* [in] */ ICall* call)
{
    String callId;
    ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(call, &callId);
    Boolean isServiceValid;
    IsServiceValid(String("disconnect"), &isServiceValid);
    if (callId != NULL && isServiceValid) {
        // try {
        LogOutgoing("disconnect %s", callId.string());
        ECode ec = mServiceInterface->Disconnect(callId);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION != ec) {
                return ec;
            }
        }
        // }
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Answer(
    /* [in] */ ICall* call,
    /* [in] */ Int32 videoState)
{
    String callId;
    ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(call, &callId);
    Boolean isServiceValid;
    IsServiceValid(String("answer"), &isServiceValid);
    if (callId != NULL && isServiceValid) {
        // try {
        ECode ec;
        LogOutgoing("answer %s %d", callId.string(), videoState);
        if (videoState == IVideoProfileVideoState::AUDIO_ONLY) {
            ec = mServiceInterface->Answer(callId);
        } else {
            ec = mServiceInterface->AnswerVideo(callId, videoState);
        }
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION != ec) {
                return ec;
            }
        }
        // }
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Deflect(
    /* [in] */ ICall* call,
    /* [in] */ const String& number)
{
    String callId;
    ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(call, &callId);
    Boolean isServiceValid;
    IsServiceValid(String("deflect"), &isServiceValid);
    if (callId != NULL && isServiceValid) {
        // try {
        LogOutgoing("deflect %s %s", callId.string(), number.string());
        assert(0 && "TODO: IIConnectionService");
        // ECode ec = mServiceInterface->Deflect(callId, number);
        // // } catch (RemoteException e) {
        // if (FAILED(ec)) {
        //     if ((ECode) E_REMOTE_EXCEPTION != ec) {
        //         return ec;
        //     }
        // }
        // }
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Reject(
    /* [in] */ ICall* call)
{
    String callId;
    ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(call, &callId);
    Boolean isServiceValid;
    IsServiceValid(String("reject"), &isServiceValid);
    if (callId != NULL && isServiceValid) {
        // try {
        LogOutgoing("reject %s", callId.string());
        ECode ec = mServiceInterface->Reject(callId);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION != ec) {
                return ec;
            }
        }
        // }
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::PlayDtmfTone(
    /* [in] */ ICall* call,
    /* [in] */ Char32 digit)
{
    String callId;
    ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(call, &callId);
    Boolean isServiceValid;
    IsServiceValid(String("playDtmfTone"), &isServiceValid);
    if (callId != NULL && isServiceValid) {
        // try {
        LogOutgoing("playDtmfTone %s %c", callId.string(), digit);
        ECode ec = mServiceInterface->PlayDtmfTone(callId, digit);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION != ec) {
                return ec;
            }
        }
        // }
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::SetLocalCallHold(
    /* [in] */ ICall* call,
    /* [in] */ Int32 lchStatus)
{
    Boolean isServiceValid;
    IsServiceValid(String("SetLocalCallHold"), &isServiceValid);
    if (isServiceValid) {
        // try {
        String callId;
        ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(call, &callId);
        LogOutgoing("SetLocalCallHold %s %d", callId.string(), lchStatus);
        assert(0 && "TODO: IIConnectionService");
        // ECode ec = mServiceInterface->SetLocalCallHold(callId, lchStatus);
        // // } catch (RemoteException e) {
        // if (FAILED(ec)) {
        //     if ((ECode) E_REMOTE_EXCEPTION != ec) {
        //         return ec;
        //     }
        // }
        // }
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::SetActiveSubscription(
    /* [in] */ ICall* call)
{
    Boolean isServiceValid;
    IsServiceValid(String("setActiveSubscription"), &isServiceValid);
    if (isServiceValid) {
        // try {
        String callId;
        ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(call, &callId);
        LogOutgoing("setActiveSubscription %s", callId.string());
        assert(0 && "TODO: IIConnectionService");
        // ECode ec = mServiceInterface->SetActiveSubscription(callId);
        // // } catch (RemoteException e) {
        // if (FAILED(ec)) {
        //     if ((ECode) E_REMOTE_EXCEPTION != ec) {
        //         return ec;
        //     }
        // }
        // // }
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::StopDtmfTone(
    /* [in] */ ICall* call)
{
    String callId;
    ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(call, &callId);
    Boolean isServiceValid;
    IsServiceValid(String("stopDtmfTone"), &isServiceValid);
    if (callId != NULL && isServiceValid) {
        // try {
        LogOutgoing("stopDtmfTone %s",callId.string());
        ECode ec = mServiceInterface->StopDtmfTone(callId);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION != ec) {
                return ec;
            }
        }
        // }
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::AddCall(
    /* [in] */ ICall* call)
{
    String callId;
    ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(call, &callId);
    if (callId == NULL) {
        ((CallIdMapper*) mCallIdMapper.Get())->AddCall(call);
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::ReplaceCall(
    /* [in] */ ICall* newCall,
    /* [in] */ ICall* callToReplace)
{
    AutoPtr<IPreconditions> preconditionsHelper;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditionsHelper);
    AutoPtr<IConnectionServiceWrapper> connectionServiceWrapper;
    ((Call*) callToReplace)->GetConnectionService((IConnectionServiceWrapper**)&connectionServiceWrapper);
    preconditionsHelper->CheckState(connectionServiceWrapper == this);
    ((CallIdMapper*) mCallIdMapper.Get())->ReplaceCall(newCall, callToReplace);
    return NOERROR;
}

ECode ConnectionServiceWrapper::RemoveCall(
    /* [in] */ ICall* call)
{
    AutoPtr<IDisconnectCause> disconnectCause;
    CDisconnectCause::New(IDisconnectCause::ERROR, (IDisconnectCause**)&disconnectCause);
    RemoveCall(call, disconnectCause);
    return NOERROR;
}

ECode ConnectionServiceWrapper::RemoveCall(
    /* [in] */ const String& callId,
    /* [in] */ IDisconnectCause* disconnectCause)
{
    AutoPtr<IInterface> obj;
    mPendingResponses->Remove(StringUtils::ParseCharSequence(callId), (IInterface**)&obj);
    AutoPtr<ICreateConnectionResponse> response = ICreateConnectionResponse::Probe(obj);
    if (response != NULL) {
        response->HandleCreateConnectionFailure(disconnectCause);
    }
    ((CallIdMapper*) mCallIdMapper.Get())->RemoveCall(callId);
    return NOERROR;
}

ECode ConnectionServiceWrapper::RemoveCall(
    /* [in] */ ICall* call,
    /* [in] */ IDisconnectCause* disconnectCause)
{
    String callId;
    ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(call, &callId);
    AutoPtr<IInterface> obj;
    mPendingResponses->Remove(StringUtils::ParseCharSequence(callId), (IInterface**)&obj);
    AutoPtr<ICreateConnectionResponse> response = ICreateConnectionResponse::Probe(obj);
    if (response != NULL) {
        response->HandleCreateConnectionFailure(disconnectCause);
    }
    ((CallIdMapper*) mCallIdMapper.Get())->RemoveCall(call);
    return NOERROR;
}

ECode ConnectionServiceWrapper::OnPostDialContinue(
    /* [in] */ ICall* call,
    /* [in] */ Boolean proceed)
{
    String callId;
    ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(call, &callId);
    Boolean isServiceValid;
    IsServiceValid(String("onPostDialContinue"), &isServiceValid);
    if (callId != NULL && isServiceValid) {
        // try {
        LogOutgoing("onPostDialContinue %s %d", callId.string(), proceed);
        ECode ec = mServiceInterface->OnPostDialContinue(callId, proceed);
        // } catch (RemoteException ignored) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION != ec) {
                return ec;
            }
        }
        // }
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::Conference(
    /* [in] */ ICall* call,
    /* [in] */ ICall* otherCall)
{
    String callId;
    ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(call, &callId);
    String otherCallId;
    ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(otherCall, &otherCallId);
    Boolean isServiceValid;
    IsServiceValid(String("conference"), &isServiceValid);
    if (callId != NULL && otherCallId != NULL && isServiceValid) {
        // try {
        LogOutgoing("conference %s %s", callId.string(), otherCallId.string());
        ECode ec = mServiceInterface->Conference(callId, otherCallId);
        // } catch (RemoteException ignored) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION != ec) {
                return ec;
            }
        }
        // }
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::SplitFromConference(
    /* [in] */ ICall* call)
{
    String callId;
    ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(call, &callId);
    Boolean isServiceValid;
    IsServiceValid(String("splitFromConference"), &isServiceValid);
    if (callId != NULL && isServiceValid) {
        // try {
        LogOutgoing("splitFromConference %s", callId.string());
        ECode ec = mServiceInterface->SplitFromConference(callId);
        // } catch (RemoteException ignored) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION != ec) {
                return ec;
            }
        }
        // }
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::MergeConference(
    /* [in] */ ICall* call)
{
    String callId;
    ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(call, &callId);
    Boolean isServiceValid;
    IsServiceValid(String("mergeConference"), &isServiceValid);
    if (callId != NULL && isServiceValid) {
        // try {
        LogOutgoing("mergeConference %s", callId.string());
        ECode ec = mServiceInterface->MergeConference(callId);
        // } catch (RemoteException ignored) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION != ec) {
                return ec;
            }
        }
        // }
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::SwapConference(
    /* [in] */ ICall* call)
{
    String callId;
    ((CallIdMapper*) mCallIdMapper.Get())->GetCallId(call, &callId);
    Boolean isServiceValid;
    IsServiceValid(String("swapConference"), &isServiceValid);
    if (callId != NULL && isServiceValid) {
        // try {
        LogOutgoing("swapConference %s", callId.string());
        ECode ec = mServiceInterface->SwapConference(callId);
        // } catch (RemoteException ignored) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION != ec) {
                return ec;
            }
        }
        // }
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::SetServiceInterface(
    /* [in] */ IBinder* binder)
{
    if (binder == NULL) {
        // We have lost our service connection. Notify the world that this service is done.
        // We must notify the adapter before CallsManager. The adapter will force any pending
        // outgoing calls to try the next service. This needs to happen before CallsManager
        // tries to clean up any calls still associated with this service.
        HandleConnectionServiceDeath();
        CallsManager::GetInstance()->HandleConnectionServiceDeath(this);
        mServiceInterface = NULL;
    } else {
        mServiceInterface = IIConnectionService::Probe(binder);
        AddConnectionServiceAdapter(mAdapter);
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::HandleCreateConnectionComplete(
    /* [in] */ const String& callId,
    /* [in] */ IConnectionRequest* request,
    /* [in] */ IParcelableConnection* connection)
{
    // TODO: Note we are not using parameter "request", which is a side effect of our tacit
    // assumption that we have at most one outgoing connection attempt per ConnectionService.
    // This may not continue to be the case.
    Int32 connectionState;
    connection->GetState(&connectionState);
    if (connectionState == IConnection::STATE_DISCONNECTED) {
        // A connection that begins in the DISCONNECTED state is an indication of
        // failure to connect; we handle all failures uniformly
        AutoPtr<IDisconnectCause> disconnectCause;
        connection->GetDisconnectCause((IDisconnectCause**)&disconnectCause);
        RemoveCall(callId, disconnectCause);
    } else {
        // Successful connection
        Boolean isContainsKey;
        mPendingResponses->ContainsKey(StringUtils::ParseCharSequence(callId), &isContainsKey);
        if (isContainsKey) {
            AutoPtr<IInterface> obj;
            mPendingResponses->Remove(StringUtils::ParseCharSequence(callId), (IInterface**)&obj);
            AutoPtr<ICall> call = ICall::Probe(obj);
            ((Call*) call.Get())->HandleCreateConnectionSuccess(mCallIdMapper, connection);
        }
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::HandleConnectionServiceDeath()
{
    Boolean isEmpty;
    mPendingResponses->IsEmpty(&isEmpty);
    if (!isEmpty) {
        AutoPtr<ICollection> values;
        mPendingResponses->GetValues((ICollection**)&values);
        Int32 size;
        values->GetSize(&size);
        AutoPtr<ArrayOf<IInterface*> > responses;
        values->ToArray(ArrayOf<IInterface*>::Alloc(size), (ArrayOf<IInterface*>**)&responses);
        mPendingResponses->Clear();
        for (Int32 i = 0; i < responses->GetLength(); i++) {
            AutoPtr<IDisconnectCause> disconnectCause;
            CDisconnectCause::New(IDisconnectCause::ERROR, (IDisconnectCause**)&disconnectCause);
            ICreateConnectionResponse::Probe((*responses)[i])->HandleCreateConnectionFailure(disconnectCause);
        }
    }
    ((CallIdMapper*) mCallIdMapper.Get())->Clear();
    return NOERROR;
}

#define MSG_BUF_SIZE    1024

ECode ConnectionServiceWrapper::LogIncoming(
    /* [in] */ const char * fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    String s;
    s.AppendFormat("ConnectionService -> Telecom: %s", msgBuf);
    Log::D("ConnectionServiceWrapper", s.string(), args);
    return NOERROR;
}

ECode ConnectionServiceWrapper::LogOutgoing(
    /* [in] */ const char * fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    String s;
    s.AppendFormat("ConnectionService -> Telecom: %s", msgBuf);
    Log::D("ConnectionServiceWrapper", s.string(), args);
    return NOERROR;
}

ECode ConnectionServiceWrapper::QueryRemoteConnectionServices(
    /* [in] */ IRemoteServiceCallback* callback)
{
    // Only give remote connection services to this connection service if it is listed as
    // the connection manager.
    AutoPtr<IPhoneAccountHandle> simCallManager;
    mPhoneAccountRegistrar->GetSimCallManager((IPhoneAccountHandle**)&simCallManager);
    Log::D("ConnectionServiceWrapper", "queryRemoteConnectionServices finds simCallManager = %s", TO_CSTR(simCallManager));
    AutoPtr<IComponentName> componentName;
    GetComponentName((IComponentName**)&componentName);
    AutoPtr<IComponentName> simCallManagerComponentName;
    simCallManager->GetComponentName((IComponentName**)&simCallManagerComponentName);
    Boolean isEquals;
    IObject::Probe(componentName)->Equals(simCallManagerComponentName, &isEquals);
    if (simCallManager == NULL || !isEquals) {
        NoRemoteServices(callback);
        return NOERROR;
    }
    // Make a list of ConnectionServices that are listed as being associated with SIM accounts
    AutoPtr<IConcurrentHashMap> concurrentHashMap;
    CConcurrentHashMap::New(8, 0.9f, 1, (IConcurrentHashMap**)&concurrentHashMap);
    AutoPtr<ICollections> collectionsHelper;
    CCollections::AcquireSingleton((ICollections**)&collectionsHelper);
    AutoPtr<ISet> simServices;
    collectionsHelper->SetFromMap(IMap::Probe(concurrentHashMap), (ISet**)&simServices);
    AutoPtr<IList> callCapablePhoneAccounts;
    mPhoneAccountRegistrar->GetCallCapablePhoneAccounts((IList**)&callCapablePhoneAccounts);
    AutoPtr<IIterator> it;
    callCapablePhoneAccounts->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IPhoneAccountHandle> handle = IPhoneAccountHandle::Probe(obj);
        AutoPtr<IPhoneAccount> account;
        mPhoneAccountRegistrar->GetPhoneAccount(handle, (IPhoneAccount**)&account);
        Int32 capabilities;
        account->GetCapabilities(&capabilities);
        if ((capabilities & IPhoneAccount::CAPABILITY_SIM_SUBSCRIPTION) != 0) {
            AutoPtr<IComponentName> componentName;
            handle->GetComponentName((IComponentName**)&componentName);
            AutoPtr<IConnectionServiceWrapper> service;
            mConnectionServiceRepository->GetService(componentName, (IConnectionServiceWrapper**)&service);
            if (service != NULL) {
                simServices->Add(service);
            }
        }
    }
    AutoPtr<IList> simServiceComponentNames;
    CArrayList::New((IList**)&simServiceComponentNames);
    AutoPtr<IList> simServiceBinders;
    CArrayList::New((IList**)&simServiceBinders);
    Log::V("ConnectionServiceWrapper", "queryRemoteConnectionServices, simServices = %s", TO_CSTR(simServices));
    it = NULL;
    simServices->GetIterator((IIterator**)&it);
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IConnectionServiceWrapper> simService = IConnectionServiceWrapper::Probe(obj);
        if (simService == this) {
            // Only happens in the unlikely case that a SIM service is also a SIM call manager
            continue;
        }
        AutoPtr<IConnectionServiceWrapper> currentSimService = simService;
        ((ConnectionServiceWrapper*) currentSimService.Get())->mBinder->Bind(new SubQueryBindCallback(this, currentSimService, simServiceComponentNames, simServiceBinders, callback, simServices));
    }
    return NOERROR;
}

ECode ConnectionServiceWrapper::SetRemoteServices(
    /* [in] */ IRemoteServiceCallback* callback,
    /* [in] */ IList* componentNames,
    /* [in] */ IList* binders)
{
    // try {
    ECode ec = callback->OnResult(componentNames, binders);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            AutoPtr<IComponentName> componentName;
            this->GetComponentName((IComponentName**)&componentName);
            Log::E("ConnectionServiceWrapper", "Contacting ConnectionService %s", TO_CSTR(componentName));
        }
        else {
            return ec;
        }
    }
    // }
    return NOERROR;
}

ECode ConnectionServiceWrapper::NoRemoteServices(
    /* [in] */ IRemoteServiceCallback* callback)
{
    // try {
    AutoPtr<ICollections> collectionsHelper;
    CCollections::AcquireSingleton((ICollections**)&collectionsHelper);
    AutoPtr<IList> emptyList;
    collectionsHelper->GetEmptyList((IList**)&emptyList);
    ECode ec = callback->OnResult(emptyList, emptyList);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            AutoPtr<IComponentName> componentName;
            this->GetComponentName((IComponentName**)&componentName);
            Log::E("ConnectionServiceWrapper", "Contacting ConnectionService %s", TO_CSTR(componentName));
        }
        else {
            return ec;
        }
    }
    // }
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
