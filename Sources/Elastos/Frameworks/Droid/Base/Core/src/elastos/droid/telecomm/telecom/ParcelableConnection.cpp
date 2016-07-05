
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/ParcelableConnection.h"
#include "elastos/droid/telecomm/telecom/CPhoneCapabilities.h"
#include "elastos/droid/telecomm/telecom/CPhoneAccountHandle.h"
#include "elastos/droid/telecomm/telecom/CStatusHints.h"
#include "elastos/droid/telecomm/telecom/CDisconnectCause.h"
#include "elastos/droid/net/CUriHelper.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

//===============================================================
// ParcelableConnection::
//===============================================================
CAR_INTERFACE_IMPL_2(ParcelableConnection, Object, IParcelableConnection, IParcelable)

ParcelableConnection::ParcelableConnection()
    : mState(0)
    , mCapabilities(0)
    , mProperties(0)
    , mAddressPresentation(0)
    , mCallerDisplayNamePresentation(0)
    , mVideoState(0)
    , mRingbackRequested(FALSE)
    , mIsVoipAudioMode(FALSE)
    , mCallSubstate(0)
{
}

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
    /* [in] */ const String& callerDisplayName,
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

ECode ParcelableConnection::GetProperties(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mProperties;
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

ECode ParcelableConnection::GetCallSubstate(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCallSubstate;
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
    AutoPtr<IPhoneCapabilities> pc;
    CPhoneCapabilities::AcquireSingleton((IPhoneCapabilities**)&pc);
    String str;
    pc->ToString(mCapabilities, &str);
    sb.Append(str);
    return sb.ToString(result);
}

ECode ParcelableConnection::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Int32 value = 0;
    source->ReadInt32(&value);
    if (value != 0) {
        AutoPtr<IPhoneAccountHandle> pa;
        CPhoneAccountHandle::New((IPhoneAccountHandle**)&pa);
        IParcelable* parcel = IParcelable::Probe(pa);
        parcel->ReadFromParcel(source);
        mPhoneAccount = pa;
    }
    else {
        mPhoneAccount = NULL;
    }

    source->ReadInt32(&mState);
    source->ReadInt32(&mCapabilities);

    source->ReadInt32(&value);
    if (value != 0) {
        //AutoPtr<IUri> address;
        //AutoPtr<IUriHelper> helper;
        //CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        //helper->GetEMPTY((IUri**)&address);
        //IParcelable* parcel = IParcelable::Probe(address);
        //parcel->ReadFromParcel(source);
        //mAddress= address;
        AutoPtr<IInterface> address;
        source->ReadInterfacePtr((Handle32*)&address);
        mAddress = IUri::Probe(address);
    }
    else {
        mAddress = NULL;
    }

    source->ReadInt32(&mAddressPresentation);
    source->ReadString(&mCallerDisplayName);
    source->ReadInt32(&mCallerDisplayNamePresentation);

    if (source->ReadInt32(&value), value != 0) {
        AutoPtr<IInterface> obj;
        source->ReadInterfacePtr((Handle32*)&obj);
        mVideoProvider = IIVideoProvider::Probe(obj);
    }
    else {
        mVideoProvider = NULL;
    }

    source->ReadInt32(&mVideoState);
    source->ReadByte((byte*)&mRingbackRequested);
    source->ReadByte((byte*)&mIsVoipAudioMode);

    source->ReadInt32(&value);
    if (value != 0) {
        AutoPtr<IStatusHints> sh;
        CStatusHints::New((IStatusHints**)&sh);
        IParcelable* parcel = IParcelable::Probe(sh);
        parcel->ReadFromParcel(source);
        mStatusHints = sh;
    }
    else {
        mStatusHints = NULL;
    }

    source->ReadInt32(&value);
    if (value != 0) {
        AutoPtr<IDisconnectCause> dc;
        CDisconnectCause::New((IDisconnectCause**)&dc);
        IParcelable* parcel = IParcelable::Probe(dc);
        parcel->ReadFromParcel(source);
        mDisconnectCause = dc;
    }
    else {
        mDisconnectCause = NULL;
    }

    source->ReadInt32(&value);
    if (value != 0) {
        AutoPtr<IInterface> obj;
        source->ReadInterfacePtr((Handle32*)&obj);
        mConferenceableConnectionIds = IList::Probe(obj);//TODO is it right??
    }
    else {
        mConferenceableConnectionIds = NULL;
    }
    return NOERROR;
}

ECode ParcelableConnection::WriteToParcel(
    /* [in] */ IParcel* destination)
{
    if (mPhoneAccount != NULL) {
        destination->WriteInt32(1);
        IParcelable::Probe(mPhoneAccount)->WriteToParcel(destination);
    }
    else {
        destination->WriteInt32(0);
    }
    destination->WriteInt32(mState);
    destination->WriteInt32(mCapabilities);
    if (mAddress != NULL) {
        destination->WriteInt32(1);
        //IParcelable::Probe(mAddress)->WriteToParcel(destination);
        destination->WriteInterfacePtr(mAddress);
    }
    else {
        destination->WriteInt32(0);
    }
    destination->WriteInt32(mAddressPresentation);
    destination->WriteString(mCallerDisplayName);
    destination->WriteInt32(mCallerDisplayNamePresentation);
    if (NULL != mVideoProvider) {
        destination->WriteInt32(1);
        destination->WriteInterfacePtr(mVideoProvider);
    }
    else {
        destination->WriteInt32(0);
    }
    destination->WriteInt32(mVideoState);
    destination->WriteByte((byte) (mRingbackRequested ? 1 : 0));
    destination->WriteByte((byte) (mIsVoipAudioMode ? 1 : 0));
    if (mStatusHints != NULL) {
        destination->WriteInt32(1);
        IParcelable::Probe(mStatusHints)->WriteToParcel(destination);
    }
    else {
        destination->WriteInt32(0);
    }
    if (mDisconnectCause != NULL) {
        destination->WriteInt32(1);
        IParcelable::Probe(mDisconnectCause)->WriteToParcel(destination);
    }
    else {
        destination->WriteInt32(0);
    }
    if (mConferenceableConnectionIds != NULL) {
        destination->WriteInt32(1);
        //IParcelable::Probe(mConferenceableConnectionIds)->WriteToParcel(destination);
        destination->WriteInterfacePtr(mConferenceableConnectionIds);//TODO is it right??
    }
    else {
        destination->WriteInt32(0);
    }
    return NOERROR;
}

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos
