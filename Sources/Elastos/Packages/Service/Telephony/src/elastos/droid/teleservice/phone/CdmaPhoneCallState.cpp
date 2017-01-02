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

#include "elastos/droid/teleservice/phone/CdmaPhoneCallState.h"

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

ECode CdmaPhoneCallState::CdmaPhoneCallStateInit()
{
    mCurrentCallState = IDLE;
    mPreviousCallState = IDLE;
    mThreeWayCallOrigStateDialing = FALSE;
    mAddCallMenuStateAfterCW = TRUE;
    return NOERROR;
}

ECode CdmaPhoneCallState::GetCurrentCallState(
    /* [out] */ PhoneCallState* state)
{
    VALIDATE_NOT_NULL(state)

    *state = mCurrentCallState;
    return NOERROR;
}

ECode CdmaPhoneCallState::SetCurrentCallState(
    /* [in] */ PhoneCallState newState)
{
    mPreviousCallState = mCurrentCallState;
    mCurrentCallState = newState;

    //Reset the 3Way display boolean
    mThreeWayCallOrigStateDialing = FALSE;

    //Set mAddCallMenuStateAfterCW to true
    //if the current state is being set to SINGLE_ACTIVE
    //and previous state was IDLE as we could reach the SINGLE_ACTIVE
    //from CW ignore too. For all other cases let the timer or
    //specific calls to setAddCallMenuStateAfterCallWaiting set
    //mAddCallMenuStateAfterCW.
    if ((mCurrentCallState == SINGLE_ACTIVE) && (mPreviousCallState == IDLE)) {
        mAddCallMenuStateAfterCW = TRUE;
    }
    return NOERROR;
}

ECode CdmaPhoneCallState::IsThreeWayCallOrigStateDialing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mThreeWayCallOrigStateDialing;
    return NOERROR;
}

ECode CdmaPhoneCallState::SetThreeWayCallOrigState(
    /* [in] */ Boolean newState)
{
    mThreeWayCallOrigStateDialing = newState;
    return NOERROR;
}

ECode CdmaPhoneCallState::GetAddCallMenuStateAfterCallWaiting(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mAddCallMenuStateAfterCW;
    return NOERROR;
}

ECode CdmaPhoneCallState::SetAddCallMenuStateAfterCallWaiting(
    /* [in] */ Boolean newState)
{
    mAddCallMenuStateAfterCW = newState;
    return NOERROR;
}

ECode CdmaPhoneCallState::GetPreviousCallState(
    /* [out] */ PhoneCallState* state)
{
    VALIDATE_NOT_NULL(state)

    *state = mPreviousCallState;
    return NOERROR;
}

ECode CdmaPhoneCallState::ResetCdmaPhoneCallState()
{
    mCurrentCallState = IDLE;
    mPreviousCallState = IDLE;
    mThreeWayCallOrigStateDialing = FALSE;
    mAddCallMenuStateAfterCW = TRUE;
    return NOERROR;
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos