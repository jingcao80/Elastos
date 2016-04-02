
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/CConnectionServiceBinder.h"
#include "elastos/droid/internal/os/CSomeArgsHelper.h"

using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Internal::Os::ISomeArgsHelper;
using Elastos::Droid::Internal::Os::CSomeArgsHelper;
using Elastos::Droid::Telecomm::Internal::EIID_IIConnectionService;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CAR_OBJECT_IMPL(CConnectionServiceBinder)

CAR_INTERFACE_IMPL_2(CConnectionServiceBinder, Object, IIConnectionService, IBinder)

ECode CConnectionServiceBinder::constructor(
    /* [in] */ IHandler* handler)
{
    mHandler = handler;
    return NOERROR;
}

ECode CConnectionServiceBinder::AddConnectionServiceAdapter(
    /* [in] */ IIConnectionServiceAdapter* adapter)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionService::MSG_ADD_CONNECTION_SERVICE_ADAPTER, adapter, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceBinder::RemoveConnectionServiceAdapter(
    /* [in] */ IIConnectionServiceAdapter* adapter)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionService::MSG_REMOVE_CONNECTION_SERVICE_ADAPTER, adapter, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceBinder::CreateConnection(
    /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
    /* [in] */ const String& id,
    /* [in] */ IConnectionRequest* request,
    /* [in] */ Boolean isIncoming,
    /* [in] */ Boolean isUnknown)
{
    AutoPtr<ISomeArgsHelper> hlp;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&hlp);
    AutoPtr<ISomeArgs> args;
    hlp->Obtain((ISomeArgs**)&args);
    args->SetObjectArg(1, connectionManagerPhoneAccount);

    AutoPtr<ICharSequence> pId;
    CString::New(id, (ICharSequence**)&pId);
    args->SetObjectArg(2, pId);

    args->SetObjectArg(3, request);

    args->SetInt32Arg(1, isIncoming ? 1 : 0);

    args->SetInt32Arg(2, isUnknown ? 1 : 0);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionService::MSG_CREATE_CONNECTION, args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceBinder::Abort(
    /* [in] */ const String& callId)
{
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionService::MSG_ABORT, pId, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceBinder::AnswerVideo(
    /* [in] */ const String& callId,
    /* [in] */ Int32 videoState)
{
    AutoPtr<ISomeArgsHelper> hlp;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&hlp);
    AutoPtr<ISomeArgs> args;
    hlp->Obtain((ISomeArgs**)&args);

    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    args->SetObjectArg(1, pId);

    args->SetInt32Arg(1, videoState);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionService::MSG_ANSWER_VIDEO, args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceBinder::Answer(
    /* [in] */ const String& callId)
{
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionService::MSG_ANSWER, pId, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceBinder::Reject(
    /* [in] */ const String& callId)
{
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionService::MSG_REJECT, pId, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceBinder::Disconnect(
    /* [in] */ const String& callId)
{
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionService::MSG_DISCONNECT, pId, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceBinder::Hold(
    /* [in] */ const String& callId)
{
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionService::MSG_HOLD, pId, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceBinder::Unhold(
    /* [in] */ const String& callId)
{
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionService::MSG_UNHOLD, pId, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceBinder::OnAudioStateChanged(
    /* [in] */ const String& callId,
    /* [in] */ IAudioState* audioState)
{
    AutoPtr<ISomeArgsHelper> hlp;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&hlp);
    AutoPtr<ISomeArgs> args;
    hlp->Obtain((ISomeArgs**)&args);

    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    args->SetObjectArg(1, pId);

    args->SetObjectArg(2, audioState);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionService::MSG_ON_AUDIO_STATE_CHANGED, args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceBinder::PlayDtmfTone(
    /* [in] */ const String& callId,
    /* [in] */ Char32 digit)
{
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionService::MSG_PLAY_DTMF_TONE, digit, 0, pId, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceBinder::StopDtmfTone(
    /* [in] */ const String& callId)
{
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionService::MSG_STOP_DTMF_TONE, pId, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceBinder::Conference(
    /* [in] */ const String& callId1,
    /* [in] */ const String& callId2)
{
    AutoPtr<ISomeArgsHelper> hlp;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&hlp);
    AutoPtr<ISomeArgs> args;
    hlp->Obtain((ISomeArgs**)&args);

    AutoPtr<ICharSequence> pId1;
    CString::New(callId1, (ICharSequence**)&pId1);
    args->SetObjectArg(1, pId1);

    AutoPtr<ICharSequence> pId2;
    CString::New(callId2, (ICharSequence**)&pId2);
    args->SetObjectArg(2, pId2);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionService::MSG_CONFERENCE, args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceBinder::SplitFromConference(
    /* [in] */ const String& callId)
{
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionService::MSG_SPLIT_FROM_CONFERENCE, pId, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceBinder::MergeConference(
    /* [in] */ const String& callId)
{
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionService::MSG_MERGE_CONFERENCE, pId, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceBinder::SwapConference(
    /* [in] */ const String& callId)
{
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionService::MSG_SWAP_CONFERENCE, pId, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceBinder::OnPostDialContinue(
    /* [in] */ const String& callId,
    /* [in] */ Boolean proceed)
{
    AutoPtr<ISomeArgsHelper> hlp;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&hlp);
    AutoPtr<ISomeArgs> args;
    hlp->Obtain((ISomeArgs**)&args);

    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    args->SetObjectArg(1, pId);

    args->SetInt32Arg(1, proceed ? 1 : 0);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IConnectionService::MSG_ON_POST_DIAL_CONTINUE, args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CConnectionServiceBinder::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return NOERROR;
}

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos