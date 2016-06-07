
#include "elastos/droid/server/telecom/CallsManagerListenerBase.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

CAR_INTERFACE_IMPL(CallsManagerListenerBase, Object, ICallsManagerListener)

ECode CallsManagerListenerBase::OnCallAdded(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode CallsManagerListenerBase::OnCallRemoved(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode CallsManagerListenerBase::OnCallStateChanged(
    /* [in] */ ICall* call,
    /* [in] */ Int32 oldState,
    /* [in] */ Int32 newState)
{
    return NOERROR;
}

ECode CallsManagerListenerBase::OnCallExtrasUpdated(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode CallsManagerListenerBase::OnConnectionServiceChanged(
    /* [in] */ ICall* call,
    /* [in] */ IConnectionServiceWrapper* oldService,
    /* [in] */ IConnectionServiceWrapper* newService)
{
    return NOERROR;
}

ECode CallsManagerListenerBase::OnIncomingCallAnswered(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode CallsManagerListenerBase::OnIncomingCallRejected(
    /* [in] */ ICall* call,
    /* [in] */ Boolean rejectWithMessage,
    /* [in] */ const String& textMessage)
{
    return NOERROR;
}

ECode CallsManagerListenerBase::OnForegroundCallChanged(
    /* [in] */ ICall* oldForegroundCall,
    /* [in] */ ICall* newForegroundCall)
{
    return NOERROR;
}

ECode CallsManagerListenerBase::OnAudioStateChanged(
    /* [in] */ IAudioState* oldAudioState,
    /* [in] */ IAudioState* newAudioState)
{
    return NOERROR;
}

ECode CallsManagerListenerBase::OnRingbackRequested(
    /* [in] */ ICall* call,
    /* [in] */ Boolean ringback)
{
    return NOERROR;
}

ECode CallsManagerListenerBase::OnIsConferencedChanged(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode CallsManagerListenerBase::OnIsVoipAudioModeChanged(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode CallsManagerListenerBase::OnVideoStateChanged(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode CallsManagerListenerBase::OnCallSubstateChanged(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

ECode CallsManagerListenerBase::OnCanAddCallChanged(
    /* [in] */ Boolean canAddCall)
{
    return NOERROR;
}

ECode CallsManagerListenerBase::OnProcessIncomingCall(
    /* [in] */ ICall* call)
{
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
