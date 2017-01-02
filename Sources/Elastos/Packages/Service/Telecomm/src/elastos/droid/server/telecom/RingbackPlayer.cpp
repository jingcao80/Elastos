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

#include "elastos/droid/server/telecom/RingbackPlayer.h"
#include "elastos/droid/server/telecom/Call.h"
#include "elastos/droid/server/telecom/InCallTonePlayer.h"
#include "elastos/droid/server/telecom/Log.h"
#include "elastos/droid/server/telecom/ThreadUtil.h"

using Elastos::Droid::Internal::Utility::CPreconditions;
using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Droid::Telecom::ICallState;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

ECode RingbackPlayer::constructor(
    /* [in] */ CallsManager* callsManager,
    /* [in] */ InCallTonePlayer::Factory* playerFactory)
{
    mCallsManager = callsManager;
    mPlayerFactory = playerFactory;
    return NOERROR;
}

ECode RingbackPlayer::OnForegroundCallChanged(
    /* [in] */ ICall* oldForegroundCall,
    /* [in] */ ICall* newForegroundCall)
{
    if (oldForegroundCall != NULL) {
        StopRingbackForCall(oldForegroundCall);
    }
    Boolean shouldStartRinging;
    ShouldStartRinging(newForegroundCall, &shouldStartRinging);
    if (shouldStartRinging) {
        StartRingbackForCall(newForegroundCall);
    }
    return NOERROR;
}

ECode RingbackPlayer::OnConnectionServiceChanged(
    /* [in] */ ICall* call,
    /* [in] */ IConnectionServiceWrapper* oldService,
    /* [in] */ IConnectionServiceWrapper* newService)
{
    // Treat as ending or begining dialing based on the state transition.
    Boolean shouldStartRinging;
    ShouldStartRinging(call, &shouldStartRinging);
    if (shouldStartRinging) {
        StartRingbackForCall(call);
    } else if (newService == NULL) {
        StopRingbackForCall(call);
    }
    return NOERROR;
}

ECode RingbackPlayer::OnRingbackRequested(
    /* [in] */ ICall* call,
    /* [in] */ Boolean ignored)
{
    Boolean shouldStartRinging;
    ShouldStartRinging(call, &shouldStartRinging);
    if (shouldStartRinging) {
        StartRingbackForCall(call);
    } else {
        StopRingbackForCall(call);
    }
    return NOERROR;
}

ECode RingbackPlayer::StartRingbackForCall(
    /* [in] */ ICall* call)
{
    AutoPtr<IPreconditions> preconditionsHelper;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditionsHelper);
    Int32 callState;
    ((Call*) call)->GetState(&callState);
    preconditionsHelper->CheckState(callState == ICallState::DIALING);
    ThreadUtil::CheckOnMainThread();
    if (mCall.Get() == call) {
        Log::W("RingbackPlayer", "Ignoring duplicate requests to ring for %s.", TO_CSTR(call));
        return NOERROR;
    }
    if (mCall != NULL) {
        // We only get here for the foreground call so, there's no reason why there should
        // exist a current dialing call.
        Log::Wtf("RingbackPlayer", "Ringback player thinks there are two foreground-dialing calls.");
    }
    mCall = call;
    if (mTonePlayer == NULL) {
        Log::D("RingbackPlayer", "Playing the ringback tone for %s.", TO_CSTR(call));
        mPlayerFactory->CreatePlayer(InCallTonePlayer::TONE_RING_BACK, (InCallTonePlayer**)&mTonePlayer);
        mTonePlayer->StartTone();
    }
    return NOERROR;
}

ECode RingbackPlayer::StopRingbackForCall(
    /* [in] */ ICall* call)
{
    ThreadUtil::CheckOnMainThread();
    if (mCall.Get() == call) {
        // The foreground call is no longer dialing or is no longer the foreground call. In
        // either case, stop the ringback tone.
        mCall = NULL;
        if (mTonePlayer == NULL) {
            Log::W("RingbackPlayer", "No player found to stop.");
        } else {
            Log::I("RingbackPlayer", "Stopping the ringback tone for %s.", TO_CSTR(call));
            mTonePlayer->StopTone();
            mTonePlayer = NULL;
        }
    }
    return NOERROR;
}

ECode RingbackPlayer::ShouldStartRinging(
    /* [in] */ ICall* call,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isRingbackRequested;
    if (call != NULL)
        ((Call*) call)->IsRingbackRequested(&isRingbackRequested);
    Int32 callState;
    if (call != NULL)
        ((Call*) call)->GetState(&callState);
    AutoPtr<ICall> foregroundCall;
    mCallsManager->GetForegroundCall((ICall**)&foregroundCall);
    *result = call != NULL
            && foregroundCall.Get() == call
            && callState == ICallState::DIALING
            && isRingbackRequested;
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
