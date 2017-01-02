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

#include "elastos/droid/server/telecom/InCallToneMonitor.h"
#include "elastos/droid/server/telecom/Log.h"

using Elastos::Droid::Telecom::ICallState;
using Elastos::Droid::Telecom::IDisconnectCause;

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
