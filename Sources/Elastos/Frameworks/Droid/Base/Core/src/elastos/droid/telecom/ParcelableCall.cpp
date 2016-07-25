#include "Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/ParcelableCall.h"
#include "elastos/droid/telecom/CVideoCallImpl.h"

using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;

namespace Elastos {
namespace Droid {
namespace Telecom {

//===============================================================
// ParcelableCall::
//===============================================================
CAR_INTERFACE_IMPL_2(ParcelableCall, Object, IParcelableCall, IParcelable)

ParcelableCall::ParcelableCall()
    : mId("")
    , mState(0)
    , mCapabilities(0)
    , mProperties(0)
    , mConnectTimeMillis(0)
    , mHandlePresentation(0)
    , mCallerDisplayNamePresentation(0)
    , mVideoState(0)
{}

ECode ParcelableCall::constructor()
{
    return NOERROR;
}

ECode ParcelableCall::constructor(
    /* [in] */ const String& id,
    /* [in] */ Int32 state,
    /* [in] */ IDisconnectCause* disconnectCause,
    /* [in] */ IList* cannedSmsResponses,
    /* [in] */ Int32 capabilities,
    /* [in] */ Int32 properties,
    /* [in] */ Int64 connectTimeMillis,
    /* [in] */ IUri* handle,
    /* [in] */ Int32 handlePresentation,
    /* [in] */ const String& callerDisplayName,
    /* [in] */ Int32 callerDisplayNamePresentation,
    /* [in] */ IGatewayInfo* gatewayInfo,
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [in] */ IIVideoProvider* videoCallProvider,
    /* [in] */ const String& parentCallId,
    /* [in] */ IList* childCallIds,
    /* [in] */ IStatusHints* statusHints,
    /* [in] */ Int32 videoState,
    /* [in] */ IList* conferenceableCallIds,
    /* [in] */ IBundle* extras)
{
    mId = id;
    mState = state;
    mDisconnectCause = disconnectCause;
    mCannedSmsResponses = cannedSmsResponses;
    mCapabilities = capabilities;
    mProperties = properties;
    mConnectTimeMillis = connectTimeMillis;
    mHandle = handle;
    mHandlePresentation = handlePresentation;
    mCallerDisplayName = callerDisplayName;
    mCallerDisplayNamePresentation = callerDisplayNamePresentation;
    mGatewayInfo = gatewayInfo;
    mAccountHandle = accountHandle;
    mVideoCallProvider = videoCallProvider;
    mParentCallId = parentCallId;
    mChildCallIds = childCallIds;
    mStatusHints = statusHints;
    mVideoState = videoState;
    AutoPtr<ICollections> cls;
    CCollections::AcquireSingleton((ICollections**)&cls);
    cls->UnmodifiableList(conferenceableCallIds, (IList**)&mConferenceableCallIds);
    mExtras = extras;
    return NOERROR;
}

ECode ParcelableCall::GetId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mId;
    return NOERROR;
}

ECode ParcelableCall::GetState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mState;
    return NOERROR;
}

ECode ParcelableCall::GetDisconnectCause(
    /* [out] */ IDisconnectCause** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDisconnectCause;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableCall::GetCannedSmsResponses(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCannedSmsResponses;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableCall::GetCapabilities(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCapabilities;
    return NOERROR;
}

ECode ParcelableCall::GetProperties(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mProperties;
    return NOERROR;
}

ECode ParcelableCall::GetConnectTimeMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mConnectTimeMillis;
    return NOERROR;
}

ECode ParcelableCall::GetHandle(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHandle;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableCall::GetHandlePresentation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHandlePresentation;
    return NOERROR;
}

ECode ParcelableCall::GetCallerDisplayName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCallerDisplayName;
    return NOERROR;
}

ECode ParcelableCall::GetCallerDisplayNamePresentation(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCallerDisplayNamePresentation;
    return NOERROR;
}

ECode ParcelableCall::GetGatewayInfo(
    /* [out] */ IGatewayInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mGatewayInfo;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableCall::GetAccountHandle(
    /* [out] */ IPhoneAccountHandle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAccountHandle;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableCall::GetVideoCall(
    /* [out] */ IInCallServiceVideoCall** result)
{
    VALIDATE_NOT_NULL(result)
    if (mVideoCall == NULL && mVideoCallProvider != NULL) {
        CVideoCallImpl::New(mVideoCallProvider, (IInCallServiceVideoCall**)&mVideoCall);
    }

    *result = mVideoCall;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableCall::GetParentCallId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mParentCallId;
    return NOERROR;
}

ECode ParcelableCall::GetChildCallIds(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mChildCallIds;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableCall::GetConferenceableCallIds(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mConferenceableCallIds;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableCall::GetStatusHints(
    /* [out] */ IStatusHints** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStatusHints;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableCall::GetVideoState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mVideoState;
    return NOERROR;
}

ECode ParcelableCall::GetExtras(
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mExtras;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ParcelableCall::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return NOERROR;
}

ECode ParcelableCall::WriteToParcel(
    /* [in] */ IParcel* destination)
{
    destination->WriteString(mId);
    destination->WriteInt32(mState);
    IParcelable::Probe(mDisconnectCause)->WriteToParcel(destination);
    IParcelable::Probe(mCannedSmsResponses)->WriteToParcel(destination);
    destination->WriteInt32(mCapabilities);
    destination->WriteInt32(mProperties);
    destination->WriteInt64(mConnectTimeMillis);
    IParcelable::Probe(mHandle)->WriteToParcel(destination);
    destination->WriteInt32(mHandlePresentation);
    destination->WriteString(mCallerDisplayName);
    destination->WriteInt32(mCallerDisplayNamePresentation);
    IParcelable::Probe(mGatewayInfo)->WriteToParcel(destination);
    IParcelable::Probe(mAccountHandle)->WriteToParcel(destination);
    // destination->WriteStrongBinder(
    //         mVideoCallProvider != null ? mVideoCallProvider.asBinder() : null);
    destination->WriteString(mParentCallId);
    IParcelable::Probe(mChildCallIds)->WriteToParcel(destination);
    IParcelable::Probe(mStatusHints)->WriteToParcel(destination);
    destination->WriteInt32(mVideoState);
    IParcelable::Probe(mConferenceableCallIds)->WriteToParcel(destination);
    IParcelable::Probe(mExtras)->WriteToParcel(destination);
    return NOERROR;
}

ECode ParcelableCall::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(3);
    AutoPtr<ICharSequence> pId;
    CString::New(mId, (ICharSequence**)&pId);
    (*arr)[0] = pId;
    AutoPtr<ICharSequence> pParentCallId;
    CString::New(mParentCallId, (ICharSequence**)&pParentCallId);
    (*arr)[1] = pParentCallId;
    (*arr)[2] = mChildCallIds;
    *result = StringUtils::Format(String("[%s, parent:%s, children:%s]"), arr);
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos
