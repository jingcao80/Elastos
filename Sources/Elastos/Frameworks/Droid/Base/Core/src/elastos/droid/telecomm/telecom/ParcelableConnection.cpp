
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/ParcelableConnection.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

//===============================================================
// ParcelableConnection::
//===============================================================
CAR_INTERFACE_IMPL_2(ParcelableConnection, Object, IParcelableConnection, IParcelable)

ECode ParcelableConnection::constructor()
{
    return NOERROR;
}

ECode ParcelableConnection::constructor(
    /* [in] */ IPhoneAccountHandle* phoneAccount,
    /* [in] */ Int32 state,
    /* [in] */ Int32 capabilities,
    /* [in] */ IUri* address,
    /* [in] */ Int32 addressPresentation,
    /* [in] */ String callerDisplayName,
    /* [in] */ Int32 callerDisplayNamePresentation,
    /* [in] */ IIVideoProvider* videoProvider,
    /* [in] */ Int32 videoState,
    /* [in] */ Boolean ringbackRequested,
    /* [in] */ Boolean isVoipAudioMode,
    /* [in] */ IStatusHints* statusHints,
    /* [in] */ IDisconnectCause* disconnectCause,
    /* [in] */ IList* conferenceableConnectionIds)
{
    mPhoneAccount = phoneAccount;
    mState = state;
    mCapabilities = capabilities;
    mAddress = address;
    mAddressPresentation = addressPresentation;
    mCallerDisplayName = callerDisplayName;
    mCallerDisplayNamePresentation = callerDisplayNamePresentation;
    mVideoProvider = videoProvider;
    mVideoState = videoState;
    mRingbackRequested = ringbackRequested;
    mIsVoipAudioMode = isVoipAudioMode;
    mStatusHints = statusHints;
    mDisconnectCause = disconnectCause;
    mConferenceableConnectionIds = conferenceableConnectionIds;
    return NOERROR;
}

ECode ParcelableConnection::GetPhoneAccount(
    /* [out] */ IPhoneAccountHandle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPhoneAccount;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableConnection::GetState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mState;
    return NOERROR;
}

ECode ParcelableConnection::GetCapabilities(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCapabilities;
    return NOERROR;
}

ECode ParcelableConnection::GetHandle(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAddress;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableConnection::GetHandlePresentation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAddressPresentation;
    return NOERROR;
}

ECode ParcelableConnection::GetCallerDisplayName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCallerDisplayName;
    return NOERROR;
}

ECode ParcelableConnection::GetCallerDisplayNamePresentation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCallerDisplayNamePresentation;
    return NOERROR;
}

ECode ParcelableConnection::GetVideoProvider(
    /* [out] */ IIVideoProvider** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mVideoProvider;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableConnection::GetVideoState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mVideoState;
    return NOERROR;
}

ECode ParcelableConnection::IsRingbackRequested(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRingbackRequested;
    return NOERROR;
}

ECode ParcelableConnection::GetIsVoipAudioMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsVoipAudioMode;
    return NOERROR;
}

ECode ParcelableConnection::GetStatusHints(
    /* [out] */ IStatusHints** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStatusHints;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableConnection::GetDisconnectCause(
    /* [out] */ IDisconnectCause** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDisconnectCause;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableConnection::GetConferenceableConnectionIds(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mConferenceableConnectionIds;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableConnection::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder sb;
    sb.Append("ParcelableConnection [act:");
    sb.Append(mPhoneAccount);
    sb.Append(", state:");
    sb.Append(mState);
    sb.Append(", capabilities:");
    //sb.Append(PhoneCapabilities.toString(mCapabilities))
    *result = sb.ToString();
    return NOERROR;
}

ECode ParcelableConnection::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return NOERROR;
}

ECode ParcelableConnection::WriteToParcel(
    /* [in] */ IParcel* destination)
{
    IParcelable::Probe(mPhoneAccount)->WriteToParcel(destination);
    destination->WriteInt32(mState);
    destination->WriteInt32(mCapabilities);
    IParcelable::Probe(mAddress)->WriteToParcel(destination);
    destination->WriteInt32(mAddressPresentation);
    destination->WriteString(mCallerDisplayName);
    destination->WriteInt32(mCallerDisplayNamePresentation);
    // destination.writeStrongBinder(
    //         mVideoProvider != null ? mVideoProvider.asBinder() : null);
    destination->WriteInt32(mVideoState);
    destination->WriteByte((byte) (mRingbackRequested ? 1 : 0));
    destination->WriteByte((byte) (mIsVoipAudioMode ? 1 : 0));
    IParcelable::Probe(mStatusHints)->WriteToParcel(destination);
    IParcelable::Probe(mDisconnectCause)->WriteToParcel(destination);
    IParcelable::Probe(mConferenceableConnectionIds)->WriteToParcel(destination);
    return NOERROR;
}

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos