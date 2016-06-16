
#include "elastos/droid/phone/CdmaPhoneCallState.h"

namespace Elastos {
namespace Droid {
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
} // namespace Droid
} // namespace Elastos