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
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/CConnectionServiceAdapterStub.h"
#include "elastos/droid/internal/os/CSomeArgsHelper.h"

using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Internal::Os::ISomeArgsHelper;
using Elastos::Droid::Internal::Os::CSomeArgsHelper;
using Elastos::Droid::Internal::Telecom::EIID_IIConnectionServiceAdapter;
using Elastos::Core::CString;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Telecom {

//===============================================================
// CConnectionServiceAdapterStub::
//===============================================================
CAR_OBJECT_IMPL(CConnectionServiceAdapterStub)

CAR_INTERFACE_IMPL(CConnectionServiceAdapterStub, Object, IIConnectionServiceAdapter)

ECode CConnectionServiceAdapterStub::constructor(
    /* [in] */ IHandler* handler)
{
    mHandler = handler;
    return NOERROR;
}

ECode CConnectionServiceAdapterStub::HandleCreateConnectionComplete(
    /* [in] */ const String& id,
    /* [in] */ IConnectionRequest* request,
    /* [in] */ IParcelableConnection* connection)
{
    AutoPtr<ISomeArgsHelper> hlp;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&hlp);
    AutoPtr<ISomeArgs> args;
    hlp->Obtain((ISomeArgs**)&args);

    AutoPtr<ICharSequence> pId;
    CString::New(id, (ICharSequence**)&pId);
    args->SetObjectArg(1, pId);
    args->SetObjectArg(2, request);
    args->SetObjectArg(3, connection);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionServiceAdapterServant::MSG_HANDLE_CREATE_CONNECTION_COMPLETE,
            args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceAdapterStub::SetActive(
    /* [in] */ const String& connectionId)
{
    AutoPtr<ICharSequence> pId;
    CString::New(connectionId, (ICharSequence**)&pId);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionServiceAdapterServant::MSG_SET_ACTIVE,
            pId, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceAdapterStub::SetRinging(
    /* [in] */ const String& connectionId)
{
    AutoPtr<ICharSequence> pId;
    CString::New(connectionId, (ICharSequence**)&pId);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionServiceAdapterServant::MSG_SET_RINGING,
            pId, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceAdapterStub::SetDialing(
    /* [in] */ const String& connectionId)
{
    AutoPtr<ICharSequence> pId;
    CString::New(connectionId, (ICharSequence**)&pId);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionServiceAdapterServant::MSG_SET_DIALING,
            pId, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceAdapterStub::SetDisconnected(
    /* [in] */ const String& connectionId,
    /* [in] */ IDisconnectCause* disconnectCause)
{
    AutoPtr<ISomeArgsHelper> hlp;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&hlp);
    AutoPtr<ISomeArgs> args;
    hlp->Obtain((ISomeArgs**)&args);

    AutoPtr<ICharSequence> pId;
    CString::New(connectionId, (ICharSequence**)&pId);
    args->SetObjectArg(1, pId);
    args->SetObjectArg(2, disconnectCause);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionServiceAdapterServant::MSG_SET_DISCONNECTED,
            args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceAdapterStub::SetOnHold(
    /* [in] */ const String& connectionId)
{
    AutoPtr<ICharSequence> pId;
    CString::New(connectionId, (ICharSequence**)&pId);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionServiceAdapterServant::MSG_SET_ON_HOLD,
            pId, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceAdapterStub::SetRingbackRequested(
    /* [in] */ const String& connectionId,
    /* [in] */ Boolean ringback)
{
    AutoPtr<ICharSequence> pId;
    CString::New(connectionId, (ICharSequence**)&pId);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionServiceAdapterServant::MSG_SET_RINGBACK_REQUESTED,
            ringback ? 1 : 0, 0, pId, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceAdapterStub::SetCallCapabilities(
    /* [in] */ const String& connectionId,
    /* [in] */ Int32 callCapabilities)
{
    AutoPtr<ICharSequence> pId;
    CString::New(connectionId, (ICharSequence**)&pId);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionServiceAdapterServant::MSG_SET_CALL_CAPABILITIES,
            callCapabilities, 0, pId, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceAdapterStub::SetIsConferenced(
    /* [in] */ const String& callId,
    /* [in] */ const String& conferenceCallId)
{
    AutoPtr<ISomeArgsHelper> hlp;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&hlp);
    AutoPtr<ISomeArgs> args;
    hlp->Obtain((ISomeArgs**)&args);

    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    args->SetObjectArg(1, pId);

    AutoPtr<ICharSequence> pConferenceCallId;
    CString::New(conferenceCallId, (ICharSequence**)&pConferenceCallId);
    args->SetObjectArg(2, pConferenceCallId);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionServiceAdapterServant::MSG_SET_IS_CONFERENCED,
            args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceAdapterStub::AddConferenceCall(
    /* [in] */ const String& callId,
    /* [in] */ IParcelableConference* parcelableConference)
{
    AutoPtr<ISomeArgsHelper> hlp;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&hlp);
    AutoPtr<ISomeArgs> args;
    hlp->Obtain((ISomeArgs**)&args);

    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    args->SetObjectArg(1, pId);
    args->SetObjectArg(2, parcelableConference);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionServiceAdapterServant::MSG_ADD_CONFERENCE_CALL,
            args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceAdapterStub::RemoveCall(
    /* [in] */ const String& connectionId)
{
    AutoPtr<ICharSequence> pId;
    CString::New(connectionId, (ICharSequence**)&pId);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionServiceAdapterServant::MSG_REMOVE_CALL,
            pId, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceAdapterStub::OnPostDialWait(
    /* [in] */ const String& connectionId,
    /* [in] */ const String& remainingDigits)
{
    AutoPtr<ISomeArgsHelper> hlp;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&hlp);
    AutoPtr<ISomeArgs> args;
    hlp->Obtain((ISomeArgs**)&args);

    AutoPtr<ICharSequence> pId;
    CString::New(connectionId, (ICharSequence**)&pId);
    args->SetObjectArg(1, pId);

    AutoPtr<ICharSequence> pRemainingDigits;
    CString::New(remainingDigits, (ICharSequence**)&pRemainingDigits);
    args->SetObjectArg(2, pRemainingDigits);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionServiceAdapterServant::MSG_ON_POST_DIAL_WAIT,
            args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceAdapterStub::QueryRemoteConnectionServices(
    /* [in] */ IRemoteServiceCallback* callback)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionServiceAdapterServant::MSG_QUERY_REMOTE_CALL_SERVICES,
            callback, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceAdapterStub::SetVideoState(
    /* [in] */ const String& connectionId,
    /* [in] */ Int32 videoState)
{
    AutoPtr<ICharSequence> pId;
    CString::New(connectionId, (ICharSequence**)&pId);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionServiceAdapterServant::MSG_SET_VIDEO_STATE,
            videoState, 0, pId, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceAdapterStub::SetVideoProvider(
    /* [in] */ const String& connectionId,
    /* [in] */ IIVideoProvider* videoProvider)
{
    AutoPtr<ISomeArgsHelper> hlp;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&hlp);
    AutoPtr<ISomeArgs> args;
    hlp->Obtain((ISomeArgs**)&args);

    AutoPtr<ICharSequence> pConnectionId;
    CString::New(connectionId, (ICharSequence**)&pConnectionId);
    args->SetObjectArg(1, pConnectionId);
    args->SetObjectArg(2, videoProvider);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionServiceAdapterServant::MSG_SET_VIDEO_CALL_PROVIDER,
            args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceAdapterStub::SetIsVoipAudioMode(
    /* [in] */ const String& connectionId,
    /* [in] */ Boolean isVoip)
{
    AutoPtr<ICharSequence> pId;
    CString::New(connectionId, (ICharSequence**)&pId);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionServiceAdapterServant::MSG_SET_IS_VOIP_AUDIO_MODE,
            isVoip ? 1 : 0, 0, pId, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceAdapterStub::SetStatusHints(
    /* [in] */ const String& connectionId,
    /* [in] */ IStatusHints* statusHints)
{
    AutoPtr<ISomeArgsHelper> hlp;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&hlp);
    AutoPtr<ISomeArgs> args;
    hlp->Obtain((ISomeArgs**)&args);

    AutoPtr<ICharSequence> pId;
    CString::New(connectionId, (ICharSequence**)&pId);
    args->SetObjectArg(1, pId);
    args->SetObjectArg(2, statusHints);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionServiceAdapterServant::MSG_SET_STATUS_HINTS,
            args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceAdapterStub::SetAddress(
    /* [in] */ const String& connectionId,
    /* [in] */ IUri* address,
    /* [in] */ Int32 presentation)
{
    AutoPtr<ISomeArgsHelper> hlp;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&hlp);
    AutoPtr<ISomeArgs> args;
    hlp->Obtain((ISomeArgs**)&args);

    AutoPtr<ICharSequence> pId;
    CString::New(connectionId, (ICharSequence**)&pId);
    args->SetObjectArg(1, pId);
    args->SetObjectArg(2, (IInterface*)(IParcelable*)address);
    AutoPtr<IInteger32> pPresentation;
    CInteger32::New(presentation, (IInteger32**)&pPresentation);
    args->SetObjectArg(3, pPresentation);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionServiceAdapterServant::MSG_SET_ADDRESS,
            args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceAdapterStub::SetCallerDisplayName(
    /* [in] */ const String& connectionId,
    /* [in] */ const String& callerDisplayName,
    /* [in] */ Int32 presentation)
{
    AutoPtr<ISomeArgsHelper> hlp;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&hlp);
    AutoPtr<ISomeArgs> args;
    hlp->Obtain((ISomeArgs**)&args);

    AutoPtr<ICharSequence> pId;
    CString::New(connectionId, (ICharSequence**)&pId);
    args->SetObjectArg(1, pId);
    AutoPtr<ICharSequence> pName;
    CString::New(callerDisplayName, (ICharSequence**)&pName);
    args->SetObjectArg(2, pName);
    AutoPtr<IInteger32> pPresentation;
    CInteger32::New(presentation, (IInteger32**)&pPresentation);
    args->SetObjectArg(3, pPresentation);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionServiceAdapterServant::MSG_SET_CALLER_DISPLAY_NAME,
            args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceAdapterStub::SetConferenceableConnections(
    /* [in] */ const String& connectionId,
    /* [in] */ IList* conferenceableConnectionIds)
{
    AutoPtr<ISomeArgsHelper> hlp;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&hlp);
    AutoPtr<ISomeArgs> args;
    hlp->Obtain((ISomeArgs**)&args);

    AutoPtr<ICharSequence> pId;
    CString::New(connectionId, (ICharSequence**)&pId);
    args->SetObjectArg(1, pId);
    args->SetObjectArg(2, (IInterface*)(ICollection*)conferenceableConnectionIds);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionServiceAdapterServant::MSG_SET_CONFERENCEABLE_CONNECTIONS,
            args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos