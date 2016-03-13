
#include "elastos/droid/telephony/CPreciseCallState.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Telephony {

CAR_INTERFACE_IMPL_2(CPreciseCallState, Object, IPreciseCallState, IParcelable)

CAR_OBJECT_IMPL(CPreciseCallState)

CPreciseCallState::CPreciseCallState()
    : mRingingCallState(PRECISE_CALL_STATE_NOT_VALID)
    , mForegroundCallState(PRECISE_CALL_STATE_NOT_VALID)
    , mBackgroundCallState(PRECISE_CALL_STATE_NOT_VALID)
    , mDisconnectCause(IDisconnectCause::NOT_VALID)
    , mPreciseDisconnectCause(IPreciseDisconnectCause::NOT_VALID)
{
}

ECode CPreciseCallState::constructor()
{
    return NOERROR;
}

ECode CPreciseCallState::constructor(
    /* [in] */ Int32 ringingCall,
    /* [in] */ Int32 foregroundCall,
    /* [in] */ Int32 backgroundCall,
    /* [in] */ Int32 disconnectCause,
    /* [in] */ Int32 preciseDisconnectCause)
{
    mRingingCallState = ringingCall;
    mForegroundCallState = foregroundCall;
    mBackgroundCallState = backgroundCall;
    mDisconnectCause = disconnectCause;
    mPreciseDisconnectCause = preciseDisconnectCause;
    return NOERROR;
}

ECode CPreciseCallState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mRingingCallState);
    source->ReadInt32(&mForegroundCallState);
    source->ReadInt32(&mBackgroundCallState);
    source->ReadInt32(&mDisconnectCause);
    source->ReadInt32(&mPreciseDisconnectCause);
    return NOERROR;
}

ECode CPreciseCallState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mRingingCallState);
    dest->WriteInt32(mForegroundCallState);
    dest->WriteInt32(mBackgroundCallState);
    dest->WriteInt32(mDisconnectCause);
    dest->WriteInt32(mPreciseDisconnectCause);
    return NOERROR;
}

ECode CPreciseCallState::GetRingingCallState(
    /* [out] */ Int32* ringingCallState)
{
    VALIDATE_NOT_NULL(ringingCallState);
    *ringingCallState = mRingingCallState;
    return NOERROR;
}

ECode CPreciseCallState::GetForegroundCallState(
    /* [out] */ Int32* foregroundCallState)
{
    VALIDATE_NOT_NULL(foregroundCallState);
    *foregroundCallState = mForegroundCallState;
    return NOERROR;
}

ECode CPreciseCallState::GetBackgroundCallState(
    /* [out] */ Int32* backgroundCallState)
{
    VALIDATE_NOT_NULL(backgroundCallState);
    *backgroundCallState = mBackgroundCallState;
    return NOERROR;
}

ECode CPreciseCallState::GetDisconnectCause(
    /* [out] */ Int32* disconnectCause)
{
    VALIDATE_NOT_NULL(disconnectCause);
    *disconnectCause = mDisconnectCause;
    return NOERROR;
}

ECode CPreciseCallState::GetPreciseDisconnectCause(
    /* [out] */ Int32* preciseDisconnectCause)
{
    VALIDATE_NOT_NULL(preciseDisconnectCause);
    *preciseDisconnectCause = mPreciseDisconnectCause;
    return NOERROR;
}

ECode CPreciseCallState::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    Int32 prime = 31;
    Int32 result = 1;
    result = prime * result + mRingingCallState;
    result = prime * result + mForegroundCallState;
    result = prime * result + mBackgroundCallState;
    result = prime * result + mDisconnectCause;
    result = prime * result + mPreciseDisconnectCause;
    *hashCode = result;
    return NOERROR;
}

ECode CPreciseCallState::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    if (other == NULL) {
        *res = FALSE;
        return NOERROR;
    }

    if(IPreciseCallState::Probe(other) == NULL) {
        *res = FALSE;
        return NOERROR;
    }

    Int32 ringingCallState, foregroundCallState, backgroundCallState, disconnectCause, preciseDisconnectCause;
    AutoPtr<IPreciseCallState> o = IPreciseCallState::Probe(other);
    o->GetRingingCallState(&ringingCallState);
    o->GetForegroundCallState(&foregroundCallState);
    o->GetBackgroundCallState(&backgroundCallState);
    o->GetDisconnectCause(&disconnectCause);
    o->GetPreciseDisconnectCause(&preciseDisconnectCause);

    *res = (mRingingCallState == ringingCallState &&
        mForegroundCallState == foregroundCallState &&
        mBackgroundCallState == backgroundCallState &&
        mDisconnectCause == disconnectCause &&
        mPreciseDisconnectCause == preciseDisconnectCause);

    return NOERROR;
}

ECode CPreciseCallState::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;
    sb.Append("Ringing call state: ");
    sb.Append(mRingingCallState);
    sb.Append(", Foreground call state: ");
    sb.Append(mForegroundCallState);
    sb.Append(", Background call state: ");
    sb.Append(mBackgroundCallState);
    sb.Append(", Disconnect cause: ");
    sb.Append(mDisconnectCause);
    sb.Append(", Precise disconnect cause: ");
    sb.Append(mPreciseDisconnectCause);

    *str = sb.ToString();
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
