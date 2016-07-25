#include "Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/InCallAdapter.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

//===============================================================
// InCallAdapter::
//===============================================================
CAR_INTERFACE_IMPL(InCallAdapter, Object, IInCallAdapter)

ECode InCallAdapter::constructor(
    /* [in] */ IIInCallAdapter* adapter)
{
    mAdapter = adapter;
    return NOERROR;
}

ECode InCallAdapter::AnswerCall(
    /* [in] */ const String& callId,
    /* [in] */ Int32 videoState)
{
    mAdapter->AnswerCall(callId, videoState);
    return NOERROR;
}

ECode InCallAdapter::RejectCall(
    /* [in] */ const String& callId,
    /* [in] */ Boolean rejectWithMessage,
    /* [in] */ const String& textMessage)
{
    mAdapter->RejectCall(callId, rejectWithMessage, textMessage);
    return NOERROR;
}

ECode InCallAdapter::DisconnectCall(
    /* [in] */ const String& callId)
{
    mAdapter->DisconnectCall(callId);
    return NOERROR;
}

ECode InCallAdapter::HoldCall(
    /* [in] */ const String& callId)
{
    mAdapter->HoldCall(callId);
    return NOERROR;
}

ECode InCallAdapter::UnholdCall(
    /* [in] */ const String& callId)
{
    mAdapter->UnholdCall(callId);
    return NOERROR;
}

ECode InCallAdapter::Mute(
    /* [in] */ Boolean shouldMute)
{
    mAdapter->Mute(shouldMute);
    return NOERROR;
}

ECode InCallAdapter::SetAudioRoute(
    /* [in] */ Int32 route)
{
    mAdapter->SetAudioRoute(route);
    return NOERROR;
}

ECode InCallAdapter::PlayDtmfTone(
    /* [in] */ const String& callId,
    /* [in] */ Char32 digit)
{
    mAdapter->PlayDtmfTone(callId, digit);
    return NOERROR;
}

ECode InCallAdapter::StopDtmfTone(
    /* [in] */ const String& callId)
{
    mAdapter->StopDtmfTone(callId);
    return NOERROR;
}

ECode InCallAdapter::PostDialContinue(
    /* [in] */ const String& callId,
    /* [in] */ Boolean proceed)
{
    mAdapter->PostDialContinue(callId, proceed);
    return NOERROR;
}

ECode InCallAdapter::PhoneAccountSelected(
    /* [in] */ const String& callId,
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    mAdapter->PhoneAccountSelected(callId, accountHandle);
    return NOERROR;
}

ECode InCallAdapter::Conference(
    /* [in] */ const String& callId,
    /* [in] */ const String& otherCallId)
{
    mAdapter->Conference(callId, otherCallId);
    return NOERROR;
}

ECode InCallAdapter::SplitFromConference(
    /* [in] */ const String& callId)
{
    mAdapter->SplitFromConference(callId);
    return NOERROR;
}

ECode InCallAdapter::MergeConference(
    /* [in] */ const String& callId)
{
    mAdapter->MergeConference(callId);
    return NOERROR;
}

ECode InCallAdapter::SwapConference(
    /* [in] */ const String& callId)
{
    mAdapter->SwapConference(callId);
    return NOERROR;
}

ECode InCallAdapter::TurnProximitySensorOn()
{
    mAdapter->TurnOnProximitySensor();
    return NOERROR;
}

ECode InCallAdapter::TurnProximitySensorOff(
    /* [in] */ Boolean screenOnImmediately)
{
    mAdapter->TurnOffProximitySensor(screenOnImmediately);
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos
