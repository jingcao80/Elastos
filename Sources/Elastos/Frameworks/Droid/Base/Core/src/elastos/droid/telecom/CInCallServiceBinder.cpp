
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/CInCallServiceBinder.h"
#include "elastos/droid/telecom/InCallService.h"
#include "elastos/droid/internal/os/CSomeArgsHelper.h"

using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Internal::Os::ISomeArgsHelper;
using Elastos::Droid::Internal::Os::CSomeArgsHelper;
using Elastos::Droid::Internal::Telecom::EIID_IIInCallService;
using Elastos::Droid::Telecom::InCallService;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Telecom {

CAR_OBJECT_IMPL(CInCallServiceBinder)

CAR_INTERFACE_IMPL(CInCallServiceBinder, Object, IIInCallService)

ECode CInCallServiceBinder::constructor(
    /* [in] */ IHandler* handler)
{
    mHandler = handler;
    return NOERROR;
}

ECode CInCallServiceBinder::SetInCallAdapter(
    /* [in] */ IIInCallAdapter* inCallAdapter)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(InCallService::MSG_SET_IN_CALL_ADAPTER, inCallAdapter, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CInCallServiceBinder::AddCall(
    /* [in] */ IParcelableCall* call)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(InCallService::MSG_ADD_CALL, call, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CInCallServiceBinder::UpdateCall(
    /* [in] */ IParcelableCall* call)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(InCallService::MSG_UPDATE_CALL, call, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CInCallServiceBinder::SetPostDial(
    /* [in] */ const String& callId,
    /* [in] */ const String& remaining)
{
    // TODO: Unused
    return NOERROR;
}

ECode CInCallServiceBinder::SetPostDialWait(
    /* [in] */ const String& callId,
    /* [in] */ const String& remaining)
{
    AutoPtr<ISomeArgsHelper> hlp;
    CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&hlp);
    AutoPtr<ISomeArgs> args;
    hlp->Obtain((ISomeArgs**)&args);
    AutoPtr<ICharSequence> pId;
    CString::New(callId, (ICharSequence**)&pId);
    args->SetObjectArg(1, pId);
    AutoPtr<ICharSequence> pRemain;
    CString::New(remaining, (ICharSequence**)&pRemain);
    args->SetObjectArg(2, pRemain);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(InCallService::MSG_SET_POST_DIAL_WAIT, args, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CInCallServiceBinder::OnAudioStateChanged(
    /* [in] */ IAudioState* audioState)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(InCallService::MSG_ON_AUDIO_STATE_CHANGED, audioState, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode CInCallServiceBinder::BringToForeground(
    /* [in] */ Boolean showDialpad)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(InCallService::MSG_BRING_TO_FOREGROUND, showDialpad ? 1 : 0, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos