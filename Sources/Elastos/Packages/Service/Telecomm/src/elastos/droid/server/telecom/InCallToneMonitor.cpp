
#include "elastos/droid/server/telecom/InCallToneMonitor.h"
#include "elastos/droid/server/telecom/Log.h"

using Elastos::Droid::Telecomm::Telecom::ICallState;
using Elastos::Droid::Telecomm::Telecom::IDisconnectCause;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

ECode InCallToneMonitor::constructor(
    /* [in] */ InCallTonePlayer::Factory* playerFactory,
    /* [in] */ CallsManager* callsManager)
{
    mPlayerFactory = playerFactory;
    mCallsManager = callsManager;
    return NOERROR;
}

ECode InCallToneMonitor::OnCallStateChanged(
    /* [in] */ ICall* call,
    /* [in] */ Int32 oldState,
    /* [in] */ Int32 newState)
{
    AutoPtr<ICall> foregroundCall;
    mCallsManager->GetForegroundCall((ICall**)&foregroundCall);
    if (foregroundCall.Get() != call) {
        // We only play tones for foreground calls.
        return NOERROR;
    }
    AutoPtr<IDisconnectCause> disconnectCause;
    ((Call*) call)->GetDisconnectCause((IDisconnectCause**)&disconnectCause);
    if (newState == ICallState::DISCONNECTED && disconnectCause != NULL) {
        Int32 toneToPlay = InCallTonePlayer::TONE_INVALID;
        Log::V("InCallToneMonitor", "Disconnect cause: %s.", TO_CSTR(disconnectCause));
        Int32 tone;
        disconnectCause->GetTone(&tone);
        switch(tone) {
            case IToneGenerator::TONE_SUP_BUSY:
                toneToPlay = InCallTonePlayer::TONE_BUSY;
                break;
            case IToneGenerator::TONE_SUP_CONGESTION:
                toneToPlay = InCallTonePlayer::TONE_CONGESTION;
                break;
            case IToneGenerator::TONE_CDMA_REORDER:
                toneToPlay = InCallTonePlayer::TONE_REORDER;
                break;
            case IToneGenerator::TONE_CDMA_ABBR_INTERCEPT:
                toneToPlay = InCallTonePlayer::TONE_INTERCEPT;
                break;
            case IToneGenerator::TONE_CDMA_CALLDROP_LITE:
                toneToPlay = InCallTonePlayer::TONE_CDMA_DROP;
                break;
            case IToneGenerator::TONE_SUP_ERROR:
                toneToPlay = InCallTonePlayer::TONE_UNOBTAINABLE_NUMBER;
                break;
            case IToneGenerator::TONE_PROP_PROMPT:
                toneToPlay = InCallTonePlayer::TONE_CALL_ENDED;
                break;
        }
        Log::D("InCallToneMonitor", "Found a disconnected call with tone to play %d.", toneToPlay);
        if (toneToPlay != InCallTonePlayer::TONE_INVALID) {
            AutoPtr<InCallTonePlayer> inCallTonePlayer;
            mPlayerFactory->CreatePlayer(toneToPlay, (InCallTonePlayer**)&inCallTonePlayer);
            inCallTonePlayer->StartTone();
        }
    }
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
